#include "global_controller.h"
#include "config.h"
#include "assert.h"
// Note :
// code implementation considering hardware structure 


CtrlCore::CtrlCore(int total_pipe_stage, int bit_size, LayerInfo* layer_info) 
  : total_pipe_stage_(total_pipe_stage+1),  // considering one in input buffer 
    bit_size_(bit_size),
    layer_info_(layer_info)
{

  pipe_entry_list.resize(total_pipe_stage_); 

  /*
     cout<< "--------------------------------------1"<<endl;
     printf("   y   x   z \n");
     for (int k = 0; k < layer_info_->nout_size[2]; k++) { // z
     for (int i = 0; i < layer_info_->nout_size[0]; i++) { // y
     for (int j = 0; j < layer_info_->nout_size[1]; j++) { // x
  //for (int k = 0; k < 1; k++) {
  printf("%4d%4d%4d :\t%d\n",i,j,k,(*layer_info_->ptr_nout_count)[i][j][k]);
  }
  }
  }

  cout<< "--------------------------------------2"<<endl;
  */

}

CtrlCore::~CtrlCore() {
}

void CtrlCore::insertEntry(int z, int bit_pos) {

  // this can be parallelizing with hardware like cache tags
  for (int i = 0; i < total_pipe_stage_; ++i) {
    pipeEntry *entry = &(pipe_entry_list.at(i));
    if(!entry->valid_) {
      entry->setData(true, z, bit_pos);
      break;
    }
  }

}

pipeEntry* CtrlCore::findEntry(int z) {
  // this can be parallelizing with hardware like cache tags
  for (int i = 0; i < total_pipe_stage_; ++i) {
    pipeEntry *entry = &(pipe_entry_list.at(i));
    if(entry->valid_ && entry->z_ == z ) {
      return entry;
    }
  }
  return NULL;
}

pipeEntry* CtrlCore::findFirstEntry() {
  // this can be parallelizing with hardware like cache tags
  for (int i = 0; i < total_pipe_stage_; ++i) {
    pipeEntry *entry = &(pipe_entry_list.at(i));
    if(entry->valid_ && entry->bit_pos_ < bit_size_ ) {
      return entry;
    }
  }
  return NULL;
}

bool CtrlCore::isValidEntry() {
  // this can be parallelizing with hardware like cache tags
  for (int i = 0; i < total_pipe_stage_; ++i) {
    pipeEntry *entry = &(pipe_entry_list.at(i));
    if(entry->valid_  ) {
      return true;
    }
  }
  return false;
}

void CtrlCore::printEntry() {
  for (int i = 0; i < total_pipe_stage_; ++i) {
    pipeEntry *entry = &(pipe_entry_list.at(i));
    if(!entry->valid_) {
      cout<< "n\t";
    }
    else {
      cout << entry->z_ << "\t";
    }
  }
  cout << endl;
}

///
GlobalCtrl::GlobalCtrl(SimConfigs *sim_config, NetConfigs *net_config, LayerInfo *layer_info) {
  sim_config_ = sim_config;
  net_config_ = net_config;
  layer_info_ = layer_info;


  pipe_ports_ = new PipePort[NUM_PIPELINE_STAGE+1];

  div_num_ = sim_config_->div_num;

  SB_pipe_stage_  = new SBsPipeStage( &pipe_ports_[0], &pipe_ports_[1], 1, div_num_ );  
  provider_pipe_stage_ = new ProviderPipeStage( &pipe_ports_[1], &pipe_ports_[2], 2, div_num_);  
  PE_pipe_stage_ = new PEsPipeStage( &pipe_ports_[2], &pipe_ports_[3], 3, div_num_);  
  ctrl_pipe_stage_ = new CtrlsPipeStage( &pipe_ports_[3], &pipe_ports_[4], 1, div_num_);  

  bit_size_ = 16;

  
  ctrl_core_.resize(div_num_, CtrlCore(1 + 2 + 3 + 1, bit_size_, layer_info_));

  end_pos_ = layer_info_->end_pos_;

  ///


  end_condition_test_  = 0;
  run_end_flag_.resize(div_num_, false);

  count_x_ = 1;
  count_y_ = 1;

  count_z_.resize(div_num_);
  // div_output_layer_z_ initialization
  div_output_layer_z_.resize(div_num_, pair<int, int>());

  

  // one PipeOp for init, push to input queue
  pipe_ports_[0].push(new PipeOp(div_num_)); // one for init



  /*

  for (int i = 0; i< div_num_; ++i) {
    ctrl_core_.at(i).printEntry();
  }

  // print
  for (int i = 0; i < div_num_; ++i) {
    cout << i << ": ";
    pipe_ports_[0].front()->printPipeOp(i);
    SB_pipe_stage_->print_stage(i);
    provider_pipe_stage_->print_stage(i);
    PE_pipe_stage_->print_stage(i);
    ctrl_pipe_stage_->print_stage(i);
    cout << endl;
  }
  cout << endl;
*/


}

