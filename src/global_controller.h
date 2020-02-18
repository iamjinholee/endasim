#ifndef __SRC_GLOBAL_CONTROLLER_H__
#define __SRC_GLOBAL_CONTROLLER_H__

#include <vector>
#include "common.h"
#include "config.h"
#include "pipe_stage.h"

using namespace std;

struct pipeEntry {
  bool valid_;
  int z_;
  int bit_pos_;

  pipeEntry() : valid_(false), z_(0), bit_pos_(0) {};
  void setData(bool valid, int z, int bit_pos) {
    valid_ = valid;
    z_ = z;
    bit_pos_ = bit_pos;
  };
  void resetEntry() {
    setData(false, 0, 0);
  }
};

class CtrlCore  {
 public:
  CtrlCore(int total_pipe_stage, int bit_size, LayerInfo* layer_info);
  ~CtrlCore();
  // controller 
  
  // structure similar to very small fully assosiative cache
  // list of z index in pipeline, bit_pos_ is maximum pipeop in pipeline
  vector<pipeEntry> pipe_entry_list;

  void insertEntry(int z, int bit_pos);
  pipeEntry* findEntry(int z);
  pipeEntry* findFirstEntry();
  bool isValidEntry();


  void printEntry();

  int bit_size_;
  int total_pipe_stage_;
  LayerInfo* layer_info_;
}; 


class GlobalCtrl {

 public:
  GlobalCtrl(SimConfigs *sim_config, NetConfigs *net_config, LayerInfo* layer_info);
  ~GlobalCtrl();
  
  void setToDoFilters(int fblock_id);  

  void initCountZ();

  void set_fblock_len(int fblock_len) {
    fblock_len_ = fblock_len; 
  }

  void cycle();
  void cycle_no_END();
  void cycle_END(); // early negative detiction

  void subProcess(PipeOp *op, int idx); 

  
  void set_end_condition_test(int num);

  void resetPipelineAll(unsigned idx);
  void setXY(int x, int y) {
    count_x_ = x;
    count_y_ = y;
  }

  bool isRunEnd();

  void calcAccess();

  SimConfigs *sim_config_; 
  NetConfigs *net_config_;
  LayerInfo *layer_info_;

  PipePort *pipe_ports_;

  SBsPipeStage *SB_pipe_stage_;
  ProviderPipeStage *provider_pipe_stage_;
  PEsPipeStage *PE_pipe_stage_;
  CtrlsPipeStage *ctrl_pipe_stage_;
 
  vector<CtrlCore> ctrl_core_; 


  int ***end_pos_;

  int end_condition_test_;

  vector<bool> run_end_flag_;

  int bit_size_; // 16 

  int count_x_;
  int count_y_;
  //int count_z_;
  vector<int> count_z_;
  

  int fblock_id_;
  int fblock_len_;

  int div_num_;
  vector< pair<int, int> > div_output_layer_z_; // pair<start, end>
 
  // for output
  uint64_t num_acc_WB_;  
  uint64_t num_acc_net_;

  uint64_t num_acc_PU_1_; 
  uint64_t num_acc_PU_2_; 
  uint64_t num_acc_PU_3_;

  uint64_t num_acc_ctrl_; 
  
};


#endif
