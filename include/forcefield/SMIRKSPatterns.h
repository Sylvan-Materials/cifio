#pragma once

#include <functional>
//#include <multi_index_container.hpp>
//#include <multi_index/sequenced_index.hpp>
//#include <multi_index/ordered_index.hpp>
//#include <multi_index/identity.hpp>
//#include <multi_index/member.hpp>
//#include "multi_index/composite_key.hpp"
//#include <multi_index/hashed_index.hpp>

#include "lemon/list_graph.h"
#include "lemon/connectivity.h"

#include "antlr4-runtime.h"
#include "parsing/SMIRKSLexer.h"
#include "parsing/SMIRKSParser.h"

#include "forcefield/bond_type.h"

namespace sylvanmats::forcefield {
    
    enum CHIRALITY_TYPE{
        UNSPECIFIED,
        ANTICLOCKWISE,
        CLOCKWISE,
        CLASS
    };
    
    struct atom_primitive{
        bool wildcard=false;
        char8_t atomic_number=0;
        char8_t connectivity=0;
        bool aromatic=false;
        bool aliphatic=false;
        short ring_connectivity=0;
        short ring_membership=0;
        short ring_size=0;
        short valence=0;
        char8_t formal_charge=0;
        CHIRALITY_TYPE chirality=UNSPECIFIED;
        short degree=0;
        short total_H_count=0;
        short implicit_H_count=0;
        short map_class=0;
    };
    
    struct bond_primitive{
        BOND_TYPE type=BOND_ANY;
        bool exclude=false;
        bool any_ring=false;
    };
    
