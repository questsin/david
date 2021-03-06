#pragma once

#include "david/david.h"
#include "david/types.h"
#include "david/bitboard.h"
#include <assert.h>
#include "david/utils/gameState.h"

#ifdef DAVID_TEST
#include "david/MoveGenTest.h"
#endif

namespace david {

namespace movegen {

}

class MoveGen {
#ifdef DAVID_TEST
  friend class MoveGenTest;
#endif
 public:
  MoveGen(const type::gameState_t& gs);

  // run all the psuedo move generators
  void runAllMoveGenerators();

  // generate moves for every piece type
  void generatePawnMoves();   // pawns
  void generateRookMoves();   // rooks
  void generateKnightMoves(); // knights
  void generateBishopMoves(); // bishops
  void generateQueenMoves();  // queens
  void generateKingMoves();   // kings


  //
  // Template functions
  //

  // extract all possible sub gameStates
  template <size_t N>
  uint16_t generateGameStates(std::array<type::gameState_t, N>& arr, const unsigned long start = 0, const unsigned long stop = N - 1)
  {
    if (stop >= N) {
      std::__throw_runtime_error("stop param is larger than array size");
    }

    // #########################################################################
    uint16_t index_gameStates = 0;
    // generate all the bitboards

    this->generateRookMoves();
    this->generateKnightMoves();
    this->generateBishopMoves();
    this->generateQueenMoves();
    this->generateKingMoves();
    this->generatePawnMoves(); // Must be last due to promotions

    //generate gameStates based on moves
    const auto nrOfPieceTypes = this->moves.size();
    for (unsigned long pieceType = 0; pieceType < nrOfPieceTypes; pieceType++) {

      // for every pieceType
      for (unsigned long board = 0; board < this->index_moves[pieceType]; board++) {

        //make sure the king hasn't been captured.
        if ((this->state.piecesArr[::david::constant::index::king][1] & this->moves[pieceType][board]) > 0) {
          continue;
        }

        // create a child game state
        type::gameState_t gs = this->reversedState;
        gs.piecesArr[pieceType][1] = this->moves[pieceType][board];       // the colour that just moved. now opponent.

        // update moved piece
        gs.piecess[1] ^= this->state.piecesArr[pieceType][0];
        gs.piecess[1] ^= this->moves[pieceType][board];


        // Check for capture, and destroy captured piece!
        if ((this->moves[pieceType][board] & this->state.piecess[1]) > 0) {
          const uint8_t attackedPiecePosition = ::utils::LSB(this->moves[pieceType][board] & this->state.piecess[1]);

          for (auto& bbArr : gs.piecesArr) {
            // since gs has the opposite indexes, use 0 in stead of 1.
            if (::utils::bitAt(bbArr[0], attackedPiecePosition)) {
              ::utils::flipBitOff(bbArr[0], attackedPiecePosition);
              break;
            }
          }

          // update pieces
          ::utils::flipBitOff(gs.piecess[0], attackedPiecePosition);
        }

          // en passant capture.
        else if (pieceType == 0 && this->state.enPassant > 15 && ::utils::bitAt(this->moves[0][board], this->state.enPassant)) {
          ::utils::flipBitOff(gs.piecesArr[0][0], this->state.enPassantPawn);
          ::utils::flipBitOff(gs.piecess[0], this->state.enPassantPawn);
          gs.passant = true;
        }

          // identify a castling situation
          // TODO: verify that the new positions cannot be attacked!
        else if (pieceType == ::david::constant::index::king) {
          // TODO-castling1a: assumption, castling happens early, so i assume that a friendly rook,
          // TODO-castling1b: is not on the opposite side, vertically. From the castling rook
          // TODO-castling1c: OTHERWISE. this is a flaw

          // king side castling
          if (this->state.kingCastlings[0] && (gs.piecesArr[5][1] & 144115188075855874ULL) > 0) {
            uint8_t castlePos = ::utils::LSB(gs.piecesArr[::david::constant::index::rook][1] & 72057594037927937ULL);
            ::utils::flipBitOff(gs.piecesArr[::david::constant::index::rook][1], castlePos);

            ::utils::flipBitOn(gs.piecesArr[::david::constant::index::rook][1], castlePos << 2);
            gs.kingCastlings[1] = false;
            gs.queenCastlings[1] = false;

            ::utils::flipBitOff(gs.piecess[1], castlePos);
            ::utils::flipBitOn(gs.piecess[1], castlePos << 2);

#ifdef DAVID_TEST
            gs.castled = true;
#endif
          }

          else if (this->state.queenCastlings[0] && (gs.piecesArr[5][1] & 2305843009213693984ULL) > 0) {
            uint8_t castlePos = ::utils::LSB(gs.piecesArr[::david::constant::index::rook][1] & 9223372036854775936ULL);
            ::utils::flipBitOff(gs.piecesArr[::david::constant::index::rook][1], castlePos);

            ::utils::flipBitOn(gs.piecesArr[::david::constant::index::rook][1], castlePos >> 3);
            gs.queenCastlings[1] = false;
            gs.kingCastlings[1] = false;

            ::utils::flipBitOff(gs.piecess[1], castlePos);
            ::utils::flipBitOn(gs.piecess[1], castlePos >> 3);

#ifdef DAVID_TEST
            gs.castled = true;
#endif
          }

            // the king has moved so disable castling for that colour
          else {
            gs.queenCastlings[1] = false;
            gs.kingCastlings[1] = false;
          }
        }

          // If a rook move, disable that sides castling rights
        else if (pieceType == ::david::constant::index::rook) {
          // king side
          if (this->state.kingCastlings[0] && (gs.piecesArr[::david::constant::index::rook][1] & 72057594037927937ULL) == 0) {
            // there is no rook at its home anymore. however what if theres a friendly rook at the hostile rank?
            gs.kingCastlings[1] = false;
          }
            // queen side
          else if (this->state.queenCastlings[0] && (gs.piecesArr[::david::constant::index::rook][1] & 9223372036854775936ULL) == 0) {
            // there is no rook at its home anymore. however what if theres a friendly rook at the hostile rank?
            gs.queenCastlings[1] = false;
          }
        }

        // a piece was promoted, so remove the pawn that was sacrificed for this promotion
        if (pieceType > 0 && pieceType < 5 && (this->moves[pieceType][board] & this->state.piecesArr[0][0]) > 0) {
          const type::bitboard_t pawnBoard = this->moves[pieceType][board] & this->state.piecesArr[0][0];

          gs.piecesArr[0][1] ^= pawnBoard;
          gs.piecesArr[pieceType][1] ^= pawnBoard;
#ifdef DAVID_TEST
          gs.promoted = true;
#endif
        }

        // complete board merge
        gs.combinedPieces = gs.piecess[0] | gs.piecess[1];


        // en passant record
        // TODO: slow
        if (pieceType == ::david::constant::index::pawn && !gs.passant) {
          auto before = this->state.piecesArr[0][0];
          auto now = gs.piecesArr[0][1];
          const auto diff = (before ^ now);
          before &= diff;
          now &= diff;

          if ((before & 71776119061282560) > 0 && (now & 1099494850560) > 0) {
            // its en passant
            gs.enPassantPawn = ::utils::LSB(now);
            gs.enPassant = this->state.isWhite ? ::utils::LSB(now >> 8) : ::utils::LSB(now << 8);
          }
        }

        // check?
        if (this->dangerousPosition(gs.piecesArr[::david::constant::index::king][1], gs, 0)) {
          continue;
        }

        // store the completed gamestate
        // TODO: fullstep, etc.

        // half step
        // Validate half moves
        if (!::utils::gameState::isHalfMove(
            this->state.piecess[0],
            gs.piecess[0],
            this->state.piecesArr[0][0],
            this->state.piecesArr[0][1],
            gs.piecesArr[0][0],
            gs.piecesArr[0][1]
        )) {
          gs.halfMoves = 0;
        }

        gs.isWhite = !this->state.isWhite;

        // is this new game state in check?
#ifdef DAVID_TEST
        if (this->dangerousPosition(gs.piecesArr[::david::constant::index::king][0], gs)) {
        gs.isInCheck = true;
      }
#endif

        // valid move, add it to the record.
        arr[start + index_gameStates++] = gs;
      }
    }

    // #########################################################################

    return index_gameStates;
  }

