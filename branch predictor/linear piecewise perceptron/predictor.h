#ifndef PREDICTOR_H
#define PREDICTOR_H

/*
  Define all your tables and their sizes here.
  All tables must be statically-sized.
  Please don't call malloc () or any of its relatives within your
  program.  The predictor will have a budget, namely (32K + 256) bits
  (not bytes).  That encompasses all storage (all tables, plus GHR, if
  necessary -- but not PC) used for the predictor.  That may not be
  the amount of storage your predictor uses, however -- for example,
  you may implement 2-bit predictors with a table of ints, in which
  case the simulator will use more memory -- that's okay, we're only
  concerned about the memory used by the simulated branch predictor.
*/
#include<iostream>
#include<cstdio>
#include<cstring>
using namespace std;

#define POWER(x) (1<<x)

#define BGHR 18
#define BGA 4
#define BPC 4
#define BW 7

#define THRESHOLD ((2.14 * (BGHR)) + 20.58)
#define RESET 0
#define NSTATE 1

#define MAX (POWER(BW-1)-1)
#define MIN (-1*(MAX+1))

/*
  Initialize the predictor.
*/
void init_predictor ();

/*
  Make a prediction for conditional branch instruction at PC 'pc'.
  Returning true indicates a prediction of taken; returning false
  indicates a prediction of not taken.
*/
bool make_prediction (unsigned int pc);

/*
  Train the predictor the last executed branch at PC 'pc' and with
  outcome 'outcome' (true indicates that the branch was taken, false
  indicates that the branch was not taken).
*/
void train_predictor (unsigned int pc, bool outcome);

#endif
