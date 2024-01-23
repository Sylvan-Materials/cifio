#pragma once

#include <functional>
#include <multi_index_container.hpp>
#include <multi_index/sequenced_index.hpp>
#include <multi_index/ordered_index.hpp>
#include <multi_index/identity.hpp>
#include <multi_index/member.hpp>
#include "multi_index/composite_key.hpp"
#include <multi_index/hashed_index.hpp>

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
        lemon::ListGraph smirksGraph;
        lemon::ListGraph::NodeMap<atom_primitive> atomicPrimitives;
        lemon::ListGraph::EdgeMap<bond_primitive> bondPrimitives;
        smirks_pattern(): smirksGraph (lemon::ListGraph()), atomicPrimitives(smirksGraph), bondPrimitives(smirksGraph) {
        
        };
        smirks_pattern(const smirks_pattern& orig): smirksGraph (lemon::ListGraph()), atomicPrimitives (smirksGraph), bondPrimitives (smirksGraph) {
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
        virtual ~smirks_pattern() = default;
        bool operator<(const smirks_pattern& e)const{return lemon::countNodes(smirksGraph)<lemon::countNodes(e.smirksGraph);}
    };
    
    struct bond{
        int id;
        mutable smirks_pattern smirksPattern;
        char8_t atomic_numberA;
        char8_t connectivityA;
        BOND_TYPE type=BOND_ANY;
        char8_t connectivityB;
        char8_t atomic_numberB;
        mutable double length;
        mutable double k;
        bond() = default;
        bond(int id) :  id (id) {};
        bond(int id, smirks_pattern smirksPattern, char8_t atomic_numberA, char8_t connectivityA, BOND_TYPE type, char8_t connectivityB, char8_t atomic_numberB, double length, double k): id (id), smirksPattern (smirksPattern), atomic_numberA (atomic_numberA), connectivityA (connectivityA), type (type), connectivityB (connectivityB), atomic_numberB (atomic_numberB), length (length), k (k) {};
        bond(const bond& orig) = default;
        virtual ~bond() = default;
        bool operator<(const bond& e)const{return id<e.id;}
    };

    struct angle{
        int id;
        mutable smirks_pattern smirksPattern;
        char8_t atomic_numberA;
        char8_t connectivityA;
        BOND_TYPE typeA=BOND_ANY;
        char8_t connectivityB;
        char8_t atomic_numberB;
        BOND_TYPE typeB=BOND_ANY;
        char8_t connectivityC;
        char8_t atomic_numberC;
        mutable double angle_length;
        mutable double k;
        angle() = default;
        angle(int id) :  id (id) {};
        angle(int id, smirks_pattern smirksPattern, char8_t atomic_numberA, char8_t connectivityA, BOND_TYPE typeA, char8_t connectivityB, char8_t atomic_numberB, BOND_TYPE typeB, char8_t connectivityC, char8_t atomic_numberC, double angle_length, double k): id (id), smirksPattern (smirksPattern), atomic_numberA (atomic_numberA), connectivityA (connectivityA), typeA (typeA), connectivityB (connectivityB), atomic_numberB (atomic_numberB), typeB (typeB), connectivityC (connectivityC), atomic_numberC (atomic_numberC), angle_length (angle_length), k (k) {};
        angle(const angle& orig) = default;
        angle(angle&& orig) = default;
        virtual ~angle() = default;
        bool operator<(const angle& e)const{return id<e.id;}
    };

    struct smirk{};
    struct bond_connectvity{};
    struct wildcard_bond_connectvity{};
    struct angle_connectvity{};
    struct wildcard_angle_connectvity{};
    struct proper_connectvity{};
    struct improper_connectvity{};
    struct vdw_connectvity{};

    using bond_set = multi_index::multi_index_container<
      bond,
      multi_index::indexed_by<
        // sort by element
        multi_index::ordered_unique<multi_index::identity<bond> >,

        //multi_index::ordered_non_unique<multi_index::tag<name>,multi_index::member<bond,std::string,&bond::name> >,
        multi_index::ordered_non_unique<multi_index::tag<smirk>,multi_index::member<bond,smirks_pattern,&bond::smirksPattern> >,
        multi_index::hashed_non_unique<
            multi_index::tag<bond_connectvity>,
            multi_index::composite_key<bond,
                    multi_index::member<bond,char8_t,&bond::atomic_numberA>,
                    multi_index::member<bond,char8_t,&bond::connectivityA>,
                    multi_index::member<bond,BOND_TYPE,&bond::type>,
                    multi_index::member<bond,char8_t,&bond::connectivityB>,
                    multi_index::member<bond,char8_t,&bond::atomic_numberB>
                >
            >,
        multi_index::hashed_non_unique<
            multi_index::tag<wildcard_bond_connectvity>,
            multi_index::composite_key<bond,
                    multi_index::member<bond,char8_t,&bond::atomic_numberA>,
                    multi_index::member<bond,BOND_TYPE,&bond::type>,
                    multi_index::member<bond,char8_t,&bond::atomic_numberB>
                >
            >
      >
    >;
    using bond_set_by_smirks = multi_index::index<bond_set,smirk>::type;
    
    using angle_set = multi_index::multi_index_container<
      angle,
      multi_index::indexed_by<
        // sort by element
        multi_index::ordered_unique<multi_index::identity<angle> >,

        //multi_index::ordered_non_unique<multi_index::tag<name>,multi_index::member<angle,std::string,&angle::name> >,
        multi_index::ordered_non_unique<multi_index::tag<smirk>,multi_index::member<angle,smirks_pattern,&angle::smirksPattern> >,
        multi_index::hashed_non_unique<
            multi_index::tag<angle_connectvity>,
            multi_index::composite_key<angle,
                    multi_index::member<angle,char8_t,&angle::atomic_numberA>,
                    multi_index::member<angle,char8_t,&angle::connectivityA>,
                    multi_index::member<angle,BOND_TYPE,&angle::typeA>,
                    multi_index::member<angle,char8_t,&angle::connectivityB>,
                    multi_index::member<angle,char8_t,&angle::atomic_numberB>,
                    multi_index::member<angle,BOND_TYPE,&angle::typeB>,
                    multi_index::member<angle,char8_t,&angle::connectivityC>,
                    multi_index::member<angle,char8_t,&angle::atomic_numberC>
                >
            >,
        multi_index::hashed_non_unique<
            multi_index::tag<wildcard_angle_connectvity>,
            multi_index::composite_key<angle,
                    multi_index::member<angle,BOND_TYPE,&angle::typeA>,
                    multi_index::member<angle,char8_t,&angle::connectivityB>,
                    multi_index::member<angle,char8_t,&angle::atomic_numberB>,
                    multi_index::member<angle,BOND_TYPE,&angle::typeB>
                >
            >
      >
    >;
    using angle_set_by_smirks = multi_index::index<angle_set,smirk>::type;
    
    class SMIRKSPatterns{
    protected:
        bond_set bondSet;
        bond_set_by_smirks& smirksIndex;
        angle_set angleSet;
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
