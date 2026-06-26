#include <iostream>
#include <string>
#include <sstream>
#include <future>

#include "interface.hpp"
#include "engine.hpp"

Interface::Interface() : engine() {}

void Interface::mainloop() {

    engine.maxDepth=4;
    std::string line;
    
    while (true) {
        getline(std::cin, line);
        std::stringstream ss(line);

        std::string cmd;
        ss>>cmd;

        if (cmd=="quit") {
            return;
        }
        else if (cmd=="uci") {
            std::cout<<engine.info<<std::endl;
            std::cout<<"uciok"<<std::endl;
        }
        else if (cmd=="isready") {
            std::cout<<"readyok"<<std::endl;
        }
        else if (cmd=="position") {
            ss>>cmd;
            if (cmd=="startpos") {
                engine.board.resetBoard();
            } 
            else if (cmd=="fen") {
                engine.board.fromFEN(ss);
            }
            //if "moves" is there in the stream
            if (ss>>cmd) {
                engine.makeMoves(ss);
            }
        } 
        else if (cmd=="go") {
            std::string token;
            ss>>token;  
            auto [bestMove,score] = engine.bestMove();
            if (token=="infinite") {
                std::cout<<"info depth "<<engine.maxDepth<<" score cp "<<score<<" pv "<<bestMove<<std::endl;
            }
            else {
                std::cout<<"bestmove "<<bestMove<<std::endl;
            }
        }
    }
}