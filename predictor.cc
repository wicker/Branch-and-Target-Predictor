#include "predictor.h"

#define 2b_saturation(*target, mod)    saturation(2, *target, mod)
#define 3b_saturation(*target, mod)    saturation(3, *target, mod)

bool PREDICTOR::get_prediction(const branch_record_c* br, const op_state_c* os, uint *predicted_target_address)
{

  //printf("%X %X %d %d %d %d ", br->instruction_addr, br->instruction_next_addr, br->is_indirect, br->is_conditional, br->is_call, br->is_return);

  bool prediction = true;
  if (br->is_conditional)
    prediction = false;
  return prediction;   // true for taken, false for not taken

  // these are the circumstances where we generate a prediction

  // this is how we predict
  if (choice_pred[path_history] & b'10')
     prediction = local_prediction[local_history] >> 2;
  else
     prediction = global_pred[path_history] >> 1;

}


// Update the predictor after a prediction has been made.  This should accept
// the branch record (br) and architectural state (os), as well as a third
// argument (taken) indicating whether or not the branch was taken.

void PREDICTOR::update_predictor(const branch_record_c* br, const op_state_c* os, bool taken, uint actual_target_address)
{
  // printf("%d %X\n", taken, actual_target_address);

  switch([actual, pred, local, global])
  {
    case 0001: // increment
    case 0101:
    case 1010:
    case 1110:
        mod = 1;
    case 0010:
    case 0110:
    case 1001:
    case 1101:
        mod = -1;
    case 0000:
    case 0111:
    case 1000:
    case 1111:
        mod = 0;
    default:
        printf(“you suck at programming”);
  }

  2b_saturation(*choice_pred[path_history], mod);
  
  mod = actual?1:-1;

  2b_saturation(&global_pred[path_history], mod);

  3b_saturation(&local_pred[local_history], mod);
 
  history_update(&path_history, actual);
  history_update(&local_history, actual);

} // end update_predictor()

