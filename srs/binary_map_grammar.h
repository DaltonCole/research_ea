#ifndef BINARY_MAP_GRAMMAR
#define BINARY_MAP_GRAMMAR

#include <utility>
#include <unordered_map>
//#include <regex>
#include <vector>
#include <cstdint>
#include <memory>
#include "dfa.h"
#include "base_grammar.h"
#include "code_coverage.h"
using namespace std;

class Binary_map_grammar : public Base_grammar {
	public:
		// Single example input
		Binary_map_grammar(const string& input);
		// Multiple example input
		Binary_map_grammar(const vector<string>& inputs);

		virtual vector<string> generate_strings() const;
		virtual float find_fitness();
		virtual float get_fitness() const;

		virtual void mutate();

		virtual void abstract();

		virtual pair<shared_ptr<Base_grammar>, shared_ptr<Base_grammar> > recombination(const shared_ptr<Base_grammar>& mate) const;

		virtual vector<string> generate_sample_strings();

		void print(ostream&) const;

	private:
		unordered_map<uint32_t, vector<vector<uint32_t> > > grammar;
		float fitness;

		static const int words_generated_count;
		// NOTE: Might pre-compute this so minor binary changes correlate to minor ascii changes
		static unordered_map<char, uint32_t> char_to_binary_mapping; 
		static unordered_map<uint32_t, Dfa> binary_to_regex_mapping;
		static uint32_t next_available_mapping;

		// Common regular expressions
		static const vector<Dfa> common_patterns;

		// Start symbol
		static const uint32_t start_symbol;
};

#endif