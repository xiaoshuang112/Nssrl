/****************************************************************************************
********
********   �ļ���:   VLMain.c 
********   ��������: ����ʶ�����庯��ʵ��
********   �汾:     V1.0
********   ����:     flybird
********   ����:      2016-9-7 19:38
********  
*****************************************************************************************/
#include "stdafx.h"
#include <math.h>
#include <malloc.h>
#include "string.h"
#include "VLMain.h"

#define  WINSIZEX    64   //ͼ��X����ߴ�
#define  WINSIZEY    64    //ͼ��Y����ߴ�
#define  ORIENDIM    9     //����ά�� 
#define  ORIVALUE    20    //180/ORIENDIM
#define  BLOCKSIZE   4     //ÿ���麬��ϸ������
#define  BLOCKFeaNum 36    //ÿ�������������� ORIENDIM*BLOCKSIZE

#define  BLOCKXSIZE  16     //ÿ����X�������ص����
#define  BLOCKYSIZE  16     //ÿ����Y�������ص����

#define  MOVESTEPX  8      //X�����ƶ�����
#define  MOVESTEPY  8      //Y�����ƶ�����

#define  CELLXSIZE  8      //ÿ��ϸ��X�������ص����
#define  CELLYSIZE  8      //ÿ��ϸ��Y�������ص����


//////////////////////////////////�ڲ�����////////////////////////////////////////
//��ȡHOG����
//OSAL_INT32 VL_GetImageHogFeature(OSAL_INT32 *pFeature,OSAL_UCHAR *pImage,OSAL_UINT32 iW,OSAL_UINT32 iH,OSAL_INT32 *pnMeasure,OSAL_INT32 *pnTheta,OSAL_INT32 *pIntegHist);

OSAL_INT32 VL_ImageGamma(OSAL_PUCHAR pImage,OSAL_INT32 iW,OSAL_INT32 iH);

//�����ݶ�ֵ
OSAL_INT32 VL_CalImageGradients(OSAL_INT32 *pnMeasure, OSAL_INT32 *pnTheta,OSAL_PUCHAR pImage,OSAL_INT32 iW,OSAL_INT32 iH);

//�����ݶ�ֱ��ͼ
OSAL_INT32 VL_CalIntegHist(OSAL_INT32 *pIntegHist,OSAL_INT32 *pnMeasure, OSAL_INT32 *pnTheta);

//����HOG����	
OSAL_INT32 VL_CalHOG(OSAL_INT32 *pFeature,OSAL_INT32 *pIntegHist,OSAL_INT32 *pnMeasure);

SDKErrCode VL_TaskCreate(OSAL_HANDLE *pHandle, OSAL_INT32 iMaxW, OSAL_INT32 iMaxH)
{
	return SDK_ERR_NONE;
}


SDKErrCode VL_TaskFree(OSAL_HANDLE *pHandle)
{
	return SDK_ERR_NONE;
}


SDKErrCode VL_TaskProcess(OSAL_HANDLE handle, OSAL_UCHAR* pImage, OSAL_INT32 iW, OSAL_INT32 iH,ImageDataType eType,PTVLResult* ppVFResult)
{



	return SDK_ERR_NONE;
}



//////////////////////////////////�ڲ�����////////////////////////////////////////


//��ȡHOG����
OSAL_INT32 VL_GetImageHogFeature(OSAL_INT32 *pFeature,OSAL_UCHAR *pImage,OSAL_UINT32 iW,OSAL_UINT32 iH,OSAL_INT32 *pnMeasure,OSAL_INT32 *pnTheta,OSAL_INT32 *pIntegHist)
{
	//��ͼ�����Gamma����
	VL_ImageGamma(pImage,iW,iH);
	//ͼ��ƽ��

	//1�����ݶ�ֵ
	VL_CalImageGradients(pnMeasure,pnTheta,pImage,iW,iH);

	//2�������ֱ��ͼ
	VL_CalIntegHist(pIntegHist,pnMeasure,pnTheta);

	//3����HOG����	
	VL_CalHOG(pFeature,pIntegHist,pnMeasure);

	return 0;
}

OSAL_INT32 VL_ImageGamma(OSAL_PUCHAR pImage,OSAL_INT32 iW,OSAL_INT32 iH)
{
	OSAL_INT32 i,j;
	OSAL_PUCHAR lpSrc=OSAL_NULL, lpSrcTemp=OSAL_NULL;

	lpSrc = pImage;

	for (i=0;i<iH;i++)
	{
		lpSrcTemp = lpSrc;
		for (j=0;j<iW;j++)
		{
			*lpSrcTemp =(OSAL_UCHAR) sqrt((double)(*lpSrcTemp));
			lpSrcTemp++;
		}
		lpSrc += iW;
	}
	return 0;
}

