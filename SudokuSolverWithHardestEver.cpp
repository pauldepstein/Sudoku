// A Sudoku solver.
// Information is presented as a vector of horizontal row vectors.
// The digit 0 indicates a blank space.
// The technique is backtracking with a stack storing the options
// to backtrack to.  At each iteration, the blank with the
// least number of legal digits is searched for.
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <stack>
#include <unordered_map>
#include <stdexcept>
#include <list>
#include <string>
#include <ctime>

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
void display(const std::vector<std::vector<int> >& grid, bool solution = true);

// True if the format of the grid makes sense.
// The legality of the grid (for example two identical digits in the same row)
// is not considered.
bool checkFormat(const std::vector<std::vector<int> >& grid, int gridSize = 9)
{
    if(grid.size() != gridSize)
        return false;

    for(int i = 0; i < gridSize; ++i)
    {
        if(grid[i].size() != gridSize)
           return false;

        for(int j = 0; j < gridSize; ++j)
            if(grid[i][j] < 0 || grid[i][j] > gridSize)
                return false;
    }
    return true;
}

// Checking that an index makes sense
bool checkIndex(int index, const std::vector<std::vector<int> >& grid)
{
    if(index < 0 || index >= grid.size())
    {
        std::cout << "Index out of range";
        return false;
    }
    return true;
}

// Checking a coordinate pair
bool checkPair(const std::pair<int, int>& coordinate, const std::vector<std::vector<int> >& grid)
{
    return checkIndex(coordinate.first, grid) && checkIndex(coordinate.second, grid);
}

// For a given occupied square, check that (return true if) there is no matching digit in the same row.
// i == 3, j == 5 means that nothing in the 3rd row should match the (3, 5) entry.
bool checkHorizontal(int i, int j, const std::vector<std::vector<int> >& grid)
{
    if(!checkPair({i,j}, grid))
        return false;

    if(grid[i][j]) // Square is not blank
      for(int J = 0; J < grid.size(); ++J)
        if(j!=J && grid[i][J] == grid[i][j])
            return false;
    return true;
}

