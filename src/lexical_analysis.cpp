#include "lexical_analysis.h"

bool extract_tokens_from_line(std::string line,int LineNo, evl_tokens &tokens)
{
	for (size_t i = 0; i < line.size();)
	{
		if (line[i] == '/')
		{
			++i;
			if ((i == line.size()) || (line[i] != '/'))
			{
				std::cerr << "LINE " << LineNo	<< ": a single / is not allowed" << std::endl;
				return false;
			}

			break;
		}
		else if (isspace(line[i]))
		{
		++i;
		}
		else if ((line[i] == '(') || (line[i] == ')')
			|| (line[i] == '[') || (line[i] == ']')
			|| (line[i] == ':') || (line[i] == ';')
			|| (line[i] == ','))
		{

			evl_token token;
			token.LineNo=LineNo;
			token.type=evl_token::SINGLE;
			token.str=std::string(1, line[i]);
			tokens.push_back(token);
			++i;
		}
		else if (isalpha(line[i]) || (line[i] == '_')
			|| (line[i] == '\\') || (line[i] == '.'))
		{
			size_t name_begin = i;
			for (++i; i < line.size(); ++i)
			{
				if (!(isalpha(line[i]) || isdigit(line[i])|| (line[i] == '_')	|| (line[i] == '\\') || (line[i] == '.')))

				{

					break;

				}

			}
			evl_token token;
			token.LineNo=LineNo;
			token.type=evl_token::NAME;
			token.str=line.substr(name_begin, i-name_begin);
			tokens.push_back(token);

		}
		else if (isdigit(line[i]))
		{
			size_t number_begin=i;
			for (++i; i<line.size();++i)
			{
				if(!isdigit(line[i]))
				{
					break; 	// [number_begin, i) is
				}
			}
			evl_token token;
			token.LineNo=LineNo;
			token.type=evl_token::NUMBER;
			token.str=line.substr(number_begin,i-number_begin);
			tokens.push_back(token);
		}
		else
		{
			std::cerr << "LINE " << LineNo	<< ": invalid character" << std::endl;
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
	for (int LineNo = 1; std::getline(input_file, line); ++LineNo)
	{
		if (!extract_tokens_from_line(line, LineNo, tokens))
		{
			return false;
		}
	}
	return true;
}

bool store_tokens_to_file(std::string file_name, const evl_tokens &tokens)


{
		std::ofstream output_file(file_name.c_str());
		if (!output_file)
		{
			std::cerr << "I can't write into file " <<file_name << "."<< std::endl;
			return false;
		}

		return true;
}

bool token_is_semicolon(const evl_token &token)

{
	if(token.str == ";")
		return true;
	else
		return false;
}
