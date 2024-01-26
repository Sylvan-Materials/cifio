#include "reading/smirnoff/SMIRKSPatterns.h"

#include "reading/smirnoff/SMIRKSTorsions.h"

#include "io/xml/Path.h"
#include "io/xml/Binder.h"

#include <algorithm>

namespace sylvanmats::reading {

    SMIRKSPatterns::SMIRKSPatterns() {
        
        sylvanmats::io::xml::Binder xmlReaper("/home/roger/Documents/Optimization/openff-2.1.0.offxml", "");
        xmlReaper([&](std::u16string& utf16, std::vector<std::pair<sylvanmats::io::xml::tag_indexer, std::vector<sylvanmats::io::xml::tag_indexer>>>& dag){
            std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> cv;
            for(std::vector<std::pair<sylvanmats::io::xml::tag_indexer, std::vector<sylvanmats::io::xml::tag_indexer>>>::iterator it=dag.begin();it!=dag.end();it++){
                sylvanmats::reading::SMIRKSTorsions torsions;
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
                            lemon::ListGraph::Node nA=torsions.processAtoms(r->atoms(), smirksPattern);
                            lemon::ListGraph::Edge e=lemon::INVALID;
                            lemon::ListGraph::Node nB=lemon::INVALID;
                            for(size_t baaIndex=0;baaIndex<r->bonds_and_atoms().size();baaIndex++){
//                            for(std::vector<sylvanmats::SMIRKSParser::Bonds_and_atomsContext *>::iterator itB=r->bonds_and_atoms().begin();itB!=r->bonds_and_atoms().end();++itB){
//                                sylvanmats::SMIRKSParser::AtomsContext* ac=(*itB)->atoms();
//                    std::cout<<"got next atom "<<std::endl;
                                nB=torsions.processAtoms(r->bonds_and_atoms(baaIndex)->atoms(), smirksPattern);
                                if(nA!=lemon::INVALID && nB!=lemon::INVALID){
                                    sylvanmats::SMIRKSParser::Bond_primitivesContext* bc=r->bonds_and_atoms(baaIndex)->bond_primitives();
                                    e=smirksPattern.smirksGraph.addEdge(nA, nB);
                                    torsions.processBonds(bc, e, smirksPattern);
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
                            lemon::ListGraph::Node nA=torsions.processAtoms(r->atoms(), smirksPattern);
                            lemon::ListGraph::Edge eA=lemon::INVALID;
                            lemon::ListGraph::Node nB=lemon::INVALID;
                            lemon::ListGraph::Edge eB=lemon::INVALID;
                            lemon::ListGraph::Node nC=lemon::INVALID;
                            for(size_t baaIndex=0;baaIndex<r->bonds_and_atoms().size();baaIndex++){
//                    std::cout<<"got next atom "<<std::endl;
                                if(baaIndex==0){
                                    nB=torsions.processAtoms(r->bonds_and_atoms(baaIndex)->atoms(), smirksPattern);
                                    if(nA!=lemon::INVALID && nB!=lemon::INVALID){
                                        sylvanmats::SMIRKSParser::Bond_primitivesContext* bc=r->bonds_and_atoms(baaIndex)->bond_primitives();
                                        eA=smirksPattern.smirksGraph.addEdge(nA, nB);
                                        torsions.processBonds(bc, eA, smirksPattern);
                                    }
                                }
                                else if(baaIndex==1){
                                    nC=torsions.processAtoms(r->bonds_and_atoms(baaIndex)->atoms(), smirksPattern);
                                    if(nB!=lemon::INVALID && nC!=lemon::INVALID){
                                        sylvanmats::SMIRKSParser::Bond_primitivesContext* bc=r->bonds_and_atoms(baaIndex)->bond_primitives();
                                        eB=smirksPattern.smirksGraph.addEdge(nB, nC);
                                        torsions.processBonds(bc, eB, smirksPattern);
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
                if(utf16.substr((*it).first.angle_start, (*it).first.angle_end-(*it).first.angle_start).starts_with(u"<ProperTorsions ")){
                    torsions(xmlReaper, utf16, dag, it, u"<Proper ", properSet);
                }
                if(utf16.substr((*it).first.angle_start, (*it).first.angle_end-(*it).first.angle_start).starts_with(u"<ImproperTorsions ")){
                    torsions(xmlReaper, utf16, dag, it, u"<Improper ", improperSet);
                }
            }

        });
    }
    
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
    
    void SMIRKSPatterns::operator()(char8_t atomic_numberA, char8_t connectivityA, sylvanmats::forcefield::BOND_TYPE typeA, char8_t connectivityB, char8_t atomic_numberB, sylvanmats::forcefield::BOND_TYPE typeB, char8_t connectivityC, char8_t atomic_numberC, sylvanmats::forcefield::BOND_TYPE typeC, char8_t connectivityD, char8_t atomic_numberD, std::function<bool(double angle, double k, smirks_torsions_pattern& smirksPattern)> apply){
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
                    ret=apply((*it).length, (*it).k, (*it));
            }
        }
        
    }
    
}
