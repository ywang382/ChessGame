#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
#include <exception>
#include <cctype>
#include "Game.h"
#include "ChessGame.h"
#include "Prompts.h"

using std::ofstream;
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
    _board_on = false; // Board is off by default
}


// Set up the chess board with game state loaded from file, and check if the file is the right game type
ChessGame::ChessGame(const std::string filename, int type) : Game() {
  //filestream to read in from file
  ifstream file(filename);
  //exits if invalid file
  if(!file.is_open()) 
    throw std::runtime_error("Load Failure");

  _board_on = false; //board is set to off by default
  
  string game; //used to store game choice
  file >> game;
  if(type == 1 && game != "chess"){//exits if wrong game choice, ChessGame has type = 1
    file.close();
    throw std::logic_error("Wrong Game");
  }
  initialize_factories();

  if(type == 2 || type == 3) // Don't need to anything more for the other 2 game types
    return;
  file >> _turn;
  load_pieces(file);
  file.close();
  return;
}

//Save current state of game to a file
void ChessGame::save_game(){
  Prompts::save_game();//Ask user for filename to save
  string name; //for storing saving filename
  cin >> name;
  //filestream for writing to file
  ofstream file(name);
  if(!file.is_open()){ //print error message if cannot open file
    Prompts::save_failure();
    return;
  }
  file << "chess" << endl; //prints game type
  file << _turn << endl; //prints turn number
  save_piece_state(file); //calss function that saves the pieces in the vector to file
  file.close();
  Prompts::save_success();
}

// Executes main game loop for all chess game. Calls appropriate update_board()
// for specific game type
void ChessGame::run(){
  std::string input;
  //buffer for previous input
  std::getline(cin, input);
  system("clear");
  if(check(opponent()))Prompts::check(opponent());
  
  //main user interface
  while(true){
    Prompts::player_prompt(player_turn(), _turn); //prompts for user input
    std::getline(cin, input); //get user input
    lowerCase(input); //case insensitive input, so converts all inputs to lower case
    //Check for non-move command
    if(input == "board"){
      _board_on = !_board_on; //toggle board on-off
      system("clear");
    }
    else if(input == "save"){
      save_game(); 
      std::getline(cin, input); //buffer for previous input
      continue;
    }
    else if(input == "q") //quits game
      break;
    else if(input == "forfeit"){ //forfeit, propmts win and game_over then exits
      Prompts::win(opponent(), _turn);
      Prompts::game_over();
      break;
    }
    else {
      int status = update_board(input); //attempt to make move
      //prints correct msg
      if(status == GAME_OVER){
	Prompts::game_over();
	draw_board();
	break;
      }
      if(status == MOVE_CAPTURE)
	Prompts::capture(opponent());
      if(status == PARSE_ERROR)
	Prompts::parse_error();
    }
    draw_board();
  }
}

// update board and make move for Chess and King of Hill Chess
// SpookyChess will override this function
// reads user input and perform make move options
// return value > 0 if move is successful, value <0 otherwise
int ChessGame::update_board(string input){
  return try_move(input);
}

