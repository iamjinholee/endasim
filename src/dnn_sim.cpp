#include "dnn_sim.h"


#define DEBUG_COUT 0
#define DEBUG_COUT2 0

DNNSim::DNNSim(SimConfigs* config, NetConfigs* net, LayerInfo* layer_info) : sim_config_(config), net_config_(net), layer_info_(layer_info){

  //m_dram_interface = new dram_interface("ini/DDR2_micron_16M_8b_x8_sg3E.ini", 
  m_dram_interface = new dram_interface("ini/DDR3_micron_32M_8B_x8_sg15.ini", 
                                        "system.ini", 
                                        "./DRAMSim2/", 
                                        "DNNSim", 
                                        16384,
                                        config->outfile_name,
                                        config->frequency
                                       );

  m_read_callback = new DRAMSim::Callback<DNNSim, void, unsigned, uint64_t, uint64_t>(this, &DNNSim::read_complete_callback);
  m_write_callback = new DRAMSim::Callback<DNNSim, void, unsigned, uint64_t, uint64_t>(this, &DNNSim::write_complete_callback);
  m_dram_interface->set_callbacks(m_read_callback, m_write_callback);



  //
  sim_state_ = INIT;  
  // for global controller
  global_ctrl_ = new GlobalCtrl(sim_config_, net_config_, layer_info_);

  GB_ = new BufferGroup(sim_config_->GB_num, sim_config_->GB_bank_num, 
                        sim_config_->GB_access_cycle, sim_config_->GB_write_cycle); 

  WB_ = new WeightBufferGroup(sim_config_->WB_num, sim_config_->WB_bank_num,
                              sim_config_->WB_access_cycle); 

  FB_ = new BufferGroup(sim_config_->FB_num, sim_config_->FB_bank_num, 
                        sim_config_->FB_access_cycle); 

  PU_ = new PUnitArray(sim_config_->row_num, sim_config_->row_PU_num / sim_config_->div_num, 
                       sim_config_->PU_access_cycle);

  GB_->setPtrNext(WB_);
  FB_->setPtrNext(PU_);


  m_sim_cycle = 0;
  test_n = 0;


  cycle_load_PU_ = 0;
  count_cycle_load_PU_ = 0;
  count_cycle_load_PU_part_ = 0;

  output_layer_x_ = net_config_->output_layer_x;
  output_layer_y_ = net_config_->output_layer_y;


  output_ = new OutputFile(sim_config_, net_config_);
  output_->resizeComponentCycle(EOP);

}

DNNSim::~DNNSim() {
  delete m_read_callback;
  delete m_write_callback;
  delete global_ctrl_;

  delete GB_;
  delete WB_;
  delete FB_;

  delete output_;
}

void DNNSim::cycle() {

  if (net_config_->total_imap_capacity <= sim_config_->total_PU_capacity) {
    cycle_fc1();
  }
  else {
    cycle_conv();
  }
}

