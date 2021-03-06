#pragma once

// local dependencies
#include "david/types.h"
#include "david/bitboard.h"
#include "david/ANN/ANN.h"
#include "david/Search.h"
#include "david/TreeGen.h"

// git submodule dependencies
#include "uci/Listener.h"

// forward declarations
#include "david/forwards/Search.h"


namespace david {

/**
 * A player object that will represent who this engine is on the board.
 */
struct Player {
  bool isWhite = true;
};


class ChessEngine {
  bool UCIProtocolActivated;
  int wtime;
  int btime;
  int timeLeft;

  // this is sent to other classes so they can communicate with each other
  type::NeuralNetwork_t neuralNet;
  type::TreeGen_t treeGen;
  type::Search_t search;
  type::UciProtocol_t UCI;


  type::gameState_t currentGameState;

  Player player;

  volatile bool uciMode;
  std::thread searchThread;

 public:

  ChessEngine();
  ChessEngine(Player self);
  ChessEngine(const std::string ANNFile);
  ChessEngine(Player self, const std::string ANNFile);
  ~ChessEngine();

  void linkUCICommands();

  /**
   * Enable uci mode
   */
  void enableUCIMode();

  /**
   * Adds typical UCI responses to the engine
   */
  void configureUCIProtocol();

  /**
   * Actives the UCI protocol, and keeps it running in another thread.
   */
  void activateUCIProtocol();

  /**
   * Check if UCI is activated on this engine or not.
   *
   * @return bool if UCI protocol has been activated.
   */
  bool hasInitiatedUCIProtocol();

  /**
   * Gives this engine a UCI Protocol command, but does not return responses.
   * @deprecated, hasn't been implemented yet.
   */
  void sayUCICommand(std::string command);

  /**
   * Retrieve the ANN file this engine instance uses for evaluating game boards.
   * @return std::string absolute path of ann file.
   */
  std::string getANNFile();

  void setANNFile(const std::string ANNFile);

  /**
   * Check if there exists a ANNFile
   */
  bool hasANNFile();

  /**
   * Check if there exists a ANN instance
   */
  bool hasANNInstance();

  /**
   * Check if this engine plays as white
   */
  constexpr bool isWhite() const
  {
    return this->player.isWhite;
  }

  /**
   * Start the ANN from given files.
   */
  void createANNInstance();

  /**
   * Run the boards through the trained neural network to get a generated output.
   *
   * @param board ::gameTree::gameState, of shared_ptr type
   * @return int board evaluation
   */
  int ANNEvaluate(type::gameState_t& board);


  /**
   * Run the boards through the trained neural network to get a generated output.
   *
   * @param fen std::string FEN(Forsyth–Edwards Notation)
   * @return int board evaluation
   */
  int ANNEvaluate(std::string fen);

  /**
   * Used to reset old data, and construct a new fresh game for this engine.
   *
   * @param fen a FEN string, must be correctly parsed otherwise worlds will collide.
   */
  void setNewGameBoard(const std::string fen);

  /**
   * Update this players color, color means what piece this player can move..
   *
   * @param color
   */
  void setPlayerColor(const bool white);

  /**
   * Get the game node from a engine. Will be known as the root node for the search class.
   *
   * @return shared_ptr of gameState
   */
  type::gameState_t getGameState();

  /**
   * Sets the game state based on a node.
   * Used when updating board states during battles.
   *
   * @param state shared_ptr of a gameState
   * @return true if the state was updated
   */
  bool setGameState(type::gameState_t& gs);

  /**
   * Check if the engine has lost.
   *
   * @return true on losing
   */
  bool lost();

  /**
   * Find the best move, and update the current game state.
   */
  void findBestMove();


};

}