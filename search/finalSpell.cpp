/*
 *  finalSpell.cpp
 *  
 *
 *  Created by Nathaniel Weinman on 12/16/10.
 *  Copyright 2010 New York University. All rights reserved.
 *
 */

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <fst/fstlib.h>
#include <fst/queue.h>
#include "timer.cpp"
#include "input.cpp"

using namespace std;
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
// Typedefs for readability.
class BeamSearchStateEquivClass;
typedef GenericComposeStateTable<StdArc, IntegerFilterState<signed char> >
StateTable;
typedef NaturalPruneQueue<NaturalShortestFirstQueue<int, TropicalWeight> ,
TropicalWeight, BeamSearchStateEquivClass> Queue;
typedef AnyArcFilter<StdArc> ArcFilter;
typedef StdArc::StateId StateId;

struct BeamSearchStateEquivClass
{
public:
	explicit BeamSearchStateEquivClass(const StateTable &state_table) :
	state_table_(state_table)
	{
	}
	
	int operator()(int s) const
	{
		return state_table_.Tuple(s).state_id1;
	}
	
private:
	const StateTable &state_table_;
};

float* TLShortestPath(MutableArcIterator<VectorFst<StdArc> > *TLArcs[44694], string currword, int numBestPath, int beamWidth, VectorFst<StdArc> *TL) {
	int i = 0;
	int remWord;
	float wordWeight = 0;
	float *result = new float[2*numBestPath];
	int prevPaths[numBestPath];
	float prevWeights[numBestPath];

	VectorFst <StdArc> I = StringToFst(currword.c_str());
	VectorFst < StdArc > ofst;
	StateId state_id;
	
	for (i = 0; i < numBestPath; i++) {

		wordWeight = 0;
		
		ComposeFstOptions < StdArc > copts;
		copts.state_table = new GenericComposeStateTable<StdArc,
		IntegerFilterState<signed char> > (I, *TL);
		ComposeFst < StdArc > composed(I, *TL, copts);
		
		vector < TropicalWeight > distance;
		NaturalPruneQueue<NaturalShortestFirstQueue<int, TropicalWeight> ,
		TropicalWeight, BeamSearchStateEquivClass> state_queue(distance,
			new NaturalShortestFirstQueue<StdFst::StateId, TropicalWeight> (
				distance), BeamSearchStateEquivClass(*copts.state_table),
			beamWidth);
		ShortestPathOptions<StdArc, Queue, ArcFilter> shortest_path_opts(
			&state_queue, ArcFilter());
		shortest_path_opts.first_path = true;
		ShortestPath(composed, &ofst, &distance, shortest_path_opts);
		for (StateIterator<StdFst> siter(ofst); !siter.Done(); siter.Next()) 
		{
			state_id = siter.Value();
			for (ArcIterator<StdFst> aiter((ofst), state_id); !aiter.Done(); aiter.Next())
			{
				const StdArc &arc = aiter.Value();
				TropicalWeight weight = arc.weight;
				wordWeight+= weight.Value();
				if ((arc.olabel > 3)) {
					remWord = arc.olabel;
				}
				
			}
		}
		prevPaths[i] = remWord;
		prevWeights[i] = wordWeight;
		result[2*i] = remWord;
		result[2*i+1] = wordWeight;
		//Removing word from TL
		const StdArc &arc = (*TLArcs[remWord]).Value();
		StdArc arc2 = arc;
		arc2.weight = 99999;
		(*TLArcs[remWord]).SetValue(arc2);
	}
	for (i = 0; i < numBestPath; i++) {
		const StdArc &arc = (*TLArcs[prevPaths[i]]).Value();
		StdArc arc2 = arc;
		arc2.weight = prevWeights[i];
		(*TLArcs[prevPaths[i]]).SetValue(arc2);
	}
	
	return result;
}

