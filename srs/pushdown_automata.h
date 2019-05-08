#ifndef PUSHDOWN_AUTOMATA
#define PUSHDOWN_AUTOMATA

#include <memory>
#include <vector>
#include <unordered_set>
#include <stack>
#include <stdlib.h>     /* srand, rand */
#include <utility>      // std::pair, std::make_pair
#include "dfa.h"
#include "pushdown_automata_node.h"

class Pushdown_automata {
	public:

		Pushdown_automata();

		Pushdown_automata(const string& input);

		pair<string, bool> generate_string();

		vector<string> generate_strings(uint number_of_strings);

		vector<shared_ptr<Pushdown_automata_edge> > possible_next_node_edges(
			const shared_ptr<Pushdown_automata_node>& current_node, const uint32_t* stack_top);

		shared_ptr<Pushdown_automata> clone();

	private:
		shared_ptr<Pushdown_automata_node> start_node;
		unordered_set<shared_ptr<Pushdown_automata_node> > nodes;

		// NOTE: I'll need to add to Dfa to see if read-in string is accepted by Dfa
		unordered_set<Dfa> input_alphabet;
		vector<uint32_t> stack_alphabet;

		static uint max_failed_attempts; 	///< Attempt to generate a string this many times
											///< before giving up. If we give up, then the max
											///< fitness is reduced since we have fewer
											///< samples to test.
};

#endif