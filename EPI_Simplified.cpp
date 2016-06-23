// Copyright (c) 2013 Elements of Programming Interviews. All rights reserved.

#include <algorithm>
#include <cmath>
#include <deque>
#include <iostream>
#include <iterator>
#include <vector>
#include <ctime>
using std::deque;
using std::endl;
using std::ostream_iterator;
using std::vector;

bool SolvePartialSudoku(int, int, vector<vector<int>>*);
bool ValidToAddVal(const vector<vector<int>>&, int, int, int);

// @include
const int kEmptyEntry = 0;

bool SolveSudoku(vector<vector<int>>* partial_assignment) {
 SolvePartialSudoku(0, 0, partial_assignment);
}
bool SolvePartialSudoku(int i, int j,
                        vector<vector<int>>* partial_assignment) {
  if (i == partial_assignment->size()) {
    i = 0;  // Starts a new row.
    if (++j == (*partial_assignment)[i].size()) {
      return true;  // Entire matrix has been filled without conflict.
    }
  }

  // Skips nonempty entries.
  if ((*partial_assignment)[i][j] != kEmptyEntry) {
    return SolvePartialSudoku(i + 1, j, partial_assignment);
  }

  for (int val = 1; val <= partial_assignment->size(); ++val) {
    // It's substantially quicker to check if entry val conflicts
    // with any of the constraints if we add it at (i,j) before
    // adding it, rather than adding it and then checking all constraints.
    // The reason is that we know we are starting with a valid configuration,
    // and the only entry which can cause a problem is entryval at (i,j).
    if (ValidToAddVal(*partial_assignment, i, j, val)) {
      (*partial_assignment)[i][j] = val;
      if (SolvePartialSudoku(i + 1, j, partial_assignment)) {
        return true;
      }
    }
  }

  (*partial_assignment)[i][j] = kEmptyEntry;  // Undo assignment.
  return false;
}

bool ValidToAddVal(const vector<vector<int>>& partial_assignment, int i,
                   int j, int val) {
  // Check row constraints.
  for (int k = 0; k < partial_assignment.size(); ++k) {
    if (val == partial_assignment[k][j]) {
      return false;
    }
  }

  // Check column constraints.
  for (int k = 0; k < partial_assignment.size(); ++k) {
    if (val == partial_assignment[i][k]) {
      return false;
    }
  }

  // Check region constraints.
  int region_size = sqrt(partial_assignment.size());
  int I = i / region_size, J = j / region_size;
  for (int a = 0; a < region_size; ++a) {
    for (int b = 0; b < region_size; ++b) {
      if (val ==
          partial_assignment[region_size * I + a][region_size * J + b]) {
        return false;
      }
    }
  }
  return true;
}
// @exclude

int main(int argc, char* argv[]) {
  vector<vector<int>> A =  {{8,0,0,0,0,0,0,0,0},{0,0,3,6,0,0,0,0,0},{0,7,0,0,9,0,2,0,0},
     {0,5,0,0,0,7,0,0,0},{0,0,0,0,4,5,7,0,0},{0,0,0,1,0,0,0,3,0},
     {0,0,1,0,0,0,0,6,8},{0,0,8,5,0,0,0,1,0},{0,9,0,0,0,0,4,0,0}};
  SolveSudoku(&A);
  return 0;
}
