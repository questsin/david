//
// Created by anders on 5/3/17.
//

#ifndef PROJECT_SEARCH_H
#define PROJECT_SEARCH_H

#include <limits>
#include <iostream>
#include <string>
#include <map>
#include "chess_ann/bitboard.h"
#include "chess_ann/uci/Parser.h"
#include "chess_ann/uci/UCIEvent.h"
#include <chess_ann/uci/Listener.h>

using ::bitboard::COLOR;

namespace search {

class Search {
public:
    Search(::uci::Listener& uci){};
    void searchInit(/*Pseudo node*/);
    int iterativeDeepening(/*Pseudo node*/);
    int negamax(/*Pseudo node*/);
private:
    int searchScore;
    int /*time[COLOR], inc[COLOR],*/ npmsec, movestogo, depth, movetime, mate, infinite, ponder;
    void uciOutput();
    void uci_go_depth(::uci::arguments_t args);
    void uci_go(uci::arguments_t args);
    void resetSearchValues();

};

    inline void Search::uciOutput() {

    }


}

#endif //PROJECT_SEARCH_H
