
#include <algorithm>
#include <iostream>

#include "lexer.hpp"

// Simple command line parser
// -------------------------------------------------------------------------------
char* getCmdOption(char** begin, char** end, const std::string& option) {
  char** itr = std::find(begin, end, option);
  if (itr != end && ++itr != end) {
    return *itr;
  }
  return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option) {
  return std::find(begin, end, option) != end;
}
// -------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
  char* filename = getCmdOption(argv, argv + argc, "-f");

  if (filename) {
    std::cout << "Compiling : " << filename << std::endl;

    // Lexer
    Lexer lexer(filename);

    lexer.run();
    lexer.output_token_list();
  }
  return 0;
}