#include "binary_map_grammar.h"

unordered_map<char, uint32_t> Binary_map_grammar::char_to_binary_mapping;
unordered_map<uint32_t, Dfa> Binary_map_grammar::binary_to_regex_mapping;
unordered_map<Dfa, uint32_t> Binary_map_grammar::regex_to_binary_mapping; 
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


unordered_set<string> Binary_map_grammar::generate_strings() {
	// List of words
	unordered_set<string> words;
	string word;
	int failed_attempts = 0;

	// Reset probabilistic grammar
	gram_prob.resize(grammar);

	// While not enough words and not too many failed attempts
	while(words.size() < static_cast<uint>(words_generated_count) 
		&& failed_attempts < max_failed_attempts) {
		
		string word = "";
		generate_string(word, 0, start_symbol);

		if(max_depth_reached == true) {
			max_depth_reached = false;
			failed_attempts++;
		} else if(words.find(word) != words.end()) {
			failed_attempts++;
		} else {
			words.insert(word);
		}
	}

	return words;
}


// NOTE: Problem CHILD
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
This is because a map's operator[] will insert a new 
element if the key isn't found, so obviously it has to be non-const.
*/
void Binary_map_grammar::generate_string
(string& word, const int depth, const uint32_t non_terminal) {
	// If reached max depth
	if(depth == max_string_depth) {
		max_depth_reached = true;
		return;
	}

	// No rules = lambda
	if(grammar[non_terminal].size() == 0) {
		return;
	}

	// Probabilistically pick a random rule,
	// favors rules that have been chosen less often
	int random_rule = gram_prob.choosen_rule_index(non_terminal);

	// For each symbol in rule
	for(const auto& symbol : grammar[non_terminal][random_rule]) {
		// If max depth has been reached, go back
		if(max_depth_reached == true) {
			return;
		}

		// If terminal character, add it to word
		if(binary_to_regex_mapping.find(symbol) != binary_to_regex_mapping.end()) {
			word += binary_to_regex_mapping[symbol].generate_string();
		} else { // Non-terminal
			if(grammar.find(symbol) != grammar.end()) {	
				generate_string(word, depth + 1, symbol);
			}
		}
	}

	return;
}

std::thread Binary_map_grammar::find_fitness_thread() {
	return std::thread(&Binary_map_grammar::find_fitness, this);
}

float Binary_map_grammar::find_fitness() {
	Code_coverage code_coverage;

	unordered_set<string> samples = generate_strings();

	// Convert samples to a vector of samples
	vector<string> vector_samples;
	vector_samples.reserve(samples.size());
	for (auto it = samples.begin(); it != samples.end(); ) {
		vector_samples.push_back(std::move(samples.extract(it++).value()));
	}

	fitness = code_coverage(vector_samples);

	// --- Favor smaller rules sets --- //
	// Do this by simply subtracting the fitness by the number of rules in grammar
	// Non-terminals left hand sides count as 1, rules count as .1
	float count = grammar.size();
	for(const auto& rules : grammar){
		count += (5.0 * static_cast<float>(rules.second.size()));
		count += 10.0;
	}

	fitness -= (count / 2);

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
	fitness += (100 * diverse.size());
	//fitness *= (1 + (diverse.size()));

	return fitness;
}

float Binary_map_grammar::get_fitness() {
	if(fitness == -9999) {
		find_fitness();
	}

	return fitness;
}

