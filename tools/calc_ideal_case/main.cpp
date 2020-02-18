#include "stdio.h"
#include <sstream>
#include <string>
#include <iomanip>
#include <iostream>
#include <vector>
#include "image_parser.h"
using namespace std;

int main() {
  LayerInfo *layer_info = NULL;
  
  vector<long long> accum_data(16);
  vector<long long> orig_data(16, 0);

  vector<int> layer_id = {2, 4, 7, 9, 12, 14, 16, 19, 21, 23, 26, 28, 30, 33, 35, 37};


  for (int i = 0; i<1000; ++i) { 
    for(int ll = 0; ll < layer_id.size(); ++ll) {
      ostringstream oss;
      oss<< "image_" << setw(8) << setfill('0') << i+1;

      string image_name = oss.str() + "-layer_"+to_string(layer_id.at(ll))+".out";
      cout << image_name<<endl;

      //string image_name =  "image_00000001-layer_21.out";
      ImageParser image_parser(image_name);
      layer_info = image_parser.getPtrLayerInfo();

      int o_x = layer_info->nout_size[1];
      int o_y = layer_info->nout_size[0];
      int o_z = layer_info->nout_size[2];
      
      orig_data.at(ll) += o_x * o_y * o_z * 16;

      for (int kk = 0; kk < o_z; ++kk) {
        for (int jj = 0; jj < o_y; ++jj) {
          for (int ii = 0; ii < o_x; ++ii) {
            accum_data.at(ll) += layer_info->end_pos_[ii][jj][kk];
          } // end ii
        } // end jj
      } // end kk
    } // end ll
  } // end i

  // print
 
  cout << "orig ";
  for (int i = 0; i<orig_data.size(); ++i) {
    printf("%lld ", orig_data.at(i) );
  }
  cout << endl; 

  cout << "saving ";
  for (int i = 0; i<accum_data.size(); ++i) {
    printf("%lld ", accum_data.at(i) );
  }
  cout << endl;


  FILE *fp = fopen("output.txt", "w");

  fprintf(fp, "orig ");
  for (int i = 0; i<orig_data.size(); ++i) {
    fprintf(fp, "%lld ", orig_data.at(i) );
  }
  fprintf(fp, "\n");

  fprintf(fp, "saving ");
  for (int i = 0; i<accum_data.size(); ++i) {
    fprintf(fp, "%lld ", accum_data.at(i) );
  }
  fprintf(fp, "\n");

  fclose(fp);





  return 0;
}
