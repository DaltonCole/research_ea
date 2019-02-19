#include "ea.h"

shared_ptr<Base_grammar> Ea::best_grammar = nullptr;
string Ea::save_file = "";

Ea::Ea() {
}

Ea::Ea(shared_ptr<Base_grammar> (*create_pop) (void), const char* config_file) {
	default_configurations();
	config_reader(config_file);

	vector<shared_ptr<Base_grammar> > initial_population = generate_population(create_pop);

	if(static_cast<uint>(stoi(config["Population Size"])) != initial_population.size()) {
		if(static_cast<uint>(stoi(config["Population Size"])) < initial_population.size()) {
			while(static_cast<uint>(stoi(config["Population Size"])) != initial_population.size()) {
				initial_population.pop_back();
			}
		} else {
			string s = "Incorrect population size given! " ;
			s += to_string(initial_population.size()) + " was given, but " + config["Population Size"] + " was required.";
			cerr << s << endl;
			throw s;
		}
	}

	population = initial_population;
	best_grammar = population[0] -> clone();
}

Ea::Ea(vector<shared_ptr<Base_grammar> >& initial_population, const char* config_file) {
	default_configurations();
	config_reader(config_file);

	if(static_cast<uint>(stoi(config["Population Size"])) != initial_population.size()) {
		if(static_cast<uint>(stoi(config["Population Size"])) < initial_population.size()) {
			while(static_cast<uint>(stoi(config["Population Size"])) != initial_population.size()) {
				initial_population.pop_back();
			}
		} else {
			string s = "Incorrect population size given! " ;
			s += to_string(initial_population.size()) + " was given, but " + config["Population Size"] + " was required.";
			cerr << s << endl;
			throw s;
		}
	}

	population = initial_population;
	best_grammar = population[0] -> clone();
}

void ctrl_c_handler(int s) {
	// Ignore unused variable warning
	(void)s;
	/*
	cout << endl;
	cout << "Fitness pre-abstract: " << Ea::best_grammar -> get_fitness() << endl;

	for(int i = 0; i < 100; i++) {
		Ea::best_grammar -> abstract();
	}
	Ea::best_grammar -> find_fitness();
	*/
	Ea::best_grammar -> abstract();

	if(Ea::save_file != "") {
		cout << endl << "Quiting" << endl;
		cout << "Look at " << Ea::save_file << " for results." << endl;
		ofstream file(Ea::save_file);
		file << *Ea::best_grammar << endl;

		file << "Samples:" << endl;
		int i = 0;
		for(const auto& string : Ea::best_grammar -> generate_strings()) {
			i++;
			file << string << endl;
			if(i == 10) {
				break;
			}
		}

		file << endl << "Fitness: " << Ea::best_grammar -> get_fitness() << endl;
		file.close();
	} else {
		cout << endl;
		cout << *Ea::best_grammar << endl;
		cout << "Fitness: " << Ea::best_grammar -> get_fitness() << endl;
	}

	// Clean up tmp directories from code_coverage
	Code_coverage::clean_up();

	exit(1);
}

vector<shared_ptr<Base_grammar> > Ea::generate_population
(shared_ptr<Base_grammar> (*create_pop) (void)) {
	vector<shared_ptr<Base_grammar> > initial_population;

	if(config[("Sample file directory")] != "") {
		for(auto& path : std::experimental::filesystem::directory_iterator(config[("Sample file directory")])) {
			std::ifstream t(path.path());
			std::stringstream buffer;
			buffer << t.rdbuf();

			initial_population.emplace_back(new Binary_map_grammar(buffer.str()));
		}
	} else {
		for(uint i = 0; i < static_cast<uint>(stoi(config["Population Size"])); i++) {
			initial_population.push_back((*create_pop)());
		}
	}

	return initial_population;
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
	print_progress(genration, 7, "Update Fitness");
	genration++;

	// Set initial population's fitness
	update_fitness(population);

	while(true) {
		// Parent Selection
		print_progress(genration, 1, "Parent Selection");
		auto parents = parent_selection();

		// Generate children
		print_progress(genration, 2, "Generate Children");
		auto children = generate_children(parents);

		// Combine population
		print_progress(genration, 3, "Combine Population");
		for(auto& child : children) {
			// Combine child with the population
			population.push_back(child);
		}

		// Mutate
		print_progress(genration, 4, "Mutate Population");
		mutate(population);

		// Update fitness
		print_progress(genration, 5, "Update Fitness");
		update_fitness(population);

		// --- Kill --- //
		print_progress(genration, 6, "Kill ψ(｀∇´)ψ");
		kill_population();

		// Update hall of fame best grammar
		auto best_in_generation = *max_element(population.begin(), population.end(),
			[](const shared_ptr<Base_grammar>& a, const shared_ptr<Base_grammar>& b) -> bool {
			return a -> get_fitness() < b -> get_fitness();
			});

		if(best_in_generation -> get_fitness() > best_grammar -> get_fitness()) {
			best_grammar = best_in_generation -> clone();
		}

		genration++;
	}
}

void Ea::print_progress(const int genration, const int step_count, const string& step) {
	cout << "On iteration: " << genration << " \t"
		 << "Current best fitness: " << (best_grammar -> get_fitness())
		 << " / " << (Base_grammar::words_generated_count * 100)
		 << "\t"
		 << step_count << " / 6 \t" << step  
		 << "             \t\t\t\t\t\r";
	
	fflush(stdout);
}

vector<shared_ptr<Base_grammar> > Ea::parent_selection() {
	return algorithm_selection(config["Parent Selection"], 
		static_cast<uint>(stoi(config["Number of parents"])));
}

