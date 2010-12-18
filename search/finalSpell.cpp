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
#include <vector>

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

//Converts a string into an FST that outputs its characters one at a time
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

StdFst *G = StdFst::Read("../G/train.3.lm.fst");
VectorFst<StdArc> *TL = VectorFst<StdArc>::Read("../TL.fst");
MutableArcIterator<VectorFst<StdArc> > *TLArcs[44694];

void init() {
	Timer initializing; 
	StateId state_id;
	int i = 0;
	int j = 0;
	
	//Builds an array of MutableArcIterators so that we can modify TL (needed for
	//finding n best paths)
	
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

}

//Input: string
//Output:
//		word.0, weight.0, ..., word.n, weight.n
vector<float> TLShortestPath(MutableArcIterator<VectorFst<StdArc> > *TLArcs[44694], string currword, int numBestPath, int beamWidth, VectorFst<StdArc> *TL) {
	int i = 0;
	int remWord;
	float wordWeight = 0;
	vector<float> result (2*numBestPath);
	int prevPaths[numBestPath];
	float prevWeights[numBestPath];

	VectorFst <StdArc> I = StringToFst(currword.c_str());
	VectorFst < StdArc > ofst;
	StateId state_id;
	
	//Basically, this program finds a best path, then removes that edge from ToL,
	//Then finds a new best path, and repeats, and then puts the edges back in
	//Note: We had to do it th
	for (i = 0; i < numBestPath; i++) {

		wordWeight = 0;
		
		//On the fly composition
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
		//extract word and weight
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
		//remember the number corresponding to the word and its original weight
		prevPaths[i] = remWord;
		prevWeights[i] = wordWeight;
		result[2*i] = remWord;
		result[2*i+1] = wordWeight;
		//Removing word from TL (just gives it a very large weight so the path
		//won't be taken again)
		const StdArc &arc = (*TLArcs[remWord]).Value();
		StdArc arc2 = arc;
		arc2.weight = 99999;
		(*TLArcs[remWord]).SetValue(arc2);
	}
	//Restore the original weights so that the same word can be chosen again later
	for (i = 0; i < numBestPath; i++) {
		const StdArc &arc = (*TLArcs[prevPaths[i]]).Value();
		StdArc arc2 = arc;
		arc2.weight = prevWeights[i];
		(*TLArcs[prevPaths[i]]).SetValue(arc2);
	}
	
	return result;
}

//Basically the same as composeThreeWords
//Input: a word (the 2nd word), and a list of choices and weights for the 1st word
//Output: 
// word1.0, word2.0,..., word1.n, word2.n
//		weight2.0, ..., weight2.n (note: edit-distance)
vector<float> composeTwoWords(MutableArcIterator<VectorFst<StdArc> > *TLArcs[44694], string currword, int numBestPath, int beamWidth, VectorFst<StdArc> *TL, StdFst *G, float *inWords, int TLGRatio) {
	StateId state_id;
	int state_value;
	int start_states[numBestPath];
	int firstWord[numBestPath];
	float firstWeight[numBestPath];
	float weights[numBestPath];
	int words[numBestPath];
	int currState;
	vector<float> result(numBestPath*3);

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
	
	vector<float> TLResult (2*numBestPath);
	TLResult = TLShortestPath(TLArcs, currword, numBestPath, beamWidth, TL);
	
	for (i = 0; i < numBestPath; i++) {
		words[i] = TLResult[2*i];
		weights[i] = TLResult[2*i+1];
	}
	
	//create an FST with every pair of words, keeping the weights from
	//the edit-distance part
	
	Timer ITLModify;
	
	result_fst.AddState();
	result_fst.SetStart(0);
	
	for (i = 0; i < numBestPath; i++) {
		currState = result_fst.AddState();
		result_fst.AddArc(0, StdArc(0, firstWord[i], firstWeight[i], currState));
		start_states[i] = currState;
	}
	
	int finalState = result_fst.AddState();
	result_fst.SetFinal(finalState,0);
	
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
	RmEpsilon(&input);
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
					if (i%2 == 1)
						//Set the ED-weight of the word (Note: This is 
						//still maintained in the composition)
						result[2*numBestPath+i/2] = arc2.weight.Value();					
				}
				currState = arc2.nextstate;
			}
		}
	}
	
	return result;
	
}

