#ifndef GAME_H
#define GAME_H

#include <vector>
#include <map>
#include "Enumerations.h"
#include "Piece.h"
#include "Terminal.h"


// Game status code enumeration. Note that any value > 0
// indicates success, and any value < 0 indicates failure.
// You might want to use these as return codes from methods
// such as make_move().
enum status {
  LOAD_FAILURE = -10,
  SAVE_FAILURE,
  PARSE_ERROR,
  MOVE_ERROR_OUT_OF_BOUNDS,
  MOVE_ERROR_NO_PIECE,
  MOVE_ERROR_BLOCKED,
  MOVE_ERROR_CANT_CASTLE,
  MOVE_ERROR_MUST_HANDLE_CHECK,
  MOVE_ERROR_CANT_EXPOSE_CHECK,
  MOVE_ERROR_ILLEGAL,
  SUCCESS = 1,
  MOVE_CHECK,
  MOVE_CAPTURE,
  MOVE_CHECKMATE,
  MOVE_STALEMATE,
  GAME_WIN,
  GAME_OVER
};




// A base class representing a game that takes place on a chess board
class Game {

    // The type of a piece factory map. Maps from int describing a
    // Piece to the factory class producing the Piece.
    typedef std::map<int, AbstractPieceFactory*> PieceGenMap;

public:
    // Construct a board with the specified dimensions
    Game(unsigned int w = 8, unsigned int h = 8, int t = 1) :
        _width(w), _height(h), _pieces(w * h, nullptr), _turn(t) {}

    // Virtual destructor is necessary for a class with virtual methods
    virtual ~Game();

    // Return the width of the board
    unsigned int width() const { return _width; }

    // Return the height of the board
    unsigned int height() const { return _height; }

    // Create a piece on the board using the factory.
    // Returns true if the piece was successfully placed on the board
    bool init_piece(int piece_type, Player owner, Position pos);

    // Return a pointer to the piece at the specified position,
    // if the position is valid and occupied, nullptr otherwise.
    Piece* get_piece(Position pos) const;

    // Return the player whose turn it is
    Player player_turn() const { 
        return static_cast<Player>(!(_turn % 2)); 
    }

    // Return the current turn number (turn sequence number)
    int turn() const {
        return _turn;
    }

    // Return true if the position is within bounds
    bool valid_position(Position pos) const {
        return pos.x < _width && pos.y < _height;
    }

    // Pure virtual function (i.e. not defined in Game)
    // so always need to override this in a subclass that
    // you want to instantiate.
    // Perform a move from the start Position to the end Position
    // The method returns an integer status where a value
    // >= 0 indicates SUCCESS, and a < 0 indicates failure
    virtual int make_move(Position start, Position end) = 0;

    //move the pawn piece, it gets its own special method!
    //called by make_move
    //virtual void move_pawn();
    
    //draw gamebiard
    void draw_board();

    //save current game state to a file
    void save_game();
    
    // Execute the main gameplay loop.
    virtual void run() = 0;

    // Pure virtual function (i.e. not defined in Game)
    // so always need to override this in subclasses
    // Reports whether the game is over.
    virtual bool game_over() const = 0;

protected:

    // Board dimensions
    unsigned int _width , _height;

    // Vector containing all the Pieces currently on the board
    std::vector<Piece*> _pieces;

    // Current game turn sequence number
    int _turn;

    // All the factories registered with this Board
    PieceGenMap _registered_factories;

    // Determine the 1D location index corresponding to a 2D position
    unsigned int index(Position pos) const {
        return pos.y * _width + pos.x;
    }

    // Functionality for creating a new piece (called by init_piece)
    Piece* new_piece(int piece_type, Player owner);

    // Functionality for adding piece factories (called by constructor)
    bool add_factory(AbstractPieceFactory* f);

};


#endif // GAME_H
