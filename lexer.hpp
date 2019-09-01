#ifndef LEXER_HPP__
#define LEXER_HPP__

#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <string>

#include "token.hpp"

#define MIN(X, Y) ((X < Y) ? (X) : (Y))

enum LEXER_STATE { l_ok = 0x0000, l_uninit = 0x0001, l_error = 0xF000 };

enum LEXER_ERROR {
  l_error_unknown = 0x0000,
  l_error_file = 0x0001,  // Input file can't be opened
  l_error_syntax = 0x0002
};

class Lexer {
 public:
  explicit Lexer()
      : mPayload((char *)0),
        mPayloadSize(0),
        mState(l_uninit),
        mError(l_error_unknown) {
    init();
  }

  explicit Lexer(std::string filename)
      : mPayload((char *)0),
        mPayloadSize(0),
        mState(l_uninit),
        mError(l_error_unknown) {
    // Read the file into memory
    // ------------------------------------------
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    if (in.fail()) {
      mState = l_error;
      mError = l_error_file;
    } else {
      mState = l_ok;

      mPayloadSize = in.tellg();

      // std::cout << "File size of : " << filesize << " bytes." << std::endl;

      // return at the begining
      in.seekg(0, in.beg);

      // Allocate mem ory for file
      mPayload = new char[mPayloadSize];

      // Read
      in.read((char *)mPayload, mPayloadSize);
      in.close();
    }

    init();
  }

  ~Lexer() {
    if (mPayload) delete[] mPayload;
  }

  bool init() {
    // Token names map
    mTokenTypeNames[token_name] = "Name";

    mTokenTypeNames[token_if] = "If";
    mTokenTypeNames[token_else] = "Else";
    mTokenTypeNames[token_for] = "For";
    mTokenTypeNames[token_return] = "Return";

    // Base Types
    mTokenTypeNames[token_void] = "Void";
    mTokenTypeNames[token_block] = "Block";

    //
    mTokenTypeNames[token_in] = "In";
    mTokenTypeNames[token_true] = "True";
    mTokenTypeNames[token_false] = "False";

    // Operators
    mTokenTypeNames[token_slash] = "Slash";
    mTokenTypeNames[token_plus] = "Plus";
    mTokenTypeNames[token_minus] = "Minus";
    mTokenTypeNames[token_star] = "Star";
    mTokenTypeNames[token_modulo] = "Modulo";
    mTokenTypeNames[token_leftpar] = "Left Parenthese";
    mTokenTypeNames[token_rightpar] = "Right Parenthese";
    mTokenTypeNames[token_leftbrace] = "Left Brace";
    mTokenTypeNames[token_rightbrace] = "Right Brace";
    mTokenTypeNames[token_leftbracket] = "Left Bracket";
    mTokenTypeNames[token_rightbracket] = "Right Bracket";
    mTokenTypeNames[token_comma] = "Comma";
    mTokenTypeNames[token_equal] = "Equal";
    mTokenTypeNames[token_notequal] = "Not Equal";
    mTokenTypeNames[token_greater] = "Greater";
    mTokenTypeNames[token_lesser] = "Lesser";
    mTokenTypeNames[token_greaterequal] = "Greater or Equal";
    mTokenTypeNames[token_lesserequal] = "Lesser or Equal";
    mTokenTypeNames[token_not] = "Not";
    mTokenTypeNames[token_or] = "Or";
    mTokenTypeNames[token_and] = "And";
    mTokenTypeNames[token_xor] = "Xor";
    mTokenTypeNames[token_bwnot] = "Bitwise Not";
    mTokenTypeNames[token_bwor] = "Bitwise Or";
    mTokenTypeNames[token_bwand] = "Bitwise And";
    mTokenTypeNames[token_bwxor] = "Bitwise Xor";
    mTokenTypeNames[token_leftshift] = "Left Shift";
    mTokenTypeNames[token_rightshift] = "Right Shift";
    mTokenTypeNames[token_assignment] = "Assignment";

    // Constant - Numerals
    mTokenTypeNames[token_numeral] = "Numeral";
    mTokenTypeNames[token_literal] = "Literal";

    // Reserved words map
    mReservedWords["void"] = token_void;
    mReservedWords["block"] = token_block;

    mReservedWords["return"] = token_return;
    mReservedWords["if"] = token_if;
    mReservedWords["else"] = token_else;
    mReservedWords["for"] = token_for;
    mReservedWords["in"] = token_in;

    mReservedWords["true"] = token_true;
    mReservedWords["false"] = token_false;

    // Error message map
    mErrorMessages[l_error_syntax] = "Syntax";
  }

  bool isalpha(const char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
  }