#if 0
//�����ݶ�ֵ
OSAL_INT32 VL_CalImageGradients(OSAL_INT32 *pnMeasure, OSAL_INT32 *pnTheta,OSAL_PUCHAR pImage,OSAL_INT32 iW,OSAL_INT32 iH)
{
	OSAL_INT32 i,j,iSrcTmp1,iSrcTmp2,iSrcNextTmp1,iSrcNextTmp2,nPerLine,GX,GY;
	OSAL_PINT32 pDstMea = OSAL_NULL,pDstMeaTemp = OSAL_NULL,pDstThe = OSAL_NULL,pDstTheTemp = OSAL_NULL;
	OSAL_PUCHAR lpSrc1=OSAL_NULL, lpSrcTemp1=OSAL_NULL,lpSrc2=OSAL_NULL, lpSrcTemp2=OSAL_NULL;

	pDstMea = pnMeasure;
	pDstThe = pnTheta;
	//�߽����и�0
	memset(pDstMea,0,iW*sizeof(OSAL_INT32));
	memset(pDstThe,0,iW*sizeof(OSAL_INT32));
	for (i = 0; i < iH; i++)
	{
		*pDstMea = 0;
		*pDstThe = 0;
		pDstMea += iW;
		pDstThe += iW;
	}
	pDstMea = pnMeasure+iW-1;
	pDstThe = pnTheta+iW-1;
	for (i = 0; i < iH; i++)
	{
		*pDstMea = 0;
		*pDstThe = 0;
		pDstMea += iW;
		pDstThe += iW;
	}
	pDstMea = pnMeasure+iW*(iH-1);
	pDstThe = pnTheta+iW*(iH-1);
	memset(pDstMea,0,iW*sizeof(OSAL_INT32));
	memset(pDstThe,0,iW*sizeof(OSAL_INT32));

	//�����������ص��x,y�����ݶ�ֵ
	nPerLine = (iW<<1);
	pDstMea = pnMeasure+iW+1;
	pDstThe = pnTheta+iW+1;
	lpSrc1 = pImage+iW;
	lpSrc2 = pImage+1;
	for (i = 1; i < iH-1; i++)
	{
		pDstMeaTemp = pDstMea;
		pDstTheTemp = pDstThe;
		lpSrcTemp1 = lpSrc1;
		lpSrcTemp2 = lpSrc2;
		for (j = 1; j < iW-1; j++)
		{
			//X����ֵ
			iSrcTmp1 = *lpSrcTemp1;
			iSrcNextTmp1 = *(lpSrcTemp1+2);
			//Y����ֵ
			iSrcTmp2 = *lpSrcTemp2;
			iSrcNextTmp2 = *(lpSrcTemp2+nPerLine);

			//�����ݶ�
			GX = iSrcNextTmp1-iSrcTmp1;
			GY = iSrcNextTmp2-iSrcTmp2;
			//�����ݶȷ�ֵ�ͷ���
			(*pDstMeaTemp) = (OSAL_INT32)(sqrt((double)(GX)*(GX) + (GY)*(GY))+0.5);
			(*pDstTheTemp) = (OSAL_INT32)(atan2((double)(GY), (double)(GX))*57.3);

			//���Ƕ�ת����ֱ��ͼ����
			(*pDstTheTemp) = ((*pDstTheTemp)<0?((*pDstTheTemp)+180)/ORIVALUE:(*pDstTheTemp)/ORIVALUE);
			if((*pDstTheTemp) >= ORIENDIM || (*pDstTheTemp) < 0)
			{//Խ�紦��
				(*pDstTheTemp) = ORIENDIM-1;
			}
			pDstMeaTemp++;
			pDstTheTemp++;
			lpSrcTemp1++;
			lpSrcTemp2++;
		}
		pDstMea += iW;
		pDstThe += iW;
		lpSrc1 += iW;
		lpSrc2 += iW;
	}
	return 0;
}

#endif


#if 0
//�����ݶ�ֵ
OSAL_INT32 VL_CalImageGradients(OSAL_INT32 *pnMeasure, OSAL_INT32 *pnTheta,OSAL_PUCHAR pImage,OSAL_INT32 iW,OSAL_INT32 iH)
{
	OSAL_INT32 i,j,iSrcTmp1,iSrcTmp2,iSrcNextTmp1,iSrcNextTmp2,nPerLine,iTemp;
	OSAL_PINT32 pDst1 = OSAL_NULL,pDstTemp1 = OSAL_NULL,pDst2 = OSAL_NULL,pDstTemp2 = OSAL_NULL;
	OSAL_PINT32 pSrc1 = OSAL_NULL,pSrcTemp1 = OSAL_NULL,pSrc2 = OSAL_NULL,pSrcTemp2 = OSAL_NULL;
	OSAL_PUCHAR lpSrc1=OSAL_NULL, lpSrcTemp1=OSAL_NULL,lpSrc2=OSAL_NULL, lpSrcTemp2=OSAL_NULL;
	OSAL_DBL64 fTheta = 0.0,mag = 0.0,fTemp = 0.0,angle;
	int * xsub,*ysub;
	xsub=(int*)malloc(iW*iH*sizeof(int));
	ysub=(int*)malloc(iW*iH*sizeof(int));

	pDst1 = xsub+1;
	lpSrc1 = pImage;
	//����X�����ݶ�
	for (i=0;i<iH;i++)
	{
		pDstTemp1 = pDst1;
		lpSrcTemp1 = lpSrc1;
		for (j=1;j<(iW-1);j++)
		{
			*pDstTemp1 = (*(lpSrcTemp1+2))-(*lpSrcTemp1);

			pDstTemp1++;
			lpSrcTemp1++;
		}
		pDst1 += iW;
		lpSrc1 += iW;
	}
	pDst1 = xsub;
	for (i=0;i<iH;i++)
	{
		*pDst1 = *(pDst1+1);
		*(pDst1+iW-1) = *(pDst1+iW-2);
		pDst1 += iW;
	}

	pDst1 = ysub+iW;
	lpSrc1 = pImage;
	//����Y�����ݶ�
	for (i=1;i<iH-1;i++)
	{
		pDstTemp1 = pDst1;
		lpSrcTemp1 = lpSrc1;
		for (j=0;j<iW;j++)
		{
			*pDstTemp1 = (*(lpSrcTemp1+2*iW))-(*lpSrcTemp1);

			pDstTemp1++;
			lpSrcTemp1++;
		}
		pDst1 += iW;
		lpSrc1 += iW;
	}

	pDst1 = ysub;
	for (j=0;j<iW;j++)
	{
		*pDst1 = *(pDst1+iW);
		*(pDst1+iW*(iH-1)) = *(pDst1+iW*(iH-2));
		pDst1++;
	}

	pDst1 = pnTheta;
	pDst2 = pnMeasure;
	pSrc1 = xsub;  
	pSrc2 = ysub;

	//����Ƕ�/ģֵ
	for (i=0;i<iH;i++)
	{
		pDstTemp1 = pDst1;
		pDstTemp2 = pDst2;
		pSrcTemp1 = pSrc1;
		pSrcTemp2 = pSrc2;
		for (j=0;j<iW;j++)
		{
			//�����ݶȷ�ֵ�ͷ���
			fTheta = (OSAL_INT32)(atan2((double)(*pSrcTemp2), (double)(*pSrcTemp1))*57.3);	
			mag = (OSAL_INT32)(sqrt((double)(*pSrcTemp1)*(*pSrcTemp1) + (*pSrcTemp2)*(*pSrcTemp2))+0.5);

			fTheta = (fTheta<0?(fTheta+180):fTheta);
			fTheta = (fTheta>180?(fTheta-180):fTheta);

			fTemp = fTheta/ORIVALUE;
			iTemp = (fTemp+0.5);
			if (fTemp>=iTemp)
			{//�ڶ�����Ϊ�������
				(*pDstTemp1) = iTemp;
				angle = 1.0-(((*pDstTemp1)+0.5)-fTemp);
				pDstTemp1++;
				(*pDstTemp1) = ((iTemp<=0)?(ORIENDIM-1):iTemp-1);
				pDstTemp1++;
			}
			else 
			{//�ڶ�����Ϊ�Ҳ�����
				(*pDstTemp1) = (OSAL_INT32)fTemp;
				angle = 1.0-(fTemp-((*pDstTemp1)+0.5));
				pDstTemp1++;
				(*pDstTemp1) = ((iTemp>ORIENDIM-1)?0:iTemp+1);
				pDstTemp1++;
				
			}
			(*pDstTemp2) = mag*angle;
			pDstTemp2++;
			(*pDstTemp2) = mag*(1.f-angle);
			pDstTemp2++;
			//���Ƕ�ת����ֱ��ͼ����
			//pDstTemp1++;
			//pDstTemp2++;
			pSrcTemp1++;
			pSrcTemp2++;
		}
		
		pDst1 += iW*2;
		pDst2 += iW*2;
		pSrc1 += iW;
		pSrc2 += iW;
	}

	free(xsub);
	free(ysub);

	return 0;
}
#endif 

