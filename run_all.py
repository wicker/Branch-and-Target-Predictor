#!/usr/bin/python
from itertools import *
import os
from decimal import *

cset = ["1","2","3","4","5"]
traces = ["FP", "INT", "MM", "SERV"]
os.system("echo > run_all_output")

for t,i in [(t,i) for t in traces for i in cset]:
	echo = "echo "+t+"-"+i+" >> run_all_output"
	os.system(echo)
	pred_cmd = "./predictor /home/solaris/faustm/ILPtraces/DIST-"+t+"-"+i+" >> run_all_output"
	os.system(pred_cmd)