GlobalCtrl::~GlobalCtrl() {
  delete[] pipe_ports_;
  delete SB_pipe_stage_;
  delete provider_pipe_stage_;
  delete PE_pipe_stage_;
  delete ctrl_pipe_stage_;
}


void GlobalCtrl::setToDoFilters(int fblock_id) {
  
  fblock_id_ = fblock_id;

  int div_len = fblock_len_ / div_num_;
  
  //cout << "set To do" << endl;
  for (int i = 0; i< div_num_; ++i) {
    count_z_.at(i) = div_len * i + 1 + (fblock_id_-1) * fblock_len_;

    div_output_layer_z_.at(i).first = div_len * i + 1 + (fblock_id_-1)*fblock_len_; // start z index
    div_output_layer_z_.at(i).second= div_len * (i + 1) + (fblock_id-1)*fblock_len_; // end z index

    //cout << i << " " << count_z_.at(i) << " " << div_output_layer_z_.at(i).first << " " << div_output_layer_z_.at(i).second << endl;
  }

  

  //cout << endl;
}

void GlobalCtrl::initCountZ() {
  PipeOp *op = pipe_ports_[0].front(); 
  for (int i = 0; i< div_num_; ++i) {
    count_z_.at(i) = div_output_layer_z_.at(i).first;
    op->setOpInfo(i, count_z_.at(i), 1);
    op->setValid(i);

    ctrl_core_.at(i).insertEntry(count_z_.at(i),1);
    run_end_flag_.at(i) = false; 
  }
}




void GlobalCtrl::cycle() {
  if(sim_config_->END_enable==1) {
    cycle_END();
  }
  else {
    cycle_no_END();
  }


}

void GlobalCtrl::cycle_no_END() {

 /* 
  for (int i = 0; i< div_num_; ++i) {
    ctrl_core_.at(i).printEntry();
  }
  // print
  for (int i = 0; i < div_num_; ++i) {
    cout << i << ": ";
    pipe_ports_[0].front()->printPipeOp(i);
    SB_pipe_stage_->print_stage(i);
    provider_pipe_stage_->print_stage(i);
    PE_pipe_stage_->print_stage(i);
    ctrl_pipe_stage_->print_stage(i);
    cout << endl;
  }
  cout << endl;

  */
  SB_pipe_stage_->countAccNum();
  provider_pipe_stage_->countAccNum();
  PE_pipe_stage_->countAccNum();
  ctrl_pipe_stage_->countAccNum();



  //cout<< "global cycle" << endl;

  SB_pipe_stage_->cycle();
  provider_pipe_stage_->cycle();
  PE_pipe_stage_->cycle();
  ctrl_pipe_stage_->cycle();

  //ctrl_core_->printEntry();

  PipeOp *op = NULL;

  if(!pipe_ports_[NUM_PIPELINE_STAGE].empty()) {
    op  = pipe_ports_[NUM_PIPELINE_STAGE].front(); 
    pipe_ports_[NUM_PIPELINE_STAGE].pop(); 

    for ( int idx = 0; idx < div_num_; ++idx) {

      PipeOpInfo* info = op->getPipeOpInfo(idx);

      pipeEntry* entry = ctrl_core_.at(idx).findEntry(info->z_);


      if( !op->isValid(idx) || entry == NULL) {
        subProcess(op, idx);

      }
      else { 
        //if(info->bit_pos_ == end_pos_[count_x_-1][count_y_-1][info->z_-1]) {
        if(info->bit_pos_ == bit_size_) {
          // out data <= 0;
          entry->resetEntry();
          subProcess(op, idx);

        }
        else {
          if( info->bit_pos_ == bit_size_) {
            // out data <= calculated data 
            entry->resetEntry();
            subProcess(op, idx);


          }
          else {
            if( entry->bit_pos_ == bit_size_) {
              subProcess(op, idx);
            }
            else {
              entry->bit_pos_++;
              info->bit_pos_ = entry->bit_pos_;

            }
          }


        }

      }

    }


    pipe_ports_[0].push(op);
  }
}


