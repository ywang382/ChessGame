#include <iostream>
#include <cassert>
#include <cctype>
#include <string>
#include <fstream>

#include "Game.h"
#include "Prompts.h"
#include "Piece.h"

using std::ofstream;
using std::string;
using std::cin;
using std::cout;
using std::endl;

Game::~Game() {

    // Delete the factories used to generate pieces
    for (size_t i = 0; i < _registered_factories.size(); i++) {
      delete _registered_factories[i];
    }
    //Delete the individual piece objects
    for(unsigned int i = 0; i < _pieces.size(); i++){
      if(_pieces[i] != nullptr)
	delete _pieces[i];
    }
    
    // Delete any other dynamically-allocated resources here
    


}

// Create a Piece on the board using the appropriate factory.
// Returns true if the piece was successfully placed on the board.
bool Game::init_piece(int piece_type, Player owner, Position pos) {
    Piece* piece = new_piece(piece_type, owner);
    if (!piece) return false;

    // Fail if the position is out of bounds
    if (!valid_position(pos)) {
        Prompts::out_of_bounds();
        return false;
    }

    // Fail if the position is occupied
    if (get_piece(pos)) {
        Prompts::blocked();
        return false;
    }
    _pieces[index(pos)] = piece;
    return true;
}

// Get the Piece at a specified Position.  Returns nullptr if no
// Piece at that Position or if Position is out of bounds.
Piece* Game::get_piece(Position pos) const {
    if (valid_position(pos))
        return _pieces[index(pos)];
    else {
        Prompts::out_of_bounds();
        return nullptr;
    }
}

// Print the appropriate character for each different piece on the screen
// Called in draw_board
void print_piece(Piece* piece){
  // get piece info
  if(piece == nullptr){
    cout << "    ";
    return;
  }  
  if(piece->owner() == WHITE){
    Terminal::color_fg(1, Terminal::BLACK);
    switch (piece->piece_type()){
    case PAWN_ENUM:
      cout << " \u2659  ";
      break;
    case KNIGHT_ENUM:
      cout << " \u2658  ";
      break;
    case BISHOP_ENUM:
      cout << " \u2657  ";
      break;
    case ROOK_ENUM:
      cout << " \u2656  ";
      break;
    case QUEEN_ENUM:
      cout << " \u2655  ";
      break;
    case KING_ENUM:
      cout << " \u2654  ";
      break;
    }
    return;
  }
  if(piece->owner() == BLACK){
    Terminal::color_fg(1, Terminal::YELLOW);
    switch (piece->piece_type()){
    case PAWN_ENUM:
      cout << " \u265F  ";
      break;
    case KNIGHT_ENUM:
      cout << " \u265E  ";
      break;
    case BISHOP_ENUM:
      cout << " \u265D  ";
      break;
    case ROOK_ENUM:
      cout << " \u265C  ";
      break;
    case QUEEN_ENUM:
      cout << " \u265B  ";
      break;
    case KING_ENUM:
      cout << " \u265A  ";
      break;
    }
  }
}


// Draw gameboard
void Game::draw_board(){
  cout << endl << "==================================" << endl;
  for(unsigned int i = _height; i > 0 ; i--){
    cout << i << " ";
    for(unsigned int j = 0; j < _width; j++){
      if((i+j)%2 == 0){
	Terminal::color_bg(Terminal::BLACK);
      }else{
	Terminal::color_bg(Terminal::CYAN);
      }
      print_piece(_pieces[index(Position(j, i-1))]);
      Terminal::set_default();
    }
    cout << endl;
  }
  Terminal::set_default();
  cout << "   ";
  for(unsigned int i = 0; i < _width; i++){
    char a = 'a';
    char b = a+i;
    cout <<  b << "   ";
  }
  cout << endl << "==================================" << endl;
}


// Perform a move from the start Position to the end Position
// The method returns an integer status where a value
// >= 0 indicates SUCCESS, and a < 0 indicates failure

//Save current state of game to a file
void Game::save_game(){
  //Ask user for filename to save
  Prompts::save_game();
  string name; //for storing saving filename
  cin >> name;
  //filestream for writing to file
  ofstream file(name);
  if(!file.is_open()){ //print error message if cannot open file
    Prompts::save_failure();
    return;
  }
  file << "chess" << endl; //this is gonna be changed in the future
  file << _turn << endl;
  for(unsigned int i = 0; i < _pieces.size(); i++){
    if(_pieces[i] != nullptr){
      file << _pieces[i]->owner() << " ";
      int x = i%_width; //x position of current piece
      char a = x + 'a'; //convert to char value for x pos
      int y = (i-x)/_width + 1; //y position of cuurent piece
      file << a << y << " " << _pieces[i]->piece_type() << endl;
    }
  }
  file.close();
}


// Search the factories to find a factory that can translate
//`piece_type' into a Piece, and use it to create the Piece.
// Returns nullptr if factory not found.
Piece* Game::new_piece(int piece_type, Player owner) {

    PieceGenMap::iterator it = _registered_factories.find(piece_type);

    if (it == _registered_factories.end()) { // not found
        std::cout << "Piece type " << piece_type << " has no generator\n";
        return nullptr;
    } else {
        return it->second->new_piece(owner);
    }
}



// Add a factory to the Board to enable producing
// a certain type of piece. Returns whether factory
// was successfully added or not.
bool Game::add_factory(AbstractPieceFactory* piece_gen) {
    // Temporary piece to get the ID
    Piece* p = piece_gen->new_piece(WHITE);
    int piece_type = p->piece_type();
    delete p;

    PieceGenMap::iterator it = _registered_factories.find(piece_type);
    if (it == _registered_factories.end()) { // not found, so add it
        _registered_factories[piece_type] = piece_gen;
        return true;
    } else {
        std::cout << "Piece type " << piece_type << " already has a generator\n";
        return false;
    }
}

