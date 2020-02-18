#ifndef __SRC_IMAGE_PARSER_H__
#define __SRC_IMAGE_PARSER_H__

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include "common.h"

using namespace std;

class ImageParser {
 public:
  ImageParser(string &fileName);
  ~ImageParser();
 
  LayerInfo* getPtrLayerInfo();

  LayerInfo info;
  int ***end_pos;
};



#endif
