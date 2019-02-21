#include "binary_map_grammar.h"

unordered_map<char, uint32_t> Binary_map_grammar::char_to_binary_mapping;
unordered_map<uint32_t, Dfa> Binary_map_grammar::binary_to_regex_mapping;
uint32_t Binary_map_grammar::next_available_mapping = 1;
const uint32_t Binary_map_grammar::start_symbol = 0;
int Binary_map_grammar::max_string_depth = 100;
int Binary_map_grammar::max_failed_attempts = 100;

// These are added to binary_to_regex_mapping upon construction
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
	// Add single characters 
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
	uint32_t last_non_terminal = start_symbol;

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

shared_ptr<Base_grammar> Binary_map_grammar::clone() const {
	return shared_ptr<Base_grammar>(new Binary_map_grammar(*this));
}


vector<string> Binary_map_grammar::generate_strings() {
	// List of words
	unordered_set<string> words;
	vector<string> unique_words;
	string word;
	int failed_attempts = 0;

	while(words.size() < static_cast<uint>(words_generated_count) 
		&& failed_attempts < max_failed_attempts) {
		word = generate_string(grammar[start_symbol], 0);

		if(max_depth_reached == true) {
			max_depth_reached = false;
			failed_attempts++;
		} else if(words.find(word) != words.end()) {
			failed_attempts++;
		} else {
			words.insert(word);
		}
	}

	unique_words.reserve(words.size());
	for (auto it = words.begin(); it != words.end(); ) {
		unique_words.push_back(std::move(words.extract(it++).value()));
	}

	return unique_words;
}

