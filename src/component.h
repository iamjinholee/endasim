#ifndef __SRC_COMPONENT_H__
#define __SRC_COMPONENT_H__
#include <list>
#include <vector>
#include "common.h"

using namespace std;

class ReqEntry {
 public:
  ReqEntry() {
    req_type_ = 0;
    address_ = 0;
  }

  ReqEntry(int type, int address) {
    req_type_ = type;
    address_ = address;
  }

  int req_type_; // 0: read, 1: wrtie
  uint64_t address_;
};


class Component {
 public:
  Component(){
    num_rd_ = 0;
    num_wr_ = 0;
  }
  ~Component(){}

  virtual void setPtrNext(Component *ptr_next)=0;

  virtual void pushReq(ReqEntry entry)=0;
  virtual void pushReqWr(uint64_t address)=0;
  virtual void pushReqRd(uint64_t address)=0;

  virtual uint64_t getWrAccessCounter()=0;
  virtual void resetWrAccessCounter()=0;

  virtual void cycle()=0;


  uint64_t num_rd_;
  uint64_t num_wr_;
};



class BufferGroup;
class Buffer: public Component {
public:

  Buffer(int bank_num, int rd_latency, int wr_latency);
  Buffer(int bank_num, int latency): Buffer(bank_num, latency, latency){}
  ~Buffer(){};

  class Bank {
   public :
    Bank() {
      is_busy_ = false;
      access_start_time_ = 0;
    }

    bool is_busy_; 
    uint64_t access_start_time_;
    ReqEntry entry_;
  };

  void setPtrNext(Component *ptr_next) {
    ptr_next_ = ptr_next;
  };

  uint64_t getWrAccessCounter() {
    return wr_access_counter_;
  }

  void resetWrAccessCounter() {
    wr_access_counter_ = 0;
  }

  void pushReq(ReqEntry entry);
  void pushReqWr(uint64_t address);
  void pushReqRd(uint64_t address);

  virtual void cycle();


  uint64_t clock_;
  
  int bank_num_;
  int rd_latency_;
  int wr_latency_;
  
  uint64_t wr_access_counter_;

  vector<Bank > banks;
  list<ReqEntry > input_queue;
  list<ReqEntry > output_queue;

  Component *ptr_next_;

};


class PUnitRow : public Component {
public:

  PUnitRow(int PU_num, int latency);
  ~PUnitRow(){}

  class PUnit {
   public :
    PUnit() {
      is_busy_ = false;
      access_start_time_ = 0;
    }

    bool is_busy_; 
    uint64_t access_start_time_;
    ReqEntry entry_;
  };

  void setPtrNext(Component *ptr_next) {};

  uint64_t getWrAccessCounter() {
    return wr_access_counter_;
  }

  void resetWrAccessCounter() {
    wr_access_counter_ = 0;
  }

  void pushReq(ReqEntry entry);
  void pushReqWr(uint64_t address);
  void pushReqRd(uint64_t address);

  virtual void cycle();


  uint64_t clock_;
  
  int PU_num_;
  int latency_;
  
  uint64_t wr_access_counter_;

  vector<PUnit> PUs;

  list<ReqEntry > input_queue;
  list<ReqEntry > output_queue;

};



class BufferGroup : public Component {
 public:
  BufferGroup(int num, int bank_num, int rd_latency, int wr_latency);

  BufferGroup(int num, int bank_num, int latency) 
      : BufferGroup(num, bank_num, latency, latency) {}


  void setPtrNext(Component *ptr_next);

  void pushReq(ReqEntry entry);
  void pushReqWr(uint64_t address);
  void pushReqRd(uint64_t address);

  uint64_t getWrAccessCounter();
  void resetWrAccessCounter();


  uint64_t getRdNum();
  uint64_t getWrNum();
  uint64_t getAccNum();



  void cycle();

  vector<Buffer > buffer;
  
  int num_;

};

class WeightBufferGroup : public BufferGroup {
 public:
  WeightBufferGroup(int num, int bank_num, int latency) 
      : BufferGroup(num, bank_num, latency, latency) 
  {
    fblock_id_=0;
  }


  void set_cur_fblock_id(int fblock_id) {
    fblock_id_ = fblock_id;
  };

  int get_cur_fblock_id() {
    return fblock_id_;
  };

  int fblock_id_; 
};


class PUnitArray : public Component {
 public:

  PUnitArray(int row_num, int PU_num, int latency);

  void setPtrNext(Component *ptr_next) {
  };


  void pushReq(ReqEntry entry);
  void pushReqWr(uint64_t address);
  void pushReqRd(uint64_t address);

  uint64_t getWrAccessCounter();
  void resetWrAccessCounter();

  uint64_t getRdNum();
  uint64_t getWrNum();
  uint64_t getAccNum();



  void cycle();

  vector<PUnitRow> PU_rows;
  
  int row_num_;

};


#endif
