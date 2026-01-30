CXX      := g++
CXXFLAGS := -std=c++17 -Isrc -I.

BISON    := bison
FLEX     := flex

BISON_SRC := src/parser/parser.y
FLEX_SRC  := src/lexer.l

GEN_PARSER_CPP := parser.cpp
GEN_PARSER_HPP := parser.hpp
GEN_LEXER_CPP  := lexer.cpp

OBJS := src/main.o \
        src/parser/parser_wrapper.o \
        src/ast/visitor.o \
        parser.o \
        lexer.o

TARGET := kompilator


.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(GEN_PARSER_CPP) $(GEN_PARSER_HPP): $(BISON_SRC)
	$(BISON) -d -o $(GEN_PARSER_CPP) $(BISON_SRC)

$(GEN_LEXER_CPP): $(FLEX_SRC) $(GEN_PARSER_HPP)
	$(FLEX) -o $(GEN_LEXER_CPP) $(FLEX_SRC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

src/parser/parser_wrapper.o: $(GEN_PARSER_HPP)
src/main.o: $(GEN_PARSER_HPP)
lexer.o: $(GEN_PARSER_HPP)

clean:
	rm -f $(TARGET) $(OBJS)
	rm -f $(GEN_PARSER_CPP) $(GEN_PARSER_HPP) $(GEN_LEXER_CPP)