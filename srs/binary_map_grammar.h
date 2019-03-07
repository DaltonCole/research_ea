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
#include "grammar_probability.h"
using namespace std;

class Binary_map_grammar : public Base_grammar {
	public:
		//////////////////////////////////////////////////////////////////////
		/// @brief 	Default constructor. 
		/// 		<ul> <li> Adds Binary_map_grammar::common_patterns to 
		/// 		binary_to_regex_mapping </ul>
		/// @post 	<ul> <li> fitness is set to -9999
		/// 		<li> binary_to_regex_mapping contains the elements from 
		/// 		common_patterns
		///			<li> next_available_mapping is set to common_patterns.size()
		//////////////////////////////////////////////////////////////////////
		Binary_map_grammar();

		//////////////////////////////////////////////////////////////////////
		/// @brief 	Constructor for a single of string.
		/// 		<ul> <li> Adds Binary_map_grammar::common_patterns to 
		/// 		binary_to_regex_mapping. 
		/// 		<li> Turns each string into a grammar rule consisting of 
		/// 		A -> 'c'B where A anb B are non-terminals and 'c' is a 
		/// 		single character from the input. B will contain the 
		/// 		following character in the input. </ul>
		/// @post	<ul> <li> fitness is set to -9999
		/// 		<li> binary_to_regex_mapping contains the elements from 
		/// 			common_patterns
		/// 		<li> next_available_mapping is set appropriately
		/// 		<li> grammar is populated </ul>
		/// @param[in] 	input 	Input string to initialize the grammar rules with
		//////////////////////////////////////////////////////////////////////
		Binary_map_grammar(const string& input);
		
		//////////////////////////////////////////////////////////////////////
		/// @brief 	Constructor for a vector of strings.
		/// 		<ul> <li> Adds Binary_map_grammar::common_patterns to 
		/// 		binary_to_regex_mapping. 
		/// 		<li> Turns each string into a grammar rule consisting of 
		/// 		A -> 'c'B where A anb B are non-terminals and 'c' is a 
		/// 		single character from the input. B will contain the 
		/// 		following character in the input. 
		/// 		<li> Each new string restarts as a possible start symbol.
		///			</ul>
		/// @post	<ul> <li> fitness is set to -9999
		/// 		<li> binary_to_regex_mapping contains the elements from 
		/// 			common_patterns
		/// 		<li> next_available_mapping is set appropriately
		/// 		<li> grammar is populated </ul>
		/// @param[in] 	input 	Input strings to initialize the grammar 
		/// 					rules with
		//////////////////////////////////////////////////////////////////////
		Binary_map_grammar(const vector<string>& inputs);

		virtual shared_ptr<Base_grammar> clone() const;

		virtual unordered_set<string> generate_strings();

		//////////////////////////////////////////////////////////////////////
		/// @brief 	Recursively generates a single string using the grammar
		/// 		Once depth reaches max_string_depth, an epsilon will be 
		/// 		returned instead of using possible rules. Due to depth limit
		/// 		the generated string my not be from the grammar.
		/// @param[in]	word 	Word that is being added to.
		/// @param[in]	depth 	The current recursive depth
		/// @param[in]	non_terminal 	The non-terminal associated with the
		/// 		list of possible production rules.

		//////////////////////////////////////////////////////////////////////
		void generate_string(string& word, const int depth, const uint32_t non_terminal);

		virtual std::thread find_fitness_thread();
		virtual float find_fitness();
		virtual float get_fitness();

		virtual void mutate();

		//////////////////////////////////////////////////////////////////////
		/// @brief 	Select a random symbol from list of possible symbols
		/// 		All symbols are mapped from a character or regex to a
		/// 		uint32_t number. A uint32_t number is returned. 
		/// 		binary_to_regex_mapping map can be used to decode the 
		/// 		number back to the symbolic form.
		/// @return A random int between [0, next_available_mapping)
		//////////////////////////////////////////////////////////////////////
		uint32_t random_term() const;

		//////////////////////////////////////////////////////////////////////
		/// @brief 	Select a random non-terminal from the keys of "grammar"
		/// @return A random non-terminal in the grammar
		//////////////////////////////////////////////////////////////////////
		uint32_t random_non_term() const;

		virtual void abstract(const bool guarantee_abstract = false);

		// --- Abstract helpers --- //
		//////////////////////////////////////////////////////////////////////
		/// @brief	Attempts to condense grammar rules
		/// 		Condensing happens in the following way:
		/// 		S -> aQ; Q -> aR
		///			Try to condense to S -> aS | aR
		/// @post 	Grammar will be condensed according to the above example
		//////////////////////////////////////////////////////////////////////
		// Attempt to condense rules adhering to the following example:
		void condense_repetition();
		//////////////////////////////////////////////////////////////////////
		/// @brief	Any empty rules (i.e. an epsilon rule) is deleted
		/// @post 	Grammar contains no empty rules
		//////////////////////////////////////////////////////////////////////
		void remove_rules_only_containing_epsilon();
		//////////////////////////////////////////////////////////////////////
		/// @brief	Rules that are only a redirect (only contain a single
		/// 		rule, which is a non-terminal), alters this redirect to
		/// 		get rid of the middle man. Only applies to rules with
		/// 		a single non-terminal
		/// 		Example: A -> aB; B -> C;
		/// 		Turns into: A -> AC and B is deleted
		/// @post 	Redirect is removed, redirect rule is deleted
		//////////////////////////////////////////////////////////////////////
		void remove_rules_only_containing_a_non_terminal();

