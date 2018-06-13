#include "benchmark.h"
#include <cpprelude/Dynamic_Array.h>
#include <cpprelude/Double_List.h>
#include <cpprelude/Benchmark.h>
#include <cpprelude/Allocators.h>
#include <cpprelude/Single_List.h>
using namespace cppr;

struct Node_A
{
	enum TYPE { BYTE, SPLT, HALT };

	TYPE type;
	byte data;
	usize branches[2];
};

struct NFA_A
{
	Dynamic_Array<Node_A> nodes;
	Dynamic_Array<usize> dangling;
	usize start;
};

struct Node_B
{
	enum TYPE { BYTE, SPLT, HALT };

	TYPE type;
	byte data;
	Node_B* branches[2];
};

struct NFA_B
{
	Node_B* start;
	Dynamic_Array<Node_B*> dangling;

	NFA_B(const Memory_Context& context = os->global_memory)
		:dangling(context)
	{}
};

void
cool_stuff()
{
	int r = rand() % 10;

	{
		Arena_Allocator node_arena(os->virtual_memory);

		NFA_B nfa(node_arena);

		Stopwatch watch;
		watch.start();
		Node_B* n = nfa.start, *p = nullptr;
		for(usize i = 0 ; i < 5000000; ++i)
		{
			n = node_arena.alloc<Node_B>().ptr;
			if(p != nullptr)
				p->branches[0] = n;
			if(i % r == 0)
				nfa.dangling.insert_back(n);
			p = n;
		}
		for(auto n: nfa.dangling)
			n->branches[0] = nullptr;
		watch.stop();
		println(watch.milliseconds(), "ms");
	}

	{
		Stopwatch watch;
		NFA_A nfa;
		watch.reset();
		usize p2 = -1;
		for(usize i = 0 ; i < 5000000; ++i)
		{
			nfa.nodes.insert_back(Node_A{});
			if(p2 != -1)
				nfa.nodes[p2].branches[0] = nfa.nodes.count() - 1;
			if(i % r == 0)
				nfa.dangling.insert_back(nfa.nodes.count() - 1);
			p2 = nfa.nodes.count() - 1;
		}
	
		for(auto i: nfa.dangling)
			nfa.nodes[i].branches[0] = 0;
		watch.stop();

		println(watch.milliseconds(), "ms");
	}
}

int
main()
{
	//benchmark();
	cool_stuff();
	return 0;
}