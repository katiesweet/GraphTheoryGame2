#include <iostream>
#include <vector>

#include "Graph.hpp"

// Desciption:
//    There are three possible outcomes for every game state:
//     1.  If it's your turn, and there is one potential move you can make
//         that will guarantee your victory, then you have a winning strategy.
//     2. If it's your turn and you don't have a winning strategy but there is a
//         move you can make that will determine a tie, then you can tie.
//     3. If it's your turn and you have neither of the above options, your
//         opponent has a winning strategy.
// Arguments:
//    g: A graph whose winner needs to be determined
//    winnersOfEachChild: a vector of the winners from each child's move
// Return Value:
//    Who has a winning strategy (if either) at this point.
//    0 if there is no determined winner for that branch
//    1 if player 1 can always win from this spot
//    2 if player two can always win from this spot
int determineWinner(std::shared_ptr<Graph> g,
                    const std::vector<int> &winnersOfEachChild) {
  int tiePossibilities = 0;
  for (auto &&childsWinner : winnersOfEachChild) {
    if (childsWinner == g->turn) {
      return childsWinner;
    } else if (childsWinner == 0) {
      ++tiePossibilities;
    }
  }
  if (tiePossibilities > 0) {
    return 0;
  }
  return GraphOperations::whoseTurnIsNext(g->turn);
}

// Description:
//    Colors the edge in the graph specified, and checks if that move caused a
//    winner. If there isn't a winner, it recurses through all the possible
//    moves that the next player can make.
// Args:
//    g: The parent graph; does not have a determined winner.
//    i and j: the vertices the edge to be colored is incident to.
// Returns:
//    Who has a winning strategy (if either) at this point.
//    0 this branch results in a tie (if both players play optimally)
//    1 if player 1 can always win from this spot
//    2 if player two can always win from this spot
int recurseAllMoves(std::shared_ptr<Graph> g, int i, int j) {
  auto moveResult = GraphOperations::colorEdge(g, i, j);

  // Check is move was a winning move (if it was, return that winner)
  if (moveResult >= 0) {
    return moveResult;
  }

  // Not a winning move. Recurse through all possible moves the next player can
  // make.
  std::vector<int> winnersOfEachChild;
  auto numVertices = g->adjacencyMatrix.size();
  for (int i = 0; i < numVertices - 1; ++i) {
    for (int j = i + 1; j < numVertices; ++j) {
      if (GraphOperations::isValidMove(g, i, j)) {
        auto newGraph = std::make_shared<Graph>(*g);
        winnersOfEachChild.push_back(recurseAllMoves(newGraph, i, j));
      }
    }
  }
  return determineWinner(g, winnersOfEachChild);
}

// Description:
//    Prints the result of the program.
// Arguments:
//    winner : who has won the game (if either player)
void printResult(const int &winner) {
  if (winner > 0) {
    std::cout << "Player " << winner << " has a winning strategy!" << std::endl;
  } else {
    std::cout << "Neither player has a winning strategy" << std::endl;
  }
}

// Vars:
//    numVertices: number of vertices in graph
//    trianglesForWin: True if triangle forces win. False if C5 forces win.
//    startingGraph: Basic graph that forms root of game tree
int main() {
  const int numVertices = 6;
  const int trianglesForWin = true;
  auto startingGraph = std::make_shared<Graph>(numVertices, trianglesForWin);

  GraphOperations::colorEdge(startingGraph, 0, 1);
  // GraphOperations::colorEdge(startingGraph, 1, 2);
  // GraphOperations::colorEdge(startingGraph, 2, 3);
  auto winner = recurseAllMoves(startingGraph, 1, 2);
  printResult(winner);

  return 0;
}
