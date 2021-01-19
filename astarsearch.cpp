#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
using std::cout;
using std::ifstream;
using std::istringstream;
using std::string;
using std::vector;
using std::abs;
using std::sort;

enum class State {kEmpty, kObstacle, kClosed, kPath, kStart, kFinish};


vector<State> ParseLine(string line) {
    istringstream sline(line);
    int n;
    char c;
    vector<State> row;
    while (sline >> n >> c && c == ',') {
      if (n == 0) {
        row.push_back(State::kEmpty);
      } else {
        row.push_back(State::kObstacle);
      }
    }
    return row;
}


vector<vector<State>> ReadBoardFile(string path) {
  ifstream myfile (path);
  vector<vector<State>> board{};
  if (myfile) {
    string line;
    while (getline(myfile, line)) {
      vector<State> row = ParseLine(line);
      board.push_back(row);
    }
  }
  return board;
}

/**
 * Compare the F values of two cells.
 */
bool Compare(vector<int> node1, vector<int> node2) {
  return (node1[2] + node1[3]) > (node2[2] + node2[3]);
  }

/**
 * Sort the two-dimensional vector of ints in descending order.
 */
void CellSort(vector<vector<int>> *v) {
  sort(v->begin(), v->end(), Compare);
}

// Calculate the manhattan distance
int Heuristic(int x1, int y1, int x2, int y2) {
    return abs(x2 - x1) + abs(y2 - y1);
}

/** 
 * Add a node to the open list and mark it as open. 
 */
void AddToOpen(int x, int y, int g, int h, vector<vector<int>> &open, vector<vector<State>> &grid) {
  // Add node to open vector, and mark grid cell as closed.
  open.push_back(vector<int>{x, y, g, h});
  grid[x][y] = State::kClosed;
}

bool CheckValidCell(int x, int y, vector<vector<State>> &grid) {
  return x >=0 && x < grid.size() && y >=0 && y < grid[0].size() && grid[x][y] == State::kEmpty;
}

void ExpandNeighbors(vector<int> &current, int goal[2], vector<vector<int>> &open, vector<vector<State>> &grid) {
  int x1 = current[0];
  int y1 = current[1];
  int g = current[2];

  // directional deltas
  const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

  for (int i = 0; i < 4; i++) {
    int x2 = x1 + delta[i][0];
    int y2 = y1 + delta[i][1];
    if (CheckValidCell(x2, y2, grid)) {
      AddToOpen(x2, y2, g + 1, Heuristic(x2, y2, goal[0], goal[1]), open, grid);
    }
  }
}

vector<vector<State>> Search(vector<vector<State>> board, int start[2], int goal[2]) {

    vector<vector<int>> open {};

    // Initialize the starting node.
    int x = start[0];
    int y = start[1];
    int g = 0;
    int h = Heuristic(x, y, goal[0], goal[1]);
    AddToOpen(x, y, g, h, open, board);

    while (!open.empty()) {
      // Get the next node
      CellSort(&open);
      vector<int> current = open.back();
      open.pop_back();
      x = current[0];
      y = current[1];
      board[x][y] = State::kPath;

      // Check if we're done.
      if (x == goal[0] && y == goal[1]) {
        return board;
      }

    // If we're not done, expand search to current node's neighbors.
    ExpandNeighbors(current, goal, open, board);
    }

    // We've run out of new nodes to explore and haven't found a path.
    cout << "No path found!";
    return vector<vector<State>>{};
}


string CellString(State cell) {
  switch(cell) {
    case State::kObstacle: return "⛰️   ";
    case State::kPath: return "🚗  ";
    case State::kStart: return "🚦  ";
    case State::kFinish: return  "🏁   ";
    default: return "0   "; 
  }
}

void PrintBoard(const vector<vector<State>> board) {
  for (int i = 0; i < board.size(); i++) {
    for (int j = 0; j < board[i].size(); j++) {
      cout << CellString(board[i][j]);
    }
    cout << "\n";
  }
}

#include "test_astarsearch.cpp" // For testing solution

int main() {
    int init[2] = {0, 0};
    int goal[2] = {4, 5};
    vector<vector<State>> board = ReadBoardFile("files/1.board");

    auto solution = Search(board, init, goal);

    solution[init[0]][init[1]] = State::kStart;
    solution[goal[0]][goal[1]] = State::kFinish;

    PrintBoard(solution);
    // Tests
    TestHeuristic();
    TestAddToOpen();
    TestCompare();
    TestSearch();
    TestCheckValidCell();
    TestExpandNeighbors();
}