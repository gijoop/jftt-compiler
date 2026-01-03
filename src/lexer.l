%option yylineno
%option reentrant
%option bison-bridge
%option noyywrap

%{
#include <stdexcept>
#include "parser.hpp"

void yyerror(void* scanner, LangAST::Node** result, const char *s);
%}

%%

[_a-z]+     { yylval->str = new std::string(yytext); return PIDENTIFIER; }
[0-9]+      { yylval->num = std::stoi(yytext); return NUMBER; }
";"         { return SEMICOLON; }
[ \t\n]+      ;
"PROGRAM"  { return KW_PROGRAM; }
"IS"       { return KW_IS; }
"IN"       { return KW_IN; }
"END"      { return KW_END; }
"WRITE"    { return KW_WRITE; }

"+"        { return OP_PLUS; }
"-"        { return OP_MINUS; }
"*"        { return OP_MULT; }
"/"        { return OP_DIV; }
"%"        { return OP_MOD; }

#.*         ;
.          { return ERROR; }

%%
