#pragma once

#include <ostream>
#include <spot/twa/fwd.hh>
#include <spot/misc/common.hh>

namespace dipot {
    void print_c_format( std::ostream& os, const spot::twa_graph_ptr& aut );
} // namespace dipot
