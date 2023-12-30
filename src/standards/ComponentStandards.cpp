#include "standards/ComponentStandards.h"

namespace sylvanmats::standards{
        bool ComponentStandards::operator()(const std::vector<std::string>& comp_ids, std::function<void(chem_comp_atom<double>& cca1, chem_comp_bond& ccb, chem_comp_atom<double>& cca2)> apply){
            bool ret=false;
            unsigned int index=0;
            do{
                     standard_set_by_name::iterator it=nameIndex.find(comp_ids[index]);
//                   std::cout<<comp_ids.back()<<" (it!=nameIndex.end())."<<(it!=nameIndex.end())<<std::endl;
            /*if(it!=nameIndex.end()){
                for(std::vector<chem_comp_bond>::iterator itStd=(*it).chemCombonds.begin();itStd!=(*it).chemCombonds.end();itStd++){
                    std::vector<chem_comp_atom<double>>::iterator cca1=std::find_if((*it).chemCompAtoms.begin(), (*it).chemCompAtoms.end(), [&](chem_comp_atom<double>& cca){return (*itStd).atom_id_1.compare(cca.atom_id)==0;});
                    std::vector<chem_comp_atom<double>>::iterator cca2=std::find_if((*it).chemCompAtoms.begin(), (*it).chemCompAtoms.end(), [&](chem_comp_atom<double>& cca){return (*itStd).atom_id_2.compare(cca.atom_id)==0;});
                    apply((*cca1), (*itStd), (*cca2));
                }
                return true;
            }*/
            sylvanmats::io::json::Path jpStart(comp_ids[index].c_str());
            jpStart["start"];
            sylvanmats::io::json::Path jpEnd(comp_ids[index].c_str());
            jpEnd["end"];
//            std::cout<<"jpStart "<<jpStart<<" jpEnd "<<jpEnd<<" "<<jsonBinder.countObjects()<<std::endl;
            try{
        jsonBinder(jpStart, [&](std::any& v){
            unsigned int start=std::any_cast<long>(v);
        jsonBinder(jpEnd, [&](std::any& v){
            unsigned int end=std::any_cast<long>(v);
//std::cout<<"start "<<start<<" end"<<end<<" "<<(end-start)<<std::endl;
            if((end-start)<=0)return false;
            mio::mmap_source mmap2nd(path.string(), start, end-start+1);
            std::string content=std::string(mmap2nd.begin(), mmap2nd.end());
//std::cout<<"content "<<content<<std::endl;
            mmap2nd.unmap();
            std::shared_ptr<antlr4::ANTLRInputStream> input=std::make_shared<antlr4::ANTLRInputStream>(content);
            std::shared_ptr<sylvanmats::CIFLexer> lexer=std::make_shared<sylvanmats::CIFLexer>(input.get());
            std::shared_ptr<antlr4::CommonTokenStream> tokens=std::make_shared<antlr4::CommonTokenStream>(lexer.get());

            std::shared_ptr<sylvanmats::CIFParser> parser=std::make_shared<sylvanmats::CIFParser>(tokens.get());
            //parser->setBuildParseTree(true);
            //std::cout<<"parser "<<std::endl;
            antlr4::tree::ParseTree* tree = parser->cif();

            //std::cout << tree->toStringTree(&parser) << std::endl << std::endl;

            const std::string thePath="/cif/dataBlock";
            std::shared_ptr<antlr4::tree::xpath::XPath> xPath=std::make_shared<antlr4::tree::xpath::XPath>(parser.get(), thePath);
            std::vector<antlr4::tree::ParseTree*> dataBlock=xPath->evaluate(tree);
            for(std::vector<antlr4::tree::ParseTree*>::iterator itDB=dataBlock.begin();!ret && itDB!=dataBlock.end();itDB++){
                    //std::cout<<" "<<(*itDB)->toStringTree()<<std::endl;
                bool atomSites=false;
                if (sylvanmats::CIFParser::DataBlockContext* r=dynamic_cast<sylvanmats::CIFParser::DataBlockContext*>((*itDB))) {
                    if(r->dataItems().size()>0 && r->dataItems(0)->tag()!=nullptr &&  r->dataItems(0)->tag()->getText().compare("\n_chem_comp.id")==0){
                        if(r->dataItems(0)->value()->getText().compare(comp_ids[index])==0){
                        //std::cout<<""<<comp_id<<" ? "<<r->dataItems(0)->value()->getText()<<std::endl;
                            auto oi=r->dataItems();
                            std::vector<chem_comp_atom<double>> chemCompAtoms;
                            for(sylvanmats::CIFParser::DataItemsContext* l: oi | std::views::filter([](sylvanmats::CIFParser::DataItemsContext* di){ return di->loop()!=nullptr && di->loop()->loopHeader()->tag().size()>0 && di->loop()->loopHeader()->tag(0)->getText().rfind("\n_chem_comp_atom.", 0) == 0; })){
                                unsigned int columnCount=0;
                                 chemCompAtoms.push_back(chem_comp_atom<double>());
                                std::vector<sylvanmats::CIFParser::ValueContext *> values=l->loop()->loopBody()->value();
                                 for(unsigned int valueIndex=0;valueIndex<values.size();valueIndex++){
                                     //std::cout<<values[valueIndex]->getText()<<" ";
                                     switch(columnCount){
                                          case 0:
                                              chemCompAtoms.back().comp_id.assign(values[valueIndex]->getText());
                                          break;
                                          case 1:
                                              chemCompAtoms.back().atom_id.assign(values[valueIndex]->getText());
                                          break;
                                          case 2:
                                              chemCompAtoms.back().alt_atom_id.assign(values[valueIndex]->getText());
                                          break;
                                          case 3:
                                              chemCompAtoms.back().type_symbol.assign(values[valueIndex]->getText());
                                          break;
                                          case 4:
                                              chemCompAtoms.back().charge=std::strtol(values[valueIndex]->getText().c_str(), nullptr, 10);
                                          break;
                                          case 6:
                                              chemCompAtoms.back().pdbx_aromatic_flag.assign(values[valueIndex]->getText());
                                          break;
                                          case 7:
                                              chemCompAtoms.back().pdbx_leaving_atom_flag.assign(values[valueIndex]->getText());
                                          break;
                                          case 8:
                                              chemCompAtoms.back().pdbx_stereo_config.assign(values[valueIndex]->getText());
                                          break;
                                     }
                                     columnCount++;
                                     if(valueIndex % (l->loop()->loopHeader()->tag().size()) == l->loop()->loopHeader()->tag().size()-1){
                                         //std::cout<<std::endl;
                                         columnCount=0;
                                        chemCompAtoms.push_back(chem_comp_atom<double>());
                                     }
                                  }
                            }
                            std::vector<chem_comp_bond> chemCombonds;
                            for(sylvanmats::CIFParser::DataItemsContext* l: oi | std::views::filter([](sylvanmats::CIFParser::DataItemsContext* di){ return di->loop()!=nullptr && di->loop()->loopHeader()->tag().size()>0 && di->loop()->loopHeader()->tag(0)->getText().rfind("\n_chem_comp_bond.", 0) == 0; })){
                                unsigned int columnCount=0;
                                 chemCombonds.push_back(chem_comp_bond());
                                 std::vector<sylvanmats::CIFParser::ValueContext *> values=l->loop()->loopBody()->value();
                                 for(unsigned int valueIndex=0;valueIndex<l->loop()->loopBody()->value().size();valueIndex++){
                                     //std::cout<<values[valueIndex]->getText()<<" ";
                                     switch(columnCount){
                                          case 0:
                                              chemCombonds.back().comp_id.assign(values[valueIndex]->getText());
                                          break;
                                          case 1:
                                              chemCombonds.back().atom_id_1.assign(values[valueIndex]->getText());
                                          break;
                                          case 2:
                                              chemCombonds.back().atom_id_2.assign(values[valueIndex]->getText());
                                          break;
                                          case 3:
                                            if(values[valueIndex]->getText().compare("DOUB")==0){
                                              chemCombonds.back().value_order=2;
                                            }
                                            else if(values[valueIndex]->getText().compare("TRIP")==0){
                                              chemCombonds.back().value_order=3;
                                            }
                                            else chemCombonds.back().value_order=1;
                                          break;
                                     }
                                     columnCount++;
                                     if(valueIndex % (l->loop()->loopHeader()->tag().size()) == l->loop()->loopHeader()->tag().size()-1){
                                         //std::cout<<std::endl;
                                         std::vector<chem_comp_atom<double>>::iterator cca1=std::find_if(std::begin(chemCompAtoms), std::end(chemCompAtoms), [&](chem_comp_atom<double>& cca){return chemCombonds.back().atom_id_1.compare(cca.atom_id)==0;});
                                         std::vector<chem_comp_atom<double>>::iterator cca2=std::find_if(std::begin(chemCompAtoms), std::end(chemCompAtoms), [&](chem_comp_atom<double>& cca){return chemCombonds.back().atom_id_2.compare(cca.atom_id)==0;});
                                         apply((*cca1), chemCombonds.back(), (*cca2));
                                         columnCount=0;
                                         ret=true;
                                     }
                                  }
                            }
                            componentStandardSet.insert(standard(componentStandardSet.size(), static_cast<std::string>(comp_ids[index]), chemCompAtoms, chemCombonds));
                            //dic->loop()
                            break;
                        }
                    }
                }
            }
        });
        });
            
            return ret;
            }
            catch(std::exception& ex){
                std::cout<<"huh? ex: "<<ex.what()<<std::endl;
            }
            index++;
            }while(index<comp_ids.size());
            return ret;
        };

}

