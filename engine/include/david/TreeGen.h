#pragma once

#include "david/david.h"
#include "david/types.h"
#include <array>
#include "david/bitboard.h"

namespace david {
namespace gameTree {

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
class TreeGen {
 private:
  const type::NeuralNetwork_t& neuralnet;

  // transposition table
  //std::array<NodeCache, constant::MAXMOVES>

  // Game tree
  std::array<type::gameState_t, (constant::MAXMOVES * constant::MAXDEPTH + /*root*/1)> tree;
  int maxDepth;

  // Table to faster access child index of a parent node
  std::array<int, (constant::MAXDEPTH + 1)> depthIndexes;

  // keep track of game history
  std::string startposFEN;
  std::array<std::string, 300> history; // number of MAX moves in a game. EGN => uint8_t * 4
  unsigned int historyIndex{0};

  // Creates EGN moves for each possible move after root node. 1 - 256.
  std::array<std::string, constant::MAXMOVES> EGNMoves; // calculate this after best move, dont waste time.
  int nrOfEGNMoves{-1}; // holds the number of moves generated after root to reduce loop check

 public:

  // Constructors
  TreeGen(const type::NeuralNetwork_t& neuralNetwork);

  // Destructor
  ~TreeGen();

  // public methods
  int /*************/ getGameStateScore(const unsigned int index) const;
  int /*************/ getDepth() const;
  void /************/ setRootNode(const type::gameState_t& gs);
  void /************/ updateRootNodeTo(const int index);
  void /************/ setRootNodeFromFEN(const std::string& FEN);
  void /************/ reset();
  void /************/ setMaxDepth(const int depth);
  void /************/ generateNode(const type::gameState_t& p, type::gameState_t& n, const type::gameState_t c);
  uint16_t /********/ generateChildren(const unsigned int index);
  unsigned int /****/ treeIndex(const uint8_t depth, const uint8_t index) const;
  type::gameState_t   getGameStateCopy(const unsigned int index) const;
  type::gameState_t&  getGameState(const unsigned int index);

  // generate EGN moves for root node
  void generateEGNMoves();

  // change root node based on EGN
  void applyEGNMove(const std::string& EGN);

  // sync engine's board track with GUI
  //void syncGameRecord(); // should this take an array of strings, or just the uci param for moves?

  //
  // inlines
  //
  inline unsigned int getChildIndex(const unsigned int parent, const unsigned int child) const {
    //return this->depthIndexes[parent + 1 + child];
    //utils::yellDeprecated("TreeGen::getChildIndex is not working correctly!!!!");
    if (parent == 0) {
      return 1 + child;
    }
    else if (parent <= constant::MAXMOVES) {
      return constant::MAXMOVES + 1 + child;
    }
    else /*if (parent > constant::MAXMOVES)*/ {
      return ((parent / constant::MAXMOVES) + 1) * constant::MAXMOVES + 1 + child;
    }
  }

};

}
}