#include "predictor.h"

bool PREDICTOR::get_prediction(const branch_record_c* br, const op_state_c* os, uint *predicted_target_address)
{

  printf("%X %X %d %d %d %d ", br->instruction_addr, br->instruction_next_addr, br->is_indirect, br->is_conditional, br->is_call, br->is_return);

  bool prediction = true;
  if (br->is_conditional)
    prediction = false;
  return prediction;   // true for taken, false for not taken

}

// Update the predictor after a prediction has been made.  This should accept
// the branch record (br) and architectural state (os), as well as a third
// argument (taken) indicating whether or not the branch was taken.

void PREDICTOR::update_predictor(const branch_record_c* br, const op_state_c* os, bool taken, uint actual_target_address)
{
	printf("%d %X\n", taken, actual_target_address);
}
