#include <string>
#include <vector>

struct Graph {
  Graph(int numVertices, bool trianglesWin);
  int numMoves;
  int numPossibleMoves;
  int turn;
  bool trianglesForWin;
  std::vector<std::vector<int>> adjacencyMatrix;
};

// In namespace instead of struct so that it's not copied every time
namespace GraphOperations {
bool isValidMove(std::shared_ptr<Graph> g, const int &i, const int &j);
int colorEdge(std::shared_ptr<Graph> g, const int &i, const int &j);
int whoseTurnIsNext(const int &currentTurn);
int checkForWinner(std::shared_ptr<Graph> g);
int hasATriangle(std::shared_ptr<Graph> g);
int hasAC5(std::shared_ptr<Graph> g);
bool isTriangle(std::shared_ptr<Graph> g, const int &i, const int &j,
                const int &k);
int checkTieGame(std::shared_ptr<Graph> g);
}
