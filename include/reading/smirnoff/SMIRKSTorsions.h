#pragma once

#include "io/xml/Path.h"
#include "io/xml/Binder.h"

namespace sylvanmats::reading{
    class SMIRKSTorsions{
    public:
        SMIRKSTorsions() = default;
        SMIRKSTorsions(const SMIRKSTorsions& orig) = default;
        virtual ~SMIRKSTorsions() = default;
        
        void operator()(sylvanmats::io::xml::Binder& xmlReaper, std::u16string& utf16, std::vector<std::pair<sylvanmats::io::xml::tag_indexer, std::vector<sylvanmats::io::xml::tag_indexer>>>& dag, std::vector<std::pair<sylvanmats::io::xml::tag_indexer, std::vector<sylvanmats::io::xml::tag_indexer>>>::iterator it, std::u16string tag, std::vector<smirks_torsions_pattern>& properSet){
            std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
            for(auto itS : dag[(*it).first.index].second | std::views::filter([&utf16, &tag](sylvanmats::io::xml::tag_indexer& di){ return utf16.substr(di.angle_start, di.angle_end-di.angle_start).starts_with(tag); })){
                std::u16string&& idValue=xmlReaper.findAttribute(u"id", itS.angle_start, itS.angle_end);
                std::u16string&& smirksValue=xmlReaper.findAttribute(u"smirks", itS.angle_start, itS.angle_end);
                std::u16string&& periodicity1Value=xmlReaper.findAttribute(u"periodicity1", itS.angle_start, itS.angle_end);
                std::u16string&& periodicity2Value=xmlReaper.findAttribute(u"periodicity2", itS.angle_start, itS.angle_end);
                std::u16string&& periodicity3Value=xmlReaper.findAttribute(u"periodicity3", itS.angle_start, itS.angle_end);
                std::u16string&& periodicity4Value=xmlReaper.findAttribute(u"periodicity4", itS.angle_start, itS.angle_end);
                std::u16string&& k1Value=xmlReaper.findAttribute(u"k1", itS.angle_start, itS.angle_end);
                std::u16string&& k2Value=xmlReaper.findAttribute(u"k2", itS.angle_start, itS.angle_end);
                std::u16string&& k3Value=xmlReaper.findAttribute(u"k3", itS.angle_start, itS.angle_end);
                std::u16string&& k4Value=xmlReaper.findAttribute(u"k4", itS.angle_start, itS.angle_end);
                std::u16string&& phase1Value=xmlReaper.findAttribute(u"phase1", itS.angle_start, itS.angle_end);
                std::u16string&& phase2Value=xmlReaper.findAttribute(u"phase2", itS.angle_start, itS.angle_end);
                std::u16string&& phase3Value=xmlReaper.findAttribute(u"phase3", itS.angle_start, itS.angle_end);
                std::u16string&& phase4Value=xmlReaper.findAttribute(u"phase4", itS.angle_start, itS.angle_end);
                std::u16string&& idivf1Value=xmlReaper.findAttribute(u"idivf1", itS.angle_start, itS.angle_end);
                std::u16string&& idivf2Value=xmlReaper.findAttribute(u"idivf2", itS.angle_start, itS.angle_end);
                std::u16string&& idivf3Value=xmlReaper.findAttribute(u"idivf3", itS.angle_start, itS.angle_end);
                std::u16string&& idivf4Value=xmlReaper.findAttribute(u"idivf4", itS.angle_start, itS.angle_end);
//                    std::cout<<"got smirks "<<cv.to_bytes(idValue)<<" "<<cv.to_bytes(smirksValue)<<" "<<std::strtod(cv.to_bytes(lengthValue).c_str(), nullptr)<<" "<<std::strtod(cv.to_bytes(kValue).c_str(), nullptr)<<std::endl;
                antlr4::ANTLRInputStream input(cv.to_bytes(smirksValue));
                sylvanmats::SMIRKSLexer lexer(&input);
                antlr4::CommonTokenStream tokens(&lexer);

                sylvanmats::SMIRKSParser parser(&tokens);
                parser.setBuildParseTree(true);
                antlr4::tree::ParseTree* tree = parser.patterns();
                const std::string patternsPath="/patterns";
                antlr4::tree::xpath::XPath xpatternsPath(&parser,patternsPath);
                std::vector<antlr4::tree::ParseTree*> pattern=xpatternsPath.evaluate(tree);
                std::string smirksBytes=cv.to_bytes(smirksValue);
                sylvanmats::reading::smirks_torsions_pattern smirksPattern(smirksBytes, std::strtod(cv.to_bytes(phase1Value).c_str(), nullptr), std::strtod(cv.to_bytes(k1Value).c_str(), nullptr));
                for(std::vector<antlr4::tree::ParseTree*>::iterator it=pattern.begin();it!=pattern.end();it++){
                    if (sylvanmats::SMIRKSParser::PatternsContext* r=dynamic_cast<sylvanmats::SMIRKSParser::PatternsContext*>((*it))) {
                        lemon::ListGraph::Node nA=processAtoms(r->atoms(), smirksPattern);
                        lemon::ListGraph::Edge e=lemon::INVALID;
                        lemon::ListGraph::Node nB=lemon::INVALID;
                        for(size_t baaIndex=0;baaIndex<r->bonds_and_atoms().size();baaIndex++){
//                            for(std::vector<sylvanmats::SMIRKSParser::Bonds_and_atomsContext *>::iterator itB=r->bonds_and_atoms().begin();itB!=r->bonds_and_atoms().end();++itB){
//                                sylvanmats::SMIRKSParser::AtomsContext* ac=(*itB)->atoms();
//                    std::cout<<"got next atom "<<std::endl;
                            nB=processAtoms(r->bonds_and_atoms(baaIndex)->atoms(), smirksPattern);
                            if(nA!=lemon::INVALID && nB!=lemon::INVALID){
                                sylvanmats::SMIRKSParser::Bond_primitivesContext* bc=r->bonds_and_atoms(baaIndex)->bond_primitives();
                                e=smirksPattern.smirksGraph.addEdge(nA, nB);
                                processBonds(bc, e, smirksPattern);
                            }
                        }
                        if(nA!=lemon::INVALID && e!=lemon::INVALID && nB!=lemon::INVALID){
                            std::cout<<"p nodes: "<<lemon::countNodes(smirksPattern.smirksGraph)<<" "<<lemon::countEdges(smirksPattern.smirksGraph)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nA].front().atomic_number)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nA].front().connectivity)<<" "<<smirksPattern.bondPrimitives[e].type<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nB].front().connectivity)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nB].front().atomic_number)<<std::endl;
                            properSet.insert(properSet.begin(), smirksPattern);
