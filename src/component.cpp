#include "component.h"


/// Buffer
Buffer::Buffer(int bank_num, int rd_latency, int wr_latency) : Component()
{
  banks.resize(bank_num, Bank());

  bank_num_ = bank_num;
  rd_latency_ = rd_latency;
  wr_latency_ = wr_latency;

  clock_ = 0; 

  wr_access_counter_ = 0;

  ptr_next_ = NULL;
}


void Buffer::pushReq(ReqEntry entry) {
  input_queue.push_back(entry);
}

void Buffer::pushReqWr(uint64_t address) {
  input_queue.push_back(ReqEntry(1, address));
}

void Buffer::pushReqRd(uint64_t address) {
  input_queue.push_back(ReqEntry(0, address));
}



void Buffer::cycle() {
  ++clock_;

  // check input_queue
  // WAR is guaranteed by data flow of the architecture

  // give higher priority to a read req than a write req
  // read req check 
  for (auto it = input_queue.begin(); it != input_queue.end(); ++it) {
    if ( it->req_type_ == 0 ) {
      int idx = (it->address_ / 64) % bank_num_; // TODO: xor hashing?
      if (!banks.at(idx).is_busy_) {
        ++num_rd_; // for output

        banks.at(idx).is_busy_ = true;
        banks.at(idx).access_start_time_ = clock_;
        banks.at(idx).entry_ = (*it);
        input_queue.erase(it--);
      }
    }
  }
  // write req check
  for (auto it = input_queue.begin(); it != input_queue.end(); ++it) {
    if ( it->req_type_ == 1 ) {
      int idx = (it->address_ / 64) % bank_num_; // TODO: xor hashing?
      if (!banks.at(idx).is_busy_) {
        ++num_wr_; // for output

        banks.at(idx).is_busy_ = true;
        banks.at(idx).access_start_time_ = clock_;
        banks.at(idx).entry_ = (*it);
        input_queue.erase(it--);
      }
    }
  }


  // bank process
  for (auto b = banks.begin(); b != banks.end(); ++b) {
    if (b->is_busy_) {
      int lat = (b->entry_.req_type_ == 0)? rd_latency_: wr_latency_;
      if (b->access_start_time_ + lat <= clock_) {
        b->is_busy_ = false;
        b->access_start_time_ = 0;
        if (b->entry_.req_type_ == 0) {
          b->entry_.req_type_ = 1; // wr
          output_queue.push_back(b->entry_);
        }
        else {
          ++wr_access_counter_;
        }
      }
    }
  }

  // check output_queue
  if (!output_queue.empty() ) {
    ReqEntry entry = output_queue.front();
    output_queue.pop_front();

    //cout<<"bf pop - "<< clock_<<  " " << wr_access_counter_<< endl;
    // push next components's input queue
    ptr_next_->pushReq(entry);
  }

}


///
/// PUnitRow 

PUnitRow::PUnitRow(int PU_num, int latency) : Component()
{

  PU_num_ = PU_num;
  PUs.resize(PU_num_, PUnit());

  latency_ = latency;

  clock_ = 0; 
  wr_access_counter_ = 0;
}


void PUnitRow::pushReq(ReqEntry entry) {
  input_queue.push_back(entry);
}

void PUnitRow::pushReqWr(uint64_t address) {
  input_queue.push_back(ReqEntry(1, address));
}

void PUnitRow::pushReqRd(uint64_t address) {
  input_queue.push_back(ReqEntry(0, address));
}


void PUnitRow::cycle() {
  ++clock_;
  // check input_queue
  // WAR is guaranteed by data flow of the architecture

  // give higher priority to a read req than a write req
  // read req check 
  for (auto it = input_queue.begin(); it != input_queue.end(); ++it) {
    if ( it->req_type_ == 0 ) {
      int idx = (it->address_ / 64) % PU_num_; // TODO: need to modify it considering addressing
      if (!PUs.at(idx).is_busy_) {
        ++num_rd_;

        PUs.at(idx).is_busy_ = true;
        PUs.at(idx).access_start_time_ = clock_;
        PUs.at(idx).entry_ = (*it);
        input_queue.erase(it--);
      }
    }
  }
  // write req check
  for (auto it = input_queue.begin(); it != input_queue.end(); ++it) {
    if ( it->req_type_ == 1 ) {
      int idx = (it->address_ / 64) % PU_num_; // TODO: need to modify it considering addressing
      if (!PUs.at(idx).is_busy_) {
        ++num_wr_;

        PUs.at(idx).is_busy_ = true;
        PUs.at(idx).access_start_time_ = clock_;
        PUs.at(idx).entry_ = (*it);
        input_queue.erase(it--);
      }
    }
  }


  // bank process
  for (auto p = PUs.begin(); p != PUs.end(); ++p) {
    if (p->is_busy_) {
      if (p->access_start_time_ + latency_ <= clock_) {
        p->is_busy_ = false;
        p->access_start_time_ = 0;
        if (p->entry_.req_type_ == 0) {
          p->entry_.req_type_ = 1; // wr
          output_queue.push_back(p->entry_);
        }
        else {
          ++wr_access_counter_;
        }
      }
    }
  }

  // check output_queue
  if (!output_queue.empty() ) {
    ReqEntry entry = output_queue.front();
    output_queue.pop_front();
    //cout<<"PU pop - "<< clock_<< endl;

  }

}


