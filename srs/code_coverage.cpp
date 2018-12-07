#include "code_coverage.h"

const string Code_coverage::tmp_directory = "/tmp/kcov_temp_code_coverage/";
// Is ".8159829a7e16cfa9" always constant?
const string Code_coverage::kcov_saved_path = "/mipl_parser/coverage.json";

Code_coverage::Code_coverage() {
	valid_outputs = {};
	invalid_outputs = {"Line"};

	// Generate tmp directory
	string dir_command = "mkdir -p " + tmp_directory;
	const int dir_err = system(dir_command.c_str());
	if(dir_err == -1) {
		string s = "Directory " + tmp_directory + " was not created!";
		cerr << s << endl;
		throw s;
	}
}

Code_coverage::Code_coverage(const string& command, const vector<string>& valid, const vector<string>& invalid) {
	command_to_run = command;
	valid_outputs = valid;
	invalid_outputs = invalid;

	// Generate tmp directory
	string dir_command = "mkdir -p " + tmp_directory;
	const int dir_err = system(dir_command.c_str());
	if(dir_err == -1) {
		string s = "Directory " + tmp_directory + " was not created!";
		cerr << s << endl;
		throw s;
	}
}

// NOTE: need to clean up after () finishes
Code_coverage::~Code_coverage() {
	// Delete tmp directory
	string command = "rm -r " + tmp_directory;
	//const int dir_err = system(command.c_str());
	/*
	if(dir_err == -1) {
		throw "Directory " + tmp_directory + " was not deleted!";
	}
	*/
}


float Code_coverage::operator()(const string& input, const int index) const {
	// NOTE: HAVE THIS MORE ABSTRACT //
	string kcov_command = "kcov " + tmp_directory + to_string(index) 
	+ " /home/drc/Desktop/CS5500/HW3/mipl_parser "
	+ tmp_directory + to_string(index) + ".txt";

	kcov_command  = "kcov " + tmp_directory + to_string(index) 
	+ " /home/drc/Desktop/Research/ea/tester_parser/mipl_parser "
	+ tmp_directory + to_string(index) + ".txt";
	///////////////////////////////////

	// Make tmp file for input
	ofstream input_file(tmp_directory + to_string(index) + ".txt");
	if(input_file.is_open()) {
		input_file << input;
		input_file.close();
	} else {
		string s = "File \"" + tmp_directory + to_string(index) + ".txt" + "\" did not open!";
		cerr << s << endl;
		throw s;
	}

	// Make sure no error occured while parsing
	if(valid_input(exec(kcov_command))) {
		// Find code coverage in JSON file
		ifstream infile(tmp_directory + to_string(index) + kcov_saved_path);

		if(infile.is_open()) {
			string line;
			while(getline(infile, line)) {
				// Make sure "file" is not part of line
				if(line.find("file") == std::string::npos) {
					// Find json line containing only "percent_covered" field
					if(line.find("percent_covered") != std::string::npos) {
						infile.close();
						return parse_json_line_containing_code_coverage(line);
					}
				}
			}
			infile.close();
		} else {
			string s = "File \"" + tmp_directory + to_string(index) + kcov_saved_path + "\" did not open!";
			cerr << s << endl;
			throw s;
		}
	}

	return 0.0;
}

float Code_coverage::operator()(const vector<string>& inputs) const {
	float total = 0.0;

	// Async threads
	vector<future<float> > threads;
	for(uint i = 0; i < inputs.size(); i++) {
		try {

			threads.push_back(async((*this), inputs[i], i));
		} catch(string e) {
			cout << e << endl;
		}
	}

	// Join threads, keep track of total
	for(auto& th : threads) {
		total += th.get();
	}

	return total;
}


float Code_coverage::parse_json_line_containing_code_coverage(const string& input) const {
	string coverage = "";
	int par_count = 0;

	for(uint i = 0; i < input.size(); i++) {
		if(input[i] == '\"') {
			par_count++;
		} else if(par_count == 3) {
			coverage += input[i];
		}
	}
	return stof(coverage);
}

std::string Code_coverage::exec(const string& cmd) const {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");

    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }

    return result;
}
// /tmp/thing/mipl_parser.8159829a7e16cfa9/coverage.json:  "percent_covered": "55.30",

// NOTE: not checking valid_outputs
bool Code_coverage::valid_input(const string& input) const {
	for(auto invalid : invalid_outputs) {
		if(input.find(invalid) != std::string::npos) {
			return false;
		}
	}

	return true;
}