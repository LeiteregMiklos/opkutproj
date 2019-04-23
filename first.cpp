//the first implementation
#include "solver.h"
#include <algorithm>
#include <list>

std::vector<RekSolver::sol> RekSolver::rek(const subproblem& sub)
{
	std::vector<sol> l_ret;
	if(finished(sub.ss)){l_ret.push_back(sol(sub,sub.ss)); return l_ret;} 
	if (sub.begin == -1)
	{
		if (sub.depth > 3)
		{
			return l_ret;
		}
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
		std::list<int> cc = consideredCuts(sub, vertical);
		for (int c : cc)
		{
			bool success;
			std::pair<rectangle, rectangle> r1r2 = cutUp(sub, c, vertical, success);
			if (success)
			{
r1r2.first.lb.print(); r1r2.first.rt.print();
				subproblem sub1(r1r2.first,sub.ss,sub.depth + 1,-1);
				std::vector<sol> l1 = rek(sub1);
				for (sol s1 : l1)
				{
					subproblem sub2(r1r2.second,s1.to,sub.depth + 1,-1);
					std::vector<sol> l2 = rek(sub2);
					for (sol s2 : l2)
					{
						sol s_ret(sub,sub.ss,s2.to,c,s1.b,s2.b); //s1,s2
						l_ret.push_back(s_ret);
					}
					/* sub2.depth = sub.depth; //the/top right side of a cut can be followed by a cut at the same level
					l2 = rek(sub2);
					for (sol s2 : l2)
					{
						sol s_ret(sub,sub.ss,s2.to,c,s1.b,s2.b);
						l_ret.push_back(s_ret);
					} */
				}
			}
		}
	}
	if (sub.begin >= 0)
	{
		rectangle rect_(Coord(0,0),area,this->bins[sub.begin].defects);
		subproblem sub1(rect_,sub.ss,0,-1);
		std::vector<sol> l1=rek(sub1);
		/* for(sol s1 : l1)
		{
			subproblem sub2(s1.to,sub.begin+1);
			std::vector<sol> l2 = rek(sub2);
			for(sol s2 : l2)
			{
				sol s_ret(sub,sub.ss,s2.to,-2,s1.b,s2.b);
				l_ret.push_back(s_ret);
			}
		} */
	}

	selectTopK(l_ret, 10);
	if(sub.begin == 0){selectTopK(l_ret, 1);}
	return l_ret;
}

std::pair<RekSolver::rectangle, RekSolver::rectangle> RekSolver::cutUp(const subproblem& sub, int c, bool vertical, bool &success)
{
	std::vector<Defect> defs1;
	std::vector<Defect> defs2;
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
		if (c > second)
		{
			defs1.push_back(def);
		}
		if (c < first)
		{
			defs2.push_back(def);
		}
	}
	success = true;
	Coord lb1,rt1,lb2,rt2;
	if (vertical)
	{
		lb1 = sub.rect.lb;
		rt1 = Coord(c,sub.rect.rt.y);

		lb2 = Coord(c,sub.rect.lb.y);
		rt2 = sub.rect.rt;
	}
	else
	{
		lb1 = sub.rect.lb;
		rt1 = Coord(sub.rect.rt.x,c);

		lb2 = Coord(sub.rect.lb.x,c);
		rt2 = sub.rect.rt;
	}
	rectangle r1(lb1,rt1,defs1);
	rectangle r2(lb2,rt2,defs2);
	return std::make_pair(r1, r2);
}

std::list<int> sums(std::vector<int> inp, int k) //possible sums form inp
{
	std::list<int> ret;
	ret.push_back(0);
	for (int i : inp)
	{
		std::list<int> ret_;
		for (int s : ret)
		{
			int ss = s + i;
			if (ss < k)
			{
				ret_.push_back(ss);
			}
		}
		ret.merge(ret_);
	}
	ret.pop_front();
	return ret;
}

//Reksolverben a solverből örökölt változó: stacks
//sub.ss stackstate
//ezek alapján vegyük a következő néhány item hosszait, és az ezekből képezhető számokat
//sub.rect alapján csak a nem túl nagy ilyen számokra van szükség
//subproblem.depth alapján tudjuk hogy függőleges vagy vizszintes vágás jön. --- depth%2
std::list<int> RekSolver::consideredCuts(const subproblem& sub, bool vertical) //returns the list of cuts
{
	int nn = 2; //number of objects considered on in stack
	std::vector<int> lens;
	for (auto items : this->stacks)
	{
		for (int i = 0; i < nn; i++)
		{
			lens.push_back(items[i].size.x);
			lens.push_back(items[i].size.y);
		}
	}
	int k;
	if (vertical)
	{
		k = sub.rect.rt.x - sub.rect.lb.x;
	}
	else
	{
		k = sub.rect.rt.y - sub.rect.lb.y;
	}
	return sums(lens, k);
}

class comparator
{
public:
	std::vector<std::vector<Solver::Item>>* stacks;
	comparator(std::vector<std::vector<Solver::Item>>* stacks)
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
				ar1+=((*stacks)[s][i]).size.area();
			}
			for(int i=rhs.from[s];i<=rhs.to[s];i++)
			{
				ar2+=((*stacks)[s][i]).size.area();
			}
		}
		return ((float)ar1/(lhs.s.rect.rt-lhs.s.rect.lb).area())>((float)ar2/(rhs.s.rect.rt-rhs.s.rect.lb).area());
	}
	private:
};

void RekSolver::selectTopK(std::vector<RekSolver::sol> &v, int k) //returns the best k solutions from l;
{
	//std::sort(v.begin(), v.end(), std::bind(comp, std::placeholders::_1, std::placeholders::_2, this->stacks));
	std::sort(v.begin(), v.end(), comparator(&(this->stacks)) );
	if((int)v.size()>k){
		v.erase(v.begin()+k,v.end());
	}
}

bool RekSolver::finished(const std::vector<int>& ss)
{
//std::cout<<"ss"<<ss.size()<<"stacks"<<stacks.size()<<std::endl;
	for(int s=0;s<(int)stacks.size();s++)
	{
		if((int)stacks[s].size()>ss[s]){return false;}
	}
	return true;
}

//felismerni hogy a sub.problem.rect az pont az egyik stacken a következő item
//hibákat figyelembe véve
std::vector<RekSolver::sol> RekSolver::fit(const subproblem& sub)
{
	std::vector<sol> ret;
	ret.push_back(sol(sub,sub.ss));
	if(sub.rect.defs.size() != 0)
	{
		return ret;
	}
	sol temp(sub,sub.ss);
	for (int s = 0; s < (int)this->stacks.size(); s++)
	{
		if( ((this->stacks)[s][sub.ss[s]]).size.fits(sub.rect.rt - sub.rect.lb))
		{
			sol _t(temp);
			_t.to[s]++;
			_t.b->itemid=((this->stacks)[s][sub.ss[s]]).id;
			ret.push_back(_t);
		}
	}
	return ret;
}