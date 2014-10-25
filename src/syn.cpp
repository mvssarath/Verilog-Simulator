#include "syn.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//....................................processing statements and wire, component, module creation..................................................................///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool is_endmodule(const evl_token &token) 
{
	return token.str == "endmodule";
}

bool is_module(const evl_token &token) 
{
	return token.str == "module";
}

bool operator<(const evl_wire &x, const evl_wire &y)
{
	return x.name < y.name;
}

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
	assert(!tokens.empty());
	for (; !tokens.empty();) 
	{
    	    evl_token token = tokens.front();
    	    if (token.type != evl_token::NAME)
	    {
     		 std::cerr << "Need a NAME token but found '" << token.str << "' on line " << token.line_no << "." << std::endl;
      		return false;
    	    }

    	evl_statement statement;
	statement.tokens.clear(); 
	if (token.str == "endmodule") 
	{ 
		statement.type = evl_statement::ENDMODULE;
      		statement.tokens.push_back(tokens.front());
      		tokens.erase(tokens.begin()); 
	  	statements.push_back(statement);	
      		continue; 
   	}

    	if (token.str == "module") 
	{ 
		statement.type = evl_statement::MODULE;
    	}
    	else if (token.str == "wire") 
	{ 
      		statement.type = evl_statement::WIRE;
    	}
    	else 
	{ 
      		statement.type = evl_statement::COMPONENT;
    	}
    
	if (!move_tokens_to_statement(statement.tokens, tokens))
	{
      		return false;
    	} 
	 
    	assert(statement.tokens.back().str == ";");
    	statements.push_back(statement);	
	
  	}
	  
	return true;
}

bool move_tokens_to_module(evl_tokens &tokens, evl_modules_map &modules_map, map_evl_modules_line &modules_line_map) 
{
	assert(!tokens.empty());
	for(;!tokens.empty();)
	{
  		evl_tokens::iterator iter_1 = std::find_if(tokens.begin(), tokens.end(), is_module);
  		evl_tokens::iterator iter_2 = std::find_if(tokens.begin(), tokens.end(), is_module);
  		++iter_2;
  		if (!((*iter_2).type == evl_token::NAME)) 
  		{
     			std::cerr << "Looking for module NAME but found '" << (*iter_2).str << "' on line " << (*iter_2).line_no << "." <<std::endl;
     			return false;
  		}	
 		std::string module_name;		
  		int module_line_no;
  		module_name = (*iter_2).str;
  		module_line_no = (*iter_2).line_no;
  
      evl_tokens::iterator next_ = std::find_if(tokens.begin(), tokens.end(), is_endmodule);
      if (next_ == tokens.end()) 
      {
    		std::cerr << "Looking for 'endmodule' but reached the end of file." << std::endl;
    		return false;
      }
      ++next_;
      evl_tokens statement_tokens;
      statement_tokens.clear();
      statement_tokens.splice(statement_tokens.begin(), tokens, iter_1, next_);
      evl_modules_map::iterator it = modules_map.find(module_name);
      if (it != modules_map.end()) 
      {
    		std::cerr << "Error at line " << module_line_no << ": The module '" << module_name << "' already exists." <<  std::endl;  
		return false;
      }

      modules_map[module_name] = statement_tokens;
      modules_line_map[module_name] = module_line_no;
      }
      return true;
}

