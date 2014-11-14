#ifndef GAURD_SYNTACTICAL_ANALYSIS_H
#define GAURD_SYNTACTICAL_ANALYSIS_H

#include "lexical_analysis.h"
struct evl_statement
{
	enum statement_type { MODULE, WIRE, COMPONENT, ENDMODULE };
	statement_type type;
	evl_tokens tokens;
};

typedef std::list<evl_statement> evl_statements;


struct evl_wire
{
  std::string name;
  int width;
}; // Structure evl_wire

typedef std::list<evl_wire> evl_wires;
typedef std::map<std::string, int> evl_wires_table;

struct evl_pin
{
  std::string name;
  int bus_msb, bus_lsb;
}; // struct evl_pin

typedef std::list<evl_pin> evl_pins;

struct evl_component
{
  int NoPins;
  std::string type,name;
  evl_pins pins;

}; // struct evl_component

typedef std::list<evl_component>evl_components;

struct evl_module
{
  	std::string name;
	//evl_wires_table mod_wires_table;
	//evl_components mod_components;
};
typedef std::list<evl_module>evl_modules;


bool process_wire_statement(evl_wires &wires, evl_statement &s);
void display_wires(std::ostream &out,const evl_wires &wires );
bool process_component_statement(evl_components &components,evl_statement &s);
void display_components(std::ostream &out,const evl_components &comps );
bool process_module_statement(evl_modules &modules,evl_statement &s);
void display_modules(std::ostream &out, const evl_modules &modules,const evl_wires &wires,const evl_components &comps );

bool group_tokens_into_statements(evl_statements &statements, evl_tokens &tokens);
void display_statements(std::ostream &out, const evl_statements &statements);
bool store_statements_to_file(std::string file_name, const evl_statements &statements);
bool move_tokens_to_statement(evl_tokens &statement_tokens, evl_tokens &tokens);
bool token_is_semicolon(const evl_token &token);
evl_wires_table make_wires_table(const evl_wires &wires);
void  display_wires_table(std::ostream &out, const evl_wires_table &wires_table);

#endif
