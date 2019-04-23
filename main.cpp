#include "solver.h"
#include <iostream>

int main()
{
	RekSolver sol;
	sol.load("A1");
	sol.init();
	sol.solve();
	
	return 0;
}