#include "predictor.h"

/*
Predict branch from branch_record_c 
Store PC as index in class data structures
*/
bool PREDICTOR::get_prediction(const branch_record_c* br, const op_state_c* os, uint *predicted_target_address)
{
  pc_index = br->instruction_addr & B10MASK;
  prediction = TAKEN;
  if (choice_pred[mask_path_history()] & LOCAL_CHOICE)
  {
	  prediction = local_pred[mask_local_history()] >> LOCAL_SHIFT;
  }
  else
  {
		 prediction = global_pred[mask_path_history()] >> GLOBAL_SHIFT;
  }
  return (bool)prediction;
}

/*
Update the predictor after a prediction has been made.  This should accept 
the branch record (br) and architectural state (os), as well as a third
argument (taken) indicating whether or not the branch was taken.
*/
void PREDICTOR::update_predictor(const branch_record_c* br, const op_state_c* os, bool taken, uint actual_target_address)
{
	int mod;
  uint8_t actual, predicted, local, global, test;

  actual = uint8_t(taken);
  predicted = PREDICTOR::prediction;
  local = (local_pred[mask_local_history()] >> LOCAL_SHIFT) & 0x1;
  global = (global_pred[mask_path_history()] >> GLOBAL_SHIFT) & 0x1;

  test = ((actual << 3) | (predicted << 2) | (local << 1) | global);

  // switch on state of branch result with prediction and saturation
  // counters : state machine
  switch(test)
  {
    case 0x1: // increment
    case 0x5:
    case 0xA:
    case 0xE:
        mod = 1;
        break;
    case 0x2:
    case 0x6:
    case 0x9:
    case 0xD:
        mod = -1;
        break;
    case 0x0:
    case 0x7:
    case 0x8:
    case 0xF:
        mod = 0;
        break;
    default:
        printf("you suck at programming: %X\n",test);
        printf("actual: %X - prediction: %X - local: %X - global: %X\n",actual,prediction,local,global);
        // Since reaching default is the result of an invalid 
        // prediction state, we use cin.get to halt execution
        std::cin.get();
  }

  // Update saturation counters
  twobit_saturation(&choice_pred[path_history], mod);
  
  mod = actual?1:-1;

  twobit_saturation(&global_pred[mask_path_history()], mod);
  threebit_saturation(&local_pred[mask_local_history()], mod);
 
  // Update history 
  update_history(&path_history, actual);
  path_history = mask_path_history();
  update_history(&local_history[pc_index], actual);
  local_history[pc_index] = mask_local_history();

} 

/*
return masked value of path_history
*/
uint16_t PREDICTOR::mask_path_history(){
	return (path_history & B12MASK);
}

/*
return masked value of local_history index by masked PC
*/
uint16_t PREDICTOR::mask_local_history(){
	return (local_history[pc_index] & B10MASK);
}


PREDICTOR::PREDICTOR() {
	for (int i = ONEK/2; i < ONEK; i++) { 
		local_pred[i] = LOCAL_SALT_UP;
	}
	for (int i = 0; i < ONEK/2; i++) {
		local_pred[i] = LOCAL_SALT_LO;
	}
	for (int i = 0; i < FOURK; i++) {
		global_pred[i] = GLOBAL_SALT;
	}
}


/*
Update the global, local and choice saturation counters.
Accessed through macros for 2 bit and 3 bit saturation.
*/
void saturation(int length, uint8_t *targ, int mod)
{
  int target = (int)*targ;
  int max;
  int mask;

  if (length == 2) 
  {
    max = 3;
    mask = B2MASK;
  }
  else
  {
    max = 7;
    mask = B3MASK;
  }

  // Check for saturated value and set mod value to 0 if found
  if ((mod > 0 && target == max) || (mod < 0 && !target))
  {
    mod = 0;
  }
  target = target + mod;
  *targ = ((uint8_t)(target & mask));
}

/*
update_history shifts the actual branch result into the LSB
of a pointer to either local_history or path_history
*/
void update_history(uint16_t *history, int actual)
{
  *history = (*history << 1) + actual;


void PREDICTOR::update_lru(uint8_t way){
	
}
