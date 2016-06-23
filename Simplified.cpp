// A Sudoku solver.
// Information is presented as a vector of horizontal row vectors.
// The digit 0 indicates a blank space.
// The technique is backtracking with a stack storing the options
// to backtrack to.  At each iteration, the blank with the
// least number of legal digits is searched for.
#include <vector>
#include <utility>
#include <algorithm>
#include <stack>
#include <unordered_map>
#include <list>
#include <string>

// The stack identifies the relevant data for each blank.
// This data is the coordinate pair identifying the location
// in the grid, and also a vector of digits identifying
// legal options. An index indicating
// the previous option considered is also included.
// A revisited bool identifies whether the
// top member is being revisited during a backtrack.
struct stackData{
std::pair<int, int> coordinates;
std::vector<int> choices;
int index;
bool revisited;
};
// Function for displaying grids
// solution is true if a solution is being displayed.

// For a given occupied square, check that (return true if) there is no matching digit in the same row.
// i == 3, j == 5 means that nothing in the 3rd row should match the (3, 5) entry.
bool checkHorizontal(int i, int j, const std::vector<std::vector<int> >& grid)
{
    if(grid[i][j]) // Square is not blank
      for(int J = 0; J < grid.size(); ++J)
        if(j!=J && grid[i][J] == grid[i][j])
            return false;
    return true;
}
// And for columns.
bool checkVertical (int i, int j, const std::vector<std::vector<int> >& grid)
{
    if(grid[i][j]) // Square is not blank
     for(int I = 0; I < grid.size(); ++I)
        if(i!=I && grid[i][j] == grid[I][j])
           return false;
    return true;
}
// Similar checks for subgrid of 3 x 3.
// This assumes that horizontal and vertical checks have both been done.
bool checkSubgrid (int i, int j, const std::vector<std::vector<int> >& grid, int subgridSize = 3)
{
    const int startI = i - i % subgridSize;
    const int endI = startI + subgridSize;
    const int startJ = j - j % subgridSize;
    const int endJ = startJ + subgridSize;
    if(grid[i][j]) // Square is not blank
      for(int I = startI; I < endI; ++I)
        for(int J = startJ; J < endJ; ++J)
            if(i!=I && j!=J && grid[i][j] == grid[I][J])
                return false;
    return true;
}
// Identifying if two pairs of coordinates exist in the same subgrid
bool sameSubgrid (const std::pair<int, int>& lhs, const std::pair<int, int>& rhs, int subgridSize = 3)
{
    return lhs.first / subgridSize == rhs.first / subgridSize && lhs.second / subgridSize == rhs.second / subgridSize;
}
// Identifying if two pairs of coordinates can legally contain the same digit.
// True if both pairs are allowed to contain the same digit.
bool consistent(const std::pair<int, int>& lhs, const std::pair<int, int>& rhs, int subgridSize = 3)
{
    return lhs == rhs || lhs.first != rhs.first && lhs.second != rhs.second && !sameSubgrid(lhs, rhs, subgridSize);
}
// Identifying if labelling a given digit at a given location is consistent with the data on
// where the digits occur.
bool consistent(int digit, const std::pair<int, int>& location, const std::vector<std::vector<int> >& grid, std::unordered_map<int, std::vector<std::pair<int, int> > >& record, int subgridSize = 3)
{
    auto column = record[digit];
    for(int i = 0; i < column.size(); ++i)
       if(!consistent(location, column[i], subgridSize))
           return false;
    return true;
}
// Create a grid to test algorithm. I chose the most recent of
// the "hard" sudokus from the Guardian.  The sudoku is available
// at this URL: http://www.theguardian.com/lifeandstyle/2016/jun/18/sudoku-3465-hard
// Two other Guardian hard sudokus and a trivial test case where every square is blank
// are made easily accessible by decommenting.
// A no solution case is also made available.  The one currently
// decommented is the supposedly hardest ever sudoku --
// www.telegraph.co.uk/news/science/science-news/9359579/Worlds-hardest-sudoku-can-you-crack-it.html
// This was timed at just under 1.5 seconds.
std::vector<std::vector<int> > createTestGrid()
{
    const std::vector<std::vector<int> > grid =
    {{8,0,0,0,0,0,0,0,0},{0,0,3,6,0,0,0,0,0},{0,7,0,0,9,0,2,0,0},
     {0,5,0,0,0,7,0,0,0},{0,0,0,0,4,5,7,0,0},{0,0,0,1,0,0,0,3,0},
     {0,0,1,0,0,0,0,6,8},{0,0,8,5,0,0,0,1,0},{0,9,0,0,0,0,4,0,0}};
    return grid;
}
// Obtain list of coordinate pairs of blanks that need to be filled in.
std::list<std::pair<int, int> > blankSquares(const std::vector <std::vector<int> >& grid)
{
    std::list<std::pair<int, int> > blanks;
    for(int i = 0; i < grid.size(); ++i)
        for(int j = 0; j < grid[i].size(); ++j)
           if(!grid[i][j])
               blanks.push_back({i, j});
    return blanks;
}
// Construct map of locations of occurrences of each digit on grid
std::unordered_map<int, std::vector<std::pair<int, int> > >occurrences(const std::vector<std::vector<int> >& grid)
{
    std::unordered_map<int, std::vector<std::pair<int, int> > > result;
    for(int i = 0; i < grid.size(); ++i)
        for(int j = 0; j < grid[i].size(); ++j)
           if(grid[i][j])
              result[grid[i][j]].push_back({i, j});
    return result;
}
// Identifying the legal digits for a grid square.
// location represents the coordinates of the originally blank square.
std::vector<int> options(const std::pair<int, int>& location, std::vector<std::vector<int> >& grid, std::unordered_map<int, std::vector<std::pair<int, int> > >& record, int subgridSize = 3)
{
    std::vector<int> result;
    const int first = location.first;
    const int second = location.second;

       for(int attemptDigit = 0; attemptDigit < grid.size(); ++attemptDigit)
       {
          grid[first][second]= attemptDigit + 1;
          const int val = grid[first][second];
          if(consistent(val, location, grid, record, subgridSize))
             result.push_back(val);
       }
       grid[first][second] = 0; // reset

    return result;
}
// Finding the iterator to the blank which has the least number of options.
// Note that we only need to know which of the blanks has the
// least options.  Once we've established that one blank has more
// options than the minimum, we can stop counting.
// A complete count is only necessary for the initial iterator.
// A bool is altered by reference to indicate whether the iterator
// points to an empty list.
// That bool is true if the list is non-empty.
std::list< std::pair<int, int> >::iterator  min(std::vector<std::vector<int> >& grid, std::list<std::pair<int, int> >& blanks, std::unordered_map<int, std::vector<std::pair<int, int> > >& record, bool& nonEmpty, int subgridSize = 3)
{
    int threshold = -1; // Counter of minimum number of options. Process can be terminated if this attains 0.
    auto iter = blanks.begin();
    auto currentMin = iter;
    while(iter != blanks.end() && threshold)
      if(iter == blanks.begin())
            threshold = options(*iter++, grid, record, subgridSize).size();
      else
        {
            int successfulDigits = 0;
            for(int attemptedDigit = 1; attemptedDigit <= grid.size() && successfulDigits < threshold; ++attemptedDigit)
               if(consistent(attemptedDigit, *iter, grid, record, subgridSize))
                  ++successfulDigits;

            if(successfulDigits < threshold)
            {
                threshold = successfulDigits;
                currentMin = iter;
            }
            ++iter;
        }
    nonEmpty = threshold; // boolean indicator of whether threshold is 0.
    return currentMin;
}
// Explore further by pushing to stack.
// Don't push stack members which correspond to no legal options.
// If we have no legal options, the push returns false.
bool push(std::stack<stackData>& stack, std::list<std::pair<int, int> >& blanksInProcess, std::unordered_map<int, std::vector<std::pair<int, int> > >& record, std::vector<std::vector<int> >& grid, int subgridSize = 3)
{
    bool nonEmptyIndicator; // Changed by reference so need not be set.
    const auto easiest = min(grid, blanksInProcess, record, nonEmptyIndicator, subgridSize);
    if(!nonEmptyIndicator)
        return false;
    // Find the choices for the new stack member.
    const auto choices = options(*easiest, grid, record, subgridSize);
    grid[easiest->first][easiest->second] = choices[0];
    // Update records of occurrences.
    record[choices[0]].push_back({easiest->first, easiest->second});
    stack.push({*easiest, choices, 0, false}); // Pushing the blank with the minimum number of options.
    blanksInProcess.erase(easiest); // We are trying to fill square in so no longer considered blank.
    return true;
}
// Pop stack when backtracking needed.
void pop(std::stack<stackData>& stack, std::list<std::pair<int, int> >& blanksInProcess, std::unordered_map<int, std::vector<std::pair<int, int> > >& record, std::vector<std::vector<int> >& grid)
{
    const auto coordinates = stack.top().coordinates;
    const auto choices = stack.top().choices;
    stack.pop();
    // Mark the exposed stack top (if any) as revisited
    if(!stack.empty())
        stack.top().revisited = true;
    grid[coordinates.first][coordinates.second] = 0;
    blanksInProcess.push_back(coordinates);
    // Update records
    auto& column = record[choices.back()];
    auto iter = std::find(column.begin(), column.end(), coordinates);
    column.erase(iter);
}
// Attempt to push, but pop if attempt fails.
void tryToPush(std::stack<stackData>& stack, std::list<std::pair<int, int> >& blanksInProcess, std::unordered_map<int, std::vector<std::pair<int, int> > >& record, std::vector<std::vector<int> >& grid, int subgridSize = 3)
{
    if(!push(stack, blanksInProcess, record, grid, subgridSize))
        pop(stack, blanksInProcess, record, grid);
}
// Increment the value at the top of the stack.
void increment(std::stack<stackData>& stack, std::unordered_map<int, std::vector<std::pair<int, int> > >& record, std::vector<std::vector<int> >& grid)
{
    auto& top = stack.top();
    const int index = top.index;
    const auto location = top.coordinates;
    int& digit = grid[location.first][location.second];
    auto& column = record[digit];
    // Find digit to erase
    const auto iter = std::find(column.begin(), column.end(), location);
    digit = top.choices[++top.index]; // Update grid and increment index.
    // Update records.
    column.erase(iter);
    record[digit].push_back(location);
}
// After incrementing, we try to push.  This is combined in one operation using overloading.
void increment(std::stack<stackData>& stack, std::list<std::pair<int, int> >& blanksInProcess, std::unordered_map<int, std::vector<std::pair<int, int> > >& record, std::vector<std::vector<int> >& grid, int subgridSize = 3)
{
    increment(stack, record, grid);
    tryToPush(stack, blanksInProcess, record, grid, subgridSize);
    // If stack has become empty, try to restart a new stack
    if(stack.empty() && !blanksInProcess.empty())
        push(stack, blanksInProcess, record, grid, subgridSize);
}
// Solve by backtracking and return completed grid.
// If problem has no solution, return empty vector.
// Constantly rearranges the blanks so that the blank with fewest options is the next to consider.
// Progress through the blanks is traced in a stack.
std::vector <std::vector<int> > backtrack(std::vector<std::vector<int> >& grid, int subgridSize = 3)
{
    auto blanksInProcess = blankSquares(grid); // Squares blank in the initial sudoku -- this changes as sudouku is filled.
    const int fullStackSize = blanksInProcess.size();
    // Maintain a stack of stackData members.
    // Each member records a coordinate pair
    // and the set of legal options at that pair.
    std::stack<stackData> blankStack;
    // Find where each digit has occurred.
    auto recordsEachDigit = occurrences(grid);
    if(blanksInProcess.empty())
        return grid;
    // Initiate the blank stack by pushing if possible
    push(blankStack, blanksInProcess, recordsEachDigit, grid, subgridSize);

    while(!blankStack.empty())
    {
        auto& topMember = blankStack.top();
        // If the stack is full the problem is solved.
        if(blankStack.size() == fullStackSize)
            return grid;
        // Increment stack if finished exploring
        // If finished exploring at current level -- will increment unless
        // the index is already positioned at the last element.
        if(topMember.revisited && topMember.index + 1 < topMember.choices.size())
            increment(blankStack, blanksInProcess, recordsEachDigit, grid, subgridSize);
        else if (!topMember.revisited) // Attempt to push.
            tryToPush(blankStack, blanksInProcess, recordsEachDigit, grid, subgridSize);
        else
            pop(blankStack, blanksInProcess, recordsEachDigit, grid);
    }
    // No solution if stack becomes empty.

    return{};
}

int main()
{
      auto grid = createTestGrid();
      backtrack(grid);
}
