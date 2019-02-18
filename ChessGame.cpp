#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include "Game.h"
#include "ChessGame.h"
#include "Prompts.h"

using std::string;
using std::ifstream;
using std::getline;
using std::vector;
using std::cout;
using std::cin;
using std::endl;

// Set up the chess board with standard initial pieces
ChessGame::ChessGame(): Game() {
    initialize_factories();
    std::vector<int> pieces {
        ROOK_ENUM, KNIGHT_ENUM, BISHOP_ENUM, QUEEN_ENUM,
        KING_ENUM, BISHOP_ENUM, KNIGHT_ENUM, ROOK_ENUM
    };
    for (size_t i = 0; i < pieces.size(); ++i) {
        init_piece(PAWN_ENUM, WHITE, Position(i, 1));
        init_piece(pieces[i], WHITE, Position(i, 0));
        init_piece(pieces[i], BLACK, Position(i, 7));
        init_piece(PAWN_ENUM, BLACK, Position(i, 6));
    }
}


// Set up the chess board with game state loaded from file
ChessGame::ChessGame(const std::string filename) : Game() {
    initialize_factories();
    //filestream to read in from file
    ifstream file(filename);
    if(file.is_open()){
      string game; //used to store game choice
      file >> game;
      if(game != "chess") //check for correct game choice
	return;
      file >> _turn;
      int p; //used to store owner of piece (White or Black)
      while(file >> p){ //continue reading in line
	int y, piece; //y position on board, piece type
	char x; //x position on board
	file >> x;
	file >> y;
	file >> piece;
	//cast to Player enum
	Player player = static_cast<Player>(p);
	//create piece from info read from file
	init_piece(piece, player, Position((int)(x-'a'), y-1));
      }
      file.close();
      return;
    }

    //Print error message if failed to open file.
    Prompts::load_failure();
    // Add code here to collect piece information from file
    // and call init_piece as needed to create and place each one
    // (see the no-argument constructor for appropriate syntax)
}

void ChessGame::run(){
  draw_board();
  std::string input = "word";
  //temporary interface for testing
  while(true){
    Player cur_player = player_turn();
    Prompts::player_prompt(cur_player, _turn);
    cin >> input;
    if(input == "s"){
      save_game();
      Prompts::player_prompt(cur_player, _turn);
      cin >> input;
    }
    if(input == "q")
      break;
    char a = input.at(0);
    char b = input.at(1);
    char c = input.at(3);
    char d = input.at(4);
    make_move(Position(a-'a',b-'1'), Position(c-'a', d-'1'));
    draw_board();
  }
  /*
  make_move(Position(0,1), Position(0,6));
  draw_board();
  make_move(Position(7,6), Position(7,4));
  draw_board();
  make_move(Position(1,0), Position(2,2));
  draw_board();
  make_move(Position(1,7), Position(1, 5));
  draw_board();*/
}


// Perform a move from the start Position to the end Position
// The method returns an integer with the status
// >= 0 is SUCCESS, < 0 is failure
int ChessGame::make_move(Position start, Position end) {
  //get current player, WHITE or BLACK
  Player cur_player = player_turn();

  //check for out of bound error
  if(!valid_position(start) || !valid_position(end)){
    Prompts::out_of_bounds();
    return MOVE_ERROR_OUT_OF_BOUNDS;
  }

  Piece * p = get_piece(start);
  //check for no piece error
  if(p == nullptr || p->owner() != cur_player){
    Prompts::no_piece();
    return MOVE_ERROR_NO_PIECE;
  }

  vector<Position> trajectory; //vector to store positions of trajectory
  //Check for valid move shape, failed to move otherwise
  if(p->valid_move_shape(start, end, trajectory) >= 0){
    //check for obstructing pieces
    for(unsigned int i = 0; i < trajectory.size(); i++){
      if(_pieces[index(trajectory[i])] != nullptr){
	Prompts::blocked();
	return MOVE_ERROR_BLOCKED;
      }
    }
    bool flag = 0;
    //if no piece is at end pos
    if(_pieces[index(end)] == nullptr && trajectory.size()> 0){
      _pieces[index(start)] = nullptr; //remove piece from starting pos
      _pieces[index(end)] = p;
      _turn++;
      flag = 1;
    }
    //check for capture
    if(_pieces[index(end)] != nullptr && _pieces[index(end)]->owner() != cur_player){
      _pieces[index(start)] = nullptr; //remove piece from starting pos
      delete _pieces[index(end)]; //memory clean up for removed piece
      _pieces[index(end)] = nullptr;
      _pieces[index(end)] = p;
      Prompts::capture(cur_player);
      _turn++;
      flag = 1;
    }
    //pawn to queen when on the other side
    if(flag && p->piece_type() == PAWN_ENUM){
      if((p->owner() == WHITE && end.y == 7)||(p->owner() == BLACK && end.y == 0)){
	_pieces[index(end)] = nullptr;
	init_piece(QUEEN_ENUM, p->owner(), end);
	delete p; //delete the orgirnal pawn
      }
    }
    if(flag)
      return SUCCESS;
  }
  Prompts::illegal_move();
  return MOVE_ERROR_ILLEGAL;
}

/*
int ChessGame::move_pawn(Position start, Position end){
  vector<Position> trajectory;
  Piece *p = get_piece(start);
  if(p->valid_move_shape(start, end, trajectory) >= 0){
    //diagonal capture
    if(trajectory.size() == 0 && _pieces[index(end)]->owner()!= player_turn()){
      delete _pieces[index(end)];
      _pieces[index(end)] = nullptr;
      _pieces[index(end)] = p;
      Prompts::capture(cur_player);
      _turn++;
      return SUCCESS;
    }
    if(


}
*/

//check for check, lol
bool ChessGame::check(){
    for(unsigned int i = 0; i < _pieces.size(); i++){
        Piece* king;
        if(_pieces[i]->piece_type() == KING_ENUM)){
            king = _pieces[i];
        }
    }
}




// Report whether the chess game is over
bool ChessGame::game_over() const {

  return false;

}


// Prepare the game to create pieces to put on the board
void ChessGame::initialize_factories() {

    // Add all factories needed to create Piece subclasses
    add_factory(new PieceFactory<Pawn>(PAWN_ENUM));
    add_factory(new PieceFactory<Rook>(ROOK_ENUM));
    add_factory(new PieceFactory<Knight>(KNIGHT_ENUM));
    add_factory(new PieceFactory<Bishop>(BISHOP_ENUM));
    add_factory(new PieceFactory<Queen>(QUEEN_ENUM));
    add_factory(new PieceFactory<King>(KING_ENUM));
}




