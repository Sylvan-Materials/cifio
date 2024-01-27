#include "reading/smirnoff/SMIRKSPatterns.h"

#include "io/xml/Path.h"
#include "io/xml/Binder.h"

#include <algorithm>

namespace sylvanmats::reading {

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
                    sylvanmats::reading::smirks_pattern smirksPattern(smirksBytes, std::strtod(cv.to_bytes(lengthValue).c_str(), nullptr), std::strtod(cv.to_bytes(kValue).c_str(), nullptr));
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
                                std::cout<<" nodes: "<<lemon::countNodes(smirksPattern.smirksGraph)<<" "<<lemon::countEdges(smirksPattern.smirksGraph)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nA].front().atomic_number)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nA].front().connectivity)<<" "<<smirksPattern.bondPrimitives[e].type<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nB].front().connectivity)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nB].front().atomic_number)<<std::endl;
//                                bondSet.insert(bondSet.begin(), smirksPattern);
                                bondSet.push_back(smirksPattern);
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
                    sylvanmats::reading::smirks_pattern smirksPattern(smirksBytes, std::numbers::pi*std::strtod(cv.to_bytes(angleValue).c_str(), nullptr)/180.0, std::strtod(cv.to_bytes(kValue).c_str(), nullptr));
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
                                std::cout<<"a nodes: "<<lemon::countNodes(smirksPattern.smirksGraph)<<" "<<lemon::countEdges(smirksPattern.smirksGraph)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nA].front().atomic_number)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nA].front().connectivity)<<" "<<smirksPattern.bondPrimitives[eA].type<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nB].front().connectivity)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nB].front().atomic_number)<<" "<<" rs: "<<static_cast<int>(smirksPattern.atomicPrimitives[nA].front().ring_size)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nB].front().ring_size)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nC].front().ring_size)<<std::endl;
