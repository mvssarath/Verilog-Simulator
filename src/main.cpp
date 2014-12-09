////    TITLE:: COMPUTER SIMUYLATION FOR DIGITAL LOGIC
////    NAME:: VENKATA SAI SARATH MAREPALLI
////    CWID:: A20319735
////    ECE 449 FALL 2014

#include "netlist_creation.h"
////////   ALL THE INCLUDE FILES ARE LOCATED IN LEXICAL ANALYSIS HEADER FILE AND CAN BE FOUND ONLY THERE
int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		std::cerr << "You should provide a file name." << std::endl;
		return -1;
	}

	std::string evl_file = argv[1];
	evl_modules modules;

	if (!parse_evl_file(argv[1], modules)) 
	{
		return -1;
	}

	netlist nl;
	if (!nl.create(modules.begin()->c_wires, modules.begin()->c_components)) 
	{
		return -1;
	}

	std::string n1_file = std::string(evl_file + ".netlist");
	nl.save(n1_file);

	nl.file_name = evl_file;
	nl.simulate(1000);


	return 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  CODE ENDS HERE   ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

