#ifndef __SRC_OUTPUT_FILE_H__
#define __SRC_OUTPUT_FILE_H__

#include "config.h"
#include "stdint.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class OutputFile {
 public:
  OutputFile(SimConfigs *config, NetConfigs *net);
  ~OutputFile() {}
  
  void resizeComponentCycle(int size){
    component_cycle.resize(size, 0);
  };

  void makeOutputFile(); 
  void calcEnergy();

  SimConfigs *config_;
  NetConfigs *net_;

  string output_name_;

  // for output
  vector<uint64_t> component_cycle;
  uint64_t total_cycle;
  
  uint64_t num_acc_MEM_;
  uint64_t num_rd_GB_;
  uint64_t num_wr_GB_;
  uint64_t num_acc_WB_;
  uint64_t num_acc_FB_;
  uint64_t num_acc_PU_;
  uint64_t num_acc_ctrl_;
  uint64_t num_acc_net_;



  double dyn_eng_acc_GB_;
  double dyn_eng_acc_FB_;
  double dyn_eng_acc_WB_;
  double dyn_eng_acc_PU_;
  double dyn_eng_acc_CTRL_;
  double dyn_eng_acc_NET_;

  double static_eng_acc_GB_;
  double static_eng_acc_FB_;
  double static_eng_acc_WB_;
  double static_eng_acc_PU_;
  double static_eng_acc_CTRL_;
  double static_eng_acc_NET_;




};




#endif
