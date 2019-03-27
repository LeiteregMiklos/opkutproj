//the first implementation
#include "solver.h"

std::list<RekSolver::sol> selectTopK(std::list<RekSolver::sol> l, int k); //select the top k solutions

std::list<RekSolver::sol> RekSolver::rek(subproblem sub)
{
	std::list<sol> l_ret;
	l_ret=fit(sub);
	std::list<int> cc = consideredCuts(sub);
	for (int c : cc)
	{
		std::pair<rectangle, rectangle> r1r2 = cutUp(sub, c);
		subproblem sub1;
		sub1.rect = r1r2.first;
		sub1.depth = sub.depth + 1;
		sub1.ss = sub.ss;
		sub1.first = true;
		std::list<sol> l1 = rek(sub1);
		for (sol s : l1)
		{
			subproblem sub2;
			sub2.rect = r1r2.second;
			sub2.depth = sub.depth + 1;
			sub2.ss = s.to;
			sub2.first = false;
			std::list<sol> l2 = rek(sub2);
			for (sol s2 : l2)
			{
				sol s_ret;
				s_ret.s = sub;
				s_ret.from = sub.ss;
				s_ret.to = s2.to;
				l_ret.push_back(s_ret);
			}
			if (!sub.first)
			{
				sub2.depth = sub.depth;
				l2 = rek(sub2);
				for (sol s2 : l2)
				{
					sol s_ret;
					s_ret.s = sub;
					s_ret.from = sub.ss;
					s_ret.to = s2.to;
					l_ret.push_back(s_ret);
				}
			}
		}
	}
	return selectTopK(l_ret,100);
}

std::pair<RekSolver::rectangle, RekSolver::rectangle> RekSolver::cutUp(subproblem sub, int c)
{
	//
}

std::list<int> RekSolver::consideredCuts(subproblem sub)
{
	//
}

std::list<RekSolver::sol> selectTopK(std::list<RekSolver::sol> l, int k)
{
	//
}

std::list<RekSolver::sol> RekSolver::fit(subproblem sub)
{
	//
}