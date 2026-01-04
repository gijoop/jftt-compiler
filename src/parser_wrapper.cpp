#include <cstdio>
#include "parser_wrapper.hpp"
#include "parser.hpp"

typedef struct yy_buffer_state* YY_BUFFER_STATE;
YY_BUFFER_STATE yy_scan_string(const char*, void*);
void yy_delete_buffer(YY_BUFFER_STATE, void*);
int yylex_init(void**);
int yylex_destroy(void*);
void yyset_lineno(int, void*);

int yyparse(void* scanner, LangAST::ProgramNode** result);

ParserWrapper::ParserWrapper() : result(nullptr), scanner(nullptr) {
    yylex_init(&scanner);
}

ParserWrapper::~ParserWrapper() {
    if (scanner) {
        yylex_destroy(scanner);
    }
}

bool ParserWrapper::parse(const std::string& input) {
    if (result) {
        result.reset();
    }

    YY_BUFFER_STATE buffer = yy_scan_string(input.c_str(), scanner);
    
    yyset_lineno(1, scanner);

    int status = yyparse(scanner, result);

    yy_delete_buffer(buffer, scanner);

    return (status == 0);
}