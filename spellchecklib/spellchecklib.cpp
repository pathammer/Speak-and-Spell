/*
 * spellchecklib.cpp
 *
 *  Created on: Dec 17, 2010
 *      Author: kkb110
 */
#include "spellchecklib.h"

class Timer {
	clock_t init, final;
public:
	Timer() {
		init = clock();
	}
	double getClock(string text) {
		cout << ((double) clock() - init) / ((double) CLOCKS_PER_SEC) << "\t" << text << "\n";
		return ((double) clock() - init) / ((double) CLOCKS_PER_SEC);
	}
	void wait(int seconds) {
		clock_t endwait;
		endwait = clock() + seconds * CLOCKS_PER_SEC;
		while (clock() < endwait) {
		}
	}
};

Symbol SYMBOL;
VectorFst<StdArc> StringToFst(const char* str) {
	VectorFst<StdArc> result;
	VectorFst<StdArc>::StateId si1;
	VectorFst<StdArc>::StateId si2;

	si1 = result.AddState();
	si2 = si1;
	result.SetStart(si1);
	for (char *ptr = (char*) str; *ptr != '\0'; ++ptr) {
		si2 = result.AddState();
		char letter[2] = { *ptr, '\0' };
		VectorFst<StdArc>::Arc
				arc(SYMBOL.Letter->Find(letter), SYMBOL.Letter->Find(letter), 0, si2);
		result.AddArc(si1, arc);
		si1 = si2;
	}
	si2 = result.AddState();

	VectorFst<StdArc>::Arc arc(SYMBOL.Letter->Find("</w>"), SYMBOL.Letter->Find("</w>"), 0, si2);
	result.AddArc(si1, arc);
	si1 = si2;

	si2 = result.AddState();

	VectorFst<StdArc>::Arc arc2(SYMBOL.Letter->Find("</s>"), SYMBOL.Letter->Find("</s>"), 0, si2);
	result.AddArc(si1, arc2);
	si1 = si2;

	result.SetFinal(si2, 0);

	//result.Write("test.fst");// for debug

	return result;

}

ofstream myfile;
StdFst *G = StdFst::Read("../G/train.3.lm.fst");
VectorFst<StdArc> *TL = VectorFst<StdArc>::Read("../TL_swap.fst");
MutableArcIterator<VectorFst<StdArc> > *TLArcs[44694] = { 0, };

void init() {
	//	Timer initializing;
	StateId state_id;
	int i = 0;
	int j = 0;

	//Builds an array of MutableArcIterators so that we can modify TL (needed for
	//finding n best paths)

	for (StateIterator<StdFst> siter(*TL); !siter.Done(); siter.Next()) {
		state_id = siter.Value();
		i = -1; //Keeps track of how many Nexts were perforemed
		//i.e. which specific arc are we looking for
		for (ArcIterator<StdFst> aiter((*TL), state_id); !aiter.Done(); aiter.Next()) {
			i++;
			const StdArc &arc = aiter.Value();
			if (arc.olabel > 3) {
				TLArcs[arc.olabel] = new MutableArcIterator<VectorFst<StdArc> > ((TL), state_id);
				for (j = 0; j < i; j++)
					TLArcs[arc.olabel]->Next();
			}
		}
	}
	//	printf("Initializing took %f seconds.\n", initializing.getClock());
}

