#ifndef EA_H
#define EA_H

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include "base_grammar.h"
using namespace std;

class Ea {
	public:
		Ea();
		Ea(vector<shared_ptr<Base_grammar> >& initial_population, const char* config_file);

		void run();

		// Parent Selection //
		vector<shared_ptr<Base_grammar> > parent_selection();
		//////////////////////

		vector<shared_ptr<Base_grammar> > fitness_proportionate_selection(const uint size) const;
		vector<shared_ptr<Base_grammar> > tournament_selection(const uint size) const;

		// Configuration //
		void default_configurations();
		void config_reader(const char* config_file);
		void config_reader_helper(const string& key, const string& line);
		void config_checker() const;
		///////////////////

	private:
		unordered_map<string, string> config;
		vector<shared_ptr<Base_grammar> > population;
};

#endif