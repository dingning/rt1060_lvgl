#include <stdlib.h>
#include <string.h>
#include "utl_base64.h"

static char base64DecodeTable[256];

/*static int Base64_GetEncodeNewLen(const char * src)
{
	int len = strlen((char*)src);
	return (len +(len%3 == 0? 0:(3-len%3)))/3*4 + 1;
}
*/

static int Base64_GetDecodeNewLen(const char* src)
{
	int len = strlen(src);
	return len/4*3+1;
}

static void initBase64DecodeTable()
{
	int i;
	//for (i = 0; i < 256; ++i) base64DecodeTable[i] = (char)0x80;
	memset(base64DecodeTable, 0x80, sizeof(base64DecodeTable));
	// default value: invalid

	for (i = 'A'; i <= 'Z'; ++i) base64DecodeTable[i] = 0 + (i - 'A');	//lint !e835
	for (i = 'a'; i <= 'z'; ++i) base64DecodeTable[i] = 26 + (i - 'a');
	for (i = '0'; i <= '9'; ++i) base64DecodeTable[i] = 52 + (i - '0');
	base64DecodeTable[(unsigned char)'+'] = 62;
	base64DecodeTable[(unsigned char)'/'] = 63;
	base64DecodeTable[(unsigned char)'='] = 0;
}


static char* _base64Decode(char const* in, unsigned int inSize,
                                    unsigned int *resultSize,
                                    int bTrimTrailingZeros)
{
	char* out;
	static int haveInitializedBase64DecodeTable = 0;
	if (!haveInitializedBase64DecodeTable)
	{
		initBase64DecodeTable();
		haveInitializedBase64DecodeTable = 1;
	}

	out = (char*)malloc(Base64_GetDecodeNewLen(in)*2); // ensures we have enough space
	if(out)
	{
		int k = 0;
		int paddingCount = 0;
		int const jMax = inSize - 3;
		// in case "inSize" is not a multiple of 4 (although it should be)
		int i,j;
		for (j = 0; j < jMax; j += 4)
		{
			unsigned char inTmp[4], outTmp[4];
			for (i = 0; i < 4; ++i)
			{
				inTmp[i] = in[i+j];
				if (inTmp[i] == '=') ++paddingCount;
				outTmp[i] = base64DecodeTable[(unsigned char)inTmp[i]];
				if ((outTmp[i]&0x80) != 0) outTmp[i] = 0; // this happens only if there was an invalid character; pretend that it was 'A'
			}

			out[k++] = (outTmp[0]<<2) | (outTmp[1]>>4);
			out[k++] = (outTmp[1]<<4) | (outTmp[2]>>2);
			out[k++] = (outTmp[2]<<6) | outTmp[3];
		}

		if (bTrimTrailingZeros)
		{
			while (paddingCount > 0 && k > 0 && out[k-1] == '\0')
			{
				--k;
				--paddingCount;
			}
		}

		*resultSize = k;
	}
	return out;
}

static const char base64Char[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char* _base64Encode(char const* origSigned, unsigned origLength)
{
	unsigned char const* orig = (unsigned char const*)origSigned; // in case any input bytes have the MSB set

	unsigned const numOrig24BitValues = origLength/3;
	int havePadding = origLength > numOrig24BitValues*3;
	int havePadding2 = origLength == numOrig24BitValues*3 + 2;
	unsigned const numResultBytes = 4*(numOrig24BitValues + havePadding);
	char* result = (char*)malloc(numResultBytes+1); // allow for trailing '\0'
	if(!result)
		return NULL;
	
	// Map each full group of 3 input bytes into 4 output base-64 characters:
	unsigned i;
	for (i = 0; i < numOrig24BitValues; ++i)
	{
		result[4*i+0] = base64Char[(orig[3*i]>>2)&0x3F];	//lint !e835
		result[4*i+1] = base64Char[(((orig[3*i]&0x3)<<4) | (orig[3*i+1]>>4))&0x3F];
		result[4*i+2] = base64Char[((orig[3*i+1]<<2) | (orig[3*i+2]>>6))&0x3F];
		result[4*i+3] = base64Char[orig[3*i+2]&0x3F];
	}

	// Now, take padding into account.  (Note: i == numOrig24BitValues)
	if (havePadding)
	{
		result[4*i+0] = base64Char[(orig[3*i]>>2)&0x3F];	//lint !e835
		if (havePadding2)
		{
			result[4*i+1] = base64Char[(((orig[3*i]&0x3)<<4) | (orig[3*i+1]>>4))&0x3F];
			result[4*i+2] = base64Char[(orig[3*i+1]<<2)&0x3F];
		}
		else
		{
			result[4*i+1] = base64Char[((orig[3*i]&0x3)<<4)&0x3F];
			result[4*i+2] = '=';
		}
		result[4*i+3] = '=';
	}

	result[numResultBytes] = '\0';
	return result;
}

//base64解码
//str			:要解码的字符串
//len			:字符串长度。0表示让本接口自己计算
//out_lenResult	:[out]输出长度
//返回值		:以'\0'结束的字符串。使用完后调用free来释放。
char *utl_base64_decode(const char *str, int len, int *out_lenResult)
{
	unsigned int resultSize;
	if(!str)
		return NULL;

	if(len <= 0)
		len = strlen(str);
	char *outStr = _base64Decode(str, len, &resultSize, 1);
	if(out_lenResult)
		*out_lenResult = resultSize;
	return outStr;
}

//base64编码
//str		:要编码的字符串
//len		:字符串长度。0表示让本接口自己计算
//返回值	:以'\0'结束的字符串。使用完后调用free来释放。
char *utl_base64_encode(const char *str, int len)
{
	if(!str)
		return NULL;

	if(len <= 0)
		len = strlen(str);
	return _base64Encode(str, len);
}

