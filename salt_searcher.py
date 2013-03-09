#!/usr/bin/python
from itertools import *
import os
from decimal import *

lset = ["0","1","2","3","4","5","6","7"]
gset = ["0","1","2","3"]
traces = ["INT", "FP", "MM", "SERV"]
lcombo = permutations(lset,2)
os.system("echo > salt.log")

def avg_tmp():
	f = open("tmp", 'r+')
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


for g,ll,lu in [(g,ll,lu) for g in gset for ll in lset for lu in lset]:
	local_lo_cmd = "sed -i 's/LOCAL_SALT_LO 0x[0-7]*./LOCAL_SALT_LO 0x"+ll+"/g' predictor.h"
	local_up_cmd = "sed -i 's/LOCAL_SALT_UP 0x[0-7]*./LOCAL_SALT_UP 0x"+lu+"/g' predictor.h"
	global_cmd = "sed -i 's/GLOBAL_SALT 0x[0-7]*./GLOBAL_SALT 0x"+g+"/g' predictor.h"
	os.system(local_lo_cmd)
	os.system(local_up_cmd)
	os.system(global_cmd)
	os.system("make")
	os.system("echo > tmp")
	for t in traces:
		print "Running "+t+" for " + g,ll,lu
		pred_cmd = "./predictor traces/DIST-"+t+"-1 | grep cc_b | awk -F '=' '{ print $2 }' | sed 's/ *[^ ]* //' >> tmp"
		os.system(pred_cmd)
	fd = open("salt.log",'a')
	log = "(%s,%s,%s) : %s\n" % (g,ll,lu,str(avg_tmp()))
	fd.write(log)
	fd.close()		
