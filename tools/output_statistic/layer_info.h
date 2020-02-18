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
  MEM_WB,
  EOP
};



enum EnergyState {
  D_GB = 0,
  D_FB = 1,
  D_WB = 2,
  D_PU = 3,
  D_NET = 4,
  D_CTRL = 5,
  S_GB = 6,
  S_FB,
  S_WB,
  S_PU,
  S_NET,
  S_CTRL,
  E_EOP
};

// PU, WB, FB, GB, NET, CTRL, 
//static int eng_reorder[] = {3, 2, 1, 0, 4, 5, 9, 8, 7, 6, 10, 11, 12  }; 
// CTRL, NET, GB, FB, WB, PU
static int eng_reorder[] = {5, 4, 0, 1 ,2 ,3, 11, 10, 6, 7, 8, 9, 12  }; 


class LayerInfo {
 public:
  LayerInfo();
  ~LayerInfo() {}
  
 // perf 
  vector<double> component_cycle_ratio;
  vector<uint64_t> component_cycle;
  uint64_t cycle_sum;
  double ratio_sum;

  double energy_sum;
  double energy_ratio_sum;
  
  double power_sum;

// eng
  vector<double> energy;
  vector<double> energy_ratio;
};



#endif