void cleanup() {
	for (int i = 0; i < 44694; ++i) {
		delete TLArcs[i];
		TLArcs[i] = 0;
	}
}
//Input: string
//Output:
//		word.0, weight.0, ..., word.n, weight.n
vector<WordWeight> TLNShortestPath(string currword, int numBestPath, int beamWidth) {
	//VectorFst<StdArc> TLlocal = *TL;
	int i = 0;
	int remWord;
	float wordWeight = 0;
	//vector<float> result(2 * numBestPath);
	vector<WordWeight> result(numBestPath);
	int prevPaths[numBestPath];
	float prevWeights[numBestPath];

	VectorFst<StdArc> I = StringToFst(currword.c_str());
	VectorFst<StdArc> ofst;
	StateId state_id;

	//Basically, this program finds a best path, then removes that edge from ToL,
	//Then finds a new best path, and repeats, and then puts the edges back in
	//Note: We had to do it th
	for (i = 0; i < numBestPath; i++) {

		wordWeight = 0;

		//On the fly composition
		ComposeFstOptions<StdArc> copts; //Todo what if we remove this option, does it affect the performance?
		copts.state_table
				= new GenericComposeStateTable<StdArc, IntegerFilterState<signed char> > (I, *TL);
		ComposeFst<StdArc> composed(I, *TL, copts);

		vector<TropicalWeight> distance;
		NaturalPruneQueue<NaturalShortestFirstQueue<int, TropicalWeight> , TropicalWeight,
				BeamSearchStateEquivClass> state_queue(distance, new NaturalShortestFirstQueue<
				StdFst::StateId, TropicalWeight> (distance), BeamSearchStateEquivClass(
				*copts.state_table), beamWidth);
		ShortestPathOptions<StdArc, Queue, ArcFilter> shortest_path_opts(&state_queue, ArcFilter());
		shortest_path_opts.first_path = true;
		//Timer b;
		ShortestPath(composed, &ofst, &distance, shortest_path_opts);
		//b.getClock("actual short");
		//extract word and weight Todo: we can just push.
		for (StateIterator<StdFst> siter(ofst); !siter.Done(); siter.Next()) {
			state_id = siter.Value();
			for (ArcIterator<StdFst> aiter((ofst), state_id); !aiter.Done(); aiter.Next()) {
				const StdArc &arc = aiter.Value();
				TropicalWeight weight = arc.weight;
				wordWeight += weight.Value();
				if ((arc.olabel > 3)) {
					remWord = arc.olabel;
				}

			}
		}
		//remember the number corresponding to the word and its original weight
		prevPaths[i] = remWord;
		prevWeights[i] = wordWeight;
		result[i].word = remWord;
		result[i].weight = wordWeight;
		//Removing word from TL (just gives it a very large weight so the path
		//won't be taken again)
		StdArc arc = (*TLArcs[remWord]).Value();
		prevWeights[i] = arc.weight.Value();
		arc.weight = 99999;//numeric_limits<float>::infinity();
		(*TLArcs[remWord]).SetValue(arc);
	}
	//Restore the original weights so that the same word can be chosen again later
	//Todo clever way to do is remembering what arcs we removed and restores only those.
	for (i = 0; i < numBestPath; i++) {
		StdArc arc = (*TLArcs[prevPaths[i]]).Value();
		arc.weight = prevWeights[i];
		(*TLArcs[prevPaths[i]]).SetValue(arc);
	}

	return result;
}
VectorFst<StdArc> BuildIntermediateFST(vector<vector<WordWeight> > Data, float TLGRatio) {
	int n = Data.size();

	vector<vector<int> > stateMap(Data.size() + 1);

	VectorFst<StdArc> result;
	stateMap[0].push_back(result.AddState());
	result.SetStart(stateMap[0][0]);

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < Data[i].size(); ++j) {
			stateMap[i + 1].push_back(result.AddState());
			for (int k = 0; k < stateMap[i].size(); ++k) {
				result.AddArc(stateMap[i][k], StdArc(0, Data[i][j].word, Data[i][j].weight
						* TLGRatio, stateMap[i + 1][j]));
			}
		}
	}
	for (int i = 0; i < stateMap.back().size(); ++i) {
		result.SetFinal(stateMap.back()[i], 0);
	}
	return result;
}

vector<WordTLWeightGWeight> ComputeCandidateWords(vector<vector<WordWeight> > previousData,
		string currword, int numBestPath, int beamWidth, float TLGRatio) {

	VectorFst<StdArc> result;
	vector<WordWeight> TLResult = TLNShortestPath(currword, numBestPath, beamWidth);
	previousData.push_back(TLResult);
	VectorFst<StdArc> intermediateFST = BuildIntermediateFST(previousData, TLGRatio);
	//intermediateFST.Write("test.fst"); //for debugging

	RmEpsilonFst<StdArc> ITLG(ComposeFst<StdArc> (intermediateFST, *G));
	ShortestPath(ITLG, &result, numBestPath);
	//result.Write("test2.fst"); //for debugging

	VectorFst<StdArc> result2;
	Reverse(result, &result2);
	RmEpsilon(&result2);
	//result2.Write("test3.fst"); //for debugging

	vector<WordTLWeightGWeight> result3;
	for (ArcIterator<StdFst> aiter(result2, 0); !aiter.Done(); aiter.Next()) {
		WordTLWeightGWeight candidate;
		candidate.Gweight = aiter.Value().weight.Value();
		candidate.word = aiter.Value().olabel;
		for (int i = 0; i < previousData.back().size(); ++i) {
			if (previousData.back()[i].word == candidate.word) {
				candidate.TLweight = previousData.back()[i].weight;
				candidate.Gweight -= candidate.TLweight * TLGRatio;
				break;
			}
		}
		result3.push_back(candidate);
	}
	return result3;
}

