#ifndef EA_H
#define EA_H

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <thread>
#include <future>
#include <experimental/filesystem>
// sigaction //
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
///////////////
#include "base_grammar.h"
#include "code_coverage.h"
#include "binary_map_grammar.h"
using namespace std;

class Ea {
	public:
		Ea();
		Ea(shared_ptr<Base_grammar> (*create_pop) (void), const char* config_file);
		Ea(vector<shared_ptr<Base_grammar> >& initial_population, const char* config_file);

		vector<shared_ptr<Base_grammar> > generate_population(shared_ptr<Base_grammar> (*create_pop) (void));


		void run();
		void print_progress(const int genration, const int step_count, const string& step);

		friend void ctrl_c_handler(int s);

		// Parent Selection //
		vector<shared_ptr<Base_grammar> > parent_selection(const int number_to_return);
		//////////////////////

		// Children //
		vector<shared_ptr<Base_grammar> > 
		generate_children(const vector<shared_ptr<Base_grammar> >& parents);
		//////////////

		// Mutate //
		void mutate(vector<shared_ptr<Base_grammar> >& mutate_population) const;
		////////////

		// Update Fitness //
		void update_fitness(vector<shared_ptr<Base_grammar> >& fitness_population) const;
		////////////////////

		// Kill //
		void sort_population();
		void kill_population();
		vector<shared_ptr<Base_grammar> > survivor_selection();
		//////////

		vector<shared_ptr<Base_grammar> > fitness_proportionate_selection(const uint size) const;
		vector<shared_ptr<Base_grammar> > tournament_selection(const uint size) const;

		// Configuration //
		void default_configurations();
		void config_reader(const char* config_file);
		void config_reader_helper(const string& key, const string& line);
		void config_checker();
		///////////////////

		// Helper Functions //
		shared_ptr<Base_grammar> 
		random_grammar_from_unordered_set(const unordered_set<shared_ptr<Base_grammar> > options) const;
		//////////////////////

		static shared_ptr<Base_grammar> best_grammar;
	private:
		unordered_map<string, string> config;
		vector<shared_ptr<Base_grammar> > population;
};

#endif