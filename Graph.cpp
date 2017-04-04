#include "Graph.hpp"
#include <numeric>

// Graph constructor. Graphs are represented by an adjacency matrix.
// If the entry adjacencyMatrix[i][j] == 0, neither play has colored that edge.
// If the entry adjacencyMatrix[i][j] == 1, player 1 has colored that edge.
// If the entry adjacencyMatrix[i][j] == 2, player 2 has colored that edge.
Graph::Graph(int numVertices, bool trianglesWin)
    : numMoves(0), numPossibleMoves(numVertices * (numVertices - 1) / 2),
      turn(1), trianglesForWin(trianglesWin) {
  adjacencyMatrix.resize(numVertices);

  for (auto &&row : adjacencyMatrix) {
    row.assign(numVertices, 0);
  }
}

// If neither player has colored that edge yet, then coloring an edge at
// adjacencyMatrix[i][j] is a valid move
bool GraphOperations::isValidMove(std::shared_ptr<Graph> g, const int &i,
                                  const int &j) {
  if (g->adjacencyMatrix[i][j] == 0) {
    return true;
  } else {
    return false;
  }
}

// Set adjacencyMatrix[i][j] = person whose turn it is.
// Make it the next person's turn
// Returns:
//    -1: neither player has won
//    0: neither player can win (tied game)
//    1: if player 1 won
//    2: if player 2 won
int GraphOperations::colorEdge(std::shared_ptr<Graph> g, const int &i,
                               const int &j) {
  g->adjacencyMatrix[i][j] = g->adjacencyMatrix[j][i] = g->turn;
  ++g->numMoves;
  g->turn = whoseTurnIsNext(g->turn);
  return checkForWinner(g);
}

// If it's players 1 turn currently, the next persons turn is player 2, and
// vice versa.
int GraphOperations::whoseTurnIsNext(const int &currentTurn) {
  if (currentTurn == 1) {
    return 2;
  } else {
    return 1;
  }
}

// If the winning move is to force a triangle, check if there is a triangle.
// If the winning move is to force a C5, check if there is a C5.
int GraphOperations::checkForWinner(std::shared_ptr<Graph> g) {
  if (g->trianglesForWin) {
    return hasATriangle(g);
  } else {
    return hasAC5(g);
  }
}

// Check all vertices to see if there is a triangle.
int GraphOperations::hasATriangle(std::shared_ptr<Graph> g) {
  auto numVertices = g->adjacencyMatrix.size();
  for (int i = 0; i < numVertices - 2; ++i) {
    for (int j = i + 1; j < numVertices - 1; ++j) {
      for (int k = j + 1; k < numVertices; ++k) {
        if (isTriangle(g, i, j, k)) {
          // If there is a triangle made by a player, their opponent won
          return whoseTurnIsNext(g->adjacencyMatrix[i][j]);
        }
      }
    }
  }
  // There are no triangles.
  return checkTieGame(g);
}

// The game is tied if all the possible moves have been made.
int GraphOperations::checkTieGame(std::shared_ptr<Graph> g) {
  if (g->numMoves == g->numPossibleMoves) {
    return 0;
  }
  return -1;
}

// Check three specific vertices to see if they form a triangle of edges colored
// by the same person.
bool GraphOperations::isTriangle(std::shared_ptr<Graph> g, const int &i,
                                 const int &j, const int &k) {
  if (g->adjacencyMatrix[i][j] != 0 &&
      g->adjacencyMatrix[i][j] == g->adjacencyMatrix[j][k] &&
      g->adjacencyMatrix[i][j] == g->adjacencyMatrix[i][k]) {
    return true;
  }
  return false;
}

// Checks to see if there is a C5 in the graph.
// This is a terrible way to do it (checking the first five vertices of all
// permutations of the vertices to see if they form a C5)
// If you have a better idea, let me know.
int GraphOperations::hasAC5(std::shared_ptr<Graph> g) {
  std::vector<int> vertices;
  vertices.resize(g->adjacencyMatrix.size());
  std::iota(std::begin(vertices), std::end(vertices), 0);
  do {
    auto i = vertices[0];
    auto j = vertices[1];
    auto k = vertices[2];
    auto l = vertices[3];
    auto m = vertices[4];
    if (g->adjacencyMatrix[i][j] != 0 &&
        g->adjacencyMatrix[i][j] == g->adjacencyMatrix[j][k] &&
        g->adjacencyMatrix[j][k] == g->adjacencyMatrix[k][l] &&
        g->adjacencyMatrix[k][l] == g->adjacencyMatrix[l][m] &&
        g->adjacencyMatrix[l][m] == g->adjacencyMatrix[i][m]) {
      return whoseTurnIsNext(g->adjacencyMatrix[i][j]);
    }
  } while (std::next_permutation(vertices.begin(), vertices.end()));

  // There are no C5s
  return checkTieGame(g);
}
