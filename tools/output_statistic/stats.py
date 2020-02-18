#!/usr/bin/python

out_dir = "../../OUTPUT/"

#tests = ['baseline', 'compend', 'exact', 'pred']
tests = ['baseline']
#tests = ['pred_0', 'pred_3', 'pred_6', 'pred_9', 'pred_12', 'pred_15', 'pred_18', 'pred_21', 'pred_24', 'pred_27', 'pred_30', 'pred_33']
#tests += ['ratio_0.5', 'ratio_0.502', 'ratio_0.504', 'ratio_0.506', 'ratio_0.508', 'ratio_0.51', 'ratio_0.512', 'ratio_0.514', 'ratio_0.516', 'ratio_0.518', 'ratio_0.52', 'ratio_1.0']

layers = range(1, 14)
#layers = range(2, 14)

for tt in tests:
  outfile = open("stat_%s.txt" % (tt), 'w')
  outfile.write("layer\t")
  outfile.write("MEM_GB\tGB_WB\tMEM_WB\tMEM_FB\tFB_PU\tRUN_PU\ttotal_cycle\t")
  outfile.write("d_GB\td_FB\td_WB\td_PU\td_NET\td_CTRL\ttotal_dyn\t")
  outfile.write("s_GB\ts_FB\td_WB\td_PU\td_NET\td_CTRL\ttotal_static\ttotal_energy")
  outfile.write("\n")
  
  for ll in layers:
    c = [0 for x in range(10)]
    e = [0 for x in range(12)]

#    for img in range(1000):
    for img in range(1):
      filename = "%s_image_%d_layer_%02d" % (tt, img, ll)
  
      out_path = out_dir + tt + '/' +  filename
  
      f = open(out_path, 'r')
      cycles = f.readline().split()
      cycles = map(int, cycles)
      energy = f.readline().split()
      energy = map(float, energy)

      c = [x+y for x,y in zip(c, cycles)]
      e = [x+y for x,y in zip(e, energy)]

    cycles = c
    energy = e

#    cycles = [x/1000. for x in cycles]
#    energy = [x/1000. for x in energy]

    outfile.write("%d\t" % (ll))
    outfile.write("%d\t%f\t%f\t%f\t%f\t%f\t%f\t" % (cycles[1], cycles[2]+cycles[6], cycles[9], cycles[3]+cycles[7], cycles[4]+cycles[5], cycles[8], sum(cycles)) )
    dyn = 0.
    for ii in range(0, 6):
      outfile.write("%f\t" % (energy[ii]))
      dyn += energy[ii]
    outfile.write("%f\t" % (dyn))
    static = 0.
    for ii in range(6, 12):
      outfile.write("%f\t" % (energy[ii]))
      static += energy[ii]
    outfile.write("%f\t%f\n" % (static, static+dyn))

  outfile.close()