  // how many possible moves can be generated from this state?
  uint16_t nrOfPossibleMoves()
  {
    return 0;
  }

  type::gameState_t generateAttacks() {
    type::gameState_t gs = this->state;

    type::bitboard_t que = 0ULL;

    // pawns
    for (uint8_t j = 0; j < 2; j++) {
      que = gs.piecesArr[0][j];
      for (uint8_t i = ::utils::LSB(que); que != 0; i = ::utils::NSB(que, i)) {
        gs.piecesArr[0][j] |= this->generatePawnPaths(i, gs, j == 1);
      }
    }

    // knights
    for (uint8_t j = 0; j < 2; j++) {
      que = gs.piecesArr[::david::constant::index::knight][0];
      for (uint8_t i = ::utils::LSB(que); que != 0; i = ::utils::NSB(que, i)) {
        gs.piecesArr[::david::constant::index::knight][0] |= this->generateKnightAttack(i, j == 1);
      }
    }

    // rooks
    for (uint8_t j = 0; j < 2; j++) {
      que = gs.piecesArr[::david::constant::index::rook][0];
      for (uint8_t i = ::utils::LSB(que); que != 0; i = ::utils::NSB(que, i)) {
        gs.piecesArr[::david::constant::index::rook][0] |= this->generateRookAttack(i, gs, j == 1);
      }
    }

    // bishop
    for (uint8_t j = 0; j < 2; j++) {
      que = gs.piecesArr[::david::constant::index::bishop][0];
      for (uint8_t i = ::utils::LSB(que); que != 0; i = ::utils::NSB(que, i)) {
        gs.piecesArr[::david::constant::index::bishop][0] |= this->generateDiagonals(i, gs, j == 1);
      }
    }

    // queen
    for (uint8_t j = 0; j < 2; j++) {
      que = gs.piecesArr[::david::constant::index::queen][0];
      for (uint8_t i = ::utils::LSB(que); que != 0; i = ::utils::NSB(que, i)) {
        gs.piecesArr[::david::constant::index::queen][0] |= this->generateDiagonals(i, gs, j == 1) | this->generateRookAttack(i, gs, j == 1);
      }
    }

    // king
    for (uint8_t j = 0; j < 2; j++) {
      que = gs.piecesArr[::david::constant::index::king][0];
      for (uint8_t i = ::utils::LSB(que); que != 0; i = ::utils::NSB(que, i)) {
        gs.piecesArr[::david::constant::index::king][0] |= this->generateKingAttack(i, j == 1);
      }
    }

    // TODO: don't let pieces move into places where their king goes into check.

    return gs;
  }


