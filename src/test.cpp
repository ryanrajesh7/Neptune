#include "board.hpp"
#include "engine.hpp"
#include <iostream>
#include <string>
#include <sstream>

int main() {
    Engine eng;
    eng.maxDepth = 4;   
    std::stringstream ss("1n1qkbnr/4r3/8/8/2p5/2N4b/P1PPQPK1/6R1 w k - 0 1");
    eng.board.fromFEN(ss);
    eng.board.makeMove(14,23,'0');
    eng.board.print();
    std::cout<<eng.board.isAttacked(23,'w')<<"\n";
    std::cout<<eng.bestMove().first<<"\n";
    return 0;
}