//�����ݶ�ֵ
OSAL_INT32 VL_CalImageGradients(OSAL_INT32 *pnMeasure, OSAL_INT32 *pnTheta,OSAL_PUCHAR pImage,OSAL_INT32 iW,OSAL_INT32 iH)
{
	OSAL_INT32 i,j,iSrcTmp1,iSrcTmp2,iSrcNextTmp1,iSrcNextTmp2,nPerLine,GX,GY,iTemp;
	OSAL_PINT32 pDst1 = OSAL_NULL,pDstTemp1 = OSAL_NULL,pDst2 = OSAL_NULL,pDstTemp2 = OSAL_NULL;
	OSAL_PINT32 pSrc1 = OSAL_NULL,pSrcTemp1 = OSAL_NULL,pSrc2 = OSAL_NULL,pSrcTemp2 = OSAL_NULL;
	OSAL_PUCHAR lpSrc1=OSAL_NULL, lpSrcTemp1=OSAL_NULL,lpSrc2=OSAL_NULL, lpSrcTemp2=OSAL_NULL;
	int * xsub,*ysub;
	xsub=(int*)malloc(iW*iH*sizeof(int));
	ysub=(int*)malloc(iW*iH*sizeof(int));

	pDst1 = xsub+1;
	lpSrc1 = pImage;
	//����X�����ݶ�
	for (i=0;i<iH;i++)
	{
		pDstTemp1 = pDst1;
		lpSrcTemp1 = lpSrc1;
		for (j=1;j<(iW-1);j++)
		{
			*pDstTemp1 = (*(lpSrcTemp1+2))-(*lpSrcTemp1);

			pDstTemp1++;
			lpSrcTemp1++;
		}
		pDst1 += iW;
		lpSrc1 += iW;
	}
	pDst1 = xsub;
	for (i=0;i<iH;i++)
	{
		*pDst1 = *(pDst1+1);
		*(pDst1+iW-1) = *(pDst1+iW-2);
		pDst1 += iW;
	}

	pDst1 = ysub+iW;
	lpSrc1 = pImage;
	//����Y�����ݶ�
	for (i=1;i<iH-1;i++)
	{
		pDstTemp1 = pDst1;
		lpSrcTemp1 = lpSrc1;
		for (j=0;j<iW;j++)
		{
			*pDstTemp1 = (*(lpSrcTemp1+2*iW))-(*lpSrcTemp1);

			pDstTemp1++;
			lpSrcTemp1++;
		}
		pDst1 += iW;
		lpSrc1 += iW;
	}

	pDst1 = ysub;
	for (j=0;j<iW;j++)
	{
		*pDst1 = *(pDst1+iW);
		*(pDst1+iW*(iH-1)) = *(pDst1+iW*(iH-2));
		pDst1++;
	}

	pDst1 = pnTheta;
	pDst2 = pnMeasure;
	pSrc1 = xsub;  
	pSrc2 = ysub;

	//����Ƕ�/ģֵ
	for (i=0;i<iH;i++)
	{
		pDstTemp1 = pDst1;
		pDstTemp2 = pDst2;
		pSrcTemp1 = pSrc1;
		pSrcTemp2 = pSrc2;
		for (j=0;j<iW;j++)
		{
			//�����ݶȷ�ֵ�ͷ���
			(*pDstTemp1) = (OSAL_INT32)(atan2((double)(*pSrcTemp2), (double)(*pSrcTemp1))*57.3);	
			(*pDstTemp2) = (OSAL_INT32)(sqrt((double)(*pSrcTemp1)*(*pSrcTemp1) + (*pSrcTemp2)*(*pSrcTemp2))+0.5);
			iTemp = (*pDstTemp1);
			//���Ƕ�ת����ֱ��ͼ����
			(*pDstTemp1) = ((*pDstTemp1)<0?((*pDstTemp1)+180)/ORIVALUE:(*pDstTemp1)/ORIVALUE);
			//if((*pDstTemp1) >= ORIENDIM || (*pDstTemp1) < 0)
			//{//Խ�紦��
			//	(*pDstTemp1) = ORIENDIM-1;
			//}
			(*pDstTemp1) =((*pDstTemp1) >= ORIENDIM?(*pDstTemp1-1):(*pDstTemp1));
			pDstTemp1++;
			pDstTemp2++;
			pSrcTemp1++;
			pSrcTemp2++;
		}

		pDst1 += iW;
		pDst2 += iW;
		pSrc1 += iW;
		pSrc2 += iW;
	}

	free(xsub);
	free(ysub);

	return 0;
}

