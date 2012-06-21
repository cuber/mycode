//
//  base64.c
//
//  Created by Zhibiao Pan on 11-11-2.
//  Copyright 2011.11 DangDang Inc. All rights reserved.
//

#include "base64.h"

#include <stdio.h>

namespace dd {
  size_t base64_encode(const unsigned char *src, size_t len, char *dest) {
    static const char *encode_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char *ori_dest = dest;
    int i = 0, n = 0, r = 0;
    
    while(i+3 <= len) {
      n  = *(src+i)   << 16;
      n |= *(src+i+1) << 8;
      n |= *(src+i+2);
      *(dest++) = encode_table[(n & 0x00FC0000) >> 18];
      *(dest++) = encode_table[(n & 0x0003F000) >> 12];
      *(dest++) = encode_table[(n & 0x00000FC0) >> 6];
      *(dest++) = encode_table[(n & 0x0000003F)];
      i += 3;
    }
    r = (int)(len % 3);
    if(r == 1) {
      n = *(src+i) << 16;
      *(dest++) = encode_table[(n & 0x00FC0000) >> 18];
      *(dest++) = encode_table[(n & 0x0003F000) >> 12];
      *(dest++) = '=';
      *(dest++) = '=';
    }
    if(r == 2) {
      n = *(src+i) << 16;
      n |= *(src+i+1) << 8;
      *(dest++) = encode_table[(n & 0x00FC0000) >> 18];
      *(dest++) = encode_table[(n & 0x0003F000) >> 12];
      *(dest++) = encode_table[(n & 0x00000FC0) >> 6];
      *(dest++) = '=';
    }
    
    *dest = '\0';
    return dest - ori_dest;
  }
  
  
  size_t base64_decode(const char *src, unsigned char *dest) {
    int i = 0, n, r;
    size_t len;
    unsigned char *ori_dest = dest;
    
    len = check_length(src);
    while(i+4 <= len) {
      n  = base64_decode_digit(*(src+i))   << 18;
      n |= base64_decode_digit(*(src+i+1)) << 12;
      n |= base64_decode_digit(*(src+i+2)) << 6;
      n |= base64_decode_digit(*(src+i+3));
      *(dest++) = n >> 16;
      *(dest++) = (n >> 8) & 0xFF;
      *(dest++) = n & 0xFF;
      i += 4;
    }
    r = (int)(len % 4);
    if(r == 2) {
      n  = base64_decode_digit(*(src+i))   << 18;
      n |= base64_decode_digit(*(src+i+1)) << 12;
      *(dest++) = n >> 16;
    }
    if(r == 3)
    {
      n  = base64_decode_digit(*(src+i))   << 18;
      n |= base64_decode_digit(*(src+i+1)) << 12;
      n |= base64_decode_digit(*(src+i+2)) << 6;
      *(dest++) = n >> 16;
      *(dest++) = (n >> 8) & 0xFF;
    }
    *dest = '\0';
    
    return dest - ori_dest;
  }
  
} // /namespace