// And for columns.
bool checkVertical (int i, int j, const std::vector<std::vector<int> >& grid)
{
    if(!checkPair({i,j}, grid))
        return false;

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
    if(!checkPair({i,j}, grid))
        return false;

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

// Combine above for a legality check at square with (i, j) coordinates.
bool checkLegal(int i, int j, const std::vector<std::vector<int> >& grid, int subgridSize = 3)
{
    return checkHorizontal(i, j, grid) && checkVertical(i, j, grid) && checkSubgrid(i, j, grid, subgridSize);
}

// Above check for entire grid.
bool checkLegal(const std::vector<std::vector<int> >& grid, int subgridSize = 3)
{
    for(int i = 0; i < grid.size(); ++i)
        for(int j = 0; j < grid.size(); ++j)
           if(!checkLegal(i, j, grid, subgridSize))
               return false;
    return true;
}

// Checking user's grid is ok
bool checkUserGrid(const std::vector<std::vector<int> >& grid, int gridSize = 9, int subgridSize = 3)
{
    return checkFormat(grid, gridSize) && checkLegal(grid, subgridSize);
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
    /*{{0,0,0,0,0,7,0,0,0},{0,5,9,0,0,0,0,0,3},{0,3,0,0,5,0,4,0,9},
     {0,0,0,0,2,0,0,0,0},{0,0,7,9,8,0,3,0,0},{1,0,0,0,0,0,8,0,6},
     {0,0,2,0,4,5,0,0,7},{0,0,0,0,0,0,0,2,1},{0,9,6,0,0,2,5,3,0}};*/
    /*
    {{0,0,0,0,6,0,1,0,0},{0,1,0,0,9,0,0,6,0},{0,0,9,2,0,0,0,0,0},
     {1,0,4,0,0,7,0,9,5},{0,3,0,0,0,6,0,1,0},{8,0,5,0,0,9,0,7,6},
     {0,0,2,8,0,0,0,0,0},{0,8,0,0,3,0,0,4,0},{0,0,0,0,7,0,2,0,0}};*/
    /*
    {{0,0,0,0,0,3,0,5,0},{0,0,7,5,0,0,0,0,1},{0,0,9,0,1,0,0,0,7},
     {0,9,3,0,5,1,0,0,0},{0,0,0,7,0,0,0,9,0},{0,8,2,0,4,6,0,0,0},
     {0,0,5,0,8,0,0,0,2},{0,0,8,2,0,0,0,0,4},{0,0,0,0,0,4,0,6,0}};*/

    //std::vector< std::vector<int> > (9, std::vector<int>(9));
    // A no solution case is added below.  These can be created
    // easily by wrongly filling in a blank from the original grid.
    /*
    {{2,0,0,0,0,7,0,0,0},{0,5,9,0,0,0,0,0,3},{0,3,0,0,5,0,4,0,9},
     {0,0,0,0,2,0,0,0,0},{0,0,7,9,8,0,3,0,0},{1,0,0,0,0,0,8,0,6},
     {0,0,2,0,4,5,0,0,7},{0,0,0,0,0,0,0,2,1},{0,9,6,0,0,2,5,3,0}};*/
    // "World's hardest sudoku" below.
    {{8,0,0,0,0,0,0,0,0},{0,0,3,6,0,0,0,0,0},{0,7,0,0,9,0,2,0,0},
     {0,5,0,0,0,7,0,0,0},{0,0,0,0,4,5,7,0,0},{0,0,0,1,0,0,0,3,0},
     {0,0,1,0,0,0,0,6,8},{0,0,8,5,0,0,0,1,0},{0,9,0,0,0,0,4,0,0}};


    display(grid, false);
    if(!checkUserGrid(grid))
       throw std::runtime_error("Grid seems to have been mistyped\n");

    return grid;
}

// Obtaining grid from user.
std::vector<std::vector<int> > getGrid(int gridLength = 9)
{
    std::vector<std::vector<int> > grid;
    for(int i = 0; i < gridLength; ++i)
    {
        std::vector<int> nextRow;
        grid.push_back(nextRow);
        std::cout << std::endl << "\nPlease enter the data for row " << i + 1 << std::endl;
        if(!i)
        {
            std::cout << std::endl << "Enter 0 for a blank square and a digit otherwise ";
            std::cout << std::endl << "You can hit either the space bar or the enter key between entries" << std::endl;
        }
        for(int j = 0; j < gridLength; ++j)
        {
            int digit;
            std::cin >> digit;
            grid.back().push_back(digit);
        }
    }
    return grid;
}

// True if user's data is ok, error diagnostics otherwise.
bool userDataFine(const std::vector<std::vector<int> >& grid, int gridSize = 9, int subgridSize = 3)
{
    if(!checkUserGrid(grid, gridSize, subgridSize))
    {
        std::cout << std::endl << "The data was found to be in error.";
        std::cout << std::endl << "If you believe you entered a valid sudoku grid, please email pauldepstein@yahoo.com";
        std::cout << std::endl << "Otherwise, simply run the program again.";
        return false;
    }
    return true;
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

// Exception thrown if program is confused about which squares are blank.
// grid[i][j] is supposed to be blank.
// Assumes index checking has already been done.
void checkBlank(int i, int j, const std::vector<std::vector<int> >& grid, int lineNumber)
{
    if(grid[i][j])
    {
        std::cout << "\nError at line " << lineNumber << std::endl;
        throw std::runtime_error("A square was filled in which was supposed to be blank. ");
    }
}

// Parallel version if presented as a pair.
void checkBlank(const std::pair<int, int>& blankPair, const std::vector<std::vector<int> >& grid, int lineNumber)
{
    checkBlank(blankPair.first, blankPair.second, grid, lineNumber);
}

// Identifying the legal digits for a grid square.
// location represents the coordinates of the originally blank square.
std::vector<int> options(const std::pair<int, int>& location, std::vector<std::vector<int> >& grid, std::unordered_map<int, std::vector<std::pair<int, int> > >& record, int subgridSize = 3)
{
    std::vector<int> result;
    const int first = location.first;
    const int second = location.second;
    if(checkPair(location, grid))
    {
       for(int attemptDigit = 0; attemptDigit < grid.size(); ++attemptDigit)
       {
          grid[first][second]= attemptDigit + 1;
          const int val = grid[first][second];
          if(consistent(val, location, grid, record, subgridSize))
             result.push_back(val);
       }
       grid[first][second] = 0; // reset
    }
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

// Another bug-tracking function introduced
void errorIntroduced(const std::list<std::pair<int, int> >& blanksInProcess, const std::vector<std::vector<int> >& grid, int lineNumber)
{
    for(auto iter = blanksInProcess.begin(); iter != blanksInProcess.end(); ++iter)
    {
        const int first = iter->first;
        const int second = iter->second;
        const int square = grid[first][second];
        if(square)
        {
            std::cout << "Error introduced at line " << lineNumber << std::endl;
            std::cout << "Filled square which should be blank" << std::endl;
            std::cout << "Value is " << square << " and coordinates are " << first << " and " << second;
            throw std::runtime_error("\nUnexpected filled square in blanks container");
        }
    }
}

// While backtracking, only blank squares should change
void errorCopyChanged(const std::vector<std::vector<int> > & original, const std::vector<std::vector<int> > & copy)
{
    for(int i = 0; i < original.size(); ++i)
        for(int j = 0; j < original[i].size(); ++j)
           if(original[i][j] && copy[i][j]!= original[i][j])
           {
              std::cout << "\n Change at square with coordinates " << i << " and " << j;
              std::cout << "\n Value changed from " << original[i][j] << " to " << copy[i][j] << std::endl;
              throw std::runtime_error("\n Only blank entries should change during backtracking\n");
           }
}

// Explore further by pushing to stack.
// Don't push stack members which correspond to no legal options.
// If we have no legal options, the push returns false.
bool push(std::stack<stackData>& stack, std::list<std::pair<int, int> >& blanksInProcess, std::unordered_map<int, std::vector<std::pair<int, int> > >& record, std::vector<std::vector<int> >& grid, int subgridSize = 3)
{
    if(blanksInProcess.empty())
       throw std::runtime_error("Shouldn't be trying to push when we've run out of blank squares");

    bool nonEmptyIndicator; // Changed by reference so need not be set.
    const auto easiest = min(grid, blanksInProcess, record, nonEmptyIndicator, subgridSize);
    if(!nonEmptyIndicator)
        return false;

    // Find the choices for the new stack member.
    const auto choices = options(*easiest, grid, record, subgridSize);
    // Choices should not be empty.
    if(choices.empty())
        throw std::runtime_error("List of choices unexpectedly empty while pushing.");

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
    // Should not be trying to pop from an empty stack.
    if(stack.empty())
        throw std::runtime_error("Trying to pop from an empty stack");
    const auto coordinates = stack.top().coordinates;
    const auto choices = stack.top().choices;

    // If the set of choices is empty, it shouldn't be on the stack.
    if(choices.empty())
        throw std::runtime_error("List of choices unexpectedly empty while popping.");

    stack.pop();
    // Mark the exposed stack top (if any) as revisited
    if(!stack.empty())
        stack.top().revisited = true;

    grid[coordinates.first][coordinates.second] = 0;
    blanksInProcess.push_back(coordinates);
    // Update records
    auto& column = record[choices.back()];
    auto iter = std::find(column.begin(), column.end(), coordinates);
    if(iter == column.end())
        throw std::runtime_error("\nWhen popping a stack, the coordinates of the popped value should be in the appropriate list");
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
    if(stack.empty())
        throw std::runtime_error("\nStack unexpectedly empty while trying to increment");
    auto& top = stack.top();
    const int index = top.index;
    if(index + 1 >= top.choices.size())
        throw std::runtime_error("\nCan't increment because already at highest option");

    const auto location = top.coordinates;
    int& digit = grid[location.first][location.second];
    auto& column = record[digit];

    // Find digit to erase
    const auto iter = std::find(column.begin(), column.end(), location);
    if(iter == column.end())
        throw std::runtime_error("\nDigit which needs removing from records can't be found");

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

// At each iteration, a maximum of one grid square should be changed.
// This can be checked through exception handling.
void tooManyChanges(const std::vector<std::vector<int> >& previousGrid, const std::vector<std::vector<int> >& currentGrid)
{
    int numChanges = 0;
    for(int i = 0; i < previousGrid.size(); ++i)
        for(int j = 0; j < previousGrid.size(); ++j)
           if(previousGrid[i][j] != currentGrid[i][j] && ++numChanges == 2)
              throw std::runtime_error("Too many changes were introduced in a single iteration");
}

// Solve by backtracking and return completed grid.
// If problem has no solution, return empty vector.
// Constantly rearranges the blanks so that the blank with fewest options is the next to consider.
// Progress through the blanks is traced in a stack.
// Displays other info on solving process.
std::vector <std::vector<int> > backtrack(std::vector<std::vector<int> >& grid, int subgridSize = 3)
{
    // Timing
    clock_t begin = clock();
    auto blanksInProcess = blankSquares(grid); // Squares blank in the initial sudoku -- this changes as sudouku is filled.
    const int fullStackSize = blanksInProcess.size();
    int numIterations = 0; // Diagnostic information which shows progress

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
        {
           double timeTaken = clock()-begin;
           timeTaken /= CLOCKS_PER_SEC;
           std::cout << std::endl << "\nSolution has been obtained in " << numIterations << " iterations, taking " << timeTaken << " seconds." << std::endl;
           return grid;
        }

        // Increment stack if finished exploring
        // If finished exploring at current level -- will increment unless
        // the index is already positioned at the last element.
        if(topMember.revisited && topMember.index + 1 < topMember.choices.size())
            increment(blankStack, blanksInProcess, recordsEachDigit, grid, subgridSize);
        else if (!topMember.revisited) // Attempt to push.
            tryToPush(blankStack, blanksInProcess, recordsEachDigit, grid, subgridSize);
        else
            pop(blankStack, blanksInProcess, recordsEachDigit, grid);
        ++numIterations;
    }

    // No solution if stack becomes empty.
    double timeTaken = clock()-begin;
    timeTaken /= CLOCKS_PER_SEC;
    std::cout << "\nSudoku found to be unsolvable after " << numIterations << " iterations, taking " << timeTaken << " seconds." << std::endl;
    return{};
}

// Displaying grid.
void display(const std::vector<std::vector<int> >& grid, bool solution)
{
    if(grid.empty())
    {
        std::cout << std::endl << "This sudoku problem can not be solved";
        std::cout << std::endl << "Please email pauldepstein@yahoo.com if you believe it has a solution";
    }
    else
    {
       const std::string message = solution ? "The solution appears below" : "The original grid appears below";
       std::cout << std::endl << message;
       for(int i = 0; i < grid.size(); ++i)
       {
           std::cout << std::endl << std::endl << "The entries in row " << i + 1 << " are ";
           for(int j = 0; j < grid.size(); ++j)
               std::cout << grid[i][j] << " ";
       }
    }
}

int main()
{
  // Two attempts.  One with a stored example,
  // and one supplied by the user.
   try
   {
      // Stored
      auto grid = createTestGrid();
      display(backtrack(grid));

      //User
      grid = getGrid();
      if(userDataFine(grid))
        display(backtrack(grid));
   }
   catch(std::runtime_error& e)
   {
       std::cout << e.what();
   }
    return 0;
}
