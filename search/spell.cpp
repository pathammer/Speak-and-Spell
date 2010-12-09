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

void demo() {
  Timer egg;
  egg.wait(3);//pass integer in seconds
  printf("I just waited %f seconds!\n",egg.getClock()); //returns a double in seconds
  
  Timer fileTimer;
  string fileName;
  fileName = "text.txt";
  Input train (fileName); //takes a string filename - this would also work Input train ("file.txt");
  while (!train.isEof()) {
	cout << train.getNext() << endl; //returns a string, or "/E0F" if you didn't check for isEof() and over-read the file
  }//autimaticlly closes the stream at EOF
  
  printf("Reading fie file took %f seconds.\n", fileTimer.getClock());
}

int process() { //written by nathaiel
	ofstream outFile;
	int n = 10;
	int m = 10;
	int firstWord[m]; //so that it only composes secondWord the first time it's run
	int secondWord[m]; //start symbol
	int i = 0;
	for (i = 0; i < m; i++) {
		firstWord[i] = -1; //just a flag that it's the beginning of the file
		secondWord[i] = 0; //NOTE: SHOULD BE START SYMBOL
	}
  string fileName;
  fileName = "bestpathtest.txt";
	int firstSymbol = 0;
	int secondSymbol = 0;
	//Begin Step 1
	//Input inputWord ("input.txt");
	//While there is still another word in the file: {
	//string currWord = train2.getNext();
	//End step 1
	//Begin Step 2
	//Run program to convert word to characer FST (through Kibeom's python program?)
	//End step 2
	//Begin step 3/4
	//(through system?) something like: fstcompose char.fst TL.fst | fstshortestpath -numberPaths 10 | fstprint > "bestpathtest.txt"
	//End step 3/4
	//Begin Step 5
	Input train (fileName);
	outFile.open("newbestpathtest.txt");
	return 0;
}


int main(int argc, char **argv) {
  string programName = "==========Speek-and-Spell!============\n";
  string authors = "Created by:\nNathaniel Weinman\tKibeom Kim\nPatrick Hammer\t\tAbhishek Suri\nShinan Zhang\t\tTaemin Cho\n";
  cout << programName << authors << endl;
  demo();
  return 0;
}
