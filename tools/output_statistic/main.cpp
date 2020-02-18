#include "assert.h"
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "layer_info.h"


using namespace std;


//int layers[] = {4, 7, 9, 12, 14, 16, 19, 21, 23, 26, 28, 30, 35, 37 };
int layers[] = {2};
int l_size = sizeof(layers)/sizeof(int);
//int num_image = 1000;
int num_image = 1;
//string out_dir = "../../OUTPUT_20171229/";
string out_dir = "../../OUTPUT/";

vector<LayerInfo > image_info_end;
vector<LayerInfo > image_info_no_end;


void init_no_end() {
  image_info_no_end.resize(l_size, LayerInfo());

  FILE *fp;
  string file_name;

  // for no_end 
  for(int l = 0; l < l_size; ++l) {
    file_name = out_dir+"out_no_end_1_"+to_string(layers[l]);

    // perf
    if( (fp = fopen(file_name.c_str(), "r")) == NULL) {
      fprintf(stderr,"\n%s No such a file\n\n", file_name.c_str());
      exit(1); 
    }
    for (int ii = 0; ii < EOP; ++ii) {
      fscanf(fp, "%ld", &(image_info_no_end.at(l).component_cycle.at(ii)));
    } 

    // energy 
    for (int ii = 0; ii < E_EOP; ++ii) {
      double temp;
      fscanf(fp, "%lf",&temp );


      if(ii==S_GB && (layers[l]==35 || layers[l] ==37)){
        image_info_no_end.at(l).energy.at(ii) = (double)0;
      }
      else {
        image_info_no_end.at(l).energy.at(ii) = temp;
      }
    }



    fclose(fp);

  }


  // no_end sum
  for(int l = 0; l < l_size; ++l) {
    for (int ii = 0; ii < EOP; ++ii) {
      image_info_no_end.at(l).cycle_sum += 
          image_info_no_end.at(l).component_cycle.at(ii);

      // energy
      image_info_no_end.at(l).energy_sum += 
          image_info_no_end.at(l).energy.at(ii);
    }
    image_info_no_end.at(l).ratio_sum = 100;
    // energy
    image_info_no_end.at(l).energy_ratio_sum = 100;

  }

  // no_end cycle ratio
  for(int l = 0; l < l_size; ++l) {
    for (int ii = 0; ii < EOP; ++ii) {
      image_info_no_end.at(l).component_cycle_ratio.at(ii) =
          (double)image_info_no_end.at(l).component_cycle.at(ii)/
          image_info_no_end.at(l).cycle_sum *100;

      //energy
      image_info_no_end.at(l).energy_ratio.at(ii) =
          (double)image_info_no_end.at(l).energy.at(ii)/
          image_info_no_end.at(l).energy_sum *100;
    }
    
    // power
    image_info_no_end.at(l).power_sum = 
        image_info_no_end.at(l).energy_sum / 
        image_info_no_end.at(l).cycle_sum * 1000; // mW
  }

}