float* composeTwoWords(MutableArcIterator<VectorFst<StdArc> > *TLArcs[44694], string currword, int numBestPath, int beamWidth, VectorFst<StdArc> *TL, StdFst *G, float *inWords, int TLGRatio) {
	StateId state_id;
	int state_value;
	int start_states[numBestPath];
	int firstWord[numBestPath];
	float firstWeight[numBestPath];
	float weights[numBestPath];
	int words[numBestPath];
	int currState;
	float *result = new float[numBestPath*3];

	int done = 0;	
	int i = 0;
	int j = 0;

	StdVectorFst result_fst;
	StdVectorFst input; //NEEDED?
	
	for (i = 0; i < numBestPath; i++) {
		firstWord[i] = *(inWords+i);
		firstWeight[i] = *(inWords + i + numBestPath);
	}	

	VectorFst<StdArc> inputFST = StringToFst(currword.c_str());
	
	float *TLResult = new float[2*numBestPath];
	TLResult = TLShortestPath(TLArcs, currword, numBestPath, beamWidth, TL);
	
	for (i = 0; i < numBestPath; i++) {
		words[i] = TLResult[2*i];
		weights[i] = TLResult[2*i+1];
		result[2*numBestPath+i] = TLResult[2*i+1];
	}
	
	//create an FST with every pair of words, keeping the weights from TL
	
	Timer ITLModify;
	
	result_fst.AddState();
	result_fst.SetStart(0);
	
	for (i = 0; i < numBestPath; i++) {
		currState = result_fst.AddState();
		result_fst.AddArc(0, StdArc(0, firstWord[i], firstWeight[i], currState));
		start_states[i] = currState;
	}
	
	int finalState = result_fst.AddState();
	result_fst.AddState();
	result_fst.AddArc(finalState, StdArc(0, 3, 0, finalState+1));
	result_fst.SetFinal(finalState+1,0);
	
	for (i = 0; i < numBestPath; i++) {
		currState = result_fst.AddState();
		result_fst.AddArc(currState, StdArc(0, words[i], weights[i]*TLGRatio, finalState));
		for (j = 0; j < numBestPath; j++) {
			result_fst.AddArc(start_states[j], StdArc(0, 0, 0, currState));
		}
	}
	
//	printf("Modifying ITL took %f seconds.\n", ITLModify.getClock());
	
	Timer GComp;
	
	Compose(result_fst,*G, &input);
	ShortestPath(input, &result_fst, numBestPath);
	
//	printf("G comp/shortest path took %f seconds.\n", GComp.getClock());

	//Extract words
	i = 0;
	for (ArcIterator<StdFst> aiter((result_fst), 0); !aiter.Done(); aiter.Next())
	{
		const StdArc &arc = aiter.Value();
		int done = 0;
		int currState = arc.nextstate;
		while (done < 2) {
			ArcIterator<StdFst> aiter2((result_fst), currState);
			if (aiter2.Done()) {
				done = 2;
			}
			else {
				const StdArc &arc2 = aiter2.Value();
				if (arc2.olabel > 3) {
					result[i] = arc2.olabel;
					i++;
					done += 1;
				}
				currState = arc2.nextstate;
			}
		}
	}
	
	return result;
	
}