void DNNSim::cycle_conv() {

  m_sim_cycle++;

  m_dram_interface->cycle();
  GB_->cycle();
  WB_->cycle();
  FB_->cycle();
  PU_->cycle();

  // this code, here or inside switch?
  switch (sim_state_) {
    case INIT:
      ++output_->component_cycle.at(INIT);

      num_MEM_GB_ = // for all weights in all filters // Number of DRAM accesses to fetch all filters into Global Buffer (STT-RAM) 
          net_config_->filter_size *
          net_config_->filter_num * 
          2 / 64; 

      // num_GB_WB_
      if (net_config_->total_filter_capacity <= sim_config_->total_WB_capacity*1024) {
        fblock_num_ = 1;
        num_GB_WB_ = net_config_->total_filter_capacity / 64;
      }
      else {
        fblock_num_ = net_config_->total_filter_capacity / (sim_config_->total_WB_capacity*1024); 
        num_GB_WB_ = sim_config_->total_WB_capacity*1024 / 64; 
      }

      // num_MEM_FB_, num_MEM_FB_PART_
      // total_FB_capacity should be large enough to store imap rows of filter y
      if (net_config_->total_imap_capacity <= sim_config_->total_FB_capacity*1024) {
        num_FB_imap_y_ = net_config_->input_layer_y;
        omap_pos_y_len_ =  net_config_->output_layer_y;
      }
      else {
        num_FB_imap_y_ = 
            sim_config_->total_FB_capacity*1024 /  
            (net_config_->total_imap_capacity/ net_config_->input_layer_y);

        omap_pos_y_len_ =  num_FB_imap_y_ - net_config_->filter_size_y + 1;
      }


      omap_pos_y_.first = 1;
      omap_pos_y_.second = omap_pos_y_len_;



      num_MEM_FB_ = 
          num_FB_imap_y_ * 
          net_config_->input_layer_x * 
          net_config_->input_layer_z * 
          2 / 64;

      num_MEM_FB_PART_TEMP_ = // for features in one rows of input feature map
          ( num_FB_imap_y_ - net_config_->filter_size_y + 1 ) *
          net_config_->input_layer_x *
          net_config_->input_layer_z *
          2 / 64;


      //
      num_FB_PU_ =  // for registers in all PUs
          sim_config_->PU_input_num *
          sim_config_->row_PU_num *
          sim_config_->row_num *
          2 / 64 / sim_config_->div_num;

      num_FB_PU_PART_ = // for registers in some rows of PUs
          sim_config_->PU_input_num *
          sim_config_->row_PU_num *
          sim_config_->row_num *
          2 / 64 / net_config_->filter_size_x / sim_config_->div_num;

      fblock_id_ = 1;
      fblock_len_ = net_config_->filter_num / fblock_num_;

      WB_->set_cur_fblock_id(fblock_id_);



      // 
      x_dir_ = true; // up 
      count_x_ = 1;
      count_y_ = 1;

      global_ctrl_->set_fblock_len(fblock_len_);
      global_ctrl_->setToDoFilters(fblock_id_); // after set fblock_len_
      global_ctrl_->setXY(count_x_,count_y_);
      global_ctrl_->initCountZ();

      //
      counter_MEM_GB_ = 0; 
      counter_GB_WB_ = 0;
      counter_MEM_FB_ = 0; 
      counter_FB_PU_ = 0; 
      counter_FB_PU_PART_ = 0; 
      counter_MEM_FB_PART_ = 0; 
      num_mem_request_ = 0; 
      num_mem_callback_ = 0;

      //
      sim_state_ = MEM_GB; 

#if DEBUG_COUT
      cout <<"cycle: " << m_sim_cycle<<  " INIT->MEM_GB. "<<endl;
#endif

      break;

    case MEM_GB:

      ++output_->component_cycle.at(MEM_GB);

      // read MEM
      if(counter_MEM_GB_ < num_MEM_GB_) {
        if(m_dram_interface->can_accept_request() ){
          ++num_mem_request_;

          ++counter_MEM_GB_;
          m_dram_interface->push_request(counter_MEM_GB_* 64 + WEIGHT_BASE_ADDRESS, false);

          // cout << "cycle: " << m_sim_cycle<< " req MEM, MEM->GB " << counter_MEM_GB_<< endl;
        }
      }

      if (GB_->getWrAccessCounter() == num_MEM_GB_) {
        GB_->resetWrAccessCounter();
        counter_MEM_GB_ = 0;

        sim_state_ = GB_WB;

#if DEBUG_COUT
        cout <<"cycle: " << m_sim_cycle<<  " MEM_GB->GB_WB. "<<num_MEM_GB_<<endl;
#endif
      }

      break;

    case GB_WB:

      ++output_->component_cycle.at(GB_WB);
      // read GB
      if (counter_GB_WB_ < num_GB_WB_) {
        ++counter_GB_WB_;
        GB_->pushReqRd(counter_GB_WB_ * 64 + (WB_->get_cur_fblock_id()-1) * num_GB_WB_ *64);
        //cout <<"cycle: " << m_sim_cycle<<  " rd GB. "<<endl;
      }

      if (WB_->getWrAccessCounter() == num_GB_WB_) {
        WB_->resetWrAccessCounter();
        counter_GB_WB_ = 0;

        sim_state_ = MEM_FB;
        //


#if DEBUG_COUT
        cout <<"cycle: " << m_sim_cycle<<  " GB_WB->MEM_FB. "<< num_GB_WB_<<endl;
#endif
      }

      break;

    case MEM_FB:

      ++output_->component_cycle.at(MEM_FB);
      if(counter_MEM_FB_ < num_MEM_FB_) {
        if(m_dram_interface->can_accept_request() ){
          ++num_mem_request_;

          ++counter_MEM_FB_;
          m_dram_interface->push_request(counter_MEM_FB_* 64 + INPUT_FEATURE_BASE_ADDRESS, false);
        }
      }

      if (FB_->getWrAccessCounter() == num_MEM_FB_) {
        FB_->resetWrAccessCounter();
        counter_MEM_FB_ = 0;

        sim_state_ = FB_PU;

#if DEBUG_COUT
        cout <<"cycle: " << m_sim_cycle<<  " MEM_FB->FB_PU. "<<endl;
#endif
      }
      break;

    case FB_PU:

      ++output_->component_cycle.at(FB_PU);
      if (counter_FB_PU_ < num_FB_PU_) {
        ++counter_FB_PU_;
        FB_->pushReqRd(counter_FB_PU_ * 64 );
        //cout <<"cycle: " << m_sim_cycle<<  " rd GB. "<<endl;
      }

      if (PU_->getWrAccessCounter() == num_FB_PU_) {
        PU_->resetWrAccessCounter();
        counter_FB_PU_ = 0;

        sim_state_ = RUN_PU;
#if DEBUG_COUT
        cout <<"cycle: " << m_sim_cycle<<  " FB_PU->RUN_PU. "<<endl;
#endif
      }

      break;

    case RUN_PU:
      ++output_->component_cycle.at(RUN_PU);
      if(!global_ctrl_->isRunEnd()) {
        global_ctrl_->cycle(); 
      }
      else {
#if DEBUG_COUT2
        cout <<"cycle: " << m_sim_cycle<<  " RUN_PU END. "<< count_x_ << " " << count_y_ <<" " << fblock_id_<<endl;
#endif
        /*
           cout <<"cycle: " << m_sim_cycle<<  " RUN_PU_done. ";
           cout << " (x,y,z)  ";
           cout << global_ctrl_->count_x_ <<", ";
           cout << global_ctrl_->count_y_ <<", | ";
           for (int i = 0; i< global_ctrl_->div_num_; ++i) {
           cout <<"("<< global_ctrl_->div_output_layer_z_.at(i).second << "), ";
           }
           cout <<"|" << endl;
           */

        if( !isEndX() ) { // not end of outlayer X

          moveX();
          global_ctrl_->setXY(count_x_, count_y_);
          global_ctrl_->initCountZ();

          // FB_PU_PART
          sim_state_ = FB_PU_PART;

#if DEBUG_COUT
          cout <<"cycle: " << m_sim_cycle<<  " RUN_PU->FB_PU_PART X. "<< count_x_ << " " << count_y_ <<" " << fblock_id_<<endl;
#endif

        }
        else {  

          if ( count_y_ < omap_pos_y_.second ) { // not end of y 
            x_dir_ = !x_dir_; // x_directon change
            ++count_y_; // up_y

            global_ctrl_->setXY(count_x_, count_y_);
            global_ctrl_->initCountZ();

            // FB_PU_PART
            sim_state_ = FB_PU_PART;

#if DEBUG_COUT
            cout <<"cycle: " << m_sim_cycle<<  " RUN_PU->FB_PU_PART Y. "<< count_x_ << " " << count_y_ <<" " << fblock_id_<<endl;
#endif
          }
          else { // count_y_ == omap_pos_y_

            if ( fblock_id_ < fblock_num_ ) { // not all filter

              x_dir_ = true; // x_direction: up
              count_x_ = 1; // count_x_ reset 
              count_y_ = omap_pos_y_.first;

              ++fblock_id_; // for z
              global_ctrl_->setToDoFilters(fblock_id_);

              global_ctrl_->setXY(count_x_, count_y_);
              global_ctrl_->initCountZ();

              //
              WB_->set_cur_fblock_id(fblock_id_);
              sim_state_ = GB_WB_PART;

#if DEBUG_COUT
              cout <<"cycle: " << m_sim_cycle<<  " RUN_PU->GB_WB_PART. "<< count_x_ << " " << count_y_ <<endl;
#endif
            }
            else { // all filter, need to update FB
              if ( omap_pos_y_.second < output_layer_y_) { // need more data for FB

                if ( omap_pos_y_.second + omap_pos_y_len_ <= output_layer_y_  ) {
                  // new index for data in FB
                  omap_pos_y_.first = omap_pos_y_.second + 1;
                  omap_pos_y_.second = omap_pos_y_.second + omap_pos_y_len_;
                  num_MEM_FB_PART_ = num_MEM_FB_PART_TEMP_;
                }
                else {
                  omap_pos_y_.first = omap_pos_y_.second + 1;
                  omap_pos_y_.second = output_layer_y_;
                  num_MEM_FB_PART_ = 
                      (output_layer_y_ - omap_pos_y_.second) *
                      net_config_->input_layer_x *
                      net_config_->input_layer_z *
                      2 / 64;
                }

                // Actually, data in the WB is reeused for new FB data
                // but for simple implementation, reset index of flock_id
                fblock_id_ = 1;
                WB_->set_cur_fblock_id(fblock_id_);

                // for PU
                x_dir_ = true; // x_direction: up
                count_x_ = 1; // count_x_ reset 
                count_y_ = omap_pos_y_.first;
                global_ctrl_->setToDoFilters(fblock_id_);
                global_ctrl_->setXY(count_x_, count_y_);
                global_ctrl_->initCountZ();

                // 
                sim_state_ = MEM_FB_PART;

#if DEBUG_COUT
                cout <<"cycle: " << m_sim_cycle<<  " RUN_PU->MEM_FB_PART. "<< count_x_ << " " << count_y_ <<endl;
                cout << omap_pos_y_.second << " " << output_layer_y_<<endl;
#endif

              }
              else { // no more data for FB

                sim_state_ = EOP;


              } // end FB

            } // all filter

          } // end Y 

        } // endX
      }

      break;

    case FB_PU_PART:

      ++output_->component_cycle.at(FB_PU_PART);
      if (counter_FB_PU_PART_ < num_FB_PU_PART_) {
        ++counter_FB_PU_PART_;
        FB_->pushReqRd(counter_FB_PU_PART_ * 64 );
        //cout <<"cycle: " << m_sim_cycle<<  "FB_PU_PART rd FB. "<< counter_FB_PU_PART_ << " " << num_FB_PU_PART_<<endl;
      }
      //cout <<"cycle: " << m_sim_cycle<<  " FB_PU_PART  "<< PU_->getWrAccessCounter()<< " " << num_FB_PU_PART_<<endl;

      if (PU_->getWrAccessCounter() == num_FB_PU_PART_) {
        PU_->resetWrAccessCounter();
        counter_FB_PU_PART_ = 0;

        sim_state_ = RUN_PU;

#if DEBUG_COUT
        cout <<"cycle: " << m_sim_cycle<<  " FB_PU_PART->RUN_PU. "<<endl;
#endif
      }
      break;

    case GB_WB_PART:

      ++output_->component_cycle.at(GB_WB_PART);
      // read GB
      if (counter_GB_WB_ < num_GB_WB_) {
        ++counter_GB_WB_;
        GB_->pushReqRd(counter_GB_WB_ * 64 + (WB_->get_cur_fblock_id()-1) * num_GB_WB_ *64);
        //cout <<"cycle: " << m_sim_cycle<<  " rd GB. "<<endl;
      }

      if (WB_->getWrAccessCounter() == num_GB_WB_) {
        WB_->resetWrAccessCounter();
        counter_GB_WB_ = 0;

        sim_state_ = FB_PU;
        //sim_state_ = EOP;

#if DEBUG_COUT
        cout <<"cycle: " << m_sim_cycle<<  " GB_WB_PART->FB_PU. "<<endl;
#endif
      }

      break;

    case MEM_FB_PART:

#if DEBUG_COUT
      cout <<"cycle: " << m_sim_cycle<<  " MEM_FB_PART->FB_PU. "<<counter_MEM_FB_PART_ << " " << num_MEM_FB_PART_<<endl;
#endif
      ++output_->component_cycle.at(MEM_FB_PART);
      if(counter_MEM_FB_PART_ < num_MEM_FB_PART_) {
        if(m_dram_interface->can_accept_request() ){
          ++num_mem_request_;

          ++counter_MEM_FB_PART_;
          m_dram_interface->push_request(counter_MEM_FB_PART_* 64 + INPUT_FEATURE_BASE_ADDRESS, false);
        }
      }

      if (FB_->getWrAccessCounter() == num_MEM_FB_PART_) {
        FB_->resetWrAccessCounter();
        counter_MEM_FB_PART_ = 0;

        sim_state_ = FB_PU;

#if DEBUG_COUT
        cout <<"cycle: " << m_sim_cycle<<  " MEM_FB_PART->FB_PU. "<<endl;
#endif
      }


      break;

    case EOP:

      cout <<"cycle: " << m_sim_cycle<<  " Done. "<<endl;;


      calcAccess();
      output_->calcEnergy();
      output_->makeOutputFile();


      pthread_exit(0); 
      break;
    default:


      break;
  }

}

