#include "cformat.h"

#include <iterator>
#include <spot/twa/bddprint.hh>
#include <spot/twa/twagraph.hh>
#include <spot/tl/print.hh>
#include <spot/twa/formula2bdd.hh>

using namespace spot;

namespace {
struct c_format_printer {
    c_format_printer( std::ostream& os ) : out_( os ), indent_level( 0 ) {}

    void print( const twa_graph_ptr & aut ) {
        print_atomic_propositions( aut );
        print_header();
        print_step_function( aut );
        print_footer();
    }

private:
    void print_atomic_propositions( const twa_graph_ptr & aut ) {
        const spot::bdd_dict_ptr & dict = aut->get_dict();
        for ( spot::formula ap : aut->ap() )
          out_ << "bool " << ap << "();\n";
        out_ << "\n";
    }

    void print_header() {
        out_ << "void __buchi_accept();\n";
        out_ << "void __buchi_cancel();\n";
        out_ << "unsigned __buchi_choose(unsigned);\n\n";

        out_ << "struct BuchiAutomaton\n";
        open_br();
    }

    void print_step_function( const twa_graph_ptr & aut ) {
        print_constructor( aut );
        out_ << indent() << "void step()\n";
        open_br();

        out_ << indent() << "switch(state)\n";
        open_br();

        for ( unsigned s = 0; s < aut->num_states(); ++s )
            print_state(s, aut);

        close_br();
        close_br();
    }

    void print_constructor( const twa_graph_ptr & aut) {
        out_ << indent() << name << "() : state(" << aut->get_init_state_number() << ") {}\n";
    }

    void print_state( const unsigned n, const twa_graph_ptr & aut) {
        out_ << indent() << "case " << n << ": {\n";
        indent_level++;

        bdd all = bddtrue;
        bool deterministic = true;
        for ( auto & t : aut->out( n ) ) {
            if ( !bdd_implies( t.cond, all ) )
                deterministic = false;
            all -= t.cond;
        }

        auto succs_num = [&]( const unsigned i ) {
          return std::distance(aut->out(i).begin(), aut->out(i).end());
        };

        if ( !deterministic )
          out_ << indent() << "unsigned nd" << n << " = __buchi_choose(" << succs_num(n) << ");\n";

        size_t idx = 0;
        for ( auto & t: aut->out( n ) ) {
            print_edge( aut, t, n, t.dst, deterministic, idx );
            ++idx;
        }

        if ( all != bddfalse )
            print_stuck_state();

        out_ << indent() << "break;\n";
        indent_level--;
        out_ << indent() << "}\n";
    }

    void print_edge( const twa_graph_ptr & aut,
                     const twa_graph_edge_data & edge,
                     const unsigned from,
                     const unsigned to,
                     bool deterministic,
                     const unsigned idx)
    {
        if ( idx == 0 )
            out_ << indent() << "if (";
        else
            out_ << indent() << "else if (";
        print_edge_label(aut, edge, from, deterministic, idx);
        out_ << ")\n";

        open_br();
        if ( edge.acc ) {
            out_ << indent() << "__buchi_accept();\n";
        }
        out_ << indent() << "state = " << to << ";\n";
        close_br();
    }

    void print_edge_label( const twa_graph_ptr & aut,
                           const twa_graph_edge_data & edge,
                           const unsigned from,
                           bool deterministic,
                           const unsigned idx )
    {
        if( !deterministic )
            out_ << "(nd" << from << " == " << idx << ") && ";

        const spot::bdd_dict_ptr & dict = aut->get_dict();
        formula f = bdd_to_formula( edge.cond, dict );
        print_formula( f );
    }

    void print_formula( const formula & f ) {
        op o = f.kind();
        switch ( o ) {
            case op::ff:
              out_ << "false";
              break;
            case op::tt:
              out_ << "true";
              break;
            case op::ap:
              out_ << f.ap_name() << "()";
              break;
            case op::Not:
              out_ << "!";
              print_formula(f[0]);
              break;
            case op::Or:
              print_formula(f[0]);
              out_ << " || ";
              print_formula(f[1]);
              break;
            case op::And:
              print_formula(f[0]);
              out_ << " && ";
              print_formula(f[1]);
            default:
              break;
        }
    }

    void print_stuck_state() {
        out_ << indent() << "else\n";
        open_br();
        out_ << indent() << "__buchi_cancel();\n";
        close_br();
    }

    void print_footer() {
        out_ << "\n";
        out_ << indent() << "unsigned state;\n";
        indent_level--;
        out_ << indent() << "};\n";
    }

    std::string indent() {
        return std::string( indent_level * indent_step, ' ' );
    }

    void open_br() {
        out_ << indent() << "{\n";
        indent_level++;
    }

    void close_br() {
        indent_level--;
        out_ << indent() << "}\n";
    }

    std::ostream & out_;
    const std::string name = "BuchiAutomaton";
    unsigned indent_level;
    const unsigned indent_step = 2;
};
} // anonymous namespace


void dipot::print_c_format( std::ostream & os, const twa_graph_ptr & aut) {
    if ( !(aut->acc().is_buchi() || aut->acc().is_all() ) )
        throw std::runtime_error( "C output only supports Büchi acceptance" );
    c_format_printer printer(os);
    printer.print(aut);
}
