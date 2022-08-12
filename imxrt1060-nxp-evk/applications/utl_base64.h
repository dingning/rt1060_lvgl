#ifndef _UTL_BASE64_H_
#define _UTL_BASE64_H_


#ifdef __cplusplus
extern "C" {
#endif

//base64����
//str			:Ҫ������ַ���
//len			:�ַ������ȡ�0��ʾ�ñ��ӿ��Լ�����
//out_lenResult	:[out]�������
//����ֵ		:��'\0'�������ַ�����ʹ��������free���ͷš�
//lint -sem(utl_base64_decode, @p == malloc(10) || @p == 0)
char *utl_base64_decode(const char *str, int len, int *out_lenResult);

//base64����
//str		:Ҫ������ַ���
//len		:�ַ������ȡ�0��ʾ�ñ��ӿ��Լ�����
//����ֵ	:��'\0'�������ַ�����ʹ��������free���ͷš�
//lint -sem(utl_base64_encode, @p == malloc(10) || @p == 0)
char *utl_base64_encode(const char *str, int len);

#ifdef __cplusplus
}
#endif

#endif