void DNNSim::cycle_fc1() {

  m_sim_cycle++;

  m_dram_interface->cycle();
  GB_->cycle();
  WB_->cycle();
  FB_->cycle();
  PU_->cycle();


  // this code, here or inside switch?
  switch (sim_state_) {
    case INIT:
      ++output_->component_cycle.at(INIT);

      // num_MEM_WB_
      num_MEM_FB_ = net_config_->total_imap_capacity/64;
      num_FB_PU_ = net_config_->total_imap_capacity/64;


      fblock_len_ = sim_config_->total_WB_capacity * 1024 / net_config_->filter_size / 2;

      num_MEM_WB_ = fblock_len_ * net_config_->filter_size * 2 / 64;

      fblock_num_ = ceil((double)net_config_->filter_num / fblock_len_);
      

      fblock_id_ = 1;

      WB_->set_cur_fblock_id(fblock_id_);

      global_ctrl_->set_fblock_len(fblock_len_);
      global_ctrl_->setToDoFilters(fblock_id_); // after set fblock_len_
      global_ctrl_->setXY(1,1);
      global_ctrl_->initCountZ();

      //
      counter_MEM_GB_ = 0; // not used
      counter_GB_WB_ = 0; // not used

      counter_MEM_WB_ = 0;
      counter_MEM_FB_ = 0; 
      counter_FB_PU_ = 0; 
      counter_FB_PU_PART_ = 0; // not used 
      counter_MEM_FB_PART_ = 0;  // not used
      num_mem_request_ = 0; 
      num_mem_callback_ = 0;

      //
      sim_state_ = MEM_FB; 

#if DEBUG_COUT
      cout <<"cycle: " << m_sim_cycle<<  " INIT->MEM_FB. "<<endl;
#endif

      break;

    case MEM_FB:

      ++output_->component_cycle.at(MEM_FB);
      if(counter_MEM_FB_ < num_MEM_FB_) {
        if(m_dram_interface->can_accept_request() ){
          ++num_mem_request_;

          ++counter_MEM_FB_;
          m_dram_interface->push_request(counter_MEM_FB_* 64 + INPUT_FEATURE_BASE_ADDRESS, false);
        }
      }

      if (FB_->getWrAccessCounter() == num_MEM_FB_) {
        FB_->resetWrAccessCounter();
        counter_MEM_FB_ = 0;

        sim_state_ = FB_PU;

#if DEBUG_COUT
        cout <<"cycle: " << m_sim_cycle<<  " MEM_FB->FB_PU. "<<endl;
#endif
      }
      break;

    case FB_PU:

      ++output_->component_cycle.at(FB_PU);
      if (counter_FB_PU_ < num_FB_PU_) {
        ++counter_FB_PU_;
        FB_->pushReqRd(counter_FB_PU_ * 64 );
        //cout <<"cycle: " << m_sim_cycle<<  " rd GB. "<<endl;
      }

      if (PU_->getWrAccessCounter() == num_FB_PU_) {
        PU_->resetWrAccessCounter();
        counter_FB_PU_ = 0;

        sim_state_ = MEM_WB;
#if DEBUG_COUT
        cout <<"cycle: " << m_sim_cycle<<  " FB_PU->MEM_WB. "<<endl;
#endif
      }

      break;


    case MEM_WB:

      ++output_->component_cycle.at(MEM_WB);

      // read MEM
      if(counter_MEM_WB_ < num_MEM_WB_) {
        if(m_dram_interface->can_accept_request() ){
          ++num_mem_request_;

          ++counter_MEM_WB_;
          m_dram_interface->push_request(counter_MEM_WB_* 64 + (WB_->get_cur_fblock_id()-1) * num_MEM_FB_*64 + WEIGHT_BASE_ADDRESS, false);

          // cout << "cycle: " << m_sim_cycle<< " req MEM, MEM->GB " << counter_MEM_GB_<< endl;
        }
      }

      if (WB_->getWrAccessCounter() == num_MEM_WB_) {
        WB_->resetWrAccessCounter();
        counter_MEM_WB_ = 0;

        sim_state_ = RUN_PU;

#if DEBUG_COUT
        cout <<"cycle: " << m_sim_cycle<<  " MEM_WB->RUN_PU. "<<num_MEM_WB_<<endl;
#endif
      }

      break;

    case RUN_PU:
      ++output_->component_cycle.at(RUN_PU);
      if(!global_ctrl_->isRunEnd()) {
        global_ctrl_->cycle(); 
      }
      else {
#if DEBUG_COUT2
        cout <<"cycle: " << m_sim_cycle<<  " RUN_PU END. "<< count_x_ << " " << count_y_ <<" " << fblock_id_<<endl;
#endif

        if( fblock_id_ < fblock_num_) {
          ++fblock_id_; // for z

          global_ctrl_->setToDoFilters(fblock_id_);
          global_ctrl_->initCountZ();

          //
          WB_->set_cur_fblock_id(fblock_id_);
          sim_state_ = MEM_WB;

#if DEBUG_COUT
          cout <<"cycle: " << m_sim_cycle<<  " RUN_PU->MEM_WB. "<< count_x_ << " " << count_y_ <<endl;
#endif

        } 
        else {
          sim_state_ = EOP;
        }
      }

      break;

    case EOP:

      cout <<"cycle: " << m_sim_cycle<<  " Done. "<<endl;;


      calcAccess();
      output_->calcEnergy();
      output_->makeOutputFile();


      pthread_exit(0); 
      break;
    default:


      break;
  }

}


