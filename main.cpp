#include "solver.h"
#include <iostream>

int main()
{
	Solver sol;
	sol.load("A1");
	std::cout << sol.bins[72].defects[0].id << std::endl;
	return 0;
}