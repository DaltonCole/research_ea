//////////////////////////////////////////////////////////////////////
/// @file 	grammar_probability.h
///			Grammar_probability class file. Used with 
/// 		Binary_map_grammar to cover each production rule in a 
/// 		more uniform fashion.
//////////////////////////////////////////////////////////////////////

#ifndef GRAMMAR_PROBABILITY_H
#define GRAMMAR_PROBABILITY_H

#include <unordered_map>
#include <vector>

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace std;

class Grammar_probability {
	public:
		//////////////////////////////////////////////////////////////////////
		/// @brief	Default constructor
		//////////////////////////////////////////////////////////////////////
		Grammar_probability() {};

		//////////////////////////////////////////////////////////////////////
		/// @brief	Resizes the "occurrences" and "total_occurrences" to fit the
		///			"grammar". Should be called after the grammar 
		/// 		is changed.
		/// @post	"occurrences" and "total_occurrences" correctly fits the
		/// 		size of "grammar". Each non-terminal in "total_occurrences"
		/// 		will be the number of production rules for that 
		/// 		non-terminal. Each production rule in "occurrences" will
		/// 		be 1.
		/// @param[in] 	grammar 	The grammar to fit the occurrences to.
		//////////////////////////////////////////////////////////////////////
		void resize(const unordered_map<uint32_t, vector<vector<uint32_t> > >& grammar);

		//////////////////////////////////////////////////////////////////////
		/// @brief	Returns an index to the rule that should be chosen next.
		/// 		Probabilistically favors rules that have been chosen less.
		/// 		This should give more diverse string generations.
		/// @pre	If grammar has been changed since "resize()" was last 
		/// 		called, "resize()" should be called again.
		/// @post	Increments the corresponding "occurrences" and 
		/// 		"total_occurrences" index.
		/// @param[in] 	non_terminal 	The non-terminal to choose a production
		/// 		rule for.
		/// @return	The index for the production rule chosen out of a vector
		/// 		of possible production rules.
		//////////////////////////////////////////////////////////////////////
		uint choosen_rule_index(const uint32_t non_terminal);

	private:
		unordered_map<uint32_t, vector<uint> > 
		occurrences; 	///< Keeps track of the total number of times each
						///< production rule for a non-terminal has been used.

		unordered_map<uint32_t, uint > 
		total_occurrences; 	///< Keeps track of the total number of times this
							///< non-terminal has been used.
};


#endif