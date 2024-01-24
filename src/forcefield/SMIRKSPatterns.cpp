#include "forcefield/SMIRKSPatterns.h"

#include "io/xml/Path.h"
#include "io/xml/Binder.h"

#include <algorithm>

namespace sylvanmats::forcefield {

    SMIRKSPatterns::SMIRKSPatterns() {
        
        sylvanmats::io::xml::Binder xmlReaper("/home/roger/Documents/Optimization/openff-2.1.0.offxml", "");
        xmlReaper([&](std::u16string& utf16, std::vector<std::pair<sylvanmats::io::xml::tag_indexer, std::vector<sylvanmats::io::xml::tag_indexer>>>& dag){
            std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
            for(std::vector<std::pair<sylvanmats::io::xml::tag_indexer, std::vector<sylvanmats::io::xml::tag_indexer>>>::iterator it=dag.begin();it!=dag.end();it++){
                if(utf16.substr((*it).first.angle_start, (*it).first.angle_end-(*it).first.angle_start).starts_with(u"<Bonds ")){
                for(auto itS : dag[(*it).first.index].second | std::views::filter([&utf16](sylvanmats::io::xml::tag_indexer& di){ return utf16.substr(di.angle_start, di.angle_end-di.angle_start).starts_with(u"<Bond "); })){
                    std::u16string&& idValue=xmlReaper.findAttribute(u"id", itS.angle_start, itS.angle_end);
                    std::u16string&& smirksValue=xmlReaper.findAttribute(u"smirks", itS.angle_start, itS.angle_end);
                    std::u16string&& lengthValue=xmlReaper.findAttribute(u"length", itS.angle_start, itS.angle_end);
                    std::u16string&& kValue=xmlReaper.findAttribute(u"k", itS.angle_start, itS.angle_end);
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
                    sylvanmats::forcefield::smirks_pattern smirksPattern(smirksBytes, std::strtod(cv.to_bytes(lengthValue).c_str(), nullptr), std::strtod(cv.to_bytes(kValue).c_str(), nullptr));
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
                                std::cout<<" nodes: "<<lemon::countNodes(smirksPattern.smirksGraph)<<" "<<lemon::countEdges(smirksPattern.smirksGraph)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nA].atomic_number)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nA].connectivity)<<" "<<smirksPattern.bondPrimitives[e].type<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nB].connectivity)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nB].atomic_number)<<std::endl;
                                bondSet.insert(bondSet.begin(), smirksPattern);