 private:
#ifdef DAVID_TEST
  bool printedState{false};
  bool printMoves{false};
#endif

  // the current board status of the game
  const type::gameState_t& state;
  type::gameState_t reversedState;

  // xray attack path for king
  //type::bitboard_t xRayRookPaths;
  //type::bitboard_t xRayDiagonalPaths;

  // moves being generated from the state, for the active colour only.
  // 0 pawns
  // 1 rooks
  // 2 knight
  // 3 bishop
  // 4 queen
  // 5 king
  std::array<std::array<type::bitboard_t, 35>, 6> moves = {{0}}; // initiator list?
  std::array<unsigned long, 6> index_moves = {{0}}; // index for every type

  type::bitboard_t hostileAttackPaths_queen = 0ULL - 1;
  type::bitboard_t hostileAttackPaths_knight = 0ULL - 1;
  type::bitboard_t hostileAttackPaths_pawn = 0ULL - 1;

  /**
   * Remove the parts of the Psuedo attack path that is illegal / invalid.
   * This only affects the path north of the piece.
   *
   * @param friendlyBlockers
   * @param northArea
   * @param psuedoPath
   * @return
   */
  inline type::bitboard_t extractLegalNorthPath(
      const type::bitboard_t psuedoPath,
      const type::bitboard_t friendlyBlockers,
      const type::bitboard_t hostiles,
      const type::bitboard_t northArea,
      const uint8_t offset = 7
  ) const {
    // find all path blockers
    type::bitboard_t northBlockers = (friendlyBlockers | ((hostiles & northArea) << offset)) & northArea; // & northArea);

    if (northBlockers > 0) {
      // find the area south of the closest blocker
      type::bitboard_t northBlockersArea = ::utils::indexToBitboard(::utils::LSB(northBlockers)) - 1;

      // get only legal moves (north of piece) that's before the first blocker.
      return psuedoPath & northBlockersArea;
    }
    else {
      return psuedoPath;
    }
  }

