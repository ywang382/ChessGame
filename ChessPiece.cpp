#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include "Game.h"
#include "ChessPiece.h"


/**
 * Helper functions to determine
 * type of move player is making
 */

//checks if user attempts to move horizontally
bool move_horizontal(Position start, Position end){
  return (start.y == end.y);
}
  
//checks if user attempts to move vertically
bool move_vertical(Position start, Position end){
  return (start.x == end.x);
}

//checks if user attempts to move diagonally
bool move_diagonal(Position start, Position end){
  return (abs((int)start.x - (int)end.x)== abs((int)start.y - (int) end.y));
}

/**
 * Helper functions to populate trajectory
 */

//populate horizontal trajectory
void horizontal_trajectory(Position start, Position end,std::vector<Position>& trajectory){
  //determine move right or left
  int dx = (start.x < end.x)? 1 : -1;

  for(unsigned int i = start.x; i != end.x; i += dx){
    trajectory.push_back(Position(i, start.y));
  }
}

//populate vertical trajectory
void vertical_trajectory(Position start, Position end, std::vector<Position>& trajectory){
  //determine move up or down
  int dy = (start.y < end.y)? 1 : -1;

  for(unsigned int i = start.y; i != end.y; i += dy){
    trajectory.push_back(Position(start.x, i));
  }
}

//populate diagonal trajectory
void diagonal_trajectory(Position start, Position end, std::vector<Position>& trajectory){
  //determine move direction
  int dx = (start.x < end.x)? 1 : -1;
  int dy = (start.y < end.y)? 1 : -1;

  unsigned int x = start.x;
  unsigned int y = start.y;
  while(x != end.x && y != end.y){
    trajectory.push_back(Position(x, y));
    x += dx;
    y += dy;
  }
}

/*
 * Implementation of valid_move_shape functions for specific chess pieces
 * Return the status of the move, successful or illegal move
 * These are called by the valid_move function in game
 */

int Rook::valid_move_shape(Position start, Position end, std::vector<Position>& trajectory) const{
  if(move_horizontal(start, end)){
    horizontal_trajectory(start, end, trajectory);
    return SUCCESS;
  }
  else if(move_vertical(start, end)){
    vertical_trajectory(start, end, trajectory);
    return SUCCESS;
  }
  else{
    return MOVE_ERROR_ILLEGAL;
  }
}

int Knight::valid_move_shape(Position start, Position end, std::vector<Position>& trajectory) const{
  int dx = abs((int)start.x - (int)end.x);
  int dy = abs((int)start.y - (int)end.y);
  if((dx == 2 && dy == 1) || (dx == 1 && dy == 2)){ //knight can only move in L shape
    trajectory.push_back(end);
    return SUCCESS;
  }
  return MOVE_ERROR_ILLEGAL;
}



int Bishop::valid_move_shape(Position start, Position end, std::vector<Position>& trajectory) const{
  if(move_diagonal(start, end)){
    diagonal_trajectory(start, end, trajectory);
    return SUCCESS;
  }
  else{
    return MOVE_ERROR_ILLEGAL;
  }
}

int Queen::valid_move_shape(Position start, Position end, std::vector<Position>& trajectory) const{
  if(move_horizontal(start, end)){
    horizontal_trajectory(start, end, trajectory);
    return SUCCESS;
  }
  else if(move_vertical(start, end)){
    vertical_trajectory(start, end, trajectory);
    return SUCCESS;
  }
  else if(move_diagonal(start, end)){
    diagonal_trajectory(start, end, trajectory);
    return SUCCESS;
  }
  else{
    return MOVE_ERROR_ILLEGAL;
  }
}

int King::valid_move_shape(Position start, Position end, std::vector<Position>& trajectory) const{
  if(abs((int)start.x - (int)end.x) > 1 || abs((int)start.y - (int)end.y) > 1){
    return MOVE_ERROR_ILLEGAL;
  }
  else if(move_horizontal(start, end)){
    horizontal_trajectory(start, end, trajectory);
    return SUCCESS;
  }
  else if(move_vertical(start, end)){
    vertical_trajectory(start, end, trajectory);
    return SUCCESS;
  }
  else if(move_diagonal(start, end)){
    diagonal_trajectory(start, end, trajectory);
    return SUCCESS;
  }
  else{
    return MOVE_ERROR_ILLEGAL;
  }
}

// Pawn's move is special because it can move 1 or 2 steps from starting position and it only captures diagonally
// This function returns true if the move shape is valid for pawn: it can be diagonal, 1 step forward,
// or 2 steps forward only if the pawn is at its starting position.
// To distinguish the actual move shape of the pawn in the calling function, the size of the trajectory will be different for each move shape.
int Pawn::valid_move_shape(Position start, Position end, std::vector<Position>& trajectory) const{
  int dx = (int)end.x - (int)start.x;
  int dy;
  //gives a positive dy value for a correct player's move
  if(_owner == WHITE)
    dy = (int)end.y - (int)start.y;
  else
    dy = (int)start.y - (int)end.y;
 
  //2 steps forward
  if(dy == 2 && dx == 0){
    //only allow move for pawn at starting position
    if((_owner == BLACK && start.y == 6) || (_owner == WHITE && start.y == 1)){
        vertical_trajectory(start, end, trajectory);
        trajectory.push_back(end);
        //trajectory will have size 2 for two steps
        return SUCCESS;
    }
  }
  
  //1 step forward
  if(dy == 1 && dx == 0){
    trajectory.push_back(end);
    //trajectory will have size 1 for one vertical step
    return SUCCESS;
  }
  
  //diagonal step
  if(dy == 1 && abs(dx) == 1){
    trajectory.clear();
    //trajectory will have size 0 for diagonal step
    return SUCCESS;
  }
  //the size of the trajectory indicates which movement the pawn takes
  
  return MOVE_ERROR_ILLEGAL; //if everything fails
}

int Ghost::valid_move_shape(Position start, Position end, std::vector<Position>& trajectory) const {
  //no need to track the ghost's movement
  trajectory.push_back(start);
  trajectory.push_back(end);
  return SUCCESS;
}

