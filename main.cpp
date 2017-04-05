#include <chrono>
#include <future>
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
  const bool trianglesForWin = false;

  // Run the result if Player 2 plays adjacent first
  auto startingGraphAdjacent =
      std::make_shared<Graph>(numVertices, trianglesForWin);
  GraphOperations::colorEdge(startingGraphAdjacent, 0, 1);
  auto futWinnerAdjacent = std::async(std::launch::async, recurseAllMoves,
                                      startingGraphAdjacent, 1, 2);

  // Run the result if Player 2 plays non-adjacent first
  auto startingGraphNonAdjacent =
      std::make_shared<Graph>(numVertices, trianglesForWin);
  GraphOperations::colorEdge(startingGraphNonAdjacent, 0, 1);
  auto futWinnerNonAdjacent = std::async(std::launch::async, recurseAllMoves,
                                         startingGraphNonAdjacent, 2, 3);
  auto start = std::chrono::system_clock::now();

  std::cout << "Waiting..." << std::endl;
  futWinnerAdjacent.wait();
  std::cout << std::endl
            << "Winner if player two plays adjacent is: " << std::endl;
  printResult(futWinnerAdjacent.get());

  std::cout << "Waiting some more..." << std::endl;
  futWinnerNonAdjacent.wait();
  std::cout << std::endl
            << "Winner if player two plays non-adjacent is: " << std::endl;
  printResult(futWinnerNonAdjacent.get());

  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(end - start);
  std::cout << "This program took: " << elapsed.count() << " min\n";
  return 0;
}
