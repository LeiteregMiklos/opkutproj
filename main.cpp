#include "solver.h"
#include <iostream>

int main()
{
	Solver sol;
	sol.load("A1");
	sol.init();
	std::cout << sol.bins[72].defects[0].id << std::endl;
	for(auto l : sol.stacks)
	{
		for(auto i : l)
		{
			std::cout << i.id << " " << std::endl;
		}
		std::cout << std::endl;
	}
	return 0;
}