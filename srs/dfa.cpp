#include "dfa.h"

const uint Dfa::absolute_max_length = 50;
const float Dfa::length_probability_of_quiting = 0.25;

Dfa::Dfa() {}

Dfa::Dfa(const Dfa& rhs) {
	max_length = rhs.max_length;
	min_length = rhs.min_length;
	exact_string = rhs.exact_string;
	non_regex_string = rhs.non_regex_string;
	possible_characters = rhs.possible_characters;
}

// Accept "program", "[a-zA-Z_]", "[0-9+]", and "[a-z+]" like inputs
Dfa::Dfa(const string& input, const bool regex) {
	// Save rule or const string being used
	non_regex_string = input;
	if(regex == false) {
		// If not a regex, remember that only a constant string was used
		exact_string = true;
	} else {
		exact_string = false;
		max_length = 1;
		min_length = 1;
		try {
			// For each char in input
			for(uint i = 0; i < input.size(); i++) {
				// + sets max length, keep min length
				if(input[i] == '+') {
					max_length = absolute_max_length;
				} else if(input[i] == '*') {
					// * sets min and max length
					min_length = 0;
					max_length = absolute_max_length;
				} else if(input[i] != '[') {
					// If not a range, add characters
					possible_characters.push_back(input[i]);
				} else {
					// If '[', go to next character
					i++;
					// While not at end of atomic rule
					while(input[i] != ']') {
						// If range
						if(input[i + 1] == '-') {
							// Add each character between and including
							// bounds in range
							for(int c = input[i]; c <= input[i+2]; c++) {
								possible_characters.push_back(c);
							}
							// Go to next singe or range
							i += 3;
						} else {
							// Add single character to possible characters
							possible_characters.push_back(input[i]);
							i++;
						}
					}
				}
			}
		} catch (...) {
			// Caused by bad input
			cerr << "Exception occured in DFA constructor! Likely out of bounds error!" << endl;
		}
	}
}

string Dfa::generate_string() const {
	// If there is an exact string, generate it
	if(exact_string == true) {
		return non_regex_string;
	}

	// Generated string
	string str;

	// Generate characters up until min_length
	for(uint i = 0; i < min_length; i++) {
		str += possible_characters[rand() % possible_characters.size()];
	}

	// Generate characters up until max_length with length_probability_of_quiting
	// chance of quiting
	while(str.size() < max_length) {
		if((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) < length_probability_of_quiting) {
			break;
		}
		str += possible_characters[rand() % possible_characters.size()];
	}

	return str;
}

ostream & operator << (ostream & os, const Dfa& binary) {
	os << binary.non_regex_string;

	return os;
}