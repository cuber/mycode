//
//  md5.h
//
//  Created by Zhibiao Pan on 11-11-2.
//  Copyright 2011.11 DangDang Inc. All rights reserved.
//

#ifndef PHP_DD_MD5_H
#define PHP_DD_MD5_H

#include <time.h>
#include <stdio.h>
#include <string.h>

namespace dd {
  
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21
  
#define TEST_BLOCK_LEN 1000
#define TEST_BLOCK_COUNT 10000
  
  /*
   static void MD5Transform PROTO_LIST ((UINT4 [4], unsigned char [64]));
   static void Encode PROTO_LIST ((unsigned char *, UINT4 *, unsigned int));
   static void Decode PROTO_LIST ((UINT4 *, unsigned char *, unsigned int));
   static void MD5_memcpy PROTO_LIST ((POINTER, POINTER, unsigned int));
   static void MD5_memset PROTO_LIST ((POINTER, int, unsigned int));
   */
  static unsigned char PADDING[64] = {0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  /* F, G, H and I are basic MD5 functions.
   */
  
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))
  
  /* ROTATE_LEFT rotates x left n bits.
   */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))
  
  /* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
   Rotation is separate from addition to prevent recomputation.
   */
#define FF(a, b, c, d, x, s, ac) { \
(a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
(a) = ROTATE_LEFT ((a), (s)); \
(a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) { \
(a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
(a) = ROTATE_LEFT ((a), (s)); \
(a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) { \
(a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
(a) = ROTATE_LEFT ((a), (s)); \
(a) += (b); \
}
#define II(a, b, c, d, x, s, ac) { \
(a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
(a) = ROTATE_LEFT ((a), (s)); \
(a) += (b); \
}
  
  /* POINTER defines a generic pointer type */
  typedef unsigned char *POINTER;
  
  /* UINT4 defines a four byte word */
  typedef unsigned int UINT4;
  
  typedef struct tMD5_CTX {
	  UINT4 state[4];             /* state (ABCD)                            */
	  UINT4 count[2];             /* number of bits, modulo 2^64 (lsb first) */
	  unsigned char buffer[64];   /* input buffer                            */
  } MD5_CTX;
  
  class MD5 {
    // Construction
  public:
    MD5() {}
    ~MD5() {}
  public:
    unsigned char result[33];
    
    /* Digests the standard input and prints the result.
     */
    void MD5Init (MD5_CTX *context);
    
    /* MD5 block update operation. Continues an MD5 message-digest
     operation, processing another message block, and updating the
     context.
     */
    void MD5Update (MD5_CTX *context, unsigned char *input, unsigned int inputLen);
    
    /* MD5 finalization. Ends an MD5 message-digest operation, writing the
     the message digest and zeroizing the context.
     */
    void MD5Final (unsigned char digest[16],MD5_CTX *context);
    
    /* Main driver.
     Arguments (may be any combination):
     -sstring - digests string
     -t       - runs time trial
     -x       - runs test script
     filename - digests file
     (none)   - digests standard input
     */
    /* Digests a string and prints the result.
     */
    void MD5String (char *string, unsigned int stringLen);
    
    void MD5Code(char* string, unsigned int stringlen,unsigned char *digest);
    
    /* Measures the time to digest TEST_BLOCK_COUNT TEST_BLOCK_LEN-byte
     blocks.
     */
    void MD5TimeTrial ();
    
    /* Digests a reference suite of strings and prints the results.
     */
    void MD5TestSuite ();
    
    /* Digests a file and prints the result.
     */
    void MD5File (char *filename);
    
    void MD5Filter ();
    
    /* Prints a message digest in hexadecimal.
     */
    void MD5Print (unsigned char digest[16]);
    
    /* MD5 basic transformation. Transforms state based on block.
     */
    void MD5Transform (UINT4 state[4],unsigned char block[64]);
    
    /* Encodes input (UINT4) into output (unsigned char). Assumes len is
     a multiple of 4.
     */
    void Encode (unsigned char *output,UINT4 *input,unsigned int len);
    
    /* Decodes input (unsigned char) into output (UINT4). Assumes len is
     a multiple of 4.
     */
    void Decode (UINT4 *output,unsigned char *input,unsigned int len);
    
    /* Note: Replace "for loop" with standard memcpy if possible.
     */
    void MD5_memcpy (POINTER output,POINTER input,unsigned int len);
    
    /* Note: Replace "for loop" with standard memset if possible.
     */
    void MD5_memset (POINTER output,int value,unsigned int len);
    
  };

}

#endif
