#include <cstdlib>
#include "assert.h"
#include "image_parser.h"

ImageParser::ImageParser(string &fileName) {

  FILE *fp;
  string dir_path = "/home/asonimha/simulators/enda-sim/IMAGES_INFO/";

  string file_name = dir_path+fileName;
  if( (fp = fopen(file_name.c_str(), "r")) == NULL) {
    fprintf(stderr,"\n%s No such a file\n\n", file_name.c_str());
    
    exit(1); 

  }

  fscanf(fp, "%d %d %d", &info.nin_size[0], &info.nin_size[1], &info.nin_size[2]);
  fscanf(fp, "%d %d %d", &info.nout_size[0], &info.nout_size[1], &info.nout_size[2]);
  fscanf(fp, "%d %d %d %d", &info.s_size[0], &info.s_size[1], &info.s_size[2], &info.s_size[3]);
/*
  nout_count.resize(info.nout_size[0]); // y
  for (int i = 0; i < info.nout_size[0]; i++) {
    nout_count[i].resize(info.nout_size[1]); // x
    for (int j = 0; j < info.nout_size[1]; j++) {
      nout_count[i][j].resize(info.nout_size[2]); // z
    }
  }

  info.ptr_nout_count = &nout_count;
*/


  end_pos = new int**[info.nout_size[0]]; //y
  for (int i = 0; i < info.nout_size[0]; i++) {
    end_pos[i] = new int*[info.nout_size[1]]; // x
    for (int j = 0; j < info.nout_size[1]; j++) {
    end_pos[i][j] = new int[info.nout_size[2]]; // x
    }
  }

  for (int k = 0; k < info.nout_size[2]; k++) { // z
      for (int i = 0; i < info.nout_size[0]; i++) { // y
    for (int j = 0; j < info.nout_size[1]; j++) { // x
        fscanf(fp, "%d", &end_pos[i][j][k]);
      }
    }
  }

  info.end_pos_= end_pos;

  fclose(fp); 
/*
  printf("   y   x   z \n");
  for (int k = 0; k < info.nout_size[2]; k++) { // z
    for (int i = 0; i < info.nout_size[0]; i++) { // y
      for (int j = 0; j < info.nout_size[1]; j++) { // x
        //for (int k = 0; k < 1; k++) {
        printf("%4d%4d%4d :\t%d\n",i,j,k,end_pos[i][j][k]);
      }
      }
    }

  for (int i=0; i<3; i++) {
    cout << info.nin_size[i]<<" ";
  }
  cout<<endl;

  for (int i=0; i<3; i++) {
    cout << info.nout_size[i]<<" ";
  }
  cout<<endl;
  for (int i=0; i<4; i++) {
    cout << info.s_size[i]<<" ";
  }
  cout<<endl;

*/


}

ImageParser::~ImageParser() {

  for (int i = 0; i < info.nout_size[0]; i++) {
    for (int j = 0; j < info.nout_size[1]; j++) {
      delete [] end_pos[i][j];
    }
    delete []end_pos[i];
  }
  delete []end_pos;
}

LayerInfo* ImageParser::getPtrLayerInfo() {
  return &info;
}

