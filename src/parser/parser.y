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
    AST::ProcHeadNode* proc_head;
    AST::ProcedureNode* procedure;
    AST::ProceduresNode* procedures;
    AST::ProcedureCallNode* proc_call;
    AST::ArgumentsNode* args;
    AST::ArgumentsDeclNode* args_decl;
    AST::Declaration* declaration;
    AST::ParamType param_type;
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
#define BINARY_COND_OP_NODE(op, v1, v2) AST::BinaryCondNode(op, std::unique_ptr<AST::ValueNode>(v1), std::unique_ptr<AST::ValueNode>(v2))
#define SET_LINE(node, scanner) (node)->set_line(yyget_lineno(scanner))
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
%type <procedures> procedures
%type <proc_head> proc_head
%type <proc_call> proc_call
%type <args> args
%type <args_decl> args_decl
%type <param_type> type

%token <num> NUMBER "NUMBER"
%token <str> PIDENTIFIER "IDENTIFIER"

// Program structure keywords
%token KW_PROGRAM "PROGRAM"
%token KW_PROCEDURE "PROCEDURE"
%token KW_IS "IS"
%token KW_IN "IN"
%token KW_END "END"

// Input/Output keywords
%token KW_WRITE "WRITE"
%token KW_READ "READ"

// If statement keywords
%token KW_IF "IF"
%token KW_THEN "THEN"
%token KW_ELSE "ELSE"
%token KW_ENDIF "ENDIF"

// While loop keywords
%token KW_WHILE "WHILE"
%token KW_DO "DO"
%token KW_ENDWHILE "ENDWHILE"

// Repeat loop keywords
%token KW_REPEAT "REPEAT"
%token KW_UNTIL "UNTIL"

// For loop keywords
%token KW_FOR "FOR"
%token KW_FROM "FROM"
%token KW_TO "TO"
%token KW_DOWNTO "DOWNTO"
%token KW_ENDFOR "ENDFOR"

// Operators
%token OP_ASSIGN ":="
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

// Punctuation
%token SEMICOLON ";"
%token COMMA ","
%token LPAREN "("
%token RPAREN ")"
%token LBRACKET "["
%token RBRACKET "]"
%token COLON ":"

// Types
%token TYPE_T "TYPE_T"
%token TYPE_I "TYPE_I"
%token TYPE_O "TYPE_O"

%%
program_all: 
    procedures main      {
      $$ = new AST::ProgramNode(std::unique_ptr<AST::ProceduresNode>($1), std::unique_ptr<AST::MainNode>($2)); 
      result = std::unique_ptr<AST::ProgramNode>($$);
    }
  ;

procedures:
    procedures KW_PROCEDURE proc_head KW_IS declarations KW_IN commands KW_END {
      auto head = std::unique_ptr<AST::ProcHeadNode>($3);
      auto decls = std::unique_ptr<AST::DeclarationsNode>($5);
      auto cmds = std::unique_ptr<AST::CommandsNode>($7);
      $$ = $1;
      $$->add_procedure(std::unique_ptr<AST::ProcedureNode>(new AST::ProcedureNode(std::move(head), std::move(decls), std::move(cmds))));
    }
  | procedures KW_PROCEDURE proc_head KW_IS KW_IN commands KW_END {
      auto head = std::unique_ptr<AST::ProcHeadNode>($3);
      auto cmds = std::unique_ptr<AST::CommandsNode>($6);
      $$ = $1;
      $$->add_procedure(std::unique_ptr<AST::ProcedureNode>(new AST::ProcedureNode(std::move(head), std::move(cmds))));
    }
  | { $$ = new AST::ProceduresNode(); }

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

commands: 
    commands command { $$ = $1; $$->add_command(std::unique_ptr<AST::CommandNode>($2)); }
  | command          { $$ = new AST::CommandsNode(std::unique_ptr<AST::CommandNode>($1)); }
  ;

