#ifndef __ETCS_LAYER_INFO_H__
#define __ETCS_LAYER_INFO_H__

#include "stdint.h"
#include <vector>

using namespace std;

enum SimState {
  INIT = 0,
  MEM_GB, // move data from MEM to GB
  GB_WB, // move data from GB to WB (weight buffer)
  MEM_FB, // move data from MEM to FB (input feature map buffer)
  FB_PU, // move data from FB to PU 
  FB_PU_PART,
  GB_WB_PART,
  MEM_FB_PART,
  RUN_PU,
  EOP
};


class LayerInfo {
 public:
  LayerInfo();
  ~LayerInfo() {}

  long count_;
  double power_sum_;   
};

#endif
