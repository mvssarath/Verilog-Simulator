#include "netlist_creation.h"
//////////////////////////////////FUNCTIONS USED WITH IN THE CODE//////////////////////////////////

int return_int_value(char value)
{
	if (value == '0')
		return 0;
	else
		return 1;
}
int to_hex(size_t i)
{
	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////NET CLASS LOGIC SIMULATION FUNCTIONS/////////////////////////////////
int net::get_logic_value()
{
	return logic_value;
}
bool net::retrieve_logic_value()
{
	if (logic_value==-1)
	{
		for (std::list<pin *>::const_iterator rlv= connections_.begin(); rlv!= connections_.end(); ++rlv)
		{
			if ((*rlv)->get_type() != "evl_output" && (*rlv)->get_pin_direction() == 0)
			{
				logic_value = (*rlv)->get_value();
			}
		}
	}
	if (logic_value == 1)
		return true;
	else
		return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////PIN CLASS LOGIC SIMULATION FUNCTIONS/////////////////////////////////
bool pin::get_value() 
{
	if (pin_direction == 1)
		return get_net()->retrieve_logic_value();
	else
		return gate_->compute();
	return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////GATE CLASS LOGIC SIMULATION FUNCTIONS////////////////////////////////

void gate::display_out_gate(std::ostream &out)
{
	out << pins_.size() << std::endl;
	for (std::vector<pin*>::size_type i = 0; i != pins_.size(); ++i) 
	{
		out<< pins_[i]->get_pin_width()<< std::endl;
	}
}
void gate::display_logic_value(std::ostream &out)
{
	for (std::vector<pin*>::size_type i = 0; i != pins_.size(); ++i) 
	{
		std::string hex;
		int count = 0;
		std::vector<net *>::size_type j = pins_[i]->nets_.size() - 1;
		double width_pin((double)(j + 1) / 4);
		int hex_width = (int)ceil(width_pin);
		for (; j != -1; j--)
		{
			if (pins_[i]->nets_[j]->get_logic_value() == 1) 
			{
				count = (int)(pow(2.0, (int)j) + count);
			}
		}
		out << std::setfill('0') << std::setw(hex_width) << std::uppercase << std::hex << count << " ";
	}
	out << std::endl;
}
bool gate::compute() 
{
	for (size_t i = 0; i < pins_.size(); ++i) 
	{
		if (pins_[i]->pin_direction == 1) 
		{ 
			pins_[i]->get_value();
		}
		else
			return compute_gate();
	}
	return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////NETLIST CLASS LOGIC SIMULATION FUNCTIONS////////////////////////////////

void netlist::simulate(int cycles)
{
	std::ofstream output_file;
	std::string out_file_name = "";
	std::string in_file_name = "";
	std::vector<int> number_of_trans;
	std::vector<int> set;
	std::vector<std::string> in;
	std::vector<std::string> in_bin_vec;

	for (std::list<gate *>::const_iterator gate_find = gates_.begin(); gate_find != gates_.end(); ++gate_find)
	{
		if ((*gate_find)->get_type() == "evl_output")
		{
			out_file_name = file_name + "." + (*gate_find)->get_name() + "." + (*gate_find)->get_type();
			output_file.open(out_file_name.c_str());
		}
	
	}

	display_sim_out(output_file);

	if (!in_bin_vec.empty())
	{
		for (int j = 1; j != number_of_trans.size(); ++j)
		{
			for (int i = number_of_trans[j]; i != 0; --i)
			{
				cycles--;
				for (std::map<std::string, net *>::iterator net_it = nets_.begin(); net_it != nets_.end(); ++net_it)
				{
					net_it->second->logic_value = -1;

					if (net_it->first == "set")
					{
						net_it->second->logic_value = set[j];
					}

					if (net_it->first == "in[0]")
						net_it->second->logic_value = return_int_value(in_bin_vec[j - 1].at(15));
					else if (net_it->first == "in[1]")
						net_it->second->logic_value = return_int_value(in_bin_vec[j - 1].at(14));
					else if (net_it->first == "in[2]")
						net_it->second->logic_value = return_int_value(in_bin_vec[j - 1].at(13));
					else if (net_it->first == "in[3]")
						net_it->second->logic_value = return_int_value(in_bin_vec[j - 1].at(12));
					else if (net_it->first == "in[4]")
						net_it->second->logic_value = return_int_value(in_bin_vec[j - 1].at(11));
					else if (net_it->first == "in[5]")
						net_it->second->logic_value = return_int_value(in_bin_vec[j - 1].at(10));
					else if (net_it->first == "in[6]")
						net_it->second->logic_value = return_int_value(in_bin_vec[j - 1].at(9));
					else if (net_it->first == "in[7]")
						net_it->second->logic_value = return_int_value(in_bin_vec[j - 1].at(8));
					else if (net_it->first == "in[8]")
						net_it->second->logic_value = return_int_value(in_bin_vec[j - 1].at(7));
					else if (net_it->first == "in[9]")
						net_it->second->logic_value = return_int_value(in_bin_vec[j - 1].at(6));
					else if (net_it->first == "in[10]")
						net_it->second->logic_value = return_int_value(in_bin_vec[j - 1].at(5));
					else if (net_it->first == "in[11]")
						net_it->second->logic_value = return_int_value(in_bin_vec[j - 1].at(4));
					else if (net_it->first == "in[12]")
						net_it->second->logic_value = return_int_value(in_bin_vec[j - 1].at(3));
					else if (net_it->first == "in[13]")
						net_it->second->logic_value = return_int_value(in_bin_vec[j - 1].at(2));
					else if (net_it->first == "in[14]")
						net_it->second->logic_value = return_int_value(in_bin_vec[j - 1].at(1));
					else if (net_it->first == "in[15]")
						net_it->second->logic_value = return_int_value(in_bin_vec[j - 1].at(0));

				}

				for (std::map<std::string, net *>::iterator net_it = nets_.begin(); net_it != nets_.end(); ++net_it) 
				{
					net_it->second->retrieve_logic_value();
				}

				for (std::list<gate *>::iterator gate_it = gates_.begin(); gate_it != gates_.end(); ++gate_it) 
				{
					if ((*gate_it)->get_type() == "evl_dff") {
						(*gate_it)->compute_next_state();
					}
				}
				display_sim_out_results(output_file);
			}
		}
		for (int i = 0; i != cycles; ++i)
		{

			for (std::map<std::string, net *>::iterator net_it = nets_.begin(); net_it != nets_.end(); ++net_it) 
			{
				net_it->second->logic_value = -1;

				if (net_it->first == "set") 
				{
					net_it->second->logic_value = set.back();
				}
				if (net_it->first == "in[0]")
					net_it->second->logic_value = return_int_value(in_bin_vec.back().at(15));
				else if (net_it->first == "in[1]")
					net_it->second->logic_value = return_int_value(in_bin_vec.back().at(14));
				else if (net_it->first == "in[2]")
					net_it->second->logic_value = return_int_value(in_bin_vec.back().at(13));
				else if (net_it->first == "in[3]")
					net_it->second->logic_value = return_int_value(in_bin_vec.back().at(12));
				else if (net_it->first == "in[4]")
					net_it->second->logic_value = return_int_value(in_bin_vec.back().at(11));
				else if (net_it->first == "in[5]")
					net_it->second->logic_value = return_int_value(in_bin_vec.back().at(10));
				else if (net_it->first == "in[6]")
					net_it->second->logic_value = return_int_value(in_bin_vec.back().at(9));
				else if (net_it->first == "in[7]")
					net_it->second->logic_value = return_int_value(in_bin_vec.back().at(8));
				else if (net_it->first == "in[8]")
					net_it->second->logic_value = return_int_value(in_bin_vec.back().at(7));
				else if (net_it->first == "in[9]")
					net_it->second->logic_value = return_int_value(in_bin_vec.back().at(6));
				else if (net_it->first == "in[10]")
					net_it->second->logic_value = return_int_value(in_bin_vec.back().at(5));
				else if (net_it->first == "in[11]")
					net_it->second->logic_value = return_int_value(in_bin_vec.back().at(4));
				else if (net_it->first == "in[12]")
					net_it->second->logic_value = return_int_value(in_bin_vec.back().at(3));
				else if (net_it->first == "in[13]")
					net_it->second->logic_value = return_int_value(in_bin_vec.back().at(2));
				else if (net_it->first == "in[14]")
					net_it->second->logic_value = return_int_value(in_bin_vec.back().at(1));
				else if (net_it->first == "in[15]")
					net_it->second->logic_value = return_int_value(in_bin_vec.back().at(0));
			}
			for (std::map<std::string, net *>::iterator net_it = nets_.begin(); net_it != nets_.end(); ++net_it)
			{
				net_it->second->retrieve_logic_value();
			}

			for (std::list<gate *>::iterator gate_it = gates_.begin(); gate_it != gates_.end(); ++gate_it) 
			{
				if ((*gate_it)->get_type() == "evl_dff") 
				{
					(*gate_it)->compute_next_state();
				}
			}
			display_sim_out_results(output_file);
		}
	}
	else 
	{
		for (int i = 0; i != cycles; ++i)
		{
			for (std::map<std::string, net *>::iterator net_it = nets_.begin(); net_it != nets_.end(); ++net_it) 
			{
				net_it->second->logic_value = -1;
			}
			for (std::map<std::string, net *>::iterator net_it = nets_.begin(); net_it != nets_.end(); ++net_it) 
			{
				net_it->second->retrieve_logic_value();
			}
			for (std::list<gate *>::iterator gate_it = gates_.begin(); gate_it != gates_.end(); ++gate_it) 
			{
				if ((*gate_it)->get_type() == "evl_dff") 
				{
					(*gate_it)->compute_next_state();
				}
			}
			display_sim_out_results(output_file);
		}
	}
	output_file.close();
}
void netlist::display_sim_out(std::ostream &out) 
{
	for (std::list<gate *>::const_iterator output_gate = gates_.begin(); output_gate != gates_.end(); ++output_gate) 
	{
		if ((*output_gate)->get_type() == "evl_output") 
		{
			(*output_gate)->display_out_gate(out);
		}
	}
}
void netlist::display_sim_out_results(std::ostream &out) 
{
	for (std::list<gate *>::const_iterator output_gate = gates_.begin(); output_gate != gates_.end(); ++output_gate) {
		if ((*output_gate)->get_type() == "evl_output") {
			(*output_gate)->display_logic_value(out);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////GATE CLASS LOGIC SIMULATION COMPUTING FOR DIFFERENT GATES////////////


bool and_gate::compute_gate() 
{
	for (size_t i = 0; i < pins_.size(); ++i) 
	{
		if (pins_[i]->get_pin_direction() == 1)
		{
			for (size_t j = 0; j < (size_t)pins_[i]->get_pin_width(); ++j) 
			{
				if (!pins_[i]->get_net()->retrieve_logic_value())
					return false;
			}
		}
	}
	return true;
}

void and_gate::compute_next_state(){}


bool or_gate::compute_gate()
{

	for (size_t i = 0; i < pins_.size(); ++i) 
	{
		if (pins_[i]->get_pin_direction() == 1)
		{
			for (size_t j = 0; j < (size_t)pins_[i]->get_pin_width(); ++j) 
			{
				if (pins_[i]->get_net()->retrieve_logic_value())
					return true;
			}
		}
	}
	return false;
}

void or_gate::compute_next_state(){}

bool xor_gate::compute_gate()
{
	int num_ones = 0;
	for (size_t i = 0; i < pins_.size(); ++i) 
	{
		if (pins_[i]->get_pin_direction() == 1)
		{
			for (size_t j = 0; j < (size_t)pins_[i]->get_pin_width(); ++j) 
			{
				if (pins_[i]->get_net()->retrieve_logic_value())	
					num_ones++;
			}
		}
	}

	if (num_ones % 2 != 0)
		return true;
	else
		return false;
}

void xor_gate::compute_next_state(){}


bool not_gate::compute_gate()
{

	for (size_t i = 0; i < pins_.size(); ++i)
	{
		if (pins_[i]->get_pin_direction() == 1)
		{
			for (size_t j = 0; j < (size_t)pins_[i]->get_pin_width(); ++j) 
			{
				if (!pins_[i]->get_net()->retrieve_logic_value())
					return true;
				else if (pins_[i]->get_net()->retrieve_logic_value())
					return false;
			}
		}
	}
	return false;
}

void not_gate::compute_next_state(){}


bool buffer::compute_gate()
{
	for (size_t i = 0; i < pins_.size(); ++i) 
	{
		if (pins_[i]->get_pin_direction() == 1)
		{
			for (size_t j = 0; j < (size_t)pins_[i]->get_pin_width(); ++j) 
			{
				if (!pins_[i]->get_net()->retrieve_logic_value())
					return false;
				else if (pins_[i]->get_net()->retrieve_logic_value())
					return true;
			}
		}
	}
	return false;
}

void buffer::compute_next_state(){}

bool flip_flop::compute_gate()
{
	return next_state_;
}

void flip_flop::compute_next_state() 
{
	net *input_net = pins_[1]->get_net();
	next_state_ = input_net->retrieve_logic_value();
}


bool evl_one::compute_gate()
{
	return true;
}

void evl_one::compute_next_state(){}


bool evl_zero::compute_gate()
{
	return false;
}

void evl_zero::compute_next_state(){}

bool evl_input::compute_gate()
{
	return true;
}
void evl_input::compute_next_state(){}

bool evl_output::compute_gate()
{
	return true;
}

void evl_output::compute_next_state(){}

bool evl_clock::compute_gate()
{
	return true;
}
void evl_clock::compute_next_state(){}
