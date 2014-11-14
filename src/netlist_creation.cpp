//////////////////////////////////Functions for project#3/////////////////////////////////////////////////////////////
#include "netlist_creation.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////FUNCTION TO CREATE NET NAME//////////////////////////////////////////////////////////
std::string make_net_name(std::string wire_name, int i){
	assert(i >= 0);
	std::ostringstream oss;
	oss << wire_name << "[" << i << "]";
	return oss.str();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////GATE CLASS FUNCTIONS/////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool gate::create(const evl_component &component, const std::map <std::string, net *> &nets_table, const evl_wires_table &wires_table)
{
	gate_type = component.type;
	gate_name = component.name;
	size_t pin_index = 0;
	for (evl_pins::const_iterator it = component.pins.begin(); it != component.pins.end(); ++it)
	{
		create_pin(*it, pin_index, nets_table, wires_table);
		++pin_index;
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool gate::create_pin(const evl_pin &ep, size_t pin_index, const std::map<std::string, net *> &nets_table, const evl_wires_table &wires_table)
{
	pin *p = new pin;
	pins_.push_back(p);
	return p->create(this, pin_index, ep, nets_table, wires_table);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////NETLIST CLASS FUNCTIONS///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool netlist::create(const evl_wires &wires, const evl_components &components, const evl_wires_table &wires_table)
{
	return create_nets(wires) && create_gates(components, wires_table);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool netlist::create_nets(const evl_wires &wires)
{
	for (evl_wires::const_iterator it = wires.begin(); it != wires.end(); it++)
	{
		if (it->width == 1)
		{
			create_net(it->name);
		}
		else
		{
			for (int i = 0; i < (it->width); ++i)
			{
				create_net(make_net_name(it->name, i));
			}
		}
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool netlist::create_gates(const evl_components &components, const evl_wires_table &wires_table)
{
	for (evl_components::const_iterator itr = components.begin(); itr != components.end(); ++itr)
	{
		create_gate(*itr, wires_table);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void netlist::create_net(std::string net_name)
{
	assert(nets_table_.find(net_name) == nets_table_.end());
	net *n = new net;
	(*n).nname = net_name;
	nets_table_[net_name] = n;
	nets_.push_back(n);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool netlist::create_gate(const evl_component &component, const evl_wires_table &wires_table)
{
	gate *g = new gate;
	gates_.push_back(g);
	return g->create(component, nets_table_, wires_table);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////NET CLASS FUNCTIONS///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void net::append_pin(pin *p)
{
	connections_.push_back(p);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////PIN CLASS FUNCTIONS///////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool pin::create(gate *g, size_t pin_index, const evl_pin &p, const std::map<std::string, net *> &nets_table, const evl_wires_table &wires_table)
{
	pin_index_ = pin_index;
	gate_ = g;
	pinmsb = p.bus_msb;
	pinlsb = p.bus_lsb;
    netname = p.name;

    evl_wires_table::const_iterator itrwire = wires_table.find(netname);
	if ((p.bus_msb == -1) && (p.bus_lsb == -1))
	{ 
		if(itrwire->second == 1)
		{		
			width = itrwire->second;
			net *netptr = new net;
			std::map<std::string, net *>::const_iterator nnameitr = nets_table.find(netname);
			netptr = nnameitr->second;
			nets_.push_back(netptr);
			netptr->append_pin(this);
		}
		else
		{
			width = itrwire->second;
			for(int i=0; i<itrwire->second; ++i)
			{
				net *netptr = new net;
				(*netptr).nname = make_net_name(netname, i);
				std::map<std::string, net *>::const_iterator nnameitr = nets_table.find((*netptr).nname);
				netptr = nnameitr->second;
				nets_.push_back(netptr);
				netptr->append_pin(this);
			}
		}
	}
	else if ((p.bus_lsb != -1) && (p.bus_msb != -1))
	{  
        width = pinmsb - pinlsb + 1;
        for(int i=pinlsb; i<=pinmsb; ++i)
		{
            net *netptr = new net;
            (*netptr).nname = make_net_name(netname, i);
            std::map<std::string, net *>::const_iterator nnameitr = nets_table.find((*netptr).nname);
            netptr = nnameitr->second;
            nets_.push_back(netptr);
            netptr->append_pin(this);
        }
	}
	else if ((p.bus_msb != -1) && (p.bus_lsb == -1))
	{
		width = 1;
        net *netptr = new net;
		(*netptr).nname = make_net_name(netname, p.bus_msb);
        std::map<std::string, net *>::const_iterator nnameitr = nets_table.find((*netptr).nname);
        netptr = nnameitr->second;
        nets_.push_back(netptr);
        netptr->append_pin(this);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////DISPLAYING THE RESULTS AND SAVING IT TO A FILE////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void netlist::display_netlist(std::ostream &out, const evl_modules &modules)
{
	for (evl_modules::const_iterator it = modules.begin(); it != modules.end(); ++it)
	{
		out << "module" << " " << it->name << " " << std::endl;
	}
	out << "nets " << nets_.size() << std::endl;
	for (std::list<net *>::const_iterator itnets = nets_.begin(); itnets != nets_.end(); ++itnets)
	{
		out << "  net " << (*itnets)->nname << " " << (*itnets)->connections_.size() << std::endl;
		for (std::list<pin *>::const_iterator itpins = (*itnets)->connections_.begin(); itpins != (*itnets)->connections_.end(); ++itpins)
		{
			if ((*itpins)->gate_->gate_name == "")
			{
				out << "    " << (*itpins)->gate_->gate_type << " " << (*itpins)->pin_index_ << std::endl;
			}
			else
			{
				out << "    " << (*itpins)->gate_->gate_type << " " << (*itpins)->gate_->gate_name << " " << (*itpins)->pin_index_ << std::endl;
			}
		}
	}

	out << "components " << gates_.size() << std::endl;
	for (std::list<gate *>::const_iterator itgts = gates_.begin(); itgts != gates_.end(); ++itgts)
	{
		if ((*itgts)->gate_name == "")
		{
			out << "  component " << (*itgts)->gate_type << " " << (*itgts)->pins_.size() << std::endl;
		}
		else
		{
			out << "  component " << (*itgts)->gate_type << " " << (*itgts)->gate_name << " " << (*itgts)->pins_.size() << std::endl;
		}
		for (std::vector<pin *>::const_iterator itrpins = (*itgts)->pins_.begin(); itrpins != (*itgts)->pins_.end(); ++itrpins)
		{

			out << "    pin " << (*itrpins)->width;
			for (std::vector <net *>::const_iterator itrnets = (*itrpins)->nets_.begin(); itrnets != (*itrpins)->nets_.end(); ++itrnets)
			{
				out << " " << (*itrnets)->nname;
			}
			out << std::endl;
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void netlist::save(std::string file_name, const evl_modules &modules)
{
	std::ofstream output_file(file_name.c_str());
	if (!output_file)
	{
		std::cerr << "I cant write" << output_file << std::endl;
	}
	display_netlist(output_file, modules);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

