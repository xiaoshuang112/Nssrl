#ifndef  SDK_COMMON_H
#define  SDK_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*��������ͼ���������ͣ�֧��BGR��YUV*/
typedef enum eImageDataType
{
	IMAGE_TYPE_BGR                         =  0,  /*BGR*/
	IMAGE_TYPE_YUV                         =  1,  /*YUV*/
	IMAGE_TYPE_OTHER                       = -1   /*δ֪��ʽ*/
} ImageDataType;

/*����Ĵ�������������߿��Ը��ݷ�����֪������ִ�д���ԭ��*/
typedef enum
{
	SDK_ERR_NONE                      =  0,
	SDK_ERR_NOINIT                    = -1,  /*δ��ʼ��*/
	SDK_ERR_PARAM                     = -2,  /*�����������*/
	SDK_ERR_FULLTASK                  = -3,  /*��������*/
	SDK_ERR_ALLOC                     = -4,  /*�ڴ����ʧ��*/
	SDK_ERR_NOBUFF                    = -5,  /*�ڲ�ͼ�񻺴治��*/
	SDK_ERR_LICENSE                   = -6,  /*license����*/
	SDK_ERR_OTHER                     = -999
} SDKErrCode;



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SDK_COMMON_H */