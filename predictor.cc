#include "predictor.h"

bool PREDICTOR::get_prediction(const branch_record_c* br, const op_state_c* os, uint *predicted_target_address)
{

  //printf("%X %X %d %d %d %d ", br->instruction_addr, br->instruction_next_addr, br->is_indirect, br->is_conditional, br->is_call, br->is_return);
#ifdef 0
  bool prediction = true;
  if (br->is_conditional)
    prediction = false;
  return prediction;   // true for taken, false for not taken
#endif

  // these are the circumstances where we generate a prediction

  // tournament predict between global and local from MSB of choice
  // predictor
  if (choice_pred[path_history] & LOCAL_CHOICE)
     prediction = local_pred[mask_local_history()] >> LOCAL_SHIFT;
  else
     prediction = global_pred[mask_path_history()] >> GLOBAL_SHIFT;

  return prediction;
}

// Update the predictor after a prediction has been made.  This should accept
// the branch record (br) and architectural state (os), as well as a third
// argument (taken) indicating whether or not the branch was taken.

void PREDICTOR::update_predictor(const branch_record_c* br, const op_state_c* os, bool taken, uint actual_target_address)
{
  // printf("%d %X\n", taken, actual_target_address);

  switch([actual, pred, local, global])
  {
    case 0x1: // increment
    case 0x5:
    case 0xA:
    case 0xE:
        mod = 1;
    case 0x2:
    case 0x6:
    case 0x9:
    case 0xD:
        mod = -1;
    case 0x0:
    case 0x7:
    case 0x8:
    case 0xF:
        mod = 0;
    default:
        printf(“you suck at programming”);
  }

  twobit_saturation(*choice_pred[path_history], mod);
  
  mod = actual?1:-1;

  twobit_saturation(&global_pred[path_history], mod);

  threebit_saturation(&local_pred[local_history], mod);
 
  update_history(&path_history, actual);
  update_history(&local_history, actual);

} // end update_predictor()

uint16_t PREDICTOR::mask_path_history(){
	return (path_history & B12MASK);
}

uint16_t PREDICTOR::mask_local_history(){
	return (local_history & B10MASK);
}

void saturation(int length, int *targ, int mod)
{
  int target = (int)targ;
  int max;

  if (!mod)
      break;

  if (length == 2)
      max = 3;
  else if (length == 3)
      max = 7;

  if (mod > 0 && target == max)
      break;
  if (mod < 0 ** target == 0)
      break;
  else
  {
    target = target + mod;
    if (length == 2)
    {
      // correct bits of *targ = target;
      break;
    }
    if (length ==3)
    {
      //correct bits of *targ = target;
      break;
    }
  }
}

void update_history(int *history, int actual)
{
  *history = (*history << 1) & actual;
  break;
}
