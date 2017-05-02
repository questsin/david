#include "../lib/Catch/include/catch.hpp"
#include "../src/uci/UCIHandler.h"
#include "../src/uci/UCIEvent.h"

// gui to engine
TEST_CASE( "Making sure commands from GUI to Engine are correctly parsed", "[UCIHandler.parseInput]" ) {
  UCIHandler uciHandler;

  std::map<uint8_t, std::string> commands = {
      {uci::event::NO_MATCHING_COMMAND, "akdjas"},
      {uci::event::NO_MATCHING_COMMAND, "ucii"},
      {uci::event::NO_MATCHING_COMMAND, "sready"},
      {uci::event::NO_MATCHING_COMMAND, "ståp"},
      {uci::event::NO_MATCHING_COMMAND, "sada4g432 fa4f23"},
      {uci::event::UCI,                 "uci"},
      {uci::event::DEBUG,               "debug"},
      {uci::event::ISREADY,             "isready"},
      {uci::event::SETOPTION,           "setoption"},
      {uci::event::REGISTER,            "register"},
      {uci::event::UCINEWGAME,          "ucinewgame"},
      {uci::event::POSITION,            "position"},
      {uci::event::GO,                  "go"},
      {uci::event::STOP,                "stop"},
      {uci::event::PONDERHIT,           "ponderhit"},
      {uci::event::QUIT,                "quit"}
  };

  for (const auto entry : commands) {
    REQUIRE( uciHandler.parseInputForCommand(entry.second) == entry.first );
  }
}

// gui to engine
TEST_CASE( "Making sure arguments from GUI to Engine are correctly parsed", "[UCIHandler.parseInputForArguments]" ) {
  UCIHandler uciHandler;

  std::map<std::string, std::map<std::string, std::string>> inputs = {
      {"uci", {}},
      {"uci this text should be completely ignored by the parser.", {}},

      {"debug", {}},

      {"debug on", {{"on", ""}} },

      {"debug off", {{"off", ""}} },

      {"isready", {} },

      {"setoption", {} },
      {"setoption name", {{"name", ""}} },
      {"setoption name test", {{"name", "test"}} },
      {"setoption name test value", {{"name", "test"}, {"value", ""}} },
      {"setoption name test value lalala", {{"name", "test"}, {"value", "lalala"}} },

      {"register later", {{"later", ""}} },
      {"register name Stefan MK code 349284", {{"name", "Stefan MK"}, {"code", "349284"}} },

      {"position", {} },
      {"position fen moves sadha", {{"fen", ""}, {"moves", "sadha"}} },
      {"position startpos moves sadha", {{"startpos", ""}, {"moves", "sadha"}} }
  };

  for (const auto entry : inputs) { // this runs a few rounds before error.
    auto a = uciHandler.parseInputForArguments(entry.first);
    auto b = entry.second;
    REQUIRE(a == b);
  }
}