// tests if use input for move is valid
// makes move if valid
// returns error type otherwise
int ChessGame::try_move(string input){
  //clears screen
  system("clear");

  //check if input length is valid
  if(input.length() != 5)
    return PARSE_ERROR;
  
  //parse for board positions
  char x_i = input.at(0);
  char y_i = input.at(1);
  char x_f = input.at(3);
  char y_f = input.at(4);
  //check if make_move input is valid
  if(!isalpha(x_i) || !isdigit(y_i) || !isspace(input[2]) || !isalpha(x_f) || !isdigit(y_f))
    return PARSE_ERROR;
  
  //calls make_move and print out appropriate error messages
  int status = make_move(Position(x_i-'a',y_i-'1'), Position(x_f-'a', y_f-'1'));
  switch(status){
  case MOVE_ERROR_OUT_OF_BOUNDS: Prompts::out_of_bounds();
    break;
  case MOVE_ERROR_NO_PIECE: Prompts::no_piece();
    break;
  case MOVE_ERROR_BLOCKED: Prompts::blocked();
    break;
  case MOVE_ERROR_ILLEGAL: Prompts::illegal_move();
    break;
  case MOVE_ERROR_CANT_EXPOSE_CHECK: Prompts::cannot_expose_check();
    break;
  case MOVE_ERROR_MUST_HANDLE_CHECK: Prompts::must_handle_check();
    break;
  }

  // Increments turn number
  if(status > 0)
    _turn++;
  else
    return status; //exit if move is illegal
  
  if(game_over()) // If game is over
    return GAME_OVER;
  
  if(check(opponent())){ // Report a check if there is one
    Prompts::check(opponent());
    return MOVE_CHECK;
  }
  if(status == MOVE_CAPTURE){ // Check for capture, msg is printed out later
    return MOVE_CAPTURE;
  }
  return status; // If no message needs to be printed
}


// Check if move is valid but doesn't make the actual move, return status of move
// return value > 0 if successful, value < 0 otherwise
int ChessGame::valid_move(Position start, Position end){
  //check for move to same cell
  if(index(start) == index(end))
    return MOVE_ERROR_ILLEGAL;
  
  //check for out of bound error
  if(!valid_position(start) || !valid_position(end))
    return MOVE_ERROR_OUT_OF_BOUNDS;
  
  Piece * p = get_piece(start);
  //check for no piece error
  if(p == nullptr || p->owner() != player_turn()) 
    return MOVE_ERROR_NO_PIECE;
  
  vector<Position> trajectory; //vector to store positions of trajectory

  //Check for valid move shape, failed to move otherwise
  if(p->valid_move_shape(start, end, trajectory) >= 0){
    //check for illegal pawn move
    if(p->piece_type() == PAWN_ENUM && trajectory.size() > 0 && _pieces[index(end)] != nullptr)
      return MOVE_ERROR_ILLEGAL;

    //check for obstructing pieces
    for(unsigned int i = 1; i < trajectory.size(); i++){
      if(_pieces[index(trajectory[i])] != nullptr)
	return MOVE_ERROR_BLOCKED;
    }

    //check for regular move
    if(_pieces[index(end)] == nullptr && trajectory.size()> 0)
      return SUCCESS;

    //check for a piece at final position
    if(_pieces[index(end)] != nullptr){
      if(_pieces[index(end)]->owner() == opponent()) //only can capture opponent's piece
	return MOVE_CAPTURE;
      else 
	return MOVE_ERROR_BLOCKED; //this will check for an attempt to capture the ghost piece in SpookyChess, too
    }
  }
  return MOVE_ERROR_ILLEGAL;
}

// Detect a check by a passed in player
// Return true if the player is checking its opponent
bool ChessGame::check(Player cur_player){
  // turn has already been incremented after a move is made.
  // Since we are calling valid_move, which needs to know the identity of the previous player
  // We decrement _turn temporarily in this function for valid_move to work properly
  _turn--;
  Position king; //get king position of other player
  for(unsigned int i = 0; i < _pieces.size(); i++){
    if(_pieces[i] != nullptr && _pieces[i]->owner() != cur_player && _pieces[i]->piece_type() == KING_ENUM ){
      king = pos(i); //convert from index to position
    }
  }
  for(unsigned int i = 0; i < _pieces.size(); i++){ //loop through all pieces to see if they can capture king
    if(_pieces[i] == nullptr || _pieces[i]->owner() != cur_player)
      continue;
    if(valid_move(pos(i), king) > 0){ //a valid move to king is possible
      _turn++; //restore original turn
      return true; //is checking
    }
  }
  _turn++; //restore original turn
  return false;
}