bool process_wire_statement(evl_wires &wires, evl_statement &s, evl_wires_map &wires_map)
{
	  assert(s.type == evl_statement::WIRE);
	  enum state_type {INIT, WIRE, BUS, BUS_MSB, BUS_COLON, BUS_LSB, BUS_DONE, WIRE_NAME, WIRES, DONE};
	  state_type state = INIT;
  	  evl_wire wire;
          int bus_width = 1;

  for (; !s.tokens.empty() && (state != DONE); s.tokens.pop_front()) 
  {
    evl_token t = s.tokens.front();
    
    switch(state) 
	{
      case INIT:
				if (t.str == "wire") 
				{
				  state = WIRE;
				}
				else 
				{
				  std::cerr << "Need 'wire' but found '" << t.str << "' on line " << t.line_no << "." << std::endl;
				  return false;
				}
				break;

      case WIRE:
				if (t.type == evl_token::NAME) 
				{
				  wire.name = t.str; wire.width = 1;
				  wire.line_no = t.line_no;
				  wires.push_back(wire);
		  
				  if (!map_wires(wires, wires_map)) 
				  {
					return false;
				  }
				  state = WIRE_NAME;
				}
				else if (t.str == "[") 
				{
				  state = BUS;
				}
				else 
				{
				  std::cerr << "Need NAME or '[' but found '" << t.str << "' on line " << t.line_no << "." << std::endl;
				  return false;
				}
				break;
		
      case BUS:
				if (t.type == evl_token::NUMBER) 
				{
				  bus_width = atoi(t.str.c_str())+1;
				  state = BUS_MSB;
				}
				else 
				{
				  std::cerr << "Need NUMBER but found '" << t.str << "' on line " << t.line_no << "." << std::endl;
				  return false;
				}
				break;
		
      case BUS_MSB:
				if (t.str == ":") 
				{
				  state = BUS_COLON;
				}
				else 
				{
				  std::cerr << "Need ':' but found '" << t.str << "' on line " << t.line_no << "." << std::endl;
				  return false;
				}	
				break;
		
      case BUS_COLON:
				if (t.str == "0")
				{
				  state = BUS_LSB;
				}
				else 
				{
				  std::cerr << "Need '0' but found '" << t.str << "' on line " << t.line_no << std::endl;
				  return false;
				}	
				break;
		
      case BUS_LSB:
				if (t.str == "]") 
				{
				  state = BUS_DONE;
				}
				else 
				{
				  std::cerr << "Need ']' but found '" << t.str << "' on line " << t.line_no << std::endl;
				  return false;
				}	
				break;
		
      case BUS_DONE:
				if (t.type == evl_token::NAME) 
				{
				  wire.name = t.str; wire.width = bus_width;
				  wire.line_no = t.line_no;
				  wires.push_back(wire);
				  if (!map_wires(wires, wires_map)) 
				  {
					return false;
				  }
				  state = WIRE_NAME;
				}
				else 
				{
				  std::cerr << "Need NAME but found '" << t.str << "' on line " << t.line_no << std::endl;
				  return false;
				}
				break;
		
      case WIRES:
				if (t.type == evl_token::NAME) 
				{
				  wire.name = t.str;
				  wire.line_no = t.line_no;
				  wires.push_back(wire);
		  
				  if (!map_wires(wires, wires_map)) 
				  {
					return false;
				  }
				  state = WIRE_NAME;
				}
				else 
				{
				  std::cerr << "Need NAME but found '" << t.str << "' on line " << t.line_no << "." << std::endl;
				  return false;
				}
				break;
		
      case WIRE_NAME:
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
				  std::cerr << "Need ',' or ';' but found '" << t.str << "' on line " << t.line_no << "." << std::endl;
				  return false;
				}	
				break;
		
      default :	
				assert(false);
				
				return false;
				break;
    }
	
  }	
  if (!s.tokens.empty() || (state != DONE)) 
  {
    std::cerr << "Something wrong with statement." << std::endl;
    return false;
  }
  return true;
}