//�����ݶ�ֱ��ͼ
OSAL_INT32 VL_CalIntegHist(OSAL_INT32 *pIntegHist,OSAL_INT32 *pnMeasure, OSAL_INT32 *pnTheta)
{
	OSAL_INT32 i,j,k,l,iCellNumX,iCellNumY;
	OSAL_PINT32 pDstHist = OSAL_NULL,pHistTemp = OSAL_NULL,pSrcMea = NULL,pMeaTemp = OSAL_NULL,pMeaTemp2 = OSAL_NULL,pSrcTheta = OSAL_NULL,pThetaTemp = OSAL_NULL,pThetaTemp2 = OSAL_NULL;

	//(����ͼ��ߴ�Ϊ64*32 ϸ����СΪ8*8 ����ά��Ϊ9 ֱ��ͼ����=(64*32/(8*8))*9)
	iCellNumX = WINSIZEX/CELLXSIZE;
	iCellNumY = WINSIZEY/CELLYSIZE;
	memset(pIntegHist,0,iCellNumX*iCellNumY*ORIENDIM*sizeof(OSAL_INT32));
	pDstHist = pIntegHist;
	pSrcMea = pnMeasure;
	pSrcTheta = pnTheta;

	for (j = 0; j < iCellNumY; j++)
	{
		pMeaTemp = pSrcMea;
		pThetaTemp = pSrcTheta;	

		for (i= 0; i < iCellNumX; i++)
		{
			pMeaTemp2 = pMeaTemp;
			pThetaTemp2 = pThetaTemp;
			//����ÿ��ϸ�����ݶ�ֱ��ͼ
			for (l = 0;l < CELLYSIZE;l++)
			{
				for (k = 0; k < CELLXSIZE;k++)
				{
					pDstHist[*(pThetaTemp2+k)] += (*(pMeaTemp2+k));
					//pDstHist[*(pThetaTemp2+2*k)] += (*(pMeaTemp2+2*k));
					//pDstHist[*(pThetaTemp2+2*k+1)] += (*(pMeaTemp2+2*k+1));
				}
				pMeaTemp2 += WINSIZEX;
				pThetaTemp2 += WINSIZEX;
			}
			
			pMeaTemp += CELLXSIZE;
			pThetaTemp += CELLXSIZE;

			pDstHist+=ORIENDIM;

		}
		pSrcMea += CELLYSIZE*WINSIZEX;
		pSrcTheta += CELLYSIZE*WINSIZEX;
	}
	return 0;
}

#if 1
//����HOG����	
OSAL_INT32 VL_CalHOG(OSAL_INT32 *pFeature,OSAL_INT32 *pIntegHist,OSAL_INT32 *pnMeasure)
{
	OSAL_INT32 i,j,l,k,iBlockNumX,iBlockNumY,nPerline,nHistStep[3],iCellNumX,iCellNumY,iCellRowNum,iCellColNum,CopySize;
	OSAL_DBL64 Sum;
	OSAL_PINT32 pDstFea = OSAL_NULL,pDstFeaTemp = OSAL_NULL,pSrcHist = OSAL_NULL,pSrcHistTemp = OSAL_NULL,pSrcHistTemp2 = OSAL_NULL;

	iCellNumX = WINSIZEX/CELLXSIZE;
	iCellNumY = WINSIZEY/CELLYSIZE;

	iCellRowNum = BLOCKYSIZE/CELLYSIZE;
	iCellColNum = BLOCKXSIZE/CELLXSIZE;
	CopySize = sizeof(int)*ORIENDIM*iCellColNum;

	iBlockNumX = WINSIZEX/MOVESTEPX-1;
	iBlockNumY = WINSIZEY/MOVESTEPY-1;
	
	nPerline = ORIENDIM*BLOCKXSIZE/CELLXSIZE;
	nHistStep[0] = (MOVESTEPY/CELLYSIZE)*(WINSIZEX/CELLXSIZE)*ORIENDIM;
	nHistStep[1] = MOVESTEPX/CELLXSIZE*ORIENDIM;
	nHistStep[2] =(WINSIZEX/CELLXSIZE)*ORIENDIM;

	pDstFea= pFeature;
	pSrcHist = pIntegHist;
	for(j = 0; j < iBlockNumY; j++)
	{
		pSrcHistTemp = pSrcHist;
		for (i = 0; i < iBlockNumX; i++)
		{
			pDstFeaTemp = pDstFea;
			pSrcHistTemp2 = pSrcHistTemp;
			for (l=0;l<iCellRowNum;l++)
			{	
				memcpy(pDstFea,pSrcHistTemp2,CopySize);
				pDstFea += nPerline;
				pSrcHistTemp2 += nHistStep[2];
			}	
			Sum = 0.0;
			//���й�һ������
			for (k=0;k<iCellRowNum*iCellColNum*ORIENDIM;k++)
			{
				Sum += pDstFeaTemp[k]*pDstFeaTemp[k];
			}
			Sum++; //��ֹ��ĸΪ0
			Sum = sqrt((double)Sum);
			for (k=0;k<iCellRowNum*iCellColNum;k++)
			{
				 pDstFeaTemp[k] = (pDstFeaTemp[k])*1000000/Sum;
			}

			pSrcHistTemp += nHistStep[1];
		}
		pSrcHist += nHistStep[0];
	}

	return 0;
}

