%code requires {
    #include "ast/lang.hpp" 
}

%{
  #include <stdexcept>
  #include <iostream>
  #include <memory>
%}

%define api.pure full
%lex-param { void* scanner }
%parse-param { void* scanner }
%parse-param { std::unique_ptr<LangAST::Node>& result }

%union {
    LangAST::Node* node;
    LangAST::ExpressionNode* expression;
    LangAST::CommandNode* command;
    LangAST::ProcedureNode* procedure;
    std::string* str;
    long long num;
}

%code {
int yylex(YYSTYPE* yylval_param, void * yyscanner);
extern int yyget_lineno(void*);
extern char* yyget_text(void*);
void yyerror(void* scanner, std::unique_ptr<LangAST::Node>& result, const char *s);
#define YYLEX_PARAM scanner
#define BINARY_OP_NODE(op, v1, v2) LangAST::BinaryOpNode(op, std::unique_ptr<LangAST::ExpressionNode>(v1), std::unique_ptr<LangAST::ExpressionNode>(v2))
}

%type <node> program_all
%type <command> commands command
%type <procedure> main
%type <expression> expr value identifier

%token SEMICOLON ";"
%token <num> NUMBER
%token <str> PIDENTIFIER

// Keywords
%token KW_PROGRAM "PROGRAM"
%token KW_IS "IS"
%token KW_IN "IN"
%token KW_END "END"
%token KW_WRITE "WRITE"

// Operators
%token OP_PLUS "+"
%token OP_MINUS "-"
%token OP_MULT "*"
%token OP_DIV "/"
%token OP_MOD "%"

%token ERROR "ERROR"

%%
program_all: 
    main      {
      $$ = new LangAST::ProgramNode(std::unique_ptr<LangAST::ProcedureNode>($1)); 
      result = std::unique_ptr<LangAST::Node>($$);
    }
  ;

main: 
    KW_PROGRAM KW_IS KW_IN commands KW_END { 
      auto id = std::make_unique<LangAST::IdentifierNode>("main");
      auto cmds = std::unique_ptr<LangAST::CommandNode>($4);
      $$ = new LangAST::ProcedureNode(std::move(id), std::move(cmds));
    }
  ;

commands: 
    command         { $$ = $1; }
  ;

command:
    KW_WRITE expr SEMICOLON   { $$ = new LangAST::WriteNode(std::unique_ptr<LangAST::ExpressionNode>($2)); }
  ;

expr: 
    value                 { $$ = $1; }
  | value OP_PLUS value   { $$ = new BINARY_OP_NODE(Tac::BinaryOp::ADD, $1, $3); }
  | value OP_MINUS value  { $$ = new BINARY_OP_NODE(Tac::BinaryOp::SUB, $1, $3); }
  | value OP_MULT value   { $$ = new BINARY_OP_NODE(Tac::BinaryOp::MUL, $1, $3); }
  | value OP_DIV value    { $$ = new BINARY_OP_NODE(Tac::BinaryOp::DIV, $1, $3); }
  | value OP_MOD value    { $$ = new BINARY_OP_NODE(Tac::BinaryOp::MOD, $1, $3); }
  ;

value:
    identifier     { $$ = $1; }
  | NUMBER         { $$ = new LangAST::ConstantNode($1); }
  ;

identifier:
    PIDENTIFIER    { $$ = new LangAST::IdentifierNode(*$1); delete $1; }
  ;

%%

void yyerror(void* scanner, std::unique_ptr<LangAST::Node>& result, const char *s) {
  char* text = yyget_text(scanner);
  int line = yyget_lineno(scanner);

  std::cerr << "Error: " << s;
  if (text && *text) {
    std::cerr << " near '" << text << "'";
  }
  std::cerr << " on line " << line << std::endl;
}