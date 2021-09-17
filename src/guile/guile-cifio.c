#include <iostream>
#include <sstream>
#include <functional>
#include <math.h>
#include <libguile.h>

#include "constitution/Graph.h"
#include "constitution/Populator.h"

extern "C" SCM
j0_wrapper (SCM x)
{
    std::cout<<"in j0_wrapper "<<x<<std::endl;
  return scm_from_double (j0 (scm_to_double (x)));
}

static SCM graph_type;

extern "C" void Populator(sylvanmats::constitution::Populator& populator, sylvanmats::constitution::Graph& graph, std::stringstream& contentBuf){
    std::cout<<"getNumberOfAtomSites "<<populator.getNumberOfStandardizedAminoAcids()<<std::endl;
        std::istream& is=dynamic_cast<std::istream&>(contentBuf);
    std::cout<<"casted Populator "<<is.good()<<std::endl;
       //std::function<void(sylvanmats::constitution::Graph& graph)> f=[](sylvanmats::constitution::Graph& graph){};
        populator.operator()(is, graph, [](sylvanmats::constitution::Graph& graph){});
        //std::invoke(&sylvanmats::constitution::Populator::operator(), populator, is, graph, f);
    std::cout<<"end Populator "<<std::endl;
}

extern "C" SCM
populate_graph (SCM content)
{
    std::stringstream contentBuf(scm_to_stringn (content, nullptr, "ascii", SCM_FAILED_CONVERSION_ERROR));
    sylvanmats::constitution::Graph* graph= (class sylvanmats::constitution::Graph *)scm_gc_malloc (sizeof (class sylvanmats::constitution::Graph), "graph");
    sylvanmats::constitution::Populator populator;
    Populator(populator, *graph, contentBuf);
    std::cout<<"in populate_graph "<<contentBuf.str().size()<<std::endl;
    return scm_make_foreign_object_1 (graph_type, graph);
}

extern "C" void
init_cifio ()
{
  SCM name = scm_from_utf8_symbol ("graph");
  SCM slots = scm_list_1 (scm_from_utf8_symbol ("data"));
  graph_type = scm_make_foreign_object_type (name, slots, nullptr);
  scm_c_define_gsubr ("j0", 1, 0, 0, (void*)j0_wrapper);
  scm_c_define_gsubr ("populate_graph", 1, 0, 0, (void*)populate_graph);
}