#endif 

#if 0
#define  OSAL_SUCCESS 0
#define  OSAL_FAILED  1
#define BLOCK_HOG_FEA_LEN  ((BLOCKSIZE)*(ORIENDIM))

/************************************************************************/
/* �Ҷ�ͼ��ߴ�任                                                     */
/************************************************************************/
/*
*������		  : vrsGrayResize
*��������	  : ���Ҷ�ͼ��ߴ�任������˫���η���
*�������˵�� : pu8Src---�Ҷ�ͼ������
			    n32Width---ԭʼͼ����
			    n32Height---ԭʼͼ��߶�
				newWidth---Ŀ��ͼ����
			    newHeight---Ŀ��ͼ��߶�
*�������˵�� : Dst---�ߴ�任��ͼ��
*����ֵ˵��	  : ִ�гɹ�����OSAL_TRUE, ���򷵻�OSAL_FALSE
*��������	  : 2012-7-2
*/
OSAL_BOOL vtrsGrayResize(OSAL_UCHAR *Src,const OSAL_INT32 n32Width,const OSAL_INT32 n32Height,
					 const OSAL_INT32 newWidth,const OSAL_INT32 newHeight,OSAL_UCHAR *Dst)
{
	OSAL_INT32 n32PixelNum=n32Width*n32Height;
	OSAL_INT32 new32PixelNum=newWidth*newHeight;
	OSAL_INT32 x,y,yDownLoc,xLeftLoc;

	OSAL_INT32 yDis=(n32Height*100-110)/(newHeight-1);
	OSAL_INT32 xDis=(n32Width*100-110)/(newWidth-1);

	OSAL_INT32 yDownDis,yUpDis,xLeftDis,xRightDis;
	OSAL_INT32 yLoc,xLoc,left,right;

	for(y=0;y<newHeight;y++)
	{
		for(x=0;x<newWidth;x++)
		{
			xLoc=x*xDis;
			yLoc=y*yDis;
			yDownLoc=yLoc/100;
			xLeftLoc=xLoc/100;
			yDownDis=yLoc-yDownLoc*100;
			yUpDis=100-yDownDis;
			xLeftDis=xLoc-xLeftLoc*100;
			xRightDis=100-xLeftDis;

			//˫���Բ�ֵ
			left = Src[(yDownLoc+1)*n32Width+xLeftLoc]*yDownDis+
				Src[(yDownLoc)*n32Width+xLeftLoc]*yUpDis;
			right = Src[(yDownLoc+1)*n32Width+xLeftLoc+1]*yDownDis+
				Src[(yDownLoc)*n32Width+xLeftLoc+1]*yUpDis;

			left /= 100;
			right /= 100;

			Dst[y*newWidth+x]=(OSAL_UCHAR)((left*xRightDis+right*xLeftDis)/100);
		}
	}

	return OSAL_SUCCESS;
}



/*
*������		  : vrsMediafilt
*��������	  : ͼ����ֵ�˲�����
*�������˵�� : pu8Src---�Ҷ�ͼ������
				n32Width---ͼ����
				n32Height---ͼ��߶�
*����ֵ˵��	  : ִ�гɹ�����OSAL_TRUE, ���򷵻�OSAL_FALSE
*��������	  : 2012-7-2
*/
OSAL_BOOL vtrsMediafilt(OSAL_UCHAR *pu8Src, const OSAL_INT32 n32Width, const OSAL_INT32 n32Height)
{
	OSAL_INT32 i, j, k, q, index;
	OSAL_UCHAR list[5];

	if (OSAL_NULL == pu8Src)
	{
		return -1;
	}

	//��ֵ�˲�
	for(i=0;i<n32Height;i++)
	{
		for(j=0;j<n32Width;j++)
		{
			for(k=0;k<5;k++)
				list[k]=0;
			if(i>=1)
				list[0]=(OSAL_UCHAR)pu8Src[(i-1)*n32Width+j];
			if(j>=1)
				list[1]=(OSAL_UCHAR)pu8Src[(i)*n32Width+j-1];
			if(i<=n32Height-2)
				list[4]=(OSAL_UCHAR)pu8Src[(i+1)*n32Width+j];
			if(j<=n32Width-2)
				list[3]=(OSAL_UCHAR)pu8Src[(i)*n32Width+j+1];
			list[2]=(OSAL_UCHAR)pu8Src[i*n32Width+j];

			//�Ƚϻ�ȡ�м�ֵ
			for(k=0;k<3;k++)
			{
				index=0;
				for(q=1;q<5;q++)
					if(list[q]>list[index])
						index=q;
				if(k<2)
					list[index]=0;
			}
			pu8Src[i*n32Width+j]=list[index];
		}
	}

	return OSAL_SUCCESS;
}

