#include "grammar_probability.h"

void Grammar_probability::resize
(const unordered_map<uint32_t, vector<vector<uint32_t> > >& grammar) {
	// --- Remove occurences rules if needed --- //
	vector<uint32_t> dead_non_terminals;
	for(const auto& rules : total_occurrences) {
		// If non-terminal is not in grammar, add to remove list
		if(grammar.find(rules.first) == grammar.end()) {
			dead_non_terminals.push_back(rules.first);
		}
	}

	for(const auto& key_to_erase : dead_non_terminals) {
		total_occurrences.erase(key_to_erase);
		occurrences.erase(key_to_erase);
	}
	///////////////////////////////////////////////

	// --- Update size --- //
	for(const auto& rules : grammar) {
		// Set total occurences to be the number of produciton rules
		total_occurrences[rules.first] = rules.second.size();

		// Set each occurence to 1 (because probability and division)
		occurrences[rules.first].resize(rules.second.size());
		for(uint i = 0; i < rules.second.size(); i++) {
			occurrences[rules.first][i] = 1;
		}
		
	}
	/////////////////////////
}

uint Grammar_probability::choosen_rule_index(const uint32_t non_terminal) {
	uint index = 0;

	uint random_num = rand() % total_occurrences[non_terminal];
	uint count = 0;

	for(index = 0; index < occurrences[non_terminal].size(); index++) {
		count += occurrences[non_terminal][index];

		if(count > random_num) {
			break;
		}
	}

	// Increment all other occurrences
	for(uint i = 0; i < occurrences[non_terminal].size(); i++) {
		if(i != index) {
			occurrences[non_terminal][i]++;
		}
	}

	// Increment total_occurrences
	total_occurrences[non_terminal] += occurrences[non_terminal].size() - 1;

	return index;
}