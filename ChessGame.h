#ifndef CHESS_GAME_H
#define CHESS_GAME_H

#include <string>
#include "Game.h"
#include "ChessPiece.h"


class ChessGame : public Game {

public:

    // Creates new game in standard start-of-game state 
    ChessGame();

    // Creates game with state indicated in specified file and the game type
    ChessGame(std::string filename, int type);

    // Main gameplay loop
    void run() override;

    // Parse user input for make move
    int try_move(std::string input);

    virtual int update_board(std::string input);
    
    
    // perform main gameplay loop for ChessGame
    //virtual int update_board(std::string input) overrid
    virtual void save_game() override;

    // Check if move is valid, return status of move
    int valid_move(Position start, Position end);
    
    // Perform a move from the start Position to the end Position
    // The method returns an integer with the status
    // >= 0 is SUCCESS, < 0 is failure
    int make_move(Position start, Position end) override;

    // Detects if the passed in player is checking
    // Return true if the player is checking the opponent, false otherwise
    bool check(Player p);
    
    // Reports whether a mate (checkmate or stalemate) is detected
    // Meaning that the player cannot make any legal move
    int mate();
    
    // Reports whether the chess game is over
    virtual bool game_over() override;

protected:

    // Create all needed factories for the kinds of pieces
    // used in chess (doesn't make the actual pieces)
    virtual void initialize_factories();

};

#endif // CHESS_GAME_H