void DNNSim::cycle_test() {

  if (m_sim_cycle >= 100) {

    pthread_exit(0); 
  }
  cout<< "m_sim_cycle : " <<m_sim_cycle <<endl;
  m_sim_cycle++;
}

void DNNSim::read_complete_callback(unsigned id, uint64_t address, uint64_t clock_cycle){
  num_mem_callback_++;

  switch (sim_state_) {
    case MEM_GB:
      GB_->pushReqWr(address);
      //        cout << "cycle: " << m_sim_cycle<< " reciv MEM, MEM->GB " << num_mem_callback_ << endl;
      break;
    case MEM_FB:
      // cout << "cycle: " << m_sim_cycle<< " reciv MEM, MEM->FB " << num_mem_callback_ << endl;
      FB_->pushReqWr(address);
      break;
    case MEM_FB_PART:
      FB_->pushReqWr(address);
      break;

    case MEM_WB:
      WB_->pushReqWr(address);
      break;


    default:

      break;
  }

}

void DNNSim::write_complete_callback(unsigned id, uint64_t address, uint64_t clock_cycle){

  cout << "write complete " <<endl;
}


void DNNSim::static_simulation() {
  /*
  // Loading filters from memory to SBs
  uint32_t DRAM_rd_filter_load_num = sim_config_->total_SB_capacity / sim_config_->DRAM_block_size;  
  uint64_t DRAM_rd_filter_load_cycle = DRAM_rd_filter_load_num * sim_config_->DRAM_access_cycle;


  // Loading a part of input neuron to PEs
  uint32_t DRAM_rd_input_a_row_num = (sim_config_->row_PE_num * 2) / sim_config_->DRAM_block_size; // 2B
  uint32_t DRAM_rd_input_load_num = DRAM_rd_input_a_row_num * sim_config_->row_num;
  uint64_t DRAM_rd_input_load_cycle = DRAM_rd_input_load_num * sim_config_->DRAM_access_cycle;


  /// calculate output neurons for loaded input neurons 
  /// curently pipeline is not considered
  uint32_t PE_array_calculate_cycle = net_config_->filter_num * 16; //
  uint32_t DRAM_rd_next_input_load_num = (sim_config_->row_num / net_config_->filter_size_x * net_config_->stride )* DRAM_rd_input_a_row_num;
  uint64_t DRAM_rd_next_input_load_cycle = DRAM_rd_next_input_load_num * sim_config_->DRAM_access_cycle;


  // iteration number
  uint32_t conv_slide_num = (net_config_->input_layer_x / net_config_->stride) * (net_config_->input_layer_x/net_config_->stride);


  // total cycle
  uint64_t total_cycle = DRAM_rd_filter_load_cycle + DRAM_rd_next_input_load_cycle; 
  */
}



