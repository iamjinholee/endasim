#include "assert.h"
#include "pipe_stage.h"

PipeOpInfo::PipeOpInfo() {
  setInfo(0, 0);
}

PipeOpInfo::PipeOpInfo(int z, int bit_pos) {
  setInfo(z, bit_pos);
}

void PipeOpInfo::setInfo(int z, int bit_pos) {
  z_ = z;
  bit_pos_ = bit_pos; 
}

void PipeOpInfo::resetInfo() {
  setInfo(0, 0);
}
///

PipeOp::PipeOp() {
  pipe_op_info_.resize(1, PipeOpInfo());
  valid_.resize(1);
  //pipe_op_info_.resize(0);
  for (int i=0; i<pipe_op_info_.size(); ++i) {
    resetInfo(i);
  }
}


PipeOp::PipeOp(unsigned info_num) {
  pipe_op_info_.resize(info_num, PipeOpInfo());
  valid_.resize(info_num, false);
}

PipeOp::~PipeOp() {

}
void PipeOp::setOpInfo(int idx, int z, int bit_pos) {
  pipe_op_info_.at(idx).setInfo(z, bit_pos);
}

void PipeOp::resetInfo(unsigned idx) {
  valid_.at(idx) = false;
  pipe_op_info_[idx].resetInfo();
}

bool PipeOp::isValidAny() {
  bool b = false;
  for(int i = 0; i < valid_.size(); i++) {
    b = b || valid_.at(i);
  }
  return b;
}

bool PipeOp::isValid(unsigned idx) {
  return valid_.at(idx);
}

void PipeOp::setValid(unsigned idx) {
  valid_.at(idx) = true;
}

PipeOpInfo* PipeOp::getPipeOpInfo(int idx) {
  assert( idx < pipe_op_info_.size() );
  return &(pipe_op_info_[idx]);
}

void PipeOp::printPipeOp(unsigned idx) {
  //cout << test_ << "\t";
  PipeOpInfo *info = &(pipe_op_info_[idx]);
  //printf("(%d|%3d,%3d,%3d,%3d)\t", valid_, info->x_, info->y_, info->z_, info->bit_pos_);
  bool v = valid_.at(idx);
  printf("(%d|%3d,%3d)\t", v, info->z_, info->bit_pos_);
}


///
PipeStage::PipeStage(PipePort *in_port, PipePort *out_port, 
            unsigned num_pipeline_stages, unsigned div_num)
  : in_port_(in_port), out_port_(out_port), 
    num_pipeline_stages_(num_pipeline_stages),
    div_num_(div_num)
    
{
  internal_pipeline_ = new PipeOp *[num_pipeline_stages];
  for(int i = 0; i < num_pipeline_stages; ++i) {
    internal_pipeline_[i] = new PipeOp(div_num);
  }

  acc_num_.resize(num_pipeline_stages,0);
}


PipeStage::~PipeStage() {
  delete[] internal_pipeline_;
}

void PipeStage::print_stage(unsigned idx) {
  for(int i = 0; i < num_pipeline_stages_; ++i) {
    if( internal_pipeline_[i]->isValid(idx) == false) {
      cout << "NULL\t\t";
    }
    else {
      internal_pipeline_[i]->printPipeOp(idx);
    }
  }
  //cout << endl;
}

void PipeStage::countAccNum() {
  for(int i = 0; i < num_pipeline_stages_; ++i) {
      for (int d = 0; d < div_num_; ++d) {
        if( internal_pipeline_[i]->isValid(d) == true) {
        }
      }
  }
  //cout << endl;
}




void PipeStage::resetPipeline(unsigned idx) {
  for(int i = 0; i < num_pipeline_stages_; ++i) {
    internal_pipeline_[i]->resetInfo(idx);
  }
  //cout << endl;
}


////
SBsPipeStage::SBsPipeStage(PipePort *in_port, PipePort *out_port, 
            unsigned num_pipeline_stages, unsigned div_num)
  : PipeStage(in_port, out_port, num_pipeline_stages, div_num )
{

} 

SBsPipeStage::~SBsPipeStage() {

}

void SBsPipeStage::cycle() {
  PipeOp *op;

  op = internal_pipeline_[num_pipeline_stages_-1]; 
  if(op) {
    //cout << "SBs: push last op to next stage" << endl;
    out_port_->push(op);
  }

  for(int i = num_pipeline_stages_; i > 0; --i) {
    internal_pipeline_[i] = internal_pipeline_[i-1];
  }
  internal_pipeline_[0] = NULL;

  if(!in_port_->empty()) {
    //cout << "SBs: pop first op from previous stage" << endl;
    op = in_port_->front();
    in_port_->pop();
    internal_pipeline_[0] = op;
  }
}



void SBsPipeStage::popInPort() {
  PipeOp *op;
  if(!in_port_->empty()) {
    //cout << "PEs: pop first op from previous stage" << endl;
    op = in_port_->front();
    in_port_->pop();
    internal_pipeline_[0] = op;
  }
}


