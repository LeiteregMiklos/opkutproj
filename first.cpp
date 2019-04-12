//the first implementation
#include "solver.h"

std::list<RekSolver::sol> selectTopK(std::list<RekSolver::sol> l, int k); //select the top k solutions

std::list<RekSolver::sol> RekSolver::rek(subproblem sub)
{
	std::list<sol> l_ret;
	l_ret = fit(sub);
	std::list<int> cc = consideredCuts(sub);
	for (int c : cc)
	{
		bool vertical;
		if (sub.depth % 2 == 0)
		{
			vertical = true;
		}
		else
		{
			vertical = false;
		}
		bool success;
		std::pair<rectangle, rectangle> r1r2 = cutUp(sub, c, vertical, success);
		if (success)
		{
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
	}
	return selectTopK(l_ret, 100);
}

std::pair<RekSolver::rectangle, RekSolver::rectangle> RekSolver::cutUp(subproblem sub, int c, bool vertical, bool &success)
{
	rectangle r1;
	rectangle r2;

	for (Defect def : sub.rect.defs)
	{
		int first, second;
		if (vertical)
		{
			first = def.pos.x;
			second = def.pos.x + def.size.x;
		}
		else
		{
			first = def.pos.y;
			second = def.pos.y + def.size.y;
		}
		if (c >= first && c <= second)
		{
			success = false;
			return std::pair<rectangle, rectangle>();
		}
		if(c>second)
		{
			r1.defs.push_back(def);
		}
		if(c<first)
		{
			r2.defs.push_back(def);
		}
	}
	success = true;

	if (vertical)
	{		
		r1.lb = sub.rect.lb;
		r1.rt.y = sub.rect.rt.y;
		r1.rt.x = c;
		
		r2.lb.y = sub.rect.lb.y;
		r2.lb.x = c;
		r2.rt = sub.rect.rt;
	} else
	{
		r1.lb = sub.rect.lb;
		r1.rt.x = sub.rect.rt.x;
		r1.rt.y = c;
		
		r2.lb.x = sub.rect.lb.x;
		r2.lb.y = c;
		r2.rt = sub.rect.rt;
	}	
	return std::make_pair(r1,r2);
}

//Reksolverben a solverből örökölt változó: stacks
//sub.ss stackstate
//ezek alapján vegyük a következő néhány item hosszait, és az ezekből képezhető számokat
//sub.rect alapján csak a nem túl nagy ilyen számokra van szükség
//subproblem.depth alapján tudjuk hogy függőleges vagy vizszintes vágás jön. --- depth%2
std::list<int> RekSolver::consideredCuts(subproblem sub) //returns the list of cuts
{
	//
}

std::list<RekSolver::sol> selectTopK(std::list<RekSolver::sol> l, int k) //returns the best k solutions from l;
{
	//
}

//felismerni hogy a sub.problem.rect az pont az egyik stacken a következő item
//hibákat figyelembe véve
std::list<RekSolver::sol> RekSolver::fit(subproblem sub)
{
	//
}