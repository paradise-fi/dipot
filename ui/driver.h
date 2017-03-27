#pragma once

#include <brick-cmd>

#include <iostream>
#include <string>
#include <exception>

#include <spot/tl/parse.hh>
#include <spot/tl/print.hh>
#include <spot/misc/version.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/neverclaim.hh>
#include <spot/twaalgos/cformat.hh>

#include <format/cformat.h>
#include <ui/cli.h>

namespace dipot {

struct Driver {
    std::vector< std::string > args;

    Driver( int argc, const char *argv[] ) :
        args( brick::cmd::from_argv( argc, argv ) )
    { }

    template< typename Parser >
    auto parse( Parser p ) {
        return p.parse( args.begin(), args.end() );
    }

    auto commands() {
        // TODO specify validator
        auto v = brick::cmd::make_validator();
        auto hopts = brick::cmd::make_option_set< Help >( v )
            .option( "[{string}]", &Help::cmd, "print man to specified command" );
        auto fopts = brick::cmd::make_option_set< Formula >( v )
            .option( "[-f {string}]", &Formula::formula, "ltl formula" );
        return brick::cmd::make_parser( v )
            .command< Help >( hopts )
            .command< Formula >( fopts );
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

/*int main( int argc, char **argv ) {
    const std::string formula = "isZero U isOne";
    spot::parsed_formula pf = spot::parse_infix_psl(formula);
    if (pf.format_errors(std::cerr))
        return 1;
    spot::translator trans;
    trans.set_type(spot::postprocessor::BA);
    spot::twa_graph_ptr aut = trans.run(pf.f);
    dipot::print_c_format(std::cout, aut);
    return 0;
}*/