float* composeThreeWords(MutableArcIterator<VectorFst<StdArc> > *TLArcs[44694], string currword, int numBestPath, int beamWidth, VectorFst<StdArc> *TL, StdFst *G, float *inWords, int TLGRatio) {
	StateId state_id;
	int state_value;
	int start_states[numBestPath];
	int firstWord[numBestPath];
	int secondWord[numBestPath];
	float firstWeight[numBestPath];
	float secondWeight[numBestPath];
	float weights[numBestPath];
	int words[numBestPath];
	int currState;
	float *result = new float[numBestPath*4 + 1];
	
	int done = 0;	
	int i = 0;
	int j = 0;
	
	StdVectorFst result_fst;
	StdVectorFst input; //NEEDED?
	
	for (i = 0; i < numBestPath; i++) {
		firstWord[i] = *(inWords+2*i);
		secondWord[i] = *(inWords+2*i+1);
		firstWeight[i] = *(inWords+2*i+2*numBestPath);
		secondWeight[i] = *(inWords+2*i+1+2*numBestPath);
		result[2*numBestPath+i] = 0;
	}	
	
	VectorFst<StdArc> inputFST = StringToFst(currword.c_str());
	float *TLResult = new float[2*numBestPath];
	TLResult = TLShortestPath(TLArcs, currword, numBestPath, beamWidth, TL);
	
	for (i = 0; i < numBestPath; i++) {
		words[i] = TLResult[2*i];
		weights[i] = TLResult[2*i+1];
		result[2*numBestPath+i] = TLResult[2*i+1];
		result[3*numBestPath+i] = 0;
	}
	
	//create an FST with every pair of words, keeping the weights from TL
	
	Timer ITLModify;
	
	result_fst.AddState();
	result_fst.SetStart(0);
	
	for (i = 0; i < numBestPath; i++) {
		currState = result_fst.AddState();
		result_fst.AddState();
		result_fst.AddState();
		result_fst.AddArc(0, StdArc(0, firstWord[i], firstWeight[i], currState));
		result_fst.AddArc(currState+1, StdArc(0, secondWord[i], secondWeight[i], currState+2));
		start_states[i] = currState + 2;
	}
	for (i = 0; i < numBestPath; i++) {
		for (j = 0; j < numBestPath; j++) {
			result_fst.AddArc(start_states[i]-2, StdArc(0, 0, 0, start_states[j]-1));
		}
	}
	
	int finalState = result_fst.AddState();
	result_fst.AddState();
	result_fst.AddArc(finalState, StdArc(0, 3, 0, finalState+1));
	result_fst.SetFinal(finalState+1,0);
	
	for (i = 0; i < numBestPath; i++) {
		currState = result_fst.AddState();
		result_fst.AddArc(currState, StdArc(0, words[i], weights[i]*TLGRatio, finalState));
		for (j = 0; j < numBestPath; j++) {
			result_fst.AddArc(start_states[j], StdArc(0, 0, 0, currState));
		}
	}
	
//	printf("Modifying ITL took %f seconds.\n", ITLModify.getClock());
	
	Timer GComp;

	
//	RmEpsilon(&result_fst);
	result_fst.Write("beforeG.fst");
	Compose(result_fst,*G, &input);
	RmEpsilon(&input);
	input.Write("middleG.fst");
	ShortestPath(input, &result_fst, numBestPath);
	result_fst.Write("afterG.fst");

//	printf("G comp/shortest path took %f seconds.\n", GComp.getClock());
	

	//Extract words
	i = 0;
	int first = 0; //NOTE: MOVE THIS
	for (ArcIterator<StdFst> aiter((result_fst), 0); !aiter.Done(); aiter.Next())
	{
		const StdArc &arc = aiter.Value();
		int done = 0;
		int currState = arc.nextstate;
		result[3*numBestPath+(i/2)] = arc.weight.Value();
		while (done < 3) {
			ArcIterator<StdFst> aiter2((result_fst), currState);
			if (aiter2.Done()) {
				done = 3;
			}
			else {
				const StdArc &arc2 = aiter2.Value();
				TropicalWeight weight = arc2.weight;
				result[3*numBestPath+(i/2)] += weight.Value();
				if (arc2.olabel > 3) {
					if ((done == 0) && (first == 0)) {
						first = 1;
						result[4*numBestPath] = arc2.olabel;
					}
					if (done != 0) {
						result[i] =	arc2.olabel;
					}
					if (done != 0) {
						i++;
					}
					done += 1;
				}
				currState = arc2.nextstate;
			}
		}
	}
	
	return result;
	
}