vector<WordWeight> GetSingleWordCandidates(vector<string> prevWords, string curWord, float TLGRatio) {
	vector<vector<WordWeight> > previousData;
	for (int i = 0; i < prevWords.size(); ++i) {
		previousData.push_back(TLNShortestPath(prevWords[i], 10, 10));
	}
	vector<WordTLWeightGWeight> candidates = ComputeCandidateWords(previousData, curWord, 10, 10,
			TLGRatio);
	vector<WordWeight> result;
	for (int i = 0; i < candidates.size(); ++i) {
		WordWeight tmp;
		tmp.word = candidates[i].word;
		tmp.weight = candidates[i].TLweight * TLGRatio + candidates[i].Gweight;
		//cout << SYMBOL.Word->Find(tmp.word) << "\t" << candidates[i].TLweight << "\t" << candidates[i].Gweight << "\n";
		result.push_back(tmp);
	}
	return result;
}
vector<vector<WordWeight> > Analysis(vector<string> words, float TLGRatio) {
	vector<vector<WordWeight> > candidates2;
	for (int i = 0; i < words.size(); ++i) {
		vector<vector<WordWeight> > previousData;
		for (int i = max(0, (int)candidates2.size() - 3); i < candidates2.size(); ++i) {
			previousData.push_back(candidates2[i]);
		}
		vector<WordTLWeightGWeight> candidates = ComputeCandidateWords(previousData, words[i], 10,
				10, TLGRatio);

		candidates2.push_back(vector<WordWeight> ());
		for (int i = 0; i < candidates.size(); ++i) {
			WordWeight tmp;
			tmp.word = candidates[i].word;
			tmp.weight = candidates[i].TLweight * TLGRatio + candidates[i].Gweight;
			candidates2.back().push_back(tmp);
		}

	}
	return candidates2;
}
vector<string> GetBestSentenceCandidate(vector<string> sentence, float TLGRatio) {
	vector<vector<WordWeight> > sentenceList;
	for (int i = 0; i < sentence.size(); ++i) {
		sentenceList.push_back(TLNShortestPath(sentence[i], 10, 10));
	}
	////////////////////
	VectorFst<StdArc> resultFST;
	VectorFst<StdArc> intermediateFST = BuildIntermediateFST(sentenceList, TLGRatio);
	intermediateFST.Write("testt.fst"); //for debugging
	RmEpsilonFst<StdArc> ITLG(ComposeFst<StdArc> (intermediateFST, *G));
	ShortestPath(ITLG, &resultFST);
	resultFST.Write("testt2.fst");

	vector<string> result;
	int state = resultFST.Start();
	while (resultFST.Final(state) == numeric_limits<float>::infinity()) {
		ArcIterator<VectorFst<StdArc> > aiter(resultFST, state);
		StdArc arc = aiter.Value();
		result.push_back(SYMBOL.Word->Find(arc.olabel));
		state = arc.nextstate;
	}
	return result;
}

vector<vector<string> > RefineText(string text) {
	vector<vector<string> > result;

	result.push_back(vector<string> (1));
	for (int i = 0; i < text.size(); ++i) {

		char chr = text[i];

		if ('A' <= chr && chr <= 'Z')
			chr = chr - 'A' + 'a';

		if ('a' <= chr && chr <= 'z') {
			//cout << chr;
			result.back().back().push_back(chr);//stay
		} else if (chr == '\'') {
			;//delete
		} else if (chr == '.' || chr == '?' || chr == '!' || chr == '\n') {
			result.push_back(vector<string> (1));//finish
		} else {
			if (result.back().back().compare("") != 0) {
				result.back().push_back(string());
			}
		}
	}
	return result;
}
vector<string> RefineText(string text, int index) {
	vector<vector<string> > result;

	result.push_back(vector<string> (1));
	for (int i = 0; i < text.size(); ++i) {
		char chr = text[i];

		if ('A' <= chr && chr <= 'Z')
			chr = chr - 'A' + 'a';

		if ('a' <= chr && chr <= 'z') {
			//cout << chr;
			result.back().back().push_back(chr);//stay
		} else if (chr == '\'') {
			;//delete
		} else if (chr == '.' || chr == '?' || chr == '!' || chr == '\n') {
			result.push_back(vector<string> (1));//finish
		} else {
			if (result.back().back().compare("") != 0) {

				if (i >= index) {
					break;
				}
				result.back().push_back(string());
			}
		}
	}
	int n = result.back().size();
	vector<string> result2;
	for (int i = max(0, n - 3); i < n; ++i) {
		result2.push_back(result.back()[i]);
	}
	return result2;
}
