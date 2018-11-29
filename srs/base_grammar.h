#ifndef BASE_GRAMMAR_H
#define BASE_GRAMMAR_H

#include <utility>
using namespace std;

class Base_grammar {
	public:
		virtual float find_fitness() = 0;
		virtual float get_fitness() const = 0;

		virtual void mutate() = 0;

		virtual void abstract() = 0;

		virtual pair<shared_ptr<Base_grammar>, shared_ptr<Base_grammar> > recombination(const shared_ptr<Base_grammar>& mate) const = 0;

		virtual vector<string> generate_sample_strings() = 0;

	private:
		static const int words_generated_count;
		static float mutate_rate;
};

const int Base_grammar::words_generated_count = 100;
float Base_grammar::mutate_rate = 0.1;

#endif