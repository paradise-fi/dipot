#pragma once

#include <brick-cmd>

#include <iostream>
#include <string>
#include <exception>

#include <ui/cli.h>

namespace dipot {

struct Driver {
    std::vector< std::string > args;

    Driver( int argc, const char *argv[] ) :
        args( brick::cmd::from_argv( argc, argv ) )
    { }

    template< typename Parser >
    auto parse( Parser p ) {
        if ( args.size() >= 1 )
            if ( args[0] == "--help" )
                args[0] = args[0].substr( 2 );
        return p.parse( args.begin(), args.end() );
    }

    auto commands() {
        // TODO specify validator
        auto v = brick::cmd::make_validator();
        auto hopts = brick::cmd::make_option_set< Help >( v )
            .option( "[{string}]", &Help::cmd, "print man to specified command" );
        auto fopts = brick::cmd::make_option_set< Automat >( v )
            .option( "[-p {string}|--property {string}]", &Automat::formula, "ltl property" );
        auto parser = brick::cmd::make_parser( v )
            .command< Automat >( fopts )
            .command< Help >( hopts );
        return parser;
    }

    int run() {
        try {
            auto cmds = commands();
            auto cmd = parse( cmds );

            if ( cmd.empty() ) {
                Help().run( cmds );
                return 0;
            }

            cmd.match( [&]( Help & help ) {
                           help.run( cmds );
                       },
                       [&]( Command & c ) {
                           c.setup();
                           c.run();
                       } );
            return 0;
        } catch( brick::except::Error &e ) {
            std::cerr << "ERROR: " << e.what() << std::endl;
            return e._exit;
        }
    }

    void process() {}
private :


};

} // namespace dipot
