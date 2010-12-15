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
		firstWord[i] = i + 33000; //just a flag that it's the beginning of the file
		secondWord[i] = i+34000; //NOTE: SHOULD BE START SYMBOL
	}
	int pid;
  string fileName;
  fileName = "bestpath.txt";
	string inputFileName;
	inputFileName = "test.txt";
	int firstSymbol = 0;
	int secondSymbol = 0;
	//Begin Step 1
//	Input inputWord (inputFileName);
	//Input inputWord ("input.txt");
	string currWord;
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
		
		StdFst *inputFST = StdFst::Read("test_input.fst");
	
		StdFst *TL = StdFst::Read("../TL.fst");

	Timer fileTimer;

		Compose(*inputFST,*TL,&input);
	
//	ComposeFst<StdArc> composition(*inputFST, *TL);
	ShortestPath(input, &result_fst, 10,true,true);
	
	
	printf("Composing fie file took %f seconds.\n", fileTimer.getClock());
		
		StateId state_id;
		
		//This nested loop is to interate over each transition of each state.
		//Can be written as a function.
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
//		printf("\t%d\n", arc.nextstate);
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
//				printf("\t%d %f\n", arc2.nextstate, arc);
				if (arc2.olabel > 3) {
					words[i] = arc2.olabel + 3 * i;
					i++;
					//done = 1;
				}
				TropicalWeight weight = arc2.weight;
				weights[i] += weight.Value();
				currState = arc2.nextstate;
			}
		}
		
		result_fst.Write("beforeMod.fst");
		
		//		printf("\t%d\n", arc.nextstate);
		//		printf("\t%d\n", arc.ilabel);
		//		printf("\t%d\n", arc.olabel);
		//		TropicalWeight weight = arc.weight;
		//		printf("\t%f\n", weight.Value());
	}
	
	for (ArcIterator<StdFst> aiter((result_fst), 0); !aiter.Done(); aiter.Next())
	{
		const StdArc &arc = aiter.Value();
		start_states[j] = arc.nextstate;
		j++;
	}
	result_fst.DeleteArcs(0);
	if (firstWord[0] == -1) {
		for (i = 0; i < m; i++) {
			state_value = result_fst.AddState();
			result_fst.AddArc(0, StdArc(0,secondWord[i],0,state_value));
			for (j = 0; j < m; j++) {
				result_fst.AddArc(state_value,StdArc(0,0,0,start_states[j]));
			}
		}
	}
	else {
		for (i = 0; i < m; i++) {
			state_value = result_fst.AddState();
			result_fst.AddState();
			result_fst.AddArc(0, StdArc(0,firstWord[i],0,state_value));
			result_fst.AddArc(state_value,StdArc(0,secondWord[i],0,state_value+1));
			for (j = 0; j < m; j++) {
				result_fst.AddArc(state_value+1,StdArc(0,0,0,start_states[j]));
			}
		}
	}
 
	StdVectorFst newFST;
	newFST.AddState();
	newFST.SetStart(0);
	int currState;
//	printf("DID I GET HERE?\n");

	if (firstWord[0] == -1) {
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
//	printf("DID I GET HERE?\n");
	
	int finalState = newFST.AddState();
	newFST.AddState();
	newFST.AddArc(finalState, StdArc(0, 3, 0, finalState+1));
	newFST.SetFinal(finalState+1,0);
	for (i = 0; i < m; i++) {
		currState = newFST.AddState();
		newFST.AddArc(currState, StdArc(0, words[i], weights[i], finalState));
		for (j = 0; j < m; j++) {
			newFST.AddArc(start_states[j], StdArc(0, 0, 0, currState));
		}
	}

//	for (StateIterator<StdFst> siter(result_fst); !siter.Done(); siter.Next()) 
//	{
//		state_id = siter.Value();
//		printf("state id: %d\n", state_id);
	
/*	for (StateIterator<StdFst> siter(result_fst); !siter.Done(); siter.Next()) 
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
	} */
	
//	printf("DID I GET HERE?\n");
	
	result_fst.Write("oldbeforeG.fst");
	
	result_fst = newFST;
	
	result_fst.Write("beforeG.fst");	
	
	StdFst *G = StdFst::Read("../G/train.3.lm.fst");
	
	Compose(result_fst,*G,&input);
	ShortestPath(input, &result_fst, 10);

	input.Write("AfterComp.fst");
	result_fst.Write("AfterG.fst");
	
//	for (i = 0; i < 10; i++) {
//		printf("Word: %d Weight: %f \n", words[i], weights[i]);
//	}
	
	float finalWeights[10];
	int firstWords[10];
	int secondWords[10];
	int thirdWords[10];
	i = 0;
	int count = 0;
	
	for (ArcIterator<StdFst> aiter((result_fst), 0); !aiter.Done(); aiter.Next())
	{
		const StdArc &arc = aiter.Value();
		//		printf("\t%d\n", arc.nextstate);
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
				//				printf("\t%d %f\n", arc2.nextstate, arc);
				if (arc2.olabel > 3) {
					if (count == 0) {
						firstWords[i] = arc2.olabel;
					}
					else if (count == 1) {
						secondWords[i] = arc2.olabel;
						firstWord[i] = arc2.olabel;
					}
					else {
						thirdWords[i] = arc2.olabel;
						secondWord[i] = arc2.olabel;
					}
					count++;
					if (count > 2) {
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
		//		printf("\t%d\n", arc.nextstate);
		//		printf("\t%d\n", arc.ilabel);
		//		printf("\t%d\n", arc.olabel);
		//		TropicalWeight weight = arc.weight;
		//		printf("\t%f\n", weight.Value());
	}
	
//	for (i = 0; i < 10; i++) {
//		printf("Word: %d %d %d Weight: %f \n", firstWords[i],secondWords[i],thirdWords[i], weights[i]);
//	}
	outString[currWordIndex] = firstWords[0];
	currWordIndex++;
	
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
	//}
	
	currWordIndex +=2;
	outString[currWordIndex-2] = secondWords[0];
	outString[currWordIndex-1] = thirdWords[0];
	for (i = 0; i < currWordIndex; i++) {
		Input train ("../word_symbols.txt");
		for (j = 0; j < outString[i]; j++) {
			train.getNext();
			train.getNext();
		}
		cout << train.getNext() << endl;
		train.close();
	}
	
	return 0;
}


int main(int argc, char **argv) {
  string programName = "==========Speek-and-Spell!============\n";
  string authors = "Created by:\nNathaniel Weinman\tKibeom Kim\nPatrick Hammer\t\tAbhishek Suri\nShinan Zhang\t\tTaemin Cho\n";
  cout << programName << authors << endl;
  process();
  return 0;
}
