#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <exception>
#include "Enumerations.h"
#include "ChessGame.h"
#include "HillChess.h"
#include "Prompts.h"

using std::cout;
using std::string;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::cin;
using std::endl;

//Exact same setup as ChessGame
HillChess::HillChess() : ChessGame(){
}

// Non-default constructor for King of Hill Chess
HillChess::HillChess(string filename, int type) : ChessGame(filename, type){
  ifstream file(filename);
  string game; //used to store game choice
  file >> game;
  if(type != 2 || game != "king"){//check for correct game choice, HillChess type = 2
    throw std::logic_error("Incorrect Game");
  }
  
  file >> _turn;
  load_pieces(file);
  file.close();
  return;
}

// Save current game state of King of Hill Chess
void HillChess::save_game(){
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
  file << "king" << endl; //save game type
  file << _turn << endl; // save turn

  //save the state of pieces
  save_piece_state(file);
  file.close();
  Prompts::save_success();
}

// Return whether the game is over, prints out msg about how the game is over:
// Checkmate, stalemate, or conquered
bool HillChess::game_over(){
  //Check for mate first
  if(mate() == CHECKMATE){
      Prompts::checkmate(opponent());
      Prompts::win(opponent(), turn()-1);
      return true;
  }else if(mate() == STALEMATE){
    Prompts::stalemate();
    return true;
  }
  
  // Check for conquer on middle squares
  for(int i = 3; i < 5; i++){
    for(int j = 3; j < 5; j++){ //loop through the 4 middle squares
      Piece* king = _pieces[index(Position(i, j))];
      if(king != nullptr && king->piece_type() == KING_ENUM){ //if there is a king, someone has won
	Prompts::conquered(king->owner()); //prompts conquered msg
	Prompts::win(king->owner(), turn()-1); 
	return true; 
      }
    }
  }
  return false;
}
