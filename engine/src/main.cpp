// check if any of the development flags are used
#if defined(DAVID_DEBUG) || defined(DAVID_TEST) || defined(DAVID_BENCHMARK) || defined(DAVID_DEVELOPMENT)
#define DAVID_FLAG_DEVELOPMENT
#endif

//
// Supported compilers
// * GCC 5.1 and higher
//
#if defined(__GNUC__) && __GNUC__ > 5
#define DAVID_SUPPORTED_COMPILER
#endif

// if the compiler is not officially supported, create an error
// This can be removed, but isn't recommended. Just remove the comments below:
//#define I_DONT_CARE_LET_ME_THROUGH
#if !defined(DAVID_SUPPORTED_COMPILER) && !defined(I_DONT_CARE_LET_ME_THROUGH)
#error Compiler type or version is not supported.
#endif


// includes
#include <iostream>
#include <cassert>

#include "david/types.h"
#include <david/ChessEngine.h>
#include "david/utils/utils.h"
#include "david/EngineMaster.h"

#include "david/MoveGeneration.h"
#include "david/utils/logger.h"


void train() {}

void fight() {
  ::david::EngineMaster em(::david::constant::ANNFile);

  int mainEngineID = em.spawnEngine();
  int opponentEngineID = em.spawnEngine();
  int battleID = em.battle(mainEngineID, opponentEngineID);

  if (em.battleWinner(battleID, mainEngineID)) {
    std::cout << mainEngineID << std::endl;
    //em.trainUntilWinner(mainEngineID, opponentEngineID);
  }
  std::cout << "lost" << std::endl;
}

void gui() {
  std::cout << "David Chess Engine v1.0.0" << std::endl;
  ::david::ChessEngine engine("float_ANNFile_6_83_1_1497360313.net");
  engine.setNewGameBoard(::david::constant::FENStartPosition);
  engine.enableUCIMode();
  engine.linkUCICommands();
  engine.configureUCIProtocol();
  engine.activateUCIProtocol();
}

// TODO: Should support changing mode without recompiling.
int main (/*int argc, char * argv[]*/)
{
  // Make sure its not some weird "cpu architecture".
  assert(sizeof(uint8_t)  == 1);
  assert(sizeof(uint16_t) == 2);
  assert(sizeof(uint32_t) == 4);
  assert(sizeof(uint64_t) == 8);


  const std::string mode = "uci"; // uci, fight, train, perft. Default: "uci"


  if (mode == "fight") {
    fight();
  }
  else if (mode == "uci") {
    gui();
  }
  else if (mode == "train") {
    train();
  }
  else if (mode == "perft") {
    ::utils::perft(5);
  }

  // Close program with exit code 0 (UCI: after all threads have joined.)
  return 0;
}

