#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <exception>
#include <vector>
#include <string>
#include <time.h>
#include <random>
#include "Game.h"
#include "Enumerations.h"
#include "ChessGame.h"
#include "SpookyChess.h"
#include "Prompts.h"

using std::string;
using std::ifstream;
using std::ofstream;
using std::getline;
using std::vector;
using std::cout;
using std::cin;
using std::endl;

// Default constructor for SpookyChess class
// Set up chess board with standard inital pieces and ghost piece
SpookyChess::SpookyChess() : ChessGame(){
  
  //add Ghost piece factory
  add_factory(new PieceFactory<Ghost>(GHOST_ENUM));
  
  //initalize additional ghost piece at a5
  ghost_position = index(Position(0,4));
  init_piece(GHOST_ENUM, NO_ONE, Position(0,4));
  
  //seed random number generator
  srand(322);

  //initalize number of times number generator has been called
  num_calls = 0;
}

// Non-default constructor for SpookyChess class
// Creates game with state indicated in specified file
SpookyChess::SpookyChess(std::string filename, int type) : ChessGame(filename, type){
   //add Ghost piece factory
   add_factory(new PieceFactory<Ghost>(GHOST_ENUM));
   //seed random number generator
   srand(322);
   //filestream to read in from file
   ifstream file(filename);
   string game; //used to store game choice
   file >> game;
   if(type != 3 || game != "spooky") //check for correct game choice
     throw std::logic_error("Wrong Game");
   file >> _turn;
   file >> num_calls;
   // call rand function correct number of times to restore state
   for(int i = 0; i < num_calls; ++i){
     rand();
   }
   load_pieces(file);//load pieces
   for(int i = 0;  i < 64; ++i){//initalize ghost position
     if(_pieces[i] != nullptr && _pieces[i]->owner() == NO_ONE){
       ghost_position = i;
     }
   }    
}

// Update board and make move for spooky chess, calls try_move() of ChessGame
int SpookyChess::update_board(string input){
  int status = try_move(input);
  if(status > 0){ //only move ghost if player move is successful
    //moves ghost and only prompts capture if there is no previous prompt with higher priority
    if(move_ghost_piece()== GHOST_CAPTURE && status == 1) 
      Prompts::ghost_capture();
    
    // After the ghost has moved
    if(status != GAME_OVER && game_over()) // If game not already over, check for game over
      return GAME_OVER;

    if(status == 1 && check(opponent())){ // If no message printed, check for check
      Prompts::check(opponent());
      return MOVE_CHECK;
    }
  }
  return status;
}


// Moves the ghost piece and return whether the ghost has performed a capture
int SpookyChess::move_ghost_piece(){
  int status = SUCCESS; //used to tell if the ghost has captured a piece
  while(true){
    int end = std::rand()%64;
    num_calls++;

    //generate pointer to ghost piece
    Piece* g = _pieces[ghost_position];
    
    //check if king is at selected position
    //jumps back to the beginning of loop if true
    if(_pieces[end] != nullptr && _pieces[end]->piece_type()== KING_ENUM)continue;
    
    if(_pieces[end] == nullptr){//if no piece is at the end position
      _pieces[ghost_position] = nullptr; //remove ghost piece from starting pos
      _pieces[end] = g;
    }
    else{//if piece exists at end position
      _pieces[ghost_position] = nullptr; //remove ghost piece from previous position
      delete _pieces[end]; //memory clean up for removed piece
      _pieces[end] = g;
      status = GHOST_CAPTURE;
    }
    //update ghost position
    ghost_position = end;
    break;
  }
  return status;
  
}

void SpookyChess::save_game(){
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
  file << "spooky" << endl; //save game type
  file << _turn << endl; // save turn
  file << num_calls << endl; //save the number of times number generator was called
  save_piece_state(file);
  file.close();
  Prompts::save_success();
}
