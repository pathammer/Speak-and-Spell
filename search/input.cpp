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
	  return "/E0F";
	}
  }
  string getNextWord () { //returns the next all character string in the file
	string temp;
	while (!fileStream.eof()) {
	  fileStream >> temp;
	  if (temp[0]<122 && temp[0]>64) return temp;
	}
	fileStream.close();
	return "/E0F";
  }
  
  int isEof() {
	if (fileStream.eof()) {
	  fileStream.close();
  	  return 1;
	}
	else return 0;
  }
  void close() {
	fileStream.close();
  }
};
