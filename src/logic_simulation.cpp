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
	std::string lut_file_name = "";
	std::string out_file_name = "";
	std::string in_file_name = "";
	std::vector<int> number_of_trans;
	std::vector<int> set;
	std::vector<std::string> lut_vec;
	std::vector<std::string> lut_bin_vec;
	std::vector<std::string> in;
	std::vector<std::string> in_bin_vec;

	for (std::list<gate *>::const_iterator gate_find = gates_.begin(); gate_find != gates_.end(); ++gate_find)
	{
		if ((*gate_find)->get_type() == "evl_output")
		{
			out_file_name = file_name + "." + (*gate_find)->get_name() + "." + (*gate_find)->get_type();
			output_file.open(out_file_name.c_str());
		}
		if ((*gate_find)->get_type() == "evl_input")
		{
			in_file_name = netlist::file_name + "." + (*gate_find)->get_name() + "." + (*gate_find)->get_type();
			std::ifstream input_file((char *)in_file_name.c_str());
			std::string line;

			for (; getline(input_file, line);){
				std::istringstream ss(line);

				int var1, var2;
				std::string var3;

				ss >> var1 >> var2 >> var3;

				number_of_trans.push_back(var1);
				set.push_back(var2);
				in.push_back(var3);

			}
			std::string in_bin;

			for (int i = 1; i != in.size(); ++i)
			{
				if (in[i] == "0") 
				{
					in_bin = "0000000000000000";
				}
				else {
					for (int j = 0; j != 4; ++j) 
					{
						switch (in[i].at(j))
						{
						case '0': in_bin.append("0000"); break;
						case '1': in_bin.append("0001"); break;
						case '2': in_bin.append("0010"); break;
						case '3': in_bin.append("0011"); break;
						case '4': in_bin.append("0100"); break;
						case '5': in_bin.append("0101"); break;
						case '6': in_bin.append("0110"); break;
						case '7': in_bin.append("0111"); break;
						case '8': in_bin.append("1000"); break;
						case '9': in_bin.append("1001"); break;
						case 'a': in_bin.append("1010"); break;
						case 'b': in_bin.append("1011"); break;
						case 'c': in_bin.append("1100"); break;
						case 'd': in_bin.append("1101"); break;
						case 'e': in_bin.append("1110"); break;
						case 'f': in_bin.append("1111"); break;
						}
					}
				}
				in_bin_vec.push_back(in_bin);
				in_bin = "";
			}

		}
		if ((*gate_find)->get_type() == "evl_lut") {
			lut_file_name = netlist::file_name + "." + (*gate_find)->get_name()+ "." + (*gate_find)->get_type();
			std::ifstream lut_file((char *)lut_file_name.c_str());
			std::string line;

			for (int line_no = 0; getline(lut_file, line); line_no++){
				std::string word, addr;
				std::istringstream ss(line);
				if (line_no != 0) {
					ss >> word;

					lut_vec.push_back(word);
				}
			}
			std::string lut_bin;

			for (int i = 0; i != lut_vec.size(); ++i) {
				for (int j = 0; j != 4; ++j) {
					switch (lut_vec[i].at(j)) {
					case '0': lut_bin.append("0000"); break;
					case '1': lut_bin.append("0001"); break;
					case '2': lut_bin.append("0010"); break;
					case '3': lut_bin.append("0011"); break;
					case '4': lut_bin.append("0100"); break;
					case '5': lut_bin.append("0101"); break;
					case '6': lut_bin.append("0110"); break;
					case '7': lut_bin.append("0111"); break;
					case '8': lut_bin.append("1000"); break;
					case '9': lut_bin.append("1001"); break;
					case 'a': lut_bin.append("1010"); break;
					case 'b': lut_bin.append("1011"); break;
					case 'c': lut_bin.append("1100"); break;
					case 'd': lut_bin.append("1101"); break;
					case 'e': lut_bin.append("1110"); break;
					case 'f': lut_bin.append("1111"); break;
					}
				}
				lut_bin_vec.push_back(lut_bin);
				lut_bin = "";
			}
		}
	
	}
	for (size_t i = 0; i<lut_vec.size(); ++i) {
		std::cout << "Addr: " << std::hex << i << " Word: " << lut_vec[i] << std::endl;
	}

	for (size_t i = 0; i<lut_vec.size(); ++i) {
		std::cout << "Addr: " << std::hex << i << " Word: " << lut_bin_vec[i] << std::endl;
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
			std::string addr;
			for (std::map<std::string, net *>::iterator net_it = nets_.begin(); net_it != nets_.end(); ++net_it)
			{
				if (net_it->first == "a[4]"){
					int temp = net_it->second->retrieve_logic_value();
					std::stringstream ss;
					ss << temp;
					addr.append(ss.str());
				}
				if (net_it->first == "a[5]"){
					int temp = net_it->second->retrieve_logic_value();
					std::stringstream ss;
					ss << temp;
					addr.append(ss.str());
				}
				if (net_it->first == "a[6]"){
					int temp = net_it->second->retrieve_logic_value();
					std::stringstream ss;
					ss << temp;
					addr.append(ss.str());
				}
				if (net_it->first == "a[7]") {
					int temp = net_it->second->retrieve_logic_value();
					std::stringstream ss;
					ss << temp;
					addr.append(ss.str());
				}
			}
			int x = 0;
			if (addr == "0000") { x = 0; }
			else if (addr == "0001") { x = 1; }
			else if (addr == "0010") { x = 2; }
			else if (addr == "0011") { x = 3; }
			else if (addr == "0100") { x = 4; }
			else if (addr == "0101") { x = 5; }
			else if (addr == "0110") { x = 6; }
			else if (addr == "0111") { x = 7; }
			else if (addr == "1000") { x = 8; }
			else if (addr == "1001") { x = 9; }
			else if (addr == "1010") { x = 10; }
			else if (addr == "1011") { x = 11; }
			else if (addr == "1100") { x = 12; }
			else if (addr == "1101") { x = 13; }
			else if (addr == "1110") { x = 14; }
			else if (addr == "1111") { x = 15; }

			for (std::map<std::string, net *>::iterator net_it = nets_.begin(); net_it != nets_.end(); ++net_it) 
			{
				net_it->second->logic_value = -1;
				if (net_it->first == "word[0]")
					net_it->second->logic_value = return_int_value(lut_bin_vec[x].at(15));
				else if (net_it->first == "word[1]")
					net_it->second->logic_value = return_int_value(lut_bin_vec[x].at(14));
				else if (net_it->first == "word[2]")
					net_it->second->logic_value = return_int_value(lut_bin_vec[x].at(13));
				else if (net_it->first == "word[3]")
					net_it->second->logic_value = return_int_value(lut_bin_vec[x].at(12));
				else if (net_it->first == "word[4]")
					net_it->second->logic_value = return_int_value(lut_bin_vec[x].at(11));
				else if (net_it->first == "word[5]")
					net_it->second->logic_value = return_int_value(lut_bin_vec[x].at(10));
				else if (net_it->first == "word[6]")
					net_it->second->logic_value = return_int_value(lut_bin_vec[x].at(9));
				else if (net_it->first == "word[7]")
					net_it->second->logic_value = return_int_value(lut_bin_vec[x].at(8));
				else if (net_it->first == "word[8]")
					net_it->second->logic_value = return_int_value(lut_bin_vec[x].at(7));
				else if (net_it->first == "word[9]")
					net_it->second->logic_value = return_int_value(lut_bin_vec[x].at(6));
				else if (net_it->first == "word[10]")
					net_it->second->logic_value = return_int_value(lut_bin_vec[x].at(5));
				else if (net_it->first == "word[11]")
					net_it->second->logic_value = return_int_value(lut_bin_vec[x].at(4));
				else if (net_it->first == "word[12]")
					net_it->second->logic_value = return_int_value(lut_bin_vec[x].at(3));
				else if (net_it->first == "word[13]")
					net_it->second->logic_value = return_int_value(lut_bin_vec[x].at(2));
				else if (net_it->first == "word[14]")
					net_it->second->logic_value = return_int_value(lut_bin_vec[x].at(1));
				else if (net_it->first == "word[15]")
					net_it->second->logic_value = return_int_value(lut_bin_vec[x].at(0));
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

bool evl_lut::compute_gate()
{

	return true;
}
void evl_lut::compute_next_state(){}


bool tris::compute_gate()
{
	return true;
}

void tris::compute_next_state() {}




