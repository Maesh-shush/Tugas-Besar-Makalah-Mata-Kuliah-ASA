#ifndef BRANCHANDBOUND_H
#define BRANCHANDBOUND_H
#include "core.h"

extern int jumlahPruning;

bool branchAndBound(int r, int c, bool transitVisited[MAKS][MAKS]);

#endif