vector<shared_ptr<Base_grammar> > Ea::algorithm_selection(const string& method,
	const int number_to_return) {

	if(method == "Fitness Proportionate Selection") {
		return fitness_proportionate_selection(number_to_return);
	} else if(method == "Tournament Selection") {
		return tournament_selection(number_to_return);
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

	while(children.size() < static_cast<uint>(stoi(config["Number of children"]))) {
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
	// Threads
	//vector<future<void> > threads;
	vector<thread> threads;

	for(auto& person : fitness_population) {
		// Call find fitness
		//person -> find_fitness();

		threads.push_back(person -> find_fitness_thread());
		//person -> find_fitness();



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
		th.join();
	}

	// Clean up tmp directories from code_coverage
	Code_coverage::clean_up();
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
	return algorithm_selection(config["Survival Selection"],
		static_cast<uint>(stoi(config["Population Size"])));
}

// Assumed every person in population already has fitness called
// NOTE: GETS STUCK SOMETIMES
vector<shared_ptr<Base_grammar> > Ea::fitness_proportionate_selection(const uint size) const {
	vector<shared_ptr<Base_grammar> > selected;
	float total_fitness = 0;
	float minimum_fitness = 0;

	// Population to pick from
	unordered_set<shared_ptr<Base_grammar> > population_to_pick_from;
	// Populate population to pick from
	for(auto& person : population) {
		population_to_pick_from.insert(person);
	}

	// Get minimum fitness
	for(const auto& person : population) {
		minimum_fitness = min(person -> get_fitness(), minimum_fitness);
	}

	// Get Total fitness
	for(const auto& person : population) {
		total_fitness += (person -> get_fitness() - minimum_fitness);
	}

	float choosen_number;
	float running_total;

	// While we need to add more
	while(selected.size() < size) {	
		// If need more to choose from, add from population
		if(population_to_pick_from.size() < 2) {
			for(auto& person : population) {
				population_to_pick_from.insert(person);
				total_fitness += person -> get_fitness();
			}
		}

		// Reset running total
		running_total = 0;
		// Choose a random float between [0 - absolute_fitness]
		choosen_number = total_fitness * static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));

		// For each person
		for(auto& person : population_to_pick_from) {
			// Add to running total
			running_total += (person -> get_fitness() - minimum_fitness);
			// If running total is greater than chosen number
			if(choosen_number <= running_total) {
				// They have been selected
				selected.push_back(person);
				// Decrement total fitness
				total_fitness -= person -> get_fitness();
				// Remove from possible picks
				population_to_pick_from.erase(population_to_pick_from.find(person));
				break;
			}
		}
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


void Ea::default_configurations() {
	config["Population Size"] = "100";
	config["Parent Selection"] = "Fitness Proportionate Selection";
	config["Survival Selection"] = "Fitness Proportionate Selection";
	config["Number of parents"] = "50";
	config["Number of children"] = "50";
	config["Mutation rate"] = "0.01";
	config["Samples generated per fitness evaluation"] = "10";

	config["Kcov"] = "false";
	config["Executable"] = "/home/drc/Desktop/Research/ea/tester_parser/mipl_parser";
	config["Sample file directory"] = "/home/drc/Desktop/Research/ea/json/";

	config["Save File"] = "/home/drc/Desktop/Research/ea/best_grammar.txt";
	/*
	config[""] = ;
	*/
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

			config_reader_helper(line);
		}
		file.close();
	} else {
		string s = "config.txt failed to open!";
		cerr << s << endl;
		throw s;
	}

	// Update static necessary static variables
	Base_grammar::mutate_rate = stof(config["Mutation rate"]);
	Base_grammar::words_generated_count = static_cast<uint>(stoi(config["Samples generated per fitness evaluation"]));

	config_checker();
}

void Ea::config_reader_helper(const string& line) {
	string value = "";
	string key = "";
	uint index = 0;
	// Find start of value
	for(index = 0; index < line.size(); index++) {
		if(line[index] == ':') {
			break;
		}
		key += line[index];
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

void Ea::config_checker() {
	if(config.find("Kcov") != config.end()) {
		if(config["Kcov"] == "True" || config["Kcov"] == "true") {
			Code_coverage::kcov_or_bool = true;
		} else {
			Code_coverage::kcov_or_bool = false;
		}
	}

	if(config.find("Executable") != config.end()) {
		Code_coverage::executable = config["Executable"];

		string executable_name = "";
		for(const auto& c : config["Executable"]) {
			if(c == '/') {
				executable_name = "";
			} else {
				executable_name += c;
			}
		}

		Code_coverage::kcov_saved_path = executable_name;
	}	

	if(config.find("Save File") != config.end()) {
		Ea::save_file = config["Save File"];
	}

	// Valid strings
	if(config.find("Valid Strings") != config.end()) {
		string word = "";
		for(const auto& c : config["Valid Strings"]) {
			if(c != ' ') {
				word += c;
			} else {
				Code_coverage::valid_strings.push_back(word);
				word = "";
			}
		}
		if(word != "") {
			Code_coverage::valid_strings.push_back(word);
		}
	}

	// Invalid Strings
	if(config.find("Invalid Strings") != config.end()) {
		string word = "";
		for(const auto& c : config["Invalid Strings"]) {
			if(c != ' ') {
				word += c;
			} else {
				Code_coverage::invalid_strings.push_back(word);
				word = "";
			}
		}
		if(word != "") {
			Code_coverage::invalid_strings.push_back(word);
		}
	}
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