#include <fstream>
#include <iostream>
#include <string>
using namespace std;

class Input {
  ifstream fileStream;
  string fileName;
public:
  Input(string file) {
	fileName = file;
	openInput();
  }
  void openInput() {
	const char* file = fileName.c_str();
	fileStream.open(file);
	if(!fileStream.is_open()) {
	  cerr << "error opening the file" << endl;
	}
	else cout << fileName << ":\tOPEN" << endl;
  }
  string getNext() {
	if (!fileStream.eof()) {
	  string temp;
	  fileStream >> temp;
	  return temp;
	}
	else {
	  fileStream.close();
	  return "/EOF";
	}
  }
	int isEof() {
	if (fileStream.eof()) {
	  fileStream.close();
  	  return 1;
	}
	else return 0;
  }
};

int main() {
  string fileName;
  fileName = "T.hr.fsm";
  Input train (fileName);
  while (!train.isEof()) {
	cout << train.getNext() << endl;
  }
  return 0;
}
//http://www.cplusplus.com/reference/clibrary/cstdlib/system/
