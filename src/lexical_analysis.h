#ifndef GAURD_LEXICAL_ANALYSIS_H
#define GAURD_LEXICAL_ANALYSIS_H
#include <assert.h>
#include <algorithm>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
struct evl_token
{
  enum token_type{NAME,NUMBER,SINGLE};
  token_type type;
  std::string str;
  int LineNo;
};
typedef std::list<evl_token> evl_tokens;

bool extract_tokens_from_file(std::string file_name, evl_tokens &tokens);
bool store_tokens_to_file(std::string file_name, const evl_tokens &tokens);

#endif