		//////////////////////////////////////////////////////////////////////
		/// @brief	Any rules that cannot be reached from start state is
		/// 		deleted.
		/// @post 	Unreachable grammar rules were removed
		//////////////////////////////////////////////////////////////////////
		void eliminate_dead_rules();
		//////////////////////////////////////////////////////////////////////
		/// @brief	Helps "eliminate_dead_rules()". Recursively calls itself
		/// 		to "mark" all rules accessible from the "non_termial"
		/// 		parameter. Marked rules are added to "touch_rules" param.
		/// @post 	The "touched_rules" parameter contains all non-terminals
		/// 		that are reachable from the start state (or what ever state
		/// 		"non_terminal" initially contains).
		/// @param[in] 	non_terminal 	Current rule to expand all possible
		/// 		paths from.	
		/// @param[in, out]	touched_rules	Rules that have been expanded and 
		/// 		are reachable from the start rule. This should initially
		/// 		contain the start symbol.
		//////////////////////////////////////////////////////////////////////
		void eliminate_dead_rules_helper
		(const uint32_t non_terminal, unordered_set<uint32_t>& touched_rules);

		//////////////////////////////////////////////////////////////////////
		/// @brief	Remove duplicate rules in each rule set
		/// @post	No duplicate rules (rhs) exist in each rule set
		//////////////////////////////////////////////////////////////////////
		void remove_duplicate_rules();

		//////////////////////////////////////////////////////////////////////
		/// @brief	Compares the given rules. Returns true if lhs is less than
		///			rhs, false otherwise. Less than is considered shorter rules
		/// 		or smaller uint32_t's starting from the front.
		/// 		Used for std::sort()
		/// @param[in]	lhs 	First comparitor.
		/// @param[in] 	rhs 	Second comparitor.
		/// @return	True if lhs is less than or equal to rhs, false otherwise.
		//////////////////////////////////////////////////////////////////////
		friend bool sort_rule_sets(const vector<uint32_t> lhs, const vector<uint32_t> rhs);

		//////////////////////////////////////////////////////////////////////
		/// @brief	True if lhs == rhs. Used for std::unique.
		/// @param[in]	lhs 	First comparitor.
		/// @param[in] 	rhs 	Second comparitor.
		/// @return	True if lhs == rhs. False otherwise.
		//////////////////////////////////////////////////////////////////////
		friend bool unique_rule_set(const vector<uint32_t> lhs, const vector<uint32_t> rhs);

		//////////////////////////////////////////////////////////////////////
		/// @brief	Removes symbols that do not exist in DFA map nor
		/// 		in the grammar as non-terminals
		//////////////////////////////////////////////////////////////////////
		void remove_non_existant_symbols();
		// ------------------------ //


		// --- Recombination --- //
		virtual vector<shared_ptr<Base_grammar> > 
		recombination(const vector<shared_ptr<Base_grammar> >& mates,
			const string& method, const uint number_of_children);

		//////////////////////////////////////////////////////////////////////
		/// @brief 	Combines *this with mates to produce a child. The child
		/// 		contains all of the parent's rules.
		/// @param[in]	mates 	The other parents
		/// @return	A child that has all the rules of the parents
		//////////////////////////////////////////////////////////////////////
		shared_ptr<Base_grammar> recombination_single(const vector<shared_ptr<Base_grammar> >& mates);

		//////////////////////////////////////////////////////////////////////
		/// @brief 	Combines *this with mate to produce two children. Each 
		/// 		child shares some gnomes from both parents.
		/// @param[in]	mates 	The other parent
		/// @return	A pair of children. 
		//////////////////////////////////////////////////////////////////////
		pair<shared_ptr<Base_grammar>, shared_ptr<Base_grammar> > 
		recombination_twins(const shared_ptr<Base_grammar>& mate);
		///////////////////////////

		//////////////////////////////////////////////////////////////////////
		/// @brief	Prints out the grammar to ostream.
		/// @post 	"os" contains the printed out grammar.
		/// @param[out]	os 	Ostream to output grammar to.
		//////////////////////////////////////////////////////////////////////
		void print(ostream& os) const;

		friend class Grammar_probability;

	private:
		unordered_map<uint32_t, vector<vector<uint32_t> > > 
		grammar; 	///< The language's grammar
					///< Ordered in <non-terminal> <Rule Set> order where each rule
					///< set is a combination of terminal and non-terminal terms.
					///< binary_to_regex_mapping maps the uint32_t to its regex
					///< counterpart.

		Grammar_probability gram_prob;

		// NOTE: Might pre-compute this so minor binary changes correlate to minor ascii changes
		static unordered_map<char, uint32_t> 
		char_to_binary_mapping;	///< When generating the initial grammar, remember what character
								///< goes with what mapping
		static unordered_map<uint32_t, Dfa> 
		binary_to_regex_mapping;	///< Mapping of uint32_t symbol to corresponding regex
		static uint32_t next_available_mapping;	///< Next symbol to use.
												///< Incremented any time a new symbol is generated.

		static const vector<Dfa> common_patterns;	///< Pre-computed common regular expressions

		static const uint32_t start_symbol;	///< The start symbol for the grammar. Will 
											///< generally be 0.

		static int max_string_depth;	///< Max recursive depth to generate sample
										///< strings from.
		bool max_depth_reached;	///< If max string depth was reached

		static int max_failed_attempts; 	///< Attempt to generate a string this many times
											///< before giving up. If we give up, then the max
											///< fitness is reduced since we have fewer
											///< samples to test.
};

#endif