command:
    identifier OP_ASSIGN expr SEMICOLON                         { 
      $$ = new AST::AssignmentNode(std::unique_ptr<AST::IdentifierNode>($1), std::unique_ptr<AST::ExpressionNode>($3)); 
      SET_LINE($$, scanner);
    }
  | KW_IF condition KW_THEN commands KW_ENDIF                   { 
      $$ = new AST::IfNode(std::unique_ptr<AST::BinaryCondNode>($2), std::unique_ptr<AST::CommandsNode>($4)); 
      SET_LINE($$, scanner);
    }
  | KW_IF condition KW_THEN commands KW_ELSE commands KW_ENDIF  { 
      $$ = new AST::IfNode(std::unique_ptr<AST::BinaryCondNode>($2), std::unique_ptr<AST::CommandsNode>($4), std::unique_ptr<AST::CommandsNode>($6)); 
      SET_LINE($$, scanner);
    }
  | KW_WHILE condition KW_DO commands KW_ENDWHILE               { 
      $$ = new AST::WhileNode(std::unique_ptr<AST::BinaryCondNode>($2), std::unique_ptr<AST::CommandsNode>($4)); 
      SET_LINE($$, scanner);
    }
  | KW_REPEAT commands KW_UNTIL condition SEMICOLON             { 
      $$ = new AST::RepeatNode(std::unique_ptr<AST::BinaryCondNode>($4), std::unique_ptr<AST::CommandsNode>($2)); 
      SET_LINE($$, scanner);
    }
  | KW_FOR PIDENTIFIER KW_FROM value KW_TO value KW_DO commands KW_ENDFOR {
      $$ = new AST::ForNode(*$2, std::unique_ptr<AST::ValueNode>($4), std::unique_ptr<AST::ValueNode>($6), AST::ForNode::Direction::TO, std::unique_ptr<AST::CommandsNode>($8));
      SET_LINE($$, scanner);
      delete $2;
    }
  | KW_FOR PIDENTIFIER KW_FROM value KW_DOWNTO value KW_DO commands KW_ENDFOR {
      $$ = new AST::ForNode(*$2, std::unique_ptr<AST::ValueNode>($4), std::unique_ptr<AST::ValueNode>($6), AST::ForNode::Direction::DOWNTO, std::unique_ptr<AST::CommandsNode>($8));
      SET_LINE($$, scanner);
      delete $2;
    }
  
  | proc_call SEMICOLON                                         { $$ = $1; }
  | KW_READ identifier SEMICOLON                                { 
      $$ = new AST::ReadNode(std::unique_ptr<AST::IdentifierNode>($2)); 
      SET_LINE($$, scanner);
    }
  | KW_WRITE value SEMICOLON                                    { 
      $$ = new AST::WriteNode(std::unique_ptr<AST::ValueNode>($2)); 
      SET_LINE($$, scanner);
    }
  ;

proc_head:
    PIDENTIFIER LPAREN args_decl RPAREN {
      auto args_decl_ptr = std::unique_ptr<AST::ArgumentsDeclNode>($3);
      $$ = new AST::ProcHeadNode(*$1, std::move(args_decl_ptr)); 
      SET_LINE($$, scanner);
      delete $1;
    }
  ;

proc_call:
    PIDENTIFIER LPAREN args RPAREN  { 
      auto args_ptr = std::unique_ptr<AST::ArgumentsNode>($3);
      $$ = new AST::ProcedureCallNode(*$1, std::move(args_ptr)); 
      SET_LINE($$, scanner);
      delete $1;
    }
  ;

declarations:
    declarations COMMA PIDENTIFIER    { 
      auto decl = AST::Declaration::make_var(*$3);
      $$ = $1; $$->add_declaration(decl); delete $3; 
    }
  | declarations COMMA PIDENTIFIER LBRACKET NUMBER COLON NUMBER RBRACKET {
    auto decl = AST::Declaration::make_array(*$3, $5, $7);
    $$ = $1; $$->add_declaration(decl); delete $3;
  }
  | PIDENTIFIER                       {
    auto decl = AST::Declaration::make_var(*$1);
    $$ = new AST::DeclarationsNode(decl); 
    SET_LINE($$, scanner);
    delete $1; 
  }
  | PIDENTIFIER LBRACKET NUMBER COLON NUMBER RBRACKET {
      auto decl = AST::Declaration::make_array(*$1, $3, $5); 
      $$ = new AST::DeclarationsNode(decl); 
      SET_LINE($$, scanner);
      delete $1; 
  }
  ;

