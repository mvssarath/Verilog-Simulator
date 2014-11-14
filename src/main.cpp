#include "netlist_creation.h"

int main(int argc, char *argv[])
{
//////////////////////////////COMMON FOR ALL PROJECTS///////////////////////////////////
	if (argc < 2)
	{
		std::cerr << "You should provide a file name." << std::endl;
		return -1;
	}
	std::string evl_file=argv[1];
////////////////////////////MAIN FOR LEXICAL ANALYSIS//////////////////////////////////
	evl_tokens tokens;
	if (!extract_tokens_from_file(evl_file, tokens))
	{
		return -1;
	}
	if(!store_tokens_to_file(evl_file+".tokens",tokens))
	{
		return -1;
	}
	////////////////////////////MAIN FOR SYNTACTICAL ANALYSIS//////////////////////////////////
	evl_statements statements;
	if(!group_tokens_into_statements(statements,tokens))
	{
		return -1;
	}
	if(!store_statements_to_file(evl_file+".statements",statements))
	{
		return -1;
	}

	evl_wires wires;
	evl_components components;
	evl_modules modules;
	std::ofstream output_file((evl_file+ ".syntax").c_str());
for (evl_statements::iterator it=statements.begin();it!= statements.end(); ++it)
	{
        if ((*it).type == evl_statement::MODULE)
		{
			if(!process_module_statement(modules,(*it)))
			{
				return -1;
			}
		}
		else if ((*it).type == evl_statement::WIRE)
		{
			if(!process_wire_statement(wires,(*it)))
			{
				return -1;
			}
		}

		else if ((*it).type == evl_statement::COMPONENT)
		{

            if(!process_component_statement(components,(*it)))
			{
				return -1;

			}
		}

		else
		{
			break ;

		}
	}
////////////////////////////MAIN FOR NETLIST CREATION//////////////////////////////////
	evl_wires_table wires_table = make_wires_table(wires);
	netlist nl;
	if (!nl.create(wires, components, wires_table))
	return -1;
	std::string nl_file = std::string(argv[1])+".netlist";
		nl.save(nl_file,modules); 
	return 0;
}