  bool isdecdigit(const char c) { return (c >= '0' && c <= '9'); }

  bool ishexdigit(const char c) {
    return isdecdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
  }

  bool readdecnumber(char **pc, token *token) {
    bool rval = true;

    // Token name is set to name - will be changed to reserved if part of the
    // reserved words
    token->mType = token_numeral;

    unsigned int current_char = 0;

    while ((isdecdigit(**pc) || ('.' == **pc)) &&
           (current_char < MAX_TOKEN_SIZE)) {
      token->mValue[current_char] = **pc;
      ++current_char;
      ++(*pc);
    }

    return rval;
  }

  bool readalpha(char **pc, token *token) {
    bool rval = true;

    // Token name is set to name - will be changed to reserved if part of the
    // reserved words
    token->mType = token_name;

    // Store first caracter
    unsigned int current_char = 1;
    token->mValue[0] = **pc;
    ++(*pc);

    while ((isalpha(**pc) || isdecdigit(**pc)) &&
           (current_char < MAX_TOKEN_SIZE)) {
      token->mValue[current_char] = **pc;
      ++current_char;
      ++(*pc);
    }
    token->mValue[current_char] = 0x00;

    // Try to find a reserved word
    auto it = mReservedWords.find(token->mValue);
    if (it != mReservedWords.end()) {
      token->mType = it->second;
    }

    return rval;
  }

  bool readcomment_line(char **pc) {
    bool rval = true;

    // Skip the double slash '//'
    ++(*pc);
    ++(*pc);

    while ('\n' != **pc) {
      ++(*pc);
    }

    return rval;
  }

  bool readoperator(char **pc, token *token) {
    bool rval = true;

    switch (**pc) {
      case '/':
        token->mType = token_slash;
        ++(*pc);
        break;

      case '-':
        token->mType = token_minus;
        ++(*pc);
        break;

      case '+':
        token->mType = token_plus;
        ++(*pc);
        break;

      case '*':
        token->mType = token_star;
        ++(*pc);
        break;

      case '(':
        token->mType = token_leftpar;
        ++(*pc);
        break;

      case ')':
        token->mType = token_rightpar;
        ++(*pc);
        break;

      case '{':
        token->mType = token_leftbrace;
        ++(*pc);
        break;

      case '}':
        token->mType = token_rightbrace;
        ++(*pc);
        break;

      case '[':
        token->mType = token_leftbracket;
        ++(*pc);
        break;

      case ']':
        token->mType = token_rightbracket;
        ++(*pc);
        break;

      case ',':
        token->mType = token_comma;
        ++(*pc);
        break;

      case '~':
        token->mType = token_bwnot;
        ++(*pc);
        break;

      case '%':
        token->mType = token_modulo;
        ++(*pc);
        break;

      case '=':
        token->mType = token_assignment;
        ++(*pc);
        if ('=' == **pc) {
          token->mType = token_equal;
          ++(*pc);
        }
        break;

      case '!':
        token->mType = token_not;
        ++(*pc);
        if ('=' == **pc) {
          token->mType = token_notequal;
          ++(*pc);
        }
        break;

      case '^':
        token->mType = token_bwxor;
        ++(*pc);
        if ('^' == **pc) {
          token->mType = token_xor;
          ++(*pc);
        }
        break;

      case '&':
        token->mType = token_bwand;
        ++(*pc);
        if ('&' == **pc) {
          token->mType = token_and;
          ++(*pc);
        }
        break;

      case '|':
        token->mType = token_bwor;
        ++(*pc);
        if ('|' == **pc) {
          token->mType = token_or;
          ++(*pc);
        }
        break;

      case '>':
        token->mType = token_greater;
        ++(*pc);
        if ('=' == **pc) {
          token->mType = token_greaterequal;
          ++(*pc);
        }
        break;

      case '<':
        token->mType = token_lesser;
        ++(*pc);
        if ('=' == **pc) {
          token->mType = token_lesserequal;
          ++(*pc);
        }
        break;
    }

    return rval;
  }