void inFile(string inputFile, int numBestPath, int beamWidth, int TLGRatio) {
	Timer initializing;
	Timer inFileTime; 
	ofstream outFile; // Note:Needed?
	int outString[1000]; //Note: Size of output Necessary for some reason???????
	float *firstWordList = new float[2*numBestPath];
	float *TLResult = new float[2*numBestPath];
	float *TotalResult = new float[4*numBestPath+1];
	float *inputWordsInput = new float[3*numBestPath];
	float *inputWords = new float[4*numBestPath];
	int currState;
	
	int i = 0;
	int j = 0;

	StdVectorFst input;
	StdVectorFst result_fst;
	StdFst *G = StdFst::Read("../G/train.3.lm.fst");
	StdFst *G1 = StdFst::Read("../G/train.1.lm.fst");
	StdFst *G2 = StdFst::Read("../G/train.2.lm.fst");
	VectorFst<StdArc> *TL = VectorFst<StdArc>::Read("../TL.fst");
	StateId state_id;

	MutableArcIterator<VectorFst<StdArc> > *TLArcs[44694];
	
	for (StateIterator<StdFst> siter(*TL); !siter.Done(); siter.Next()) 
	{
		state_id = siter.Value();
		i = -1; //Keeps track of how many Nexts were perforemed
			    //i.e. which specific arc are we looking for
		for (ArcIterator<StdFst> aiter((*TL), state_id); !aiter.Done(); aiter.Next())
		{
			i++;
			const StdArc &arc = aiter.Value();
			if (arc.olabel > 3) {
				TLArcs[arc.olabel] = new MutableArcIterator<VectorFst<StdArc> > ((TL), state_id);
				for (j = 0; j < i; j++) 
					TLArcs[arc.olabel]->Next();				
			}
			
		}
	}
	
	printf("Initializing took %f seconds.\n", initializing.getClock());

	
	Input inputWord (inputFile);
	string currword = inputWord.getNext();
	TLResult = TLShortestPath(TLArcs, currword, numBestPath, beamWidth, TL);
	result_fst.AddState();
	result_fst.SetStart(0);
	result_fst.AddState();
	result_fst.AddState();
	result_fst.AddArc(1, StdArc(0, 3, 0, 2));
	result_fst.SetFinal(2,0);

	for (i = 0; i < numBestPath; i++) {
		result_fst.AddArc(0, StdArc(0, TLResult[2*i], TLResult[2*i+1], 1));
	}
	Compose(result_fst,*G1,&input);
	ShortestPath(input, &result_fst,numBestPath);
	
	//Extract the words
	i=0;
	for (ArcIterator<StdFst> aiter((result_fst), 0); !aiter.Done(); aiter.Next())
	{
		const StdArc &arc = aiter.Value();
		int done = 0;
		int currState = arc.nextstate;
		firstWordList[numBestPath+i] = arc.weight.Value();
		while (done == 0) {
			ArcIterator<StdFst> aiter2((result_fst), currState);
			if (aiter2.Done()) {
				done = 1;
			}
			else {
				const StdArc &arc2 = aiter2.Value();
				if (arc2.olabel > 3) {
					firstWordList[i] = arc2.olabel;
				}
				firstWordList[numBestPath+i] += arc2.weight.Value();
				currState = arc2.nextstate;
				if (arc2.olabel > 3) {
					i++;
				}
			}
		}
	}

	currword = inputWord.getNext();
	inputWordsInput = composeTwoWords(TLArcs, currword, numBestPath, beamWidth, TL, G2, firstWordList, TLGRatio);	
	for (i = 0; i < numBestPath; i++) {
		inputWords[2*i] = inputWordsInput[2*i];
		inputWords[2*i+1] = inputWordsInput[2*i+1];
		inputWords[2*i+2*numBestPath] = firstWordList[numBestPath+i];
		inputWords[2*i+1+2*numBestPath] = inputWordsInput[2*numBestPath+i];
	}
	while(!inputWord.isEof()) {
		currword = inputWord.getNext();
		TotalResult = composeThreeWords(TLArcs, currword, numBestPath, beamWidth, TL, G, inputWords, TLGRatio);
		for (i = 0; i < numBestPath; i++) {
			inputWords[2*i] = TotalResult[2*i];
			inputWords[2*i+1] = TotalResult[2*i+1];
			inputWords[2*i + 2 * numBestPath] = inputWords[2*i+1+2*numBestPath];
			inputWords[2*i + 2 * numBestPath + 1] = TotalResult[i+2*numBestPath];
		}
		
		Input train ("../word_symbols.txt");
		for (i = 0; i < TotalResult[4*numBestPath]; i++) {
			train.getNext();
			train.getNext();
		}
		cout << train.getNext() << " ";
		train.close();
	}
	Input train ("../word_symbols.txt");

	for (i = 0; i < TotalResult[0]; i++) {
		train.getNext();
		train.getNext();
	}
	cout << train.getNext() << " ";
	train.close();
	Input train2 ("../word_symbols.txt");
	for (i = 0; i < TotalResult[1]; i++) {
		train2.getNext();
		train2.getNext();
	}
	cout << train2.getNext() << endl;
	train2.close();
	
	printf("inFile took %f seconds.\n", inFileTime.getClock());

}

