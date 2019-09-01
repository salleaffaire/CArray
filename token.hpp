#ifndef CARRAYTOKEN_HPP__
#define CARRAYTOKEN_HPP__

#define MAX_TOKEN_SIZE 256

enum TOKEN_TYPE {
  // General names
  token_name,

  // Reserved words
  token_if,
  token_else,
  token_for,
  token_return,

  // Types
  token_void,
  token_block,

  token_in,
  token_true,
  token_false,

  // Operators
  token_slash,   // Binary
  token_plus,    // Binary
  token_minus,   // Binary
  token_star,    // Binary
  token_modulo,  // Binary
  token_leftpar,
  token_rightpar,
  token_leftbrace,
  token_rightbrace,
  token_leftbracket,
  token_rightbracket,
  token_comma,
  token_equal,         // Binary
  token_notequal,      // Binary
  token_greater,       // Binary
  token_lesser,        // Binary
  token_greaterequal,  // Binary
  token_lesserequal,   // Binary
  token_not,
  token_or,   // Binary
  token_and,  // Binary
  token_xor,  // Binary
  token_bwnot,
  token_bwor,        // Binary
  token_bwand,       // Binary
  token_bwxor,       // Binary
  token_leftshift,   // Binary
  token_rightshift,  // Binary
  token_assignment,

  // Numerals
  token_numeral,
  token_literal
};

struct token {
  TOKEN_TYPE mType;
  char mValue[MAX_TOKEN_SIZE];
  unsigned int mLineNumber;
  unsigned int mCharacterOnLine;
};

void output_token(TOKEN_TYPE token);

#endif