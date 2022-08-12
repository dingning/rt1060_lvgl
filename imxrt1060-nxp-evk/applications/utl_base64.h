#ifndef _UTL_BASE64_H_
#define _UTL_BASE64_H_


#ifdef __cplusplus
extern "C" {
#endif

//base64解码
//str			:要解码的字符串
//len			:字符串长度。0表示让本接口自己计算
//out_lenResult	:[out]输出长度
//返回值		:以'\0'结束的字符串。使用完后调用free来释放。
//lint -sem(utl_base64_decode, @p == malloc(10) || @p == 0)
char *utl_base64_decode(const char *str, int len, int *out_lenResult);

//base64编码
//str		:要编码的字符串
//len		:字符串长度。0表示让本接口自己计算
//返回值	:以'\0'结束的字符串。使用完后调用free来释放。
//lint -sem(utl_base64_encode, @p == malloc(10) || @p == 0)
char *utl_base64_encode(const char *str, int len);

#ifdef __cplusplus
}
#endif

#endif