/*
*������		  : vrsOneDimGuassfilter
*��������	  : ��˹�˲�
*�������˵�� : pu8Src---�Ҷ�ͼ��
				n32Width---ͼ����
				n32Height---ͼ��߶�
				d64Sigma---�˲��뾶
*�������˵�� :	pu8Dst---���ͼ��
*����ֵ˵��	  : ִ�гɹ�����OSAL_TRUE, ���򷵻�OSAL_FALSE
*��������	  : 2013-03-14
*/
OSAL_BOOL vtrsOneDimGuassfilter(OSAL_INT32 *pTmp1, OSAL_INT32 *pTmp2, const OSAL_UCHAR *pu8Src, const OSAL_INT32 n32Width, const OSAL_INT32 n32Height, OSAL_UCHAR *pu8Dst)
{
	OSAL_INT32 i, x, y, m;

	//һά��˹�ֲ�����������ƽ�����������ɵĸ�˹�˲�ϵ��
	//�����˲����ڵĴ�С 
	OSAL_INT32 n32WindowSize = 5;   
	//�����˲��������ĵ�����
	OSAL_INT32 n32Center = (n32WindowSize)/2;

	OSAL_INT32 sum;

	OSAL_INT32 pd64Kernel[5] = {3, 40387, 919217, 40387, 3};
	OSAL_INT32 *temp1, *temp2;

	if ((OSAL_NULL == pu8Src) || (0 >= n32Width) || (0 >= n32Height) || (OSAL_NULL == pu8Dst))
	{
		return OSAL_FALSE;
	}

	temp1 = pTmp1;
	SETVALUE(temp1, 0, OSAL_INT32, n32Width*n32Height);
	temp2 = pTmp2;
	SETVALUE(temp2, 0, OSAL_INT32, n32Width*n32Height);

	//ˮƽ�����˲�
	for( y=0;y<n32Height;y++)
	{
		for( x=0;x<n32Width;x++)
		{
			sum=0;
			for( m=-n32Center;m<=n32Center;m++)
			{
				if(m+x>=0 && m+x<n32Width)
				{
					sum=sum+pd64Kernel[m+n32Center]*pu8Src[y*n32Width+m+x];
				}
			}
			temp1[y*n32Width+x]=sum*1e-6;
		}
	}

	//��ֱ�����˲�
	for( x=0;x<n32Width;x++)
	{
		for( y=0;y<n32Height;y++)
		{
			sum=0;
			for( m=-n32Center;m<=n32Center;m++)
			{
				if(m+y>=0 && m+y<n32Height)
				{
					sum=sum+pd64Kernel[m+n32Center]*temp1[(y+m)*n32Width+x];
				}
			}
			temp2[y*n32Width+x]=sum*1e-6;
		}
	}

	for(i=0;i<n32Width*n32Height;i++)
	{
		pu8Dst[i]=(OSAL_UCHAR)temp2[i];
	}

	return OSAL_TRUE;
}

/************************************************************************/
/*HOG����                                                               */
/************************************************************************/
/*
*������		  : HistExpand
*��������	  : ��ͼ������ȷ�Χ��չ��0-255��������չ
*�������˵�� : pu8Src---�Ҷ�ͼ������
			    n32Width---ͼ����
			    n32Height---ͼ��߶�
*����ֵ˵��	  : ִ�гɹ�����OSAL_TRUE, ���򷵻�OSAL_FALSE
*��������	  : 2012-7-2
*/
OSAL_INT32 vtrsCalHistExpand(OSAL_UCHAR *pu8Src, const OSAL_INT32 n32Width, const OSAL_INT32 n32Height, OSAL_UCHAR *pu8Dst)
{
	OSAL_INT32 i = 0;
	OSAL_UCHAR *tempCur = OSAL_NULL;
	OSAL_UCHAR max = 0, min = 255, dis = 0;
	OSAL_UCHAR  tempValue = 0,*tempDst = OSAL_NULL;
	
	OSAL_INT32 n32PixelNum = n32Width*n32Height;
	OSAL_UCHAR *pu8SrcCur = pu8Src;

	if ((OSAL_NULL == pu8Src) || (OSAL_NULL == pu8Dst))
	{
		return -1;
	}

	//��ȡ�Ҷ�ͼ��Ҷ�ֵ���Χ
	for(i = 0; i < n32PixelNum; i++)
	{
		tempValue = (*pu8SrcCur);
		if(max < (tempValue))
		{
			max = (tempValue);
		}
		if(min > (tempValue))
		{
			min = (tempValue);
		}
		pu8SrcCur++;
	}
	dis = max - min;

	//�ԻҶ�ͼ������������
	tempCur = pu8Src;
	tempDst = pu8Dst;
	for(i = 0; i< n32PixelNum; i++)
	{
		*tempDst = (((*tempCur) - min)*255/(dis+1));
		tempCur++;
		tempDst++;
	}

	return OSAL_SUCCESS;
}


