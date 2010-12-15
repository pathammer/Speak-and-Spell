#include <sstream>
#include <iostream>
#include <string>

class MakeFst {
  std::stringstream output;
public:

  MakeFst (string input) {
	int stateNumber = 0;
	for (int i = 0; i<input.size(); i++) {
	  output << stateNumber << "\t" << (stateNumber+1) << "\t" << input[i] << "\t" << input[i] << "\n";
	  stateNumber++;
	}
	output << stateNumber << "\t" << (stateNumber+1) << "\t" << "</w>\t</w>\n";
	  stateNumber++;

 output << stateNumber << "\t" << (stateNumber+1) << "\t" << "</s>\t</s>\n";
	  stateNumber++;

	output << stateNumber;
  }
  
  string getString() {
	return output.str();
  }
};
