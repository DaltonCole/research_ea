#ifndef DFA_H
#define DFA_H

#include <iostream>
#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
using namespace std;

/*
struct Node {
	char label;
	vector<Node*> neighbors;
}
*/

class Dfa {
	public:
		Dfa(const string& input, const bool regex = true);
		Dfa(const Dfa& rhs);
		string generate_string() const;
		Dfa();

		friend ostream & operator << (ostream & os, const Dfa& binary);

	private:
		vector<char> possible_characters;

		uint max_length;
		uint min_length;
		static const uint absolute_max_length;
		static const float length_probability_of_quiting;
		bool exact_string;
		string non_regex_string;
};

#endif