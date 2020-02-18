#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "option_parser.h"
#include "common.h"

class SimConfigs {
 public:
  SimConfigs(){};
  ~SimConfigs(){};

  void reg_options(option_parser_t opp);
  
  void init_params_else();
  void print_else_params();
  
  void static_simulation();



  unsigned END_enable;

  // for Processing Element (PU)
  unsigned PU_input_num;
  float PU_area; // mm2
  unsigned PU_access_cycle;
  float PU_static_power ; // mW 
  float PU_dynamic_energy; // nJ 
 

  // for Global Buffer (GB)
  unsigned GB_type; // 1: SRAM, 2: STT-RAM 
  unsigned GB_capacity; // kB
  unsigned GB_access_cycle;
  float GB_static_power ; // mW 
  float GB_dynamic_energy; // nJ 
  float GB_area; // mm2
  unsigned GB_bank_num;
  unsigned GB_num; // 
  // for STT-RAM GB
  unsigned GB_write_cycle;
  float GB_dynamic_write_energy; // nJ 


  // for Weight Buffer (WB)
  unsigned WB_capacity; // kB
  unsigned WB_access_cycle;
  float WB_static_power ; // mW 
  float WB_dynamic_energy; // nJ 
  float WB_area; // mm2
  unsigned WB_bank_num; // 
  unsigned WB_num; // 


  // for Input Featue map Buffer (FB)
  unsigned FB_capacity; // kB
  unsigned FB_access_cycle;
  float FB_static_power ; // mW 
  float FB_dynamic_energy; // nJ 
  float FB_area; // mm2
  unsigned FB_bank_num; // 
  unsigned FB_num; // 

  // for Provier network
  float NET_static_power ; // mW 
  float NET_dynamic_energy; // nJ 
  float NET_area; // mm2

  // for Global controller 
  float CTRL_static_power ; // mW 
  float CTRL_dynamic_energy; // nJ 
  float CTRL_area; // mm2

  // for architecture
  unsigned row_PU_num;
  unsigned row_num;
  float frequency;

  // for DRAM
  unsigned DRAM_access_cycle;



  //// param else
  unsigned div_num;
  
  uint64_t total_WB_capacity; // kB
  uint64_t total_FB_capacity; // kB
  
  uint64_t total_PU_capacity; // B

  // power

  string outfile_name;
};

class NetConfigs {
 public:
  NetConfigs(){};
  ~NetConfigs(){};


  void setLayerInfo(LayerInfo *layer_info);
  void reg_options(option_parser_t opp);
  
 
  // input activation layer
  unsigned input_layer_x;
  unsigned input_layer_y;
  unsigned input_layer_z;

  // output activation layer
  unsigned output_layer_x;
  unsigned output_layer_y;
  unsigned output_layer_z;


  // 3D filters
  unsigned filter_size_x;
  unsigned filter_size_y;
  unsigned filter_size_z;
  unsigned filter_num;

  // convolution
  unsigned stride;

  string image_name; 


  // else
  uint64_t filter_size;
  uint64_t total_filter_capacity; 
  uint64_t total_imap_capacity; 

};

#endif
