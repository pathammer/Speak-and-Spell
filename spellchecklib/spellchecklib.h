/*
 * spellchecklib.h
 *
 *  Created on: Dec 17, 2010
 *      Author: kkb110
 */

#ifndef SPELLCHECKLIB_H_
#define SPELLCHECKLIB_H_

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <fst/fstlib.h>
#include <fst/queue.h>
#include <vector>

using namespace std;
using namespace fst;

struct BeamSearchStateEquivClass;
typedef GenericComposeStateTable<StdArc, IntegerFilterState<signed char> > StateTable;
typedef NaturalPruneQueue<NaturalShortestFirstQueue<int, TropicalWeight> , TropicalWeight,
		BeamSearchStateEquivClass> Queue;
typedef AnyArcFilter<StdArc> ArcFilter;
typedef StdArc::StateId StateId;

struct BeamSearchStateEquivClass {
public:
	explicit BeamSearchStateEquivClass(const StateTable &state_table) :
		state_table_(state_table) {
	}

	int operator()(int s) const {
		return state_table_.Tuple(s).state_id1;
	}

private:
	const StateTable &state_table_;
};

///////////////////////////////////////////////////////////////

struct Symbol {
	SymbolTable *Letter;
	SymbolTable *Word;
	Symbol() {
		Letter = SymbolTable::ReadText("../letter_symbols.txt", false);
		Word = SymbolTable::ReadText("../word_symbols.txt", false);

	}
};
extern Symbol SYMBOL;

struct WordWeight {
	WordWeight() {
		word = 0;
		weight = 0;
	}
	int64 word;
	float weight;
};
struct WordTLWeightGWeight {
	WordTLWeightGWeight() {
		word = 0;
		TLweight = 0;
		Gweight = 0;
	}
	int64 word;
	float TLweight;
	float Gweight;
};

//Converts a string into an FST that outputs its characters one at a time
extern VectorFst<StdArc> StringToFst(const char* str);

extern ofstream myfile;
extern StdFst *G;
extern VectorFst<StdArc> *TL;
extern MutableArcIterator<VectorFst<StdArc> > *TLArcs[44694];

extern void init();
extern void cleanup();
extern vector<WordWeight> TLNShortestPath(string currword, int numBestPath, int beamWidth,
		float TLGRatio);
extern vector<WordTLWeightGWeight> ComputeCandidateWords(vector<vector<WordWeight> > previousData,
		string currword, int numBestPath, int beamWidth, float TLGRatio);
extern vector<WordWeight> GetSingleWordCandidates(vector<string> prevWords, string curWord,
		float TLGRatio);
extern vector<vector<string> > RefineText(string text);
extern void GetBestSentenceCandidate(vector<string> sentence, float TLGRatio);
#endif /* SPELLCHECKLIB_H_ */

