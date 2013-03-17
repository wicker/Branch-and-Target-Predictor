#!/usr/bin/python
from itertools import *
import os
from decimal import *

lset = ["0","1","2","3","4","5","6","7"]
gset = ["0","1","2","3"]
cset = ["0","1","2","3"]
traces = ["INT", "FP", "MM", "SERV"]
#lcombo = permutations(lset,2)
#os.system("echo > salt.log")

def avg_tmp(filen):
	f = open(filen, 'r+')
	vals = f.readlines()
	count = 0
	total = 0
	for i in vals:
		val = i.rstrip() 
		if val:
			total = total + Decimal(val)
			count = count + 1
	f.close()
	return (total / count)

for i in ("test_values","BASELINE_values"):
	print avg_tmp(i)

def run():
	for g,l,c in [(g,l,c) for g in gset for l in lset for c in cset]:
		local_cmd = "sed -i 's/LOCAL_SALT 0x[0-7]*./LOCAL_SALT 0x"+l+"/g' predictor.h"
		choice_cmd = "sed -i 's/CHOICE_SALT 0x[0-3]*./CHOICE_SALT 0x"+c+"/g' predictor.h"
		global_cmd = "sed -i 's/GLOBAL_SALT 0x[0-3]*./GLOBAL_SALT 0x"+g+"/g' predictor.h"
		os.system(local_cmd)
		os.system(choice_cmd)
		os.system(global_cmd)
		os.system("make")
		os.system("echo > tmp")
		for t in traces:
			print "Running "+t+" for " + g,l,c
			#pred_cmd = "./predictor traces/DIST-"+t+"-1 | grep cc_b | awk -F '=' '{ print $2 }' | sed 's/ *[^ ]* //' >> tmp"
			pred_cmd = "./predictor traces/DIST-"+t+"-1 | grep cc_b | awk -F '=' {'print $2'} | awk -F ' ' {'print $1'} | grep -v '^$' >> tmp"
			os.system(pred_cmd)
		fd = open("salt.log",'a')
		log = "(%s,%s,%s) : %s\n" % (g,l,c,str(avg_tmp()))
		fd.write(log)
		fd.close()		
