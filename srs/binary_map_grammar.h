#ifndef BASE_GRAMMAR_H
#define BASE_GRAMMAR_H

#include <utility>
#include <unordered_map>
#include <regex>
using namespace std;

class Binary_map_grammar : public Base_grammar {
	public:
		Binary_map_grammar();

		virtual float find_fitness();
		virtual float get_fitness() const;

		virtual void mutate();

		virtual void abstract();

		virtual pair<shared_ptr<Base_grammar>, shared_ptr<Base_grammar> > recombination(const shared_ptr<Base_grammar>& mate) const;

		virtual vector<string> generate_sample_strings();

	private:
		static const int words_generated_count;
		static unordered_map<> char_to_binary_mapping;
		static unordered_map<> binary_to_regex mapping;

		// Common regular expressions
		static const vector<regex> common_patterns;
};

const int Binary_map_grammar::words_generated_count = 100;

const vector<regex> Binary_map_grammar::common_patterns = {
	// Single characters
	"[A-Z]", // Any capital letters
	"[a-z]", // Any lowercase letters
	"[A-Za-z]", // Any letters
	"[0-9]", // Any numbers
	"[0-9A-Za-z]", // Any alpha-numeric
	// One or more character
	"[A-Z+]", // capital letters
	"[a-z+]", // lowercase letters
	"[A-Za-z+]", // letters
	"[0-9+]", // numbers
	"[0-9A-Za-z+]", // alpha-numeric
	// Any number of characters
	"[A-Z*]", // capital letters
	"[a-z*]", // lowercase letters
	"[A-Za-z*]", // letters
	"[0-9*]", // numbers
	"[0-9A-Za-z*]" // alpha-numeric

	// NOTE: Should add hex, _ stuff, binary, base64, char with newline and tab (white space)
}


Binary_map_grammar::Binary_map_grammar() {

}

float Binary_map_grammar::find_fitness() {

}

float Binary_map_grammar::get_fitness() const {

}

void Binary_map_grammar::mutate() {

}

void Binary_map_grammar::abstract() {

}

pair<shared_ptr<Base_grammar>, shared_ptr<Base_grammar> > 
Binary_map_grammar::recombination(const shared_ptr<Base_grammar>& mate) const {

}

vector<string> Binary_map_grammar::generate_sample_strings() {

		}


#endif