#include <cstdio>
#include <cstdlib>
#include "tread.h"
#include "string.h"
#include <iostream>

using namespace std;

// include and define the predictor
#include "predictor.h"
PREDICTOR predictor;

int main(int argc, char ** argv) {
	
	branch_record_c br;
	uint32_t true_addr;
	bool taken;
        FILE* fd;
	
	br.init();
	
	string fName = "test_trace";

	fd = fopen(fName.c_str(),"r");
	printf("Reading test_trace\n");
	while (!feof(fd)) {
	  fscanf(fd, "%X %X %d %d %d %d %d %X", br.instruction_addr, br.instruction_next_addr, br.is_indirect, br.is_conditional, br.is_call, br.is_return, taken, true_addr);
	}
	printf("trace read complete\n");
	
	return 0;
}
