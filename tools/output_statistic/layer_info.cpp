#include "layer_info.h"

LayerInfo::LayerInfo() {

  component_cycle.resize(EOP, 0);
  component_cycle_ratio.resize(EOP, 0);
  cycle_sum = 0;
  ratio_sum = 0;

  energy.resize(E_EOP,0);
  energy_ratio.resize(E_EOP,0);
  energy_sum = 0;
  energy_ratio_sum = 0;

  power_sum = 0;

}
