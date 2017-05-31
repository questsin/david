#include "catch.hpp"
#include "david/uci/Listener.h"
#include "david/uci/events.h"

TEST_CASE( "Functions are added and being correctly called", "[Listener.addListener]" ) {
  david::uci::Listener uciListener;

  std::array<bool, 100> checks{false};

  auto loopLength = checks.size();
  for (int i = 0; i < loopLength; i++) {
    auto function = [&checks, i](david::uci::arguments_t args){
      checks[i] = true;
    };

    uciListener.addListener(david::uci::event::TEST, function);
  }

  // Fire all the events
  uciListener.fireEvent(david::uci::event::TEST);

  // verify the changes
  for (int i = 0; i < loopLength; i++) {
    REQUIRE( checks[i] == true );
  }
}

TEST_CASE( "Functions are added and being removed", "[Listener.addListener, Listener.removeListener]" ) {
  david::uci::Listener uciListener;

  std::array<bool, 100> checks{false};
  std::vector<int> listenerIDs;

  auto loopLength = checks.size();
  for (int i = 0; i < loopLength; i++) {
    auto function = [&checks, i](david::uci::arguments_t args){
      checks[i] = true;
    };

    int listenerID = uciListener.addListener(david::uci::event::TEST, function);
    listenerIDs.push_back(listenerID);
  }

  // Fire all the events
  uciListener.fireEvent(david::uci::event::TEST);

  // verify the changes
  for (int i = 0; i < loopLength; i++) {
    REQUIRE( checks[i] == true );
  }

  // now delete all the functions
  for (const int id : listenerIDs) {
    REQUIRE( uciListener.removeListener(id) );
  }

  // reset all the variables in the array to false, and fire the event again.
  // This should not be true afterwards
  for (int i = 0; i < loopLength; i++) {
    checks[i] = false;
  }

  // Fire all the events
  uciListener.fireEvent(david::uci::event::TEST);

  // verify that nothing has changed
  for (int i = 0; i < loopLength; i++) {
    REQUIRE( checks[i] == false );
  }
}
