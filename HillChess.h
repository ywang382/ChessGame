#ifndef HILLCHESS_H
#define HILLCHESS_H

#include <string>
#include "ChessGame.h"

class HillChess : public ChessGame {
public:
    // Creates new game, same as constructor for ChessGame
    HillChess();

    // Creates new game from loaded file
    HillChess(std::string filename, int type);

    // Return true if game is over.
    // Prints out message indicating how the game ended:
    // Checkmate, stalemate, or conquered hill
    bool game_over() override;

    //saves current state of game
    void save_game() override;

};

#endif // HILLCHESS_H