void SBsPipeStage::countAccNum() {

    for(int i = 0; i < num_pipeline_stages_; ++i) {
      bool isWB = false;
      for (int d = 0; d < div_num_; ++d) {
        if( internal_pipeline_[i]->isValid(d) == true) {
          isWB = isWB || true;
        }
      }
      if (isWB) {
        ++acc_num_.at(i);
      }
    }

}


////
ProviderPipeStage::ProviderPipeStage(PipePort *in_port, PipePort *out_port, 
            unsigned num_pipeline_stages, unsigned div_num)
  : PipeStage(in_port, out_port, num_pipeline_stages, div_num)
{

} 

ProviderPipeStage::~ProviderPipeStage() {

}

void ProviderPipeStage::cycle() {
  PipeOp *op;

  op = internal_pipeline_[num_pipeline_stages_-1]; 
  if(op) {
   // cout << "Provider: push last op to next stage" << endl;
    out_port_->push(op);
  }

  for(int i = num_pipeline_stages_-1; i > 0; --i) {
    internal_pipeline_[i] = internal_pipeline_[i-1];
  }
  internal_pipeline_[0] = NULL;

  if(!in_port_->empty()) {
    //cout << "Provider: pop first op from previous stage" << endl;
    op = in_port_->front();
    in_port_->pop();
    internal_pipeline_[0] = op;
  }

}

void ProviderPipeStage::countAccNum() {

    for(int i = 0; i < num_pipeline_stages_; ++i) {
      bool isPV = false;
      for (int d = 0; d < div_num_; ++d) {
        if( internal_pipeline_[i]->isValid(d) == true) {
          isPV = isPV || true;
        }
      }
      if (isPV) {
        ++acc_num_.at(i);
      }
    }

}
////
PEsPipeStage::PEsPipeStage(PipePort *in_port, PipePort *out_port, 
            unsigned num_pipeline_stages, unsigned div_num)
  : PipeStage(in_port, out_port, num_pipeline_stages, div_num)
{

} 

PEsPipeStage::~PEsPipeStage() {

}

void PEsPipeStage::cycle() {
  PipeOp *op;

  op = internal_pipeline_[num_pipeline_stages_-1]; 
  if(op) {
    //cout << "PEs: push last op to next stage" << endl;
    out_port_->push(op);
  }

  for(int i = num_pipeline_stages_-1; i > 0; --i) {
    internal_pipeline_[i] = internal_pipeline_[i-1];
  }
  internal_pipeline_[0] = NULL;

  if(!in_port_->empty()) {
    //cout << "PEs: pop first op from previous stage" << endl;
    op = in_port_->front();
    in_port_->pop();
    internal_pipeline_[0] = op;
  }

}

void PEsPipeStage::countAccNum() {

  // staeg 1
  for (int d = 0; d < div_num_; ++d) {
    if( internal_pipeline_[0]->isValid(d) == true) {
      ++acc_num_.at(0);
    }
  }
  // stage 2
  for (int d = 0; d < div_num_; ++d) {
    if( internal_pipeline_[1]->isValid(d) == true) {
      ++acc_num_.at(1);
    }
  }

  // stage 3

  bool isPV = false;
  for (int d = 0; d < div_num_; ++d) {
    if( internal_pipeline_[2]->isValid(d) == true) {
      isPV = isPV || true;
    }
  }
  if (isPV) {
    ++acc_num_.at(2);
  }

  //cout << endl;
}


////
CtrlsPipeStage::CtrlsPipeStage(PipePort *in_port, PipePort *out_port, 
            unsigned num_pipeline_stages, unsigned div_num)
  : PipeStage(in_port, out_port, num_pipeline_stages, div_num)
{
  end_flag_= false;
} 

CtrlsPipeStage::~CtrlsPipeStage() {

}

void CtrlsPipeStage::cycle() {
  PipeOp *op;

  op = internal_pipeline_[num_pipeline_stages_-1]; 
  if(op) {
  //  cout << "Ctrls: push last op to next stage" << endl;
    out_port_->push(op);
  }

  for(int i = num_pipeline_stages_-1; i > 0; --i) {
    internal_pipeline_[i] = internal_pipeline_[i-1];
  }
  internal_pipeline_[0] = NULL;

  if(!in_port_->empty()) {
//    cout << "Ctrls: pop first op from previous stage" << endl;
    op = in_port_->front();
    in_port_->pop();
    internal_pipeline_[0] = op;
  }

}


void CtrlsPipeStage::set_end_condition_test(int num) {
  end_condition_test_ = num;

}

bool CtrlsPipeStage::is_end() {
  return end_flag_;
}

void CtrlsPipeStage::countAccNum() {

  for (int d = 0; d < div_num_; ++d) {
    if( internal_pipeline_[0]->isValid(d) == true) {
      ++acc_num_.at(0);
    }
  }

}
