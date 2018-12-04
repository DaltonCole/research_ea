#include <iostream>
#include <regex>
#include <fstream>
#include <string>
using namespace std;

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("You must specify a file in the command line!\n");
		exit(1);
	}

	string line;
	string all = "";
	ifstream myfile(argv[1]);
	if(myfile.is_open()) {
		while(getline(myfile, line)) {
			all += line;
		}
	}


	if(regex_match(all, regex("a*b*"))) {
		cout << "Accept" << endl;
	} else {
		cout << "Line" << endl;
	}

	return 0;
}
