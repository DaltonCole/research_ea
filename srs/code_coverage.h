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
		//////////////////////////////////////////////////////////////////////
		/// @brief	Default constructor. Creates "/tmp/" directory for kcov.
		/// @post 	Temporary directories are created to save kcov information
		/// 		to.
		//////////////////////////////////////////////////////////////////////
		Code_coverage();

		//////////////////////////////////////////////////////////////////////
		/// @brief	Returns the code coverage used by (*this)(input, index).
		/// 		Finds the next available index automatically. See
		/// 		"operator()(const string& input, const int index)"
		/// 		for full details.
		/// @param[in]	input 	The input string to test code coverage on
		/// @return	Code coverage as found by kcov. Code coverage
		/// 		is a percentage out of 100. If invalid "input", then
		/// 		-100 is returned.
		//////////////////////////////////////////////////////////////////////
		float operator()(const string& input);

		//////////////////////////////////////////////////////////////////////
		/// @brief	Returns the code coverage as found by kcov
		/// @pre	No other thread or process is using the same "index"
		/// 		kcov temp directory as the one supplied.
		/// @post	A directory under "tmp_directory"/index is created.
		/// 		Will need to be cleaned up with "Code_coverage::clean_up()"
		/// @param[in]	input 	Input string to test code coverage on
		/// @param[in] 	index 	Index to create directory under "tmp_directory"
		/// 		on.
		/// @return	Code coverage as found by kcov. Code coverage
		/// 		is a percentage out of 100. If invalid "input", then
		/// 		-100 is returned.
		//////////////////////////////////////////////////////////////////////
		float operator()(const string& input, const int index);

		//////////////////////////////////////////////////////////////////////
		/// @brief	Returns the code coverage as found by kcov as a sum of
		/// 		each sample string provided. If invalid "input", then
		/// 		-100 is returned. If Code_coverage::kcov_or_bool
		/// 		is set to false, then either 100 or -100 will be added to
		/// 		the sum, for valid and invalid string, respectively. 
		/// @post	Directories under "tmp_directory" are created.
		/// 		Will need to be cleaned up with "Code_coverage::clean_up()"
		/// @param[in]	inputs 	Vector of input strings to calculate code
		/// 		coverage on.
		/// @return	Code coverage. Individual code coverage
		/// 		 is a percentage out of 100.
		//////////////////////////////////////////////////////////////////////
		float operator()(const vector<string>& inputs);

		//////////////////////////////////////////////////////////////////////
		/// @brief	Returns 100.0 if "input" is valid, -100.0 otherwise.
		/// @param[in] 	input 	Input string to test code on.
		/// @param[in] 	index 	Index to create directory under "tmp_directory"
		/// 		on.
		/// @return	Returns 100.0 if "input" is valid, -100.0 otherwise.
		//////////////////////////////////////////////////////////////////////
		float fitness_no_code_coverage(const string& input, const int index);

		//////////////////////////////////////////////////////////////////////
		/// @brief	Cleans up the temporary directories created. Resets
		/// 		"directory_index" to 0.
		/// @post	None of the temp directories created will exist.
		//////////////////////////////////////////////////////////////////////
		static void clean_up();

		static string executable;	///< The executable to test code coverage on
		static bool kcov_or_bool;	///< true = use kcov code coverage;
									///< false = use valid vs invalid (1, -1) * 100;
		static string kcov_saved_path;	///< Directory kcov saves code coverage
										///< information to.
		static vector<string> valid_strings;	///< All strings that represent
												///< valid input.
		static vector<string> invalid_strings;	///< All strings that represent
												///< invalid input.

	private:
		//////////////////////////////////////////////////////////////////////
		/// @brief	Check to see if program input was valid based on the output
		/// 		provided. Uses "valid_strings" and "invalid_strings". If
		/// 		"valid_strings" is not empty, then this will check to see 
		/// 		if any valid strings exist in the input. If they do, true
		/// 		is returned, false otherwise. If "valid_strings" is empty
		/// 		then "invalid_strings" is used. If any strings from
		/// 		"invalid_strings" is found in "input", then false is
		///			returned, true otherwise.
		/// @param[in] 	input 	The output from the cmd command.
		/// @return	Bool value as to whether valid or invalid strings were
		/// 		found in the input.
		//////////////////////////////////////////////////////////////////////
		bool valid_input(const string& input) const;

		//////////////////////////////////////////////////////////////////////
		/// @brief	Parse json file's correct line from () and returns 
		/// 		coverage's float as a percentage out of 100.
		/// @param[in]	input 	A line if json input that contains the code
		/// 		coverage.
		/// @return	A float of the code coverage out of 100 percent.
		//////////////////////////////////////////////////////////////////////
		float parse_json_line_containing_code_coverage(const string& input) const;

		//////////////////////////////////////////////////////////////////////
		/// @brief	Get the index of the next available directory to use for
		/// 		kcov data saving. To reset count, call 
		///			"Code_coverage::clean_up()".
		/// @post	"directory_index" is incremented by 1
		/// @return	The next avaiable "directory_index"
		//////////////////////////////////////////////////////////////////////
		int get_index();

		//////////////////////////////////////////////////////////////////////
		/// @brief	Runs the specified command and returns output from command.
		/// @param[in] 	cmd 	Command line command to run
		/// @return	Output of command.
		//////////////////////////////////////////////////////////////////////
		string exec(const string& cmd) const;

		static const string tmp_directory;	///< Temp directory to save
											///< information to.

		static int directory_index;	///< Directory index to save current code
									///< coverage information to
		static std::mutex mtx;	///< Mutex to make sure each "directory_index"
								///< is unique.
};

#endif