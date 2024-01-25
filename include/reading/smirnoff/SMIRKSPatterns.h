#pragma once

#include <functional>

#include "lemon/list_graph.h"
#include "lemon/connectivity.h"

#include "antlr4-runtime.h"
#include "parsing/SMIRKSLexer.h"
#include "parsing/SMIRKSParser.h"

#include "forcefield/bond_type.h"

namespace sylvanmats::reading {
    
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
        sylvanmats::forcefield::BOND_TYPE type=sylvanmats::forcefield::BOND_ANY;
        bool exclude=false;
        bool any_ring=false;
    };
    
    struct smirks_pattern{
    public:
        std::string smirks;
        mutable double length;
        mutable double k;
        lemon::ListGraph smirksGraph;
        lemon::ListGraph::NodeMap<std::vector<atom_primitive>> atomicPrimitives;
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
                    atomicPrimitives[nr[n]]=std::vector<atom_primitive>(orig.atomicPrimitives[n].begin(), orig.atomicPrimitives[n].end());
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
                    atomicPrimitives[nr[n]]=std::vector<atom_primitive>(other.atomicPrimitives[n].begin(), other.atomicPrimitives[n].end());
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
    
    struct smirks_torsions_pattern : public smirks_pattern {
    public:
        using smirks_pattern::smirks;
        using smirks_pattern::smirksGraph;
        
        smirks_torsions_pattern(): smirks_pattern(){};
        smirks_torsions_pattern(std::string& smirks, double length, double k) : smirks_pattern(smirks, 0.0, 0.0){};
        virtual ~smirks_torsions_pattern() = default;
        
    };
    struct smirks_vdw_pattern : public smirks_pattern {
        
    };
    class SMIRKSPatterns{
    protected:
        std::vector<smirks_pattern> bondSet;
//        bond_set_by_smirks& smirksIndex;
        std::vector<smirks_pattern> angleSet;
        std::vector<smirks_torsions_pattern> properSet;
        std::vector<smirks_torsions_pattern> improperSet;
        std::vector<smirks_vdw_pattern> vdwSet;
    public:
        SMIRKSPatterns();
        SMIRKSPatterns(const SMIRKSPatterns& orig) = delete;
        virtual ~SMIRKSPatterns() =  default;
        
        void operator()(char8_t atomic_numberA, char8_t connectivityA, sylvanmats::forcefield::BOND_TYPE type, char8_t connectivityB, char8_t atomic_numberB, std::function<bool(double length, double k, smirks_pattern& smirksPattern)> apply);
        void operator()(char8_t atomic_numberA, char8_t connectivityA, sylvanmats::forcefield::BOND_TYPE typeA, char8_t connectivityB, char8_t atomic_numberB, sylvanmats::forcefield::BOND_TYPE typeB, char8_t connectivityC, char8_t atomic_numberC, std::function<bool(double angle, double k, smirks_pattern& smirksPattern)> apply);
        
    private:
        lemon::ListGraph::Node processAtoms(sylvanmats::SMIRKSParser::AtomsContext* atoms, sylvanmats::reading::smirks_pattern& smirksPattern);
        void processBonds(sylvanmats::SMIRKSParser::Bond_primitivesContext* bc, lemon::ListGraph::Edge& e, sylvanmats::reading::smirks_pattern& smirksPattern);
    };
}