  //TODO sometimes, when close to a enemy. neighbour square (north). the path ud doesnt get added..
  inline type::bitboard_t extractLegalSouthPath(
      const type::bitboard_t psuedoPath,
      const type::bitboard_t friendlyBlockers,
      const type::bitboard_t hostiles,
      const type::bitboard_t southArea,
      const uint8_t offset = 7
  ) const {
    // find all southern blockers
    type::bitboard_t southBlockers = (friendlyBlockers | ((hostiles & southArea) >> offset)) & southArea;

    if (southBlockers > 0) {
      // get the area of illegal moves, starting from the first blocker downwards to index 0.
      type::bitboard_t southBlockersArea = ::utils::indexToBitboard(::utils::MSB(southBlockers) + 1) - 1;

      // get the legal path that's outside the illegal area.
      return psuedoPath & (~southBlockersArea);
    }
    else {
      return psuedoPath;
    }
  }

  /**
   * Create promotions
   */
   inline void generatePromotions (const type::bitboard_t board, const type::bitboard_t pawn) {
    // add every option
    for (uint8_t pieceType = 1/*skip Pawn*/; pieceType < 5/*skip king*/; pieceType++) {
      // for every promotion, the pawn position is added to its board.
      // so u can just quickly check if any of the set bits overlap with any of the pawn board.
      this->moves[pieceType][this->index_moves[pieceType]++] = pawn | board | this->state.piecesArr[pieceType][0];
    }
  }

  /**
   * generate pawn moves for a given index
   *
   * On promotions, the promoted pieces are added directly to the move array
   */
  inline type::bitboard_t generatePawnPaths (const uint8_t index, const type::gameState_t& gs, const bool hostile = false) {
    const auto pieceBoard = ::utils::indexToBitboard(index);
    const auto hostiles = gs.piecess[hostile ? 0 : 1];
    const auto friendly = gs.piecess[hostile ? 1 : 0];
    const auto allowedSquares = (~this->state.combinedPieces) ^ ::utils::indexToBitboard(index);
    const bool whiteMoving = gs.isWhite && !hostile;

    type::bitboard_t board = 0ULL;
    type::bitboard_t attacks = ::utils::constant::pawnAttackPaths[index];

    if (whiteMoving) {
      // move one and two squares up if possible
      board = ((pieceBoard << 8) | ((65280ULL & pieceBoard) << 16)) & (allowedSquares & (allowedSquares << 8));

      // attacks
      attacks &= (hostiles | ::utils::indexToBitboard(this->state.enPassant));
      board |= attacks & ~(pieceBoard - 1);
    }

    // black
    else {
      // move one and two squares up if possible
      board = ((pieceBoard >> 8) | ((71776119061217280ULL & pieceBoard) >> 16)) & (allowedSquares & (allowedSquares >> 8));

      // attacks
      attacks &= (hostiles | (this->state.enPassant > 0 ? ::utils::indexToBitboard(this->state.enPassant) : 0)); // hostile pieces that can be attacked.
      board |= attacks & (pieceBoard - 1);
    }

    // get the promotions from the moves
    type::bitboard_t promotions = 18374686479671623935ULL & board;
    // remove pawn moves that hits a promotion square
    board ^= promotions;

    uint_fast8_t i = 0;
    while (promotions != 0) {
      i = ::utils::LSB(promotions);
      promotions = ::utils::flipBitOffCopy(promotions, i);

      this->generatePromotions(::utils::indexToBitboard(i), pieceBoard);
    }

    return board;
  }

