#include "parse_tree_nodes.h"
#ifdef _MSC_VER
#endif

#include "lexer.h"
#include "parser.h"
#include <cstring>
#include <iostream>
#include <stdio.h>

using namespace std;

extern "C" {
extern FILE *yyin;
extern FILE *yyout;
extern int yyleng;
extern char *yytext;
extern int yylineno;
extern int yylex();
}

extern int nextToken;

bool printParse = false;
bool printTree = false;
bool printSymbolTable = false;

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-p") == 0) {
      printParse = true;
    } else if (strcmp(argv[i], "-t") == 0) {
      printTree = true;
    } else if (strcmp(argv[i], "-s") == 0) {
      printSymbolTable = true;
    } else {
      printf("INFO: Using the %s file for input\n", argv[i]);
      yyin = fopen(argv[i], "r");
    }
  }

  if (!yyin) {
    printf("ERROR: input file not found\n");
    return EXIT_FAILURE;
  }

  yyout = stdout;

  nextToken = yylex();

  ProgramNode *root = nullptr;

  try {
    root = program();

    if (nextToken != TOK_EOF)
      throw "EOF expected";

  } catch (char const *errmsg) {
    cout << endl << "***ERROR:" << endl;
    cout << "On line number " << yylineno << ", near |" << yytext
         << "|, error type ";
    cout << errmsg << endl;
    return EXIT_FAILURE;
  }

  cout << endl << "=== parse successful ===" << endl;

  if (printTree) {
    cout << endl << "*** Program Tree ***" << endl;

    if (root != nullptr)
      cout << *root << endl;
  }

  cout << root->interpret() << "\n";

  if (printSymbolTable) {
    cout << endl << endl << "*** User Defined Symbols ***" << endl;
    for (auto it = symbolTable.begin(); it != symbolTable.end(); ++it)
      cout << (*it).first << ": " << std::to_string((*it).second) << std::endl;
  }

  return EXIT_SUCCESS;
}
