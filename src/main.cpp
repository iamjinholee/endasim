#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <cassert>
#include <string>
#include <vector>
#include "config.h"
#include "option_parser.h"
#include "dnn_sim.h"
#include "image_parser.h"

using namespace std;

DNNSim *dnn_sim = NULL;
SimConfigs *sim_config = NULL;
NetConfigs *net_config = NULL;
LayerInfo *layer_info= NULL;

void *main_sim_loop(void *args);


int main(int argc, char **argv) {


  pthread_t sim_thread;
  
  sim_config = new SimConfigs();
  net_config = new NetConfigs();

  // Parse configuration file borrowed from GPGPU-Sim 2015 (www.gpgpu-sim.org)
  option_parser_t opp = option_parser_create();
  sim_config->reg_options(opp);
  //net_config->reg_options(opp);

  option_parser_cmdline(opp, argc, argv);

  //string image_name =  "image_00000001-layer_16.out";
  string image_name = getImageName(opp);
  string output_name = getOutputName(opp);
  sim_config->outfile_name = output_name;

  cout <<"main " << output_name<< endl;
  ImageParser image_parser(image_name);
  layer_info = image_parser.getPtrLayerInfo();

  net_config->setLayerInfo(layer_info);
  net_config->image_name = image_name;

  if(sim_config->END_enable==1) {
    cout<<"END"<<endl;
  }
  else {

    cout<<"no END"<<endl;
  }

  if ((sim_config->row_PU_num * sim_config->PU_input_num ) >= net_config->input_layer_z) {
    sim_config->div_num = (sim_config->row_PU_num * sim_config->PU_input_num ) / net_config->input_layer_z  ; // Number of filters processed at a time 
  }
  else {
    sim_config->div_num = 1;
  }

  sim_config->init_params_else();


  option_parser_print(opp, stdout);
  sim_config->print_else_params();

  dnn_sim = new DNNSim(sim_config, net_config, layer_info);

  std::cout << "Launching main simulation thread" << std::endl;
  if(pthread_create(&sim_thread, NULL, main_sim_loop, NULL)){
    std::cout << "Error creating main simulation thread. Aborting..." << std::endl;
    abort();
  }
  void *pthread_ret;
  pthread_join(sim_thread, &pthread_ret);

  ////
  
  

  
  std::cout << "delete" << std::endl;
  delete dnn_sim;
  delete sim_config;
  delete net_config;

  std::cout << "Finished" << std::endl;
  // for simulator, load 3x3 filters filled with counter value.  

  //// load activation data to input registers in PEs (assuming static DRAM latency)
  // dram_latency  * (size of total SRAM)/min_block
  //// load data to SB
  //
  
  /// excution of pipelined adder with filter data in SB
  // pipeline 1, 2, 3 
  

  // after finishing execution of adder, 
  /// load new activation data

  return 0;
}


void test_dnn_sim() {
   // for(unsigned i=0; i<600000; ++i){
   while(true) {
        dnn_sim->cycle();
    }
   //dnn_sim->cycle_test();
   //while(true) {
   // dnn_sim->cycle();
   //}
}


void *main_sim_loop(void *args) {
    std::cout << "In main simulation loop" << std::endl;
    
    assert(dnn_sim);

    test_dnn_sim();   
    
}