  /**
   * Given a board with one set bit. Calculate if that position is in danger from an enemy attacks.
   *
   * @param board type::bitboard_t
   * @return bool
   * @slow
   */
   // TODO.. but this doesnt use the new move
  inline bool dangerousPosition(const type::bitboard_t board, const type::gameState_t& gs, const uint8_t hostile = 1) const {
    const uint8_t pos = ::utils::LSB(board);

    // knights
    if ((::utils::constant::knightAttackPaths[pos] & gs.piecesArr[::david::constant::index::knight][hostile]) > 0) {
      return true;
    }

    // pawns.
    type::bitboard_t pawnAttacks = ::utils::constant::pawnAttackPaths[pos];
    pawnAttacks &= gs.isWhite ? ~(board - 1) : board - 1;
    if ((pawnAttacks & gs.piecesArr[::david::constant::index::pawn][hostile]) > 0) {
      return true;
    }

    // bishop & diagonal queens
    const type::bitboard_t diagonals = this->generateDiagonals(pos, gs, hostile == 0);
    if (((diagonals & gs.piecesArr[::david::constant::index::bishop][hostile]) | (diagonals & gs.piecesArr[::david::constant::index::queen][hostile])) > 0) {
      // bug: generates a attack path that goes through a hostile piece.
      // in this case, king is at E8, a friendly pawn at G6 and a hostile queen at H5.
      // yet the path stretches beyond the pawn and into the queen, making this seem like
      // the queen has a open route to attack the king. This is obviusly false.
      // attack path generated: 1128103225065472

      return true;
    }

    // rook & vertical+horizontal queens
    const type::bitboard_t rookPaths = this->generateRookAttack(pos, gs, hostile == 0);
    if (((rookPaths & gs.piecesArr[::david::constant::index::rook][hostile]) | (rookPaths & gs.piecesArr[::david::constant::index::queen][hostile])) > 0) {
      return true;
    }

    // king
    if ((this->generateKingAttack(pos) & gs.piecesArr[::david::constant::index::king][hostile]) > 0) {
      return true;
    }

    // position seems to be safe
    return false;
  }

