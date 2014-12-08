#include "syntactical_analysis.h"
bool move_tokens_to_statement(evl_tokens &statement_tokens, evl_tokens &tokens)
{
	assert(statement_tokens.empty());
	assert(!tokens.empty());

	for (; !tokens.empty();) 
	{
		statement_tokens.push_back(tokens.front());
		tokens.erase(tokens.begin());
		if (statement_tokens.back().str == ";")
			break;		
	}
	if (statement_tokens.back().str != ";") 
	{
		std::cerr << "Look for ';' but reach the end of file" << std::endl;
		return false;
	}
	return true;
}
bool group_tokens_into_statements(evl_statements &statements, evl_tokens &tokens) 
{
	assert(statements.empty());

	for (; !tokens.empty();)
	{	
		evl_token token = tokens.front();

		if (token.type != evl_token::NAME)
		{
			std::cerr << "Need a NAME token but found '" << token.str << "' on line" << token.line_no << std::endl;
			return false;
		}

		if (token.str == "module") 
		{				
			evl_statement module;
			module.type = evl_statement::MODULE;

			if (!move_tokens_to_statement(module.tokens, tokens))
			{
				return false;
			}
			statements.push_back(module);
		}
		else if (token.str == "endmodule") 
		{	
			evl_statement endmodule;
			endmodule.type = evl_statement::ENDMODULE;
			endmodule.tokens.push_back(token);

			tokens.erase(tokens.begin());
			statements.push_back(endmodule);
		}
		else if (token.str == "wire")
		{			
			evl_statement wire;
			wire.type = evl_statement::WIRE;

			if (!move_tokens_to_statement(wire.tokens, tokens))
			{
				return false;
			}
			statements.push_back(wire);
		}
		else 
		{									
			evl_statement component;
			component.type = evl_statement::COMPONENT;

			if (!move_tokens_to_statement(component.tokens, tokens))
			{
				return false;
			}
			statements.push_back(component);
		}
	}
	return true;
}

void display_statements(std::ostream &out, const evl_statements &statements) 
{
	int count = 1;
	for (evl_statements::const_iterator iter = statements.begin(); iter != statements.end(); ++iter) 
	{
		if (iter->type == evl_statement::MODULE) 
		{
			out << "STATEMENT " << count << " MODULE, " << iter->tokens.size() << " tokens" << std::endl;
		}
		else if (iter->type == evl_statement::WIRE) 
		{
			out << "STATEMENT " << count << " WIRE, " << iter->tokens.size() << " tokens" << std::endl;
		}
		else if (iter->type == evl_statement::COMPONENT) 
		{
			out << "STATEMENT " << count << " COMPONENT, " << iter->tokens.size() << " tokens" << std::endl;
		}
		else if (iter->type == evl_statement::ENDMODULE) 
		{
			out << "STATEMENT " << count << " ENDMODULE" << std::endl;
		}
		else 
		{
			out << "ERROR" << std::endl;
		}
		count++;
	}
}
bool store_statements_to_file(std::string file_name, const evl_statements &statements) 
{
	std::ofstream output_file(file_name.c_str());
	if (!output_file)
	{
		//std::cerr << "I can't write"  output_file << "." << std::endl;
		return false;
	}
	display_statements(output_file, statements);
	return true;
}

