#include "binary_map_grammar.h"

const int Binary_map_grammar::words_generated_count = 100;
unordered_map<char, uint32_t> Binary_map_grammar::char_to_binary_mapping;
unordered_map<uint32_t, Dfa> Binary_map_grammar::binary_to_regex_mapping;
uint32_t Binary_map_grammar::next_available_mapping = 1;
const uint32_t Binary_map_grammar::start_symbol = 0;

// NOTE: NEED TO ADD THESE binary_to_regex_mapping
const vector<Dfa> Binary_map_grammar::common_patterns = {
	// Single characters
	Dfa("[A-Z]"), // Any capital letters
	Dfa("[a-z]"), // Any lowercase letters
	Dfa("[A-Za-z]"), // Any letters
	Dfa("[0-9]"), // Any numbers
	Dfa("[0-9A-Za-z]"), // Any alpha-numeric
	// One or more character
	Dfa("[A-Z+]"), // capital letters
	Dfa("[a-z+]"), // lowercase letters
	Dfa("[A-Za-z+]"), // letters
	Dfa("[0-9+]"), // numbers
	Dfa("[0-9A-Za-z+]"), // alpha-numeric
	// Any number of characters
	Dfa("[A-Z*]"), // capital letters
	Dfa("[a-z*]"), // lowercase letters
	Dfa("[A-Za-z*]"), // letters
	Dfa("[0-9*]"), // numbers
	Dfa("[0-9A-Za-z*]"), // alpha-numeric

	// One or more character with _
	Dfa("[A-Z+_]"), // capital letters
	Dfa("[a-z_+]"), // lowercase letters
	Dfa("[A-Za-z_+]"), // letters
	Dfa("[0-9_+]"), // numbers
	Dfa("[0-9A-Za-z_+]"), // alpha-numeric
	// Any number of characters with _
	Dfa("[A-Z_*]"), // capital letters
	Dfa("[a-z_*]"), // lowercase letters
	Dfa("[A-Za-z_*]"), // letters
	Dfa("[0-9_*]"), // numbers
	Dfa("[0-9A-Za-z_*]") // alpha-numeric

	// NOTE: Should add hex, _ stuff, binary, base64, char with newline and tab (white space)
};

Binary_map_grammar::Binary_map_grammar(const string& input) {
	// A -> aB // Later try to abstract 'a' into a regex
	// B -> bC
	// ...

	// Add common_patterns to binary map first time this is called
	if(binary_to_regex_mapping.size() == 0) {
		for(auto& pattern : common_patterns) {
			binary_to_regex_mapping[next_available_mapping] = pattern;
			next_available_mapping++;
		}
	}

	// Set fitness
	fitness = -9999;

	// Set first non_terminal to be start symbol
	uint32_t last_non_terminal;

	// For each character in input
	for(uint i = 0; i < input.size(); i++) {
		// Current character
		char c = input[i];

		// If character has no binary representation yet
		if(char_to_binary_mapping.find(c) == char_to_binary_mapping.end()) {
			// Set to next available binary representation
			char_to_binary_mapping[c] = next_available_mapping;
			// Add mapping to binary_to_regex_mapping
			binary_to_regex_mapping[next_available_mapping] = Dfa(string(1, c), false);
			// Increment mapping count
			next_available_mapping++;
		}

		// Create single rule
		vector<uint32_t> single_rule;
		// Add terminal to start of rule
		single_rule.push_back(char_to_binary_mapping[c]);
		// If not the last character
		if(i != input.size() - 1) {
			// Generate new non-terminal
			single_rule.push_back(next_available_mapping);
		}
		// Add single rule to last
		grammar[last_non_terminal].push_back(single_rule);
		last_non_terminal = next_available_mapping;
		// Increment next available mapping
		next_available_mapping++;
	}
}

Binary_map_grammar::Binary_map_grammar(const vector<string>& inputs) {
	// Add common_patterns to binary map first time this is called
	if(binary_to_regex_mapping.size() == 0) {
		for(auto& pattern : common_patterns) {
			binary_to_regex_mapping[next_available_mapping] = pattern;
			next_available_mapping++;
		}
	}

	// Set fitness
	fitness = -9999;

	uint32_t last_non_terminal;

	for(const auto& input : inputs) {
		// Set first non_terminal to be start symbol
		last_non_terminal = start_symbol;

		// For each character in input
		for(uint i = 0; i < input.size(); i++) {
			// Current character
			char c = input[i];

			// If character has no binary representation yet
			if(char_to_binary_mapping.find(c) == char_to_binary_mapping.end()) {
				// Set to next available binary representation
				char_to_binary_mapping[c] = next_available_mapping;
				// Add mapping to binary_to_regex_mapping
				binary_to_regex_mapping[next_available_mapping] = Dfa(string(1, c), false);
				// Increment mapping count
				next_available_mapping++;
			}

			// Create single rule
			vector<uint32_t> single_rule;
			// Add terminal to start of rule
			single_rule.push_back(char_to_binary_mapping[c]);
			// If not the last character
			if(i != input.size() - 1) {
				// Generate new non-terminal
				single_rule.push_back(next_available_mapping);
			}
			// Add single rule to last
			grammar[last_non_terminal].push_back(single_rule);
			last_non_terminal = next_available_mapping;
			// Increment next available mapping
			next_available_mapping++;
		}
	}
}

vector<string> Binary_map_grammar::generate_strings() const {
	return {""};
	vector<string> words;

	while(words.size() < words_generated_count) {
		// Randomly choose rules until termination
		// NOTE: need to improve on this idea with research stuff

	}

	return words;
}

float Binary_map_grammar::find_fitness() {
	Code_coverage code_coverage;

	fitness = code_coverage(generate_strings());

	return fitness;
}

float Binary_map_grammar::get_fitness() const {
	if(fitness == -9999) {
		throw "Fitness not yet set!";
	}

	return fitness;
}

void Binary_map_grammar::mutate() {

}

// Try to combine rules to make it more CFG-like (instead of regex)
// Abstract out terminals (ex p = [a-z])
void Binary_map_grammar::abstract() {

}

pair<shared_ptr<Base_grammar>, shared_ptr<Base_grammar> > 
Binary_map_grammar::recombination(const shared_ptr<Base_grammar>& mate) const {
	return make_pair(mate, mate);
}

vector<string> Binary_map_grammar::generate_sample_strings() {
	return {""};
}

void Binary_map_grammar::print(ostream& os) const {
	// For each grammar non terminal
	for(const auto& gram : grammar) {
		// Highlight start symbol
		if(gram.first == start_symbol) {
			os << "\033[43m" << "Start Symbol" << "\033[0m" << endl;
		}
		// Print non terminal
		os << gram.first << ": " << endl;
		// For each non terminal
		for(const auto& rule : gram.second) {
			// For each rule
			for(const auto& term : rule) {
				// Print (non)regex string
				if(binary_to_regex_mapping.find(term) != binary_to_regex_mapping.end()) {
					os << binary_to_regex_mapping[term];
				} else { // Print non-terminal in special coloring
					os << "\033[36m\\" << term << "\033[0m";
				}
			}
			os << endl;
		}
	}
}