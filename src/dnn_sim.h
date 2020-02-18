#ifndef __DNN_SIM__
#define __DNN_SIM__

#include <math.h>
#include <iostream>
#include "common.h"
#include "config.h"
#include "dram_interface.h"
#include "global_controller.h"
#include "component.h"
#include "output_file.h"

using namespace std;

class DNNSim {

 public:
  DNNSim(SimConfigs *config, NetConfigs *net, LayerInfo* layer_info);

  ~DNNSim();
  
  void cycle();
  void cycle_conv();
  void cycle_fc1();
  void cycle_test();

  void read_complete_callback(unsigned id, uint64_t address, uint64_t clock_cycle);
  void write_complete_callback(unsigned id, uint64_t address, uint64_t clock_cycle);





  void set_net_config(NetConfigs *net) {
    net_config_ = net;
  };


  void static_simulation();

 private:

  bool isEndX();
  void moveX();


  void calcAccess();

  SimConfigs *sim_config_;
  NetConfigs *net_config_;
  LayerInfo *layer_info_;
    
  GlobalCtrl *global_ctrl_;

  dram_interface *m_dram_interface;
  DRAMSim::TransactionCompleteCB *m_read_callback;
  DRAMSim::TransactionCompleteCB *m_write_callback;

  BufferGroup *GB_;
  WeightBufferGroup *WB_;
  BufferGroup *FB_;
  PUnitArray *PU_;


  SimState sim_state_; 



  uint64_t num_MEM_GB_;
  uint64_t num_GB_WB_;
  uint64_t num_MEM_FB_;
  uint64_t num_MEM_WB_;
  uint64_t num_FB_PU_;
  uint64_t num_FB_PU_PART_;
  uint64_t num_MEM_FB_PART_;
  uint64_t num_MEM_FB_PART_TEMP_;


  uint64_t counter_MEM_GB_;
  uint64_t counter_GB_WB_;
  uint64_t counter_MEM_FB_;
  uint64_t counter_MEM_WB_;
  uint64_t counter_FB_PU_;
  uint64_t counter_FB_PU_PART_;
  uint64_t counter_MEM_FB_PART_;
  

  // for WB
  int fblock_num_; // for WB
  int fblock_len_; // for WB
  int fblock_id_;
  
  // for FB
  int num_FB_imap_y_; //  

  int omap_pos_y_len_; 
  
  // for PU
  pair<int, int> omap_pos_y_; // start, end
  bool x_dir_; // true: up, false: down
  int output_layer_x_;
  int output_layer_y_;
  int count_x_;
  int count_y_;
  
  //
  int num_mem_request_;
  int num_mem_callback_;
  
  int cycle_load_PU_;
  int cycle_load_PU_part_;
  int count_cycle_load_PU_;
  int count_cycle_load_PU_part_;


  // Stats
  unsigned long long m_sim_cycle;
    
  int test_n;

  OutputFile *output_;
  
};

#endif 