OSAL_INT32 vtrsCalcGradients(OSAL_PUCHAR pImage, const OSAL_INT32 n32Width, const OSAL_INT32 n32Height, OSAL_INT32 *pnMeasure, OSAL_INT32 *pnTheta)
{
	OSAL_INT32 i = 0, j = 0;

	OSAL_INT32 d64SrcTmp1 = 0;
	OSAL_INT32 d64SrcTmp2 = 0;
	OSAL_INT32 d64SrcNextTmp1 = 0;
	OSAL_INT32 d64SrcNextTmp2 = 0;

	//��ʼ��ָ��
	OSAL_UCHAR *pu8SrcCur, *pu8SrcTemp, *pu8SrcNext, *pu8SrcNextTemp;
	OSAL_INT32 pd64GradientX, pd64GradientY; 
	OSAL_INT32 *pn32MeasureCur, *pn32ThetaCur;

	if ((OSAL_NULL == pImage) || (0 >= n32Width) || (0 >= n32Height))
	{
		return -1;
	}

	pu8SrcCur = pImage;
	pu8SrcNext = pu8SrcCur + n32Width;

	pn32MeasureCur = pnMeasure;
	pn32ThetaCur = pnTheta;

	//����x,y�����ƫ����
	for(i=0; i<n32Height; i++)   
	{   
		pu8SrcTemp = pu8SrcCur;
		pu8SrcNextTemp = pu8SrcNext;


		for(j=0; j<n32Width; j++)   
		{
			if ((j > n32Width-2) || (i > n32Height-2))
			{
				(pd64GradientX) = 0;
				(pd64GradientY) = 0;
			}
			else
			{
				d64SrcTmp1 = (*(pu8SrcTemp++));
				d64SrcTmp2 = (*pu8SrcTemp);
				d64SrcNextTmp1 = (*(pu8SrcNextTemp++));
				d64SrcNextTmp2 = (*pu8SrcNextTemp);

				//x�����ݶ�
				(pd64GradientX) = (d64SrcTmp2 - d64SrcTmp1 + d64SrcNextTmp2 - d64SrcNextTmp1)/2;

				//y�����ݶ�
				(pd64GradientY) = -(d64SrcNextTmp1 - d64SrcTmp1 + d64SrcNextTmp2 - d64SrcTmp2)/2;
			}

			//�����ݶȷ�ֵ���ݶȵķ���
			(*pn32MeasureCur) = (sqrt((pd64GradientX)*(pd64GradientX) + (pd64GradientY)*(pd64GradientY))+0.5);
			(*pn32ThetaCur) = (atan2((pd64GradientY), (pd64GradientX))*57.3);
			// 			(*pn32MeasureCur) = (OSAL_INT32)(vtrsSqrt((pd64GradientX)*(pd64GradientX) + (pd64GradientY)*(pd64GradientY))+0.5);
			// 			(*pn32ThetaCur) = vrsAtan((pd64GradientY), (pd64GradientX));

			//���ýǶ�ת����0  ~  360  ��֮��
			if((*pn32ThetaCur) < 0)
			{
				(*pn32ThetaCur) += 180;   
			}
			(*pn32ThetaCur) /= 20;
			if((*pn32ThetaCur) >= 9 || (*pn32ThetaCur) < 0)
			{
				(*pn32ThetaCur) = 8;
			}

			pn32ThetaCur++;
			pn32MeasureCur++;

		}

		pu8SrcCur += n32Width;
		pu8SrcNext += n32Width;
	}

	return OSAL_FALSE;
}


/*
*������		  : GetIntegHists
*��������	  : �������ֱ��ͼ
*�������˵�� : handle ---������
				n32Width---ͼ��Ŀ��
				n32Height---ͼ��ĸ߶�
*�������˵�� : ��
*����ֵ˵��	  : ִ�гɹ�����OSAL_TRUE��ִ��ʧ�ܷ���OSAL_FALSE
*��������	  : 2013-03-14
*/

OSAL_INT32 vtrsCalcIntegHist(OSAL_INT32 *pnMeasure, OSAL_INT32 *pnTheta, OSAL_INT32 *TempHist, OSAL_INT32 *pIntegHist)
{
	OSAL_INT32 i = 0, j = 0, BinNum = 0;
	OSAL_INT32 *preLine = OSAL_NULL;
	OSAL_INT32 *pcurline;
	OSAL_INT32 *pCurrPixPos;
	OSAL_INT32 *m_pn32Measure;
	OSAL_INT32 *m_pn32Theta;
	OSAL_INT32 *pTempHist = TempHist;

	if ((OSAL_NULL == pnMeasure) || (OSAL_NULL == pnTheta) || (OSAL_NULL == pIntegHist))
	{
		return -1;
	}

	SETVALUE(pIntegHist,0,OSAL_INT32,(SIZE+1)*(SIZE+1)*ORIENDIM);	

	pcurline =pIntegHist + (SIZE+2)*ORIENDIM;
	pCurrPixPos = pcurline;
	m_pn32Measure = pnMeasure;
	m_pn32Theta = pnTheta;

	//�������ͼ
	for (i = 0; i < SIZE; i++)
	{
		if (i != 0)
		{
			COPYVALUE(pcurline, preLine, OSAL_PINT32, SIZE*ORIENDIM);
		}

		pTempHist = TempHist;
		SETVALUE(pTempHist, 0, OSAL_INT32, ORIENDIM);

		for (j = 0; j < SIZE; j++)
		{
			TempHist[*(m_pn32Theta++)] += (*(m_pn32Measure++)) ;
			pTempHist = TempHist;
			for(BinNum = 0; BinNum < ORIENDIM; ++BinNum)
				(*(pCurrPixPos++)) += (*(pTempHist++)) ;
		}
		preLine = pcurline;
		pcurline += (SIZE+1)*ORIENDIM;
		pCurrPixPos += ORIENDIM;
	}

	return OSAL_SUCCESS; 
}

