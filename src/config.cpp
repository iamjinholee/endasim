#include <iostream>
#include <stdio.h>
#include "config.h"
using namespace std;
void SimConfigs::reg_options(option_parser_t opp) {

  option_parser_register(opp, "-END_enable", OPT_INT32, &END_enable,
                         "Early negative detection, enable = 1, disable = 0 ",
                         "0");

  // for Processing Element (PU)
  option_parser_register(opp, "-PU_input_num", OPT_INT32, &PU_input_num,
                         "Number of inputs in a PU ",
                         "32");

  option_parser_register(opp, "-PU_access_cycle", OPT_INT32, &PU_access_cycle,
                         "Cycle of accessing a PU ",
                         "1");

  option_parser_register(opp, "-PU_static_power", OPT_FLOAT, &PU_static_power,
                         "Static power consumption of a PU (mW)",
                         "1");

  option_parser_register(opp, "-PU_dynamic_energy", OPT_FLOAT, &PU_dynamic_energy,
                         "Dynamic energy consumption of a PU (nJ)",
                         "1");

  option_parser_register(opp, "-PU_area", OPT_FLOAT, &PU_area,
                         "Area of a PU (mm2)",
                         "1");

  // for Global Buffer (GB)
  option_parser_register(opp, "-GB_type", OPT_INT32, &GB_type,
                         "Type of a Global Buffer (1: SRAM, 2:STT-RAM) ",
                         "1");

  option_parser_register(opp, "-GB_capacity", OPT_INT32, &GB_capacity,
                         "Capacity of a Global Buffer (kB) ",
                         "1024");

  option_parser_register(opp, "-GB_access_cycle", OPT_INT32, &GB_access_cycle,
                         "Cycle of accessing a Global Buffer",
                         "1");

  option_parser_register(opp, "-GB_static_power", OPT_FLOAT, &GB_static_power,
                         "Static power consumption of a Global Buffer (mW)",
                         "1");

  option_parser_register(opp, "-GB_dynamic_energy", OPT_FLOAT, &GB_dynamic_energy,
                         "Dynamic energy consumption of accessing a Global Buffer (nJ)",
                         "1");


  option_parser_register(opp, "-GB_area", OPT_FLOAT, &GB_area,
                         "Area of a Global Buffer (mm2)",
                         "1");

  option_parser_register(opp, "-GB_bank_num", OPT_INT32, &GB_bank_num,
                         "Number of banks in a GB ",
                         "1");

  option_parser_register(opp, "-GB_num", OPT_INT32, &GB_num,
                         "Number of GBs ",
                         "5");

  option_parser_register(opp, "-GB_write_cycle", OPT_INT32, &GB_write_cycle,
                         "Cycle of writing a Global Buffer",
                         "1");

  option_parser_register(opp, "-GB_dynamic_write_energy", OPT_FLOAT, &GB_dynamic_write_energy,
                         "Dynamic energy consumption of writing a Global Buffer (nJ)",
                         "1");


  // for Weight Buffer (WB)
  option_parser_register(opp, "-WB_capacity", OPT_INT32, &WB_capacity,
                         "Capacity of a Weight Buffer (kB) ",
                         "32");

  option_parser_register(opp, "-WB_access_cycle", OPT_INT32, &WB_access_cycle,
                         "Cycle of accessing a Weight Buffer",
                         "1");

  option_parser_register(opp, "-WB_static_power", OPT_FLOAT, &WB_static_power,
                         "Static power consumption of a Weight Buffer (mW)",
                         "1");

  option_parser_register(opp, "-WB_dynamic_energy", OPT_FLOAT, &WB_dynamic_energy,
                         "Dynamic energy consumption of accessing a Weight Buffer (nJ)",
                         "1");

  option_parser_register(opp, "-WB_area", OPT_FLOAT, &WB_area,
                         "Area of a Weight Buffer (mm2)",
                         "1");

  option_parser_register(opp, "-WB_bank_num", OPT_INT32, &WB_bank_num,
                         "Number of banks in a WB ",
                         "1");

  option_parser_register(opp, "-WB_num", OPT_INT32, &WB_num,
                         "Number of Weight Buffers ",
                         "9");

  // for Input Feature Map Buffer (FB)
  option_parser_register(opp, "-FB_capacity", OPT_INT32, &FB_capacity,
                         "Capacity of a Feature Buffer (kB) ",
                         "256");

  option_parser_register(opp, "-FB_access_cycle", OPT_INT32, &FB_access_cycle,
                         "Cycle of accessing a Feature Buffer",
                         "1");

  option_parser_register(opp, "-FB_static_power", OPT_FLOAT, &FB_static_power,
                         "Static power consumption of a Feature Buffer (mW)",
                         "1");

  option_parser_register(opp, "-FB_dynamic_energy", OPT_FLOAT, &FB_dynamic_energy,
                         "Dynamic energy consumption of accessing a Feature Buffer (nJ)",
                         "1");

  option_parser_register(opp, "-FB_area", OPT_FLOAT, &FB_area,
                         "Area of a Feature Buffer (mm2)",
                         "1");

  option_parser_register(opp, "-FB_bank_num", OPT_INT32, &FB_bank_num,
                         "Number of banks in a FB ",
                         "1");

  option_parser_register(opp, "-FB_num", OPT_INT32, &FB_num,
                         "Number of Feature Buffers ",
                         "1");


  // for Provider network
  option_parser_register(opp, "-NET_static_power", OPT_FLOAT, &NET_static_power,
                         "Static power consumption of a Provider Network (mW)",
                         "1");

  option_parser_register(opp, "-NET_dynamic_energy", OPT_FLOAT, &NET_dynamic_energy,
                         "Dynamic energy consumption of accessing a Provider Network (nJ)",
                         "1");

  option_parser_register(opp, "-NET_area", OPT_FLOAT, &NET_area,
                         "Area of a Providing Network (mm2)",
                         "1");


  // for Provider network
  option_parser_register(opp, "-CTRL_static_power", OPT_FLOAT, &CTRL_static_power,
                         "Static power consumption of a Global Controller (mW)",
                         "1");

  option_parser_register(opp, "-CTRL_dynamic_energy", OPT_FLOAT, &CTRL_dynamic_energy,
                         "Dynamic energy consumption of accessing a Global Controller (nJ)",
                         "1");

  option_parser_register(opp, "-CTRL_area", OPT_FLOAT, &CTRL_area,
                         "Area of a Global Controller (mm2)",
                         "1");




  // for Architecture 
  option_parser_register(opp, "-row_PU_num", OPT_INT32, &row_PU_num,
                         "Number of PUs in a row ",
                         "16");

  option_parser_register(opp, "-row_num", OPT_INT32, &row_num,
                         "Number of rows in a architecture ",
                         "9");

  option_parser_register(opp, "-frequency", OPT_FLOAT, &frequency,
                         "operating frequency of a architecture ( Mhz)",
                         "1000");


  // for DRAM
  option_parser_register(opp, "-DRAM_access_cycle", OPT_INT32, &DRAM_access_cycle,
                         "Cycle of accessing DRAM ",
                         "5");

}


