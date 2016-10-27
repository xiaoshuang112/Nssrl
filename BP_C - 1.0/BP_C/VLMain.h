/****************************************************************************************
********
********   �ļ���:   VLMain.h 
********   ��������: ����ʶ���ڲ��ṹ�嶨��
********   �汾:     V1.0
********   ����:     flybird
********   ����:     2016-9-7 19:38
********  
*****************************************************************************************/

#ifndef   VLMAIN_H
#define   VLMAIN_H

#include "OSAL_type.h"
#include "SDKCommon.h"

	/*����ʶ�����ṹ��*/
typedef struct TagVLResult
{
	OSAL_INT32	 iLogoType;					/*  ��������  */
	OSAL_INT32   iXStart;                   /*  ������ͼƬ��X�������ʼλ��  */
	OSAL_INT32   iXEnd;                     /*  ������ͼƬ��X�������ֹλ��  */
	OSAL_INT32   iYStart;                   /*  ������ͼƬ��Y�������ʼλ��  */
	OSAL_INT32   iYEnd;                     /*  ������ͼƬ��Y�������ֹλ��  */
}TVLResult,*PTVLResult;

SDKErrCode VL_TaskCreate(OSAL_HANDLE *pHandle, OSAL_INT32 iMaxW, OSAL_INT32 iMaxH);

SDKErrCode VL_TaskFree(OSAL_HANDLE *pHandle);

SDKErrCode VL_TaskProcess(OSAL_HANDLE handle, OSAL_UCHAR* pImage, OSAL_INT32 iW, OSAL_INT32 iH,ImageDataType eType,PTVLResult* ppVFResult);

//��ȡHOG����
OSAL_INT32 VL_GetImageHogFeature(OSAL_INT32 *pFeature,OSAL_UCHAR *pImage,OSAL_UINT32 iW,OSAL_UINT32 iH,OSAL_INT32 *pnMeasure,OSAL_INT32 *pnTheta,OSAL_INT32 *pIntegHist);

#endif /* VLMAIN_H */
