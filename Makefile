# ==========================================
# JfttCompiler Makefile (Fixed)
# ==========================================

# Compiler and Flags
CXX      := g++
CXXFLAGS := -std=c++17 -Isrc -I.

# Tools
BISON    := bison
FLEX     := flex

# Source Files
BISON_SRC := src/parser/parser.y
FLEX_SRC  := src/lexer.l

# Generated Files
GEN_PARSER_CPP := parser.cpp
GEN_PARSER_HPP := parser.hpp
GEN_LEXER_CPP  := lexer.cpp

# Object Files
OBJS := src/main.o \
        src/parser/parser_wrapper.o \
        src/ast/visitor.o \
        parser.o \
        lexer.o

# Output Executable
TARGET := kompilator

# ==========================================
# Rules
# ==========================================

.PHONY: all clean

# Default rule
all: $(TARGET)

# Link the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# 1. Generate Parser (Bison)
# -d creates the header file (parser.hpp)
$(GEN_PARSER_CPP) $(GEN_PARSER_HPP): $(BISON_SRC)
	$(BISON) -d -o $(GEN_PARSER_CPP) $(BISON_SRC)

# 2. Generate Lexer (Flex)
$(GEN_LEXER_CPP): $(FLEX_SRC) $(GEN_PARSER_HPP)
	$(FLEX) -o $(GEN_LEXER_CPP) $(FLEX_SRC)

# 3. Generic Compilation Rule
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- EXPLICIT DEPENDENCIES (The Fix) ---
# These ensure generated files exist before compiling the wrappers that include them

src/parser/parser_wrapper.o: $(GEN_PARSER_HPP)
src/main.o: $(GEN_PARSER_HPP)
lexer.o: $(GEN_PARSER_HPP)

# Clean rule
clean:
	rm -f $(TARGET) $(OBJS)
	rm -f $(GEN_PARSER_CPP) $(GEN_PARSER_HPP) $(GEN_LEXER_CPP)