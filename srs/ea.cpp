#include "ea.h"

Ea::Ea() {
}

Ea::Ea(vector<shared_ptr<Base_grammar> >& initial_population, const char* config_file) {
	default_configurations();
	config_reader(config_file);

	if(stoi(config["Population Size"]) != initial_population.size()) {
		string s = "Incorrect population size given! " ;
		s += to_string(initial_population.size()) + " was given, but " + config["Population Size"] + " was required.";
		cerr << s << endl;
		throw s;
	}

	population = initial_population;
}

void Ea::run() {
	for(auto& person : population) {
		person -> find_fitness();
	}
	parent_selection();
}

void Ea::default_configurations() {
	config["Population Size"] = "100";
	config["Parent Selection"] = "Fitness Proportionate Selection";
	config["Number of parents"] = "50";
}

vector<shared_ptr<Base_grammar> > Ea::parent_selection() {
	if(config["Parent Selection"] == "Fitness Proportionate Selection") {
		return fitness_proportionate_selection(stoi(config["Number of parents"]));
	} else if(config["Parent Selection"] == "Tournament Selection") {
		return tournament_selection(stoi(config["Number of parents"]));
	}

	return {};
}

// Assumed every person in population already has fitness called
vector<shared_ptr<Base_grammar> > Ea::fitness_proportionate_selection(const uint size) const {
	vector<shared_ptr<Base_grammar> > selected;
	float total_fitness;

	// Population to pick from
	unordered_set<shared_ptr<Base_grammar> > population_to_pick_from;
	// Populate population to pick from
	for(auto& person : population) {
		population_to_pick_from.insert(person);
	}

	// Get Total fitness
	for(const auto& person : population) {
		total_fitness = person -> get_fitness();
	}

	float choosen_number;
	float running_total;

	// While we need to add more
	while(selected.size() < size) {
		// Reset running total
		running_total = 0;
		// Choose a random float between [0 - total_fitness]
		choosen_number = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/total_fitness));
		// For each person
		for(auto& person : population_to_pick_from) {
			// Add to running total
			running_total += person -> get_fitness();
			// If running total is greater than choosen number
			if(choosen_number <= running_total) {
				// They have been selected
				selected.push_back(person);
				// Remove from possible picks
				population_to_pick_from.erase(population_to_pick_from.find(person));
				break;
			}
		}
		// Decrement total fitness
		total_fitness -= selected.back() -> get_fitness();
	}

	return selected;
}

vector<shared_ptr<Base_grammar> > Ea::tournament_selection(const uint size) const {
	vector<shared_ptr<Base_grammar> > selected;
	unordered_set<shared_ptr<Base_grammar> > population_to_pick_from;

	for(auto& person : population) {
		population_to_pick_from.insert(person);
	}

	// Random element indexes
	int first;
	int second;
	// Random elements
	shared_ptr<Base_grammar> one;
	shared_ptr<Base_grammar> two;

	while(selected.size() < size) {
		// Randomly select two elements
		first = random() % population_to_pick_from.size();
		second = random() % population_to_pick_from.size();

		// Go to first index and set one
		auto it = population_to_pick_from.begin();
		for(int i = 0; i < first; i++, it++) {}
		one = *it;

		// Go to second index and set two
		it = population_to_pick_from.begin();
		for(int i = 0; i < first; i++, it++) {}
		two = *it;

		if(one -> get_fitness() < two -> get_fitness()) {
			one = two;
		}

		// Add highest scorer to selected
		selected.push_back(one);

		// Remove highest selected from population to pick from
		population_to_pick_from.erase(population_to_pick_from.find(one));
	}

	return selected;
}


void Ea::config_reader(const char* config_file) {
	string line;
	ifstream file(config_file);

	if(file.is_open()) {
		while(getline(file, line)) {
			// "#" represents commented line
			if(line.size() > 0 && line[0] == '#') {
				continue;
			}

			// For each configuration
			for(auto& conf : config) {
				// If config key is in line
				if(line.find(conf.first) != std::string::npos) {
					// Add config
					config_reader_helper(conf.first, line);
					break;
				}
			}
		}
		file.close();
	} else {
		throw "config.txt failed to open!";
	}

	config_checker();
}

void Ea::config_reader_helper(const string& key, const string& line) {
	string value = "";
	uint index = 0;
	// Find start of value
	for(index = 0; index < line.size(); index++) {
		if(line[index] == ':') {
			break;
		}
	}
	index++;
	// Remove initial spaces
	for(; index < line.size(); index++) {
		if(line[index] != ' ') {
			break;
		}
	}
	
	// Set value as rest of line
	for(; index < line.size(); index++) {
		value += line[index];
	}

	while(value.back() == ' ') {
		value.pop_back();
	}

	config[key] = value;
}

void Ea::config_checker() const {

}