#include "binary_map_grammar.h"

const int Binary_map_grammar::words_generated_count = 5;
unordered_map<char, uint32_t> Binary_map_grammar::char_to_binary_mapping;
unordered_map<uint32_t, Dfa> Binary_map_grammar::binary_to_regex_mapping;
uint32_t Binary_map_grammar::next_available_mapping = 1;
const uint32_t Binary_map_grammar::start_symbol = 0;
const int Binary_map_grammar::max_string_depth = 100;
const int Binary_map_grammar::max_failed_attempts = 100;

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

Binary_map_grammar::Binary_map_grammar() {
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
}

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
	next_available_mapping--;
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
		next_available_mapping--;
	}
}

vector<string> Binary_map_grammar::generate_strings() {
	// List of words
	vector<string> words;
	string word;
	int failed_attempts = 0;

	while(words.size() < words_generated_count && failed_attempts < max_failed_attempts) {
		word = generate_string(grammar[start_symbol], 0);

		if(max_depth_reached == true) {
			max_depth_reached = false;
			failed_attempts++;
		} else {
			words.push_back(word);
		}

		cout << words.back() << endl;
	}

	return words;
}

string Binary_map_grammar::generate_string(const vector<vector<uint32_t> >& rules, const int depth) {
	// If reached max depth
	if(depth == max_string_depth) {
		max_depth_reached = true;
		return "";
	}

	string word = "";
	int random_rule = rand() % rules.size();

	// Randomly choose rules until termination
	// NOTE: need to improve on this idea with research stuff
	for(const auto& non_terminal : rules[random_rule]) {
		// If max depth has been reached, go back
		if(max_depth_reached == true) {
			return "";
		}

		// If terminal character, add it to word
		if(binary_to_regex_mapping.find(non_terminal) != binary_to_regex_mapping.end()) {
			word += binary_to_regex_mapping[non_terminal].generate_string();
		} else { // Non-terminal
			word += generate_string(grammar[non_terminal], depth + 1);
		}
	}

	return word;
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
	for(auto& rules : grammar) {
		// Delete rule

		// Add a rule as a single non terminal or terminal
		if(success()) {
			rules.second.push_back({random_term()});
		}

		// Add rule from random other rule
		if(success()) {
			uint32_t a_term = random_term();
			if(grammar.find(a_term) != grammar.end()) {
				rules.second.push_back(grammar[a_term][rand() % grammar[a_term].size()]);
			}
		}

		for(auto& rule : rules.second) {
			// Add to rule (insert)
			if(success()) {
				rule.insert(rule.begin() + (rand() % rule.size()), random_term()); // NOTE: CAN CAUSE GRAMMAR TO NEVER END
			}

			// For each (non)terminal in each rule
			for(auto& terms : rule) {
				// Change it to a different symbol
				if(success()) {
					terms = random_term();
				}
			}
		}
	}
}

uint32_t Binary_map_grammar::random_term() const {
	return rand() % next_available_mapping;
}

// Try to combine rules to make it more CFG-like (instead of regex)
// Abstract out terminals (ex p = [a-z])
void Binary_map_grammar::abstract() {
	eliminate_dead_rules();
}

void Binary_map_grammar::eliminate_dead_rules() {
	// Rules that we have looked at
	unordered_set<uint32_t> touched_rules;

	// Call helper on start symbol
	touched_rules.insert(start_symbol);
	eliminate_dead_rules_helper(start_symbol, touched_rules);

	// Keys in grammar
	vector<uint32_t> keys;
	for(const auto& non_terminal : grammar) {
		keys.push_back(non_terminal.first);
	}

	// Delete rules not in touched_rules
	for(const auto& non_terminal : keys) {
		if(touched_rules.find(non_terminal) == touched_rules.end()) {
			grammar.erase(grammar.find(non_terminal));
		}
	}
}

void Binary_map_grammar::eliminate_dead_rules_helper
(const uint32_t non_terminal, unordered_set<uint32_t>& touched_rules) {
	// Explore each rule in non_terminal
	for(const vector<uint32_t>& rule : grammar[non_terminal]) {
		// Explore each term in the rule
		for(const uint32_t& term : rule) {
			// If term is a an un-touched non-terminal
			if(grammar.find(term) != grammar.end() && touched_rules.find(term) == touched_rules.end()) {
				// Add term to touched rules
				touched_rules.insert(term);
				eliminate_dead_rules_helper(term, touched_rules);
			}
		}
	}
}