//Input: a word (the 3rd word), and a list of choices and weights for the 1st and 2nd word
//Output: 
// word2.0, word3.0,..., word2.n, word3.n
//		weight3.0, ..., weight3.n (note: edit-distance)
//		Gweight.0, ..., Gweight.n (note: weight for a triple of words from 
//		edit-distance AND from G
//		Best choice for word1
vector<float> composeThreeWords(MutableArcIterator<VectorFst<StdArc> > *TLArcs[44694], string currword, int numBestPath, int beamWidth, VectorFst<StdArc> *TL, StdFst *G, float *inWords, int TLGRatio) {
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
	vector<float> result(numBestPath*4 + 1);
	
	int done = 0;	
	int i = 0;
	int j = 0;
	
	StdVectorFst result_fst;
	StdVectorFst input;
	
	//seperate information of the lists of words and their weights
	for (i = 0; i < numBestPath; i++) {
		firstWord[i] = *(inWords+2*i);
		secondWord[i] = *(inWords+2*i+1);
		firstWeight[i] = *(inWords+2*i+2*numBestPath);
		secondWeight[i] = *(inWords+2*i+1+2*numBestPath);
		result[2*numBestPath+i] = 0;
	}	
	
	VectorFst<StdArc> inputFST = StringToFst(currword.c_str());
	vector<float> TLResult (2*numBestPath);
	//Get the choices and weights for the 3rd word
	TLResult = TLShortestPath(TLArcs, currword, numBestPath, beamWidth, TL);
	
	//seperate words and weights from outpur
	for (i = 0; i < numBestPath; i++) {
		words[i] = TLResult[2*i];
		weights[i] = TLResult[2*i+1];
		result[2*numBestPath+i] = TLResult[2*i+1]; //FIXFIXFIXFIXFIXFIXFIXFIX
		result[3*numBestPath+i] = 0;
	}
	
	//create an FST with every pair of words, keeping the weights from TL
	
	Timer ITLModify;
	
	//Build a simple FST that basically outputs any combination of the 3 words
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
	result_fst.SetFinal(finalState,0);
	
	for (i = 0; i < numBestPath; i++) {
		currState = result_fst.AddState();
		result_fst.AddArc(currState, StdArc(0, words[i], weights[i]*TLGRatio, finalState));
		for (j = 0; j < numBestPath; j++) {
			result_fst.AddArc(start_states[j], StdArc(0, 0, 0, currState));
		}
	}
	
//	printf("Modifying ITL took %f seconds.\n", ITLModify.getClock());
	
	Timer GComp;

	//Compose our FST with G and get the best paths
	Compose(result_fst,*G, &input);
	RmEpsilon(&input);
	ShortestPath(input, &result_fst, numBestPath);

//	printf("G comp/shortest path took %f seconds.\n", GComp.getClock());
	

	//Extract words and weights
	i = 0;
	int first = 0; 
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
						if (i%2 == 1)
							//Set the ED-weight of the word (Note: This is 
							//still maintained in the composition)
							result[2*numBestPath+i/2] = arc2.weight.Value();
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

//Takes in a text and outputs the corrected version of the text
void inFile(string inputFile, int numBestPath, int beamWidth, int TLGRatio) {
	Timer inFileTime; 
	int outString[1000];
	float *firstWordList = new float[2*numBestPath];
	vector<float> TLResult (2*numBestPath);
	vector<float> TotalResult (4*numBestPath+1);
	vector<float> inputWordsInput (3*numBestPath);
	float *inputWords = new float[4*numBestPath];
	int currState;
	
	int i = 0;
	int j = 0;

	StdVectorFst input;
	StdVectorFst result_fst;
	StateId state_id;
	

	
	Input inputWord (inputFile);
	//get the next word of the input
	string currword = inputWord.getNext();
	//Get the n best choices for the first word
	TLResult = TLShortestPath(TLArcs, currword, numBestPath, beamWidth, TL);
	//Build a simple FST that has a start state, end state, and a transition between
	//for each of the 10 words with its corresponding weight
	result_fst.AddState();
	result_fst.SetStart(0);
	result_fst.AddState();
	result_fst.SetFinal(1,0);

	for (i = 0; i < numBestPath; i++) {
		result_fst.AddArc(0, StdArc(0, TLResult[2*i], TLResult[2*i+1], 1));
	}
	
	//Compose this FST with G and get shortest paths
	Compose(result_fst,*G,&input);
	ShortestPath(input, &result_fst,numBestPath);
	
	//Extract the words and weights for the choice of the first word
	//Note: firstWordList is in the format
		//word1.0,word1.1,word1.2,...,word1.n
		//weight1.0,weight1.1,...,weight1.n
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

	//get the 2nd word
	currword = inputWord.getNext();
	//get the choices and weights for the 2nd word
	inputWordsInput = composeTwoWords(TLArcs, currword, numBestPath, beamWidth, TL, G, firstWordList, TLGRatio);	
	//Note: inputWords is int eh format
		//word1,0, word2.0, word1.1, word2.1, ..., word1.n, word2.n
		//weight1.0, weight2.0, ..., weight1.n, weight2.n
	for (i = 0; i < numBestPath; i++) {
		inputWords[2*i] = inputWordsInput[2*i];
		inputWords[2*i+1] = inputWordsInput[2*i+1];
		inputWords[2*i+2*numBestPath] = firstWordList[numBestPath+i];
		inputWords[2*i+1+2*numBestPath] = inputWordsInput[2*numBestPath+i];
	}
	//Loop as long as there are more words to read in the file
	while(!inputWord.isEof()) {
		//get the next word
		currword = inputWord.getNext();
		//get the results of composing with ToLoG
		TotalResult = composeThreeWords(TLArcs, currword, numBestPath, beamWidth, TL, G, inputWords, TLGRatio);
		//store the appropriate values in inputWords
		//Note that weight0.x is the same as the previous weight1.x
		for (i = 0; i < numBestPath; i++) {
			inputWords[2*i] = TotalResult[2*i];
			inputWords[2*i+1] = TotalResult[2*i+1];
			inputWords[2*i + 2 * numBestPath] = inputWords[2*i+1+2*numBestPath];
			inputWords[2*i + 2 * numBestPath + 1] = TotalResult[i+2*numBestPath];
		}
		//Print out the word that has now been determined (from the 1st list of 
		//words that was passed into composeThreeWords
		Input train ("../word_symbols.txt");
		for (i = 0; i < TotalResult[4*numBestPath]; i++) {
			train.getNext();
			train.getNext();
		}
		cout << train.getNext() << " ";
		train.close();
	}
	//Output the current best choice for the last 2 words
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

//This function takes 3 words as input and outputs the top choices for the 3rd word and their weights based on the first 2 words
void outWeight(string firstWord, string secondWord, string thirdWord, int numBestPath, int beamWidth, int TLGRatio) {
	Timer inFileTime; 
	int outString[1000];
	vector<float> TotalResult(4*numBestPath+1);
	float *inputWords = new float[4*numBestPath];
	int currState;
	//Get the number corresponding to the first word
	int firstNum = SYMBOL.Word->Find(firstWord);
	if (firstNum == -1) {
		printf("%s is an incorrect word\n",firstWord.c_str());
		return;
	}
	//Get the number corresponding to the first word
	int secondNum = SYMBOL.Word->Find(secondWord);
	if (secondNum == -1) {
		printf("%s is an incorrect word\n",secondWord.c_str());
		return;
	}	
	int i = 0;
	int j = 0;
	
	StdVectorFst input;
	StdVectorFst result_fst;
	StateId state_id;
		
	for (i = 0; i < numBestPath; i++) {
		//represents the list of n first words (all the same first word in this case)
		inputWords[2*i] = firstNum;
		//represents the list of n second words (all the same second word in this case)
		inputWords[2*i+1] = secondNum;
		//Set the weights (basically makes it so that only 1 pair of first/second words will be traversed by shortest distance
		inputWords[2*numBestPath+2*i] = i*1000;
		inputWords[2*numBestPath+2*i+1] = i*1000;		
	}
	//Put the 3 words together
	TotalResult = composeThreeWords(TLArcs, thirdWord,numBestPath,beamWidth,TL,G,inputWords, TLGRatio);
	//Print out the word and its weight
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
	init();
	inFile("input.txt", 10, 10, 5);
	outWeight("b","a","colsed",3,10,0);
	inFile("input.txt", 10, 5, 5);
	inFile("input.txt", 10, 20, 5);
	inFile("input.txt", 10, 5, 10);
	return 0;
}
