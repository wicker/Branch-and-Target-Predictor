#include <cstdio>
#include <cstdlib>
#include "tread.h"

// include and define the predictor
#include "predictor.h"
PREDICTOR predictor;

int main(int argc, char ** argv) {
	
	branch_record_c br;
	uint32_t true_addr;
	bool taken;
	
	br.init();
	
	char fName[] = "test_trace";
	
	fd = fopen(fName,'r');
	while (fd->peek() != EOF) {
		fscanf("%X %X %d %d %d %d %d %X", br.instruction_addr, br.instruction_next_addr, br.is_indirect, br.is_conditional, br.is_call, br.is_return, taken, true_addr);
	}
	
	
	return 0;
}