    struct smirks_pattern{
    public:
        std::string smirks;
        mutable double length;
        mutable double k;
        lemon::ListGraph smirksGraph;
        lemon::ListGraph::NodeMap<atom_primitive> atomicPrimitives;
        lemon::ListGraph::EdgeMap<bond_primitive> bondPrimitives;
        smirks_pattern(): smirks (""), length (0.0), k (0.0), smirksGraph (lemon::ListGraph()), atomicPrimitives(smirksGraph), bondPrimitives(smirksGraph) {
        
        };
        smirks_pattern(std::string& smirks, double length, double k): smirks (smirks), length (length), k (k), smirksGraph (lemon::ListGraph()), atomicPrimitives(smirksGraph), bondPrimitives(smirksGraph) {
        
        };
        smirks_pattern(const smirks_pattern& orig): smirks (orig.smirks), length (orig.length), k (orig.k), smirksGraph (lemon::ListGraph()), atomicPrimitives (smirksGraph), bondPrimitives (smirksGraph) {
            if(lemon::countNodes(orig.smirksGraph)>0){
                lemon::ListGraph::NodeMap<lemon::ListGraph::Node> nr(orig.smirksGraph);
                lemon::ListGraph::EdgeMap<lemon::ListGraph::Edge> ecr(orig.smirksGraph);
                lemon::graphCopy<lemon::ListGraph, lemon::ListGraph>(orig.smirksGraph, smirksGraph).nodeRef(nr).edgeRef(ecr).run();
                for(lemon::ListGraph::NodeIt n(orig.smirksGraph);n!=lemon::INVALID; ++n){
                    atomicPrimitives[nr[n]].wildcard=orig.atomicPrimitives[n].wildcard;
                    atomicPrimitives[nr[n]].atomic_number=orig.atomicPrimitives[n].atomic_number;
                    atomicPrimitives[nr[n]].connectivity=orig.atomicPrimitives[n].connectivity;
                    atomicPrimitives[nr[n]].aromatic=orig.atomicPrimitives[n].aromatic;
                    atomicPrimitives[nr[n]].aliphatic=orig.atomicPrimitives[n].aliphatic;
                    atomicPrimitives[nr[n]].ring_connectivity=orig.atomicPrimitives[n].ring_connectivity;
                    atomicPrimitives[nr[n]].ring_membership=orig.atomicPrimitives[n].ring_membership;
                    atomicPrimitives[nr[n]].ring_size=orig.atomicPrimitives[n].ring_size;
                    atomicPrimitives[nr[n]].valence=orig.atomicPrimitives[n].valence;
                    atomicPrimitives[nr[n]].formal_charge=orig.atomicPrimitives[n].formal_charge;
                    atomicPrimitives[nr[n]].chirality=orig.atomicPrimitives[n].chirality;
                    atomicPrimitives[nr[n]].degree=orig.atomicPrimitives[n].degree;
                    atomicPrimitives[nr[n]].total_H_count=orig.atomicPrimitives[n].total_H_count;
                    atomicPrimitives[nr[n]].implicit_H_count=orig.atomicPrimitives[n].implicit_H_count;
                    atomicPrimitives[nr[n]].map_class=orig.atomicPrimitives[n].map_class;
                }
                for(lemon::ListGraph::EdgeIt e(orig.smirksGraph);e!=lemon::INVALID; ++e){
                    bondPrimitives[ecr[e]].type=orig.bondPrimitives[e].type;
                    bondPrimitives[ecr[e]].exclude=orig.bondPrimitives[e].exclude;
                    bondPrimitives[ecr[e]].any_ring=orig.bondPrimitives[e].any_ring;
                }
            }
        };
        smirks_pattern(smirks_pattern&& orig) = default;
        virtual ~smirks_pattern() = default;
        smirks_pattern& operator=(const smirks_pattern& other) // copy assignment
    {
        return *this = smirks_pattern(other);
    }
        smirks_pattern& operator=(smirks_pattern&& other) noexcept // move assignment
    {
        std::swap(smirks, other.smirks);
        std::swap(length, other.length);
        std::swap(k, other.k);
            if(lemon::countNodes(other.smirksGraph)>0){
                lemon::ListGraph::NodeMap<lemon::ListGraph::Node> nr(other.smirksGraph);
                lemon::ListGraph::EdgeMap<lemon::ListGraph::Edge> ecr(other.smirksGraph);
                lemon::graphCopy<lemon::ListGraph, lemon::ListGraph>(other.smirksGraph, smirksGraph).nodeRef(nr).edgeRef(ecr).run();
                for(lemon::ListGraph::NodeIt n(other.smirksGraph);n!=lemon::INVALID; ++n){
                    atomicPrimitives[nr[n]].wildcard=other.atomicPrimitives[n].wildcard;
                    atomicPrimitives[nr[n]].atomic_number=other.atomicPrimitives[n].atomic_number;
                    atomicPrimitives[nr[n]].connectivity=other.atomicPrimitives[n].connectivity;
                    atomicPrimitives[nr[n]].aromatic=other.atomicPrimitives[n].aromatic;
                    atomicPrimitives[nr[n]].aliphatic=other.atomicPrimitives[n].aliphatic;
                    atomicPrimitives[nr[n]].ring_connectivity=other.atomicPrimitives[n].ring_connectivity;
                    atomicPrimitives[nr[n]].ring_membership=other.atomicPrimitives[n].ring_membership;
                    atomicPrimitives[nr[n]].ring_size=other.atomicPrimitives[n].ring_size;
                    atomicPrimitives[nr[n]].valence=other.atomicPrimitives[n].valence;
                    atomicPrimitives[nr[n]].formal_charge=other.atomicPrimitives[n].formal_charge;
                    atomicPrimitives[nr[n]].chirality=other.atomicPrimitives[n].chirality;
                    atomicPrimitives[nr[n]].degree=other.atomicPrimitives[n].degree;
                    atomicPrimitives[nr[n]].total_H_count=other.atomicPrimitives[n].total_H_count;
                    atomicPrimitives[nr[n]].implicit_H_count=other.atomicPrimitives[n].implicit_H_count;
                    atomicPrimitives[nr[n]].map_class=other.atomicPrimitives[n].map_class;
                }
                for(lemon::ListGraph::EdgeIt e(other.smirksGraph);e!=lemon::INVALID; ++e){
                    bondPrimitives[ecr[e]].type=other.bondPrimitives[e].type;
                    bondPrimitives[ecr[e]].exclude=other.bondPrimitives[e].exclude;
                    bondPrimitives[ecr[e]].any_ring=other.bondPrimitives[e].any_ring;
                }
            }
        return *this;
    }
//        bool operator<(const smirks_pattern& e)const{return lemon::countNodes(smirksGraph)<lemon::countNodes(e.smirksGraph);}
    };
    
    class SMIRKSPatterns{
    protected:
        std::vector<smirks_pattern> bondSet;
//        bond_set_by_smirks& smirksIndex;
        std::vector<smirks_pattern> angleSet;
    public:
        SMIRKSPatterns();
        SMIRKSPatterns(const SMIRKSPatterns& orig) = delete;
        virtual ~SMIRKSPatterns() =  default;
        
        void operator()(char8_t atomic_numberA, char8_t connectivityA, BOND_TYPE type, char8_t connectivityB, char8_t atomic_numberB, std::function<bool(double length, double k, smirks_pattern& smirksPattern)> apply);
        void operator()(char8_t atomic_numberA, char8_t connectivityA, BOND_TYPE typeA, char8_t connectivityB, char8_t atomic_numberB, BOND_TYPE typeB, char8_t connectivityC, char8_t atomic_numberC, std::function<bool(double angle, double k, smirks_pattern& smirksPattern)> apply);
        
    private:
        lemon::ListGraph::Node processAtoms(sylvanmats::SMIRKSParser::AtomsContext* atoms, sylvanmats::forcefield::smirks_pattern& smirksPattern);
        void processBonds(sylvanmats::SMIRKSParser::Bond_primitivesContext* bc, lemon::ListGraph::Edge& e, sylvanmats::forcefield::smirks_pattern& smirksPattern);
    };
}
