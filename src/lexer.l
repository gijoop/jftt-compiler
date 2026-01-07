%option yylineno
%option reentrant
%option bison-bridge
%option noyywrap

%{
#include <stdexcept>
#include "parser.hpp"

void yyerror(void* scanner, AST::ProgramNode** result, const char *s);
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
"READ"     { return KW_READ; }

"IF"       { return KW_IF; }
"THEN"     { return KW_THEN; }
"ELSE"     { return KW_ELSE; }
"ENDIF"    { return KW_ENDIF; }

"WHILE"    { return KW_WHILE; }
"DO"       { return KW_DO; }
"ENDWHILE" { return KW_ENDWHILE; }

"REPEAT"   { return KW_REPEAT; }
"UNTIL"    { return KW_UNTIL; }

":="       { return OP_ASSIGN; }
","        { return COMMA; }

"+"        { return OP_PLUS; }
"-"        { return OP_MINUS; }
"*"        { return OP_MULT; }
"/"        { return OP_DIV; }
"%"        { return OP_MOD; }

"="        { return OP_EQ; }
"!="       { return OP_NEQ; }
"<"        { return OP_LT; }
">"        { return OP_GT; }
"<="       { return OP_LE; }
">="       { return OP_GE; }

#.*         ;
.          { return yytext[0]; }

%%
