#include <string>
#include <vector>
#include <list>
#include <memory>
#include <iostream>

struct Coord
{
	int x, y;
	int area(){return x*y;}
	Coord() = default;
	Coord(const Coord& c) = default;
	Coord(int x, int y): x(x), y(y) {}
	Coord operator-(Coord& rhs)
	{
		return Coord(this->x-rhs.x,this->y-rhs.y);
	}
	bool fits(const Coord& other)
	{
		return (this->x==other.x && this->y==other.y) || (this->x==other.y && this->y==other.x);
	}
};

class Solver
{
public:
	// std::string plan_id;
	Coord area; //size of a bin

	class Item
	{
	public:
		Coord size;
		int bin;
		int stack;
		int seq;
		int id;
	};

	class Defect
	{
	public:
		int id;
		Coord pos;
		Coord size;
	};

	class Bin
	{
	public:
		std::vector<Defect> defects;
	};

	std::vector<Item> items;
	std::vector<Bin> bins;

	//typedef typename std::list<Item> sequence
	//stacks[2]=2. stacken az itemek
	std::vector<std::vector<Item>> stacks;

	Solver()
	{
		area=Coord(6000,3210);
	}

	void load(std::string filename);
	void save(std::string filename);

	void init();
};


class RekSolver : Solver
{
public:
	struct rectangle
	{
		Coord lb; //left bottom
		Coord rt; //right top corner of the rectangle
		std::vector<Defect> defs;
		rectangle(const Coord& lb, const Coord& rt, const std::vector<Defect>& defs): lb(lb),rt(rt),defs(defs){}
		rectangle()=default;
	};

/* 	struct stackstate
	{
		std::vector<int> state;
		std::vector<std::vector<Item>>* p;
	}; */

	struct subproblem //subproblem
	{
		rectangle rect;
		std::vector<int> ss;
		int depth; //depth of the next cut
		int begin; //-1 if the subproblem is a rectangle; begin=k means the subproblem corresponds to all the bins from the k-th one on
		subproblem(const subproblem& sub) = default;
		subproblem(int begin): begin(begin) {}
		subproblem(const std::vector<int>& ss, int begin): ss(ss), begin(begin) {}
		subproblem(const rectangle& rect,const std::vector<int>& ss, int depth, int begin): rect(rect), ss(ss), depth(depth), begin(begin) {}
	};
	
	struct bintree //for storing the cutting pattern
	{
		std::shared_ptr<bintree> p1; 
		std::shared_ptr<bintree> p2; //sides of the cut
		int cut; //-1 -- no cut -- -2 p1 and p2 are different bins
		int itemid; //-1 -- no item
		bintree():p1(NULL), p2(NULL),cut(-1),itemid(-1) {}
		bintree(std::shared_ptr<bintree> p1,std::shared_ptr<bintree> p2,int c): p1(p1), p2(p2), cut(c), itemid(-1) {}

		void print()
		{
			int c=0;
			std::shared_ptr<bintree> p1_(p1);
			std::shared_ptr<bintree> p2_(p2);
			std::cout<<"bin "<<c<<std::endl;
			pr(p1_);
			c++;
			while(p2_!=NULL)
			{
				p1_=p2_->p1;
				p2_=p2_->p2;
				std::cout<<"bin "<<c<<std::endl;
				pr(p1_);
				c++;
			}
		}
		static void pr(std::shared_ptr<bintree> p)
		{
			if(p!=NULL)
			{
				if(p->cut!=-1)
				{
					std::cout<<"cut "<<p->cut<<std::endl;
				}
				if(p->itemid!=-1)
				{
					std::cout<<"itemid: "<<p->itemid<<std::endl;
				}
			}
			pr(p->p1);
			pr(p->p2);
		}
	};

	struct sol //TODO: remove unnecesery information;
	{
		subproblem s;
		std::vector<int> from;  
		std::vector<int> to; //[ , )
		std::shared_ptr<bintree> b;
		sol(const subproblem& sub,const std::vector<int>& v): s(sub), b(new bintree()) //we didnt pack anything
		{
			this->from=v;
			this->to=v;
		}
		sol(subproblem sub, std::vector<int> from,std::vector<int> to,int c,std::shared_ptr<bintree> p1,std::shared_ptr<bintree> p2):
		s(sub),from(from),to(to), b(new bintree(p1,p2,c)) {}
		/* sol~()
		{
			del(b->p1);
			del(b->p2);
			delete b;
			void del(bintree* p)
			{
				if(p!=NULL)
				{
					del(p->p1);
					del(p->p2);
					delete p;
				}
			}
		} */
	};

	

	std::vector<sol> rek(subproblem sub);
	std::list<int> consideredCuts(subproblem sub,bool vertical);
	std::pair<rectangle, rectangle> cutUp(subproblem sub, int cut, bool vertical, bool& success);
	std::vector<sol> fit(subproblem sub); //solves the special case where no more cuts are allowed 
	void solve()
	{
		rek(subproblem(0));
	}
	//i.e. checking it sub.rect matches any upcomming rectangles
	void selectTopK(std::vector<RekSolver::sol>& l, int k);
	bool finished(std::vector<int>& ss);
};