void SimConfigs::init_params_else() {

  total_WB_capacity = WB_capacity * WB_num;
  total_FB_capacity = FB_capacity * FB_num;

  total_PU_capacity = PU_input_num * row_PU_num * row_num * 2 ;

/*
  // area
  total_PU_leaf_num = row_PU_num * row_num;
  total_PU_root_num = row_PU_num;
  total_PU_top_num = 1;

  total_PU_num = total_PU_leaf_num + total_PU_root_num + total_PU_top_num;
  total_PU_area = (PU_area * total_PU_num);

  total_SB_capacity = SB_capacity * SB_num;
  total_NB_capacity = NB_capacity * NB_num;

  total_SB_area = (SB_area * SB_num);
  total_NB_area = (NB_area * NB_num);



  total_arch_area = (total_PU_area + total_SB_area+ total_NB_area);

  // power
  // dynamic power
  
  total_PU_dynamic = total_PU_num * PU_dynamic_power; // mW
  total_PU_static = total_PU_num * PU_static_power; // mW
  
  total_SB_dynamic = SB_num *SB_dynamic_energy*(frequency/1000) * 1000; // mW 
  total_SB_static = SB_num * SB_static_power; // mW
  
  // NB_dynamic should be calculated
  //total_NB_dynamic = NB_num *NB_dynamic_energy*(frequency/1000) / 1000; // mW 
  //total_NB_dynamic = NB_num *NB_dynamic_energy*(frequency/1000) * 1000 *(600/1000); // mW 
  total_NB_dynamic = NB_num *NB_dynamic_energy*(frequency/1000) * 1000;

  total_NB_static = SB_num * NB_static_power; // mW
  
  total_dynamic = total_PU_dynamic + total_SB_dynamic + total_NB_dynamic;
  total_static = total_PU_static + total_SB_static + total_NB_static;
  total_power = total_dynamic + total_static;

*/
}


