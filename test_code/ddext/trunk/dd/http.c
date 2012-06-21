//
//  http.c
//  smsd
//
//  Created by Zhibiao Pan on 12-3-8.
//  Copyright 2012年 Dangdang Inc. All rights reserved.
//

#include "http.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>


/*
 -------------------------------------------------------------------------------
 static function
 -------------------------------------------------------------------------------
 */
static size_t cb_curl_write_func(char *data, size_t size, size_t nmemb,
                                 void *userdata);

static int http_request(struct http *h, const char *url,
                        struct http_buf *write_htp_buf,
                        struct http_buf *post_htp_buf);

static int http_expand_double(struct http_buf *htp_buf);
static int http_buf_add_vprintf(struct http_buf *htp_buf, const char *fmt, va_list ap);


/*
 -------------------------------------------------------------------------------
 http_buf_*
 -------------------------------------------------------------------------------
 */
struct http_buf *http_buf_new()
{
  struct http_buf *htp_buf;
  htp_buf = (struct http_buf *)calloc(1, sizeof(struct http_buf));
  
  const int init_data_size = 1024 * 32;
  htp_buf->data     = (char *)calloc(1, init_data_size);
  htp_buf->dsize    = 0;
  htp_buf->capacity = init_data_size;
  return htp_buf;
}


void http_buf_free(struct http_buf *htp_buf)
{
  assert(htp_buf);
  free(htp_buf->data);
  free(htp_buf);
}


int http_buf_append(struct http_buf *htp_buf,
                    const char *data, size_t len)
{
  /* expand if need
   * htp_buf->data is always null-terminated
   */
  while ((int)len > htp_buf->capacity - htp_buf->dsize - 1) {
    if (http_expand_double(htp_buf) != 0) {
      return -1;
    }
  }
  
  memcpy(htp_buf->data + htp_buf->dsize, data, len);
  htp_buf->dsize += len;
  
  assert(htp_buf->capacity - htp_buf->dsize > 0);
  return 0;
}


int http_buf_add_printf(struct http_buf *htp_buf, const char *fmt, ...)
{
  assert(htp_buf);
  
  va_list ap;
  va_start(ap, fmt);
  int res = http_buf_add_vprintf(htp_buf, fmt, ap);
  va_end(ap);
  return res;
}


static int http_expand_double(struct http_buf *htp_buf)
{
  assert(htp_buf);
  htp_buf->data = (char *)realloc(htp_buf->data, htp_buf->capacity*2);
  if (htp_buf->data == NULL) { return -1; }
  memset(htp_buf->data + htp_buf->dsize,
         '\0', htp_buf->capacity*2 - htp_buf->dsize);
  htp_buf->capacity *= 2;
  return 0;
}


static int http_buf_add_vprintf(struct http_buf *htp_buf,
                                const char *fmt, va_list ap)
{
  int space, n;
  va_list aq;
  
  for (;;) {
    space = htp_buf->capacity - htp_buf->dsize;
    assert(space > 0);

    va_copy(aq, ap);
    n = vsnprintf(htp_buf->data + htp_buf->dsize, space, fmt, aq);
    va_end(aq);
    
    /* if the return value is greater than or equal to the space argument,
       the string was too short and some of the printed characters 
       were discarded
     */
    if (n < space) {
      htp_buf->dsize += n;
      break;
    }
    /* no enough space, expand buffer and continue to try again */
    if (http_expand_double(htp_buf) != 0) { return -1; }
  }
  return 0;
}


int http_buf_size(struct http_buf *htp_buf)
{
  assert(htp_buf);
  return htp_buf->dsize;
}

const char *http_buf_data(struct http_buf *htp_buf)
{
  assert(htp_buf);
  return htp_buf->data;
}

void http_buf_clear(struct http_buf *htp_buf)
{
  assert(htp_buf);
  memset(htp_buf->data, '\0', htp_buf->capacity);
  htp_buf->dsize = 0;
}




/*
 -------------------------------------------------------------------------------
 http_*
 -------------------------------------------------------------------------------
 */

/*
 * curl_global_init() should be invoked exactly once for each application that
 * uses libcurl and before any call of other libcurl functions.
 *
 * This function is not thread-safe!
 */