//                                bondSet.push_back(smirksPattern);
                            }
                        }
                    }
                }
                }
                if(utf16.substr((*it).first.angle_start, (*it).first.angle_end-(*it).first.angle_start).starts_with(u"<Angles ")){
                for(auto itS : dag[(*it).first.index].second | std::views::filter([&utf16](sylvanmats::io::xml::tag_indexer& di){ return utf16.substr(di.angle_start, di.angle_end-di.angle_start).starts_with(u"<Angle "); })){
                    std::u16string&& idValue=xmlReaper.findAttribute(u"id", itS.angle_start, itS.angle_end);
                    std::u16string&& smirksValue=xmlReaper.findAttribute(u"smirks", itS.angle_start, itS.angle_end);
                    std::u16string&& angleValue=xmlReaper.findAttribute(u"angle", itS.angle_start, itS.angle_end);
                    std::u16string&& kValue=xmlReaper.findAttribute(u"k", itS.angle_start, itS.angle_end);
                    std::cout<<"got angle smirks "<<cv.to_bytes(idValue)<<" "<<cv.to_bytes(smirksValue)<<" "<<std::strtod(cv.to_bytes(angleValue).c_str(), nullptr)<<" "<<std::strtod(cv.to_bytes(kValue).c_str(), nullptr)<<std::endl;
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
                    sylvanmats::forcefield::smirks_pattern smirksPattern(smirksBytes, std::numbers::pi*std::strtod(cv.to_bytes(angleValue).c_str(), nullptr)/180.0, std::strtod(cv.to_bytes(kValue).c_str(), nullptr));
                    for(std::vector<antlr4::tree::ParseTree*>::iterator it=pattern.begin();it!=pattern.end();it++){
                        if (sylvanmats::SMIRKSParser::PatternsContext* r=dynamic_cast<sylvanmats::SMIRKSParser::PatternsContext*>((*it))) {
                            lemon::ListGraph::Node nA=processAtoms(r->atoms(), smirksPattern);
                            lemon::ListGraph::Edge eA=lemon::INVALID;
                            lemon::ListGraph::Node nB=lemon::INVALID;
                            lemon::ListGraph::Edge eB=lemon::INVALID;
                            lemon::ListGraph::Node nC=lemon::INVALID;
                            for(size_t baaIndex=0;baaIndex<r->bonds_and_atoms().size();baaIndex++){
//                    std::cout<<"got next atom "<<std::endl;
                                if(baaIndex==0){
                                    nB=processAtoms(r->bonds_and_atoms(baaIndex)->atoms(), smirksPattern);
                                    if(nA!=lemon::INVALID && nB!=lemon::INVALID){
                                        sylvanmats::SMIRKSParser::Bond_primitivesContext* bc=r->bonds_and_atoms(baaIndex)->bond_primitives();
                                        eA=smirksPattern.smirksGraph.addEdge(nA, nB);
                                        processBonds(bc, eA, smirksPattern);
                                    }
                                }
                                else if(baaIndex==1){
                                    nC=processAtoms(r->bonds_and_atoms(baaIndex)->atoms(), smirksPattern);
                                    if(nB!=lemon::INVALID && nC!=lemon::INVALID){
                                        sylvanmats::SMIRKSParser::Bond_primitivesContext* bc=r->bonds_and_atoms(baaIndex)->bond_primitives();
                                        eB=smirksPattern.smirksGraph.addEdge(nB, nC);
                                        processBonds(bc, eB, smirksPattern);
                                    }
                                }
                            }
                            if(nA!=lemon::INVALID && eA!=lemon::INVALID && nB!=lemon::INVALID && eB!=lemon::INVALID && nC!=lemon::INVALID){
                                std::cout<<"a nodes: "<<lemon::countNodes(smirksPattern.smirksGraph)<<" "<<lemon::countEdges(smirksPattern.smirksGraph)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nA].atomic_number)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nA].connectivity)<<" "<<smirksPattern.bondPrimitives[eA].type<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nB].connectivity)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nB].atomic_number)<<" "<<" rs: "<<static_cast<int>(smirksPattern.atomicPrimitives[nA].ring_size)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nB].ring_size)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nC].ring_size)<<std::endl;
                                angleSet.insert(angleSet.begin(), smirksPattern);
