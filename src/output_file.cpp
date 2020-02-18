#include "assert.h"
#include <cmath>
#include "output_file.h"


OutputFile::OutputFile(SimConfigs *config, NetConfigs *net) {
  
  config_ = config;
  net_ = net;

  output_name_ = config_->outfile_name;
  total_cycle = 0;
 
  num_acc_MEM_ = 0;
  num_rd_GB_ = 0;
  num_wr_GB_ = 0;
  num_acc_WB_ = 0;
  num_acc_FB_ = 0;
  num_acc_PU_ = 0;
  num_acc_ctrl_ = 0;
  num_acc_net_ = 0;

}


void OutputFile::makeOutputFile(){
  
  FILE *fp;

  string dir_path = "./OUTPUT/";
  string file_name = dir_path+output_name_;
  cout << file_name<<endl;
  
  if ( (fp = fopen(file_name.c_str(), "w")) == NULL) {
    fprintf(stderr,"\n%s Can't create a file\n\n", file_name.c_str());
    exit(1);
  }

 
          
  for (int i = 0; i < component_cycle.size(); ++i) {
    fprintf(fp, "%ld ", component_cycle.at(i));
  }
  fprintf(fp, "\n");


  //
  /*
  fprintf(fp, "%lf %lf ", dyn_eng_acc_GB_, static_eng_acc_GB_);
  fprintf(fp, "%lf %lf ", dyn_eng_acc_FB_, static_eng_acc_FB_);
  fprintf(fp, "%lf %lf ", dyn_eng_acc_WB_, static_eng_acc_WB_);
  fprintf(fp, "%lf %lf ", dyn_eng_acc_PU_, static_eng_acc_PU_);
  fprintf(fp, "%lf %lf ", dyn_eng_acc_NET_, static_eng_acc_NET_);
  fprintf(fp, "%lf %lf ", dyn_eng_acc_CTRL_, static_eng_acc_CTRL_);
  fprintf(fp, "\n");
  */
  fprintf(fp, "%lf %lf %lf %lf %lf %lf ", dyn_eng_acc_GB_, dyn_eng_acc_FB_, dyn_eng_acc_WB_, dyn_eng_acc_PU_, dyn_eng_acc_NET_, dyn_eng_acc_CTRL_);
  fprintf(fp, "%lf %lf %lf %lf %lf %lf ", static_eng_acc_GB_, static_eng_acc_FB_, static_eng_acc_WB_, static_eng_acc_PU_, static_eng_acc_NET_, static_eng_acc_CTRL_);
  fprintf(fp, "\n");

  fprintf(fp, "%lu %lu %lu %lu %lu %lu %lu ", num_rd_GB_, num_wr_GB_, num_acc_FB_ , num_acc_WB_, num_acc_PU_, num_acc_ctrl_, num_acc_net_);
  fprintf(fp, "\n");
  fprintf(fp, "%lu \n", total_cycle);


  fclose(fp);

}



void OutputFile::calcEnergy() {
  // energy
  double dyn_eng_rd_GB_ = num_rd_GB_ * config_->GB_dynamic_energy; 
  double dyn_eng_wr_GB_ = num_wr_GB_ * config_->GB_dynamic_write_energy; 
  
  double unit = pow(10,6);
  dyn_eng_acc_GB_ = (dyn_eng_rd_GB_ + dyn_eng_wr_GB_) / unit;
  dyn_eng_acc_FB_ = num_acc_FB_ * config_->FB_dynamic_energy / unit; // nJ -> mJ
  dyn_eng_acc_WB_ = num_acc_WB_ * config_->WB_dynamic_energy / unit; // nJ -> mJ
  dyn_eng_acc_PU_ = num_acc_PU_ * config_->PU_dynamic_energy / unit; // nJ -> mJ
  dyn_eng_acc_CTRL_ = num_acc_ctrl_ * config_->CTRL_dynamic_energy / unit; // nJ -> mJ
  dyn_eng_acc_NET_ = num_acc_net_ * config_->NET_dynamic_energy / unit; // nJ -> mJ
  
  double runtime = total_cycle / (config_->frequency * pow(10,6) ); // sec
  static_eng_acc_GB_ = (double) config_->GB_static_power * config_->GB_num * runtime; // mJ
  static_eng_acc_FB_ = (double) config_->FB_static_power * config_->FB_num * runtime; // mJ
  static_eng_acc_WB_ = (double) config_->WB_static_power * config_->WB_num * runtime; // mJ

  int total_PU_num = config_->row_PU_num * config_->row_num + config_->div_num  + 1;
  static_eng_acc_PU_ =(double) config_->PU_static_power * total_PU_num * runtime; // mJ

  int total_CTRL_num =  config_->div_num;
  static_eng_acc_CTRL_ = (double) config_->CTRL_static_power * total_CTRL_num * runtime; // mJ

  static_eng_acc_NET_ = (double) config_->NET_static_power *  runtime; // mJ
  
}
