CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -g

play: Play.o Game.o ChessGame.o ChessPiece.o SpookyChess.o HillChess.o
	$(CXX) Play.o Game.o ChessGame.o SpookyChess.o ChessPiece.o HillChess.o -o play

Play.o: Play.cpp Game.h ChessGame.h Prompts.h
	$(CXX) $(CXXFLAGS) -c Play.cpp

Game.o: Game.cpp Game.h Piece.h Prompts.h Enumerations.h Terminal.h
	$(CXX) $(CXXFLAGS) -c Game.cpp

ChessPiece.o: ChessPiece.cpp Game.h ChessPiece.h
	$(CXX) $(CXXFLAGS) -c ChessPiece.cpp

ChessGame.o: ChessGame.cpp Game.h ChessGame.h Piece.h ChessPiece.h Prompts.h Enumerations.h
	$(CXX) $(CXXFLAGS) -c ChessGame.cpp

SpookyChess.o: SpookyChess.cpp Game.h SpookyChess.h Piece.h ChessPiece.h Prompts.h Enumerations.h
	$(CXX) $(CXXFLAGS) -c SpookyChess.cpp

HillChess.o: HillChess.cpp Game.h HillChess.h Piece.h ChessPiece.h Prompts.h Enumerations.h
	$(CXX) $(CXXFLAGS) -c HillChess.cpp

clean:
	rm *.o play

