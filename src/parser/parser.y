%code requires {
    #include "ast/ast.hpp" 
    #include "types.hpp"
}

%{
  #include <stdexcept>
  #include <iostream>
  #include <memory>
%}

%define api.pure full
%define parse.error verbose
%lex-param { void* scanner }
%parse-param { void* scanner }
%parse-param { std::unique_ptr<AST::ProgramNode>& result }

%union {
    AST::Node* node;
    AST::ExpressionNode* expression;
    AST::CommandNode* command;
    AST::CommandsNode* commands;
    AST::DeclarationsNode* declarations;
    AST::MainNode* main;
    AST::ProgramNode* program_all;
    AST::IdentifierNode* identifier;
    AST::ValueNode* value;
    AST::BinaryCondNode* condition;
    std::string* str;
    long long num;
}

%code {
int yylex(YYSTYPE* yylval_param, void * yyscanner);
extern int yyget_lineno(void*);
extern char* yyget_text(void*);
void yyerror(void* scanner, std::unique_ptr<AST::ProgramNode>& result, const char *s);
#define YYLEX_PARAM scanner
#define BINARY_OP_NODE(op, v1, v2) AST::BinaryOpNode(op, std::unique_ptr<AST::ValueNode>(v1), std::unique_ptr<AST::ValueNode>(v2))
#define BINARY_COND_OP_NODE(op, v1, v2) AST::BinaryCondOpNode(op, std::unique_ptr<AST::ValueNode>(v1), std::unique_ptr<AST::ValueNode>(v2))
}

%type <program_all> program_all
%type <commands> commands
%type <command> command
%type <declarations> declarations
%type <main> main
%type <identifier> identifier
%type <expression> expr
%type <value> value
%type <condition> condition

%token SEMICOLON ";"
%token <num> NUMBER "NUMBER"
%token <str> PIDENTIFIER "IDENTIFIER"

// Keywords
%token KW_PROGRAM "PROGRAM"
%token KW_IS "IS"
%token KW_IN "IN"
%token KW_END "END"
%token KW_WRITE "WRITE"
%token KW_READ "READ"
%token KW_IF "IF"
%token KW_THEN "THEN"
%token KW_ELSE "ELSE"
%token KW_ENDIF "ENDIF"

// Operators
%token OP_PLUS "+"
%token OP_MINUS "-"
%token OP_MULT "*"
%token OP_DIV "/"
%token OP_MOD "%"

// Conditional Operators
%token OP_EQ "="
%token OP_NEQ "!="
%token OP_GT ">"
%token OP_LT "<"
%token OP_GE ">="
%token OP_LE "<="

%token OP_COMMA ","
%token OP_ASSIGN ":="

%%
program_all: 
    main      {
      $$ = new AST::ProgramNode(std::unique_ptr<AST::MainNode>($1)); 
      result = std::unique_ptr<AST::ProgramNode>($$);
    }
  ;

main: 
    KW_PROGRAM KW_IS declarations KW_IN commands KW_END {
      auto decls = std::unique_ptr<AST::DeclarationsNode>($3);
      auto cmds = std::unique_ptr<AST::CommandsNode>($5);
      $$ = new AST::MainNode(std::move(decls), std::move(cmds));
    }
  | KW_PROGRAM KW_IS KW_IN commands KW_END { 
      auto cmds = std::unique_ptr<AST::CommandsNode>($4);
      $$ = new AST::MainNode(std::move(cmds));
    }
  ;

declarations:
    declarations OP_COMMA identifier { $$ = $1; $$->add_declaration(std::unique_ptr<AST::IdentifierNode>($3)); }
  | identifier                       { $$ = new AST::DeclarationsNode(std::unique_ptr<AST::IdentifierNode>($1)); }
  ;

commands: 
    commands command { $$ = $1; $$->add_command(std::unique_ptr<AST::CommandNode>($2)); }
  | command          { $$ = new AST::CommandsNode(std::unique_ptr<AST::CommandNode>($1)); }
  ;

command:
    identifier OP_ASSIGN expr SEMICOLON { $$ = new AST::AssignmentNode(std::unique_ptr<AST::IdentifierNode>($1), std::unique_ptr<AST::ExpressionNode>($3)); }
  | KW_WRITE value SEMICOLON             { $$ = new AST::WriteNode(std::unique_ptr<AST::ValueNode>($2)); }
  | KW_READ identifier SEMICOLON         { $$ = new AST::ReadNode(std::unique_ptr<AST::IdentifierNode>($2)); }
  ;

expr: 
    value                 { $$ = $1; }
  | value OP_PLUS value   { $$ = new BINARY_OP_NODE(BinaryOp::ADD, $1, $3); }
  | value OP_MINUS value  { $$ = new BINARY_OP_NODE(BinaryOp::SUB, $1, $3); }
  | value OP_MULT value   { $$ = new BINARY_OP_NODE(BinaryOp::MUL, $1, $3); }
  | value OP_DIV value    { $$ = new BINARY_OP_NODE(BinaryOp::DIV, $1, $3); }
  | value OP_MOD value    { $$ = new BINARY_OP_NODE(BinaryOp::MOD, $1, $3); }
  ;

condition:
    value OP_EQ value   { $$ = new BINARY_COND_OP_NODE(BinaryCondOp::EQ, $1, $3); }
  | value OP_NEQ value  { $$ = new BINARY_COND_OP_NODE(BinaryCondOp::NEQ, $1, $3); }
  | value OP_LT value   { $$ = new BINARY_COND_OP_NODE(BinaryCondOp::LT, $1, $3); }
  | value OP_GT value   { $$ = new BINARY_COND_OP_NODE(BinaryCondOp::GT, $1, $3); }
  | value OP_LE value   { $$ = new BINARY_COND_OP_NODE(BinaryCondOp::LE, $1, $3); }
  | value OP_GE value   { $$ = new BINARY_COND_OP_NODE(BinaryCondOp::GE, $1, $3); }
  ;

value:
    identifier     { $$ = $1; }
  | NUMBER         { $$ = new AST::ConstantNode($1); }
  ;

identifier:
    PIDENTIFIER    { $$ = new AST::IdentifierNode(*$1); delete $1; }
  ;

%%

void yyerror(void* scanner, std::unique_ptr<AST::ProgramNode>& result, const char *s) {
  char* text = yyget_text(scanner);
  int line = yyget_lineno(scanner);

  std::cerr << "Error: " << s;
  if (text && *text) {
    std::cerr << " near '" << text << "'";
  }
  std::cerr << " on line " << line << std::endl;
}