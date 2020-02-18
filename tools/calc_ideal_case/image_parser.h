#ifndef __SRC_IMAGE_PARSER_H__
#define __SRC_IMAGE_PARSER_H__

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

class LayerInfo {
 public:
  int nin_size[3]; // input neuron layer size (y, x, z)
  int nout_size[3]; // output neuron layer size (y, x, z)
  int s_size[4]; // 3D synapse size and number 
  //Matrix3D *ptr_nout_count;
  int ***end_pos_; 
};


class ImageParser {
 public:
  ImageParser(string &fileName);
  ~ImageParser();
 
  LayerInfo* getPtrLayerInfo();

  LayerInfo info;
  //Matrix3D nout_count;
  int ***end_pos;
};



#endif
