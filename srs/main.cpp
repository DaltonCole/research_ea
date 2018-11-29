#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
#include <thread>
#include <future>
#include "code_coverage.h"
#include "base_grammar.h"
#include "binary_map_grammar.h"

using namespace std;

int main() {
	//string "kcov  /tmp/thing /home/drc/Desktop/CS5500/HW3/mipl_parser /home/drc/Desktop/CS5500/HW3/inputs/procCalls_noErrors.txt"
	Code_coverage a("echo dalton", vector<string>(), vector<string>({"Line"}));

	vector<string> list({"comboNoErrors.txt", "procCalls_noErrors.txt",
							"assignmentToArray.txt", "procVarMismatch4.txt"});
	vector<future<float> > threads;

	vector<string> inputs;

	for(int i = 0; i < list.size(); i++) {
		std::ifstream t("/home/drc/Desktop/CS5500/HW3/inputs/" + list[i]);
		std::string str((std::istreambuf_iterator<char>(t)),
		                 std::istreambuf_iterator<char>());

		inputs.push_back(str);
	}

	cout << a(inputs) << endl;;

	return 0;
}
