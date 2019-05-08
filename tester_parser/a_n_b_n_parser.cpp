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
		cout << "Accept" << endl;
		return 0;
	}

	bool at_b = false;
	int count = 0;
	for(const auto& c : all) {
		if(c == 'a' || c == 'b') {
			if(at_b == true && c == 'a') {
				cout << "Invalid Line" << endl;
				return 0;
			}
			if(c == 'a') {
				count++;
			}
			if(c == 'b') {
				at_b = true;
				count--;
			}
		} else {
			cout << "Invalid Line" << endl;
			return 0;
		}
	}

	if(count != 0) {
		cout << "Invalid Line" << endl;
		return 0;
	}

	cout << "Accept" << endl;

	return 0;
}
