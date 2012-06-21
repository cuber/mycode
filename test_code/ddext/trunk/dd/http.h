//
//  http.h
//  smsd
//
//  Created by Zhibiao Pan on 12-3-8.
//  Copyright 2012年 Dangdang Inc. All rights reserved.
//

#ifndef smsd_http_h
#define smsd_http_h

#include <stdarg.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

/*
 -------------------------------------------------------------------------------
 how to use
 -------------------------------------------------------------------------------
 struct http *http;
 struct http_buf *htp_buf;
 
 http_init();
 
 if (NULL != (http = http_new())) {
 // error
 }
 http_set_timeout(http, 10);
 
 if (NULL != (htp_buf = http_buf_new())) {
 // error
 }
 
 http_get(http, "http://www.baidu.com/", htp_buf);
 printf("data string: %s\n", http_buf_data(htp_buf));
 printf("data size: %d\n",   http_buf_size(htp_buf));
 printf("http rsp_code: %ld\n", http->rsp_code);
 
 
 struct http_buf *post = http_buf_new();
 
 const char *s = "hello=1&sd=23";
 http_buf_append(post, s, strlen(s));
 
 http_buf_add_printf(post, "&%s=%s", "1", "1");
 http_buf_add_printf(post, "&%s=%s", "2", "2");
 http_buf_add_printf(post, "&%s=%s", "3", "3");
 http_buf_add_printf(post, "&%s=%s", "4", "4");
 http_buf_add_printf(post, "&%s=%s", "5", "5");
 
 http_post(http, "http://localhost/www/t.php?a=1&b=2", htp_buf, post);
 printf("data string: %s\n", http_buf_data(htp_buf));
 printf("data size: %d\n",   http_buf_size(htp_buf));
 printf("http rsp_code: %ld\n", http->rsp_code);
 
 http_buf_free(htp_buf);
 http_free(http);
*/



struct http_buf {
  char *data;
  int   dsize;     /* data size */
  int   capacity;  /* capacity */
};


struct http_buf *http_buf_new();
void             http_buf_free(struct http_buf *htp_buf);
void        http_buf_clear(struct http_buf *htp_buf);
int         http_buf_size (struct http_buf *htp_buf);
const char *http_buf_data (struct http_buf *htp_buf);
int  http_buf_append(struct http_buf *htp_buf, const char *data, size_t nsize);
int  http_buf_add_printf(struct http_buf *htp_buf, const char *fmt, ...);


struct http {
  char     user_agent[1024];
  
  /* last request & response info */
  CURLcode curl_code;
  long   rsp_code;        /* CURLINFO_RESPONSE_CODE */
  int    conn_timeout;    /* seconds, CURLOPT_TIMEOUT */
};


void http_init();

struct http *http_new();
void         http_free(struct http *h);
int http_set_user_agent(struct http *h, const char *user_agent);
int http_set_timeout   (struct http *h, const int timeout_sec);

/*
 * support url redirect
 */
int http_get (struct http *h, const char *url, struct http_buf *write_htp_buf);
int http_post(struct http *h, const char *url,
              struct http_buf *write_htp_buf, struct http_buf *post_htp_buf);

int http_url_encode(struct http_buf *htp_buf, const char *s);

#endif