bool process_component_statement(evl_components &components, evl_statement &s, evl_wires_map &wires_map, evl_ports_map &ports_map) 
{
  assert(s.type == evl_statement::COMPONENT);
  
  enum state_type {INIT, TYPE, NAME, PINS, PIN_NAME, BUS, BUS_MSB, BUS_COLON, BUS_LSB, BUS_DONE, PINS_DONE, DONE};
  
  state_type state = INIT;
  evl_pin pin;
  evl_component comp;   

  for (; !s.tokens.empty() && (state != DONE); s.tokens.pop_front()) 
  {
    evl_token t = s.tokens.front();
    switch(state)
	{

      case INIT:
				if (t.type == evl_token::NAME) 
				{
				  comp.type = t.str;
				  comp.name = "NONE";
				  state = TYPE;
				}
				else 
				{
				  std::cerr << "Need NAME but found '" << t.str << "' on line " << t.line_no << "." << std::endl;
				  return false;
				}
				break;

      case TYPE:
				if (t.type == evl_token::NAME) 
				{
				  comp.name = t.str;
				  state = NAME;
				}
				else if (t.str == "(") 
				{
				  state = PINS;
				}
				else 
				{
				  std::cerr << "Need NAME or '(' but found '" << t.str << "' on line " << t.line_no << "." << std::endl;
				  return false;
				}
				break;
		
      case NAME:
				if (t.str == "(") 
				{
				  state = PINS;
				}
				else 
				{
				  std::cerr << "Need '(' but found '" << t.str << "' on line " << t.line_no << "." << std::endl;
				  return false;
				}
				break;
		
      case PINS:

				if (t.type == evl_token::NAME)
				{
				  pin.name = t.str; 
				  pin.bus_lsb = -1;
				  pin.bus_msb = -1;
				  state = PIN_NAME;
				}
				else
				{
				  std::cerr << "Need NAME but found '" << t.str << "' on line " << t.line_no << "." << std::endl;
				  return false;
				}
				break;
		
      case PIN_NAME:
				if (t.str == "[") 
				{
				  state = BUS;
				}
				else if (t.str == ",") 
				{
				  if(!match_pins_and_wires(pin, wires_map, ports_map, t)) 
				  {
					return false;
				  }
				  comp.pins.push_back(pin);

				  state = PINS;
				}
				else if (t.str == ")") 
				{
				  if(!match_pins_and_wires(pin, wires_map, ports_map, t)) {
					return false;
				  }
				  comp.pins.push_back(pin);
				  state = PINS_DONE;
				}
				else 
				{
				  std::cerr << "Need '[' or ',' or ')' but found '" << t.str << "' on line " << t.line_no << "." << std::endl;
				  return false;
				}
				break;
		
		
      case BUS:
				if (t.type == evl_token::NUMBER) 
				{
				  pin.bus_msb = atoi(t.str.c_str());
				  state = BUS_MSB;
				}
				else 
				{
				  std::cerr << "Need NUMBER but found '" << t.str << "' on line " << t.line_no << "." << std::endl;
				  return false;
				}
				break;
		
      case BUS_MSB:
				if (t.str == ":") 
				{
				  state = BUS_COLON;
				}
				else if (t.str == "]") 
				{
				  state = BUS_DONE;
				}
				else 
				{
				  std::cerr << "Need ':' or ']' but found '" << t.str << "' on line " << t.line_no << "." << std::endl;
				  return false;
				}	
				break;
		
      case BUS_COLON:
				if (t.type == evl_token::NUMBER) 
				{
				  pin.bus_lsb = atoi(t.str.c_str());
				  state = BUS_LSB;
				}
				else 
				{
				  std::cerr << "Need NUMBER but found '" << t.str << "' on line " << t.line_no << "." << std::endl;
				  return false;
				}	
				break;
		
      case BUS_LSB:
				if (t.str == "]")
				{
				  state = BUS_DONE;
				}
				else 
				{
				  std::cerr << "Need ']' but found '" << t.str << "' on line " << t.line_no << "." << std::endl;
				  return false;
				}	
				break;
		
      case BUS_DONE:
				if (t.str == ",") 
				{
				  if(!match_pins_and_wires(pin, wires_map, ports_map, t)) 
				  {
					return false;
				  }
				  comp.pins.push_back(pin);
				  state = PINS;
				}
				else if (t.str == ")") 
				{
				  if(!match_pins_and_wires(pin, wires_map, ports_map, t))
				  {
					return false;
				  }
				  comp.pins.push_back(pin);
				  state = PINS_DONE;
				}
				else
				{
				  std::cerr << "Need ',' or ')' but found '" << t.str << "' on line " << t.line_no << "." << std::endl;
				  return false;
				}	
				break;
		
      case PINS_DONE:
				if (t.str == ";") 
				{
				  comp.pin_count = comp.pins.size();
				  components.push_back(comp);
//				  std::cout << "Component - Push Back" << comp.name << " " << " " << comp.pin_count << " " << comp.type << " " << std::endl;
				  state = DONE;
				}
				else 
				{
				  std::cerr << "Need ';' but found '" << t.str << "' on line " << t.line_no << "." << std::endl;
				  return false;
				}	
				break;
		
      default :	
				assert(false); // shouldn't reach here
				return false;
				break;
    
    }
	
  }	
  if (!s.tokens.empty() || (state != DONE)) 
  {
    std::cerr << "Something wrong with the statement." << std::endl;
    return false;
  }
  return true;
}

