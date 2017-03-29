#pragma once
#include <string>
#include <iostream>
#include <ui/die.h>

namespace dipot {

struct Command {
    virtual void run() = 0;
    virtual void setup() {}
    virtual ~Command() {}
};

struct Automat : Command {
    std::string formula;

    void run() override;
};

struct Help {
    std::string cmd = std::string("");

    template< typename Parser >
    void run( Parser cmds ) {
        std::string description = cmds.describe( cmd );
        if ( description.empty() && !cmd.empty() )
            die( "Unknown command '" + cmd + "'. Available commands are:\n" + cmds.describe() );
        if ( cmd.empty() )
        {
            std::cerr << "To print details about a specific command, run 'dipot help {command}'.\n\n";
            std::cerr << cmds.describe() << std::endl;
        }
        else std::cerr << description << std::endl;
    }
};

} // namespace dipot
