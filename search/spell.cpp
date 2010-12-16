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
#include "makeFst.cpp"
#include "makeFst.cpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <fst/fstlib.h>

void demo() {
  makeFst tree;
  VectorFst<StdArc> out = tree.StringToFst("hello");

  Timer egg;
  egg.wait(1);//pass integer in seconds
  printf("I just waited %f seconds!\n",egg.getClock()); //returns a double in seconds
  
  Timer fileTimer;
  string fileName;
  fileName = "test";
  Input train (fileName); //takes a string filename - this would also work Input train ("file.txt");
  while (!train.isEof()) {
	cout << train.getNext() << endl; //returns a string, or "/E0F" if you didn't check for isEof() and over-read the file
  }//autimaticlly closes the stream at EOF
  
  printf("Reading fie file took %f seconds.\n", fileTimer.getClock());

  fileName = "test";
  Input test (fileName);	
  string temp = test.getNextWord();
  while (temp.compare("/E0F") != 0) { //while not eof!
	cout << temp << endl;
	temp = test.getNextWord();
  }
}

int process() {
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
  int pid;
  string fileName = "bestpath.txt";
  string inputFileName = "test.txt";
  int firstSymbol = 0;
  int secondSymbol = 0;
  //Begin Step 1
  Input inputWord (inputFileName);
  //Input inputWord ("input.txt");
  string currWord;
  //While there is still another word in the file: {
  while (!inputWord.isEof()) {
	currWord = inputWord.getNext();
	//End step 1
	//Begin Step 2
		
	//NOTE: Run program to convert word to characer FST (through Kibeom's python program?)
		
	//End step 2
	//Begin step 3/4
	pid = fork();
	if (pid < 0) {
	  fprintf(stderr, "Fork failed.\n");
	  exit(-1);
	}
	else {
	  //NOTE: Incorrect now
	  //Correct command line would be: 
	  //fstcompose pythonProgramName.fst TL.fst | fstprint | fsmcompile -t  | fsmbestpath -n 10 -u | fsmprint > bestpath.txt			
	  execlp("fstcompose", "fstcompose", "char.fst", "TL.fst","|","fstshortestpath","--nshortest=10","|","fstprint",">","bestpath.txt",NULL);
	}
		
	//(through system?) something like: fstcompose char.fst TL.fst | fstshortestpath -numberPaths 10 | fstprint > "bestpathtest.txt"
	//End step 3/4
	//Begin Step 5
	Input train (fileName);
	outFile.open("newbestpath.txt");
	//Add extra states for the previous pairs of words
	//If it's the very beginning, there's only 1 previous word (the start symbol)
	if (firstWord[0] == -1) {
	  for (i = 0; i < m; i++) {
		outFile << "0\t" << (10001+2*i) <<"\t0\t"<<secondWord[i]<<"\n";
	  }		
	}
	else {
	  for (i = 0; i < m; i++) {
		outFile << "0\t" << (10000+2*i) <<"\t0\t"<<firstWord[i]<<"\n";
		outFile << (10000+2*i) << "\t" << (10001+2*i) << "\t0\t"<<secondWord[i]<<"\n";
	  }
	}
	//Gets the start states of each word, and inserts each pair of words inbetween
	firstSymbol = atoi(train.getNext().c_str());
	secondSymbol = atoi(train.getNext().c_str());
	while (firstSymbol == 0) {
	  train.getNext();
	  train.getNext();
	  for (i = 0; i < m; i++) {
		outFile<< (10001+2*i) << "\t" << secondSymbol << "\t0\t0\n";
	  }
	  firstSymbol = atoi(train.getNext().c_str());
	  secondSymbol = atoi(train.getNext().c_str());
	}
	//Do differnetly now, should read in line at a time and output line (including newLine symbol)
	int count = 1;
	//copies rest of the file
	outFile<<firstSymbol<<"\t"<<secondSymbol<<"\t";
	while (!train.isEof()) {
	  count++;
	  if (count != 4) {
		outFile << train.getNext() << "\t";
	  }
	  else {
		outFile << train.getNext() << "\n";
		count = 0;
	  }
			
	}
	outFile.close();
	//End Step 5
	//Begin Step 6/7
	pid = fork();
	if (pid < 0) {
	  fprintf(stderr, "Fork failed.\n");
	  exit(-1);
	}
	else {
	  //NOTE: Incorrect now
	  //Correct command line would be: 
	  //fstcompile newbestpath.txt | fstcompose - G/train.3.lm.fst | fstprint | fsmcompile -t | fsmbestpath -n 10 -u | fsmprint 0o word_symbols.txt > bestfinalpath.txt
	  execlp("fstcompile","fstcompile","newbestpath.txt","|", "fstcompose", "G.fst","|","fstshortestpath","--nshortest=10","|","fstprint",">","bestfinalpath.txt",NULL);
	}
	//End Step 6/7
	//Begin Step 8
	//Something similar to above (need a sample file to see the format), basically extract the pairs of words, store them into firstword[i] secondword[i]
	//Also, add the best paths old firstword[i] to a string keeping the corrected message so far
	//NOTE: A way to do this would be a function that does something like getNext, but keeps doing getNext until it gets a string
	//Then, when it sees the 1st string, add that to the valid text so far.
	//When it sees the 2nd, set it to firstword[0]
	//When it sees the 3rd, set it to secondword[0]
	//Repeat 9 times:
	//ignore the next word
	//set the next word to firstword[1+i]
	//set the next word to secondword[1+]
	//End Step 8
	// End While loop}		
  }
  return 0;
}


int main(int argc, char **argv) {
  string programName = "==========Speek-and-Spell!============\n";
  string authors = "Created by:\nNathaniel Weinman\tKibeom Kim\nPatrick Hammer\t\tAbhishek Suri\nShinan Zhang\t\tTaemin Cho\n";
  cout << programName << authors << endl;
  demo();
  return 0;
}
