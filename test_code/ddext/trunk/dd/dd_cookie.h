//
//  dd_cookie.h
//
//  Created by Zhibiao Pan on 11-11-2.
//  Copyright 2011.11 DangDang Inc. All rights reserved.
//

#ifndef PHP_DD_COOKIE_H
#define PHP_DD_COOKIE_H

#include "base64.h"
#include "md5.h"

#include <mcrypt.h>
#include <string>


#define DD_COOKIE_CRYPTO_KEY            "cookie_crypto_key_initial"

/* For $_COOKIE['dangdang_com'] */
#define DD_COOKIE_USER_KEY_SIZE       6
#define DD_COOKIE_USER_VALUE_SIZE     100

#define DD_COOKIE_USER_KEY_EMAIL_IDX       0
#define DD_COOKIE_USER_KEY_CUSTOMERID_IDX  1
#define DD_COOKIE_USER_KEY_NICKNAME_IDX    2
#define DD_COOKIE_USER_KEY_DISPLAYID_IDX   3
#define DD_COOKIE_USER_KEY_VIPTYPE_IDX     4
#define DD_COOKIE_USER_KEY_SHOWNAME_IDX    5


class DDCookie {
private:
  MCRYPT td_;
  char *IV_;
  int block_size_;
  dd::MD5 *cmd5_;

  void md5(char *str, size_t len, unsigned char *md5);
  inline void fillKey24(const char *, char *);
  void getAuthKey(const char *, char *, size_t);
  void getOrignalUserInfo(const char *,
                          char (*)[DD_COOKIE_USER_VALUE_SIZE]);
  
public:
  DDCookie();
  ~DDCookie();
  int Init();
  
  // get login user info
  int getLoginUser(const char *dangdang_com,
                   const char *login_dangdang_com,
                   char (*user_arr)[DD_COOKIE_USER_VALUE_SIZE]);
  
  // decryptNET3DES
  void decryptNET3DES(const char *key,
                      const char *encoded_str,  char *outbuf);
  
  // encryptNET3DES
  // 警告：outbuf 的长度有可能超过 orignal_str，最大超过长度为 block_size_-1
  void encryptNET3DES(const char *key,
                      const char *orignal_str,  char *outbuf);
  
};



// 补齐key至24位
inline void DDCookie::fillKey24(const char *old_key, char *key) {
  // 把key值补充完整，在PHP里面如果key值不够24位剩下的会自动补0
  // 但是在.net中，会做一个循环把前面的值补充到后面补够24位，所以这里强制补前面的字符
  // 在这里所有的key都是md5后生成的，一般不会出现key长度小于12的情形
  size_t old_key_len = strlen(old_key);
  memcpy(key, old_key, old_key_len);
  if (old_key_len < 24) {
    memcpy(key + old_key_len, old_key, 24 - old_key_len);
  }
}



#endif