// Perform a move from the start Position to the end Position                   
// The method returns an integer with the status                                
// > 0 is SUCCESS, < 0 is failure    
int ChessGame::make_move(Position start, Position end) {
  bool in_check = check(opponent()); //true if currently in check, used to print out correct check handling msg
  
  int status = valid_move(start, end); //move status of attempted move
  if(status < 0) //if move status is invalid, exits
    return status;

  Piece* p = _pieces[index(start)]; //get piece at start pos
  if(status == SUCCESS){ //move piece with no capture
    _pieces[index(start)] = nullptr; //remove piece from starting pos
    _pieces[index(end)] = p;
    if(check(opponent())){ //revert the move if the move results in check
      _pieces[index(start)] = p;
      _pieces[index(end)] = nullptr;
      if(in_check) //prints out specific msg for disallowed move
	return MOVE_ERROR_MUST_HANDLE_CHECK; //if previously in check
      return MOVE_ERROR_CANT_EXPOSE_CHECK; //if previously not in check
    }
  }
  else if(status == MOVE_CAPTURE){ //capturing
    _pieces[index(start)] = nullptr; //remove piece from starting pos
    Piece* captured = _pieces[index(end)]; //set a temp pointer to the captured piece
    _pieces[index(end)] = nullptr;
    _pieces[index(end)] = p;
    if(check(opponent())){ //revert the move if it results in check
      _pieces[index(start)] = p;
      _pieces[index(end)] = captured;
      if(in_check) 
	return MOVE_ERROR_MUST_HANDLE_CHECK;
      return MOVE_ERROR_CANT_EXPOSE_CHECK;
    }
    delete captured; //clean up pointer to captured piece
  }
  
  // pawn to queen on other side
  if(p->piece_type() == PAWN_ENUM){
    if((p->owner() == WHITE && end.y == 7)||(p->owner() == BLACK && end.y == 0)){
      _pieces[index(end)] = nullptr;
      init_piece(QUEEN_ENUM, p->owner(), end);
      delete p; //clean up pointer to the orgirnal pawn
    }
  }
  return status;   
}

// Report whether a mate occurs, either checkmate or stalemate
// This would essentially result in game_over
// Return 0 if no mate is detected
int ChessGame::mate(){
  bool in_check = check(opponent());
  bool flag = 1; //whether no move can be made, set to 0 if a move is possible
  
  //for each piece of the current player
  for(unsigned int i = 0; i < _pieces.size(); i++){
    if(_pieces[i] == nullptr || _pieces[i]->owner() != player_turn()) //loop through all pieces of current player
      continue;
    //check for all moves to each position on board
    for(unsigned int j = 0; j < _pieces.size(); j++){
      if(j==i) continue; //ignore move to same pos
      int status = valid_move(pos(i), pos(j));
      if (status == SUCCESS){ 
	_pieces[j] = _pieces[i];
	_pieces[i] = nullptr;
	if(!check(opponent())) //the move can cancel check
	  flag = 0;
	_pieces[i] = _pieces[j]; //revert back, we don't actually want to make the move
	_pieces[j] = nullptr;
      } 
      if (status == MOVE_CAPTURE){
	Piece* tmp = _pieces[j];
	_pieces[j] = nullptr;
	_pieces[j] = _pieces[i];
	_pieces[i] = nullptr; 
	if(!check(opponent()))
	  flag = 0; 
	_pieces[i] = _pieces[j]; //revert back
	_pieces[j] = tmp;
      }
    }
  }
  if(in_check && flag)
    return CHECKMATE;
  if(!in_check && flag)
    return STALEMATE;
  //return 0 if no mate detected
  return 0;
}

// Returns true if game is over, print out message about how game ended (check/stale mate)
// This is different for HillChess due to an added condition
bool ChessGame::game_over(){
  if(mate() == CHECKMATE){
    Prompts::checkmate(opponent()); //Prompts corect msg
    Prompts::win(opponent(), turn()-1);
    return true;
  }else if(mate() == STALEMATE){
    Prompts::stalemate();
    return true;
  }
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




