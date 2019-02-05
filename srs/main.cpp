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

int main(int argc, char *argv[]) {
	if(argc != 2) {
		cout << "NOPE!" << endl;
		return 0;
	}

	// Initalize random
	srand (time(NULL));
	//string "kcov  /tmp/thing /home/drc/Desktop/CS5500/HW3/mipl_parser /home/drc/Desktop/CS5500/HW3/inputs/procCalls_noErrors.txt"
	/*
	Code_coverage a("echo dalton", vector<string>(), vector<string>({"Line"}));

	vector<string> list({"comboNoErrors.txt", "procCalls_noErrors.txt",
							"assignmentToArray.txt", "procVarMismatch4.txt"});

	vector<string> inputs;

	for(uint i = 0; i < list.size(); i++) {
		std::ifstream t("/home/drc/Desktop/CS5500/HW3/inputs/" + list[i]);
		std::string str((std::istreambuf_iterator<char>(t)),
		                 std::istreambuf_iterator<char>());

		inputs.push_back(str);
	}

	//cout << a(inputs) << endl;
	*/

	//vector<string> list({"1", "2", "3", "4"});

	vector<string> inputs;

	/*
	for(uint i = 0; i < list.size(); i++) {
		std::ifstream t("/home/drc/Desktop/Research/ea/tester_parser/" + list[i]);
		string str;
		if(t.is_open()) {
			str = string((std::istreambuf_iterator<char>(t)),
					std::istreambuf_iterator<char>());
		} else {
			cout << "Error opening file " << list[i] << endl;
		}

		inputs.push_back(str);
	}
	*/

	while(inputs.size() < 100) {
		string s = "";
		///*
		for(int i = 0; i < rand() % 100; i++) {
			s += "a";
		}
		for(int i = 0; i < rand() % 100; i++) {
			s += "b";
		}
		//*/
		/*
		// aaabbb
		for(int i = 0; i < 3; i++) {
			s += "a";
		}
		for(int i = 0; i < 3; i++) {
			s += "b";
		}
		*/

		inputs.push_back(s);
	}

	//shared_ptr<Base_grammar> grammar(new Binary_map_grammar(inputs[0])); // inputs.front()
	//shared_ptr<Base_grammar> other(new Binary_map_grammar(inputs[1]));

	//auto test = grammar;

	//shared_ptr<Base_grammar> apple(new Binary_map_grammar());
	//shared_ptr<Base_grammar> ban(new decltype(*apple)());

	vector<shared_ptr<Base_grammar> > population;
	for(auto& i : inputs) {
		population.emplace_back(new Binary_map_grammar(i));
	}

	//try {
		Ea ea(population, argv[1]);

		ea.run();
	//} catch(...) {
	//	cout << "Error occured" << endl;
	//}

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
