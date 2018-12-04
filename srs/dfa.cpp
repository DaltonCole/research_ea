#include "dfa.h"

const uint Dfa::absolute_max_length = 50;
const float Dfa::length_probability_of_quiting = 0.25;

Dfa::Dfa() {

}

Dfa::Dfa(const Dfa& rhs) {
	max_length = rhs.max_length;
	min_length = rhs.min_length;
	exact_string = rhs.exact_string;
	non_regex_string = rhs.non_regex_string;
	possible_characters = rhs.possible_characters;
}

// Accept "program", "[a-zA-Z_]", "[0-9+]", and "[a-z+]" like inputs
Dfa::Dfa(const string& input, const bool regex) {
	non_regex_string = input;
	if(regex == false) {
		exact_string = true;
	} else {
		exact_string = false;
		max_length = 1;
		min_length = 1;
		try {
			for(uint i = 0; i < input.size(); i++) {
				if(input[i] == '+') {
					max_length = absolute_max_length;
				} else if(input[i] == '*') {
					min_length = 0;
					max_length = absolute_max_length;
				} else if(input[i] != '[') {
					possible_characters.push_back(input[i]);
				} else {
					i++;
					while(input[i] != ']') {
						if(input[i + 1] == '-') {
							for(int c = input[i]; c <= input[i+2]; c++) {
								possible_characters.push_back(c);
							}
							i += 3;
						} else {
							possible_characters.push_back(input[i]);
							i++;
						}
					}
				}
			}
		} catch (...) {
			cout << "Exception occured in DFA constructor! Likely out of bounds error!" << endl;
		}
	}
}

string Dfa::generate_string() const {
	if(exact_string == true) {
		return non_regex_string;
	}

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