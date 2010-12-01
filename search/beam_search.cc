#include <string>
#include <vector>
#include "nlp/fst/lib/compose-inl.h"
#include "nlp/fst/lib/fst-decl.h"
#include "nlp/fst/lib/shortest-path-inl.h"
#include "nlp/fst/lib/vector-fst-inl.h"

namespace nlp_fst {

// Typedefs for readability.
class BeamSearchStateEquivClass;
typedef GenericComposeStateTable<
  StdArc,
  IntegerFilterState<signed char> > StateTable;
typedef NaturalPruneQueue<
  NaturalShortestFirstQueue<int, TropicalWeight>,
  TropicalWeight,
  BeamSearchStateEquivClass> Queue;
typedef AnyArcFilter<StdArc> ArcFilter;

// \class BeamSearchStateEquivClass
// Functor that maps each composition state to the state in the first
// operand of the composition. This is used in combination with PruneQueue
// to perform a beam search. PruneQueue will discard states in
// the same equivalence class whose potential is further than the beam
// from the best potential currently in the equivalence class.
struct BeamSearchStateEquivClass {
 public:
  explicit BeamSearchStateEquivClass(const StateTable &state_table)
      : state_table_(state_table) {}

  int operator()(int s) const {
    return state_table_.Tuple(s).state_id1;
  }

 private:
  const StateTable &state_table_;
};

// \class BeamSearchExample
// Code to exemplify search with different beam values.
class BeamSearchExample {
 public:
  void Run(const string &in1,
           const string &in2,
           const string &result) {
    // Load input fsts.
    LOG(INFO) << "Loading " << in1;
    scoped_ptr<StdFst> fst1(CHECK_NOTNULL(StdVectorFst::Read(in1)));
    LOG(INFO) << "Loading " << in2;
    scoped_ptr<StdFst> fst2(CHECK_NOTNULL(StdVectorFst::Read(in2)));

    // Perform beam searches for increasing beam values.
    double beam_values[] = { 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 1e9 };
    for (int i = 0; i < 8; ++i) {
      RunSearchWithBeam(*fst1, *fst2, beam_values[i],
                        StringPrintf("%s-%f",
                                     result.c_str(), beam_values[i]));
    }
  }

 private:
  void RunSearchWithBeam(const StdFst &fst1,
                         const StdFst &fst2,
                         double beam,
                         const string result) {
    // Setup delayed composition.
    LOG(INFO) << "Setting up search";
    ComposeFstOptions<StdArc> copts;
    // copts.gc: defaults to --fst_default_cache_gc.
    // copts.gc_limit: defaults to --fst_default_cache_gc_limit.
    copts.state_table = new StateTable(fst1, fst2);  // Owned by composition.
    ComposeFst<StdArc> composition(fst1, fst2, copts);

    // Setup a PrunedQueue for shortest path.
    vector<TropicalWeight> distance;
    Queue state_queue(distance,
                      new NaturalShortestFirstQueue<
                      StdFst::StateId, TropicalWeight>(distance),
                      BeamSearchStateEquivClass(*copts.state_table),
                      beam);

    // Compute shortest path.
    StdVectorFst result_fst;
    ShortestPathOptions<StdArc, Queue, ArcFilter >
        shortest_path_opts(&state_queue, ArcFilter());
    shortest_path_opts.first_path = true;
    LOG(INFO) << "Performing search with beam " << beam;
    WallTime t = WallTime_Now();
    nlp_fst::ShortestPath(composition,
                          &result_fst,
                          &distance,
                          shortest_path_opts);
    LOG(INFO) << "Search took " <<  WallTime_Now() - t << " sec";
    LOG(INFO) << "Writing result";
    CHECK(result_fst.Write(result));
    LOG(INFO) << "Done!";
  }
};

}  // namespace nlp_fst

int main(int argc, char **argv) {
  string usage = "Composes two fsts and then runs shortest path using a\n";
  usage += "specified beam\n\n";
  usage += "Usage: \n  ";
  usage += argv[0];
  usage += " in1.fst in2.fst";
  usage += " result.fst";
  InitGoogle(usage.c_str(), &argc, &argv, true);
  File::Init();
  if (argc != 4) {
    ShowUsageWithFlagsRestrict(argv[0], "beam_search");
    return 1;
  }
  nlp_fst::BeamSearchExample example;
  example.Run(argv[1], argv[2], argv[3]);
  return 0;
}
