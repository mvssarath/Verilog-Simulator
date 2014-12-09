#include "netlist_creation.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////SOME FUNCTIONS USED WITH IN THE CODE NOT UNDER ANY CLASS       ////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
std::string make_net_name(std::string wire_name, int i)
{
	assert(i >= 0);
	std::ostringstream oss;
	oss << wire_name << "[" << i << "]";
	return oss.str();
}
void net::append_pin(pin *p)
{
	connections_.push_back(p);
}

std::string net::get_net_name()
{
	return net_name;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////  PIN CLASS FUNCTIONS    /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
bool pin::create(gate *g, size_t pin_index, const evl_pin &p, const std::map<std::string, net *> &netlist_nets) 
{

	gate_ = g;
	pin_index_ = pin_index;

	std::string net_name = p.pin_name;
	if (p.msb == -1) 
	{ 
		std::map<std::string, net *>::const_iterator net = netlist_nets.find(net_name);
		nets_.push_back(net->second);
		net->second->append_pin(this);
	}
	else
	{	
		for (int i = p.lsb; i <= p.msb; ++i) 
		{
		
			std::map<std::string, net *>::const_iterator net = netlist_nets.find(make_net_name(net_name, i));
			nets_.push_back(net->second);
			net->second->append_pin(this);
		}
	}
	return true;
}

void pin::set_as_input()
{
	pin_direction = 1;
}

void pin::set_as_output() 
{
	pin_direction = 0;
}

int pin::get_pin_width()
{
	return nets_.size();
}

net * pin::get_net()
{
	return nets_[0];
}


int pin::get_pin_direction()
{
	return pin_direction;
}

std::string pin::get_type()
{
	return gate_->get_type();
}
void pin::display(std::ostream &out)
{
	out << "pin " << gate_->get_type() << " " << gate_->get_name() << " " << pin_index_ << std::endl;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////     GATE CLASS FUNCTIONS      /////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
bool gate::create_pins(const evl_pins &pins, const std::map<std::string, net*> &netlist_nets)
{
	size_t pin_index = 0;
	for (evl_pins::const_iterator p = pins.begin(); p != pins.end(); ++p)
	{
		create_pin(*p, pin_index, netlist_nets);
		++pin_index;
	}
	return validate_structural_semantics();
}
bool gate::create_pin(const evl_pin &p, size_t pin_index, const std::map<std::string, net *> &netlist_nets)
{
	pins_.push_back(new pin);
	return pins_.back()->create(this, pin_index, p, netlist_nets);
}
gate::~gate()
{
	for (size_t i = 0; i < pins_.size(); ++i) 
	{
		delete pins_[i];
	}
}
std::string gate::get_name() const
{
	return name_;
}
std::string gate::get_type() const 
{
	return type_;
}
void gate::display(std::ostream &out)
{
	out << "gate " << get_type() << " " << get_name() << " " << pins_.size() << std::endl;
	for (std::vector<pin*>::size_type i = 0; i != pins_.size(); ++i) 
	{
		out << "pin " << pins_[i]->get_pin_width();

		for (std::vector<net *>::size_type j = 0; j != pins_[i]->nets_.size(); ++j) 
		{
			out << " " << pins_[i]->nets_[j]->get_net_name();
		}
		out << std::endl;
	}
}
void gate::display_in_gate(std::ostream &out)
{
	out << pins_.size() << " ";
	for (std::vector<pin*>::size_type i = 0; i != pins_.size(); ++i) {
		out << pins_[i]->get_pin_width() << " ";
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////       NETLIST CLASS FUCTIONS    /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

netlist::~netlist()   ///////NETLIST CLASS DESTRUCTOR
{
	for (std::list<gate *>::const_iterator it = gates_.begin(); it != gates_.end(); ++it) 
	{
		delete *it;
	}
	for (std::map<std::string, net *>::const_iterator it2 = nets_.begin(); it2 != nets_.end(); ++it2) 
	{
		delete it2->second;
	}
}
bool netlist::create(const evl_wires &wires, const evl_components &comps) 
{
	return create_nets(wires) && create_gates(comps);
}
bool netlist::create_nets(const evl_wires &wires) 
{
	for (evl_wires::const_iterator it = wires.begin(); it != wires.end(); ++it) 
	{
		if (it->second == 1) 
		{
			create_net(it->first);
		}
		else
		{
			for (int i = 0; i < it->second; ++i) 
			{
				create_net(make_net_name(it->first, i));
			}
		}
	}
	return true;
}
void netlist::create_net(std::string net_name) 
{
	assert(nets_.find(net_name) == nets_.end());
	nets_[net_name] = new net;
	nets_[net_name]->net_name = net_name;
}
bool netlist::create_gates(const evl_components &comps) 
{
	for (evl_components::const_iterator it = comps.begin(); it != comps.end(); ++it) 
	{
		create_gate(*it);
	}
	return true;
}
bool netlist::create_gate(const evl_component &c) 
{
	if (c.type == "and")
	{
		gates_.push_back(new and_gate(c.name));
	}
	else if (c.type == "or") 
	{
		gates_.push_back(new or_gate(c.name));
	}
	else if (c.type == "xor") 
	{
		gates_.push_back(new xor_gate(c.name));
	}
	else if (c.type == "not") 
	{
		gate *g = new not_gate(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "buf") 
	{
		gate *g = new buffer(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "evl_dff") 
	{
		gate *g = new flip_flop(c.name);
		gates_.push_back(g);
	}
	else if (c.type == "evl_one") 
	{
		gates_.push_back(new evl_one(c.name));
	}
	else if (c.type == "evl_zero") 
	{
		gates_.push_back(new evl_zero(c.name));
	}
	else if (c.type == "evl_input") 
	{
		gates_.push_back(new evl_input(c.name));
	}
	else if (c.type == "evl_output") 
	{
		gates_.push_back(new evl_output(c.name));
	}
	else if (c.type == "evl_clock") 
	{
		gates_.push_back(new evl_clock(c.name));
	}
	else if (c.type == "evl_lut")
	{
		gates_.push_back(new evl_lut(c.name));
	}
	else if (c.type == "tris")
	{
		gates_.push_back(new tris(c.name));
	}
	else
	{
		std::cerr << c.name << " " << c.type << " Gate does not exist" << std::endl;
	}

	return gates_.back()->create_pins(c.comp_pins, nets_);
}

void netlist::save(std::string file_name)
{
	std::ofstream output_file(file_name.c_str());
	if (!output_file)
	{
	}
	display_netlist(output_file);
}
void netlist::display_netlist(std::ostream &out)
{
	out << nets_.size() << " " << gates_.size() << std::endl;
	for (std::map<std::string, net *>::const_iterator net_it = nets_.begin(); net_it != nets_.end(); ++net_it) 
	{
		out << "net " << net_it->first << " " << net_it->second->connections_.size() << std::endl;
		for (std::list<pin *>::const_iterator pin_iter = net_it->second->connections_.begin();pin_iter != net_it->second->connections_.end(); pin_iter++)
		{
			(*pin_iter)->display(out);
		}
	}

	for (std::list<gate *>::const_iterator gate_it = gates_.begin(); gate_it != gates_.end(); ++gate_it) 
	{
		(*gate_it)->display(out);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////  STRUCTURAL SEMANTICS OF ALL THE GATES  ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
bool and_gate::validate_structural_semantics() 
{
	if (pins_.size() < 3)
		return false;
	pins_[0]->set_as_output();
	for (size_t i = 1; i < pins_.size(); ++i) 
	{
		pins_[i]->set_as_input();
	}
	return true;
}
bool or_gate::validate_structural_semantics() 
{
	if (pins_.size() < 3)
		return false;
	pins_[0]->set_as_output();
	for (size_t i = 1; i < pins_.size(); ++i) {
		pins_[i]->set_as_input();
	}
	return true;
}
bool xor_gate::validate_structural_semantics() 
{
	if (pins_.size() < 3)
		return false;
	pins_[0]->set_as_output();
	for (size_t i = 1; i < pins_.size(); ++i) 
	{
		pins_[i]->set_as_input();
	}
	return true;
}
bool not_gate::validate_structural_semantics() 
{
	if (pins_.size() != 2) return false;
	pins_[0]->set_as_output();
	pins_[1]->set_as_input();
	return true;
}
bool buffer::validate_structural_semantics() 
{
	if (pins_.size() != 2) return false;
	pins_[0]->set_as_output();
	pins_[1]->set_as_input();
	return true;
}
bool flip_flop::validate_structural_semantics() 
{
	if (pins_.size() != 3) return false;
	pins_[0]->set_as_output();
	pins_[1]->set_as_input();
	pins_[2]->set_as_input();
	return true;
}
bool evl_one::validate_structural_semantics() 
{
	if (pins_.size() < 1)
		return false;
	for (size_t i = 0; i < pins_.size(); ++i) 
	{
		pins_[i]->set_as_output();
	}
	return true;
}
bool evl_zero::validate_structural_semantics() 
{
	if (pins_.size() < 1)
		return false;
	for (size_t i = 0; i < pins_.size(); ++i) 
	{
		pins_[i]->set_as_output();
	}
	return true;
}
bool evl_input::validate_structural_semantics() 
{
	if (pins_.size() < 1)
		return false;
	for (size_t i = 0; i < pins_.size(); ++i)    
	{
		pins_[i]->set_as_output();
	}
	return true;
}
bool evl_output::validate_structural_semantics() 
{
	if (pins_.size() < 1)
		return false;
	for (size_t i = 0; i < pins_.size(); ++i) 
	{
		pins_[i]->set_as_input();
	}
	return true;
}
bool evl_clock::validate_structural_semantics() 
{
	if (pins_.size() != 1)
		return false;
	pins_[0]->set_as_input();
	return true;
}
bool evl_lut::validate_structural_semantics()
{
	if (pins_.size() != 2) return false;
	pins_[0]->set_as_output();
	pins_[1]->set_as_input();
	return true;
}
bool tris::validate_structural_semantics()
{
	if (pins_.size() != 3) return false;
	pins_[0]->set_as_output();
	pins_[1]->set_as_input();
	pins_[2]->set_as_input();
		return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////    CODE ENDS HERE      /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////


