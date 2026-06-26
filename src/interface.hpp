#ifndef INTERFACE_H
#define INTERFACE_H

#include "engine.hpp"

class Interface {
    public:
    Engine engine;

    Interface();
    void mainloop();
};

#endif
