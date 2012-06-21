//
//  dd_cookie.cc
//
//  Created by Zhibiao Pan on 11-11-2.
//  Copyright 2011.11 DangDang Inc. All rights reserved.
//

#include "dd_cookie.h"

#include "md5.h"

#include <mcrypt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>


DDCookie::DDCookie() {}


int DDCookie::Init() {
  cmd5_ = new dd::MD5();
  
  td_ = mcrypt_module_open(MCRYPT_3DES, NULL, MCRYPT_ECB, NULL);
  block_size_ = mcrypt_enc_get_block_size(td_);
  
  size_t iv_size = mcrypt_enc_get_iv_size(td_);
  IV_ = (char *)malloc(iv_size);
  /* Put random data in IV. using /dev/random or /dev/urandom. */
  int    fd;
  int    n = 0;
  size_t read_bytes = 0;
  fd = open("/dev/urandom", O_RDONLY);
  if (fd == -1) { return -1; }
  while (read_bytes < iv_size) {
    n = (int)read(fd, IV_ + read_bytes, iv_size - read_bytes);
    if (n < 0) { break; }
    read_bytes += n;
  }
  close(fd);
  
  return 0;
}


DDCookie::~DDCookie() {
  mcrypt_module_close(td_);
  
  if (IV_) { free(IV_); }
  if (cmd5_) { delete cmd5_; }
}


void DDCookie::md5(char *str, size_t len, unsigned char *md5) {
  cmd5_->MD5Code(str, (unsigned int)len, md5);
}


// decryptNET3DES
void DDCookie::decryptNET3DES(const char *key,
                              const char *encoded_str, char *outbuf) {
  if (strlen(encoded_str) == 0) { return; }
  
  char key24[25] = {0};
  fillKey24(key, key24);

  size_t data_len = strlen(encoded_str);
  memset(outbuf, 0, data_len);
  memcpy(outbuf, encoded_str, data_len);
  // decrypt
  mcrypt_generic_init(td_, key24, 24, IV_);
  mdecrypt_generic(td_, outbuf, (int)data_len);
  mcrypt_generic_deinit(td_);
  outbuf[data_len] = '\0';

  // 判断是否有padding尾巴，如果有，则去掉
  // 去掉padding的尾巴，因为.net中默认的padding是PKCS7
  size_t outbuf_len = strlen(outbuf);
  int padding = (unsigned char)outbuf[outbuf_len-1];
  if (0 < padding && padding <= block_size_) {
    // !! 这里有可能等于block_size_ !!
    bool has_padding_tail = true;
    for (int i = 0; i < padding; ++i) {
      if ((unsigned char)outbuf[outbuf_len - 1 - i] != padding) {
        has_padding_tail = false; // 出现不一致，表示没有尾巴
        break;
      }
    }
    if (has_padding_tail == true) {
      outbuf[outbuf_len - padding] = '\0';
    }
  }
}


// encryptNET3DES
// 警告：outbuf 的长度有可能超过 orignal_str，最大超过长度为 block_size_-1
//      encryptNET3DES，此函数目前未严格测试
void DDCookie::encryptNET3DES(const char *key,
                              const char *orignal_str,  char *outbuf) {
  if (strlen(orignal_str) == 0) { return; }

  char key24[25] = {0};
  fillKey24(key, key24);
  
  int data_len = (int)strlen(orignal_str);
  // 拷贝原内容
  memcpy(outbuf, orignal_str, data_len);
  // 补齐padding
  int padding_len = block_size_ - data_len % block_size_;
  if (padding_len) {
    memset(outbuf + data_len, padding_len, padding_len);
  }
  
  // encrypt
  mcrypt_generic_init(td_, key24, 24, IV_);
  mcrypt_generic(td_, outbuf, data_len + padding_len);
  mcrypt_generic_deinit(td_);
  outbuf[data_len + padding_len] = '\0';
}


// get authKey, $_COOKIE['login_dangdang_com']
void DDCookie::getAuthKey(const char *login_dangdang_com, char *authKey,
                          size_t authKey_len) {
  char *tmp_str = strdup(login_dangdang_com);
   if (tmp_str == NULL) { return; }
  
  char *pch = strtok(tmp_str, "&");
  while (pch != NULL) {
    if (strstr(pch, ".ASPXAUTH=") == pch) {
      strncpy(authKey, pch + strlen(".ASPXAUTH="), authKey_len-1);
      break;
    }
    pch = strtok (NULL, "&");
  }  // /while (pch != NULL)
  
  // replace ' ' to '+'
  for (int i = (int)strlen(authKey) - 1; i >= 0; --i) {
    if (authKey[i] == ' ') {
      authKey[i] = '+';
    }
  }
  if (tmp_str) { free(tmp_str); }
}


