//
//  dd_stat.h
//
//  Created by Zhibiao Pan on 2012-05-31
//  Copyright 2012 DangDang Inc. All rights reserved.
//

#ifndef PHP_DD_STAT_H
#define PHP_DD_STAT_H

class DDStat
{
private:
  char host_[64];
  int  port_;

public:
  DDStat();
  ~DDStat();
  
  int initTrackServer(const char *host, int port);

  int trackByUDP (int op, int trk_id, const char *data, int dtlen);
  int trackByHTTP(int op, int trk_id, const char *data, int dtlen);
  
  int trackCountByUDP(int op, int trk_id, int val);
  int trackValueByUDP(int op, int trk_id, int val);
  
  int trackCountByHTTP(int op, int trk_id, int val);
  int trackValueByHTTP(int op, int trk_id, int val);
};

#endif
