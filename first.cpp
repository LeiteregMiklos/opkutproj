//the first implementation
#include "solver.h"
#include <algorithm>
#include <list>
#include <functional>

std::vector<RekSolver::sol> RekSolver::rek(subproblem sub)
{
	std::vector<sol> l_ret;
	if(sub.depth>3){return l_ret;}
	l_ret = fit(sub);
	bool vertical;
	if (sub.depth % 2 == 0)
	{
		vertical = true;
	}
	else
	{
		vertical = false;
	}
	std::list<int> cc = consideredCuts(sub,vertical);
	for (int c : cc)
	{
		
		bool success;
		std::pair<rectangle, rectangle> r1r2 = cutUp(sub, c, vertical, success);
		if (success)
		{
			subproblem sub1;
			sub1.rect = r1r2.first;
			sub1.depth = sub.depth + 1;
			sub1.ss = sub.ss;
			sub1.first = true;
			std::vector<sol> l1 = rek(sub1);
			for (sol s : l1)
			{
				subproblem sub2;
				sub2.rect = r1r2.second;
				sub2.depth = sub.depth + 1;
				sub2.ss = s.to;
				sub2.first = false;
				std::vector<sol> l2 = rek(sub2);
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
	selectTopK(l_ret, 100);
	return l_ret;
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

std::list<int> sums(std::vector<int> inp, int k)//possible sums form inp
{
	std::list<int> ret;
	ret.push_back(0);
	for(int i : inp)
	{
		for(int s : ret)
		{
			std::list<int> ret_;
			int ss=s+i;
			if(ss<k)
			{
				ret_.push_back(ss);
			}
			ret.merge(ret_);
		}
	}
	return ret;
}

//Reksolverben a solverből örökölt változó: stacks
//sub.ss stackstate
//ezek alapján vegyük a következő néhány item hosszait, és az ezekből képezhető számokat
//sub.rect alapján csak a nem túl nagy ilyen számokra van szükség
//subproblem.depth alapján tudjuk hogy függőleges vagy vizszintes vágás jön. --- depth%2
std::list<int> RekSolver::consideredCuts(subproblem sub, bool vertical) //returns the list of cuts
{
	int nn=5; //number of objects considered on in stack
	std::vector<int> lens;
	for(auto items : this->stacks)
	{
		for(int i=0;i<nn;i++)
		{
			lens.push_back(items[i].size.x);
			lens.push_back(items[i].size.y);
		}
	}
	int k;
	if(vertical){k=sub.rect.rt.x-sub.rect.lb.x;} else {k=sub.rect.rt.y-sub.rect.lb.y;}
	return sums(lens,k);
}

bool comp(RekSolver::sol& lhs, RekSolver::sol& rhs,std::vector<std::vector<Solver::Item>>& stacks)
{
	int ar1=0;
	int ar2=0;
	for(int s=0;s<(int)stacks.size();s++)
	{
		for(int i=lhs.from[s];i<=lhs.to[s];i++)
		{
			ar1+=stacks[s][i].size.area();
		}
		for(int i=rhs.from[s];i<=rhs.to[s];i++)
		{
			ar2+=stacks[s][i].size.area();
		}
	}
	return ((float)ar1/(lhs.s.rect.rt-lhs.s.rect.lb).area())>((float)ar2/(rhs.s.rect.rt-rhs.s.rect.lb).area());
}

/* class compare
{
public:
	std::vector<std::vector<Solver::Item>>* stacks;
	compare(std::vector<std::vector<Solver::Item>>* stacks)
	{
		this->stacks=stacks;
	}
	bool operator()(RekSolver::sol& lhs, RekSolver::sol& rhs)
	{
		int ar1=0;
		int ar2=0;
		for(int s=0;s<(int)stacks->size();s++)
		{
			for(int i=lhs.from[s];i<=lhs.to[s];i++)
			{
				ar1+=*stacks[s][i].size.area();
			}
			for(int i=rhs.from[s];i<=rhs.to[s];i++)
			{
				ar2+=*stacks[s][i].size.area();
			}
		}
		return ((float)ar1/(lhs.s.rect.rt-lhs.s.rect.lb).area())>((float)ar2/(rhs.s.rect.rt-rhs.s.rect.lb).area());
	}
	private:
}; */


void RekSolver::selectTopK(std::vector<RekSolver::sol> &v, int k) //returns the best k solutions from l;
{
	std::sort(v.begin(),v.end(),std::bind(comp,std::placeholders::_1,std::placeholders::_2,this->stacks));
	v.resize(k);
}

//felismerni hogy a sub.problem.rect az pont az egyik stacken a következő item
//hibákat figyelembe véve
std::vector<RekSolver::sol> RekSolver::fit(subproblem sub)
{	
	std::vector<sol> ret;
	if(sub.rect.defs.size()!=0){return ret;}
	sol temp;
	temp.s=sub;
	temp.from=sub.ss;
	temp.to=sub.ss;
	for(int s=0;s<(int)this->stacks.size();s++){temp.to[s]--;}
	for(int s=0;s<(int)this->stacks.size();s++)
	{
		if(this->stacks[s][sub.ss[s]].size.fit(sub.rect.rt-sub.rect.lb))
		{
			sol _t=temp;
			_t.to[s]++;
			ret.push_back(_t);
		}
	}
	return ret;
}