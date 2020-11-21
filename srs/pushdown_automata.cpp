include "pushdown_automata.h"

uint Pushdown_automata::max_failed_attempts = 100;

Pushdown_automata::Pushdown_automata() {
	// Generate start node
	start_node = make_shared<Pushdown_automata_node> ();
	nodes.insert(start_node);

	// Start node is an accept node
	start_node -> accept_node = true;
}

Pushdown_automata::Pushdown_automata(const string& input) {
	// Generate start node
	start_node = make_shared<Pushdown_automata_node> ();
	nodes.insert(start_node);

	// If empty string, start node is an accept node
	if(input.size() == 0) {
		start_node -> accept_node = true;
	} else {
		auto previous_node = start_node;

		for(const auto& c : input) {
			// Generate new node
			auto next_node = make_shared<Pushdown_automata_node> ();

			// Make unique pointers to generate edge (stupid that this has to be done)
			unique_ptr<char> c_ptr = make_unique<char>(c);
			unique_ptr<uint32_t> n_ptr = nullptr;

			// Add edge between previous node and next node
			previous_node -> out_going_edges.emplace(make_shared<Pushdown_automata_edge>(
				previous_node, next_node, c_ptr, n_ptr, n_ptr));

			// New node becomes previous node
			previous_node = next_node;

			// Add character to input alphabet
			input_alphabet.emplace(string(1, c), false);
		}
		// Mark end of input as accept node
		previous_node -> accept_node = true;
	}
}

pair<string, bool> Pushdown_automata::generate_string() {
	// Keep track of max length string generated so far in case we fail at generating a longer one
	string longest_string =  "";
	bool generated_valid_string = false;

	// Number of failed attempts in generating a string
	uint failed_attempts = 0;

	// While we still have failed attempts
	while(failed_attempts < max_failed_attempts) {
		// Set current node to start
		auto current_node = start_node;
		// Initialize stack
		stack<uint32_t> string_stack;
		// Current string to build upon
		string str = "";

		while(true) {
			// Pointer to top of stack
			uint32_t* top_of_stack = nullptr;
			if(string_stack.empty() == false) {
				top_of_stack = &string_stack.top();
			}

			// Vector of possible valid edges to traverse
			auto possible_node_edges = possible_next_node_edges(current_node, top_of_stack);

			// If no possible edges and in an end state, quit
			if(possible_node_edges.size() == 0 && current_node -> accept_node == true) {
				failed_attempts = max_failed_attempts + 1;
				break;
			}
			// If no possible edges, failed attempt
			else if(possible_node_edges.size() == 0) {
				failed_attempts++;
				break;
			}
			// Select a random edge to go down
			auto choosen_edge = possible_node_edges[rand()% possible_node_edges.size()];
			// Add read_char to str
			str += *(choosen_edge -> read_input_symbol);
			// Pop top of stack if necessary
			if(choosen_edge -> read_stack_symbol != nullptr) {
				string_stack.pop();
			}
			// Push stack symbol
			if(choosen_edge -> pushed_stack_symbol != nullptr) {
				string_stack.push(*(choosen_edge -> pushed_stack_symbol));
			}

			// Set current node to next node
			current_node = choosen_edge -> ending_node;

			// If valid end state and str is longer than previous longest found string
			if(current_node -> accept_node && str > longest_string) {
				longest_string = str;
				generated_valid_string = true;
			}
		}
	}

	return make_pair(longest_string, generated_valid_string);
}

vector<shared_ptr<Pushdown_automata_edge> > Pushdown_automata::possible_next_node_edges(
	const shared_ptr<Pushdown_automata_node>& current_node, const uint32_t* stack_top) {
	// Vector of possible next nodes
	vector<shared_ptr<Pushdown_automata_edge> > possible_nodes;

	// For each edge
	for(const auto& edge : current_node -> out_going_edges) {
		// If edge matches stack symbol
		if(*(edge -> read_stack_symbol) == *stack_top) {
			// Add end node to possible nodes
			possible_nodes.push_back(edge);
		}
	}

	return possible_nodes;
}

/*
		string generate_string();

		vector<string> generate_strings(uint number_of_strings);

		shared_ptr<Pushdown_automata> clone();

	private:
		shared_ptr<Pushdown_automata_node> start_node;
		unordered_set<shared_ptr<Pushdown_automata_node> > nodes;

		// NOTE: I'll need to add to Dfa to see if read-in string is accepted by Dfa
		vector<Dfa> input_alphabet;
		vector<uint32_t> stack_alphabet;


};

// Edge
class Pushdown_automata_edge {
	public:
		shared_ptr<Pushdown_automata_node> starting_node;
		shared_ptr<Pushdown_automata_node> ending_node;

		char read_input_symbol;
		char read_stack_symbol;
		char pushed_stack_symbol;
};



// Node
class Pushdown_automata_node {
	public:
		unordered_set<Pushdown_automata_edge> out_going_edges;
		bool accept_node;
};

#endif


struct Pushdown_automata_edge {
	public:
		Pushdown_automata_node* starting_node;
		Pushdown_automata_node* ending_node;

		char read_input_symbol;
		char read_stack_symbol;
		char pushed_stack_symbol;
};

class Pushdown_automata_node {
	public:
		unodered_set<Pushdown_automata_edge> out_going_edges;
};

namespace std {
	// Forces the pushdown automata to be deterministic
	template<>
	struct hash<Pushdown_automata_edge> {
		size_t operator()(const Pushdown_automata_edge& edge) const {
			return hash<string>()(
				edge.read_input_symbol +
				edge.read_stack_symbol +
				edge.pushed_stack_symbol
				);
		}
	};
}

#endif

class Pushdown_automata {
	public:

		Pushdown_automata();

		Pushdown_automata(string input);

		string generate_string();

		vector<string> generate_strings(uint number_of_strings);

		shared_ptr<Pushdown_automata> clone();

	private:
		shared_ptr<Pushdown_automata_node> start_node;
		vector<shared_ptr<Pushdown_automata_node> > nodes;

		// NOTE: I'll need to add to Dfa to see if read-in string is accepted by Dfa
		vector<Dfa> input_alphabet;
		vector<uint32_t> stack_alphabet;


};

#endif

*/