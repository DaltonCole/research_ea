#ifndef BASE_GRAMMAR_H
#define BASE_GRAMMAR_H

#include <iostream>
#include <utility>
#include <vector>
#include <memory>
using namespace std;

class Base_grammar {
	public:
		virtual vector<string> generate_strings() = 0;
		virtual float find_fitness() = 0;
		virtual float get_fitness() const = 0;

		virtual void mutate() = 0;

		virtual void abstract() = 0;

		virtual pair<shared_ptr<Base_grammar>, shared_ptr<Base_grammar> > 
		recombination(shared_ptr<Base_grammar>& mate) = 0;

		virtual void print(ostream& os) const = 0;

		bool success() const;

		friend ostream & operator << (ostream&, const Base_grammar&);

	//private:
		static const int words_generated_count;
		static float mutate_rate; // Between 0 and 1
};

#endif