void static_simulation() {

}


void SimConfigs::print_else_params() {
  /*
  printf("div_num\t %d \n", div_num);
  cout<< endl;
  printf("-total_PU_num\t\t\t%d\n", total_PU_num);
  printf("-total_SB_capacity\t\t%d kB\n", total_SB_capacity);
  printf("-total_PU_area\t\t\t%.4f mm2\n", total_PU_area);
  printf("-total_SB_area\t\t\t%.4f mm2\n", total_SB_area);
  printf("-total_arch_area\t\t%.4f mm2\n", total_arch_area);
  printf("total_SB_capacity\t\t%d kB\n", total_SB_capacity);
  printf("total_NB_capacity\t\t%d kB\n", total_NB_capacity);
  cout<< endl;
  printf("total_PU_area\t %.4f mm2\n", total_PU_area);
  printf("total_SB_area\t %.4f mm2\n", total_SB_area);
  printf("total_NB_area\t %.4f mm2\n", total_NB_area);
  printf("total_arch_area\t %.4f mm2\n", total_arch_area);
  cout<< endl;
  printf("total_PU_dynamic\t %.4f mW\n", total_PU_dynamic);
  printf("total_SB_dynamic\t %.4f mW\n", total_SB_dynamic);
  printf("total_NB_dynamic\t %.4f mW\n", total_NB_dynamic);
  printf("total_dynamic\t %.4f mW\n", total_dynamic);
  cout<< endl;
  printf("total_PU_static\t %.4f mW\n", total_PU_static);
  printf("total_SB_static\t %.4f mW\n", total_SB_static);
  printf("total_NB_static\t %.4f mW\n", total_NB_static);
  printf("total_static\t %.4f mW\n", total_static);
  cout<< endl;
  */
}


void NetConfigs::setLayerInfo(LayerInfo * layer_info) {

  input_layer_x = layer_info->nin_size[1];
  input_layer_y = layer_info->nin_size[0];
  input_layer_z = layer_info->nin_size[2];

  output_layer_x = layer_info->nout_size[1];
  output_layer_y = layer_info->nout_size[0];
  output_layer_z = layer_info->nout_size[2];

  filter_size_x = layer_info->s_size[1];
  filter_size_y = layer_info->s_size[0];
  filter_size_z = layer_info->s_size[2];
  filter_num = layer_info->s_size[3];

  
  filter_size = filter_size_x * filter_size_y * filter_size_z;
  total_filter_capacity = filter_size * filter_num * 2;
  total_imap_capacity = input_layer_x * input_layer_y * input_layer_z * 2;
 
}


void NetConfigs::reg_options(option_parser_t opp) {

  option_parser_register(opp, "-input_layer_x", OPT_INT32, &input_layer_x,
                         "x size of input activation layer",
                         "28");

  option_parser_register(opp, "-input_layer_y", OPT_INT32, &input_layer_y,
                         "y size of input activation layer",
                         "28");

  option_parser_register(opp, "-input_layer_z", OPT_INT32, &input_layer_z,
                         "z size of input activation layer",
                         "512");

  option_parser_register(opp, "-output_layer_x", OPT_INT32, &output_layer_x,
                         "x size of output activation layer",
                         "28");

  option_parser_register(opp, "-output_layer_y", OPT_INT32, &output_layer_y,
                         "y size of output activation layer",
                         "28");

  option_parser_register(opp, "-output_layer_z", OPT_INT32, &output_layer_z,
                         "z size of output activation layer",
                         "512");

  option_parser_register(opp, "-filter_size_x", OPT_INT32, &filter_size_x,
                         "x size of 3d filter",
                         "3");

  option_parser_register(opp, "-filter_size_y", OPT_INT32, &filter_size_y,
                         "y size of 3d filter",
                         "3");

  option_parser_register(opp, "-filter_size_z", OPT_INT32, &filter_size_z,
                         "z size of 3d filter",
                         "512");

  option_parser_register(opp, "-stride", OPT_INT32, &stride,
                         "stride size for convolution operation",
                         "1");

} 
