#include <array>
#include <string>
#include <sstream>
#include <cctype>
#include <iostream>

#include "board.hpp"

Board::Board() : board(),state(0) {
    resetBoard();
}

void Board::fromFEN(std::stringstream& ss) {

    //starting fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

    std::string position;
    ss>>position;
    int r=7,c=0;
    for (int i=0; i<position.size(); i++) {

        if (position[i]=='/') r--, c=0;
        else if (std::isdigit(position[i])) {
            for (int j='0'; j<position[i]; j++) {
                board[r*8+c]='0';
                c++;
            }
        }
        else {
            board[r*8+c] = position[i];
            c++;
        }
    }

    ss>>turn;

    std::string castling;
    ss>>castling;
    
    if (castling != "-") {
        for (int i=0; i<castling.size(); i++) {

            if (castling[i]=='K') {
                state|= (1<<7);
            } 
            else if (castling[i]=='Q') {
                state|= (1<<6);
            }
            else if (castling[i]=='k') {
                state|=(1<<5);
            }
            else {
                state|=(1<<4);
            }
        }
    }

    std::string enpassant;
    ss>>enpassant;
    if (enpassant != "-") {
        state|=(1<<3);
        int col= enpassant[0]-'a';
        state &= (0b11111000);
        state |= (col & 0b00000111);
    }

    //we are ignoring last two fields.. but reading it out of the stream..
    ss>>position;
    ss>>position;
}

void Board::resetBoard() {
    std::stringstream startpos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"); //this is the fen for starting position
    fromFEN(startpos);
}

void Board::makeMove(int start, int dest, char promotion) {

    state &= (0b11110000);  //resetting enpassant effect

    char piece = board[start];

    //boundary for one complete move in the stack
    moveStack.push(-1);

    //stroing the altered squares so we can undo its effect easily
    moveStack.push(start);
    moveStack.push(piece);
    moveStack.push(dest);
    moveStack.push(board[dest]);
    
    //moves the require special attention
    if (piece=='p' || piece=='P') {
        //moved two steps... set enpassant 
        if (abs(start-dest)==16) {
            state|=(1<<3);
            int col = dest%8;
            state &= (0b11111000);
            state |= (col & 0b00000111);
        }
        else if (abs(start-dest)!=8 && board[dest]=='0') {
            //en passant condition
            if (piece=='P') moveStack.push(dest-8),moveStack.push(board[dest-8]);
            else moveStack.push(dest+8), moveStack.push(board[dest+8]);

            if (piece=='P') board[dest-8] = '0';
            else board[dest+8] = '0';
        } 
    }
    else if (piece=='k' || piece=='K') {
        //remove castling rights
        if (piece=='K') {
            state &= ~(1<<7);
            state &= ~(1<<6);
        }
        else {
            state &= ~(1<<5);
            state &= ~(1<<4);
        }

        //kingside castling
        if (dest-start==2) {
            moveStack.push(dest+1); //the rook
            moveStack.push(board[dest+1]);
            moveStack.push(dest-1);// the empty space on the right of the king
            moveStack.push(board[dest-1]);

            //moving the rook
            board[dest+1]='0';
            board[dest-1] = (turn=='w')?'R':'r';
        }
        else if (start-dest==2) { //queenside castle
            moveStack.push(dest-2); //the rook
            moveStack.push(board[dest-2]);
            moveStack.push(dest+1);// the empty space on the right of the king
            moveStack.push(board[dest+1]);

            //move the rook
            board[dest-2]='0';
            board[dest+1]=(turn=='w')?'R':'r';
        }
    }
    else if (piece=='r' || piece=='R') {    //removing corresponding castling rights on moving the rooks
        if (piece=='R') {
            if (start==0) state &= ~(1<<6);
            else if (start==7) state &= ~(1<<7);
        }
        else {
            if (start%8==0) state &= ~(1<<4);
            else if (start%8==7) state &= ~(1<<5);
        }
    }

    //removing corresponding castling rights when the rooks are captured
    if (board[dest]=='R') {
        if (dest==0) state &= ~(1<<6);
        else if (dest==7) state &= ~(1<<7);
    }
    else if (board[dest]=='r') {
        if (dest%8==0) state &= ~(1<<4);
        else if (dest%8==7) state &= ~(1<<5);
    }

    

    //make the move (the main one described in the move string)
    board[start] = '0';
    board[dest] = piece;

    //promotion 
    if (promotion!='0') {
        //in fen promotion is always in small . so we have to capitalize when white
        board[dest]=(turn=='w')?toupper(promotion):promotion;
    }

    //switching sides
    if (turn=='w') turn='b';
    else turn = 'w';
}

