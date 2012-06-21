//
//  dd_stat.cc
//
//  Created by Zhibiao Pan on 2012-05-31
//  Copyright 2012 DangDang Inc. All rights reserved.
//

#include "dd_stat.h"
extern "C" {
#include "http.h"
}

#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>


DDStat::DDStat()
{
  memset(host_, 0, sizeof(host_));
  port_ = 0;
  
  http_init();
}

DDStat::~DDStat() {}


int DDStat::initTrackServer(const char *host, int port)
{
  if (host == NULL || port <= 0) {
    return -1;
  }
  strncpy(host_, host, sizeof(host_));
  port_ = port;
  
  return 0;
}


int DDStat::trackByHTTP(int op, int trk_id, const char *data, int dtlen)
{
  struct http     *http;
  struct http_buf *htp_buf;
  
  http    = http_new();
  htp_buf = http_buf_new();
  if (NULL == http || htp_buf == NULL) {
    return -1;
  }
  http_set_timeout(http, 1);
  
  char url[2048] = {0};
  snprintf(url, sizeof(url), "http://%s:%d/?trk_id=%d&op=%d&dtlen=%d&data=%s",
           host_, port_, trk_id, op, dtlen, data);
  
  int res;
  if (http_get(http, url, htp_buf) != 0 ) {
    res = -1;
    goto finish;
  } else {
    res = 0;
  }
//  printf("data string: %s\n", http_buf_data(htp_buf));
//  printf("data size: %d\n",   http_buf_size(htp_buf));
//  printf("http rsp_code: %ld\n", http->rsp_code);
  goto finish;
  
finish:
  http_buf_free(htp_buf);
  http_free(http);
  return res;
}


int DDStat::trackCountByHTTP(int op, int trk_id, int val)
{
  char data[16] = {0};
  snprintf(data, sizeof(data), "%d", val);
  return trackByHTTP(op, trk_id, data, (int)strlen(data));
}


int DDStat::trackValueByHTTP(int op, int trk_id, int val)
{
  // the same as trackCountByHTTP
  return trackCountByHTTP(op, trk_id, val);
}



int DDStat::trackByUDP(int op, int trk_id, const char *data, int dtlen)
{
  int sockfd;
  struct sockaddr_in srv_addr; /* connector's address information */
  struct hostent *he;
  int res;
  
  if ((he = gethostbyname(host_)) == NULL) {
    return -1;
  }
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    return -1;
  }
  
  srv_addr.sin_family = AF_INET;
  srv_addr.sin_port   = htons(port_);
  srv_addr.sin_addr   = *((struct in_addr *)he->h_addr);
  bzero(&(srv_addr.sin_zero), 8);       /* zero the rest of the struct */
  
  char send_data[2048] = {0};
  snprintf(send_data, sizeof(send_data),
           "trk_id=%d&op=%d&dtlen=%d&data=%s",
           trk_id, op, dtlen, data);
  
  if (sendto(sockfd, send_data, strlen(send_data), 0,
             (struct sockaddr *)&srv_addr, sizeof(struct sockaddr)) == -1)  {
    res = -1;
  } else {
    res = 0; // success
  }
  close(sockfd);
  return res;
}


int DDStat::trackCountByUDP(int op, int trk_id, int val)
{
  char data[16] = {0};
  snprintf(data, sizeof(data), "%d", val);
  return trackByUDP(op, trk_id, data, (int)strlen(data));
}


int DDStat::trackValueByUDP(int op, int trk_id, int val)
{
  // the same as trackCountByUDP
  return trackCountByUDP(op, trk_id, val);
}





