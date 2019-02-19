#ifndef DFA_H
#define DFA_H

#include <iostream>
#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
using namespace std;

class Dfa {
	public:
		//////////////////////////////////////////////////////////////////////
		/// @brief	Constructor generating a regex
		/// @param[in]	input 	Input string to form regex upon
		/// @param[in] 	regex 	Whether to create a "real" regex or just an
		/// 		exact string. A "real" regex would be something along the
		/// 		lines of "[a-zA-Z_]".
		//////////////////////////////////////////////////////////////////////
		Dfa(const string& input, const bool regex = true);

		//////////////////////////////////////////////////////////////////////
		/// @brief Copy constructor
		//////////////////////////////////////////////////////////////////////
		Dfa(const Dfa& rhs);

		//////////////////////////////////////////////////////////////////////
		/// @brief	Empty default constructor
		//////////////////////////////////////////////////////////////////////
		Dfa();

		//////////////////////////////////////////////////////////////////////
		/// @brief	Generate string based on regex. The string's max length
		/// 		adheres to "absolute_max_length". After each atomic rule
		/// 		of the regex, there is a "length_probability_of_quiting"
		/// 		probability of ending the string (applies to *, + expressions).
		/// @return A string following the format of the regular expression
		//////////////////////////////////////////////////////////////////////
		string generate_string() const;

		//////////////////////////////////////////////////////////////////////
		/// @brief	Prints the "non_regex_string" to ostream. Note, 
		/// 		"non_regex_string" is a constant string or the rule
		/// 		being used.
		/// @param[out] 	os 	Ostream to print to
		/// @param[in] 	binary 	Dfa to print "non_regex_string" from
		//////////////////////////////////////////////////////////////////////
		friend ostream & operator << (ostream & os, const Dfa& binary);

	private:
		vector<char> possible_characters; 	///< Possible characters to use
											///< in the regex

		uint max_length;	///< Max length of the string
		uint min_length; 	///< Min length of the string
		static const uint 
		absolute_max_length;	///< Absolute max length of generated string.
								///< Takes precedence over "max_length"
								///< Must be set before constructor is called
		static const float 
		length_probability_of_quiting;	///< Probability of quiting after each
										///< atomic rule. Applies to * and +
										///< expressions
		bool exact_string;	///< If a constant string is being used instead
							///< of a traditional regex
		string non_regex_string;	///< The constant string or regex being used
};

#endif