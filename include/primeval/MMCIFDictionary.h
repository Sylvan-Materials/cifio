#pragma once

#include <string_view>
#include <tuple>


#include "cairns/fixed_string.h"
#include "cairns/fixed_stack.h"
#include "cairns/fixed_accumulator.h"

namespace sylvanmats::primeval{

    enum STAR_CAIRN{
        STAR_CAIRN_ROOT,
        STAR_CAIRN_DATA,
        STAR_CAIRN_LOOP,
        STAR_CAIRN_SAVE,
        STAR_CAIRN_TAG,
        STAR_CAIRN_VALUE,
        STAR_CAIRN_LINE_COMMENT
    };

    struct cairn{
        STAR_CAIRN type;
        size_t parent;
        std::string_view token;
        std::string_view value;
        size_t absolute_position;
        size_t column_index;
        size_t depth;
    };

    template <size_t BufferSize>
    class MMCIFDictionary {
        private:
        static constexpr size_t MaxNodes = std::max(size_t{128}, BufferSize / 16);
        static constexpr size_t TotalRuleSlots = MaxNodes * 3;
        static constexpr size_t AccumulatorSize = std::max(size_t{16384}, BufferSize * 4);

        public:
        MMCIFDictionary() = default;
        MMCIFDictionary(const MMCIFDictionary& orig) = delete;
        virtual ~MMCIFDictionary() =  default;
        constexpr std::tuple<size_t, size_t> operator()(std::string_view dicBuffer){
            cairns::fixed_stack<std::tuple<size_t, cairn>, MaxNodes> vertices;
            cairns::fixed_stack<std::tuple<size_t, size_t>, MaxNodes*2> edges;
            cairns::fixed_stack<size_t, 64> parentStack;
            size_t cursor = 0;
            size_t loop_column_count=0;
            size_t loop_column_index=0;
            vertices.emplace(0, cairn{.type=STAR_CAIRN_ROOT});
            parentStack.push(0);
            while(cursor<dicBuffer.size()-200){
                if(cursor==0 || dicBuffer[cursor]=='\n'){
                    if(cursor>0 && cursor<dicBuffer.size()-1)cursor++;
                    if(dicBuffer[cursor]=='d' && cursor<dicBuffer.size()-5 && dicBuffer.substr(cursor, 5).starts_with("data_")){
                        size_t idx=vertices.size();
                        size_t token_length=distanceToWhitespace(cursor+5, dicBuffer);
                        while(parentStack.size()>1)parentStack.pop();
                        vertices.emplace(idx, cairn{.type=STAR_CAIRN_DATA, .parent=parentStack.back(), .token=dicBuffer.substr(cursor, token_length+5), .absolute_position=cursor, .depth=parentStack.size()});
                        edges.emplace(parentStack.back(), idx);
                        parentStack.push(idx);
                        cursor+=token_length+5;
                    }
                    else if(dicBuffer[cursor]=='s' && cursor<dicBuffer.size()-5 && dicBuffer.substr(cursor, 5).starts_with("save_")){
                        size_t idx=vertices.size();
                        size_t token_length=distanceToWhitespace(cursor+5, dicBuffer);
                        if(token_length==0 && parentStack.size()>1)parentStack.pop();
                        vertices.emplace(idx, cairn{.type=STAR_CAIRN_SAVE, .parent=parentStack.back(), .token=dicBuffer.substr(cursor, token_length+5), .absolute_position=cursor, .depth=parentStack.size()});
                        edges.emplace(parentStack.back(), idx);
                        if(token_length>0)parentStack.push(idx);
                        cursor+=token_length+5;
                    }
                    else if(dicBuffer[cursor]=='l' && cursor<dicBuffer.size()-5 && dicBuffer.substr(cursor, 5).starts_with("loop_")){
                        size_t idx=vertices.size();
                        size_t token_length=distanceToWhitespace(cursor+1, dicBuffer);
                        while(parentStack.size()>1)parentStack.pop();
                        vertices.emplace(idx, cairn{.type=STAR_CAIRN_LOOP, .parent=parentStack.back(), .token=dicBuffer.substr(cursor, token_length), .absolute_position=cursor, .depth=parentStack.size()});
                        edges.emplace(parentStack.back(), idx);
                        cursor+=token_length;
                        loop_column_count=0;
                    }
                    else if(dicBuffer[cursor]=='_'){
                        size_t idx=vertices.size();
                        size_t token_length=distanceToWhitespace(cursor+1, dicBuffer);
                        size_t value_start=distanceToNonWhitespace(cursor+1+token_length, dicBuffer)+cursor+token_length;
                        size_t value_length=distanceToWhitespace(value_start+1, dicBuffer);
                        if(value_length==0){
                            if(dicBuffer[value_start]=='\n' && dicBuffer[value_start+1]==';' && dicBuffer[value_start+2]=='\n'){
                                value_length=lengthOfTextBlock(value_start+2, dicBuffer);
                                value_start++;
                            }
                        }
                        vertices.emplace(idx, cairn{.type=STAR_CAIRN_LOOP, .parent=parentStack.back(), .token=dicBuffer.substr(cursor, token_length), .value=dicBuffer.substr(value_start, value_length), .absolute_position=cursor, .depth=parentStack.size()});
                        edges.emplace(parentStack.back(), idx);
                        cursor=value_start+value_length;
                        if(std::get<1>(vertices[parentStack.back()]).type==STAR_CAIRN_LOOP)loop_column_count++;

                    }
                    else if(dicBuffer[cursor]=='#'){
                        while(cursor<dicBuffer.size()-2 && dicBuffer[cursor+1]!='\n')cursor++;
                    }
                    else if(std::get<1>(vertices[parentStack.back()]).type==STAR_CAIRN_LOOP){
                        size_t idx=vertices.size();
                        size_t value_start=distanceToNonWhitespace(cursor+1, dicBuffer)+cursor;
                        size_t value_length=distanceToWhitespace(value_start+1, dicBuffer);
                        if(value_length==0){
                            if(dicBuffer[value_start]=='\n' && dicBuffer[value_start+1]==';' && dicBuffer[value_start+2]=='\n'){
                                value_length=lengthOfTextBlock(value_start+2, dicBuffer);
                                value_start++;
                            }
                        }
                        vertices.emplace(idx, cairn{.type=STAR_CAIRN_VALUE, .parent=parentStack.back(), .value=dicBuffer.substr(value_start, value_length), .absolute_position=cursor, .column_index=loop_column_index, .depth=parentStack.size()});
                        edges.emplace(parentStack.back(), idx);
                        cursor=value_start+value_length;

                        if(loop_column_index==loop_column_count)loop_column_index=0; else loop_column_index++;
                    }
                    else if(dicBuffer[cursor-1]=='\n' && dicBuffer[cursor]==';' && dicBuffer[cursor+1]=='\n'){

                        while(cursor<dicBuffer.size()-2 && dicBuffer[cursor]!='\n' && dicBuffer[cursor+1]!=';')cursor++;
                    }

                }
                cursor++;
                size_t next_lf=cursor;
                while(dicBuffer[next_lf]!='\n' && next_lf<dicBuffer.size())next_lf++;
                if(dicBuffer[next_lf]=='\n')cursor=next_lf;
            }
            return {vertices.size(), edges.size()};       
        };

        inline constexpr size_t distanceToWhitespace(size_t cursor, std::string_view& buffer){
            size_t next_ws=cursor;
            while(next_ws<buffer.size() && buffer[next_ws]!=' ' && buffer[next_ws]!='\t'  && buffer[next_ws]!='\n')next_ws++;
            return next_ws-cursor;
        };

        inline constexpr size_t distanceToNonWhitespace(size_t cursor, std::string_view& buffer){
            size_t next_ws=cursor;
            while(next_ws<buffer.size() && (buffer[next_ws]==' ' || buffer[next_ws]=='\t'  || buffer[next_ws]=='\n'))next_ws++;
            return next_ws-cursor;
        };

        inline constexpr size_t lengthOfTextBlock(size_t cursor, std::string_view& buffer){
            size_t length=cursor;
            while(length<buffer.size()-2 && buffer[length]!='\n' && buffer[length+1]!=';')length++;
            return length-cursor+2;
        };

    };
}