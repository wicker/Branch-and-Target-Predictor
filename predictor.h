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

class PREDICTOR
{
public:
    bool get_prediction(const branch_record_c* br, const op_state_c* os, uint *predicted_target_address);

    void update_predictor(const branch_record_c* br, const op_state_c* os, bool taken, uint actual_target_address);

};

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
~                                                                                                   

#endif // PREDICTOR_H_SEEN

