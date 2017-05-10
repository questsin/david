//
// Created by anders on 5/5/17.
//

#ifndef CHESS_ANN_GAMETREE_H
#define CHESS_ANN_GAMETREE_H

#include "chess_ann/bitboard.h"
#include <memory>
#include <iomanip>
#include "chess_ann/Context.h"

namespace gameTree {
using ::bitboard::gameState;

typedef std::shared_ptr<gameState> nodePtr;


/**
 * This class instance lives through the whole engine lifetime.
 *
 * Used to generate nodes from a gameState node.
 *
 * Number of levels to be generated or nr of nodes can be specified.
 *
 * This class starts always on the leftmost node, generates children and then moves
 * to the neighbour node to his right. If there are no neighbour node at this level,
 * it's either skipped if the parent discovered chess mate or the generating is finished.
 */
class GameTree {
 private:
  // previous can be used for en passant
  nodePtr previous;// in case a player regrets their move?
  nodePtr current; // this represent the active game board
  int maxNumberOfNodes; // nodes in memory

  void getNumberOfNodes(nodePtr node, int& counter);
  void getDepth(nodePtr node, int& depth);

  std::shared_ptr<chess_ann::Context> context;

 public:
  GameTree(std::shared_ptr<chess_ann::Context> context);
  GameTree(std::shared_ptr<chess_ann::Context> context, std::shared_ptr<gameState> node);
  ~GameTree();
  void reset();
  void reset(nodePtr node);
  void resetChildren(nodePtr node);
  void newRootNode(nodePtr node);
  nodePtr regretNewRootNode();
  void setMaxNumberOfNodes(int n);
  int getMaxNumberOfNodes();
  void generateNodes();
  nodePtr generateNode(nodePtr parent, gameState child);
  nodePtr getCurrentNode();
  int getNumberOfNodes();
  void generateChildren(nodePtr node);
  void sortChildren(nodePtr node);
  int getDepth();
  void printAllScores(nodePtr root);
  
};

}

#endif //CHESS_ANN_GAMETREE_H
