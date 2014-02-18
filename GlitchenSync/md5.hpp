// stolen from libcurl and made self-contained (ok it relies on libcc)
// may 22 2008 carl corcoran

#pragma once

#include <string.h>
#include <windows.h>
#include <string>
#include "..\libcc\libcc\stringutil.hpp"
#include "..\libcc\libcc\winapi.hpp"
#include "..\libcc\libcc\result.hpp"

namespace CurlX
{

/* UINT4 defines a four byte word */
typedef unsigned int UINT4;

/* MD5 context. */
struct md5_ctx {
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];                         /* input buffer */
};

typedef struct md5_ctx MD5_CTX;

static void MD5_Init(struct md5_ctx *);
static void MD5_Update(struct md5_ctx *, const unsigned char *, unsigned int);
static void MD5_Final(unsigned char [16], struct md5_ctx *);

/* Constants for MD5Transform routine.
 */

static const int S11 = 7;
static const int S12 = 12;
static const int S13 = 17;
static const int S14 = 22;
static const int S21 = 5;
static const int S22 = 9;
static const int S23 = 14;
static const int S24 = 20;
static const int S31 = 4;
static const int S32 = 11;
static const int S33 = 16;
static const int S34 = 23;
static const int S41 = 6;
static const int S42 = 10;
static const int S43 = 15;
static const int S44 = 21;

static void MD5Transform(UINT4 [4], const unsigned char [64]);
static void Encode(unsigned char *, UINT4 *, unsigned int);
static void Decode(UINT4 *, const unsigned char *, unsigned int);

static const unsigned char PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions.
 */
inline UINT4 F(const UINT4& x, const UINT4& y, const UINT4& z) { return (((x) & (y)) | ((~x) & (z))); }

inline UINT4 G(const UINT4& x, const UINT4& y, const UINT4& z) { return (((x) & (z)) | ((y) & (~z))); }

inline UINT4 H(const UINT4& x, const UINT4& y, const UINT4& z) { return ((x) ^ (y) ^ (z)); }

inline UINT4 I(const UINT4& x, const UINT4& y, const UINT4& z) { return (y) ^ ((x) | (~z)); }


//#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
//#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
//#define H(x, y, z) ((x) ^ (y) ^ (z))
//#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
//#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

inline UINT4 ROTATE_LEFT(const UINT4& x, const int& n) { return (((x) << (n)) | ((x) >> (32-(n)))); }

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
inline void FF(UINT4& a, const UINT4& b, const UINT4& c, const UINT4& d, const UINT4& x, const int& s, const UINT4& ac)
{
 (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac);
 (a) = ROTATE_LEFT ((a), (s));
 (a) += (b);
}
//#define FF(a, b, c, d, x, s, ac) { \
// (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
// (a) = ROTATE_LEFT ((a), (s)); \
// (a) += (b); \
//  }

inline void GG(UINT4& a, const UINT4& b, const UINT4& c, const UINT4& d, const UINT4& x, const int& s, const UINT4& ac)
{
 (a) += G ((b), (c), (d)) + (x) + (UINT4)(ac);
 (a) = ROTATE_LEFT ((a), (s));
 (a) += (b);
}
//#define GG(a, b, c, d, x, s, ac) { \
// (a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
// (a) = ROTATE_LEFT ((a), (s)); \
// (a) += (b); \
//  }

inline void HH(UINT4& a, const UINT4& b, const UINT4& c, const UINT4& d, const UINT4& x, const int& s, const UINT4& ac)
{
 (a) += H ((b), (c), (d)) + (x) + (UINT4)(ac);
 (a) = ROTATE_LEFT ((a), (s));
 (a) += (b);
}
//#define HH(a, b, c, d, x, s, ac) { \
// (a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
// (a) = ROTATE_LEFT ((a), (s)); \
// (a) += (b); \
//  }

inline void II(UINT4& a, const UINT4& b, const UINT4& c, const UINT4& d, const UINT4& x, const int& s, const UINT4& ac)
{
 (a) += I ((b), (c), (d)) + (x) + (UINT4)(ac);
 (a) = ROTATE_LEFT ((a), (s));
 (a) += (b);
}
//#define II(a, b, c, d, x, s, ac) { \
// (a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
// (a) = ROTATE_LEFT ((a), (s)); \
// (a) += (b); \
//  }

/* MD5 initialization. Begins an MD5 operation, writing a new context.
 */
inline static void MD5_Init(struct md5_ctx *context)
{
  context->count[0] = context->count[1] = 0;
  /* Load magic initialization constants. */
  context->state[0] = 0x67452301;
  context->state[1] = 0xefcdab89;
  context->state[2] = 0x98badcfe;
  context->state[3] = 0x10325476;
}

/* MD5 block update operation. Continues an MD5 message-digest
  operation, processing another message block, and updating the
  context.
 */
inline static void MD5_Update (struct md5_ctx *context,    /* context */
                        const unsigned char *input, /* input block */
                        unsigned int inputLen)      /* length of input block */
{
  unsigned int i, bufindex, partLen;

  /* Compute number of bytes mod 64 */
  bufindex = (unsigned int)((context->count[0] >> 3) & 0x3F);

  /* Update number of bits */
  if ((context->count[0] += ((UINT4)inputLen << 3))
      < ((UINT4)inputLen << 3))
    context->count[1]++;
  context->count[1] += ((UINT4)inputLen >> 29);
  
  partLen = 64 - bufindex;

  /* Transform as many times as possible. */
  if (inputLen >= partLen) {
    memcpy((void *)&context->buffer[bufindex], (void *)input, partLen);
    MD5Transform(context->state, context->buffer);
    
    for (i = partLen; i + 63 < inputLen; i += 64)
      MD5Transform(context->state, &input[i]);
    
    bufindex = 0;
  }
  else
    i = 0;

  /* Buffer remaining input */
  memcpy((void *)&context->buffer[bufindex], (void *)&input[i], inputLen-i);
}

