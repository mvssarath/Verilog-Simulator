#include "lexical_analysis.h"
bool issingle(char ch)
{
	return (ch == '(') || (ch == ')') || (ch == '[')
		|| (ch == ']') || (ch == ':') || (ch == ';')
		|| (ch == ',');
}
bool isname(char ch)
{
	return isalpha(ch) || (ch == '_') || (ch == '\\') || (ch == '.');
}
bool extract_tokens_from_line(std::string line, int line_no, evl_tokens &tokens) 
{
	for (size_t i = 0; i < line.size();) 
	{
	
		if (line[i] == '/')
		{
			++i;
			if ((i == line.size()) || (line[i] != '/'))
			{
				std::cerr << "LINE " << line_no << ": a single / is not allowed" << std::endl;
				return false;
			}
			break;
		}

		else if (isspace(line[i]))
		{
			++i;
			continue; 
		}
		else if (issingle(line[i]))
		{
			evl_token token;
			token.line_no = line_no;
			token.type = evl_token::SINGLE;
			token.str = std::string(1, line[i]);
			tokens.push_back(token);
			++i;										
			continue;									
		}
		else if (isname(line[i])) 
		{
			size_t name_begin = i;
			for (++i; i < line.size(); ++i)
			{
				if (!(isname(line[i]) || isdigit(line[i]))) 
				{
					break;
				}
			}
			evl_token token;
			token.line_no = line_no;
			token.type = evl_token::NAME;
			token.str = std::string(line.substr(name_begin, i - name_begin));
			tokens.push_back(token);
		}
		else if (isdigit(line[i]))
		{
			size_t number_begin = i;
			for (++i; i < line.size(); ++i)
			{
				if (isname(line[i]))
				{
					std::cerr << "LINE " << line_no << ": invalid character" << std::endl;
					return false;
				}
				else if (issingle(line[i]) || !((line[i]) || isname(line[i])))
				{
					break;
				}
			}
			evl_token token;
			token.line_no = line_no;
			token.type = evl_token::NUMBER;
			token.str = std::string(line.substr(number_begin, i - number_begin));
			tokens.push_back(token);
		}
		else
		{
			std::cerr << "LINE " << line_no << ": invalid character" << std::endl;
			return false;
		}
	}
	return true;
}
bool extract_tokens_from_file(std::string file_name, evl_tokens &tokens) 
{ 
	std::ifstream input_file(file_name.c_str());
	if (!input_file)
	{
		std::cerr << "I can't read " << file_name << "." << std::endl;
		return false;
	}
	tokens.clear();
	std::string line;
	for (int line_no = 1; std::getline(input_file, line); ++line_no)
	{
		if (!extract_tokens_from_line(line, line_no, tokens)) 
		{
			return false;
		}
	}
	return true;
}
void display_tokens(std::ostream &out, const evl_tokens &tokens) 
{
	for (evl_tokens::const_iterator iter = tokens.begin(); iter != tokens.end(); ++iter)
	{
		if (iter->type == evl_token::SINGLE)
		{
			out << "SINGLE " << iter->str << std::endl;
		}
		else if (iter->type == evl_token::NAME)
		{
			out << "NAME " << iter->str << std::endl;
		}
		else if (iter->type == evl_token::NUMBER)
		{
			out << "NUMBER " << iter->str << std::endl;
		}
		else {
			out << "LINE " << iter->line_no << ": invalid character" << std::endl;

		}
	}


}
bool store_tokens_to_file(std::string file_name, const evl_tokens &tokens) 
{
	std::ofstream output_file(file_name.c_str());
	if (!output_file)
	{
//		std::cerr << "I can't write" << output_file << "." << std::endl;
		return false;
	}
	display_tokens(output_file, tokens);
	return true;
}

