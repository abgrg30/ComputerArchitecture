#include "traceread.h"
#include <inttypes.h>
#include <stdio.h>

extern FILE * stream;

bool read_branch (unsigned int * pc, bool * outcome)
{
  // read the pc
  if (fread (pc, sizeof (uint32_t), 1, stream) != 1)
    return false;

  // read the outcome
  uint8_t outcome_int;
  if (fread (&outcome_int, sizeof (uint8_t), 1, stream) != 1)
    return false;
  if (outcome_int == 0)
    *outcome = false;
  else
    *outcome = true;

  return true;

}