bool process_all_statements(evl_statements &statements, evl_wires &wires, evl_components &components, evl_ports &ports, evl_wires_map &wires_map, evl_ports_map &ports_map) {
  int count = 1;

  for (evl_statements::iterator iter = statements.begin(); iter != statements.end(); ++iter, ++count) 
  {
    if ((*iter).type == evl_statement::WIRE) 
	{
      if (!process_wire_statement(wires, *iter, wires_map)) 
	  {
        return false;
      }
    }
    else if ((*iter).type == evl_statement::COMPONENT) 
	{
      if (!process_component_statement(components,*iter, wires_map, ports_map)) 
	  {
        return false;
      } 
    }
    else if ((*iter).type == evl_statement::ENDMODULE) 
	{
  	  for (evl_tokens::iterator iter1 = (*iter).tokens.begin(); iter1 != (*iter).tokens.end(); ++iter1) 
	  {
        if ((*iter1).str != "endmodule") 
		{
          std::cerr << "Need 'endmodule' but found '" << (*iter1).str << "' on line " << (*iter1).line_no << "." << std::endl;
          return false;
		}
      } 
    }
  }
  return true;
}  

bool map_wires(evl_wires &wires, evl_wires_map &wires_map) 
{
    evl_wires_map::iterator it = wires_map.find(wires.back().name);
    
	if (it != wires_map.end()) 
	{
      std::cerr << "Error at line " << wires.back().line_no << ": The wire '" << wires.back().name << "' already exists." <<  std::endl;  
	  return false;
	}
    wires_map[wires.back().name] = wires.back().width;

    return true;    
}

bool map_ports(evl_ports &ports, evl_ports_map &ports_map) 
{
    evl_ports_map::iterator it = ports_map.find(ports.back().name);

    if (it != ports_map.end()) 
	{
      std::cerr << "Error at line " << ports.back().line_no << ": The port '" << ports.back().name << "' already exists." <<  std::endl;  
	  return false;
	}
    ports_map[ports.back().name] = ports.back().width;
    return true;    
}

