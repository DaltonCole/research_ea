#include "code_coverage.h"

// --- Initalize static variables --- //
const string Code_coverage::tmp_directory = "/tmp/kcov_temp_code_coverage/";
string Code_coverage::kcov_saved_path = "";
string Code_coverage::executable = "";
bool Code_coverage::kcov_or_bool = true;
int Code_coverage::directory_index = 0;
std::mutex Code_coverage::mtx;
vector<string> Code_coverage::valid_strings;
vector<string> Code_coverage::invalid_strings;
////////////////////////////////////////


Code_coverage::Code_coverage() {
	// Generate tmp directory
	string dir_command = "mkdir -p " + tmp_directory;
	const int dir_err = system(dir_command.c_str());
	if(dir_err == -1) {
		string s = "Directory " + tmp_directory + " was not created!";
		cerr << s << endl;
		throw s;
	}
}

int Code_coverage::get_index() {
	Code_coverage::mtx.lock();
	int index = directory_index;
	directory_index++;
	Code_coverage::mtx.unlock();

	return index;
}

// Called after all code_coverage runs are finished in ea.cpp
// Removes tmp directories
void Code_coverage::clean_up() {
	// Delete tmp directory
	string command = "rm -r " + tmp_directory + " 2>/dev/null";
	const int dir_err = system(command.c_str());
	
	if(dir_err == -1) {
		throw "Directory " + tmp_directory + " was not deleted!";
	}

	// Reset directory index
	Code_coverage::mtx.lock();
	directory_index = 0;
	Code_coverage::mtx.unlock();
}

float Code_coverage::operator()(const string& input) {
	int index = get_index();
	return (*this)(input, index);
}

float Code_coverage::operator()(const string& input, const int index) {
	string kcov_command = "kcov " + tmp_directory + to_string(index) 
	+ " " + Code_coverage::executable + " "
	+ tmp_directory + to_string(index) + ".txt";

	// Make tmp file for input
	ofstream input_file(tmp_directory + to_string(index) + ".txt");
	if(input_file.is_open()) {
		input_file << input;
		input_file.close();
	} else {
		string s = "File \"" + tmp_directory + to_string(index) + ".txt" + "\" did not open!";
		throw s;
	}

	// Make sure no error occured while parsing
	if(valid_input(exec(kcov_command))) {
		// Find code coverage in JSON file
		ifstream infile(tmp_directory + to_string(index) + "/" + kcov_saved_path + "/coverage.json");

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
			string s = "File \"" + tmp_directory + to_string(index) + "/" + kcov_saved_path + "/coverage.json" + "\" did not open!";
			throw s;
		}
	} else { // Return code_coverage - 100, more code coverage => better, although, still wrong
		// Find code coverage in JSON file
		ifstream infile(tmp_directory + to_string(index) + "/" + kcov_saved_path + "/coverage.json");

		if(infile.is_open()) {
			string line;
			while(getline(infile, line)) {
				// Make sure "file" is not part of line
				if(line.find("file") == std::string::npos) {
					// Find json line containing only "percent_covered" field
					if(line.find("percent_covered") != std::string::npos) {
						infile.close();
						return (parse_json_line_containing_code_coverage(line) - 100);
					}
				}
			}
			infile.close();
		} else {
			string s = "File \"" + tmp_directory + to_string(index) + "/" + kcov_saved_path + "/coverage.json" + "\" did not open!";
			throw s;
		}
	}

	// If invalid input, subtract fitness
	return -100.0;
}



float Code_coverage::operator()(const vector<string>& inputs) {
	float total = 0.0;
	int index = get_index();

	for(uint i = 0; i < inputs.size(); i++) {
		try {
			if(Code_coverage::kcov_or_bool == true) {
				total += (*this)(inputs[i], index);
			} else {
				total += fitness_no_code_coverage(inputs[i], index);
			}
		} catch(string e) {
			cerr << e << endl;
		}
	}

	return total;
}

float Code_coverage::fitness_no_code_coverage(const string& input, const int index) {
	// System command to run
	string command = Code_coverage::executable + " " + tmp_directory + to_string(index) + ".txt";

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
	if(valid_input(exec(command))) {
		return 100.0;
	}

	// If invalid input, subtract 1 from fitness
	return -100.0;
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

bool Code_coverage::valid_input(const string& input) const {
	// If we are looking for valid input
	if(valid_strings.size() > 0) {
		// For each possible valid string
		for(auto valid : valid_strings) {
			// See if string appears in line
			if(input.find(valid) != std::string::npos) {
				return true;
			}
		}
		return false;
	}

	// For each invalid string
	for(auto invalid : invalid_strings) {
		// See if string appears in line
		if(input.find(invalid) != std::string::npos) {
			return false;
		}
	}

	return true;
}