//                                properSet.push_back(smirksPattern);
                        }
                    }
                }
            }
        }
    lemon::ListGraph::Node processAtoms(sylvanmats::SMIRKSParser::AtomsContext* atoms, sylvanmats::reading::smirks_pattern& smirksPattern){
        lemon::ListGraph::Node nPrev=lemon::INVALID;
        size_t mapClass=0;
        if(atoms->map_class()!=nullptr && !atoms->map_class()->getText().empty()){
            mapClass=std::strtoul(atoms->map_class()->getText().c_str(), nullptr, 10);
        }
        if(!atoms->atomic_primitives().empty()){
            lemon::ListGraph::Node n=smirksPattern.smirksGraph.addNode();
            for(size_t index=0;index<atoms->atomic_primitives().size();index++){
                smirksPattern.atomicPrimitives[n].push_back(atom_primitive());
                if(atoms->atomic_primitives(index)->atomic_number()!=nullptr)smirksPattern.atomicPrimitives[n].back().atomic_number=std::strtoul(atoms->atomic_primitives(index)->atomic_number()->getText().c_str(), nullptr, 10);
                if(atoms->atomic_primitives(index)->Star()!=nullptr)smirksPattern.atomicPrimitives[n].back().wildcard=true;
                if(!atoms->atomic_primitives(index)->connectivity().empty())smirksPattern.atomicPrimitives[n].back().connectivity=std::strtoul(atoms->atomic_primitives(index)->connectivity(0)->getText().c_str(), nullptr, 10);
                else if(smirksPattern.atomicPrimitives[n].back().atomic_number==1)smirksPattern.atomicPrimitives[n].back().connectivity=1;
                if(!atoms->atomic_primitives(index)->ring_connectivity().empty())smirksPattern.atomicPrimitives[n].back().ring_connectivity=std::strtoul(atoms->atomic_primitives(index)->ring_connectivity(0)->getText().c_str(), nullptr, 10);
                if(!atoms->atomic_primitives(index)->ring_size().empty())smirksPattern.atomicPrimitives[n].back().ring_size=std::strtoul(atoms->atomic_primitives(index)->ring_size(0)->getText().c_str(), nullptr, 10);
                if(!atoms->atomic_primitives(index)->proton_count().empty())smirksPattern.atomicPrimitives[n].back().total_H_count=std::strtoul(atoms->atomic_primitives(index)->proton_count(0)->getText().c_str(), nullptr, 10);
                if(!atoms->atomic_primitives(index)->formal_charge().empty())smirksPattern.atomicPrimitives[n].back().formal_charge=std::strtoul(atoms->atomic_primitives(index)->formal_charge(0)->getText().c_str(), nullptr, 10);
                smirksPattern.atomicPrimitives[n].back().map_class=mapClass;
            }
                nPrev=n;
             if(!atoms->branch().empty() && nPrev!=lemon::INVALID){
                 for(size_t branchIndex=0;branchIndex<std::min(1ul, atoms->branch().size());branchIndex++){
                    lemon::ListGraph::Node nB=processAtoms(atoms->branch(branchIndex)->atoms(), smirksPattern);
                    if(nB!=lemon::INVALID){
                        lemon::ListGraph::Edge e=smirksPattern.smirksGraph.addEdge(nPrev, nB);
                        processBonds(atoms->branch(branchIndex)->bond_primitives(), e, smirksPattern);
                    }
                 }
             }
        }
        return nPrev;
    };
    
    void processBonds(sylvanmats::SMIRKSParser::Bond_primitivesContext* bc, lemon::ListGraph::Edge& e, sylvanmats::reading::smirks_pattern& smirksPattern){
        if(!bc->Minus().empty()){
            smirksPattern.bondPrimitives[e].type=sylvanmats::forcefield::BOND_SINGLE;
        }
        else if(!bc->Double().empty()){
            smirksPattern.bondPrimitives[e].type=sylvanmats::forcefield::BOND_DOUBLE;
        }
        else if(!bc->Pound().empty()){
            smirksPattern.bondPrimitives[e].type=sylvanmats::forcefield::BOND_TRIPLE;
        }
        else if(!bc->Colon().empty()){
            smirksPattern.bondPrimitives[e].type=sylvanmats::forcefield::BOND_AROMATIC;
        }
        else if(!bc->Any().empty()){
            smirksPattern.bondPrimitives[e].type=sylvanmats::forcefield::BOND_ANY;
        }
        else if(!bc->At().empty()){
            smirksPattern.bondPrimitives[e].type=sylvanmats::forcefield::BOND_ANY_RING;
            smirksPattern.bondPrimitives[e].any_ring=true;
        }
        if(!bc->Not().empty()){
            smirksPattern.bondPrimitives[e].exclude=true;
        }
        
    };
    
    };
}