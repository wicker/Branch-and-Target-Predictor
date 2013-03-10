/* Author: Mark Faust
 *
 * C version of predictor file
*/

#ifndef PREDICTOR_H_SEEN
#define PREDICTOR_H_SEEN

#include <cstddef>
#include <cstring>
#include <inttypes.h>
#include <vector>
#include "op_state.h"   // defines op_state_c (architectural state) class 
#include "tread.h"      // defines branch_record_c class
#include <iostream>		// used for debug

#define LOCAL_SALT_LO 0x5
#define LOCAL_SALT_UP 0x5
#define GLOBAL_SALT 0x0

#define FOURK 4096
#define ONEK 1024
#define CHOICE_PRED_SIZE FOURK
#define GLOBAL_PRED_SIZE FOURK
#define LOCAL_PRED_SIZE ONEK
#define LOCAL_HISTORY_SIZE ONEK

#define B2MASK 0x000003
#define B3MASK 0x000007
#define B10MASK 0x000003FF
#define B12MASK 0x00000FFF

#define LOCAL_CHOICE 0x2
#define LOCAL_SHIFT 2
#define GLOBAL_SHIFT 1
#define HISTORY_SHIFT 1

#define TAKEN 1
#define NOT_TAKEN 0

#define ASSOC_SIZE 4
#define T_CACHE_SIZE ONEK
#define CR_CACHE_SIZE 12

#define twobit_saturation(target, mod)    saturation(2, target, mod)
#define threebit_saturation(target, mod)    saturation(3, target, mod)


typedef struct {
	bool valid;
	uint32_t tag;
} line;

typedef struct {
	line way[ASSOC_SIZE];
	uint8_t lru;
} set;


void saturation(int, uint8_t *, int);
void update_history(uint16_t *, int);

class PREDICTOR
{
public:
    PREDICTOR();
    bool get_prediction(const branch_record_c* br, const op_state_c* os, uint *predicted_target_address);

    void update_predictor(const branch_record_c* br, const op_state_c* os, bool taken, uint actual_target_address);
private:
	uint8_t    choice_pred[CHOICE_PRED_SIZE];
	uint8_t    local_pred[LOCAL_PRED_SIZE];
	uint8_t    global_pred[GLOBAL_PRED_SIZE];
	uint16_t   local_history[LOCAL_HISTORY_SIZE];
	uint16_t   path_history;
	uint8_t    prediction;
	uint16_t   pc_index;

   // utility functions
	uint16_t   mask_path_history();
	uint16_t   mask_local_history();

	//Branch Target Predictor Data
	target_cache[T_CACHE_SIZE];
	uint32_t cr_cache[CR_CACHE_SIZE];
	uint32_t * cr_head, cr_tail;
	
	// Branch Target Functions
	void push_cr();
	uint32_t pop_cr();
	void insert_target(uint16_t, uint32_t);
	uint32_t get_target(uint16_t);
	void update_lru(uint8_t, uint8_t);
	uint8_t get_victim(uint8_t);
};

                                                                                                  
#endif // PREDICTOR_H_SEEN

