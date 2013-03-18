#include "predictor.h"

/*
Predict branch from branch_record_c 
Store PC as index in class data structures
*/
bool PREDICTOR::get_prediction(const branch_record_c* br, const op_state_c* os, uint *predicted_target_address)
{
  pc_index = br->instruction_addr & B10MASK;
  target_index = pc_index >> T_INDEX_SHIFT;
  *predicted_target_address = 0;
  if (!br->is_indirect) {
    if (choice_pred[mask_path_history()] & LOCAL_CHOICE)
    {
      prediction = (local_pred[mask_local_history()] & B3MASK) >> LOCAL_SHIFT;
    }
    else
    {
 	   prediction = (global_pred[mask_path_history()] & B2MASK) >> GLOBAL_SHIFT;
    }
  } 
  else prediction = TAKEN;
  if (br->is_call) {
    push_cr(br->instruction_next_addr);
  }
  if (br->is_indirect && !br->is_return) {
	 target_index = ((br->instruction_addr ^ thr) & B10MASK) >> T_INDEX_SHIFT;
  }
 
  if (br->is_return) {
    *predicted_target_address = pop_cr();
  }
  else  *predicted_target_address = get_target(br->instruction_addr >> TAG_SHIFT);
  
  if (!*predicted_target_address) {
    *predicted_target_address = last_target;
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

  // store actual target for catch all prediction
  last_target = actual_target_address;
  
  // insert actual target into BTB
  insert_target((br->instruction_addr >> TAG_SHIFT), actual_target_address); 

  // update thr with least significant 3 bits of actual target address
  if (br->is_indirect && !br->is_return) thr = ((thr << 3) | (actual_target_address & THR_MASK)); 
 
  // switch on state of branch result with prediction and saturation
  // counters : state machine
  // bit field: [actual, predicted, local, global]
  actual = uint8_t(taken);
  predicted = prediction;
  local = (local_pred[mask_local_history()] >> LOCAL_SHIFT) & 0x1;
  global = (global_pred[mask_path_history()] >> GLOBAL_SHIFT) & 0x1;
  test = ((actual << 3) | (predicted << 2) | (local << 1) | global);
  if (!br->is_indirect) {
	  switch(test)
	  {
		 case 0x1: // increment, train 'local'
		 case 0x5:
		 case 0xA:
		 case 0xE:
			  mod = 1;
			  break;
		 case 0x2: // decrement, train 'global'
		 case 0x6:
		 case 0x9:
		 case 0xD:
			  mod = -1;
			  break;
		 case 0x0: // no change
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
	  
	 // Train local and global with 'taken' | 'not taken'
	  mod = actual?1:-1;
	  twobit_saturation(&global_pred[mask_path_history()], mod);
	  threebit_saturation(&local_pred[mask_local_history()], mod);
	 
	  // Update history 
	  update_history(&path_history, actual);
	  path_history = mask_path_history();
	  update_history(&local_history[pc_index], actual);
	  local_history[pc_index] = mask_local_history();
  }
} 

/*
Class contrstructor
Init predictors to *magic* init values as determined by salt_searcher test.
init rotating indexes to head and tail of stack
*/
PREDICTOR::PREDICTOR() {
	for (int i = 0; i < LOCAL_PRED_SIZE; i++) { 
		local_pred[i] = LOCAL_SALT;
	}
	for (int i = 0; i < GLOBAL_PRED_SIZE; i++) {
		global_pred[i] = GLOBAL_SALT;
	}
	for (int i = 0; i < CHOICE_PRED_SIZE; i++){
		choice_pred[i] = CHOICE_SALT;
	}
	cr_head = 0;
	cr_tail = CR_CACHE_SIZE - 1;	
	cr_depth = 0;
	orphan_return = 0;
}

PREDICTOR::~PREDICTOR() {
//	printf("Cache Miss Rate: %f\n",((cache_access - cache_hit) / cache_access) * 100);
}

/*
Accept accessed way as input, update lru data in memory
For a cache with ASSOC_SIZE associativity
Model each LRU bit with a one byte bool 
where each bit represent the value of (j < k)
for the set of comparisons of way j and way k lru state machine values.
If the way accessed is 'j', set the value to true
If the way accessed is 'k', set the value to false

The outer for loop for the i counter is not needed because the size of the lru array is dependent on ASSOC_SIZE, so out of bounds array access is limited by the inner loops break comparison. 
*/
void PREDICTOR::update_lru(uint8_t index, int way){
	int i = 0;
	for (int j = 0; j < (ASSOC_SIZE - 1); j++) {
		for (int k = (j + 1); k <= (ASSOC_SIZE - 1); k++) {
			if (way == j) target_cache[index].lru[i] = 1;
			else if (way == k) target_cache[index].lru[i] = 0;
			i++;
		}
	}
}

/*
Access the current indexed cache set
Algorithim to decode the lru state machine output as a one hot
Allocate an array the size of the cache associativty level.
iterate through the lru state machine output checking the following:
If j < k is false, k can not be victim, mark the one hot output array entry j as false.
if j < k is true, j can not be the victim, mark the one hot output arrary k entry as false.
After iterating through the entire lru output, only one entry in the one hot array will be true. 
Return the index value of that entry.

*/
int PREDICTOR::get_victim(uint8_t index) {
	int i = 0;
	bool victim[ASSOC_SIZE];
	
	for(int l = 0; l < ASSOC_SIZE; l++) {
		if (!target_cache[index].lines[l].valid) {
			return l;
		}
	}

	for (int j = 0; j < (ASSOC_SIZE - 1); j++) {
		for (int k = (j + 1); k <= (ASSOC_SIZE - 1); k++) {
			if (target_cache[index].lru[i]) 
				victim[j] = false;
			else 
				victim[k] = false;
			i++;
		}
	}
	for (i = 0; i < (ASSOC_SIZE); i++) {
		if (victim[i]) break;
	}
	return i; 
}

/*
Accept tag and target address as input
Search buffer target cache for tag as indexed by the lower 10 bits of the PC
if tag found, insert target address as data
else, evict a way and insert in evicted way
*/
void PREDICTOR::insert_target(uint32_t tag, uint32_t target){
	
	int way;
	bool hit = false;

	// search for existing entry, check if correct, increment miss count on misprediction. Replace after THRESHOLD is reached
	for (way = 0; way < ASSOC_SIZE; way++) {
		if (target_cache[target_index].lines[way].tag == tag && target_cache[target_index].lines[way].valid) {
			if (target_cache[target_index].lines[way].data != target) twobit_saturation(&target_cache[target_index].lines[way].miss_rate,1);
			else twobit_saturation(&target_cache[target_index].lines[way].miss_rate,-1);
			if (target_cache[target_index].lines[way].miss_rate > TM_THRESH) target_cache[target_index].lines[way].data = target;
			update_lru(target_index, way);
			hit = true;
		}
	}
	// if no hit, evict a way and insert
	if (!hit) {
		way = get_victim(target_index);
		target_cache[target_index].lines[way].valid = true;
		target_cache[target_index].lines[way].tag = tag;
		target_cache[target_index].lines[way].data = target;
		target_cache[target_index].lines[way].miss_rate = 0;
		update_lru(target_index,way);
	}
	cache_access++;
}

/*
Search buffer target cache for PC tag bits
If found, return data from way's data field
else, return 0 as address (this creates an edge case)
*/
uint32_t PREDICTOR::get_target(uint32_t tag) {
	uint32_t target = 0;
	for (int way = 0; way < ASSOC_SIZE; way++) {
		if (target_cache[target_index].lines[way].valid && target_cache[target_index].lines[way].tag == tag) {
			target = target_cache[target_index].lines[way].data;
			update_lru(target_index, way);
			cache_hit++;
			break;
		}
	}
	return target;
}

/*
rotating stack of buffer target addresses
Insert at head
Increment head and tail, rotate as needed
*/
void PREDICTOR::push_cr(uint32_t target) {
	cr_cache[cr_head] = target;
	if (cr_head == (CR_CACHE_SIZE - 1)) {
		cr_head = 0;
		cr_tail++;
	}
	else if (cr_tail == (CR_CACHE_SIZE - 1)) {
		cr_head++;
		cr_tail = 0;
	}
	else {
		cr_head++;
		cr_tail++;
	}
}

/*
rotating stack of buffer target addresses
Return from tail
decrement head and tail, rotate as needed
*/
uint32_t PREDICTOR::pop_cr() {
	uint32_t target = cr_cache[cr_tail];
	if (cr_head == 0) {
		cr_head = CR_CACHE_SIZE - 1;
		cr_tail--;
	}
	else if (cr_tail == 0) {
		cr_head--;
		cr_tail = CR_CACHE_SIZE - 1;
	}
	else {
		cr_head--;
		cr_tail--;
	}
	return target;
}

/*
Update the global, local and choice saturation counters.
Accessed through macros for 2 bit and 3 bit saturation.
*/
void PREDICTOR::saturation(int length, uint8_t *targ, int mod)
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
void PREDICTOR::update_history(uint16_t *history, int actual){

  *history = (*history << 1) + actual;
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
