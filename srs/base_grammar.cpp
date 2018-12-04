#include "base_grammar.h"

const int Base_grammar::words_generated_count = 100;
float Base_grammar::mutate_rate = 0.1;

ostream & operator << (ostream& os, const Base_grammar& grammar) {
	grammar.print(os);
	return os;
}