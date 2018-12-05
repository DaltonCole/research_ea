#include "ea.h"


template <typename T> 
Ea<T>::Ea() {
}

template <typename T>
Ea<T>::Ea(vector<shared_ptr<T> >& initial_population) {
	population = initial_population;
}

template <typename T>
vector<shared_ptr<T> > Ea<T>::parent_selection() {
	if(config["Parent Selection"] == "Fitness Proportionate Selection") {
		return fitness_proportionate_selection(config["Number of parents"]);
	}
}

// Assumed every person in population already has fitness called
template <typename T>
vector<shared_ptr<T> > Ea<T>::fitness_proportionate_selection(const int size) const {
	vector<shared_ptr<T> > selected;
	float total_fitness;

	for(const auto& person : population) {
		total_fitness = person -> get_fitness();
	}

	float choosen_number;
	float running_total;
	while(selected.size() < size) {
		running_total = 0;
		choosen_number = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/total_fitness));
		for(auto& person : population) {
			running_total += person -> get_fitness();
			if(choosen_number <= running_total) {
				selected.push_back(person);
				break;
			}
		}
	}

	return selected;
}

template <typename T> 
void Ea<T>::default_configurations() {

}

template <typename T> 
void Ea<T>::config_reader() {
	string line;
	ifstream file("config.txt");
	if(file.is_open()) {
		while(getline(file, line)) {
			// For each configuration
			for(auto& conf : config) {
				// If config key is in line
				if(line.find(conf.first) != std::string::npos) {
					// Add config
					config_reader_helper(conf, line);
					break;
				}
			}
		}
		file.close();
	} else {
		throw "config.txt failed to open!";
	}
}

template <typename T> 
void Ea<T>::config_reader_helper(const string& key, const string& line) {
	string value = "";
	int index = 0;
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