void Binary_map_grammar::mutate() {
	// Delete random rule
	if(success() && success()) {
		delete_random_rule_from_grammar();
	}

	for(auto& rules : grammar) {
		// Add a rule as a single symbol
		if(success()) {
			insert_random_rule(rules.second);
		}

		// Add rule from random other rule
		if(success()) {
			insert_random_other_rule(rules.second);
		}

		for(auto& rule : rules.second) {
			// Insert random symbol into rule
			if(success()) {
				insert_random_symbol(rule);
			}

			// Change random symbol to another random symbol
			if(success()) {
				mutate_random_symbol(rule);
			}

			// Delete single symbol in each rule with success() probability
			if(success() && success()) {
				delete_random_symbol(rule);
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
	//abstract(true);
}

bool Binary_map_grammar::delete_random_rule_from_grammar() {
	// Select a random rule-set
	uint32_t non_term = random_non_term();

	// Make sure rule-size is greater than 0
	if(grammar[non_term].size() == 0) {
		return false;
	}

	// NOTE: CAUSES FLOATING POINT EXCEPTION
	try{
		// Swap random rule with the back rule
		swap(grammar[non_term][rand() % grammar[non_term].size()], grammar[non_term].back());
		// Pop back rule
		grammar[non_term].pop_back();

		return true;
	} catch(...) {}

	return false;
}

bool Binary_map_grammar::insert_random_rule(vector<vector<uint32_t> >& rules) const {
	rules.push_back({random_term()});

	return true;
}

bool Binary_map_grammar::insert_random_other_rule(vector<vector<uint32_t> >& rules) {
	// Select random rule-set
	if(grammar.size() == 0) {
		return false;
	}
	uint32_t random_non_terminal = random_non_term();

	// Select random rule
	if(grammar[random_non_terminal].size() == 0) {
		return false;
	}
	uint32_t random_rule = rand() % grammar[random_non_terminal].size();

	rules.push_back(grammar[random_non_terminal][random_rule]);

	return true;
}

bool Binary_map_grammar::delete_random_symbol(vector<uint32_t>& rule) const {
	// Make sure rule size is not 0
	if(rule.size() == 0) {
		return false;
	}

	// Select random symbol to delete
	int rand_symbol = rand() % rule.size();

	rule.erase(rule.begin() + rand_symbol);

	return true;
}

bool Binary_map_grammar::insert_random_symbol(vector<uint32_t>& rule) const {
	int random_spot = 0;
	if(rule.size() != 0) {
		random_spot = rand() % rule.size();
	}

	rule.insert(rule.begin() + random_spot, random_term());

	return true;
}

bool Binary_map_grammar::mutate_random_symbol(vector<uint32_t>& rule) const {
	if(rule.size() == 0) {
		return false;
	}
	rule[rand() % rule.size()] = random_term();

	return true;
}

uint32_t Binary_map_grammar::random_term() const {
	return rand() % next_available_mapping;
}

uint32_t Binary_map_grammar::random_non_term() const {
	int random = rand() % grammar.size();
	auto it = grammar.begin();

	// Advance iterator "random" times
	advance(it, random);

	// Return non-terminal
	return (*it).first;
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


	if(success() || guarantee_abstract) {
		remove_non_existant_symbols();
	}
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
// NOTE: FINISH
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

	// For each rule to remove
	for(const auto& ep_rule : epsilon_rules) {
		// For each ruleset
		for(auto& rules : grammar) {
			// For each rule in a ruleset
			for(auto& rule : rules.second) {
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
	return false;
}

bool unique_rule_set(const vector<uint32_t> lhs, const vector<uint32_t> rhs) {
	// If vectors are not the same size, return false
	if(lhs.size() != rhs.size()) {
		return false;
	} else {
		for(uint i = 0; i < lhs.size() && i < rhs.size(); i++) {
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

void Binary_map_grammar::remove_non_existant_symbols() {
	unordered_set<uint32_t> non_existant_symbols;

	// For each rule-set
	for(auto& rules : grammar) {
		// For each rule in a rule-set
		for(auto& rule : rules.second) {
			// For each symbol
			for(auto& symbol : rule) {
				// If symbol is not in DFA mapping or is not a non-terminal grammar key
				if(binary_to_regex_mapping.find(symbol) == binary_to_regex_mapping.end()
					&& grammar.find(symbol) == grammar.end()) {
					// Add to non-existant symbols
					non_existant_symbols.insert(symbol);
				}
			}
		}
	}

	// For each rule to remove
	for(const auto& symbol : non_existant_symbols) {
		// For each rule-set
		for(auto& rules : grammar) {
			// For each rule in a rule-set
			for(auto& rule : rules.second) {
				// Erase non-existant symbol
				rule.erase(remove(rule.begin(), rule.end(), symbol), rule.end());
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

vector<shared_ptr<Base_grammar> > Binary_map_grammar::recombination(
const vector<shared_ptr<Base_grammar> >& mates, 
const string& method, const uint number_of_children) {
	(void)method;

	// Children
	vector<shared_ptr<Base_grammar> > children;

	if(number_of_children == 1) {
		children.push_back(recombination_single(mates));
	} else if(number_of_children == 2) {
		if(mates.size() == 1) {
			auto pair_of_children = recombination_twins(mates[0]);
			children.push_back(pair_of_children.first);
			children.push_back(pair_of_children.second);
		}
	}

	return children;
}

// Child Combine both grammars
shared_ptr<Base_grammar> Binary_map_grammar::recombination_single
(const vector<shared_ptr<Base_grammar> >& mates) {
	// Mother = this, father = mates

	// Copy mothers's grammar into child
	shared_ptr<Binary_map_grammar> child = static_pointer_cast<Binary_map_grammar>(clone());

	// Copy each father's grammar into child
	for(const auto& father : mates) {
		// For each rule set in each grammar
		for(const auto& rules : static_pointer_cast<Binary_map_grammar>(father) -> grammar) {
			// If non-terminal rhs does not already exist in child's grammar
			if(child -> grammar.find(rules.first) == child -> grammar.end()) {
				// Add rhs non-terminal
				child -> grammar[rules.first] = rules.second;
			} else { // rhs already exists in child
				// Add each rule
				for(const auto& rule : rules.second) {
					child -> grammar[rules.first].push_back(rule);
				}
			}
		}
	}

	return child;
}

pair<shared_ptr<Base_grammar>, shared_ptr<Base_grammar> > 
Binary_map_grammar::recombination_twins(const shared_ptr<Base_grammar>& mate) {
	// Mother = this, father = mate

	// Generate empty children
	shared_ptr<Base_grammar> first_child(new Binary_map_grammar());
	shared_ptr<Base_grammar> second_child(new Binary_map_grammar());

	// Copy mother's grammar into first child
	static_pointer_cast<Binary_map_grammar>(first_child) -> grammar = grammar;

	// Put first half of mother into first child and second half into second child
	uint32_t non_terminal_count = 0;
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
		os << "\033[45m" << gram.first << ":\033[0m" << "\t" << gram.second.size()

		<< endl;
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