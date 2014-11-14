#ifndef GAURD_NETLIST_CREATION_H
#define GAURD_NETLIST_CREATION_H
#include "syntactical_analysis.h"

class netlist;
class gate;
class net;
class pin;


class net{
public:
    std::string nname;
	std::list <pin *> connections_;
	std::map <std::string, net *> nets_table_;
	void append_pin(pin *);
}; 

class pin{
public:
    std::string netname;
    int pinmsb, pinlsb, width;
	gate *gate_;
	size_t pin_index_;
	std::vector <net *> nets_;
	bool create(gate *g, size_t pin_index, const evl_pin &p, const std::map<std::string, net *> &nets_table, const evl_wires_table &wires_table);
}; 

class gate{
public:
	std::string gate_type, gate_name;
	std::vector <pin *> pins_;
	typedef std::map <std::string, std::string> gates_table;
	gates_table gatespredef;
	bool validate_structural_semantics();
	bool create(const evl_component &component, const std::map <std::string, net *> &nets_table_, const evl_wires_table &wires_table);
	bool create_pin(const evl_pin &ep, size_t pin_index, const std::map<std::string, net *> &nets_table, const evl_wires_table &wires_table);
	bool validate_structural_semantics(std::string &gate_type, std::string &gate_name, const evl_pins &pins, gates_table &gatespredef);
}; 

class netlist{
public:
	std::list <gate *> gates_;
	std::list <net *> nets_;
	std::string net_name;
	std::map <std::string, net *> nets_table_;
	std::map <std::string, std::string> gates_table;

	bool create(const evl_wires &wires, const evl_components &components, const evl_wires_table &wires_table);
	void display_netlist(std::ostream &out, const evl_modules &modules);
	void save(std::string file_name, const evl_modules &modules);

private:
	void create_net(std::string net_name);
	bool create_nets(const evl_wires &wires);
	bool create_gate(const evl_component &component, const evl_wires_table &wires_table);
	bool create_gates(const evl_components &components, const evl_wires_table &wires_table);
}; 
std::string make_net_name(std::string wire_name, int i);
#endif

