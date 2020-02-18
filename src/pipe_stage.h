#ifndef __SRC_PIPE_STAGE_H__
#define __SRC_PIPE_STAGE_H__
#include "stdio.h"
#include <queue>
#include <vector>
#include <iostream>
#include <string>
#include "common.h"

using namespace std;

class PipeOpInfo {
 public:
  PipeOpInfo();
  PipeOpInfo( int z, int bit_pos);

  int z_;
  int bit_pos_;
  
  void setInfo(int z, int bit_pos);
  void resetInfo();
  
};

class PipeOp {
 public:
  PipeOp();
  PipeOp(unsigned info_num);
  ~PipeOp();
  

  void setOpInfo(int idx, int z, int bit_pos);
  void resetInfo(unsigned idx);

  bool isValid(unsigned idx);
  bool isValidAny();
  void setValid(unsigned idx);

  PipeOpInfo * getPipeOpInfo(int pos);

  void printPipeOp(unsigned idx);

  
  vector<PipeOpInfo > pipe_op_info_;
  vector<bool> valid_;
  
};

typedef queue<PipeOp *> PipePort;


class PipeStage {

 public:
  PipeStage(PipePort *in_port, PipePort *out_port, 
            unsigned num_pipeline_stages, unsigned div_num);


  virtual ~PipeStage();
  virtual void cycle()=0;
  void resetPipeline(unsigned idx);

  void print_stage(unsigned idx);
  virtual void countAccNum()=0;

  // In/Out port shared between stages
  PipePort *in_port_;
  PipePort *out_port_;
  
  // Internal pipe stages
  unsigned num_pipeline_stages_;
  PipeOp **internal_pipeline_;
  unsigned div_num_;
  
  // for output
  vector<uint64_t> acc_num_;

};

class SBsPipeStage : public PipeStage {
 public:
  SBsPipeStage(PipePort *in_port, PipePort *out_port, 
            unsigned num_pipeline_stages, unsigned div_num);

  virtual ~SBsPipeStage();
  
  virtual void cycle();
  virtual void countAccNum();

  void popInPort();

};

class ProviderPipeStage : public PipeStage {
 public:
  ProviderPipeStage(PipePort *in_port, PipePort *out_port, 
            unsigned num_pipeline_stages, unsigned div_num);

  virtual ~ProviderPipeStage();
  
  virtual void cycle();
  virtual void countAccNum();


};



class PEsPipeStage : public PipeStage {
 public:
  PEsPipeStage(PipePort *in_port, PipePort *out_port, 
            unsigned num_pipeline_stages, unsigned div_num);

  virtual ~PEsPipeStage();
  
  virtual void cycle();
  virtual void countAccNum();
  
};


class CtrlsPipeStage : public PipeStage {
 public:
  CtrlsPipeStage(PipePort *in_port, PipePort *out_port, 
            unsigned num_pipeline_stages, unsigned div_num);

  virtual ~CtrlsPipeStage();
  
  virtual void cycle();
  virtual void countAccNum();

  void set_end_condition_test(int num);
  bool is_end();

  bool end_flag_;
  
  int end_condition_test_;
  int out_count_;

};



#endif