  bool readliteral(char **pc, token *token, unsigned int charsleft) {
    bool rval = true;

    // Token name is set to name - will be changed to reserved if part of the
    // reserved words
    token->mType = token_literal;

    // Skip first caracter "
    unsigned int current_char = 0;
    ++(*pc);

    const unsigned int limit = MIN(MAX_TOKEN_SIZE, charsleft);
    // std::cout << "Chars Left = " << charsleft << std::endl;
    // std::cout << "Chars Limit = " << limit << std::endl;

    while (('"' != **pc) && (current_char < limit)) {
      // If escaped
      if ('\\' == **pc) {
        ++(*pc);
        if ('t' == **pc) {
          token->mValue[current_char] = '\t';  // Tab
        } else if ('n' == **pc) {
          token->mValue[current_char] = '\n';  // New line
        } else if ('r' == **pc) {
          token->mValue[current_char] = '\r';  // Carriage Return
        } else if ('"' == **pc) {
          token->mValue[current_char] = '"';  // Double quote
        } else if ('\\' == **pc) {
          token->mValue[current_char] = '\\';  // Backslash
        }
      } else {
        token->mValue[current_char] = **pc;
      }

      ++current_char;
      ++(*pc);
    }

    if ('"' != **pc) {
      mState = l_error;
      mError = l_error_syntax;
      rval = false;
    } else {
      // Skip last caracter "
      ++(*pc);
    }

    return rval;
  }

  bool run() {
    if (l_ok == mState) {
      char *pc = mPayload;
      const char *pe = mPayload + mPayloadSize;

      bool need_new_token = true;
      token *current_token;

      mCurrentLineNumber = 1;
      mCurrentCharacterOnLine = 1;

      while (pc < pe) {
        std::cout << *pc;
        if (need_new_token) {
          current_token = new token;
          current_token->mValue[0] = 0x00;
          current_token->mLineNumber = mCurrentLineNumber;
          current_token->mCharacterOnLine = 0;
          need_new_token = false;
        }

        // If a name
        if (isalpha(*pc)) {
          if (readalpha(&pc, current_token)) {
            mTokenList.push_back(std::shared_ptr<token>(current_token));
            need_new_token = true;
          }
        }
        // If a comment
        else if (*pc == '/') {
          // Peek ahead -
          if (*(pc + 1) == '/') {
            readcomment_line(&pc);
          } else {
            if (readoperator(&pc, current_token)) {
              mTokenList.push_back(std::shared_ptr<token>(current_token));
              need_new_token = true;
            }
          }
        }
        // If an operator
        else if ((*pc == '*') || (*pc == '+') || (*pc == '-') || (*pc == '(') ||
                 (*pc == ')') || (*pc == '{') || (*pc == '}') || (*pc == '[') ||
                 (*pc == ']') || (*pc == '<') || (*pc == ',') || (*pc == '|') ||
                 (*pc == '%') || (*pc == '>') || (*pc == '=') || (*pc == '!') ||
                 (*pc == '^') || (*pc == '&')) {
          if (readoperator(&pc, current_token)) {
            mTokenList.push_back(std::shared_ptr<token>(current_token));
            need_new_token = true;
          }
        }
        // If a string literal
        //   else if (*pc == '"') {
        //     if (readliteral(&pc, current_token, pe - pc)) {
        //       mTokenList.push_back(std::shared_ptr<token>(current_token));
        //       need_new_token = true;
        //     } else {
        //       err();
        //     }
        //   }
        // If a number
        else if (isdecdigit(*pc)) {
          if (('0' == *pc) && (('x' == *(pc + 1)) || ('X' == *(pc + 1)))) {
            // TODO HEX
          } else {
            if (readdecnumber(&pc, current_token)) {
              mTokenList.push_back(std::shared_ptr<token>(current_token));
              need_new_token = true;
            } else {
              err();
            }
          }
        } else if (*pc == '\n') {
          ++mCurrentLineNumber;
          ++pc;
        }
        // It's something else
        else {
          // Advance the pointer
          ++pc;
        }
      }
    } else {
      err();
    }
  }

  void output_token_list() {
    std::cout << std::endl;
    std::cout << "TOKEN LIST" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;
    for (auto &a : mTokenList) {
      std::cout << mTokenTypeNames[a->mType] << " : " << a->mValue << std::endl;
    }
  }

  std::list<std::shared_ptr<token>> *get_token_list_pointer() {
    return &mTokenList;
  }
  std::map<TOKEN_TYPE, std::string> *get_token_type_names() {
    return &mTokenTypeNames;
  }

 private:
  char *mPayload;
  unsigned int mPayloadSize;

  LEXER_STATE mState;
  LEXER_ERROR mError;

  unsigned int mCurrentLineNumber;
  unsigned int mCurrentCharacterOnLine;

  std::list<std::shared_ptr<token>> mTokenList;
  std::map<std::string, TOKEN_TYPE> mReservedWords;
  std::map<LEXER_ERROR, std::string> mErrorMessages;

  // This is mostly used for debugging
  std::map<TOKEN_TYPE, std::string> mTokenTypeNames;

  bool err() {
    std::cout << std::endl;
    std::cout << "Error " << mError << std::endl;
  }
};

#endif
