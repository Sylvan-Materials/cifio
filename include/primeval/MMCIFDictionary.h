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
        size_t absolute_position;
        size_t token_position;
        size_t token_length;
        size_t value_position;
        size_t value_length;
        size_t column_index;
        size_t depth;
    };

    template <typename SchemaType, size_t BufferSize>
    class MMCIFDictionary {
        private:
        static constexpr size_t MaxNodes = std::max(size_t{128}, BufferSize / 16);
        static constexpr size_t TotalRuleSlots = MaxNodes * 3;
        static constexpr size_t AccumulatorSize = std::max(size_t{16384}, BufferSize * 4);

        public:
        MMCIFDictionary() = default;
        MMCIFDictionary(const MMCIFDictionary& orig) = delete;
        virtual ~MMCIFDictionary() =  default;
        constexpr SchemaType operator()(std::string_view dicBuffer){
            cairns::fixed_stack<std::tuple<size_t, cairn>, MaxNodes> vertices;
            cairns::fixed_stack<std::tuple<size_t, size_t>, MaxNodes*2> edges;
            cairns::fixed_stack<size_t, 64> parentStack;
            size_t cursor = 0;
            size_t loop_column_count=0;
            size_t loop_column_index=0;
            vertices.emplace(0, cairn{.type=STAR_CAIRN_ROOT});
            parentStack.push(0);
            bool whitespaced_tag=false;
            while(cursor<dicBuffer.size()){
                if(cursor==0 || dicBuffer[cursor]=='\n' || std::get<1>(vertices[parentStack.back()]).type==STAR_CAIRN_LOOP){
                    if(cursor>0 && cursor<dicBuffer.size()-1)cursor++;
                    if(dicBuffer[cursor]=='d' && cursor<dicBuffer.size()-5 && dicBuffer[cursor+1]=='a' && dicBuffer[cursor+2]=='t' && dicBuffer[cursor+3]=='a' && dicBuffer[cursor+4]=='_'){
                        size_t idx=vertices.size();
                        size_t value_length=distanceToWhitespace(cursor+5, dicBuffer);
                        while(parentStack.size()>1)parentStack.pop();
                        vertices.emplace(idx, cairn{.type=STAR_CAIRN_DATA, .parent=parentStack.back(), .absolute_position=cursor, .token_position=cursor, .token_length=5, .value_position=cursor+5, .value_length=value_length, .depth=parentStack.size()});
                        edges.emplace(parentStack.back(), idx);
                        parentStack.push(idx);
                        cursor+=value_length+5;
                    }
                    else if(dicBuffer[cursor]=='s' && cursor<dicBuffer.size()-5 && dicBuffer.substr(cursor, 5).starts_with("save_")){
                        size_t idx=vertices.size();
                        size_t value_length=distanceToWhitespace(cursor+5, dicBuffer);
                        if(value_length==0 && parentStack.size()>1)parentStack.pop();
                        vertices.emplace(idx, cairn{.type=STAR_CAIRN_SAVE, .parent=parentStack.back(), .absolute_position=cursor, .token_position=cursor, .token_length=5, .value_position=cursor+5, .value_length=value_length, .depth=parentStack.size()});
                        edges.emplace(parentStack.back(), idx);
                        if(value_length>0)parentStack.push(idx);
                        cursor+=value_length+5;
                    }
                    else if(dicBuffer[cursor]=='l' && cursor<dicBuffer.size()-5 && dicBuffer.substr(cursor, 5).starts_with("loop_")){
                        size_t idx=vertices.size();
                        size_t token_length=distanceToWhitespace(cursor, dicBuffer);
                        while(parentStack.size()>1)parentStack.pop();
                        vertices.emplace(idx, cairn{.type=STAR_CAIRN_LOOP, .parent=parentStack.back(), .absolute_position=cursor, .token_position=cursor, .token_length=token_length, .depth=parentStack.size()});
                        edges.emplace(parentStack.back(), idx);
                        parentStack.push(idx);
                        cursor+=token_length;
                        loop_column_count=0;
                    }
                    else if(dicBuffer[cursor]=='_' || isWhitespacedTag(cursor, dicBuffer)){
                        if(isWhitespacedTag(cursor, dicBuffer)){
                            size_t jump_distance=distanceToNonWhitespace(cursor, dicBuffer);
                            cursor+=jump_distance;
                        }
                        size_t idx=vertices.size();
                        size_t token_length=distanceToWhitespace(cursor, dicBuffer);
                        size_t value_start=cursor+token_length;
                        size_t value_length=0;
                        if(hasNotEndOfLine(value_start, dicBuffer)){
                            value_start+=distanceToNonWhitespace(value_start, dicBuffer);
                            value_length=distanceToWhitespace(value_start, dicBuffer);
                        }
                        if(value_length==0){
                            jumpToEndOfLine(value_start, dicBuffer);
                            if(dicBuffer[value_start]=='\n' && dicBuffer[value_start+1]==';'){
                                value_length=lengthOfTextBlock(value_start+3, dicBuffer);
                                value_start+=2;
                            }
                        }
                        vertices.emplace(idx, cairn{.type=STAR_CAIRN_TAG, .parent=parentStack.back(), .absolute_position=cursor, .token_position=cursor, .token_length=token_length, .value_position=value_start, .value_length=value_length, .depth=parentStack.size()});
                        edges.emplace(parentStack.back(), idx);
                        cursor=value_start+value_length;
                        if(std::get<1>(vertices[parentStack.back()]).type==STAR_CAIRN_LOOP)loop_column_count++;
                        else loop_column_count=0;

                    }
                    else if(dicBuffer[cursor]=='#' || hasBlankLine(cursor, dicBuffer)){
                        cursor++;
                        while(cursor<dicBuffer.size()-2 && dicBuffer[cursor]!='\n')cursor++;
                    }
                    else if(std::get<1>(vertices[parentStack.back()]).type==STAR_CAIRN_LOOP){
                        size_t idx=vertices.size();
                        size_t value_start=cursor;
                        // if(hasNotEndOfLine(cursor, dicBuffer)){
                            value_start+=distanceToNonWhitespace(cursor, dicBuffer);
                            // if(dicBuffer[value_start]=='\n')
                        // }
                    
                        size_t value_length=0;
                        if(dicBuffer[value_start]==';'){
                            value_length=lengthOfTextBlock(value_start+2, dicBuffer);
                            value_start+=1;
                        }
                        else if(hasNotEndOfLine(value_start, dicBuffer)){
                            if(dicBuffer[value_start]=='"'){
                                value_length=lengthOfDoubleQuote(value_start, dicBuffer);
                            }
                            else value_length=distanceToWhitespace(value_start, dicBuffer);
                        }
                        // if(value_length==0){
                        //     jumpToEndOfLine(value_start, dicBuffer);
                        //     if(dicBuffer[value_start]=='\n' && dicBuffer[value_start+1]==';'){
                        //         value_length=lengthOfTextBlock(value_start+3, dicBuffer);
                        //         value_start+=2;
                        //     }
                        // }
                        vertices.emplace(idx, cairn{.type=STAR_CAIRN_VALUE, .parent=parentStack.back(), .absolute_position=cursor, .value_position=value_start, .value_length=value_length, .column_index=loop_column_index, .depth=parentStack.size()});
                        edges.emplace(parentStack.back(), idx);
                        cursor=value_start+value_length;
                        if(!hasNotEndOfLine(cursor, dicBuffer) && dicBuffer[cursor]!='\n'){
                            size_t jump_test=cursor;
                            jumpToEndOfLine(jump_test, dicBuffer);
                            if(dicBuffer[jump_test+1]!=';')cursor=jump_test;
                        }
                        if(loop_column_index==loop_column_count)loop_column_index=0; else loop_column_index++;
                        continue;
                    }
                    else if(dicBuffer[cursor-1]=='\n' && dicBuffer[cursor]==';' && dicBuffer[cursor+1]=='\n'){

                        while(cursor<dicBuffer.size()-2 && dicBuffer[cursor]!='\n' && dicBuffer[cursor+1]!=';')cursor++;
                    }

                }
                
                size_t next_lf=cursor;
                while(dicBuffer[next_lf]!='\n' && next_lf<dicBuffer.size())next_lf++;
                if(dicBuffer[next_lf]=='\n' && next_lf<dicBuffer.size()-1)cursor=next_lf;
                else cursor++;
            }
            return SchemaType{std::move(vertices), std::move(edges)};
        };

        inline constexpr bool isWhitespacedTag(size_t cursor, std::string_view& buffer){
            size_t next_ws=cursor;
            while(next_ws<buffer.size() && (buffer[next_ws]==' ' || buffer[next_ws]=='\t')  && buffer[next_ws]!='\n'){
                next_ws++;
            }
            if(next_ws<buffer.size() && buffer[next_ws]=='_') return true;
            return false;
            
        };

        inline constexpr size_t distanceToWhitespace(size_t cursor, std::string_view& buffer){
            size_t next_ws=cursor;
            while(next_ws<buffer.size() && buffer[next_ws]!=' ' && buffer[next_ws]!='\t'  && buffer[next_ws]!='\n')next_ws++;
            return (next_ws<buffer.size()) ? next_ws-cursor : next_ws-cursor-1;
        };

        inline constexpr size_t distanceToNonWhitespace(size_t cursor, std::string_view& buffer){
            size_t next_ws=cursor;
            while(next_ws<buffer.size() && (buffer[next_ws]==' ' || buffer[next_ws]=='\t'  || buffer[next_ws]=='\n'))next_ws++;
            return (next_ws<buffer.size()) ? next_ws-cursor : next_ws-cursor-1;
        };

        inline constexpr bool hasBlankLine(size_t cursor, std::string_view& buffer){
            if(cursor>0 && buffer[cursor]!='\n') return false;
            size_t next_ws=cursor+1;
            while(next_ws<buffer.size() && (buffer[next_ws]==' ' || buffer[next_ws]=='\t'))next_ws++;
            return  (next_ws<buffer.size() && buffer[next_ws]=='\n') ? true: false;
        };

        inline constexpr bool hasNotEndOfLine(size_t cursor, std::string_view& buffer){
            size_t next_ws=cursor;
            while(next_ws<buffer.size() && (buffer[next_ws]==' ' || buffer[next_ws]=='\t'))next_ws++;
            return (next_ws<buffer.size() && buffer[next_ws]=='\n') ? false: true;
        };

        inline constexpr void jumpToEndOfLine(size_t& cursor, std::string_view& buffer){
            while(buffer[cursor]!='\n')cursor++;
        };

        inline constexpr bool hasNotDoubleQuote(size_t cursor, std::string_view& buffer){
            size_t next_ws=cursor;
            while(next_ws<buffer.size() && buffer[next_ws]!='\n'){
                if(buffer[next_ws]==' ' || buffer[next_ws]=='\t')return false;
                if(buffer[next_ws]=='"') return true;
                next_ws++;
            }
            return  (next_ws<buffer.size() && buffer[next_ws]=='\n') ? false: true;
        };

        inline constexpr bool isDoubleQuoted(size_t cursor, std::string_view& buffer){
            size_t next_ws=cursor;
            while(next_ws<buffer.size() && (buffer[next_ws]!='"' && buffer[next_ws]!='\n'))next_ws++;
            if(buffer[next_ws]=='\n')return false;
            next_ws++;
            while(next_ws<buffer.size() && (buffer[next_ws]!='"' && buffer[next_ws]!='\n'))next_ws++;
            return  (next_ws<buffer.size() && buffer[next_ws]=='"') ? true: false;
        };

        inline constexpr size_t lengthOfTextBlock(size_t cursor, std::string_view& buffer){
            size_t length=cursor;
            while(length<buffer.size()-2 && !(buffer[length]=='\n' && buffer[length+1]==';'))length++;
            return length-cursor+2;
        };

        inline constexpr size_t lengthOfDoubleQuote(size_t cursor, std::string_view& buffer){
            size_t length=(buffer[cursor]=='"')? cursor+1:cursor;
            while(length<buffer.size()-2 && buffer[length]!='"'){
                if(buffer[length]=='\n')break;
                length++;
            }
            return length-cursor+1;
        };

    };
}