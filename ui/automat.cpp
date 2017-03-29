#include <ui/cli.h>

#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>

#include <format/cformat.h>

namespace dipot {
    void Automat::run() {
        spot::parsed_formula pf = spot::parse_infix_psl( formula );
        if ( pf.format_errors( std::cerr ) )
            die( "LTL formula format error" );
        spot::translator trans;
        trans.set_type( spot::postprocessor::BA );
        spot::twa_graph_ptr aut = trans.run( pf.f );
        dipot::print_c_format( std::cout, aut );
    }
}