string Binary_map_grammar::generate_string(const vector<vector<uint32_t> >& rules, const int depth) {
	// If reached max depth
	if(depth == max_string_depth) {
		max_depth_reached = true;
		return "";
	}

	string word = "";

	// No rules = lambda
	if(rules.size() == 0) {
		return word;
	}

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

std::thread Binary_map_grammar::find_fitness_thread() {
	return std::thread(&Binary_map_grammar::find_fitness, this);
}

float Binary_map_grammar::find_fitness() {
	Code_coverage code_coverage;

	vector<string> samples = generate_strings();

	// Decrease possible fitness for repeated strings
	sort(samples.begin(), samples.end());
	auto it = unique(samples.begin(), samples.end());
	samples.resize(std::distance(samples.begin(), it));

	fitness = code_coverage(samples);

	// --- Favor smaller rules sets --- //
	// Do this by simply subtracting the fitness by the number of rules in grammar
	// Non-terminals left hand sides count as 1, rules count as .1
	float count = grammar.size();
	for(const auto& rules : grammar){
		count += (5.0 * static_cast<float>(rules.second.size()));
		count += 10.0;
	}

	fitness -= count;

	// --- Favor diverse languages --- //
	unordered_set<uint32_t> diverse;
	for(const auto& rules : grammar) {
		for(const auto& rule : rules.second) {
			for(const auto& symbol : rule) {
				if(binary_to_regex_mapping.find(symbol) != binary_to_regex_mapping.end()) {
					diverse.insert(symbol);
				}
			}
		}
	}
	// NOTE: change
	fitness *= (1 + (diverse.size()));

	return fitness;
}

float Binary_map_grammar::get_fitness() {
	if(fitness == -9999) {
		find_fitness();
	}

	return fitness;
}

void Binary_map_grammar::mutate() {
	for(auto& rules : grammar) {
		// Delete rule

		// Delete symbol
		if(success()) {
			// Delete single symbol in each rule with success() probability
			// favors rules that are closer to the start
			for(auto& rule : rules.second) {
				if(success() && rule.size() != 0) {
					std::swap(rule[rand() % rule.size()], rule.back());
    				rule.pop_back();
					break;
				}
			}
		}

		// Add a rule as a single non terminal or terminal
		if(success()) {
			rules.second.push_back({random_term()});
		}

		// Add rule from random other rule
		if(success()) {
			uint32_t a_term = random_term();
			if(grammar.find(a_term) != grammar.end()) {
				// Throw error if bad size
				if(grammar[a_term].size() == 0) {
					string s = "Error in mutate! Rules size is 0! 1";
					//cerr << s << endl;
					//throw s;
				} else {
					rules.second.push_back(grammar[a_term][rand() % grammar[a_term].size()]);
				}
			}
		}

		for(auto& rule : rules.second) {
			// Add to rule (insert)
			if(success()) {
				// Throw error if bad size
				if(rule.size() == 0) {
					string s = "Error in mutate! Rule size is 0! 2";
					//cerr << s << endl;
					//throw s;
				} else {
					rule.insert(rule.begin() + (rand() % rule.size()), random_term()); // NOTE: CAN CAUSE GRAMMAR TO NEVER END
				}
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

	// If start symbol was deleted, add it back and have it point to random rule
	if(grammar.find(start_symbol) == grammar.end()) {
		//auto test = (vector<uint32_t>());
		grammar[start_symbol].push_back({vector<uint32_t>()});

		if(grammar.size() > 0) {
			auto random_it = std::next(std::begin(grammar), rand() % grammar.size());
			grammar[start_symbol].back().push_back({(*random_it).first});
		}
	}

	// Abstract rules out with "mutate" probability
	abstract();
}

uint32_t Binary_map_grammar::random_term() const {
	return rand() % next_available_mapping;
}

// Try to combine rules to make it more CFG-like (instead of regex)
// Abstract out terminals (ex p = [a-z])
void Binary_map_grammar::abstract(const bool guarantee_abstract) {
	if(success() || guarantee_abstract) {
		eliminate_dead_rules();
	}
	if(success() || guarantee_abstract) {
		condense_repetition();
	}
	// Remove rules that only contain an epsilon
	if(success() || guarantee_abstract) {
		remove_rules_only_containing_epsilon();
	}

	if(success() || guarantee_abstract) {
		remove_rules_only_containing_a_non_terminal();
	}

	// Remove duplicate rules in rule sets
	if(success() || guarantee_abstract) {
		remove_duplicate_rules();
	}

	// NOTE: Add rule where if two non-terminal's rules are the same
	// eliminate one and replace
}

// Attempt to condense rules adhering to the following example:
// S -> aQ
// Q -> aR ...
// Try to condense to S -> aS | aR
//
void Binary_map_grammar::condense_repetition() {
	// Keys in grammar
	unordered_set<uint32_t> keys;
	for(const auto& non_terminal : grammar) {
		keys.insert(non_terminal.first);
	}

	// For each rule
	for(auto& rules : grammar) {
		vector<vector<uint32_t> > rules_to_add;
		decltype(grammar) rules_to_remove;
		for(auto& rule : rules.second) {
			// If there is a terminal followed by a non-terminal
			for(uint i = 1; i < rule.size(); i++) {
				// If rule[i] is a non-terminal that isn't the same as rules.first
				if(keys.find(rule[i]) != keys.end() && rule[i] != rules.first) {
					// If rule[i-1] is a terminal
					if(keys.find(rule[i-1]) == keys.end()) {
						// Check to see if rule[i] -> stuff
						// starts with the same terminal as rule[i-1]
						
						// -- If other key only has one rule -- //
						if(grammar[rule[i]].size() == 1) {
							// If there are 1 or more (non)terminals and
							// 	first terminal is the same as rule[i-1]
							if(grammar[rule[i]][0].size() > 0 && 
								grammar[rule[i]][0][0] == rule[i-1]) {
								// Add recursion to the rule so multiples of the terminal
								// can happen i.e. S -> aS
								vector<uint32_t> recursive_rule;
								recursive_rule.push_back(rule[i-1]);
								recursive_rule.push_back(rules.first);
								rules_to_add.push_back(recursive_rule);

								// For each part after the similar terminal
								for(uint j = 1; j < grammar[rule[i]][0].size(); j++) {
									// Insert inserts before the specified location
									rule.insert(rule.begin() + i + j, grammar[rule[i]][0][j]);
								}

								// --- Remove ---
								// Add to rules_to_remove
								rules_to_remove[rule[i]].push_back(grammar[rule[i]][0]);
							} 
						}
						// -- If other key has multiple rules -- //
						else if(grammar[rule[i]].size() > 1) {

						}

					}

					// Since current part is a non-terminal, skip next search
					i++;
				}
			}
		}
		// Add all rules_to_add to grammar rules
		for(auto& r : rules_to_add) {
			rules.second.push_back(r);
		}

		// Remove rules that should be deleted
		for(auto& r : rules_to_remove) {
			for(auto& rule : r.second) {
				auto it = find(grammar[r.first].begin(), grammar[r.first].end() , rule);

				if(it != grammar[r.first].end()) {
					grammar[r.first].erase(it);
				}
			}
		}
	}

	/*
	cout << "-------------------------------------" << endl;
	cout << *this << endl;
	cout << "-------------------------------------" << endl;
	*/
}

void Binary_map_grammar::remove_rules_only_containing_epsilon() {
	unordered_set<uint32_t> epsilon_rules;

	// Find epsilon rules
	for(const auto& gram : grammar) {
		if(gram.second.size() == 0) {
			// Add to remove list
			epsilon_rules.insert(gram.first);
			
		}
	}

	// Remove empty rules from grammar
	for(const auto& ep_rule : epsilon_rules) {
		grammar.erase(ep_rule);
	}

	for(auto& gram : grammar) {
		for(auto& rule : gram.second) {
			for(const auto& ep_rule : epsilon_rules) {
				rule.erase(remove(rule.begin(), rule.end(), ep_rule), rule.end());
			}
		}
	}
}

void Binary_map_grammar::remove_rules_only_containing_a_non_terminal() {
	// First = rule being replaced, second = rule to replace with
	unordered_map<uint32_t, uint32_t> only_non_term_rules;

	// Find rules only containing a single non-terminal
	for(const auto& gram : grammar) {
		if(gram.second.size() == 1 && gram.second[0].size() == 1) {
			// If non-terminal
			if(grammar.find(gram.second[0][0]) != grammar.end()) {
				// Add to remove list
				only_non_term_rules[gram.first] = gram.second[0][0];
			}
		}
	}

	// Remove replaced rules from grammar
	for(const auto& non_term : only_non_term_rules) {
		grammar.erase(non_term.first);
	}

	for(auto& gram : grammar) {
		for(auto& rule : gram.second) {
			for(auto& symbol : rule) {
				for(const auto& non_term : only_non_term_rules) {
					if(symbol == non_term.first) {
						symbol = non_term.second;
					}
				}
			}
		}
	}
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

bool sort_rule_sets(const vector<uint32_t> lhs, const vector<uint32_t> rhs) {
	if(lhs.size() < rhs.size()) {
		return true;
	} else if(lhs.size() > rhs.size()) {
		return false;
	} else { // Same size
		for(uint i = 0; i < lhs.size(); i++) {
			if(lhs[i] < rhs[i]) {
				return true;
			} else if(lhs[i] > rhs[i]) {
				return false;
			}
		}
	}
	return true;
}

bool unique_rule_set(const vector<uint32_t> lhs, const vector<uint32_t> rhs) {
	if(lhs.size() != rhs.size()) {
		return false;
	} else {
		for(uint i = 0; i < lhs.size(); i++) {
			if(lhs[i] != rhs[i]) {
				return false;
			}
		}
	}

	return true;
}

void Binary_map_grammar::remove_duplicate_rules() {
	for(auto& rules : grammar) {
		// Sort
		sort(rules.second.begin(), rules.second.end(), sort_rule_sets);
		// Make unique
		rules.second.erase(unique(rules.second.begin(), 
			rules.second.end(), unique_rule_set), rules.second.end());
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
	map<uint32_t, vector<vector<uint32_t> > > ordered_gramar;

	for(const auto& gram : grammar) {
		ordered_gramar[gram.first] = gram.second;
	}

	// For each grammar non terminal
	for(const auto& gram : ordered_gramar) {
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