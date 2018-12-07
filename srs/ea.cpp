#include "ea.h"

shared_ptr<Base_grammar> Ea::best_grammar = nullptr;

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
	best_grammar = population[0];
	//best_grammar = shared_ptr<Base_grammar>(new decltype(*population[0])(*population[0]));
}

void ctrl_c_handler(int s) {
	cout << endl;
	cout << *Ea::best_grammar << endl;
	cout << "Fitness: " << Ea::best_grammar -> get_fitness() << endl;
	exit(1);
}

void Ea::run() {
	// Set up signal handler to print out best grammar so far
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = ctrl_c_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	// Keep track of generation
	int genration = 0;
	cout << "On iteration: " << genration << "\r";
	fflush(stdout);
	genration++;

	// Set initial population's fitness
	update_fitness(population);

	while(true) {
		// Print out progress
		cout << "On iteration: " << genration << " \t"
			 << "Current best fitness: " << (best_grammar -> get_fitness())
			 << " / " << (Base_grammar::words_generated_count * 100)
			 << "\t\t\t\r";
		fflush(stdout);
		genration++;

		// Parent Selection
		auto parents = parent_selection();

		// Generate children
		auto children = generate_children(parents);

		// Combine population
		for(auto& child : children) {
			population.push_back(child);
		}

		// Mutate
		mutate(population);

		// Update fitness
		update_fitness(population);

		// --- Kill --- //
		sort_population();
		kill_population();

		// Update hall of fame best grammar
		if(population.size() > 0 && population[0] -> get_fitness() > best_grammar -> get_fitness()) {
			best_grammar = population[0];
		}
	}
}

void Ea::default_configurations() {
	config["Population Size"] = "100";
	config["Parent Selection"] = "Fitness Proportionate Selection";
	config["Number of parents"] = "50";
	config["Number of children"] = "50";
	config["Mutation rate"] = "0.01";
	config["Samples generated per fitness evaluation"] = "10";
}

vector<shared_ptr<Base_grammar> > Ea::parent_selection() {
	if(config["Parent Selection"] == "Fitness Proportionate Selection") {
		return fitness_proportionate_selection(stoi(config["Number of parents"]));
	} else if(config["Parent Selection"] == "Tournament Selection") {
		return tournament_selection(stoi(config["Number of parents"]));
	}

	return {};
}



// Children //
vector<shared_ptr<Base_grammar> > Ea::generate_children
(const vector<shared_ptr<Base_grammar> >& parents) {
	// Children population
	vector<shared_ptr<Base_grammar> > children;
	// Subset of parent population - Gets decreased as parents are selected
	unordered_set<shared_ptr<Base_grammar> > parents_to_choose_from;

	// Populate parents to choose from
	for(const auto& par : parents) {
		parents_to_choose_from.insert(par);
	}

	while(children.size() < stoi(config["Number of children"])) {
		// Shuffle in all possible parents if we need more options
		if(parents_to_choose_from.size() < 2) {
			for(const auto& par : parents) {
				parents_to_choose_from.insert(par);
			}
		}

		auto first_mate = random_grammar_from_unordered_set(parents_to_choose_from);
		parents_to_choose_from.erase(parents_to_choose_from.find(first_mate));
		auto second_mate = random_grammar_from_unordered_set(parents_to_choose_from);
		parents_to_choose_from.erase(parents_to_choose_from.find(second_mate));

		auto couple = first_mate -> recombination(second_mate);

		children.push_back(couple.first);
		children.push_back(couple.second);
	}

	return children;
}

// Mutate //
void Ea::mutate(vector<shared_ptr<Base_grammar> >& mutate_population) const {
	for(auto& person : mutate_population) {
		person -> mutate();
	}
}

// Update Fitness //
void Ea::update_fitness(vector<shared_ptr<Base_grammar> >& fitness_population) const {
	// Async threads
	vector<future<void> > threads;

	for(auto& person : fitness_population) {
		// Call find fitness
		person -> find_fitness();
		//auto test = [=]{person -> find_fitness();};
		//threads.push_back(async(test));
		//threads.emplace_back(person -> find_fitness);
		//threads.push_back((*person));
		//auto test = [=]{person -> find_fitness();};
		//threads.push_back(test);
	}
	//thread thread_1(&test_class::createS, this, 0, nCells/2, map1); 


	for(auto& th : threads) {
		// Join threads
		th.get();
	}
}

// Kill //
void Ea::sort_population() {
	sort(population.begin(), population.end(),
		[](const shared_ptr<Base_grammar>& a, const shared_ptr<Base_grammar>& b) -> bool {
			return a -> get_fitness() > b -> get_fitness();
		});
}

void Ea::kill_population() {
	population = survivor_selection();
}

vector<shared_ptr<Base_grammar> > Ea::survivor_selection() {
	return parent_selection();
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

	// Random elements
	shared_ptr<Base_grammar> one;
	shared_ptr<Base_grammar> two;

	while(selected.size() < size) {
		// If need more to choose from, add from population
		if(population_to_pick_from.size() < 2) {
			for(auto& person : population) {
				population_to_pick_from.insert(person);
			}
		}

		// Randomly select two grammars
		one = random_grammar_from_unordered_set(population_to_pick_from);
		two = random_grammar_from_unordered_set(population_to_pick_from);

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
		string s = "config.txt failed to open!";
		cerr << s << endl;
		throw s;
	}

	// Update static necessary static variables
	Base_grammar::mutate_rate = stof(config["Mutation rate"]);
	Base_grammar::words_generated_count = stoi(config["Samples generated per fitness evaluation"]);

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

shared_ptr<Base_grammar> Ea::random_grammar_from_unordered_set(const unordered_set<shared_ptr<Base_grammar> > options) const {
	if(options.size() == 0) {
		string s = "Error in random_grammar_from_unordered_set! Population size is 0!";
		cerr << s << endl;
		throw s;
	}

	// Select random number
	int count = rand() % options.size();
	// Go to random index
	auto it = options.begin();
	for(int i = 0; i < count; i++, it++) {}
	return *it;
}