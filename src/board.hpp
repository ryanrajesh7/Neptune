#ifndef BOARD_H
#define BOARD_H

#include <array>
#include <string>
#include <stack>

class Board {
    public:
    std::array<char,64> board;
    std::stack<int> moveStack;
    // [KQkq- 1 bit for each][enpassant possible-1 bit][3 bits for the column if enpassant possible]
    //K for white king side castle, Q for queenside, small letter for corresponding black
    unsigned char state;     
    char turn;

    Board();
    void resetBoard();                              //resets the board
    void fromFEN(std::stringstream& ss);                  //matches the board with the fen
    void makeMove(std::string move);                //makes the move.. takes in uci format (eg e2e4) for (e7e8q for promotion)
    void makeMove(int start,int dest,char promotion); //makes the move.. takes in start and destination square, 
                                                        //and promotion piece, '0' for no promotion
    bool isValid(int start, int dest); //checks if the suggested move (start,dest)is valid one or not
    bool isAttacked(int pos, char side);                      //checks if the  given square is attacked by opposite side
    char& operator[](int i);
    void print();
    void undo();
};

#endif