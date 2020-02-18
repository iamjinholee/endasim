#ifndef __DRAM_H__
#define __DRAM_H__


#include <DRAMSim.h>
#include <string>

/*
 *   DRAM interface using DRAMSim2
 */
class dram_interface {
public:
    
    dram_interface(const std::string& dram_config_file,
                    const std::string& system_config,
                    const std::string& dram_sim_dir,
                    const std::string& prog_name,
                    unsigned memory_size,
                    std::string image_name,
                    int frequency);

    ~dram_interface();

    void cycle();
    bool can_accept_request() const;
    void push_request(uint64_t addr, bool is_write);

    void set_callbacks(DRAMSim::TransactionCompleteCB *read_callback, 
                       DRAMSim::TransactionCompleteCB *write_callback);

private:

    DRAMSim::MultiChannelMemorySystem *m_dram_sim;

};



#endif