bool match_pins_and_wires(evl_pin &pin, evl_wires_map &wires_map, evl_ports_map &ports_map, const evl_token &t) 
{
    evl_wires_map::iterator iter_3 = wires_map.find(pin.name);
    evl_ports_map::iterator iter_4 = ports_map.find(pin.name);

	if (iter_3 != wires_map.end()) 
	{ 
      if (wires_map[pin.name] != 1 && pin.bus_msb == -1 && pin.bus_lsb == -1)  
	  { 
        pin.bus_msb = wires_map[pin.name] - 1;
        pin.bus_lsb = 0;
	  }
	  else if (pin.bus_msb != pin.bus_lsb && pin.bus_lsb == -1)  
	  { 
	    if (!((wires_map[pin.name] > pin.bus_msb) && (pin.bus_msb >= 0))) 
		{
          std::cerr << "The wire '" << pin.name << "' does not satisfy the condition [width > msb >= 0] on line " << t.line_no << "." << std::endl;
          return false;
	    }
	    else if (((wires_map[pin.name] == 1) && (pin.bus_msb >= 0))) 
		{
          std::cerr << "The wire '" << pin.name << "' is not a bus on line " << t.line_no << "." << std::endl;
          return false;
	    }
	    pin.bus_lsb = pin.bus_msb;
	  }
	  else if (wires_map[pin.name] == 1 && pin.bus_msb == -1 && pin.bus_lsb == -1)  
	  { 
		;
	  }
	  else 
	  { 
	    if (!((wires_map[pin.name] > pin.bus_msb) && (pin.bus_msb >= pin.bus_lsb) && (pin.bus_lsb >= 0))) 
		{
          std::cerr << "The wire '" << pin.name << "' does not satisfy the condition [width > msb >= lsb >= 0] on line " << t.line_no << "." << std::endl;
          return false;
	    }
	  }
	} 
    else if(iter_4 != ports_map.end())
	{ 
      if (ports_map[pin.name] != 1 && pin.bus_msb == -1 && pin.bus_lsb == -1)  
	  { 
        pin.bus_msb = ports_map[pin.name] - 1;
        pin.bus_lsb = 0;
	  }
	  else if (pin.bus_msb != pin.bus_lsb && pin.bus_lsb == -1)  
	  { 
	    if (!((ports_map[pin.name] > pin.bus_msb) && (pin.bus_msb >= 0))) 
		{
          std::cerr << "The port '" << pin.name << "' does not satisfy the condition [width > msb >= 0] on line " << t.line_no << "." << std::endl;
          return false;
	    }
	    else if (((ports_map[pin.name] == 1) && (pin.bus_msb >= 0))) 
		{
          std::cerr << "The port '" << pin.name << "' is not a bus on line " << t.line_no << "." << std::endl;
          return false;
	    }
	    pin.bus_lsb = pin.bus_msb;
	  }
	  else if (ports_map[pin.name] == 1 && pin.bus_msb == -1 && pin.bus_lsb == -1)  
	  { 
		;
	  }
	  else 
	  { 
	    if (!((ports_map[pin.name] > pin.bus_msb) && (pin.bus_msb >= pin.bus_lsb) && (pin.bus_lsb >= 0))) 
		{
          std::cerr << "The port '" << pin.name << "' does not satisfy the condition [width > msb >= lsb >= 0] on line: " << t.line_no << "." << std::endl;
          return false;
	    }
	  }
	} 
	
    return true;    
}

void display_syntactic(std::ostream &out, const evl_statements &statements, evl_wires &wires, const evl_components &components) 
{
  for (evl_statements::const_iterator iter = statements.begin(); iter != statements.end(); ++iter) 
  {
    if ((*iter).type == evl_statement::MODULE) 
	{
	  for (evl_tokens::const_iterator iter1 = (*iter).tokens.begin(); iter1 != (*iter).tokens.end(); ++iter1) 
	  {
        if ((*iter1).str != ";") 
		{
          out << (*iter1).str << " ";
	    }	
	  }
      out << wires.size() << " " << components.size() <<  std::endl;	  
    }
  }	
  wires.sort();

  for (evl_wires::iterator iter1 = wires.begin(); iter1 != wires.end(); ++iter1) 
  {
    out << "wire " << (*iter1).name << " " << (*iter1).width <<  std::endl;
  }	
      
  for (evl_components::const_iterator iter1 = components.begin(); iter1 != components.end(); ++iter1) 
  {
    out << "component " << (*iter1).type << " " << (*iter1).name << " " << (*iter1).pin_count <<  std::endl;
	
	for (evl_pins::const_iterator iter2 = (*iter1).pins.begin(); iter2 != (*iter1).pins.end(); ++iter2) 
	{
      out << "pin " << (*iter2).name << " " << (*iter2).bus_msb << " " << (*iter2).bus_lsb <<  std::endl;
    }	
  }	
}