///
BufferGroup::BufferGroup(int num, int bank_num, int rd_latency, int wr_latency) : Component()
{

  num_ = num;
  buffer.resize(num_, Buffer(bank_num, rd_latency, wr_latency));
}

void BufferGroup::setPtrNext( Component* ptr_next) {
  for (auto bf = buffer.begin(); bf != buffer.end(); ++bf) {
    bf->setPtrNext(ptr_next);
  }
}

void BufferGroup::pushReq( ReqEntry entry) {
  int idx = (entry.address_ / 64) % num_; // TODO: xor hashing?
  buffer.at(idx).pushReq(entry);
}

void BufferGroup::pushReqWr( uint64_t address) {
  int idx = (address / 64) % num_; // TODO: xor hashing?
  buffer.at(idx).pushReqWr(address);
}

void BufferGroup::pushReqRd( uint64_t address) {
  int idx = (address / 64) % num_; // TODO: xor hashing?
  buffer.at(idx).pushReqRd(address);
}

uint64_t BufferGroup::getWrAccessCounter() {
  uint64_t out = 0;
  for (auto bf = buffer.begin(); bf != buffer.end(); ++bf) {
    out += bf->getWrAccessCounter();
  }
  return out;
}

void BufferGroup::resetWrAccessCounter() {
  for (auto bf = buffer.begin(); bf != buffer.end(); ++bf) {
    bf->resetWrAccessCounter();
  }
}

uint64_t BufferGroup::getRdNum() {
  uint64_t out = 0;
  for (auto bf = buffer.begin(); bf != buffer.end(); ++bf) {
    out += bf->num_rd_;
  }
  return out;
}

uint64_t BufferGroup::getWrNum() {
  uint64_t out = 0;
  for (auto bf = buffer.begin(); bf != buffer.end(); ++bf) {
    out += bf->num_wr_;
  }
  return out;
}


uint64_t BufferGroup::getAccNum() {
  uint64_t out = 0;
  out += getRdNum();
  out += getWrNum();
  return out;
}

void BufferGroup::cycle() {
  for (auto bf = buffer.begin(); bf != buffer.end(); ++bf) {
    bf->cycle();
  }
}



///
PUnitArray::PUnitArray(int row_num, int PU_num, int latency) : Component()
{
  row_num_ = row_num;
  PU_rows.resize(row_num_, PUnitRow(PU_num, latency));
}

void PUnitArray::pushReq( ReqEntry entry) {
  int idx = (entry.address_ / 64) % row_num_; // TODO: need to change
  PU_rows.at(idx).pushReq(entry);
}

void PUnitArray::pushReqWr( uint64_t address) {
  int idx = (address / 64) % row_num_; // TODO: need to change
  PU_rows.at(idx).pushReqWr(address);
}

void PUnitArray::pushReqRd( uint64_t address) {
  int idx = (address / 64) % row_num_; // TODO: need to change
  PU_rows.at(idx).pushReqRd(address);
}

uint64_t PUnitArray::getWrAccessCounter() {
  uint64_t out = 0;
  for (auto r = PU_rows.begin(); r != PU_rows.end(); ++r) {
    out += r->getWrAccessCounter();
  }
  return out;
}

void PUnitArray::resetWrAccessCounter() {
  for (auto r = PU_rows.begin(); r != PU_rows.end(); ++r) {
    r->resetWrAccessCounter();
  }
}


uint64_t PUnitArray::getRdNum() {
  uint64_t out = 0;
  for (auto r = PU_rows.begin(); r != PU_rows.end(); ++r) {
    out += r->num_rd_;
  }
  return out;
}

uint64_t PUnitArray::getWrNum() {
  uint64_t out = 0;
  for (auto r = PU_rows.begin(); r != PU_rows.end(); ++r) {
    out += r->num_wr_;
  }
  return out;
}


uint64_t PUnitArray::getAccNum() {
  uint64_t out = 0;
  out += getRdNum();
  out += getWrNum();
  return out;
}

void PUnitArray::cycle() {
  for (auto r = PU_rows.begin(); r != PU_rows.end(); ++r) {
    r->cycle();
  }
}

