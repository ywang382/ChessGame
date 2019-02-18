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

//checks if user attempts to move to the same cell
bool move_to_same_cell(Position start, Position end){
  if(start.x == end.x && start.y == end.y){
    return true;
  }
  return false;
}

//checks if user attempts to move horizontally
bool move_horizontal(Position start, Position end){
  if(start.y == end.y){
    return true;
  }
  return false;
}

//checks if user attempts to move vertically
bool move_vertical(Position start, Position end){
  if(start.x == end.x){
    return true;
  }
  return false;
}

//checks if user attempts to move diagonally
bool move_diagonal(Position start, Position end){
  if(abs(((int)start.x - (int)end.x)/((int)start.y - (int) end.y)) == 1){
    return true;
  }
  return false;
}

/**
 * Helper functions to populate trajectory
 */

//populate horizontal trajectory
void horizontal_trajectory(Position start, Position end,std::vector<Position>& trajectory){
  //determine move right or left
  int dx = (start.x < end.x)? 1 : -1;

  for(unsigned int i = start.x + dx; i != end.x; i += dx){
    trajectory.push_back(Position(i, start.y));
  }
}

//populate vertical trajectory
void vertical_trajectory(Position start, Position end, std::vector<Position>& trajectory){
  //determine move up or down
  int dy = (start.y < end.y)? 1 : -1;

  for(unsigned int i = start.y + dy; i != end.y; i += dy){
    trajectory.push_back(Position(start.x, i));
  }
}

//populate diagonal trajectory
void diagonal_trajectory(Position start, Position end, std::vector<Position>& trajectory){
  //determine move direction
  int dx = (start.x < end.x)? 1 : -1;
  int dy = (start.y < end.y)? 1 : -1;

  unsigned int x = start.x + dx;
  unsigned int y = start.y + dy;
  while(x != end.x && y != end.y){
    trajectory.push_back(Position(x, y));
    x += dx;
    y += dy;
  }
}

int Rook::valid_move_shape(Position start, Position end, std::vector<Position>& trajectory) const{
  if(move_to_same_cell(start, end)){
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
  else{
    return MOVE_ERROR_ILLEGAL;
  }
}

int Knight::valid_move_shape(Position start, Position end, std::vector<Position>& trajectory) const{
  //trajectory.push_back(start);
  trajectory.push_back(end);
  if(move_to_same_cell(start, end)){
    return MOVE_ERROR_ILLEGAL;
  }
  else{
    static const int offsets[][2] = {
      {1, -2},
      {2, -1},
      {2, 1},
      {1, 2},
      {-1, 2},
      {-2, -1},
      {-1, -2}
    };
    bool flag = false;
    for(int i = 0; i < 8; i++){
      unsigned int new_x = start.x + offsets[i][0];
      unsigned int new_y = start.y + offsets[i][1];
      if(end.x == new_x && end.y == new_y){flag = true;}
    }
    return (flag == true)? SUCCESS:MOVE_ERROR_ILLEGAL;
  }
}


int Bishop::valid_move_shape(Position start, Position end, std::vector<Position>& trajectory) const{
  if(move_to_same_cell(start,end)){
    return MOVE_ERROR_ILLEGAL;
  }
  else if(move_diagonal(start, end)){
    diagonal_trajectory(start, end, trajectory);
    return SUCCESS;
  }
  else{
    return MOVE_ERROR_ILLEGAL;
  }
}

int Queen::valid_move_shape(Position start, Position end, std::vector<Position>& trajectory) const{
  if(move_to_same_cell(start, end)){
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

int King::valid_move_shape(Position start, Position end, std::vector<Position>& trajectory) const{
  if(move_to_same_cell(start, end)){
    return MOVE_ERROR_ILLEGAL;
  }
  else if(abs((int)start.x - (int)end.x) > 1 || abs((int)start.y - (int)end.y) > 1){
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

int Pawn::valid_move_shape(Position start, Position end, std::vector<Position>& trajectory) const{
  int dx = (int)end.x - (int)start.x;
  int dy;
  //gives positive dy value for a correct player's move
  if(_owner == WHITE){
    dy = (int)end.y - (int)start.y;
  }else{
    dy = (int)start.y - (int)end.y;
  }
 
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
  return MOVE_ERROR_ILLEGAL;
}