//                                angleSet.push_back(smirksPattern);
                            }
                        }
                    }
                }
                }
            }

        });
    }
    
    lemon::ListGraph::Node SMIRKSPatterns::processAtoms(sylvanmats::SMIRKSParser::AtomsContext* atoms, sylvanmats::forcefield::smirks_pattern& smirksPattern){
        lemon::ListGraph::Node nPrev=lemon::INVALID;
        size_t mapClass=0;
        if(atoms->map_class()!=nullptr && !atoms->map_class()->getText().empty()){
            mapClass=std::strtoul(atoms->map_class()->getText().c_str(), nullptr, 10);
        }
        if(!atoms->atomic_primitives().empty()){
            for(size_t index=0;index<std::min(1ul, atoms->atomic_primitives().size());index++){
                lemon::ListGraph::Node n=smirksPattern.smirksGraph.addNode();
                if(atoms->atomic_primitives(index)->atomic_number()!=nullptr)smirksPattern.atomicPrimitives[n].atomic_number=std::strtoul(atoms->atomic_primitives(index)->atomic_number()->getText().c_str(), nullptr, 10);
                if(atoms->atomic_primitives(index)->Star()!=nullptr)smirksPattern.atomicPrimitives[n].wildcard=true;
                if(!atoms->atomic_primitives(index)->connectivity().empty())smirksPattern.atomicPrimitives[n].connectivity=std::strtoul(atoms->atomic_primitives(index)->connectivity(0)->getText().c_str(), nullptr, 10);
                else if(smirksPattern.atomicPrimitives[n].atomic_number==1)smirksPattern.atomicPrimitives[n].connectivity=1;
                if(!atoms->atomic_primitives(index)->ring_connectivity().empty())smirksPattern.atomicPrimitives[n].ring_connectivity=std::strtoul(atoms->atomic_primitives(index)->ring_connectivity(0)->getText().c_str(), nullptr, 10);
                if(!atoms->atomic_primitives(index)->ring_size().empty())smirksPattern.atomicPrimitives[n].ring_size=std::strtoul(atoms->atomic_primitives(index)->ring_size(0)->getText().c_str(), nullptr, 10);
                if(!atoms->atomic_primitives(index)->proton_count().empty())smirksPattern.atomicPrimitives[n].total_H_count=std::strtoul(atoms->atomic_primitives(index)->proton_count(0)->getText().c_str(), nullptr, 10);
                if(!atoms->atomic_primitives(index)->formal_charge().empty())smirksPattern.atomicPrimitives[n].formal_charge=std::strtoul(atoms->atomic_primitives(index)->formal_charge(0)->getText().c_str(), nullptr, 10);
                smirksPattern.atomicPrimitives[n].map_class=mapClass;
                nPrev=n;
            }
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
    }
    
    void SMIRKSPatterns::processBonds(sylvanmats::SMIRKSParser::Bond_primitivesContext* bc, lemon::ListGraph::Edge& e, sylvanmats::forcefield::smirks_pattern& smirksPattern){
        if(!bc->Minus().empty()){
            smirksPattern.bondPrimitives[e].type=BOND_SINGLE;
        }
        else if(!bc->Double().empty()){
            smirksPattern.bondPrimitives[e].type=BOND_DOUBLE;
        }
        else if(!bc->Pound().empty()){
            smirksPattern.bondPrimitives[e].type=BOND_TRIPLE;
        }
        else if(!bc->Colon().empty()){
            smirksPattern.bondPrimitives[e].type=BOND_AROMATIC;
        }
        else if(!bc->Any().empty()){
            smirksPattern.bondPrimitives[e].type=BOND_ANY;
        }
        else if(!bc->At().empty()){
            smirksPattern.bondPrimitives[e].type=BOND_ANY_RING;
            smirksPattern.bondPrimitives[e].any_ring=true;
        }
        if(!bc->Not().empty()){
            smirksPattern.bondPrimitives[e].exclude=true;
        }
        
    }
    
    void SMIRKSPatterns::operator()(char8_t atomic_numberA, char8_t connectivityA, BOND_TYPE type, char8_t connectivityB, char8_t atomic_numberB, std::function<bool(double length, double k, smirks_pattern& smirksPattern)> apply){
        bool ret=false;
        for (std::vector<smirks_pattern>::iterator it=bondSet.begin();!ret && it != bondSet.end();it++) {
            lemon::ListGraph::EdgeIt eSiteA((*it).smirksGraph);
            lemon::ListGraph::Node nSiteA=(*it).smirksGraph.u(eSiteA);
            lemon::ListGraph::Node nSiteB=(*it).smirksGraph.v(eSiteA);
            if (eSiteA!=lemon::INVALID && ((*it).bondPrimitives[eSiteA].type==BOND_ANY || (*it).bondPrimitives[eSiteA].type==type) && ((((*it).atomicPrimitives[nSiteA].wildcard || (*it).atomicPrimitives[nSiteA].atomic_number==atomic_numberA) && ((*it).atomicPrimitives[nSiteB].wildcard || (*it).atomicPrimitives[nSiteB].atomic_number==atomic_numberB)) || (((*it).atomicPrimitives[nSiteA].wildcard || (*it).atomicPrimitives[nSiteA].atomic_number==atomic_numberB) && ((*it).atomicPrimitives[nSiteB].wildcard || (*it).atomicPrimitives[nSiteB].atomic_number==atomic_numberA))))
                ret=apply((*it).length, (*it).k, (*it));
        }
    }
    
    void SMIRKSPatterns::operator()(char8_t atomic_numberA, char8_t connectivityA, BOND_TYPE typeA, char8_t connectivityB, char8_t atomic_numberB, BOND_TYPE typeB, char8_t connectivityC, char8_t atomic_numberC, std::function<bool(double angle, double k, smirks_pattern& smirksPattern)> apply){
        bool ret=false;
        for (std::vector<smirks_pattern>::iterator it=angleSet.begin();!ret && it != angleSet.end();it++) {
            lemon::ListGraph::EdgeIt eSiteA((*it).smirksGraph);
            lemon::ListGraph::EdgeIt eSiteB=eSiteA;
            ++eSiteB;
            if (eSiteA!=lemon::INVALID && eSiteB!=lemon::INVALID && (((*it).bondPrimitives[eSiteA].type==BOND_ANY || (*it).bondPrimitives[eSiteA].type==typeA) && ((*it).bondPrimitives[eSiteB].type==BOND_ANY || (*it).bondPrimitives[eSiteB].type==typeB)) || (((*it).bondPrimitives[eSiteA].type==BOND_ANY || (*it).bondPrimitives[eSiteA].type==typeB) && ((*it).bondPrimitives[eSiteB].type==BOND_ANY || (*it).bondPrimitives[eSiteB].type==typeA)))
             ret=apply((*it).length, (*it).k, (*it));
        }
   }
    
}
