#include <string>
#include <sstream>
#include <array>
#include <iostream>

#include "engine.hpp"
#include "board.hpp"

const int INF = 10000000;

Engine::Engine() : board() {
    info = "id name Neptune\nid author RyanRajesh";
}



void Engine::makeMoves(std::stringstream& ss) {
    std::string move;
    while (ss>>move) {
        board.makeMove(move);
    }
}

int Engine::backtrack(int start, int dest, char promotion, int bestScore, int depth) {
    //remembering past state
    unsigned char pastState = board.state;
    char pastTurn = board.turn;

    //making the move 
    board.makeMove(start, dest, promotion);

    int nextScore = search(depth+1);

    //undoing the move
    board.undo();

    //set the states back
    board.state=pastState;
    board.turn = pastTurn;

    return nextScore;
}



int Engine::search(int depth) {
    if (depth==maxDepth) return staticEval();

    int bestScore = (board.turn=='w')?-INF:INF;


    for (int start=0;start<64; start++) {
        if ((board.turn=='w' && isupper(board[start])) || (board.turn=='b'&& islower(board[start]))) {
            char piece = board[start];
            
            if (piece=='p' || piece=='P'){
                int dest;
                //single step move
                if (board.turn=='w') dest=start+8;
                else dest=start-8;
                
                if (board.isValid(start,dest) && board[dest]=='0') {
                    if (dest/8==7 || dest/8==0) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'q',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'q',bestScore,depth));
                    }
                    else {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                }

                //double step move
                if (board.turn=='w') dest=start+16;
                else dest=start-16;

                if ((board.isValid(start,dest)) && (board[dest]=='0') && ((board.turn=='w' && start/8==1 && board[start+8]=='0') || (board.turn=='b' && start/8==6 && board[start-8]=='0'))) {
                    if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                }

                //capture
                if (board.turn=='w') dest=start+7;
                else dest=start-7;

                if (board.isValid(start,dest) && board[dest]!='0') {
                    if (dest/8==7 || dest/8==0) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'q',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'q',bestScore,depth));
                    }
                    else {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                }

                //capture
                if (board.turn=='w') dest=start+9;
                else dest = start-9;

                if (board.isValid(start,dest) && board[dest]!='0') {
                    if (dest/8==7 || dest/8==0) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'q',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'q',bestScore,depth));
                    }
                    else {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                }
            }
            else if (piece=='k' || piece=='K') {
                //main moves+castles
                std::array<int,10> moves = {1,7,8,9,-1,-7,-8,-9,2,-2}; 
                for (int move : moves) {
                    int dest = start+move;
                    if (board.isValid(start,dest)) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                }
            }
            else if (piece=='n' || piece=='N') {
                std::array<int,8> moves = {17,15,10,6,-6,-10,-15,-17};
                for (int move : moves) {
                    int dest = start+move;
                    if (board.isValid(start,dest)) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                }
            }
            else if (piece=='b' || piece=='B') {
                for (int dest=start+9; dest<64; dest+=9) {
                    if (dest%8==0) break;
                    if (board.isValid(start,dest)) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start-9; dest>=0; dest-=9) {
                    if (dest%8==7) break;
                    if (board.isValid(start,dest)) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start+7; dest<64; dest+=7) {
                    if (dest%8==7) break;
                    if (board.isValid(start,dest)) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start-7; dest>=0; dest-=7) {
                    if (dest%8==0) break;
                    if (board.isValid(start,dest)) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                    if (board[dest]!='0') break;
                }
            }
            else if (piece=='r'|| piece=='R') {
                for (int dest=start+1; dest<64; dest++) {
                    if (dest%8==0) break;
                    if (board.isValid(start,dest)) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start-1; dest>=0; dest--) {
                    if (dest%8==7) break;
                    if (board.isValid(start,dest)) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start+8; dest<64; dest+=8) {
                    if (board.isValid(start,dest)) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start-8; dest>=0; dest-=8) {
                    if (board.isValid(start,dest)) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                    if (board[dest]!='0') break;
                }
            }
            else if (piece=='q' || piece=='Q') {
                for (int dest=start+9; dest<64; dest+=9) {
                    if (dest%8==0) break;
                    if (board.isValid(start,dest)) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start-9; dest>=0; dest-=9) {
                    if (dest%8==7) break;
                    if (board.isValid(start,dest)) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start+7; dest<64; dest+=7) {
                    if (dest%8==7) break;
                    if (board.isValid(start,dest)) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start-7; dest>=0; dest-=7) {
                    if (dest%8==0) break;
                    if (board.isValid(start,dest)) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start+1; dest<64; dest++) {
                    if (dest%8==0) break;
                    if (board.isValid(start,dest)) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start-1; dest>=0; dest--) {
                    if (dest%8==7) break;
                    if (board.isValid(start,dest)) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start+8; dest<64; dest+=8) {
                    if (board.isValid(start,dest)) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start-8; dest>=0; dest-=8) {
                    if (board.isValid(start,dest)) {
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    }
                    if (board[dest]!='0') break;
                }
            }
        }  
    }
    return bestScore;
}