// get user info, $_COOKIE['dangdang_com']
void DDCookie::getOrignalUserInfo(const char *dangdang_com,
                                  char (*orignal_user_arr)[DD_COOKIE_USER_VALUE_SIZE]) {
  char *tmp_str = strdup(dangdang_com);
  if (tmp_str == NULL) { return; }
  
  static const char *key_arr[] = {
    /* 请勿随意调整下列key的顺序，新增key请加到最后，并在dd_cookie.h新增相关宏 */
    "email=",       "customerid=",   "nickname=",
    "display_id=",  "viptype=",      "show_name="
  };
  char *pch = strtok(tmp_str, "&");
  while (pch != NULL) {
    int key_len = 0, key_index = 0;
    for (key_index = 0; key_index < sizeof(key_arr)/sizeof(key_arr[0]); ++key_index) {
      if (strstr(pch, key_arr[key_index]) == pch) {
        key_len = (int)strlen(key_arr[key_index]);
        break;
      }
    }
    // cpy
    strncpy(orignal_user_arr[key_index],
            pch + key_len, DD_COOKIE_USER_VALUE_SIZE - 1);
    // replace ' ' to '+'
    for (int i = (int)strlen(orignal_user_arr[key_index]) - 1; i >= 0; --i) {
      if (orignal_user_arr[key_index][i] == ' ') {
        orignal_user_arr[key_index][i] = '+';
      }
    }
    // find next
    pch = strtok (NULL, "&");
  } // /while (pch != NULL)
  
  if (tmp_str) { free(tmp_str); }
}


// get login user info
int DDCookie::getLoginUser(const char *dangdang_com,
                           const char *login_dangdang_com,
                           char (*user_arr)[DD_COOKIE_USER_VALUE_SIZE]) {
  if (dangdang_com == NULL || login_dangdang_com == NULL) { return -1; }
  char authKey[50] = {0};
  getAuthKey(login_dangdang_com, authKey, sizeof(authKey));
  char orignal_user_arr[DD_COOKIE_USER_KEY_SIZE][DD_COOKIE_USER_VALUE_SIZE] = {0};
  getOrignalUserInfo(dangdang_com, orignal_user_arr);

  // check necessary fields
  if (strlen(authKey) == 0 ||
      strlen(orignal_user_arr[DD_COOKIE_USER_KEY_EMAIL_IDX]) == 0 ||
      strlen(orignal_user_arr[DD_COOKIE_USER_KEY_CUSTOMERID_IDX]) == 0) {
    return -1;
  }
  
  // these fields not need decrypt
  dd::base64_decode(orignal_user_arr[DD_COOKIE_USER_KEY_EMAIL_IDX],     // email
                    (unsigned char *)user_arr[DD_COOKIE_USER_KEY_EMAIL_IDX]);
  dd::base64_decode(orignal_user_arr[DD_COOKIE_USER_KEY_NICKNAME_IDX],  // nickname
                    (unsigned char *)user_arr[DD_COOKIE_USER_KEY_NICKNAME_IDX]);
  strcpy(user_arr[DD_COOKIE_USER_KEY_DISPLAYID_IDX],    // display_id
         orignal_user_arr[DD_COOKIE_USER_KEY_DISPLAYID_IDX]);
  strcpy(user_arr[DD_COOKIE_USER_KEY_SHOWNAME_IDX],     // show_name
         orignal_user_arr[DD_COOKIE_USER_KEY_SHOWNAME_IDX]);
  
  // TODO
  // 'viptype' 如何编码的?
  char     first_md5_str[50] = {0};  // 第一次md5的源字符串
  unsigned char md5val_1[17] = {0};  // 第一次md5的值
  unsigned char md5val_2[17] = {0};  // 第二次md5的值
  char            base64[80] = {0};  // 第一次md5值进行base64编码的值,并作第二次md5的源字符串
  char       encryed_str[50] = {0};  // 密文，大部分由base64解码得到

  /* customerid */
  strcpy(first_md5_str, DD_COOKIE_CRYPTO_KEY);
  strcpy(first_md5_str + strlen(DD_COOKIE_CRYPTO_KEY),
         user_arr[DD_COOKIE_USER_KEY_EMAIL_IDX]);
  // md5 -> base64_encode -> md5
  md5(first_md5_str, strlen(first_md5_str), md5val_1);
  dd::base64_encode(md5val_1, strlen((char *)md5val_1), base64);
  md5(base64, strlen(base64), md5val_2);
  // decrypt
  dd::base64_decode(orignal_user_arr[DD_COOKIE_USER_KEY_CUSTOMERID_IDX],
                    (unsigned char*)encryed_str);
  memset(user_arr[DD_COOKIE_USER_KEY_CUSTOMERID_IDX], 0, DD_COOKIE_USER_VALUE_SIZE);
  decryptNET3DES((char *)md5val_2, encryed_str,
                 user_arr[DD_COOKIE_USER_KEY_CUSTOMERID_IDX]);
  
  memset(first_md5_str, 0, sizeof(first_md5_str));
  memset(md5val_1,      0, sizeof(md5val_1));
  memset(md5val_2,      0, sizeof(md5val_2));
  memset(base64,        0, sizeof(base64));
  memset(encryed_str,   0, sizeof(encryed_str));
  /* email */
  char check_email[80] = {0};
  strcpy(first_md5_str, DD_COOKIE_CRYPTO_KEY);
  strcpy(first_md5_str + strlen(DD_COOKIE_CRYPTO_KEY),
         user_arr[DD_COOKIE_USER_KEY_CUSTOMERID_IDX]);
  // md5 -> base64_encode -> md5
  md5(first_md5_str, strlen(first_md5_str), md5val_1);
  dd::base64_encode(md5val_1, strlen((char *)md5val_1), base64);
  md5(base64, strlen(base64), md5val_2);
  // decrypt
  dd::base64_decode(authKey, (unsigned char*)encryed_str);
  decryptNET3DES((char *)md5val_2, encryed_str, check_email);

  if (strcmp(check_email, user_arr[DD_COOKIE_USER_KEY_EMAIL_IDX]) == 0) {
    return 0; // check user info ok
  }
  return -1;
}  // /DDCookie::getLoginUser


