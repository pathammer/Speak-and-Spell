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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <fst/fstlib.h>
using namespace fst;
typedef StdArc::StateId StateId;











struct Symbol
{
	SymbolTable *Letter;
	SymbolTable *Word;
	Symbol()
	{
		Letter = SymbolTable::ReadText("../letter_symbols.txt", false);
		Word = SymbolTable::ReadText("../word_symbols.txt", false);
	}
} SYMBOL;

VectorFst<StdArc> StringToFst(const char* str)
{
	VectorFst < StdArc > result;
	VectorFst<StdArc>::StateId si1;
	VectorFst<StdArc>::StateId si2;
	
	si1 = result.AddState();
	si2 = si1;
	result.SetStart(si1);
	for (char *ptr = (char*)str; *ptr != '\0'; ++ptr)
	{
		si2 = result.AddState();
		char letter[2] =
		{ *ptr, '\0' };
		VectorFst<StdArc>::Arc arc(SYMBOL.Letter->Find(letter),
								   SYMBOL.Letter->Find(letter), 0, si2);
		result.AddArc(si1, arc);
		si1 = si2;
	}
	si2 = result.AddState();
	
	VectorFst<StdArc>::Arc arc(SYMBOL.Letter->Find("</w>"),
							   SYMBOL.Letter->Find("</w>"), 0, si2);
	result.AddArc(si1, arc);
	si1 = si2;
	
	si2 = result.AddState();
	
	VectorFst<StdArc>::Arc arc2(SYMBOL.Letter->Find("</s>"),
							   SYMBOL.Letter->Find("</s>"), 0, si2);
	result.AddArc(si1, arc2);
	si1 = si2;	
	
	result.SetFinal(si2, 0);
	
	//result.Write("test.fst");// for debug

	return result;
	
}













void demo() {
  Timer egg;
  egg.wait(3);//pass integer in seconds
  printf("I just waited %f seconds!\n",egg.getClock()); //returns a double in seconds
  
  Timer fileTimer;
  string fileName;
  fileName = "test.txt";
  Input train (fileName); //takes a string filename - this would also work Input train ("file.txt");
  while (!train.isEof()) {
	cout << train.getNext() << endl; //returns a string, or "/E0F" if you didn't check for isEof() and over-read the file
  }//autimaticlly closes the stream at EOF
  
  printf("Reading fie file took %f seconds.\n", fileTimer.getClock());
}

