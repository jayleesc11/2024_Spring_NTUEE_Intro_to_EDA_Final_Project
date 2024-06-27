#ifndef CONSTANT_H
#define CONSTANT_H

const float ALPHA = 35;                // OAST: Congestion proportion of edge weight
const float THRESHOLD = 0.7;            // OAST: Congestion threshold
const bool OVERFLOW_TYPE = true;        // OAST: Overflow value selection: true for the min one, false for the entire loop
const bool USE_HEURISTIC = false;       // OARST: use later 2 or not when constructing

const int RRR_ITER = 1;                 // RRR: Number of RRR iterations
const int RRR_THRESHOLD = 10;           // RRR: Threshold for RRR iterations
const int RRR_NUM = 3000;               // RRR: Number of nets to be re-routed each time
// For no bounding box OASG, please directly comment & uncomment .o generation in Makefile

#endif