/* MD5 finalization. Ends an MD5 message-digest operation, writing the
   the message digest and zeroizing the context.
*/
inline static void MD5_Final(unsigned char digest[16], /* message digest */
                      struct md5_ctx *context) /* context */
{
  unsigned char bits[8];
  unsigned int count, padLen;

  /* Save number of bits */
  Encode (bits, context->count, 8);

  /* Pad out to 56 mod 64. */
  count = (unsigned int)((context->count[0] >> 3) & 0x3f);
  padLen = (count < 56) ? (56 - count) : (120 - count);
  MD5_Update (context, PADDING, padLen);

  /* Append length (before padding) */
  MD5_Update (context, bits, 8);

  /* Store state in digest */
  Encode (digest, context->state, 16);

  /* Zeroize sensitive information. */
  memset ((void *)context, 0, sizeof (*context));
}

/* MD5 basic transformation. Transforms state based on block. */
inline static void MD5Transform(UINT4 state[4],
                         const unsigned char block[64])
{
  UINT4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

  Decode (x, block, 64);

  /* Round 1 */
  FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
  FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
  FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
  FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
  FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
  FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
  FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
  FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
  FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
  FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
  FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
  FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
  FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
  FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
  FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
  FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

 /* Round 2 */
  GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
  GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
  GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
  GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
  GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
  GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
  GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
  GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
  GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
  GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
  GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
  GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
  GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
  GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
  GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
  GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

  /* Round 3 */
  HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
  HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
  HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
  HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
  HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
  HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
  HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
  HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
  HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
  HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
  HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
  HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
  HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
  HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
  HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
  HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

  /* Round 4 */
  II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
  II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
  II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
  II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
  II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
  II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
  II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
  II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
  II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
  II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
  II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
  II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
  II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
  II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
  II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
  II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;

  /* Zeroize sensitive information. */
  memset((void *)x, 0, sizeof (x));
}

/* Encodes input (UINT4) into output (unsigned char). Assumes len is
  a multiple of 4.
 */
inline static void Encode (unsigned char *output,
                    UINT4 *input,
                    unsigned int len)
{
  unsigned int i, j;

  for (i = 0, j = 0; j < len; i++, j += 4) {
    output[j] = (unsigned char)(input[i] & 0xff);
    output[j+1] = (unsigned char)((input[i] >> 8) & 0xff);
    output[j+2] = (unsigned char)((input[i] >> 16) & 0xff);
    output[j+3] = (unsigned char)((input[i] >> 24) & 0xff);
  }
}

/* Decodes input (unsigned char) into output (UINT4). Assumes len is
   a multiple of 4.
*/
inline static void Decode (UINT4 *output,
                    const unsigned char *input,
                    unsigned int len)
{
  unsigned int i, j;

  for (i = 0, j = 0; j < len; i++, j += 4)
    output[i] = ((UINT4)input[j]) | (((UINT4)input[j+1]) << 8) |
      (((UINT4)input[j+2]) << 16) | (((UINT4)input[j+3]) << 24);
}


//#include "md5.h"

inline void Curl_md5it(unsigned char *outbuffer, /* 16 bytes */
                const unsigned char *input)
{
  MD5_CTX ctx;
  MD5_Init(&ctx);
  MD5_Update(&ctx, input, (unsigned int)strlen((char *)input));
  MD5_Final(outbuffer, &ctx);
}

}


class MD5Result
{
public:
	MD5Result()
	{
	}

	bool Equals(const MD5Result& rhs) const
	{
		return memcmp(checksum, rhs.checksum, 16) == 0;
	}

	BYTE checksum[16];

	std::wstring ToString() const
	{
		LibCC::Format f;
		for(int i = 0; i < 16; ++ i)
		{
			f.i<16, 2>(checksum[i]);
		}
		return f.Str();
	}
};

inline MD5Result MD5Encode(const void* data, size_t len)
{
	MD5Result ret;
	//unsigned char outbuffer[16];
	CurlX::MD5_CTX ctx;
  CurlX::MD5_Init(&ctx);
  CurlX::MD5_Update(&ctx, (const unsigned char*)data, (unsigned int)len);
  CurlX::MD5_Final(ret.checksum, &ctx);
	//std::basic_string<Char> ret;
	//LibCC::StringConvert(outbuffer, ret);
	return ret;
}


inline LibCC::Result MD5EncodeFile(MD5Result& out, const std::wstring& fileName, size_t bufferSize = 1000000)
{
	HANDLE h = CreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if(LibCC::IsBadHandle(h))
		return LibCC::Result::Failure(LibCC::Format(L"Error opening file (error %) %").gle().qs(fileName).Str());

	LibCC::Blob<BYTE> buffer(bufferSize);
	CurlX::MD5_CTX ctx;
  CurlX::MD5_Init(&ctx);

	DWORD totalBytesRead = 0;
	while(true)
	{
		DWORD bytesRead = 0;
		if(0 != ReadFile(h, buffer.GetBuffer(), (DWORD)bufferSize, &bytesRead, 0))
		{
			if(bytesRead == 0)
				break;
		}
		totalBytesRead += bytesRead;
		CurlX::MD5_Update(&ctx, buffer.GetBuffer(), bytesRead);
	}

	CloseHandle(h);

	CurlX::MD5_Final(out.checksum, &ctx);

	return LibCC::Result::Success(LibCC::Format(L"MD5 %, % bytes - %").s(out.ToString()).ui(totalBytesRead).qs(fileName).Str());
}


