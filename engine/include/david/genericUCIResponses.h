#pragma once

#include "david/uci/definitions.h"
#include "david/uci/Response.h"
#include "engineInformation.h"

namespace david {
namespace uciResponses {
// http://stackoverflow.com/questions/17003561/using-the-universal-chess-interface


/**
 * Send id information to gui.
 *
 * @param args
 */
auto id = [&]() {
  uci::send("id name " + engineInformation::ENGINE_NAME);
  uci::send("id author " + engineInformation::ENGINE_AUTHOR);
};

auto uciok = [&]() {
  uci::send("uciok");
};

auto option = [&]() {
  //::uci::send("option name Hash type spin default 1 min 1 max 128");
};

auto responseToUCI = [&](const uci::arguments_t args) {
  id();
  option();
  uciok();
};

// TODO: This needs to wait until everything has been setup correctly.
auto responseToISREADY = [&](const uci::arguments_t args) {
  uci::send("readyok");
};

}
}