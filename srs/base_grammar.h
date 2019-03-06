#ifndef BASE_GRAMMAR_H
#define BASE_GRAMMAR_H

//////////////////////////////////////////////////////////////////////
/// @file 	base_grammar.h
/// @author	Dalton Cole
///	@brief	This header file declares the Base_grammar class.
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <utility>
#include <vector>
#include <unordered_set>
#include <memory>
#include <thread>
#include <sstream>      // std::ostringstream
using namespace std;

//////////////////////////////////////////////////////////////////////
/// @class Base_grammar
/// @brief Abstract grammar class
//////////////////////////////////////////////////////////////////////
class Base_grammar {
	public:
		//////////////////////////////////////////////////////////////////////
		/// @brief	Compares two grammars. Grammars are compared using the
		/// 		<< operator.
		/// @param	paramName2 description of the parameter
		/// @return	True if both grammars have the same <<, false otherwise
		//////////////////////////////////////////////////////////////////////
		virtual bool operator==(const Base_grammar& other) const;

		//////////////////////////////////////////////////////////////////////
		/// @brief 	Creates a copy of the original object wrapped in a shared 
		/// 		pointer.
		/// @return A copy of this, wrapped in a shared_ptr
		//////////////////////////////////////////////////////////////////////
		virtual shared_ptr<Base_grammar> clone() const = 0;

		//////////////////////////////////////////////////////////////////////
		/// @brief 	Generates sample strings from the language generated from 
		/// 		the grammar. Each string is unique
		/// @return An unordered set of sample strings
		//////////////////////////////////////////////////////////////////////
		virtual unordered_set<string> generate_strings() = 0;

		//////////////////////////////////////////////////////////////////////
		/// @brief 	Creates a thread that calls the find_fitness function.
		/// @return A thread running the find_fitness function
		//////////////////////////////////////////////////////////////////////
		virtual std::thread find_fitness_thread() = 0;

		//////////////////////////////////////////////////////////////////////
		/// @brief 	Finds the fitness of the underlying grammar.
		/// @post	Modifies "fitness" member variable with the found fitness
		/// @return The new fitness value
		//////////////////////////////////////////////////////////////////////
		virtual float find_fitness() = 0;

		//////////////////////////////////////////////////////////////////////
		/// @brief 	Getter for "fitness".
		/// @pre	"fitness" has previously been set by "find_fitness()" and 
		///			has been updated since last change to the grammar.
		/// @return The previously set fitness value
		//////////////////////////////////////////////////////////////////////
		virtual float get_fitness() = 0;

		//////////////////////////////////////////////////////////////////////
		/// @brief 	Mutates the grammar.
		/// @pre	Mutations rate defaults to 0.1, but 
		/// 		Base_grammar::mutate_rate should be updated with the
		///			desired value
		//////////////////////////////////////////////////////////////////////
		virtual void mutate() = 0;

		//////////////////////////////////////////////////////////////////////
		/// @brief 	Makes the grammar more abstract using a predefined set of 
		/// 		rules.
		/// 		Example: Change a single string into a regex
		/// @param[in]	guarantee_abstract 	Guarantee certain abstracts 
		/// 		instead of probabilistically choosing them based on
		/// 		mutation rate.	
		//////////////////////////////////////////////////////////////////////
		virtual void abstract(const bool guarantee_abstract = false) = 0;

		//////////////////////////////////////////////////////////////////////
		/// @brief 	Combines *this with mates to produce children. Each 
		/// 		child shares some gnomes from all parents.
		/// @param[in]	mates 	The other parents
		/// @param[in]	method 	Method to use to generate children
		/// @param[in]	number_of_children 	Number of children to generate
		/// @return	A vector of children
		//////////////////////////////////////////////////////////////////////
		virtual vector<shared_ptr<Base_grammar> > 
		recombination(const vector<shared_ptr<Base_grammar> >& mates,
			const string& method, const uint number_of_children) = 0;

		//////////////////////////////////////////////////////////////////////
		/// @brief 	Prints out the grammar
		/// @param[out]	os 	Ostream to be outputted to.
		//////////////////////////////////////////////////////////////////////
		virtual void print(ostream& os) const = 0;

		//////////////////////////////////////////////////////////////////////
		/// @brief 	Returns true if a random value between [0,1) is less than
		/// 		the mutation rate. 
		/// @return	True if random value is less than mutation rate.
		///			False otherwise.
		//////////////////////////////////////////////////////////////////////
		bool success() const;

		//////////////////////////////////////////////////////////////////////
		/// @brief 	Calls the "print" member function
		/// @param	os 	The ostream to use with "print"
		/// @param[in]	grammar 	The grammar to call "print" on
		/// @return	A pair of children
		//////////////////////////////////////////////////////////////////////
		friend ostream & operator << (ostream& os, const Base_grammar& grammar);

		static int words_generated_count; 	///< Number of words to generate for generate_string()
		static float mutate_rate; ///< The mutation rate. Between [0-1)

	protected:
		float fitness;	///< Grammar's fitness
};

namespace std {
	template<>
	struct hash<Base_grammar> {
		size_t operator()(const Base_grammar& grammar) const {
			ostringstream stream;
			stream << grammar;
			return hash<string>()(stream.str());
		}
	};
}

#endif