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
#include <math.h>
#include <ctype.h>
#include <iterator>
#include <iomanip>
struct evl_token
{
	enum token_type { NAME, NUMBER, SINGLE };

	token_type type;
	std::string str;
	int line_no;
};
typedef std::list<evl_token> evl_tokens;

bool issingle(char ch);
bool isname(char ch);
bool extract_tokens_from_line(std::string line, int line_no, evl_tokens &tokens);
bool extract_tokens_from_file(std::string file_name, evl_tokens &tokens);
void display_tokens(std::ostream &out, const evl_tokens &tokens);
bool store_tokens_to_file(std::string file_name, const evl_tokens &tokens);
#endif
