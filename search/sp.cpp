/*
  Spell Checker top level class
  This class is designed to take a file of
  english text and correct any spelling errors
  in it. It will also log performance data for
  annlisys later.
*/

#include <iostream>
#include <string>
using namespace std;
#include "timer.cpp"
#include "input.cpp"
class timer;
class input;

void demo() {
  timer egg;
  egg.wait(3);
  printf("I just waited %f seconds!\n",egg.getClock());
  
  timer fileTimer;
  string fileName;
  fileName = "text.txt";
  Input train (fileName);
  while (!train.isEof()) {
	cout << train.getNext() << endl;
  }
  printf("Reading fie file took %f seconds.\n", fileTimer.getClock());
}

int main(int argc, char **argv) {
  string programName = "==========Speek-and-Spell!============\n";
  string authors = "Created by:\nNathaniel Weinman\tKibeom Kim\nPatrick Hammer\t\tAbhishek Suri\nShinan Zhang\t\tTaemin Cho\n";
  cout << programName << authors << endl;
  demo();
  return 0;
}