void GlobalCtrl::cycle_END() {

 /* 
  for (int i = 0; i< div_num_; ++i) {
    ctrl_core_.at(i).printEntry();
  }
  // print
  for (int i = 0; i < div_num_; ++i) {
    cout << i << ": ";
    pipe_ports_[0].front()->printPipeOp(i);
    SB_pipe_stage_->print_stage(i);
    provider_pipe_stage_->print_stage(i);
    PE_pipe_stage_->print_stage(i);
    ctrl_pipe_stage_->print_stage(i);
    cout << endl;
  }
  cout << endl;

  */


  SB_pipe_stage_->countAccNum();
  provider_pipe_stage_->countAccNum();
  PE_pipe_stage_->countAccNum();
  ctrl_pipe_stage_->countAccNum();

  //cout<< "global cycle" << endl;

  SB_pipe_stage_->cycle();
  provider_pipe_stage_->cycle();
  PE_pipe_stage_->cycle();
  ctrl_pipe_stage_->cycle();

  //ctrl_core_->printEntry();

  PipeOp *op = NULL;

  if(!pipe_ports_[NUM_PIPELINE_STAGE].empty()) {
    op  = pipe_ports_[NUM_PIPELINE_STAGE].front(); 
    pipe_ports_[NUM_PIPELINE_STAGE].pop(); 
  


    for ( int idx = 0; idx < div_num_; ++idx) {

      PipeOpInfo* info = op->getPipeOpInfo(idx);

      pipeEntry* entry = ctrl_core_.at(idx).findEntry(info->z_);


      if( !op->isValid(idx) || entry == NULL) {
        subProcess(op, idx);

      }
      else {
//        printf("%d\t%d\n", info->bit_pos_, end_pos_[count_x_-1][count_y_-1][info->z_-1]);
//        assert(0);
        if(info->bit_pos_ >= end_pos_[count_x_-1][count_y_-1][info->z_-1]) { // negative?
          // out data <= 0;
          entry->resetEntry();
          subProcess(op, idx);

        }
        else {
          if( info->bit_pos_ == bit_size_) {
            // out data <= calculated data 
            entry->resetEntry();
            subProcess(op, idx);


          }
          else {
            if( entry->bit_pos_ == bit_size_) {
              subProcess(op, idx);
            }
            else {
              entry->bit_pos_++;
              info->bit_pos_ = entry->bit_pos_;

            }
          }

        }

      }

    }


    pipe_ports_[0].push(op);
  }
}

void GlobalCtrl::subProcess(PipeOp *op, int idx) {

      PipeOpInfo* info = op->getPipeOpInfo(idx);

      if(count_z_.at(idx) != div_output_layer_z_.at(idx).second) { //
        count_z_.at(idx)++;

        ctrl_core_.at(idx).insertEntry(count_z_.at(idx), 1); 

        info->z_ = count_z_.at(idx);
        info->bit_pos_ = 1;
        op->setValid(idx);

      }
      else {

        if(ctrl_core_.at(idx).isValidEntry()) {
          pipeEntry *entry_ref = ctrl_core_.at(idx).findFirstEntry();
          if(entry_ref != NULL) {
            entry_ref->bit_pos_++;

            info->z_ = entry_ref->z_;
            info->bit_pos_ = entry_ref->bit_pos_;
            op->setValid(idx);
          }
          else {
            op->resetInfo(idx);
          }
        }
        else {
          op->resetInfo(idx);
          resetPipelineAll(idx);
          run_end_flag_.at(idx) = true; 
        }
      }

}

void GlobalCtrl::set_end_condition_test(int num) {
  end_condition_test_ = num;
  ctrl_pipe_stage_->set_end_condition_test(num);
}

void GlobalCtrl::resetPipelineAll(unsigned idx) {

  SB_pipe_stage_->resetPipeline(idx);
  provider_pipe_stage_->resetPipeline(idx);
  PE_pipe_stage_->resetPipeline(idx);
  ctrl_pipe_stage_->resetPipeline(idx);
}


bool GlobalCtrl::isRunEnd() {
  bool b = true;
  for (int i = 0; i < div_num_; ++i) {
    b = b && run_end_flag_.at(i);
  }
  return b;
}

void GlobalCtrl::calcAccess() {
  // for WB 
  uint64_t tmp_num_WB_ = 0;
  for (int i = 0; i < SB_pipe_stage_->acc_num_.size(); ++i) {
    tmp_num_WB_ += SB_pipe_stage_->acc_num_.at(i);
  }
  num_acc_WB_ = sim_config_->WB_num * tmp_num_WB_; 

  // for provider
  uint64_t tmp_num_NET_ = 0;
  for (int i = 0; i < provider_pipe_stage_->acc_num_.size(); ++i) {
    tmp_num_NET_ += provider_pipe_stage_->acc_num_.at(i);
  } 

  num_acc_net_ = tmp_num_NET_; 

  // for PU
  num_acc_PU_1_ = PE_pipe_stage_->acc_num_.at(0);
  num_acc_PU_2_ = PE_pipe_stage_->acc_num_.at(1);
  num_acc_PU_3_ = PE_pipe_stage_->acc_num_.at(2);

  // for Global controller
  num_acc_ctrl_ = ctrl_pipe_stage_->acc_num_.at(0); 

}



