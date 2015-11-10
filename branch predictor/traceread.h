#ifndef TRACEREAD_H
#define TRACEREAD_H

/*
  Read the next conditional branch from the trace from stdin.
  Returns true if a conditional branch was found, false otherwise.
  The first pointer argument will contain the branch instruction's PC.
  The second pointer argument will contain the branch outcome.
*/
bool read_branch (unsigned int * pc, bool * outcome);

#endif
