#pragma once


// local dependencies
#include "david/types.h"
#include "david/bitboard.h"
#include "david/GameTree.h"

// system dependencies
#include <string>


// forward declarations
#include "david/forwards/EngineContext.h"
#include "uci/forwards/Listener.h"


namespace david {

/**
 * Search class is responsible for searching through a gamestate tree
 * bestScore is stored in object, this is the heuristic og a move
 * bestMove stores the gamestate which is the best throughout.
 * Should return a fen string or something to UCI
 */
class Search {
 public:
  Search(); // This can be used for unit testing and benchmarking.
  Search(type::engineContext_ptr ctx);
  type::gameState_t searchInit(type::gameState_t node);
  int iterativeDeepening(type::gameState_t node);
  int negamax(unsigned int index, int alpha, int beta, int depth, int iterativeDepthLimit);
  void setAbort(bool isAborted);
  void setComplete(bool isComplete);

  //Test/debug
  int returnDepth();
  int returnTimeToSearch();
  int returnScore();
  bool returnComplete();
  void performanceTest(type::gameState_t& node, int iterations);

  // forwards protocol methods, this can be used in unit testing
  void stopSearch();
  void quitSearch();
  void setDepth(int depth);
  void setSearchMoves(std::string moves);
  void setWTime(int wtime);
  void setBTime(int btime);
  void setWinc(int winc);
  void setBinc(int binc);
  void setMovesToGo(int movestogo);
  void setNodes(int nodes);
  void setMoveTime(int movetime);
  void setMate(int mate);
  void setInfinite(int mate); // bool ?
  void setPonder(int ponder); // bool ?
  void setDifficulty(int difficulty);

  clock_t startTime;

 private:
  int depth;
  int movestogo;
  int movetime;
  int mate;
  int infinite;
  int ponder;
  int wtime;
  int btime;
  int winc;
  int binc;
  int nodes;
  std::string searchMoves;
  int searchScore;
  type::engineContext_ptr engineContextPtr;
  type::gameState_t bestMove;
  type::gameTree_t gt;
  int /*time[COLOR], inc[COLOR],*/ npmsec;
  //void uciOutput();
  void resetSearchValues();
  bool isAborted;
  bool isComplete;
  bool debug;
  int nodesSearched;
  std::vector<int> expanded;
};





}