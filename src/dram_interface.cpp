#include "dram_interface.h"

dram_interface::dram_interface(const std::string& dram_config_file, 
                                const std::string& system_config, 
                                const std::string& dram_sim_dir,
                                const std::string& prog_name,
                                unsigned memory_size,
                                std::string image_name,
                                int frequency ){

    
    m_dram_sim = DRAMSim::getMemorySystemInstance(dram_config_file, 
                                            system_config, 
                                            dram_sim_dir, 
                                            prog_name, 
                                            memory_size);
    m_dram_sim->setLogFileName(image_name);
    
    //m_dram_sim->setCPUClockSpeed(0);
    m_dram_sim->setCPUClockSpeed(frequency * 1000000);


}

dram_interface::~dram_interface(){
   delete m_dram_sim; 
}

void dram_interface::dram_interface::cycle(){
    m_dram_sim->update();
}

bool dram_interface::can_accept_request() const {
    return m_dram_sim->willAcceptTransaction();
}

void dram_interface::push_request(uint64_t addr, bool is_write){
    m_dram_sim->addTransaction(is_write, addr);
}

void dram_interface::set_callbacks(DRAMSim::TransactionCompleteCB *read_callback,
                   DRAMSim::TransactionCompleteCB *write_callback){
     m_dram_sim->RegisterCallbacks(read_callback, write_callback, NULL);
}