void http_init()
{
  curl_global_init(CURL_GLOBAL_DEFAULT);
}


struct http *http_new()
{
  struct http *h;
  h = (struct http *)calloc(1, sizeof(struct http));
  if (h == NULL) { return NULL; }
  
  strncpy(h->user_agent,
          /* UA: IE8 on Windows 7 */
          "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; Trident/4.0)",
          sizeof(h->user_agent));
  h->user_agent[sizeof(h->user_agent) - 1] = '\0';
  h->curl_code      = CURLE_OK;
  h->conn_timeout   = 10;             /* http request timeout seconds */
  
  return h;
}

void http_free(struct http *h)
{
  assert(h);
  free(h);
}


int http_set_user_agent(struct http *h, const char *user_agent)
{
  assert(h);
  if (user_agent == NULL || strlen(user_agent) == 0) { return -1; }
  
  strncpy(h->user_agent, user_agent, sizeof(h->user_agent));
  h->user_agent[sizeof(h->user_agent) - 1] = '\0';
  return 0;
}

int http_set_timeout(struct http *h, const int timeout_sec)
{
  assert(h);
  if (timeout_sec <= 0) { return -1; }
  h->conn_timeout = timeout_sec;
  return 0;
}


static int http_request(struct http *h, const char *url,
                        struct http_buf *write_htp_buf,
                        struct http_buf *post_htp_buf)
{
  assert(h);
  assert(write_htp_buf);
  
  if (NULL == url) { return -1; }
  http_buf_clear(write_htp_buf);
  
  CURL *curl;
  if ((curl = curl_easy_init()) == NULL) {
    return -1;
  }
  
  /* curl easy options:
   * @see <http://curl.haxx.se/libcurl/c/curl_easy_setopt.html>
   */
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, h->user_agent);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT,   h->conn_timeout);
  /* use DNS curl global cache */
  curl_easy_setopt(curl, CURLOPT_DNS_USE_GLOBAL_CACHE, 1L);
  curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, 5*60); /* cache seconds */
  
  if (NULL != post_htp_buf) {
    /* HTTP POST */
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,    http_buf_data(post_htp_buf));
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, http_buf_size(post_htp_buf));
  } else {
    /* If the long is 1, this forces the HTTP request to get back to GET */
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
  }
  
  /* set to 1 tells the library to include the header in the body output */
  curl_easy_setopt(curl, CURLOPT_HEADER, 0);
  
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb_curl_write_func);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, write_htp_buf);
  
  h->curl_code = curl_easy_perform(curl);
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &h->rsp_code);
  
  /* always cleanup */ 
  curl_easy_cleanup(curl);
  
  if (h->curl_code == CURLE_OK) { return 0; }
  
  return -1;
}

int http_get(struct http *h, const char *url, struct http_buf *htp_buf)
{
  return http_request(h, url, htp_buf, NULL);
}

int http_post(struct http *h, const char *url,
              struct http_buf *write_htp_buf, struct http_buf *post_htp_buf)
{
  return http_request(h, url, write_htp_buf, post_htp_buf);
}


static size_t cb_curl_write_func(char *data, size_t size, size_t nmemb,
                                 void *userdata)
{
  if (userdata == NULL) { return 0; }
  http_buf_append((struct http_buf *)userdata, data, size * nmemb);
  return size * nmemb;
}


int http_url_encode(struct http_buf *htp_buf, const char *s)
{
  static unsigned char hexchars[] = "0123456789ABCDEF";
  
  http_buf_clear(htp_buf);
  if (NULL == s) { return -1; }
  
  unsigned char str[3] = {0};
  while (*s) {
    str[0] = (unsigned char)*s;
    
    if ((str[0] < '0' && str[0] != '-' && str[0] != '.') ||
        (str[0] < 'A' && str[0] > '9') ||
        (str[0] > 'Z' && str[0] < 'a' && str[0] != '_') ||
        (str[0] > 'z' && str[0] != '~')) {
      str[0] = '%';
      str[1] = hexchars[(unsigned char)*s >> 4];
      str[2] = hexchars[(unsigned char)*s & 15];
      http_buf_append(htp_buf, (char *)str, 3);
    } else {
      http_buf_append(htp_buf, (char *)str, 1);
    }
    s++;
  }
  return 0;
}



