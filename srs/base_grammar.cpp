#include "base_grammar.h"

const int Base_grammar::words_generated_count = 100;
float Base_grammar::mutate_rate = 1;//0.1; // NOTE: change

bool Base_grammar::success() const {
	if((static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) < mutate_rate) {
		return true;
	}
	return false;
}

ostream & operator << (ostream& os, const Base_grammar& grammar) {
	grammar.print(os);
	return os;
}