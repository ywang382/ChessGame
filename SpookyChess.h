#ifndef SPOOKYCHESS_H
#define SPOOKYCHESS_H

#include <string>
#include "ChessGame.h"

class SpookyChess : public ChessGame {

public:

    // Creates new game from scratch
    SpookyChess();

    // Creates game with state indicated in specified file
    SpookyChess(std::string filename, int type);

    // perform main gameplay loop for SpookyChess
    int update_board(std::string input) override;

    //move ghost piece to new random position
    int move_ghost_piece();

    //saves current state of game
    void save_game() override;

protected:
    int ghost_position; //1D index indicating location of the ghost on board
    int num_calls; //number of times rand() has been called

};

#endif // SPOOKYCHESS_H
