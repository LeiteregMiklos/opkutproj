//the first implementation
#include "solver.h"
#include <algorithm>
#include <list>

template<class T> //T=RekSolver::sol or fsub
void trim(std::vector<T>& v);

std::vector<RekSolver::sol> RekSolver::rek(const subproblem& sub)
{
	std::vector<sol> l_ret;
	if(finished(sub.ss)){l_ret.push_back(sol(sub,sub.ss)); return l_ret;} 
	if (sub.begin == -1)
	{
sub.rect.lb.print(); sub.rect.rt.print(); std::cout<<std::endl;
for(int x : sub.ss){std::cout<<x<<" ";} std::cout<<std::endl;
std::cout<<sub.depth<<std::endl;
		l_ret = fit(sub);
std::cout<<sub.ss[0]<<"->"<<l_ret[0].to[0]<<std::endl;
		if (sub.depth == 3)
		{
			return l_ret;
		}
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
		if(cc.size()==0){return l_ret;}
for (int c : cc){std::cout<<c<<" ";}
std::cout<<std::endl;
		std::vector<fsub> v;
		int min=10000;
		rectangle minr;
		for (int c : cc)
		{
			bool success;
			std::pair<rectangle, rectangle> r1r2 = cutUp(sub, c, vertical, success);
			if(c<min){min=c; minr=r1r2.second;} 
			if (success)
			{
				subproblem sub1(r1r2.first,sub.ss,sub.depth + 1,-1);
				std::vector<sol> l1 = rek(sub1);

				for (sol s1 : l1)
				{
					v.push_back(fsub(s1,r1r2.second,c));
				}
			}
		}
		selectTopK(v,1);
		trim(v);
		if(v.size()==0)
		{
			sol s_ret(sub,sub.ss);
			fsub f_ret(s_ret,minr,min);
			v.push_back(f_ret);
		}
		for(fsub s1 : v)
		{
			subproblem sub2(s1.rect,s1.so.to,sub.depth + 1,-1);
			std::vector<sol> l2 = rek(sub2);
			for(sol s2 : l2)
			{
				sol s_ret(sub,sub.ss,s2.to,s1.cut,s1.so.b,s2.b);
				l_ret.push_back(s_ret);
			}
			sub2.depth = sub.depth; //the/top right side of a cut can be followed by a cut at the same level
			l2 = rek(sub2);
			for (sol s2 : l2)
			{
				sol s_ret(sub,sub.ss,s2.to,s1.cut,s1.so.b,s2.b);
				l_ret.push_back(s_ret);
			}
		}
		selectTopK(l_ret,1);
		trim(l_ret);
		//if(sub.begin == 0){selectTopK(l_ret, 1);}
		if(l_ret.size()==0){std::cout<<"nay"<<std::endl;} else {std::cout<<"yay"<<std::endl;}
			if(l_ret.size()==0)
			{
				sol s_ret(sub,sub.ss);
				l_ret.push_back(s_ret);
			}
		std::cout<<"to"<<l_ret[0].to[0]<<std::endl;
	}
	if (sub.begin >= 0) //here we aim for only one solution
	{
		rectangle rect_(Coord(0,0),area,this->bins[sub.begin].defects);
		subproblem sub1(rect_,sub.ss,0,-1);
		std::vector<sol> l1=rek(sub1);
std::cout<<l1.size()<<"!"<<std::endl;
		selectTopK(l1,1);
//
		for(sol s1 : l1) //we assume this is only executed once
		{
			if(finished(s1.to)){
				sol s_ret(sub,sub.ss,l1[0].to,-2,l1[0].b,nullptr);
				l_ret.push_back(s_ret);
				return l_ret;
			}
			subproblem sub2(s1.to,sub.begin+1);
			std::vector<sol> l2 = rek(sub2);
			for(sol s2 : l2)
			{
				sol s_ret(sub,sub.ss,s2.to,-2,s1.b,s2.b);
				l_ret.push_back(s_ret);
			}
		}
		//selectTopK(l_ret,1);
	}
	return l_ret;
}

//from v remove all solutions witch are packing 0 items in a rect with no defects
//remove solutions wich are strictly worse than others
//always leave at least one solution because it might be inpossible to pack anything in an area
/* void trim(std::vector<fsub>& v)
{
	//convert std::vector<fsub> to std::vector<Reksolver::sol> call removeRed...
}
 */
//same as above
bool better(RekSolver::sol v1,RekSolver::sol v2)
{
	bool ret=true;
	for(int i=0;i<(int)v1.to.size();i++)
	{
		if(v1.to[i]<v2.to[i]){ret=false;}
	}
	return ret;
}

bool better(RekSolver::fsub v1, RekSolver::fsub v2)
{
	if(v1.cut>v2.cut){return false;}
	for(int i=0;i<(int)v1.so.to.size();i++)
	{
		if(v1.so.to[i]<v2.so.to[i]){return false;}
	}
	return true;
}

bool noitems(RekSolver::sol s)
{
	for(int i=0;i<(int)s.to.size();i++)
	{
		if(s.to[i]>s.from[i]){return false;}
	}
	return true;
}

