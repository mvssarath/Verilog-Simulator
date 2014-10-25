#include "net.h"


int main(int argc, char *argv[]) 
{

///////////Checking the presemce of file before starting of compiling
  if (argc < 2) 
  {
    std::cerr << "You should provide a file name." << std::endl;
    return -1;
  }
//////////////initializing wires, components, modules classes with objects
  	evl_wires wires;
	evl_components components;
	evl_ports ports;
	evl_modules_map modules_map;

////////////////////// all the project 2 code has been moved to parse_evl_file

	if (!parse_evl_file(argv[1], wires, components, ports))
	{
		return false;
	}

 
  std::string output_file_ = argv[1];
  output_file_ += ".netlist";
  std::ofstream output_file(output_file_.c_str());
  if (!output_file) 
  {
    std::cerr << "I can't write " << output_file_ << "." << std::endl;
    return -1;
  }
netlist netlist_1;
if (!netlist_1.create(wires,components))
  {
	  return -1;
  }
netlist_1.netlist_write (output_file, components);
std::cout << "Successfully wrote the following files: " << std::endl;
	std::cout << output_file_ + ".tokens" << std::endl;
	std::cout << output_file_ + ".statements" << std::endl;
	std::cout << output_file_ + ".syntax" << std::endl;
	std::cout << output_file_ + ".netlist" << std::endl;
}