std::pair<std::string,int> Engine::bestMove() {
    int bestStart=-1,bestDest=-1;
    int depth=0;
    int bestScore = (board.turn=='w')?-INF:INF;
    char promotion='0';

    for (int start=0; start<64; start++) {
        if ((board.turn=='w' && isupper(board[start])) || (board.turn=='b'&& islower(board[start]))) {
            char piece = board[start];
            
            if (piece=='p' || piece=='P'){
                int dest;

                //single step move
                if (board.turn=='w') dest=start+8;
                else dest=start-8;
                
                if (board.isValid(start,dest) && board[dest]=='0') {
                    if (bestStart==-1) {
                        bestStart=start;
                        bestDest=dest;
                    }
                    if (dest/8==7 || dest/8==0) {
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'q',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'q',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                            promotion='q';
                        }
                    }
                    else {
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                }

                //double step move
                if (board.turn=='w') dest=start+16;
                else dest=start-16;

                if ((board.isValid(start,dest)) && (board[dest]=='0') && ((board.turn=='w' && start/8==1 && board[start+8]=='0') || (board.turn=='b' && start/8==6 && board[start-8]=='0'))) {
                    if (bestStart==-1) {
                        bestStart=start;
                        bestDest=dest;
                    }
                    int pastScore = bestScore;
                    if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                    if (pastScore!=bestScore) {
                        bestStart=start;
                        bestDest = dest;
                    }
                }

                //capture
                if (board.turn=='w') dest=start+7;
                else dest=start-7;

                if (board.isValid(start,dest) && board[dest]!='0') {
                    if (bestStart==-1) {
                        bestStart=start;
                        bestDest=dest;
                    }
                    if (dest/8==7 || dest/8==0) {
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'q',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'q',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                            promotion='q';
                        }
                    }
                    else {
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                }

                //captrue
                if (board.turn=='w') dest=start+9;
                else dest = start-9;

                if (board.isValid(start,dest) && board[dest]!='0') {
                    if (bestStart==-1) {
                        bestStart=start;
                        bestDest=dest;
                    }
                    if (dest/8==7 || dest/8==0) {
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'q',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'q',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                            promotion='q';
                        }
                    }
                    else {
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                }
            }
            else if (piece=='k' || piece=='K') {
                //main moves+castles
                std::array<int,10> moves = {1,7,8,9,-1,-7,-8,-9,2,-2}; 
                for (int move : moves) {
                    int dest = start+move;
                    if (board.isValid(start,dest)) {
                        if (bestStart==-1) {
                            bestStart=start;
                            bestDest=dest;
                        }
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                }
            }
            else if (piece=='n' || piece=='N') {
                std::array<int,8> moves = {17,15,10,6,-6,-10,-15,-17};
                for (int move : moves) {
                    int dest = start+move;
                    if (board.isValid(start,dest)) {
                        if (bestStart==-1) {
                            bestStart=start;
                            bestDest=dest;
                        }
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                }
            }
            else if (piece=='b' || piece=='B') {
                for (int dest=start+9; dest<64; dest+=9) {
                    if (dest%8==0) break;
                    if (board.isValid(start,dest)) {
                        if (bestStart==-1) {
                            bestStart=start;
                            bestDest=dest;
                        }
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start-9; dest>=0; dest-=9) {
                    if (dest%8==7) break;
                    if (board.isValid(start,dest)) {
                        if (bestStart==-1) {
                            bestStart=start;
                            bestDest=dest;
                        }
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start+7; dest<64; dest+=7) {
                    if (dest%8==7) break;
                    if (board.isValid(start,dest)) {
                        if (bestStart==-1) {
                            bestStart=start;
                            bestDest=dest;
                        }
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start-7; dest>=0; dest-=7) {
                    if (dest%8==0) break;
                    if (board.isValid(start,dest)) {
                        if (bestStart==-1) {
                            bestStart=start;
                            bestDest=dest;
                        }
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                    if (board[dest]!='0') break;
                }
            }
            else if (piece=='r'|| piece=='R') {
                for (int dest=start+1; dest<64; dest++) {
                    if (dest%8==0) break;
                    if (board.isValid(start,dest)) {
                        if (bestStart==-1) {
                            bestStart=start;
                            bestDest=dest;
                        }
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start-1; dest>=0; dest--) {
                    if (dest%8==7) break;
                    if (board.isValid(start,dest)) {
                        if (bestStart==-1) {
                            bestStart=start;
                            bestDest=dest;
                        }
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start+8; dest<64; dest+=8) {
                    if (board.isValid(start,dest)) {
                        if (bestStart==-1) {
                            bestStart=start;
                            bestDest=dest;
                        }
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start-8; dest>=0; dest-=8) {
                    if (board.isValid(start,dest)) {
                        if (bestStart==-1) {
                            bestStart=start;
                            bestDest=dest;
                        }
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                    if (board[dest]!='0') break;
                }
            }
            else if (piece=='q' || piece=='Q') {
                for (int dest=start+9; dest<64; dest+=9) {
                    if (dest%8==0) break;
                    if (board.isValid(start,dest)) {
                        if (bestStart==-1) {
                            bestStart=start;
                            bestDest=dest;
                        }
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start-9; dest>=0; dest-=9) {
                    if (dest%8==7) break;
                    if (board.isValid(start,dest)) {
                        if (bestStart==-1) {
                            bestStart=start;
                            bestDest=dest;
                        }
                       int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start+7; dest<64; dest+=7) {
                    if (dest%8==7) break;
                    if (board.isValid(start,dest)) {
                        if (bestStart==-1) {
                            bestStart=start;
                            bestDest=dest;
                        }
                       int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start-7; dest>=0; dest-=7) {
                    if (dest%8==0) break;
                    if (board.isValid(start,dest)) {
                        if (bestStart==-1) {
                            bestStart=start;
                            bestDest=dest;
                        }
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start+1; dest<64; dest++) {
                    if (dest%8==0) break;
                    if (board.isValid(start,dest)) {
                        if (bestStart==-1) {
                            bestStart=start;
                            bestDest=dest;
                        }
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start-1; dest>=0; dest--) {
                    if (dest%8==7) break;
                    if (board.isValid(start,dest)) {
                        if (bestStart==-1) {
                            bestStart=start;
                            bestDest=dest;
                        }
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start+8; dest<64; dest+=8) {
                    if (board.isValid(start,dest)) {
                        if (bestStart==-1) {
                            bestStart=start;
                            bestDest=dest;
                        }
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                    if (board[dest]!='0') break;
                }
                for (int dest=start-8; dest>=0; dest-=8) {
                    if (board.isValid(start,dest)) {
                        if (bestStart==-1) {
                            bestStart=start;
                            bestDest=dest;
                        }
                        int pastScore = bestScore;
                        if (board.turn=='w') bestScore = std::max(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        else bestScore = std::min(bestScore,backtrack(start,dest,'0',bestScore,depth));
                        if (pastScore!=bestScore) {
                            bestStart=start;
                            bestDest = dest;
                        }
                    }
                    if (board[dest]!='0') break;
                }
            }
        }  
    }
    char startCol = (bestStart%8)+'a';
    char startRow = (bestStart/8) +'1';
    char destCol = (bestDest%8) +'a';
    char destRow = (bestDest/8)+'1';

    std::string bestmove;
    bestmove+=startCol;
    bestmove+=startRow;
    bestmove+=destCol;
    bestmove+=destRow;

    if (promotion!='0') bestmove+=promotion;

    return {bestmove,bestScore};
 }

 int Engine::staticEval() {
    int eval=0;
    
    for (int i=0; i<64; i++) {
        char piece = board[i];
        if (piece=='0') continue;
        if (piece=='p') eval-=(100),eval+=pawnScore[i];
        else if (piece=='P') eval+=(100),eval+=pawnScore[i];
        else if (piece=='n') eval-=(320+knightScore[i]);
        else if (piece=='N') eval+=(320+knightScore[i]);
        else if (piece=='b') eval-=(330+bishopScore[i]);
        else if (piece=='B') eval+=(330+bishopScore[i]);
        else if (piece=='r') eval-=(500+rookScore[i]);
        else if (piece=='R') eval+=(500+rookScore[i]);
        else if (piece=='q') eval-=900;
        else if (piece=='Q') eval+=900;
        else if (piece=='k') eval-=(20000+kingScore[i]);
        else if (piece=='K') eval+=(20000+kingScore[i]);
        
    }
    
    return eval;
 }