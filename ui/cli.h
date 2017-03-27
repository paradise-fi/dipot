#pragma once
#include <string>
#include <iostream>

namespace dipot {

struct Command {
    virtual void run() = 0;
    virtual void setup() {}
    virtual ~Command() {}
};

struct Formula : Command {
    std::string formula;

    void run() override;
};

struct Help {
    std::string cmd;

    template< typename Parser >
    void run( Parser cmds ) {
        std::cerr << "Here should be dipot help." << std::endl;
    }
};

} // namespace dipot
