#ifndef EA_H
#define EA_H

#include <unordered_map>
#include <memory>
#include <vector>
#include <fstream>
#include <string>
using namespace std;

template <typename T> 
class Ea {
	public:
		Ea();
		Ea(vector<shared_ptr<T> >& initial_population);

		// Parent Selection //
		vector<shared_ptr<T> > parent_selection();
		//////////////////////

		vector<shared_ptr<T> > fitness_proportionate_selection(const int size) const;

		// Configuration //
		void default_configurations();
		void config_reader();
		void config_reader_helper(const string& key, const string& line);
		///////////////////

	private:
		unordered_map<string, string> config;
		vector<shared_ptr<T> > population;
};

#include "ea.hpp"

#endif