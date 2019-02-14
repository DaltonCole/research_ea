#ifndef BINARY_MAP_GRAMMAR
#define BINARY_MAP_GRAMMAR

#include <utility>
#include <unordered_map>
#include <map>
#include <unordered_set>
//#include <regex>
#include <vector>
#include <cstdint>
#include <memory>
#include <future>
#include <algorithm>
#include "dfa.h"
#include "base_grammar.h"
#include "code_coverage.h"
using namespace std;

class Binary_map_grammar : public Base_grammar {
	public:
		// Default constructor
		Binary_map_grammar();

		// Single example input
		Binary_map_grammar(const string& input);
		// Multiple example input
		Binary_map_grammar(const vector<string>& inputs);

		virtual shared_ptr<Base_grammar> clone() const;

		virtual vector<string> generate_strings();
		string generate_string(const vector<vector<uint32_t> >& rules, const int depth);

		virtual std::thread find_fitness_thread();
		virtual float find_fitness();
		virtual float get_fitness();

		virtual void mutate();

		uint32_t random_term() const;

		virtual void abstract();

		// --- Abstract helpers ---
		void condense_repetition();
		void remove_rules_only_containing_epsilon();
		void remove_rules_only_containing_a_non_terminal();

		void eliminate_dead_rules();
		void eliminate_dead_rules_helper
		(const uint32_t non_terminal, unordered_set<uint32_t>& touched_rules);
		///////////////////////////

		virtual pair<shared_ptr<Base_grammar>, shared_ptr<Base_grammar> > 
		recombination(shared_ptr<Base_grammar>& mate);

		void print(ostream&) const;

	private:
		unordered_map<uint32_t, vector<vector<uint32_t> > > grammar;
		bool max_depth_reached;

		// NOTE: Might pre-compute this so minor binary changes correlate to minor ascii changes
		static unordered_map<char, uint32_t> char_to_binary_mapping; 
		static unordered_map<uint32_t, Dfa> binary_to_regex_mapping;
		static uint32_t next_available_mapping;

		// Common regular expressions
		static const vector<Dfa> common_patterns;

		// Start symbol
		static const uint32_t start_symbol;

		// Max depth for a single rule
		static int max_string_depth;

		// Number of failed attempts allowed when generating a string
		static int max_failed_attempts;
};

#endif