//                                angleSet.insert(angleSet.begin(), smirksPattern);
                                angleSet.push_back(smirksPattern);
                            }
                        }
                    }
                }
                }
                if(utf16.substr((*it).first.angle_start, (*it).first.angle_end-(*it).first.angle_start).starts_with(u"<vdW ")){
                for(auto itS : dag[(*it).first.index].second | std::views::filter([&utf16](sylvanmats::io::xml::tag_indexer& di){ return utf16.substr(di.angle_start, di.angle_end-di.angle_start).starts_with(u"<Atom "); })){
                    std::u16string&& idValue=xmlReaper.findAttribute(u"id", itS.angle_start, itS.angle_end);
                    std::u16string&& smirksValue=xmlReaper.findAttribute(u"smirks", itS.angle_start, itS.angle_end);
                    std::u16string&& epsilonValue=xmlReaper.findAttribute(u"epsilon", itS.angle_start, itS.angle_end);
                    std::u16string&& rmin_halfValue=xmlReaper.findAttribute(u"rmin_half", itS.angle_start, itS.angle_end);
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
                    sylvanmats::reading::smirks_vdw_pattern smirksPattern(smirksBytes, std::strtod(cv.to_bytes(epsilonValue).c_str(), nullptr), std::strtod(cv.to_bytes(rmin_halfValue).c_str(), nullptr));
                    for(std::vector<antlr4::tree::ParseTree*>::iterator it=pattern.begin();it!=pattern.end();it++){
                        if (sylvanmats::SMIRKSParser::PatternsContext* r=dynamic_cast<sylvanmats::SMIRKSParser::PatternsContext*>((*it))) {
                            lemon::ListGraph::Node nA=processAtoms(r->atoms(), smirksPattern);
                            if(nA!=lemon::INVALID){
//                                std::cout<<" nodes: "<<lemon::countNodes(smirksPattern.smirksGraph)<<" "<<lemon::countEdges(smirksPattern.smirksGraph)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nA].front().atomic_number)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nA].front().connectivity)<<" "<<smirksPattern.bondPrimitives[e].type<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nB].front().connectivity)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nB].front().atomic_number)<<std::endl;
//                                bondSet.insert(bondSet.begin(), smirksPattern);
                                vdwSet.push_back(smirksPattern);
                            }
                        }
                    }
                }
                }
                if(utf16.substr((*it).first.angle_start, (*it).first.angle_end-(*it).first.angle_start).starts_with(u"<ProperTorsions ")){
                    for(auto itS : dag[(*it).first.index].second | std::views::filter([&utf16](sylvanmats::io::xml::tag_indexer& di){ return utf16.substr(di.angle_start, di.angle_end-di.angle_start).starts_with(u"<Proper "); })){
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
                            std::cout<<"got smirks "<<cv.to_bytes(idValue)<<" "<<cv.to_bytes(smirksValue)<<" "<<std::strtod(cv.to_bytes(phase1Value).c_str(), nullptr)<<" "<<std::strtod(cv.to_bytes(k1Value).c_str(), nullptr)<<std::endl;
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
                        sylvanmats::reading::smirks_torsions_pattern smirksPattern(smirksBytes);
                        if(!periodicity1Value.empty())smirksPattern.periodicity1Value=std::strtod(cv.to_bytes(k1Value).c_str(), nullptr);
                        if(!periodicity2Value.empty())smirksPattern.periodicity2Value=std::strtod(cv.to_bytes(k2Value).c_str(), nullptr);
                        if(!periodicity3Value.empty())smirksPattern.periodicity3Value=std::strtod(cv.to_bytes(k3Value).c_str(), nullptr);
                        if(!periodicity4Value.empty())smirksPattern.periodicity4Value=std::strtod(cv.to_bytes(k4Value).c_str(), nullptr);
                        if(!k1Value.empty())smirksPattern.k1Value=std::strtod(cv.to_bytes(k1Value).c_str(), nullptr);
                        if(!k2Value.empty())smirksPattern.k2Value=std::strtod(cv.to_bytes(k2Value).c_str(), nullptr);
                        if(!k3Value.empty())smirksPattern.k3Value=std::strtod(cv.to_bytes(k3Value).c_str(), nullptr);
                        if(!k4Value.empty())smirksPattern.k4Value=std::strtod(cv.to_bytes(k4Value).c_str(), nullptr);
                        if(!phase1Value.empty())smirksPattern.phase1Value=std::strtod(cv.to_bytes(phase1Value).c_str(), nullptr);
                        if(!phase2Value.empty())smirksPattern.phase2Value=std::strtod(cv.to_bytes(phase2Value).c_str(), nullptr);
                        if(!phase3Value.empty())smirksPattern.phase3Value=std::strtod(cv.to_bytes(phase3Value).c_str(), nullptr);
                        if(!phase4Value.empty())smirksPattern.phase4Value=std::strtod(cv.to_bytes(phase4Value).c_str(), nullptr);
                        if(!idivf1Value.empty())smirksPattern.idivf1Value=std::strtod(cv.to_bytes(idivf1Value).c_str(), nullptr);
                        if(!idivf2Value.empty())smirksPattern.idivf2Value=std::strtod(cv.to_bytes(idivf2Value).c_str(), nullptr);
                        if(!idivf3Value.empty())smirksPattern.idivf3Value=std::strtod(cv.to_bytes(idivf3Value).c_str(), nullptr);
                        if(!idivf4Value.empty())smirksPattern.idivf4Value=std::strtod(cv.to_bytes(idivf4Value).c_str(), nullptr);
                        for(std::vector<antlr4::tree::ParseTree*>::iterator it=pattern.begin();it!=pattern.end();it++){
                            if (sylvanmats::SMIRKSParser::PatternsContext* r=dynamic_cast<sylvanmats::SMIRKSParser::PatternsContext*>((*it))) {
                                lemon::ListGraph::Node nA=processAtoms(r->atoms(), smirksPattern);
                                lemon::ListGraph::Edge eA=lemon::INVALID;
                                lemon::ListGraph::Node nB=lemon::INVALID;
                                lemon::ListGraph::Edge eB=lemon::INVALID;
                                lemon::ListGraph::Node nC=lemon::INVALID;
                                lemon::ListGraph::Edge eC=lemon::INVALID;
                                lemon::ListGraph::Node nD=lemon::INVALID;
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
                                    else if(baaIndex==2){
                                        nD=processAtoms(r->bonds_and_atoms(baaIndex)->atoms(), smirksPattern);
                                        if(nC!=lemon::INVALID && nD!=lemon::INVALID){
                                            sylvanmats::SMIRKSParser::Bond_primitivesContext* bc=r->bonds_and_atoms(baaIndex)->bond_primitives();
                                            eC=smirksPattern.smirksGraph.addEdge(nC, nD);
                                            processBonds(bc, eC, smirksPattern);
                                        }
                                    }
                                }
                                if(nA!=lemon::INVALID && eA!=lemon::INVALID && nB!=lemon::INVALID && eB!=lemon::INVALID && nC!=lemon::INVALID && eC!=lemon::INVALID && nD!=lemon::INVALID){
                                    std::cout<<"p nodes: "<<lemon::countNodes(smirksPattern.smirksGraph)<<" "<<lemon::countEdges(smirksPattern.smirksGraph)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nA].front().atomic_number)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nA].front().connectivity)<<" "<<smirksPattern.bondPrimitives[eA].type<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nB].front().connectivity)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nB].front().atomic_number)<<" "<<" rs: "<<static_cast<int>(smirksPattern.atomicPrimitives[nA].front().ring_size)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nB].front().ring_size)<<" "<<static_cast<int>(smirksPattern.atomicPrimitives[nC].front().ring_size)<<std::endl;
            //                            properSet.insert(properSet.begin(), smirksPattern);
                                        properSet.push_back(smirksPattern);
                            }
                            }
                        }
                    }
                }
                if(utf16.substr((*it).first.angle_start, (*it).first.angle_end-(*it).first.angle_start).starts_with(u"<ImproperTorsions ")){
//                    torsions(xmlReaper, utf16, dag, it, u"<Improper ", improperSet);
                }
            }

        });
    }
    
    lemon::ListGraph::Node SMIRKSPatterns::processAtoms(sylvanmats::SMIRKSParser::AtomsContext* atoms, sylvanmats::reading::smirks_pattern& smirksPattern){
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
    
    void SMIRKSPatterns::processBonds(sylvanmats::SMIRKSParser::Bond_primitivesContext* bc, lemon::ListGraph::Edge& e, sylvanmats::reading::smirks_pattern& smirksPattern){
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
    
    void SMIRKSPatterns::operator()(char8_t atomic_numberA, char8_t connectivityA, sylvanmats::forcefield::BOND_TYPE type, char8_t connectivityB, char8_t atomic_numberB, std::function<bool(double length, double k, smirks_pattern& smirksPattern)> apply){
        bool ret=false;
        for (std::vector<smirks_pattern>::iterator it=bondSet.begin();!ret && it != bondSet.end();it++) {
            lemon::ListGraph::EdgeIt eSiteA((*it).smirksGraph);
            lemon::ListGraph::Node nSiteA=(*it).smirksGraph.u(eSiteA);
            lemon::ListGraph::Node nSiteB=(*it).smirksGraph.v(eSiteA);
            if (eSiteA!=lemon::INVALID && ((*it).bondPrimitives[eSiteA].type==sylvanmats::forcefield::BOND_ANY || (*it).bondPrimitives[eSiteA].type==type)){
                bool anAN=false;
                bool anBN=false;
                for(std::vector<atom_primitive>::iterator itAN=(*it).atomicPrimitives[nSiteA].begin();(!anAN || !anBN) && itAN!=(*it).atomicPrimitives[nSiteA].end();itAN++){
                    if((*itAN).wildcard || (*itAN).atomic_number==atomic_numberA)anAN=true;
                    if((*itAN).wildcard || (*itAN).atomic_number==atomic_numberB)anBN=true;
                }
                bool bnAN=false;
                bool bnBN=false;
                for(std::vector<atom_primitive>::iterator itBN=(*it).atomicPrimitives[nSiteB].begin();(!bnAN || !bnBN) && itBN!=(*it).atomicPrimitives[nSiteB].end();itBN++){
                    if((*itBN).wildcard || (*itBN).atomic_number==atomic_numberA)bnAN=true;
                    if((*itBN).wildcard || (*itBN).atomic_number==atomic_numberB)bnBN=true;
                }
                if(((anAN) && (bnBN)) || ((anBN) && (bnAN)))
                ret=apply((*it).length, (*it).k, (*it));
            }
        }
    }
    
    void SMIRKSPatterns::operator()(char8_t atomic_numberA, char8_t connectivityA, sylvanmats::forcefield::BOND_TYPE typeA, char8_t connectivityB, char8_t atomic_numberB, sylvanmats::forcefield::BOND_TYPE typeB, char8_t connectivityC, char8_t atomic_numberC, std::function<bool(double angle, double k, smirks_pattern& smirksPattern)> apply){
        bool ret=false;
        for (std::vector<smirks_pattern>::iterator it=angleSet.begin();!ret && it != angleSet.end();it++) {
            lemon::ListGraph::EdgeIt eSiteA((*it).smirksGraph);
            lemon::ListGraph::EdgeIt eSiteB=eSiteA;
            ++eSiteB;
            if (eSiteA!=lemon::INVALID && eSiteB!=lemon::INVALID && (((*it).bondPrimitives[eSiteA].type==sylvanmats::forcefield::BOND_ANY || (*it).bondPrimitives[eSiteA].type==typeA) && ((*it).bondPrimitives[eSiteB].type==sylvanmats::forcefield::BOND_ANY || (*it).bondPrimitives[eSiteB].type==typeB)) || (((*it).bondPrimitives[eSiteA].type==sylvanmats::forcefield::BOND_ANY || (*it).bondPrimitives[eSiteA].type==typeB) && ((*it).bondPrimitives[eSiteB].type==sylvanmats::forcefield::BOND_ANY || (*it).bondPrimitives[eSiteB].type==typeA))){
                lemon::ListGraph::Node nSiteA=lemon::INVALID;
                lemon::ListGraph::Node nSiteB=lemon::INVALID;
                lemon::ListGraph::Node nSiteC=lemon::INVALID;
                for (lemon::ListGraph::NodeIt n((*it).smirksGraph); n!=lemon::INVALID; ++n){
                    if(nSiteA==lemon::INVALID && (*it).atomicPrimitives[n].front().map_class==1)nSiteA=n;
                    if(nSiteB==lemon::INVALID && (*it).atomicPrimitives[n].front().map_class==2)nSiteB=n;
                    if(nSiteC==lemon::INVALID && (*it).atomicPrimitives[n].front().map_class==3)nSiteC=n;
                }
                if(((*it).atomicPrimitives[nSiteB].front().wildcard || (*it).atomicPrimitives[nSiteB].front().atomic_number==atomic_numberB)  && ((((*it).atomicPrimitives[nSiteA].front().wildcard || (*it).atomicPrimitives[nSiteA].front().atomic_number==atomic_numberA) && ((*it).atomicPrimitives[nSiteC].front().wildcard || (*it).atomicPrimitives[nSiteC].front().atomic_number==atomic_numberC)) || (((*it).atomicPrimitives[nSiteA].front().wildcard || (*it).atomicPrimitives[nSiteA].front().atomic_number==atomic_numberC) && ((*it).atomicPrimitives[nSiteC].front().wildcard || (*it).atomicPrimitives[nSiteC].front().atomic_number==atomic_numberA))))
                    ret=apply((*it).length, (*it).k, (*it));
            }
        }
    }
    
    void SMIRKSPatterns::operator()(char8_t atomic_numberA, char8_t connectivityA, sylvanmats::forcefield::BOND_TYPE typeA, char8_t connectivityB, char8_t atomic_numberB, sylvanmats::forcefield::BOND_TYPE typeB, char8_t connectivityC, char8_t atomic_numberC, sylvanmats::forcefield::BOND_TYPE typeC, char8_t connectivityD, char8_t atomic_numberD, std::function<bool(smirks_torsions_pattern& smirksPattern)> apply){
        bool ret=false;
        for (std::vector<smirks_torsions_pattern>::iterator it=properSet.begin();!ret && it != properSet.end();it++) {
            lemon::ListGraph::EdgeIt eSiteA((*it).smirksGraph);
            lemon::ListGraph::EdgeIt eSiteB=eSiteA;
            ++eSiteB;
            lemon::ListGraph::EdgeIt eSiteC=eSiteA;
            ++eSiteC;
            if (eSiteA!=lemon::INVALID && eSiteB!=lemon::INVALID && eSiteC!=lemon::INVALID && (((*it).bondPrimitives[eSiteA].type==sylvanmats::forcefield::BOND_ANY || (*it).bondPrimitives[eSiteA].type==typeA) && ((*it).bondPrimitives[eSiteB].type==sylvanmats::forcefield::BOND_ANY || (*it).bondPrimitives[eSiteB].type==typeB) && ((*it).bondPrimitives[eSiteC].type==sylvanmats::forcefield::BOND_ANY || (*it).bondPrimitives[eSiteC].type==typeC)) || (((*it).bondPrimitives[eSiteA].type==sylvanmats::forcefield::BOND_ANY || (*it).bondPrimitives[eSiteA].type==typeC) && ((*it).bondPrimitives[eSiteB].type==sylvanmats::forcefield::BOND_ANY || (*it).bondPrimitives[eSiteB].type==typeB)) && (((*it).bondPrimitives[eSiteC].type==sylvanmats::forcefield::BOND_ANY || (*it).bondPrimitives[eSiteC].type==typeA))){
                lemon::ListGraph::Node nSiteA=lemon::INVALID;
                lemon::ListGraph::Node nSiteB=lemon::INVALID;
                lemon::ListGraph::Node nSiteC=lemon::INVALID;
                lemon::ListGraph::Node nSiteD=lemon::INVALID;
                for (lemon::ListGraph::NodeIt n((*it).smirksGraph); n!=lemon::INVALID; ++n){
                    if(nSiteA==lemon::INVALID && (*it).atomicPrimitives[n].front().map_class==1)nSiteA=n;
                    if(nSiteB==lemon::INVALID && (*it).atomicPrimitives[n].front().map_class==2)nSiteB=n;
                    if(nSiteC==lemon::INVALID && (*it).atomicPrimitives[n].front().map_class==3)nSiteC=n;
                    if(nSiteD==lemon::INVALID && (*it).atomicPrimitives[n].front().map_class==4)nSiteD=n;
                }
                if(nSiteA!=lemon::INVALID && nSiteB!=lemon::INVALID && nSiteC!=lemon::INVALID && nSiteD!=lemon::INVALID)
                if(((((*it).atomicPrimitives[nSiteA].front().wildcard || (*it).atomicPrimitives[nSiteA].front().atomic_number==atomic_numberA) && ((*it).atomicPrimitives[nSiteB].front().wildcard || (*it).atomicPrimitives[nSiteB].front().atomic_number==atomic_numberB) && ((*it).atomicPrimitives[nSiteC].front().wildcard || (*it).atomicPrimitives[nSiteC].front().atomic_number==atomic_numberC) && ((*it).atomicPrimitives[nSiteD].front().wildcard || (*it).atomicPrimitives[nSiteD].front().atomic_number==atomic_numberD)) || (((*it).atomicPrimitives[nSiteA].front().wildcard || (*it).atomicPrimitives[nSiteA].front().atomic_number==atomic_numberD) && ((*it).atomicPrimitives[nSiteB].front().wildcard || (*it).atomicPrimitives[nSiteB].front().atomic_number==atomic_numberC)) && ((*it).atomicPrimitives[nSiteC].front().wildcard || (*it).atomicPrimitives[nSiteC].front().atomic_number==atomic_numberB) && ((*it).atomicPrimitives[nSiteD].front().wildcard || (*it).atomicPrimitives[nSiteD].front().atomic_number==atomic_numberA)))
                    ret=apply((*it));
            }
        }
        
    }
    
}
