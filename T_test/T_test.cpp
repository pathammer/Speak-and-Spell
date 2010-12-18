#include <iostream>
#include <fst/fstlib.h>
#include <string>
#include <fst/queue.h>
#include <fst/determinize.h>

using namespace fst;
using namespace std;

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

VectorFst<StdArc> StringToFst(const char str[])
{
	VectorFst < StdArc > result;
	VectorFst<StdArc>::StateId si1;
	VectorFst<StdArc>::StateId si2;

	si1 = result.AddState();
	si2 = si1;
	result.SetStart(si1);
	for (char *ptr = (char*) (str); *ptr != '\0'; ++ptr)
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

class BeamSearchStateEquivClass;
typedef GenericComposeStateTable<StdArc, IntegerFilterState<signed char> >
		StateTable;
typedef NaturalPruneQueue<NaturalShortestFirstQueue<int, TropicalWeight> ,
		TropicalWeight, BeamSearchStateEquivClass> Queue;
typedef AnyArcFilter<StdArc> ArcFilter;


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
int main()
{
	VectorFst < StdArc > I = StringToFst("sanaeta");
	StdFst *TL = StdFst::Read("../TL.fst");

	VectorFst < StdArc > ofst;

	ComposeFstOptions < StdArc > copts;
	copts.state_table = new GenericComposeStateTable<StdArc,
			IntegerFilterState<signed char> > (I, *TL);
	ComposeFst < StdArc > composed(I, *TL, copts);

	vector < TropicalWeight > distance;
	NaturalPruneQueue<NaturalShortestFirstQueue<int, TropicalWeight> ,
			TropicalWeight, BeamSearchStateEquivClass> state_queue(distance,
			new NaturalShortestFirstQueue<StdFst::StateId, TropicalWeight> (
					distance), BeamSearchStateEquivClass(*copts.state_table),
			100);

	ShortestPathOptions<StdArc, Queue, ArcFilter> shortest_path_opts(
			&state_queue, ArcFilter());
	shortest_path_opts.first_path = true;
	//shortest_path_opts.nshortest = 10;
	//shortest_path_opts.weight_threshold = 1;
	//shortest_path_opts.state_threshold = 4;
	//shortest_path_opts.unique = true;
	//DeterminizeFst<StdArc> determinized(composed);
	ShortestPath(composed, &ofst, &distance, shortest_path_opts);
	ofst.Write("test2.fst");
	return 0;
}
