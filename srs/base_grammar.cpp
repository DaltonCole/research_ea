#include "base_grammar.h"

int Base_grammar::words_generated_count = 10;
float Base_grammar::mutate_rate = 0.1;//0.1; // NOTE: change

bool Base_grammar::operator==(const Base_grammar& other) const {
	ostringstream stream1, stream2;
	stream1 << (*this);
	stream2 << other;

	return (stream1.str() == stream2.str());
}

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