bool process_module_statements(evl_modules &modules, evl_statement &s) 
{
	enum state_type { INIT, NAME, SEMI_CHECK, CHK_STMT, WIRE, COMPONENT, ENDMOD, DONE };
	state_type state;
	if (s.type == evl_statement::MODULE) 
	{
		state = INIT;
	}
	else if (s.type == evl_statement::ENDMODULE) 
	{
		state = ENDMOD;
	}
	else if (s.type == evl_statement::WIRE) 
	{
		state = WIRE;
	}
	else if (s.type == evl_statement::COMPONENT) 
	{
		state = COMPONENT;
	}

	for (; !s.tokens.empty() && (state != DONE); s.tokens.pop_front()) 
	{
		evl_token t = s.tokens.front();

		if (state == INIT)
		{
			if (t.type == evl_token::NAME && t.str == "module") 
			{
				state = NAME;
			}
			else
			{
				std::cerr << "Need 'module' but found '" << t.str << "' at line " << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == NAME) 
		{
			if (t.type == evl_token::NAME && t.str != "module") 
			{
				evl_module mod;
				mod.name = t.str;
				modules.push_back(mod);
				state = SEMI_CHECK;
			}
			else 
			{
				std::cerr << "Need a NAME but found '" << t.str << "' at line " << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == SEMI_CHECK)
		{
			if (t.type == evl_token::SINGLE && t.str == ";") 
			{
				state = DONE;
			}
			else 
			{
				std::cerr << "Need a ';' but found '" << t.str << "' at line " << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == WIRE) 
		{
			evl_wires wires;
			if (!process_wire_statements(wires, s)) 
			{
				return false;
			}
			else 
			{
				modules.back().c_wires.insert(wires.begin(), wires.end());
				state = DONE;
				break;
			}
		}
		else if (state == COMPONENT)
		{
			evl_components comp;
			if (!process_component_statements(comp, s, modules.back().c_wires)) 
			{
				return false;
			}
			else 
			{
				modules.back().c_components.splice(modules.back().c_components.end(), comp);
				state = DONE;
				break;
			}
		}
		else if (state == ENDMOD) 
		{
			if (s.type == evl_statement::ENDMODULE) 
			{
				state = DONE;
			}
		}
		else 
		{
			assert(false); 
		}
	}
	if (!s.tokens.empty() || (state != DONE)) 
	{
		std::cerr << "something wrong with the statement in mod" << std::endl;
		return false;
	}
	return true;
}
bool process_wire_statements(evl_wires &wires, evl_statement &s) 
{
	assert(s.type == evl_statement::WIRE);

	enum state_type { INIT, WIRE, WIRES, WIRE_NAME, BUS, BUS_MSB, BUS_COLON, BUS_LSB, BUS_DONE, DONE };

	state_type state = INIT;
	int bus_width = 1;
	for (; !s.tokens.empty() && (state != DONE); s.tokens.pop_front()) 
	{
		evl_token t = s.tokens.front();
		if (state == INIT) 
		{
			if (t.str == "wire")
			{
				state = WIRE;
			}
			else 
			{
				std::cerr << "Need 'wire' but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == WIRE)
		{
			if (t.type == evl_token::NAME) 
			{
				evl_wires::iterator it = wires.find(t.str);
				if (it != wires.end()) 
				{
					std::cerr << "Wire '" << t.str << "' on line " << t.line_no << " is already defined" << std::endl;
					return false;
				}
				wires.insert(std::make_pair(t.str, bus_width));
				state = WIRE_NAME;
			}
			else if (t.type == evl_token::SINGLE) 
			{
				if (t.str != "[")
				{
					std::cerr << "Need a '[' buf found '" << t.str << "' on line" << t.line_no << std::endl;
					return false;
				}
				state = BUS;
			}
			else 
			{
				std::cerr << "Need 'wire' but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == WIRES) 
		{
			if (t.type == evl_token::NAME) 
			{
				evl_wires::iterator it = wires.find(t.str);
				if (it != wires.end()) 
				{
					std::cerr << "Wire '" << t.str << "' on line " << t.line_no << " is already defined" << std::endl;
					return false;
				}
				wires.insert(std::make_pair(t.str, bus_width));
				state = WIRE_NAME;
			}

		}
		else if (state == BUS)
		{
			if (t.type == evl_token::NUMBER) 
			{
				bus_width = atoi(t.str.c_str()) + 1;
				if (bus_width < 2) 
				{
					std::cerr << "Bus width entered is " << bus_width << " but needs a bus width of at least 2. Line No: " << t.line_no << std::endl;
					return false;
				}
			}
			else 
			{
				std::cerr << "Need 'number' but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
			state = BUS_MSB;
		}
		else if (state == BUS_MSB)
		{
			if (t.str != ":") 
			{
				std::cerr << "Need a ':' buf found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
			state = BUS_COLON;

		}
		else if (state == BUS_COLON) 
		{
			if (t.type == evl_token::NUMBER) 
			{
				if (t.str != "0")
				{
					std::cerr << "Need a '0' buf found '" << t.str << "' on line " << t.line_no << std::endl;
					return false;
				}
				state = BUS_LSB;
			}
		}
		else if (state == BUS_LSB) 
		{
			if (t.type == evl_token::SINGLE) 
			{
				if (t.str != "]") 
				{
					std::cerr << "Need a ']' buf found '" << t.str << "' on line" << t.line_no << std::endl;
					return false;
				}
				state = BUS_DONE;
			}

		}
		else if (state == BUS_DONE) 
		{
			wires.insert(std::make_pair(t.str, bus_width));
			state = WIRE_NAME;
		}


		else if (state == WIRE_NAME) 
		{
			if (t.str == ",") 
			{
				state = WIRES;
			}
			else if (t.str == ";") 
			{
				state = DONE;
			}
			else
			{
				std::cerr << "Need ',' or ';' but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
		}
		else 
		{
			assert(false);
		}
	}
	if (!s.tokens.empty() || (state != DONE)) 
	{
		std::cerr << "something wrong with the statement in wires" << std::endl;
		return false;
	}

	return true;
}
bool process_component_statements(evl_components &components, evl_statement &s, const evl_wires &wires) 
{
	assert(s.type == evl_statement::COMPONENT);

	enum state_type { INIT, TYPE, NAME, PINS, PIN_NAME, PINS_DONE, BUS, BUS_MSB, BUS_COLON, BUS_LSB, BUS_DONE, DONE };

	std::string comp_type = "";
	std::string comp_name = "NONE";
	std::string pin_name = "";
	int bus_msb = -1;
	int bus_lsb = -1;

	state_type state = INIT;

	for (; !s.tokens.empty() && (state != DONE); s.tokens.pop_front())
	{
		evl_token t = s.tokens.front();

		if (state == INIT) 
		{
			if (t.type == evl_token::NAME) 
			{
				comp_type = t.str;
				state = TYPE;
			}
			else 
			{
				state = TYPE;
			}
		}
		else if (state == TYPE)
		{
			if (t.type == evl_token::NAME) 
			{
				comp_name = t.str;
				state = NAME;
			}
			else if (t.type == evl_token::SINGLE && t.str == "(") 
			{
				state = PINS;
			}
			else 
			{
				std::cerr << "Need a NAME or a '(' but found '" << t.str << "' at line " << t.line_no << std::endl;
				return false;
			}
			evl_component component;
			component.type = comp_type;
			component.name = comp_name;
			components.push_back(component);

		}
		else if (state == NAME) 
		{
			if (t.type == evl_token::SINGLE &&  t.str == "(")
			{
				state = PINS;
			}
			else 
			{
				std::cerr << "Need '(' but found '" << t.str << "' at line " << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == PINS)
		{

			if (t.type == evl_token::NAME) 
			{
				evl_wires::const_iterator it = wires.find(t.str);
				if (it == wires.end()) 
				{
					std::cerr << "Wire '" << t.str << "' on line " << t.line_no << " is not defined" << std::endl;
					return false;
				}
				pin_name = t.str;
			}
			state = PIN_NAME;
		}
		else if (state == PIN_NAME) 
		{
			bus_msb = -1;
			bus_lsb = -1;

			if (t.str == ")")
			{
				evl_wires::const_iterator it = wires.find(pin_name);
				if (it->second >= 2) 
				{
					bus_msb = it->second - 1;
					bus_lsb = 0;
				}
				evl_pin pins;
				pins.lsb = bus_lsb;
				pins.msb = bus_msb;
				pins.pin_name = pin_name;

				components.back().comp_pins.push_back(pins);

				state = PINS_DONE;
			}
			else if (t.str == ",")
			{
				evl_wires::const_iterator it = wires.find(pin_name);
				if (it->second >= 2) 
				{
					bus_msb = it->second - 1;
					bus_lsb = 0;
				}

				evl_pin pins;
				pins.lsb = bus_lsb;
				pins.msb = bus_msb;
				pins.pin_name = pin_name;

				components.back().comp_pins.push_back(pins);

				state = PINS;
			}
			else if (t.str == "[") 
			{
				state = BUS;
			}
			else
			{
				std::cerr << "Need ',' or ')' or '[' but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == BUS) 
		{
			if (t.type == evl_token::NUMBER) 
			{
				bus_msb = atoi(t.str.c_str());
			}
			else
			{
				std::cerr << "Need 'number' but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
			state = BUS_MSB;
		}
		else if (state == BUS_MSB)
		{
			if (t.str == "]") 
			{
				evl_wires::const_iterator it = wires.find(pin_name);
				int found_wire_msb = it->second - 1;

				if (bus_msb > found_wire_msb)
				{
					std::cerr << "Bus MSB of value: " << bus_msb << " must be greater than or equal to the wire bus msb of value: " << found_wire_msb << std::endl;
					return false;
				}
				if (bus_msb < 0) 
				{
//					std::cerr << "Bus MSB of value: " << bus_msb << " must be greater than 0." << std::cout;
				}
				bus_lsb = bus_msb;
				state = BUS_DONE;
			}
			else if (t.str == ":") 
			{
				state = BUS_COLON;
			}
			else 
			{
				std::cerr << "Need ']' or ':' but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
		}
		else if (state == BUS_COLON) 
		{
			evl_wires::const_iterator it = wires.find(pin_name);
			int found_wire_msb = it->second - 1;

			if (t.type == evl_token::NUMBER) 
			{
				bus_lsb = atoi(t.str.c_str());
			}
			else 
			{
				std::cerr << "Need 'number' but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
			if (bus_msb < bus_lsb) 
			{
				std::cerr << "Bus msb: " << bus_msb << " can not be less than Bus lsb " << bus_lsb << std::endl;
				return false;
			}
			if (bus_msb > found_wire_msb) 
			{
				std::cerr << "Bus msb is: " << bus_msb << ". Needs to less than or equal to  wire bus msb: " << found_wire_msb << std::endl;
				return false;
			}
			if (bus_lsb < 0)
			{
				std::cerr << "Bus lsb is: " << bus_lsb << ". Needs to be greater than 0" << std::endl;
				return false;
			}

			state = BUS_LSB;
		}
		else if (state == BUS_LSB) 
		{
			if (!(t.str == "]")) 
			{
				std::cerr << "Need ']' but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
			state = BUS_DONE;
		}
		else if (state == BUS_DONE) 
		{
			if (t.str == ")") 
			{
				evl_pin pins;
				pins.lsb = bus_lsb;
				pins.msb = bus_msb;
				pins.pin_name = pin_name;

				components.back().comp_pins.push_back(pins);
				state = PINS_DONE;
			}
			else if (t.str == ",") 
			{

				evl_pin pins;
				pins.lsb = bus_lsb;
				pins.msb = bus_msb;
				pins.pin_name = pin_name;

				components.back().comp_pins.push_back(pins);

				state = PINS;
			}
		}

		else if (state == PINS_DONE) 
		{
			if (t.str == ";") 
			{
				state = DONE;
			}
			else 
			{
				std::cerr << "Need ';' but found '" << t.str << "' on line " << t.line_no << std::endl;
				return false;
			}
		}
		else 
		{
			assert(false); 
		}
	}
	if (!s.tokens.empty() || (state != DONE)) 
	{
		std::cerr << "something wrong with the statement in Comp" << std::endl;
		return false;
	}

	return true;
}

void display_module(std::ostream &out, const evl_modules &modules) 
{
	for (evl_modules::const_iterator it1 = modules.begin(); it1 != modules.end(); ++it1) 
	{
		//out << "module " << it1->name << " " << it1->c_wires.size() << " " << it1->c_components.size() << std::endl;
	out <<"module "<<it1->name << std::endl;
	for (evl_wires::const_iterator it2 = it1->c_wires.begin(); it2 != it1->c_wires.end(); ++it2) 
		{
			out << "wire " << it2->first << " " << it2->second << std::endl;
	//out<< "wire" << it2->first << std::endl;
	}
		for (evl_components::const_iterator it3 = it1->c_components.begin(); it3 != it1->c_components.end(); ++it3) 
		{
			out << "component " << it3->type << " " << it3->name << " " << it3->comp_pins.size() << std::endl;
			for (evl_pins::const_iterator it4 = it3->comp_pins.begin(); it4 != it3->comp_pins.end(); ++it4) 
			{
				out << "pin " << it4->pin_name << " " << it4->msb << " " << it4->lsb << std::endl;
			}
		}

	}

}
bool store_module_to_file(std::string file_name, evl_modules &modules) 
{
	std::ofstream output_file(file_name.c_str());
	if (!output_file)
	{
//		std::cerr << "I can't write" << output_file << "." << std::endl;
		return false;
	}
	display_module(output_file, modules);
	return true;
}

bool parse_evl_file(std::string file_name, evl_modules &modules) 
{
	evl_tokens tokens;
	if (!extract_tokens_from_file(file_name, tokens)) 
	{
		return false;
	}
	if (!store_tokens_to_file(file_name + ".tokens", tokens)) 
	{
		return false;
	}

	evl_statements statements;
	if (!group_tokens_into_statements(statements, tokens))
	{
		return false;
	}
	if (!store_statements_to_file(file_name + ".statements", statements)) 
	{
		return false;
	}
	for (evl_statements::iterator iter = statements.begin(); iter != statements.end(); ++iter) 
	{
		if (!process_module_statements(modules, *iter)) 
		{
			return false;
		}
	}
	if (!store_module_to_file(file_name + ".syntax", modules)) 
	
	{
		return false;
	}
	return true;
}


