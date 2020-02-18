#ifndef __SRC_COMMON_H__
#define __SRC_COMMON_H__
#include "math.h"
#include <vector>
#include <string>
#include <iostream>

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
  MEM_WB, // move data from MEM to WB (for fully-connected case)
  EOP
};

enum ComponetID {
  MEM = 0,
  GB,
  FB,
  WB,
  PU
};


#define NUM_PIPELINE_STAGE 4

class LayerInfo {
 public:
  int nin_size[3]; // input neuron layer size (y, x, z)
  int nout_size[3]; // output neuron layer size (y, x, z)
  int s_size[4]; // 3D synapse size and number 
  int ***end_pos_; 
};

#define WEIGHT_BASE_ADDRESS 0x100000
#define INPUT_FEATURE_BASE_ADDRESS  0x200000
#define OUTPUT_FEATURE_BASE_ADDRESS  0x300000


#endif
