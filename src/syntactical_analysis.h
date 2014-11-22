#ifndef GAURD_SYNTACTICAL_ANALYSIS_H
#define GAURD_SYNTACTICAL_ANALYSIS_H

#include "lexical_analysis.h"

struct evl_statement
{
	enum statement_type {MODULE, WIRE, COMPONENT, ENDMODULE};

	statement_type type;
	evl_tokens tokens;
};
typedef std::list<evl_statement> evl_statements;

struct evl_pin
{
	std::string pin_name;
	int msb;
	int lsb;
};
typedef std::list<evl_pin> evl_pins;

struct evl_component
{
	std::string type;
	std::string name;

	evl_pins comp_pins;
};
typedef std::list<evl_component> evl_components;
typedef std::map<std::string, int> evl_wires;

struct evl_module
{
	std::string name;

	evl_wires c_wires;
	evl_components c_components;
};
typedef std::list<evl_module> evl_modules;

bool move_tokens_to_statement(evl_tokens &statement_tokens, evl_tokens &tokens);
bool group_tokens_into_statements(evl_statements &statements, evl_tokens &tokens);
void display_statements(std::ostream &out, const evl_statements &statements);
bool store_statements_to_file(std::string file_name, const evl_statements &statements);
bool process_module_statements(evl_modules &modules, evl_statement &s);
bool process_wire_statements(evl_wires &wires, evl_statement &s);
bool process_component_statements(evl_components &components, evl_statement &s, const evl_wires &wires);
void display_module(std::ostream &out, const evl_modules &modules);
bool store_module_to_file(std::string file_name, const evl_modules &modules);
bool parse_evl_file(std::string file_name, evl_modules &modules);

#endif



