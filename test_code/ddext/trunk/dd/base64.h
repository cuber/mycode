//
//  base64.h
//
//  Created by Zhibiao Pan on 11-11-2.
//  Copyright 2011.11 DangDang Inc. All rights reserved.
//

#ifndef PHP_DD_BASE64_H
#define PHP_DD_BASE64_H

#include <string.h>
#include <ctype.h>

namespace dd {
  size_t base64_encode(const unsigned char *src, size_t len, char *dest);
  size_t base64_decode(const char *src, unsigned char *dest);
  
  inline size_t check_length(const char *src);
  inline char base64_decode_digit(const char ch);
  
  // 这个不是完整长度，是第一个'='出现的长度
  inline size_t check_length(const char *src) {
    const char *p = strchr(src, '=');
    if(p == NULL) return strlen(src);
    else
      return p - src;
  }
  
  inline char base64_decode_digit(const char ch) {
    switch(ch) {
      case '+': return 62;
      case '/': return 63;
      default:
        if(isdigit(ch)) return ch - '0' + 52;
        else if(islower(ch)) return ch - 'a' + 26;
        else if(isupper(ch)) return ch - 'A';
    }
    return 0xFF;
  }
}

#endif