bool DNNSim::isEndX() {
  if ( x_dir_ ) { // x is in up direction
    if (count_x_ == output_layer_x_) {
      return true;
    }
  }
  else { // x is in down direction
    if (count_x_ == 1) {
      return true;
    }
  }
  return false;
}

void DNNSim::moveX() {
  if ( x_dir_ ) { // x is in up direction
    ++count_x_;
  }
  else { // x is in down direction
    --count_x_;

  }
}


void DNNSim::calcAccess() {
  global_ctrl_->calcAccess();

  output_->total_cycle = m_sim_cycle;

  output_->num_acc_MEM_ = num_mem_request_;

  output_->num_rd_GB_ = GB_->getRdNum(); 
  output_->num_wr_GB_ = GB_->getWrNum(); 

  output_->num_acc_WB_ = WB_->getAccNum();

  output_->num_acc_FB_ = FB_->getAccNum();

  output_->num_acc_PU_ = PU_->getAccNum() * sim_config_->div_num;

  // for PU_RUN pipeline

  output_->num_acc_WB_ += global_ctrl_->num_acc_WB_;

  uint64_t PU_1_ = 
      (sim_config_->row_PU_num / sim_config_->div_num) * 
      sim_config_->row_num *
      global_ctrl_->num_acc_PU_1_;

  uint64_t PU_2_ = 
      sim_config_->div_num * 
      global_ctrl_->num_acc_PU_2_;

  uint64_t PU_3_ = 
      global_ctrl_->num_acc_PU_3_;

  output_->num_acc_PU_ += (PU_1_ + PU_2_ + PU_3_);

  output_->num_acc_ctrl_ = global_ctrl_->num_acc_ctrl_ ;
  output_->num_acc_net_ = global_ctrl_->num_acc_net_ ;





}