void init_end() {
  FILE *fp;
  string file_name;

  image_info_end.resize(l_size, LayerInfo());
  // for end 
  for (int i = 1; i <= num_image; ++i) {
    for(int l = 0; l < l_size; ++l) {
      file_name = out_dir+"out_end_"+to_string(i)+"_"+to_string(layers[l]);
      if( (fp = fopen(file_name.c_str(), "r")) == NULL) {
        fprintf(stderr,"\n%s No such a file\n\n", file_name.c_str());
        exit(1); 
      }
      for (int ii = 0; ii < EOP; ++ii) {
        long temp;
        fscanf(fp, "%ld",&temp );
        image_info_end.at(l).component_cycle.at(ii) += temp;
      } 


      // energy 
      for (int ii = 0; ii < E_EOP; ++ii) {
        double temp;
        fscanf(fp, "%lf",&temp );

        if(ii==S_GB && (layers[l]==35 || layers[l] ==37)){
          image_info_end.at(l).energy.at(ii) += (double)0;
        }
        else {
          image_info_end.at(l).energy.at(ii) += (double)temp;
        }
      }

      fclose(fp);
    }
  }


  for(int l = 0; l < l_size; ++l) {
    for (int ii = 0; ii < EOP; ++ii) {
      image_info_end.at(l).component_cycle.at(ii) /= num_image;
      image_info_end.at(l).cycle_sum += 
          image_info_end.at(l).component_cycle.at(ii);

      // energy
      image_info_end.at(l).energy_sum += 
          (double) image_info_end.at(l).energy.at(ii);

    }
    image_info_end.at(l).ratio_sum = 
        (double) image_info_end.at(l).cycle_sum /
        image_info_no_end.at(l).cycle_sum * 100;

    // energy
    image_info_end.at(l).energy_ratio_sum = 
        (double) image_info_end.at(l).energy_sum /
        image_info_no_end.at(l).energy_sum * 100;
  }


  // end cycle ratio
  for(int l = 0; l < l_size; ++l) {
    for (int ii = 0; ii < EOP; ++ii) {
      image_info_end.at(l).component_cycle_ratio.at(ii) =
          (double)image_info_end.at(l).component_cycle.at(ii)/
          image_info_no_end.at(l).cycle_sum * 100; // normalized by no_end sum

      // energy
      image_info_end.at(l).energy_ratio.at(ii) =
          (double)image_info_end.at(l).energy.at(ii) /
          image_info_no_end.at(l).energy_sum * 100; // normalized by no_end sum
      
    }

    // power
    image_info_end.at(l).power_sum = 
        image_info_end.at(l).energy_sum / 
        image_info_end.at(l).cycle_sum * 1000; // mW

  //energy2.resize(E_EOP,0);
  }
}
void printPerf2() {  
  // print
  for(int l = 0; l < l_size; ++l) {
    //printf("||L%d    |",  layers[l]);
    //printf("||L%d    |",  l+2);
  }
  //cout << endl;


  double temp_no_end; 
  double temp_end; 


  for(int l = 0; l < l_size; ++l) {
    // MEM_GB
    temp_no_end = image_info_no_end[l].component_cycle.at(1);
    temp_end = image_info_end[l].component_cycle.at(1);
    printf("|%lf|%lf|", temp_no_end, temp_end); 
  }
  cout << endl;

  for(int l = 0; l < l_size; ++l) {
    // GB_WB
    temp_no_end = 
        image_info_no_end[l].component_cycle.at(2) +
        image_info_no_end[l].component_cycle.at(6);

    temp_end = 
        image_info_end[l].component_cycle.at(2) +
        image_info_end[l].component_cycle.at(6); 

    printf("|%lf|%lf|", temp_no_end, temp_end); 
  }
  cout << endl;

  for(int l = 0; l < l_size; ++l) {
    // MEM_WB 
    temp_no_end = image_info_no_end[l].component_cycle.at(9);
    temp_end = image_info_end[l].component_cycle.at(9);
    printf("|%lf|%lf|", temp_no_end, temp_end); 
  }
  cout << endl;




  for(int l = 0; l < l_size; ++l) {
    // MEM_FB
    temp_no_end = 
        image_info_no_end[l].component_cycle.at(3) +
        image_info_no_end[l].component_cycle.at(7);

    temp_end = 
        image_info_end[l].component_cycle.at(3) +
        image_info_end[l].component_cycle.at(7); 

    printf("|%lf|%lf|", temp_no_end, temp_end); 
  }
  cout << endl;

  for(int l = 0; l < l_size; ++l) {
    // FB_PU
    temp_no_end = 
        image_info_no_end[l].component_cycle.at(4) +
        image_info_no_end[l].component_cycle.at(5);

    temp_end = 
        image_info_end[l].component_cycle.at(4) +
        image_info_end[l].component_cycle.at(5); 

    printf("|%lf|%lf|", temp_no_end, temp_end); 
  }
  cout << endl;

  for(int l = 0; l < l_size; ++l) {
    // RUN_PU
    temp_no_end = image_info_no_end[l].component_cycle.at(8);
    temp_end = image_info_end[l].component_cycle.at(8);
    printf("|%lf|%lf|", temp_no_end, temp_end); 
  }
cout << endl;




  for(int l = 0; l < l_size; ++l) {
    printf("|%ld|%ld|",   
           image_info_no_end.at(l).cycle_sum-1,
           image_info_end[l].cycle_sum-1);
  }
  cout << endl;
}

