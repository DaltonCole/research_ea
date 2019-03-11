#include <iostream>
#include <fstream>

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

	if(all == "") {
		cout << "Invalid Line" << endl;
		return 0;
	}

	// At least 5 char long
	if(all.size() < 5) {
		cout << "Invalid Line" << endl;
		return 0;
	}

	try {
		// Check < and ending > tag
		if(all[0] != '<' || all[all.size() - 1] != '>') {
			cout << "Invalid Line" << endl;
			return 0;
		}

		int index = 1;
		// Parse to first closing brace
		for(; index < all.size(); index++) {
			if(all[index] == '>') {
				break;
			} else if(all[index] < 'a' || all[index] > 'z') {
				cout << "Invalid Line" << endl;
				return 0;
			}
		}

		if(all[index] != '>' || index == 1) {
			cout << "Invalid Line" << endl;
			return 0;
		}


		index++;
		// Parse to second opening brace
		for(; index < all.size(); index++) {
			if(all[index] == '<') {
				break;
			} else if(all[index] < 'a' || all[index] > 'z') {
				cout << "Invalid Line" << endl;
				return 0;
			}
		}

		if(all[index] != '<') {
			cout << "Invalid Line" << endl;
			return 0;
		}
		index++;

		if(all[index] != '/') {
			cout << "Invalid Line" << endl;
			return 0;
		}

		index++;

		// Parse to first closing brace
		for(; index < all.size(); index++) {
			if(all[index] == '>') {
				break;
			} else if(all[index] < 'a' || all[index] > 'z') {
				cout << "Invalid Line" << endl;
				return 0;
			}
		}

		cout << "Accept" << endl;
	} catch(...) {
		cout << "Invalid Line" << endl;
	}

	return 0;
}
