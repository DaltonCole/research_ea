#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <thread>
#include <future>
#include <memory>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "code_coverage.h"
#include "base_grammar.h"
#include "binary_map_grammar.h"
#include "dfa.h"
#include "ea.h"

using namespace std;

shared_ptr<Base_grammar> a_star_b_star() {
	string s = "";
	///*
	for(int i = 1; i < rand() % 100; i++) {
		s += "a";
	}
	for(int i = 1; i < rand() % 100; i++) {
		s += "b";
	}

	shared_ptr<Base_grammar> temp(new Binary_map_grammar(s));

	return temp;
}

shared_ptr<Base_grammar> xml_like() {
	string label = "";
	string internal = "";

	for(int i = 1; i <= (rand() % 10) + 1; i++) {
		label += ('a' + (rand() % 26));
	}

	for(int i = 1; i <= (rand() % 10) + 1; i++) {
		internal += ('a' + (rand() % 26));
	}

	string s = "<" + label + ">" + internal + "</" + label + ">";

	return shared_ptr<Base_grammar>(new Binary_map_grammar(s));
}

int main(int argc, char *argv[]) {
	if(argc != 2) {
		cout << "NOPE!" << endl;
		return 0;
	}

	// Turn off cerr for now
	std::cerr.setstate(std::ios_base::failbit);

	// Initalize random
	srand (time(NULL));

	//shared_ptr<Base_grammar> grammar(new Binary_map_grammar(inputs[0])); // inputs.front()
	//shared_ptr<Base_grammar> other(new Binary_map_grammar(inputs[1]));

	//auto test = grammar;

	//shared_ptr<Base_grammar> apple(new Binary_map_grammar());
	//shared_ptr<Base_grammar> ban(new decltype(*apple)());

	Ea ea(a_star_b_star, argv[1]);

	ea.run();

	//ea.parent_selection();

	//cout << (*grammar) << endl;

	//cout << (*grammar) << endl;

	//grammar -> abstract();

	//cout << (*grammar) << endl;

	//auto test = grammar -> recombination(other);

	//cout << *test.first << endl << endl;

	//cout << *test.second << endl;

	/*
	Dfa d("[a-zA-Z0-9_]+"); 

	for(int i = 0; i < 10; i++) {
		cout << d.generate_string() << endl;
	}
	*/


	/*
	for(int i = 0; i < 100; i++) {
		cout << i << endl;
		grammar -> find_fitness();
	}*/

	/*
	grammar -> mutate();

	cout << endl;
	cout << "-------------" << endl;
	cout << (*grammar) << endl;
	cout << "-------------" << endl;
	grammar -> generate_strings();
	*/
	//cout << (grammar -> find_fitness()) << endl;


	return 0;
}
