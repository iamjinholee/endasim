#!/usr/bin/python

img_dir = "../../IMAGES_INFO/"

#tests = ['compend', 'exact', 'pred']
#tests = ['all_8']
#tests = ['0_2.2', '0_2.6']
#tests = ['ratio_0.51']
tests = ['0_ratio_0.51_fix']
layers = range(2, 14)

for tt in tests:
  imgfile = open("stats_skip_%s.txt" % (tt), 'w')
  imgfile.write("layer\tcalculated\ttotal\tratio")
  imgfile.write("\n")
  cb = 0
  tb = 0
  for ll in layers:
    filename = "image_%s_layer_%02d" % (tt, ll)

    img_path = img_dir + filename

    f = open(img_path, 'r')
    idim = f.readline()
    odim = f.readline()
    wdim = f.readline()
    bits = f.readline().split()
    bits = map(int, bits)

    calc_bits = sum(bits)
    total_bits = len(bits) * 16

    cb += calc_bits
    tb += total_bits

    imgfile.write("%d\t" % (ll))
    imgfile.write("%d\t%d\t%f\n" % (calc_bits, total_bits, float(calc_bits)/float(total_bits)) )

  imgfile.write("sum\t")
  imgfile.write("%d\t%d\t%f\n" % (cb, tb, float(cb)/float(tb)) )
  
  imgfile.close()

