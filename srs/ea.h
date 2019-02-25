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
#include <iterator>	// inserter
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
		//////////////////////////////////////////////////////////////////////
		/// @brief	Default constructor
		//////////////////////////////////////////////////////////////////////
		Ea();

		//////////////////////////////////////////////////////////////////////
		/// @brief	Constructor where the initial population is either created
		/// 		from "Sample file directory" if exists, or 
		/// 		create_pop otherwise.
		/// @param[in] create_pop 	Function pointer pointing to a function
		/// 		that will create grammars. If "Sample file directory" is
		/// 		defined in the config file, then the config takes 
		/// 		precedence
		/// @param[in] 	config_file 	File containing the EA configurations
		//////////////////////////////////////////////////////////////////////
		Ea(shared_ptr<Base_grammar> (*create_pop) (void), const char* config_file);

		//////////////////////////////////////////////////////////////////////
		/// @brief	Constructor where the initial population is given.
		/// @param[in] 	inital_population 	The initial population of the EA.
		/// 		Requires at least "Population Size" members. If more are
		/// 		given, they are discarded.
		/// @param[in] 	config_file 	File containing the EA configurations
		//////////////////////////////////////////////////////////////////////
		Ea(vector<shared_ptr<Base_grammar> >& initial_population, const char* config_file);

		//////////////////////////////////////////////////////////////////////
		/// @brief	Starts the EA. Prints out fitness and generation progress.
		/// 		Saves best grammar to a file if "Save File" is set in the
		/// 		config file.
		//////////////////////////////////////////////////////////////////////
		void run();

		//////////////////////////////////////////////////////////////////////
		/// @brief	Intercepts ctrl-c. Prints best grammar and saves it
		/// 		if "save_file" is not "".
		/// @param 	s 	Not used.
		//////////////////////////////////////////////////////////////////////
		friend void ctrl_c_handler(int s);

		static shared_ptr<Base_grammar> best_grammar; 	///< Best grammar
														///< found so far
		static string save_file; 	///< File to save best grammar to upon
									///< ctrl-c. Not saved if "Save File"
									///< does not exist in config file
									///< Initialized to "".

	private:
		//////////////////////////////////////////////////////////////////////
		/// @brief	Helper function for the constructor that generates the
		/// 		initial population. Vector is either created
		/// 		from "Sample file directory" if exists, or 
		/// 		create_pop otherwise.
		/// @pre
		/// @param[in] create_pop 	Function pointer pointing to a function
		/// 		that will create grammars. If "Sample file directory" is
		/// 		defined in the config file, then the config takes 
		/// 		precedence
		/// @param[in]	size 	Size of output vector. Number of samples
		/// 		to generate.
		/// @return	Vector of initial grammars
		//////////////////////////////////////////////////////////////////////
		vector<shared_ptr<Base_grammar> > generate_population
		(shared_ptr<Base_grammar> (*create_pop) (void), const uint size);

		//////////////////////////////////////////////////////////////////////
		/// @brief	Prints the current progress of the EA.
		/// @param[in]	best_in_geneation 	Best grammar in current generation
		/// @param[in]	generation 	Current generation	
		/// @param[in]	step_count 	Current step number
		/// @param[in]	step 		Current step name
		//////////////////////////////////////////////////////////////////////
		void print_progress(const shared_ptr<Base_grammar>& best_in_generation, 
			const int genration, const int step_count, const string& step);

		// --- Parent Selection --- //

		//////////////////////////////////////////////////////////////////////
		/// @brief	Calls the appropriate parent_selection method described in
		/// 		the "Parent Selection" section of the config file.
		/// @param[in]	number_to_return 	The number of parents to select
		/// @return	A vector of shared pointers to possible parents.
		//////////////////////////////////////////////////////////////////////
		vector<shared_ptr<Base_grammar> > parent_selection();
		
		//////////////////////////////

		// --- Children --- //

		//////////////////////////////////////////////////////////////////////
		/// @brief	Creates children up to "Number of children" from the config
		/// 		file. Randomly chooses parents (without replacement unless
		/// 		needed to reach count) and uses Base_grammar's recombination
		/// 		function to generate two new children
		/// @pre	"parents" should contain more elements than 
		/// 		"Number of children" specified in config file. If this is
		/// 		not true, parents are re-added to possible parent pool.
		/// @param[in] 	parents 	Parents to generate children from.
		/// @return	Children generated by recombination of the parents.
		//////////////////////////////////////////////////////////////////////
		vector<shared_ptr<Base_grammar> > 
		generate_children(const vector<shared_ptr<Base_grammar> >& parents);

		//////////////////////

		// --- Mutate --- //

		//////////////////////////////////////////////////////////////////////
		/// @brief	Mutate the population according to Base_grammar's mutate
		/// 		function. 
		/// @post	Every member of "mutate_population" is mutated (according
		/// 		to a probability)
		/// @param[in, out]	mutate_population 	The population to mutate via
		/// 		Base_grammar's mutate method
		//////////////////////////////////////////////////////////////////////
		void mutate(vector<shared_ptr<Base_grammar> >& mutate_population) const;
		
		////////////////////

		// --- Update Fitness --- //

		//////////////////////////////////////////////////////////////////////
		/// @brief	Update the population's fitness according to Base_grammar's
		/// 		"find_fitness_thread" method
		/// @post	Every member of "fitness_population" has its fitness updated
		/// @param[in, out]	fitness_population 	The population to update its 
		/// 		fitness.
		//////////////////////////////////////////////////////////////////////
		void update_fitness(vector<shared_ptr<Base_grammar> >& fitness_population) const;
		
		////////////////////////////

		// --- Kill --- //

		//////////////////////////////////////////////////////////////////////
		/// @brief	Kill off part of the population. Uses survivor_selection
		/// 		to determine who survives.
		/// @post	"population" is the same size as "Population Size" in config
		//////////////////////////////////////////////////////////////////////
		void kill_population();

		//////////////////////////////////////////////////////////////////////
		/// @brief	Applies "Survival Selection" algorithm from config file
		/// 		on the "population".
		/// @pre	"Survival Selection" exists in the config file
		/// @return	Survivors of the "population" based on the algorithm in
		/// 		the "Survival Selection" field of the config file.
		//////////////////////////////////////////////////////////////////////
		vector<shared_ptr<Base_grammar> > survivor_selection();
		
		//////////////////

		// --- Algorithms --- //

		//////////////////////////////////////////////////////////////////////
		/// @brief	Generates a vector of grammars by applying an algorithm.
		/// 		The returned vector contains select members of the population
		/// @param[in]	method 	Algorithm to use. Options are in config file
		/// @param[in] 	number_to_return 	How large returned vector should be
		/// @return	Vector of selected individuals of the population based on
		/// 		the algorithm selected.
		//////////////////////////////////////////////////////////////////////
		vector<shared_ptr<Base_grammar> > algorithm_selection
		(const string& method, const int number_to_return);

		//////////////////////////////////////////////////////////////////////
		/// @brief	Applies fitness proportionate selection to the population
		/// @pre	Population contains at least 2 individuals
		/// @param[in]	size 	Size of returned vector
		/// @return	Vector of selected individuals of the population based on
		/// 		the fitness proportionate selection algorithm. 
		//////////////////////////////////////////////////////////////////////
		vector<shared_ptr<Base_grammar> > fitness_proportionate_selection(const uint size) const;
		
		//////////////////////////////////////////////////////////////////////
		/// @brief	Applies tournament selection to the population
		/// @pre	Population contains at least 2 individuals
		/// @param[in]	size 	Size of returned vector
		/// @return	Vector of selected individuals of the population based on
		/// 		the tournament selection algorithm. 
		//////////////////////////////////////////////////////////////////////
		vector<shared_ptr<Base_grammar> > tournament_selection(const uint size) const;

		////////////////////////		

		// --- Configuration --- //

		//////////////////////////////////////////////////////////////////////
		/// @brief	Sets "config" to default configurations
		/// @post	"config" contains default configurations
		//////////////////////////////////////////////////////////////////////
		void default_configurations();

		//////////////////////////////////////////////////////////////////////
		/// @brief	Reads in config data from config file argument. Ignoring
		/// 		lines starting with '#', adds each key value pair to
		/// 		the "config" map.
		/// @post 	"config" contains the key value pairs from the config file
		/// @param[in]	config_file	File to read in config data from	
		//////////////////////////////////////////////////////////////////////
		void config_reader(const char* config_file);

		//////////////////////////////////////////////////////////////////////
		/// @brief	Parses a line and saves value in "config"
		/// @post	"config" contains the key value pairs from "line"
		/// @param[in]	line 	Line to parse to obtain key value pair	
		//////////////////////////////////////////////////////////////////////
		void config_reader_helper(const string& line);

		//////////////////////////////////////////////////////////////////////
		/// @brief 	Uses some "config" configurations to set some variables
		/// @post	Code_coverage::kcov_or_bool is set; 
		/// 		Code_coverage::executable
		/// 		Code_coverage::kcov_saved_path
		/// 		Ea::save_file
		//////////////////////////////////////////////////////////////////////
		void config_checker();
		
		///////////////////////////

		// --- Helper Functions --- //

		//////////////////////////////////////////////////////////////////////
		/// @brief	Sort "population" from greatest fitness to least
		/// @post	"population" is sorted from greatest fitness to least
		//////////////////////////////////////////////////////////////////////
		void sort_population();

		//////////////////////////////////////////////////////////////////////
		/// @brief	Select a random grammar from an unordered set
		/// @pre	"options"'s must be 1 or greater
		/// @param[in] 	options 	Unordered set to randomly select a grammar
		/// 		from.
		/// @return Randomly chosen grammar.
		//////////////////////////////////////////////////////////////////////
		shared_ptr<Base_grammar> random_grammar_from_unordered_set
		(const unordered_set<shared_ptr<Base_grammar> > options) const;
		
		//////////////////////////////

		unordered_map<string, string> config;			///< Configuration map
		vector<shared_ptr<Base_grammar> > population; 	///< EA population
};

#endif