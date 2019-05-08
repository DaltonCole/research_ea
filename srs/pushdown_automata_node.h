#ifndef PUSHDOWN_AUTOMATA_NODE
#define PUSHDOWN_AUTOMATA_NODE

#include <memory>
#include <unordered_set>

class Pushdown_automata_node;

// Edge
class Pushdown_automata_edge {
	public:
		Pushdown_automata_edge() {};

		Pushdown_automata_edge(shared_ptr<Pushdown_automata_node>& s, 
			shared_ptr<Pushdown_automata_node>& e, unique_ptr<char>& r_i, 
			unique_ptr<uint32_t>& r_s, unique_ptr<uint32_t>& p_s):
			starting_node(s), ending_node(e), read_input_symbol(new char(*r_i)),
			read_stack_symbol(new uint32_t(*r_s)), pushed_stack_symbol(new uint32_t(*p_s)) {}

		Pushdown_automata_edge(const shared_ptr<Pushdown_automata_node>& s, 
			const shared_ptr<Pushdown_automata_node>& e, char r_i, uint32_t r_s, uint32_t p_s):
			starting_node(s), ending_node(e) {
				read_input_symbol = make_unique<char>(r_i);
				read_stack_symbol = make_unique<uint32_t>(r_s);
				pushed_stack_symbol = make_unique<uint32_t>(p_s);
		}

		bool operator ==(const Pushdown_automata_edge& rhs) const {
			if(starting_node == rhs.starting_node
				&& ending_node == rhs.ending_node
				&& *read_input_symbol == *(rhs.read_input_symbol)
				&& *read_stack_symbol == *(rhs.read_stack_symbol)
				&& *pushed_stack_symbol == *(pushed_stack_symbol)) {
				return true;
			}
			return false;
		}

		shared_ptr<Pushdown_automata_node> starting_node;
		shared_ptr<Pushdown_automata_node> ending_node;

		unique_ptr<char> read_input_symbol;
		unique_ptr<uint32_t> read_stack_symbol;
		unique_ptr<uint32_t> pushed_stack_symbol;
};

namespace std {
	// Forces the pushdown automata to be "deterministic"
	//	If start and end node hashes aren't used, otherwise, still non-deterministic
	template<>
	struct hash<Pushdown_automata_edge> {
		size_t operator()(const Pushdown_automata_edge& edge) const {
			return 
				hash<char>()(*(edge.read_input_symbol))
				^ hash<uint32_t>()(*(edge.read_stack_symbol))
				^ hash<uint32_t>()(*(edge.pushed_stack_symbol))
				^ hash<shared_ptr<Pushdown_automata_node> >()(edge.starting_node) 
				^ hash<shared_ptr<Pushdown_automata_node> >()(edge.ending_node);
		}
	};
}


// Node
class Pushdown_automata_node {
	public:
		unordered_set<shared_ptr<Pushdown_automata_edge> > out_going_edges;
		bool accept_node;
};

#endif