void printPerf() {  
  // print
  for(int l = 0; l < l_size; ++l) {
    //printf("||L%d    |",  layers[l]);
    printf("||L%d    |",  l+2);
  }
  cout << endl;


  double temp_no_end; 
  double temp_end; 


  for(int l = 0; l < l_size; ++l) {
    // MEM_GB
    temp_no_end = image_info_no_end[l].component_cycle_ratio.at(1);
    temp_end = image_info_end[l].component_cycle_ratio.at(1);
    printf("|%lf|%lf|", temp_no_end, temp_end); 
  }
  cout << endl;

  for(int l = 0; l < l_size; ++l) {
    // GB_WB
    temp_no_end = 
        image_info_no_end[l].component_cycle_ratio.at(2) +
        image_info_no_end[l].component_cycle_ratio.at(6);

    temp_end = 
        image_info_end[l].component_cycle_ratio.at(2) +
        image_info_end[l].component_cycle_ratio.at(6); 

    printf("|%lf|%lf|", temp_no_end, temp_end); 
  }
  cout << endl;

  for(int l = 0; l < l_size; ++l) {
    // MEM_WB 
    temp_no_end = image_info_no_end[l].component_cycle_ratio.at(9);
    temp_end = image_info_end[l].component_cycle_ratio.at(9);
    printf("|%lf|%lf|", temp_no_end, temp_end); 
  }
  cout << endl;




  for(int l = 0; l < l_size; ++l) {
    // MEM_FB
    temp_no_end = 
        image_info_no_end[l].component_cycle_ratio.at(3) +
        image_info_no_end[l].component_cycle_ratio.at(7);

    temp_end = 
        image_info_end[l].component_cycle_ratio.at(3) +
        image_info_end[l].component_cycle_ratio.at(7); 

    printf("|%lf|%lf|", temp_no_end, temp_end); 
  }
  cout << endl;

  for(int l = 0; l < l_size; ++l) {
    // FB_PU
    temp_no_end = 
        image_info_no_end[l].component_cycle_ratio.at(4) +
        image_info_no_end[l].component_cycle_ratio.at(5);

    temp_end = 
        image_info_end[l].component_cycle_ratio.at(4) +
        image_info_end[l].component_cycle_ratio.at(5); 

    printf("|%lf|%lf|", temp_no_end, temp_end); 
  }
  cout << endl;

  for(int l = 0; l < l_size; ++l) {
    // RUN_PU
    temp_no_end = image_info_no_end[l].component_cycle_ratio.at(8);
    temp_end = image_info_end[l].component_cycle_ratio.at(8);
    printf("|%lf|%lf|", temp_no_end, temp_end); 
  }
  cout << endl;




  for(int l = 0; l < l_size; ++l) {
    printf("|%lf|%lf|",   
           image_info_no_end.at(l).ratio_sum,
           image_info_end[l].ratio_sum
          );
  }
  cout << endl;
}

void printEnergy() {
  // print
  //for(int l = 0; l < l_size; ++l) {
    //printf("||L%d    |",  layers[l]);
    //printf("||L%d    |",  l+2);
 // }
  //cout << endl;

  for (int ii = 0; ii < E_EOP; ++ii) {
    for(int l = 0; l < l_size; ++l) {
      printf("|%lf|%lf|", 
             image_info_no_end[l].energy_ratio.at(eng_reorder[ii]),
             image_info_end[l].energy_ratio.at(eng_reorder[ii])
            );
    }
    cout << endl;
  }

  for(int l = 0; l < l_size; ++l) {
    printf("|%lf|%lf|", 
           image_info_no_end[l].energy_ratio_sum,
           image_info_end[l].energy_ratio_sum); 
  }
  cout << endl;


}

void printEnergy2() {
  // print
  //for(int l = 0; l < l_size; ++l) {
    //printf("||L%d    |",  layers[l]);
  //  printf("||L%d    |",  l+2);
 // }
  //cout << endl;

  for (int ii = 0; ii < E_EOP; ++ii) {
    for(int l = 0; l < l_size; ++l) {
      printf("|%lf|%lf|", 
             image_info_no_end[l].energy.at(eng_reorder[ii]),
             image_info_end[l].energy.at(eng_reorder[ii])/num_image
            );
    }
    cout << endl;
  }

  for(int l = 0; l < l_size; ++l) {
    printf("|%lf|%lf|", 
           image_info_no_end[l].energy_sum,
           image_info_end[l].energy_sum/num_image); 
  }
  cout << endl;


}

void printPower() {
  for(int l = 0; l < l_size; ++l) {
    //printf("||L%d    |",  layers[l]);
    double power_no_end = 
        image_info_no_end[l].energy_sum /
        image_info_no_end[l].cycle_sum * 1000;

    double power_end = 
        image_info_end[l].energy_sum /
        image_info_end[l].cycle_sum * 1000; // mW
    
    printf("L%d|%lf|%lf \n",  l+2, power_no_end, power_end);
  }

}

int main() {
  init_no_end(); 
  init_end(); 

  printPerf();
  //printEnergy2();
  //printPower();
  return 0;
}


