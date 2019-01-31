#ifndef CODE_COVERAGE_H
#define CODE_COVERAGE_H

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <future>
#include <mutex>
using namespace std;

class Code_coverage {
	public:
		Code_coverage();
		Code_coverage(const string& command, const vector<string>& valid, const vector<string>& invalid);
		~Code_coverage();

		// Accept word, output percent of code covered
		float operator()(const string& input);

		// Call () on a vector of inputs
		float operator()(const vector<string>& inputs);

		// Make sure input consists of valid strings and no invalid strings
		bool valid_input(const string& input) const;

		// Parse json file's correct line from () and returns coverage's float
		float parse_json_line_containing_code_coverage(const string& input) const;

		// Get temp directory index
		int get_index();
		static void clean_up();

	private:
		string command_to_run;
		vector<string> valid_outputs;
		vector<string> invalid_outputs;

		string exec(const string& cmd) const;

		static const string tmp_directory;
		static const string kcov_saved_path;

		static int directory_index;
		static std::mutex mtx;
};

#endif