int process() { //written by nathaniel
	Timer egg;
	int first = 0;
	int firstWords[10];
	int secondWords[10];
	int thirdWords[10];	
	ofstream outFile;
	int outString[1000]; //Size of output
	int i = 0;
	for (i = 0; i < 1000; i++) {
		outString[i] = -1;
	}
	int currWordIndex = 0;
	int n = 10;
	int m = 10;
	int firstWord[m]; //so that it only composes secondWord the first time it's run
	int secondWord[m]; //start symbol

	int j = 0;
	for (i = 0; i < m; i++) {
		firstWord[i] = -1; //just a flag that it's the beginning of the file
		secondWord[i] = 0; //NOTE: SHOULD BE START SYMBOL
	}
	int pid;
  string fileName;
  fileName = "bestpath.txt";
	string inputFileName;
	inputFileName = "input.txt";
	int firstSymbol = 0;
	int secondSymbol = 0;
	//Begin Step 1
//	Input inputWord (inputFileName);
	Input inputWord ("input.txt");
	//string currWord;
	//While there is still another word in the file: {
//	while (!inputWord.isEof()) {
		//string currWord = inputWord.getNext();
//		currWord = inputWord.getNext();
	//End step 1
	//Begin Step 2
		
	//NOTE: Run program to convert word to characer FST (through Kibeom's python program?)
		
	//End step 2
	
	//----------------------------------------------------------
	//		input.fst is fst input name
	//----------------------------------------------------------
		
		StdVectorFst input;
		StdVectorFst result_fst;
		
// = StdFst::Read("test_input.fst");
	
		StdFst *TL = StdFst::Read("../TL.fst");
	StdFst *G = StdFst::Read("../G/train.3.lm.fst");

/*	string inputFSTS[12];
	inputFSTS[0] = "1.fst";
	inputFSTS[1] = "2.fst";
	inputFSTS[2] = "3.fst";
	inputFSTS[3] = "4.fst";
	inputFSTS[4] = "5.fst";
	inputFSTS[5] = "6.fst";
	inputFSTS[6] = "7.fst";
	inputFSTS[7] = "8.fst";
	inputFSTS[8] = "9.fst";
	inputFSTS[9] = "10.fst";
	inputFSTS[10] = "11.fst";
	inputFSTS[11] = "12.fst";*/

	
	
	
	
	
	
	
	
	
	

	
	string currword;
	currword = inputWord.getNext();
	VectorFst<StdArc> inputFST2 = StringToFst(currword.c_str());//StdFst::Read(inputFSTS[0].c_str());	
//	StdFst *inputFST3 = StdFst::Read(inputFSTS[1].c_str());
	Compose(inputFST2,*TL,&input);
	ShortestPath(input, &result_fst, 10);

	Compose(result_fst,*G,&input);
	ShortestPath(input, &result_fst,10);
	
	j = 0;
	i = 0;
	
	
	for (ArcIterator<StdFst> aiter((result_fst), 0); !aiter.Done(); aiter.Next())
	{
		const StdArc &arc = aiter.Value();
		int done = 0;
		int currState = arc.nextstate;
		while (done == 0) {
			ArcIterator<StdFst> aiter2((result_fst), currState);
			if (aiter2.Done()) {
				done = 1;
			}
			else {
				const StdArc &arc2 = aiter2.Value();
				if (arc2.olabel > 3) {
					secondWord[i] = arc2.olabel;
					i++;
				}
				currState = arc2.nextstate;
			}
		}
	}

/*	
	Compose(*inputFST3,*TL,&input);
		
		ShortestPath(input, &result_fst, 10);
		
				
		j = 0;
		i = 0;
		
		
		for (ArcIterator<StdFst> aiter((result_fst), 0); !aiter.Done(); aiter.Next())
		{
			const StdArc &arc = aiter.Value();
			int done = 0;
			int currState = arc.nextstate;
			while (done == 0) {
				ArcIterator<StdFst> aiter2((result_fst), currState);
				if (aiter2.Done()) {
					done = 1;
				}
				else {
					const StdArc &arc2 = aiter2.Value();
					if (arc2.olabel > 3) {
						secondWord[i] = arc2.olabel;
						i++;
					}
					currState = arc2.nextstate;
				}
			}
		}
			*/
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

	
	
	
	
//	int globCounter = 0;
//	for (globCounter = 1; globCounter < 10; globCounter++) {
	while (!inputWord.isEof()) {
		currword = inputWord.getNext();
		VectorFst<StdArc> inputFST = StringToFst(currword.c_str());
	
	Timer fileTimer;

		Compose(inputFST,*TL,&input);
	
	ShortestPath(input, &result_fst, 10);
	
	
	printf("Composing the file took %f seconds.\n", fileTimer.getClock());
	
		StateId state_id;
		
/* ------------------------------------------
	for (StateIterator<StdFst> siter(result_fst); !siter.Done(); siter.Next()) 
		{
			state_id = siter.Value();
			printf("state id: %d\n", state_id);
			for (ArcIterator<StdFst> aiter((result_fst), state_id); !aiter.Done(); aiter.Next())
			{
				const StdArc &arc = aiter.Value();
				printf("\t%d\n", arc.nextstate);
				printf("\t%d\n", arc.ilabel);
				printf("\t%d\n", arc.olabel);
				TropicalWeight weight = arc.weight;
				printf("\t%f\n", weight.Value());
			}
		}
  ------------------------------------------ */
	int state_value;
	int start_states[m];
	StateIterator<StdFst> siter(result_fst);
	j = 0;

	float weights[10];
	int words[10];
	i = 0;

	
	for (ArcIterator<StdFst> aiter((result_fst), 0); !aiter.Done(); aiter.Next())
	{
		const StdArc &arc = aiter.Value();
		int done = 0;
		TropicalWeight weight1 = arc.weight;
		weights[i] = weight1.Value();
		int currState = arc.nextstate;
		while (done == 0) {
			ArcIterator<StdFst> aiter2((result_fst), currState);
			if (aiter2.Done()) {
				done = 1;
			}
			else {
				const StdArc &arc2 = aiter2.Value();
				if (arc2.olabel > 3) {
					words[i] = arc2.olabel;
					i++;
				}
				TropicalWeight weight = arc2.weight;
				weights[i] += weight.Value();
				currState = arc2.nextstate;
			}
		}
		
		result_fst.Write("beforeMod.fst");
		
	}	
 
	StdVectorFst newFST;
	newFST.AddState();
	newFST.SetStart(0);
	int currState;

	if (first == 0) {
		for (i = 0; i < m; i++) {
			currState = newFST.AddState();
			newFST.AddArc(0, StdArc(0, secondWord[i], 0, currState));
			start_states[i] = currState;
		}
	}
	else {
		for (i = 0; i < m; i++) {
			currState = newFST.AddState();
			newFST.AddState();
			newFST.AddArc(0, StdArc(0, firstWord[i], 0, currState));
			newFST.AddArc(currState, StdArc(0, secondWord[i], 0, currState+1));
			start_states[i] = currState + 1;
		}
	}
	
	int finalState = newFST.AddState();
	newFST.AddState();
	newFST.AddArc(finalState, StdArc(0, 3, 0, finalState+1));
	newFST.SetFinal(finalState+1,0);

	for (i = 0; i < m; i++) {
		currState = newFST.AddState();
		newFST.AddArc(currState, StdArc(0, words[i], weights[i]*5, finalState));
		for (j = 0; j < m; j++) {
			newFST.AddArc(start_states[j], StdArc(0, 0, 0, currState));
		}
	}
		
	result_fst = newFST;
	
	result_fst.Write("beforeG.fst");	
	
	
	Compose(result_fst,*G,&input);
	ShortestPath(input, &result_fst, 10);

	input.Write("AfterComp.fst");
	result_fst.Write("AfterG.fst");
	float finalWeights[10];
	i = 0;
	int count = 0;

	for (ArcIterator<StdFst> aiter((result_fst), 0); !aiter.Done(); aiter.Next())
	{
		const StdArc &arc = aiter.Value();
		int done = 0;
		TropicalWeight weight1 = arc.weight;
		weights[i] = weight1.Value();
		int currState = arc.nextstate;
		int maxCount = 2;
		if (first == 0)
			maxCount--;

		while (done == 0) {
			ArcIterator<StdFst> aiter2((result_fst), currState);
			if (aiter2.Done()) {
				done = 1;
			}
			else {
				const StdArc &arc2 = aiter2.Value();
				if (arc2.olabel > 3) {
					if (count == (maxCount-2)) {
						firstWords[i] = arc2.olabel;
					}
					else if (count == (maxCount-1)) {
						secondWords[i] = arc2.olabel;
						firstWord[i] = arc2.olabel;
					}
					else {
						thirdWords[i] = arc2.olabel;
						secondWord[i] = arc2.olabel;
					}
					count++;
					if (count > maxCount) {
						count = 0;
						i++;
						done = 1;
					}
				}
				TropicalWeight weight = arc2.weight;
				weights[i] += weight.Value();
				currState = arc2.nextstate;
			}
		}
	}
	if (first != 0) {
		outString[currWordIndex] = firstWords[0];
		currWordIndex++;
	}
	else {
		first = 1;
	}

	
	}
	currWordIndex +=2;
	outString[currWordIndex-2] = secondWords[0];
	outString[currWordIndex-1] = thirdWords[0];
	for (i = 0; i < currWordIndex; i++) {
		Input train ("../word_symbols.txt");
		for (j = 0; j < outString[i]; j++) {
			train.getNext();
			train.getNext();
		}
		cout << train.getNext() << " ";
		train.close();
	}
	printf("\nReading the file took %f seconds.\n", egg.getClock());
	return 0;
}


int main(int argc, char **argv) {
  string programName = "==========Speek-and-Spell!============\n";
  string authors = "Created by:\nNathaniel Weinman\tKibeom Kim\nPatrick Hammer\t\tAbhishek Suri\nShinan Zhang\t\tTaemin Cho\n";
  cout << programName << authors << endl;
  process();
  return 0;
}