  // TODO: if this->state is in check, then only some pieces can move.
  // TODO, does this generate paths that the king can actually be attacked from? NOPE
  inline type::bitboard_t generateRookXRayPaths (const type::bitboard_t pBoard, const uint8_t friendly = 0, const uint8_t hostile = 1) const {
    // pass through friendlies, but only the first friendly.
    const auto friendlies = this->state.piecess[friendly];
    const uint8_t index = ::utils::LSB(pBoard);
    const uint8_t iRow = index / 8;
    const uint8_t iCol = index % 8;
    const type::bitboard_t hostiles = (this->state.piecesArr[::david::constant::index::queen][hostile] | this->state.piecesArr[::david::constant::index::rook][hostile]);

    // the result board
    type::bitboard_t result = 0ULL;

    // holds the tmp path to check if it's useful.
    type::bitboard_t tmp = 0ULL;

    // the number of blocker hits
    uint8_t hitTracker = 0;

    uint8_t mult = 1;


    // #####
    // Vertical attacks
    //

    // upwards, TODO: slow..
    for (uint8_t i = iRow + 1; i < 8; i++, mult++) {
      type::bitboard_t board = pBoard << (8 * mult);

      // on hit increment tracker
      if ((friendlies & board) > 0) {
        hitTracker += 1;
      }

      if (hitTracker == 2) {
        break;
      }

      // add attack path
      tmp |= board;
    }
    hitTracker = 0;
    mult = 1;

    // downwards, TODO: slow..
    for (uint8_t i = iRow - 1; i > 0; i--, mult++) {
      type::bitboard_t board = pBoard >> (8 * mult);

      // on hit increment tracker
      if ((friendlies & board) > 0) {
        hitTracker += 1;
      }

      if (hitTracker == 2) {
        break;
      }

      // add attack path
      tmp |= board;
    }
    hitTracker = 0;
    mult = 1;
    if ((hostiles & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }

    // #####
    // Horizontal attacks
    //

    // left, TODO: slow..
    for (uint8_t i = iCol + 1; i < 8; i++, mult++) {
      type::bitboard_t board = pBoard << (1 * mult); // left

      // is it on the same row?
      if (::utils::LSB(board) != iRow) {
        break;
      }

      // on hit increment tracker
      if ((friendlies & board) > 0) {
        hitTracker += 1;
      }

      if (hitTracker == 2) {
        break;
      }

      // add attack path
      tmp |= board;
    }
    hitTracker = 0;
    mult = 1;
    if ((hostiles & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }

    // right, TODO: slow..
    for (uint8_t i = iCol - 1; i > 0; i--, mult++) {
      type::bitboard_t board = pBoard >> (1 * mult);

      // is it on the same row?
      if (::utils::LSB(board) != iRow) {
        break;
      }

      // on hit increment tracker
      if ((friendlies & board) > 0) {
        hitTracker += 1;
      }

      if (hitTracker == 2) {
        break;
      }

      // add attack path
      tmp |= board;
    }
    if ((hostiles & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }

    return result;
  } // generateRookXRayPaths

  inline type::bitboard_t generateDiagonalXRayPaths (const type::bitboard_t pBoard, const uint8_t friendly = 0, const uint8_t hostile = 1) const {
    // pass through friendlies, but only the first friendly.
    const auto friendlies = this->state.piecess[friendly];
    const uint8_t index = ::utils::LSB(pBoard);
    const uint8_t iRow = index / 8;
    const uint8_t iCol = index % 8;
    const type::bitboard_t hostiles = (this->state.piecesArr[::david::constant::index::queen][hostile] | this->state.piecesArr[::david::constant::index::bishop][hostile]);

    // the result board
    type::bitboard_t result = 0ULL;

    // holds the tmp path to check if it's useful.
    type::bitboard_t tmp = 0ULL;

    // the number of blocker hits
    uint8_t hitTracker = 0;

    uint8_t mult = 1;

    // ######
    // Diagonal attacks, up right
    //

    // up and right
    for (uint8_t i = 0; i < 8 && (tmp & 9259542123273814271ULL) == 0; i++, mult++) {
      type::bitboard_t board = pBoard << (7 * mult);

      // on hit increment tracker
      if ((friendlies & board) > 0) {
        hitTracker += 1;
      }

      if (hitTracker == 2) {
        break;
      }

      // add attack path
      tmp |= board;
    }
    hitTracker = 0;
    mult = 1;
    if ((hostiles & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }

    // up and left
    for (uint8_t i = 0; i < 8 && (tmp & 72340172838076927ULL) == 0; i++, mult++) {
      type::bitboard_t board = pBoard << (9 * mult);

      // on hit increment tracker
      if ((friendlies & board) > 0) {
        hitTracker += 1;
      }

      if (hitTracker == 2) {
        break;
      }

      // add attack path
      tmp |= board;
    }
    hitTracker = 0;
    mult = 1;
    if ((hostiles & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }


    // down and right
    for (uint8_t i = 0; i < 8 && (tmp & 18410856566090662016ULL) == 0; i++, mult++) {
      type::bitboard_t board = pBoard >> (9 * mult);

      // on hit increment tracker
      if ((friendlies & board) > 0) {
        hitTracker += 1;
      }

      if (hitTracker == 2) {
        break;
      }

      // add attack path
      tmp |= board;
    }
    hitTracker = 0;
    mult = 1;
    if ((hostiles & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }


    // down and left
    for (uint8_t i = 0; i < 8 && (tmp & 18374969058471772417ULL) == 0; i++, mult++) {
      type::bitboard_t board = pBoard >> (7 * mult);

      // on hit increment tracker
      if ((friendlies & board) > 0) {
        hitTracker += 1;
      }

      if (hitTracker == 2) {
        break;
      }

      // add attack path
      tmp |= board;
    }
    if ((hostiles & tmp) > 0) {
      result |= tmp;
      tmp = 0ULL;
    }

    return result;
  } // generateDiagonalXRayPaths

  /**
   * Use for only one piece at the time!
   * @param piece index
   * @param piece index
   * @return
   */
  inline type::bitboard_t generateDiagonals (const uint8_t index, const type::gameState_t& gs, const bool hostilePath = false) const {
    const auto friendly   = gs.piecess[hostilePath ? 1 : 0];
    const auto hostiles   = gs.piecess[hostilePath ? 0 : 1];
    const auto iBoard     = ::utils::indexToBitboard(index);
    const type::bitboard_t southArea = iBoard - 1;
    const type::bitboard_t northArea = (~iBoard) ^ southArea;

    // attack path starting from bottom going upwards, reading left to right
    type::bitboard_t r1 = ::utils::constant::diagonalDUAttackPaths[index];

    r1 = this->extractLegalSouthPath(r1, friendly, hostiles, southArea & r1, 7);
    r1 = this->extractLegalNorthPath(r1, friendly, hostiles, northArea & r1, 7);

    // attack path starting from bottom going upwards, reading left to right
    type::bitboard_t r2 = ::utils::constant::diagonalUDAttackPaths[index];

    r2 = this->extractLegalSouthPath(r2, friendly, hostiles, southArea & r2, 9);
    r2 = this->extractLegalNorthPath(r2, friendly, hostiles, northArea & r2, 9);

    // combine
    return r1 | r2; // remove start position of piece
  }

  /**
   * Attack position for knights
   */
  inline uint64_t generateKnightAttack (const uint8_t index, const uint8_t friendly = 0) const {
    const type::bitboard_t friendlies = this->state.piecess[friendly];

    return (~friendlies) & ::utils::constant::knightAttackPaths[index];
  }

  /**
   * Generate King attack
   * @param index
   * @param hostilePath
   * @return
   */
  inline uint64_t generateKingAttack (const uint8_t index, const bool hostilePath = false) const {
    const type::bitboard_t friendly = this->state.piecess[hostilePath ? 1 : 0];

    // <---
    uint8_t maskRowOffset = 3; // offset from row 0
    const std::array<type::bitboard_t, 8> masks = {
        12918652928,
        30149115904,
        60298231808,
        120596463616,
        241192927232,
        482385854464,
        964771708928,
        825720045568
    };

    uint8_t row = index / 8;
    type::bitboard_t mask = masks[index % 8];
    type::bitboard_t rMask = 0ULL;

    if (row > maskRowOffset) {
      rMask = mask << (8 * (row - maskRowOffset));
    }
    else {
      rMask = mask >> (8 * (maskRowOffset - row));
    }

    // legal moves, not checked if in check mate or smth
    return (rMask & friendly) ^ rMask;
  }

  inline uint64_t generateRookAttack (const uint8_t index, const type::gameState_t& gs, const bool hostilePath = false) const {
    const auto friendly   = gs.piecess[hostilePath ? 1 : 0];
    const auto hostiles   = gs.piecess[hostilePath ? 0 : 1];
    const auto iBoard     = ::utils::indexToBitboard(index);
    const type::bitboard_t southArea = iBoard - 1;
    const type::bitboard_t northArea = (~iBoard) ^ southArea;

    // attack path starting from bottom going upwards, reading left to right
    type::bitboard_t r1 = ::utils::constant::verticalAttackPaths[index];

    r1 = this->extractLegalSouthPath(r1, friendly, hostiles, southArea & r1, 8);
    r1 = this->extractLegalNorthPath(r1, friendly, hostiles, northArea & r1, 8);

    // attack path starting from bottom going upwards, reading left to right
    type::bitboard_t r2 = ::utils::constant::horizontalAttackPaths[index];

    r2 = this->extractLegalSouthPath(r2, friendly, hostiles, southArea & r2, 1);
    r2 = this->extractLegalNorthPath(r2, friendly, hostiles, northArea & r2, 1);

    // combine
    return r1 | r2; // remove start position of piece
  }


};


}