/*
*������		  : vrsCalcHOG
*��������	  : ͳ��ͼ���hog������
*�������˵�� : handle---������
			    n32Width---ͼ����
			    n32Height---ͼ��߶�
*�������˵�� : Dst---�õ�������ֵָ��
*����ֵ˵��	  : ִ�гɹ�����OSAL_TRUE, ���򷵻�OSAL_FALSE
*��������	  : 2013-03-14
*/
OSAL_BOOL vtrsCalcHOG(OSAL_INT32 *pHist, const OSAL_INT32 n32Width,const OSAL_INT32 n32Height, OSAL_INT32 *tempHist, OSAL_INT32 *pFeature)
{
	//����HOG����
	OSAL_INT32 i = 0, j = 0;
	OSAL_INT32 stepX = SIZE/CELLXSIZE;
	OSAL_INT32 stepY = SIZE/CELLYSIZE;
	OSAL_INT32 *pIntegHist = pHist;
	OSAL_INT32 *pIntegHist1 = OSAL_NULL;
	OSAL_INT32 *pIntegHist2 = OSAL_NULL;
	OSAL_INT32 *pIntegHist3 = OSAL_NULL;
	OSAL_INT32 *pIntegHist4 = OSAL_NULL;
	OSAL_INT32 BinNum = 0;
	OSAL_INT32 *pDst = pFeature;
	OSAL_INT32 *pTempHist, *description = tempHist + CELLYSIZE*CELLXSIZE*ORIENDIM;
	OSAL_INT32 *pTempHist1, *pTempHist2;

	if ((OSAL_NULL == pIntegHist) || (OSAL_NULL == pFeature))
	{
		return -1;
	}

	//���ݻ���ͼ����ÿһ�����Ӧ����
	pTempHist = tempHist;
	for (i = 0; i < CELLYSIZE; i ++)
	{
		for (j = 0; j < CELLXSIZE; j ++)
		{
			pIntegHist4 = pIntegHist + ((i+1) *stepY* (SIZE+1) + (j+1)*stepX) * ORIENDIM;
			pIntegHist3 = pIntegHist + ((i+1) *stepY* (SIZE+1) + j*stepX) * ORIENDIM;
			pIntegHist2 = pIntegHist + ( i*stepY* (SIZE+1) + (j+1)*stepX) * ORIENDIM;
			pIntegHist1 = pIntegHist + (i*stepY* (SIZE+1) + j*stepX) * ORIENDIM;

			for (BinNum = 0; BinNum < ORIENDIM; BinNum++)
			{
				*pTempHist++ = (*pIntegHist4++) - (*pIntegHist3++) - 
					         (*pIntegHist2++) + (*pIntegHist1++);
			}
		}
	}
	
	pTempHist1 = tempHist;
	pTempHist2 = tempHist+CELLXSIZE*ORIENDIM;

	//����ÿһά������ֵ
	for (j = 1; j <= Stepx*Stepy; ++j)
	{

		OSAL_INT32 *pDescription = description;
		OSAL_INT32 sum = 0 ;
		OSAL_INT32 HistBin = 0 ;

		for(HistBin=0;HistBin<2*ORIENDIM;++HistBin)
		{
			*pDescription++ =  *pTempHist1;
		//	sum += *pTempHist1++;
			pTempHist1++;
		}

		for(HistBin = 0; HistBin < 2*ORIENDIM; ++HistBin)
		{
			*pDescription++ =  *pTempHist2;
		//	sum += *pTempHist2++;
			pTempHist2++;
		}

		if(j%3 !=0)
		{
			pTempHist1 -= (1*ORIENDIM) ;
			pTempHist2 -= (1*ORIENDIM) ;
		}

		pDescription = description;
		sum = 48*255;
		for(i=0;i<BLOCK_HOG_FEA_LEN;i++)
		{
			(*(pDst++)) = ((*(pDescription++))*100)/sum;
		}
	
	}

	return OSAL_TRUE;
}


/*
*������		  : vrsCalcHOGFeature
*��������	  : ��������HOG����
*�������˵�� : handle ---������
				n32Width---ͼ��Ŀ��
				n32Height---ͼ��ĸ߶�
*�������˵�� : Dst---����ֵ����ָ��
*����ֵ˵��	  : ִ�гɹ�����OSAL_TRUE��ִ��ʧ�ܷ���OSAL_FALSE
*��������	  : 2013-03-14
*/
OSAL_BOOL vtrsCalcHOGFeature(OSAL_PUCHAR pLogoSrc, OSAL_INT32 nWidth, OSAL_INT32 nHeight, OSAL_PUCHAR pTmpSrc1, OSAL_PUCHAR pTmpSrc2, 
						   OSAL_INT32 nRW, OSAL_INT32 nRH, OSAL_INT32 *pnMeasure, OSAL_INT32 *pnTheta, OSAL_INT32 *pIntegHist, OSAL_INT32  *pFeature)
{
	//ͼ���������
	vtrsGrayResize(pLogoSrc, nWidth, nHeight, nRW, nRH, pTmpSrc1);

	//��ֵ�˲�
	vtrsMediafilt(pTmpSrc1, nRW, nRH);

	//��˹�˲�
	vtrsOneDimGuassfilter(pnMeasure, pnTheta, pTmpSrc1, nRW, nRH, pTmpSrc2);

	//�Ҷ�����
	vtrsCalHistExpand(pTmpSrc2, nRW, nRH, pTmpSrc1);

	//�����ݶ�ֵ
	vtrsCalcGradients(pTmpSrc1, nRW, nRH, pnMeasure, pnTheta);

	//�������ֱ��ͼ
	vtrsCalcIntegHist(pnMeasure, pnTheta, pFeature, pIntegHist);

	//����HOG����
	vtrsCalcHOG(pIntegHist, nRW, nRH, pnMeasure, pFeature);

	return OSAL_SUCCESS;
}

#endif 

