void Board::makeMove(std::string move) {

    int start = move[0]-'a' + 8*(move[1]-'1');
    int dest = move[2]-'a' + 8*(move[3]-'1');
    char promotion = (move.size()==5)?move[4]:'0';

    makeMove(start,dest,promotion);
}


bool Board::isAttacked(int pos, char side) {

    if (side=='b') {
        //by pawns
        if (pos+7<64 && board[pos+7]=='p') return true;
        if (pos+9<64 && board[pos+9]=='p') return true;

        //by knights
        if (pos+17<64 && (pos%8<7) && board[pos+17]=='n') return true;
        if (pos+15<64 && (pos%8>0) && board[pos+15]=='n') return true;
        if (pos+10<64 && (pos%8<6) && board[pos+10]=='n') return true;
        if (pos+6<64 && (pos%8>1) && board[pos+6]=='n') return true;
        if (pos-6>=0 && (pos%8<6) && board[pos-6]=='n') return true;
        if (pos-10>=0 && (pos%8>1) && board[pos-10]=='n' ) return true;
        if (pos-15>=0 && (pos%8<7) && board[pos-15]=='n') return true;
        if (pos-17>=0 && (pos%8>0) && board[pos-17]=='n') return true;

        //bishops and queen(diagonal)
        for (int i=pos+9; i<64; i+=9) {
            if (i%8==0) break;
            if (board[i]=='b' || board[i]=='q') return true;
            if (board[i]!='0') break;
        }
        for (int i=pos-9; i>=0; i-=9) {
            if (i%8==7) break;
            if (board[i]=='b' || board[i]=='q') return true;
            if (board[i]!='0') break;
        }
        for (int i=pos+7; i<64; i+=7) {
            if (i%8==7) break;
            if (board[i]=='b' || board[i]=='q') return true;
            if (board[i]!='0') break;
        }
        for (int i=pos-7; i>=0; i-=7) {
            if (i%8==0) break;
            if (board[i]=='b' || board[i]=='q') return true;
            if (board[i]!='0') break;
        }

        //by rook and queen(horizontal)
        for (int i=pos+8; i<64; i+=8) {
            if (board[i]=='r' || board[i]=='q') return true;
            if (board[i]!='0') break;
        }
        for (int i=pos-8; i>=0; i-=8) {
            if (board[i]=='r' || board[i]=='q') return true;
            if (board[i]!='0') break;
        }
        for (int i=pos+1; i<64; i++) {
            if (i%8==0) break;
            if (board[i]=='r' || board[i]=='q') return true;
            if (board[i]!='0') break;
        }
        for (int i=pos-1; i>=0; i--) {
            if (i%8==7) break;
            if (board[i]=='r' || board[i]=='q') return true;
            if (board[i]!='0') break;
        }


        //by king
        if (pos+1<64 && board[pos+1]=='k') return true;
        if (pos+7<64 && board[pos+7]=='k') return true;
        if (pos+8<64 && board[pos+8]=='k') return true;
        if (pos+9<64 && board[pos+9]=='k') return true;
        if (pos-1>=0 && board[pos-1]=='k') return true;
        if (pos-7>=0 && board[pos-7]=='k') return true;
        if (pos-8>=0 && board[pos-8]=='k') return true;
        if (pos-9>=0 && board[pos-9]=='k') return true;
    }
    else {
        //by pawns
        if (pos-7>=0 && board[pos-7]=='P') return true;
        if (pos-9>=0 && board[pos-9]=='P') return true;

        //by knights
        if (pos+17<64 && board[pos+17]=='N') return true;
        if (pos+15<64 && board[pos+15]=='N') return true;
        if (pos+10<64 && board[pos+10]=='N') return true;
        if (pos+6<64 && board[pos+6]=='N') return true;
        if (pos-6>=0 && board[pos-6]=='N') return true;
        if (pos-10>=0 && board[pos-10]=='N' ) return true;
        if (pos-15>=0 && board[pos-15]=='N') return true;
        if (pos-17>=0 && board[pos-17]=='N') return true;

        //bishops and queen(diagonal)
        for (int i=pos+9; i<64; i+=9) {
            if (i%8==0) break;
            if (board[i]=='B' || board[i]=='Q') return true;
            if (board[i]!='0') break;
        }
        for (int i=pos-9; i>=0; i-=9) {
            if (i%8==7) break;
            if (board[i]=='B' || board[i]=='Q') return true;
            if (board[i]!='0') break;
        }
        for (int i=pos+7; i<64; i+=7) {
            if (i%8==7) break;
            if (board[i]=='B' || board[i]=='Q') return true;
            if (board[i]!='0') break;
        }
        for (int i=pos-7; i>=0; i-=7) {
            if (i%8==0) break;
            if (board[i]=='B' || board[i]=='Q') return true;
            if (board[i]!='0') break;
        }

        //by rook and queen(horizontal)
        for (int i=pos+8; i<64; i+=8) {
            if (board[i]=='R' || board[i]=='Q') return true;
            if (board[i]!='0') break;
        }
        for (int i=pos-8; i>=0; i-=8) {
            if (board[i]=='R' || board[i]=='Q') return true;
            if (board[i]!='0') break;
        }
        for (int i=pos+1; i<64; i++) {
            if (i%8==0) break;
            if (board[i]=='R' || board[i]=='Q') return true;
            if (board[i]!='0') break;
        }
        for (int i=pos-1; i>=0; i--) {
            if (i%8==7) break;
            if (board[i]=='R' || board[i]=='Q') return true;
            if (board[i]!='0') break;
        }


        //by king
        if (pos+1<64 && board[pos+1]=='K') return true;
        if (pos+7<64 && board[pos+7]=='K') return true;
        if (pos+8<64 && board[pos+8]=='K') return true;
        if (pos+9<64 && board[pos+9]=='K') return true;
        if (pos-1>=0 && board[pos-1]=='K') return true;
        if (pos-7>=0 && board[pos-7]=='K') return true;
        if (pos-8>=0 && board[pos-8]=='K') return true;
        if (pos-9>=0 && board[pos-9]=='K') return true;
    }
    return false;
}