bool noitems(RekSolver::fsub s)
{
	for(int i=0;i<(int)s.so.to.size();i++)
	{
		if(s.so.to[i]>s.so.from[i]){return false;}
	}
	return true;
}

template<class T> //T=RekSolver::sol or fsub
void trim(std::vector<T>& v)
{
	std::vector<bool> bests(v.size(),true);
	for(int i=0;i<(int)v.size();i++)
	{
		if(noitems(v[i])){bests[i]=false;}
	}
	for(int i=0;i<(int)v.size();i++)
	{
		for(int j=0;j<i;j++)
		{
			if(better(v[i],v[j])){bests[j]=false;}
			if(better(v[j],v[i])){bests[i]=false;}
		}
	}
	std::vector<T> vret;
	for(int i=0;i<(int)v.size();i++)
	{
		if(bests[i]){vret.push_back(v[i]);}
	}
	v=vret;
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
		if (c > first && c < second)
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

std::vector<int> rightsidesofdefs(RekSolver::rectangle r)
{
	std::vector<int> ret;
	for(Solver::Defect d : r.defs)
	{
		ret.push_back(d.pos.x+d.size.x);
	}
	return ret;
}

std::vector<int> topsidesofdefs(RekSolver::rectangle r)
{
	std::vector<int> ret;
	for(Solver::Defect d : r.defs)
	{
		ret.push_back(d.pos.y+d.size.y);
	}
	return ret;
}

//Reksolverben a solverből örökölt változó: stacks
//sub.ss stackstate
//ezek alapján vegyük a következő néhány item hosszait, és az ezekből képezhető számokat
//sub.rect alapján csak a nem túl nagy ilyen számokra van szükség
//subproblem.depth alapján tudjuk hogy függőleges vagy vizszintes vágás jön. --- depth%2

//TODO: átgondolni, hogy egy adott téglalap, adott stackstate adott mélység esetében milyen vágásoknak van egyáltalán értelme
std::list<int> RekSolver::consideredCuts(const subproblem& sub, bool vertical) //returns the list of cuts
{
	int nn = 1; //number of objects considered on in stack
	std::vector<int> lens;
	for(int j=0;j<(int)this->stacks.size();j++)
	{
		for (int i = sub.ss[j]; i < sub.ss[j]+nn; i++)
		{
			lens.push_back(stacks[j][i].size.x);
			lens.push_back(stacks[j][i].size.y);
		}
	}
	int k;
	int base;
	//std::list<int> sums_=sums(lens, k);
	std::list<int> sums_;
	if (vertical)
	{
		base=sub.rect.lb.x;
		k = sub.rect.rt.x - sub.rect.lb.x;
		/* for(int x : rightsidesofdefs(sub.rect))
		{
			if(x<sub.rect.rt.x-100)
			{
				sums_.push_back(x);
			}
		} */
	}
	else
	{
		base=sub.rect.lb.y;
		k = sub.rect.rt.y - sub.rect.lb.y;
		/* for(int x : topsidesofdefs(sub.rect))
		{
			if(x<sub.rect.rt.y-100)
			{
				sums_.push_back(x);
			}
		} */
	}
	
	for(int i=0;i<(int)lens.size();i++)
	{
		if(lens[i]<k){sums_.push_back(lens[i]);}
	}
	for(auto it=sums_.begin();it!=sums_.end();it++){*it+=base;}
	return sums_;
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
			for(int i=lhs.from[s];i<lhs.to[s];i++)
			{
				ar1+=((*stacks)[s][i]).size.area();
			}
			for(int i=rhs.from[s];i<rhs.to[s];i++)
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

class comparator2
{
public:
	std::vector<std::vector<Solver::Item>>* stacks;
	comparator2(std::vector<std::vector<Solver::Item>>* stacks)
	{
		this->stacks=stacks;
	}
	bool operator()(RekSolver::fsub& lhs, RekSolver::fsub& rhs)
	{
		int ar1=0;
		int ar2=0;
		for(int s=0;s<(int)stacks->size();s++)
		{
			for(int i=lhs.so.from[s];i<lhs.so.to[s];i++)
			{
				ar1+=((*stacks)[s][i]).size.area();
			}
			for(int i=rhs.so.from[s];i<rhs.so.to[s];i++)
			{
				ar2+=((*stacks)[s][i]).size.area();
			}
		}
		return ((float)ar1/(lhs.so.s.rect.rt-lhs.so.s.rect.lb).area())>((float)ar2/(rhs.so.s.rect.rt-rhs.so.s.rect.lb).area());
	}
	private:
};

void RekSolver::selectTopK(std::vector<fsub> &v, int k) //returns the best k solutions from l;
{
	//std::sort(v.begin(), v.end(), std::bind(comp, std::placeholders::_1, std::placeholders::_2, this->stacks));
	std::sort(v.begin(), v.end(), comparator2(&(this->stacks)) );
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
	if(sub.rect.defs.size() != 0)
	{
		ret.push_back(sol(sub,sub.ss));
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
	if(ret.size()==0){ret.push_back(sol(sub,sub.ss));}
std::cout<<sub.ss[0]<<"->"<<ret[0].to[0]<<std::endl;
	return ret;
}
