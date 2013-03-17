#!/usr/bin/python
from itertools import *
import os
from decimal import *

c = ["1","2","3","4","5"]
traces = ["FP", "INT", "MM", "SERV"]
os.system("echo > run_all_output")

def avg(filen):
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
	if not count:
		return 0
	else: 
		return (total / count)
																			
for t,i in [(t,i) for t in traces for i in c]:
	echo = "echo %s-%s >> run_all_output" %(t, i)
	os.system(echo)
	pred_cmd = "./predictor /home/solaris/faustm/ILPtraces/DIST-%s-%s >> run_all_output" %(t, i)
	os.system(pred_cmd)

for i in ["wrong_cc", "wrong_t"]:
	cmd = "cat run_all_output | grep %s | awk -F'=' {'print $2'} > tmp" %(i)
	os.system(cmd)
	result = avg("tmp")
	output = "%s: %s\n" %(i, result)
	f = open("test_results", 'a')
	f.write(output)
	f.close()

cmd = "cat run_all_output >> compiled_output"
os.system(cmd)