/*
Binary_map_grammar::Binary_map_grammar(const Base_grammar& lhs, const Base_grammar& rhs) {
	// Mother = lhs, father = rhs
	// Genes = first half of mother, second half of father
	uint32_t non_terminal_count = 0;
	while(first_child -> grammar.size() <= grammar.size() / 2) {
		if(grammar.find(non_terminal_count) != grammar.end()) {
			first_child -> grammar[non_terminal_count] = grammar[non_terminal_count];
		}
		non_terminal_count++;
	}
	while(non_terminal_count < next_available_mapping) {
		if(grammar.find(non_terminal_count) != grammar.end()) {
			second_child -> grammar[non_terminal_count] = grammar[non_terminal_count];
		}
		non_terminal_count++;
	}
}
*/

pair<shared_ptr<Base_grammar>, shared_ptr<Base_grammar> > 
Binary_map_grammar::recombination(shared_ptr<Base_grammar>& mate) {
	shared_ptr<Base_grammar> first_child(new Binary_map_grammar());
	shared_ptr<Base_grammar> second_child(new Binary_map_grammar());

	static_pointer_cast<Binary_map_grammar>(first_child) -> grammar = grammar;

	// Mother = this, father = mate
	// Put first half of mother into first child and second half into second child
	uint32_t non_terminal_count = 0;
	// first -> grammar.size() <= grammar.size() / 2
	while(static_pointer_cast<Binary_map_grammar>(first_child) -> grammar.size() <= grammar.size() / 2) {
		// grammar.find != grammar.end
		if(grammar.find(non_terminal_count) != grammar.end()) {
			//first -> grammar = grammar
			static_pointer_cast<Binary_map_grammar>(first_child) -> grammar[non_terminal_count] = 
				grammar[non_terminal_count];
		}
		non_terminal_count++;
	}
	while(non_terminal_count < next_available_mapping) {
		// grammar.find != grammar.end
		if(grammar.find(non_terminal_count) != grammar.end()) {
			// second -> grammar = grammar
			static_pointer_cast<Binary_map_grammar>(second_child) -> grammar[non_terminal_count] = 
				grammar[non_terminal_count];
		}
		non_terminal_count++;
	}

	// Put first half of father into second child and second half into first child
	non_terminal_count = 0;
	// second -> grammar.size() <= mate -> grammar.size() < 2
	while(static_pointer_cast<Binary_map_grammar>(second_child)-> grammar.size() 
			<= static_pointer_cast<Binary_map_grammar>(mate) -> grammar.size() / 2) {
		// mate -> grammar.find != mate -> grammar.end
		if(static_pointer_cast<Binary_map_grammar>(mate) -> grammar.find(non_terminal_count) 
				!= static_pointer_cast<Binary_map_grammar>(mate) -> grammar.end()) {
			// second -> grammar = mate -> grammar
			static_pointer_cast<Binary_map_grammar>(second_child) -> grammar[non_terminal_count] = 
				static_pointer_cast<Binary_map_grammar>(mate) -> grammar[non_terminal_count];
		}
		non_terminal_count++;
	}
	while(non_terminal_count < next_available_mapping) {
		// mate -> grammar.find != mate -> grammar.end
		if(static_pointer_cast<Binary_map_grammar>(mate) -> grammar.find(non_terminal_count) != 
				static_pointer_cast<Binary_map_grammar>(mate) -> grammar.end()) {
			// first -> grammar = mate -> grammar
			static_pointer_cast<Binary_map_grammar>(first_child) -> grammar[non_terminal_count] = 
				static_pointer_cast<Binary_map_grammar>(mate) -> grammar[non_terminal_count];
		}
		non_terminal_count++;
	}

	return make_pair(first_child, second_child);
}

void Binary_map_grammar::print(ostream& os) const {
	// For each grammar non terminal
	for(const auto& gram : grammar) {
		// Highlight start symbol
		if(gram.first == start_symbol) {
			os << "\033[43m" << "Start Symbol" << "\033[0m" << endl;
		}
		// Print non terminal
		os << "\033[45m" << gram.first << ":\033[0m" << endl;
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