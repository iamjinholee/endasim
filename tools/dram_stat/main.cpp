#include "assert.h"
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cmath>
#include "layer_info.h"


using namespace std;

int main() {
  
  string out_dir = "../../DRAMSIM_OUTPUT/";

  int layers[] = {4, 7, 9, 12, 14, 16, 19, 21, 23, 26, 28, 30};
  int l_size = sizeof(layers)/sizeof(int);

  int num_image = 1;

  vector<vector<LayerInfo > > image_info_end;
  vector<vector<LayerInfo > > image_info_no_end;
  
  image_info_end.resize(num_image, vector<LayerInfo>(12, LayerInfo()));
  image_info_no_end.resize(num_image, vector<LayerInfo>(12, LayerInfo()));


  ifstream fin;
  string file_name;

  string line_temp;
  for (int i = 0; i < num_image; ++i) {
    for(int l = 0; l < l_size; ++l) {
      file_name = out_dir+"out_end_"+to_string(i+1)+"_"+to_string(layers[l])+".log";
      fin.open(file_name);
      
      if (!fin.is_open()){
        fprintf(stderr,"\n%s No such a file\n\n", file_name.c_str());
        exit(1); 
      }
  
      while (getline(fin,line_temp)) {
        ssize_t pos;
       if( (pos = line_temp.find("Average Power")) != string::npos) {
         char * cstr = new char [line_temp.length()+1];
         strcpy(cstr, line_temp.c_str());
         char * p = strtok(cstr, ":");
          
          double power = stof(line_temp.substr(31));
          image_info_end[i][l].power_sum_ += power;
          ++image_info_end[i][l].count_;
         delete cstr;
       };

        if (fin.eof()) {
          break;
        }
      }
        
      fin.close();
    }
  }

  for (int i = 0; i < num_image; ++i) {
    for(int l = 0; l < l_size; ++l) {
      file_name = out_dir+"out_no_end_"+to_string(i+1)+"_"+to_string(layers[l])+".log";
      fin.open(file_name);
      
      if (!fin.is_open()){
        fprintf(stderr,"\n%s No such a file\n\n", file_name.c_str());
        exit(1); 
      }
  
      while (getline(fin,line_temp)) {
        ssize_t pos;
       if( (pos = line_temp.find("Average Power")) != string::npos) {
         char * cstr = new char [line_temp.length()+1];
         strcpy(cstr, line_temp.c_str());
         char * p = strtok(cstr, ":");
          
          double power = stof(line_temp.substr(31));
          image_info_no_end[i][l].power_sum_ += power;
          ++image_info_no_end[i][l].count_;
         delete cstr;
       };

        if (fin.eof()) {
          break;
        }
      }
        
      fin.close();
    }
  }


/*
  for (int i = 0; i < num_image; ++i) {
    for(int l = 0; l < l_size; ++l) {
      double psum_W = image_info_end[i][l].power_sum_;
      long count = image_info_end[i][l].count_;
      
      double psum_J = psum_W * count * pow(0.1, 4);

      double W = psum_W / count;

      printf("%d %d : %lf %ld %f %f\n" ,i+1,layers[l], psum_W, count, psum_J, W);
    }
  }
*/

  for (int i = 0; i < num_image; ++i) {
    for(int l = 0; l < l_size; ++l) {
      double psum_W = image_info_no_end[i][l].power_sum_;
      long count = image_info_no_end[i][l].count_;
      
      double psum_J = psum_W * count * pow(0.1, 4);

      double W = psum_W / count;

      printf("%d %d : %lf %ld %f %f\n" ,i+1,l+1, psum_W, count, psum_J, W);
    }
  }


  return 0;
}