void outWeight(string firstWord, string secondWord, string thirdWord, int numBestPath, int beamWidth, int TLGRatio) {
	Timer initializing;
	Timer inFileTime; 
	ofstream outFile; // Note:Needed?
	int outString[1000]; //Note: Size of output Necessary for some reason???????
	float *TotalResult = new float[4*numBestPath+1];
	float *inputWords = new float[4*numBestPath];
	int currState;
	int firstNum = SYMBOL.Word->Find(firstWord);
	int secondNum = SYMBOL.Word->Find(secondWord);
	
	int i = 0;
	int j = 0;
	
	StdVectorFst input;
	StdVectorFst result_fst;
	StdFst *G = StdFst::Read("../G/train.3.lm.fst");
	VectorFst<StdArc> *TL = VectorFst<StdArc>::Read("../TL.fst");
	StateId state_id;
	
	for (i = 0; i < 1000; i++) { //Note: get rid of for loop?
		outString[i] = -1;
	}
	
	MutableArcIterator<VectorFst<StdArc> > *TLArcs[44694];
	
	for (StateIterator<StdFst> siter(*TL); !siter.Done(); siter.Next()) 
	{
		state_id = siter.Value();
		i = -1; //Keeps track of how many Nexts were perforemed
		//i.e. which specific arc are we looking for
		for (ArcIterator<StdFst> aiter((*TL), state_id); !aiter.Done(); aiter.Next())
		{
			i++;
			const StdArc &arc = aiter.Value();
			if (arc.olabel > 3) {
				TLArcs[arc.olabel] = new MutableArcIterator<VectorFst<StdArc> > ((TL), state_id);
				for (j = 0; j < i; j++) 
					TLArcs[arc.olabel]->Next();
			}
			
		}
	}
	
	printf("Initializing took %f seconds.\n", initializing.getClock());
	
	for (i = 0; i < numBestPath; i++) {
		inputWords[2*i] = firstNum;
		inputWords[2*i+1] = secondNum;
		inputWords[2*numBestPath+2*i] = i*100;
		inputWords[2*numBestPath+2*i+1] = i*100;		
	}
	TotalResult = composeThreeWords(TLArcs, thirdWord,numBestPath,beamWidth,TL,G,inputWords, TLGRatio);
	for (i = 0; i < numBestPath; i++) {
		Input train ("../word_symbols.txt");
		for (j = 0; j < TotalResult[2*i+1]; j++) {
			train.getNext();
			train.getNext();
		}
		cout << "word:" << train.getNext() << "\tweight:" << TotalResult[3*numBestPath+i] << endl;
	}
	printf("outWeight took %f seconds.\n", inFileTime.getClock());

	return;
}



int main() {
//	inFile("input.txt", 10, 10, 5);
	outWeight("a","b","colsed",10,10,5);
	return 0;
}