args_decl:
    args_decl COMMA type PIDENTIFIER    { $$ = $1; $$->add_argument({*$4, $3}); delete $4; }
  | type PIDENTIFIER                    { 
      $$ = new AST::ArgumentsDeclNode({*$2, $1}); 
      SET_LINE($$, scanner);
      delete $2; 
    }
  ;

type:
    TYPE_T    { $$ = AST::ParamType::ARRAY; }
  | TYPE_O    { $$ = AST::ParamType::UNDEFINED; }
  | TYPE_I    { $$ = AST::ParamType::CONST; }
  |           { $$ = AST::ParamType::DEFAULT; }  
  ;

args:
    args COMMA PIDENTIFIER    { $$ = $1; $$->add_argument(*$3); delete $3; }
  | PIDENTIFIER               { 
      $$ = new AST::ArgumentsNode(*$1); 
      SET_LINE($$, scanner);
      delete $1; 
    }
  ;

expr: 
    value                 { $$ = $1; }
  | value OP_PLUS value   { 
      $$ = new BINARY_OP_NODE(BinaryOp::ADD, $1, $3); 
      SET_LINE($$, scanner);
    }
  | value OP_MINUS value  { 
      $$ = new BINARY_OP_NODE(BinaryOp::SUB, $1, $3); 
      SET_LINE($$, scanner);
    }
  | value OP_MULT value   { 
      $$ = new BINARY_OP_NODE(BinaryOp::MUL, $1, $3); 
      SET_LINE($$, scanner);
    }
  | value OP_DIV value    { 
      $$ = new BINARY_OP_NODE(BinaryOp::DIV, $1, $3); 
      SET_LINE($$, scanner);
    }
  | value OP_MOD value    { 
      $$ = new BINARY_OP_NODE(BinaryOp::MOD, $1, $3); 
      SET_LINE($$, scanner);
    }
  ;

condition:
    value OP_EQ value   { 
      $$ = new BINARY_COND_OP_NODE(BinaryCondOp::EQ, $1, $3); 
      SET_LINE($$, scanner);
    } 
  | value OP_NEQ value  { 
      $$ = new BINARY_COND_OP_NODE(BinaryCondOp::NEQ, $1, $3); 
      SET_LINE($$, scanner);
    }
  | value OP_LT value   { 
      $$ = new BINARY_COND_OP_NODE(BinaryCondOp::LT, $1, $3); 
      SET_LINE($$, scanner);
    }
  | value OP_GT value   { 
      $$ = new BINARY_COND_OP_NODE(BinaryCondOp::GT, $1, $3); 
      SET_LINE($$, scanner);
    }
  | value OP_LE value   { 
      $$ = new BINARY_COND_OP_NODE(BinaryCondOp::LTE, $1, $3); 
      SET_LINE($$, scanner);
    }
  | value OP_GE value   { 
      $$ = new BINARY_COND_OP_NODE(BinaryCondOp::GTE, $1, $3); 
      SET_LINE($$, scanner);
    }
  ;

value:
    identifier     { $$ = $1; }
  | NUMBER         { 
      $$ = new AST::ConstantNode($1); 
      SET_LINE($$, scanner);
    }
  ;

identifier:
    PIDENTIFIER    { 
      $$ = new AST::IdentifierNode(*$1); 
      SET_LINE($$, scanner);
      delete $1; 
    }
  | PIDENTIFIER LBRACKET PIDENTIFIER RBRACKET {
      $$ = new AST::IdentifierNode(*$1, *$3);
      SET_LINE($$, scanner);
      delete $1;
      delete $3;
    }
  | PIDENTIFIER LBRACKET NUMBER RBRACKET {
      $$ = new AST::IdentifierNode(*$1, $3);
      SET_LINE($$, scanner);
      delete $1;
    }
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