bool Board::isValid(int start, int dest) {
    if (dest<0 || dest>=64) return false;

    if (turn=='w' && isupper(board[dest])) return false;
    if (turn=='b' && islower(board[dest])) return false;

    char piece = board[start];

    //checking for pawn and king moving warping around the side edges
    if (piece=='P' || piece=='p' || piece=='k' || piece=='K') {
        if (abs(dest%8-start%8)>2) return false;
    }


    //checking for knight move warping around the side edges
    if (piece=='n' || piece=='N') {
        if (dest-start==17 && (start%8>6)) return false;
        if (dest-start==15 && (start%8<1)) return false;
        if (dest-start==10 && (start%8>5)) return false;
        if (dest-start==6 && (start%8<2)) return false;
        if (dest-start==-6 && (start%8>5)) return false;
        if (dest-start==-10 && (start%8<2)) return false;
        if (dest-start==-15 && (start%8>6)) return false;
        if (dest-start==-17 && (start%8<1)) return false;
    }
    //cheking for enpassant
    if (piece=='p' || piece=='P') {
        if ((abs(dest-start)==7 || abs(dest-start)==9) && board[dest]=='0') {
            //checking if the row is right
            if (turn=='w' && dest/8!=5) return false;
            else if (turn=='b' && dest/8!=2) return false;

            //enpassant itself is not possible
            if (!(state&(1<<3))) return false;
            //checking if column is right
            if (dest%8!= state%8) return false;
        }   
    }

    //check castling
    if ((piece=='k' || piece=='K') && (abs(dest-start)==2)) {
        if (dest>start) {
            if (turn=='w') {
                if (!(state & (1<<7))) return false;
            }
            else {
                if (!(state& (1<<5))) return false;
            }
            for (int i=start+1; i<=dest; i++) {
                if (board[i]!='0') return false;
            }
            for (int i=start; i<=dest; i++) {
                if (isAttacked(i,(turn=='w')?'b':'w')) return false;
            }
        }
        else {
            if (turn=='w') {
                if (!(state & (1<<6))) return false;
            }
            else {
                if (!(state & (1<<4))) return false;
            }
            for (int i=start-1; i>=dest-1; i--) {
                if (board[i]!='0') return false;
            }
            for (int i=start; i>=dest; i--) {
                if (isAttacked(i,(turn=='w')?'b':'w')) return false;
            }
        }
    }

    //remembering the states to undo
    unsigned char pastState = state;
    char pastTurn = turn;
    
    makeMove(start,dest,'0');

    //checking if king is still in check after the move is made
    int kingPos=0;
    if (pastTurn=='w') {
        for (;kingPos<64; kingPos++) {
            if (board[kingPos]=='K') break;
        }
    }
    else {
        for (kingPos=63;kingPos>=0; kingPos--) {
            if (board[kingPos]=='k') break;
        }
    }
    
    
    bool flg=true;

    if (isAttacked(kingPos,turn)) flg=false;

    undo();
    state=pastState;
    turn=pastTurn;

    return flg;
}

char& Board::operator[](int i) {
    return board[i];
}

void Board::print() {
    for (int i=0;i<64; i++) std::cout<<board[i]<<((i%8==7)?"\n":" ");
}

void Board::undo() {
    char pc = moveStack.top();
    moveStack.pop();
    while (pc!=-1) {   
        int dest = moveStack.top();
        moveStack.pop();
        board[dest] = pc;
        pc = moveStack.top();
        moveStack.pop();
    }
}