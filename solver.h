#include <string>
#include <vector>
#include <list>
#include <memory>
#include <iostream>

//TODO: a subproblemekhez id
//megoldás ábrázolás illetve log kiírni a részproblémát az idjét és az apja idjét
//kiprobálni hibák nélkül lehetne készíteni valahogy 
//TODO: globális változó: legfeljebb 100 függvényhívás után hogy néz ki log.

struct Coord
{
	int x, y;
	int area(){return x*y;}
	Coord() = default;
	Coord(const Coord& c) = default;
	Coord(int x, int y): x(x), y(y) {}
	Coord operator-(const Coord& rhs) const
	{
		return Coord(this->x-rhs.x,this->y-rhs.y);
	}
	bool fits(const Coord& other)
	{
		return (this->x==other.x && this->y==other.y) || (this->x==other.y && this->y==other.x);
	}
	void print() const
	{
		std::cout<<"x:"<<x<<"y:"<<y;
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
	//stacks[1]=2. stacken az itemek
	std::vector<std::vector<Item>> stacks;

	Solver()
	{
		area=Coord(6000,3210);
	}

	void load(std::string filename);
	void save(std::string filename);

	void init();
};

class RekSolver : public Solver
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

	struct subproblem //subproblem
	{
int id;
		rectangle rect;
		std::vector<int> ss; //stack state
		int depth; //depth of the next cut
		int begin; //-1 if the subproblem is a rectangle; begin=k means the subproblem corresponds to all the bins from the k-th one on
		subproblem(const subproblem& sub) = default;
		subproblem(const std::vector<int>& ss, int begin): ss(ss), begin(begin) {}
		subproblem(const rectangle& rect,const std::vector<int>& ss, int depth, int begin): rect(rect), ss(ss), depth(depth), begin(begin) {}
	};
	
	struct bintree //for storing the cutting pattern
	{
		std::shared_ptr<bintree> p1; 
		std::shared_ptr<bintree> p2; //sides of the cut
		int cut; //-1 -- no cut -- -2 p1 and p2 are different bins
		int itemid; //-1 -- no item
		bintree():p1(nullptr), p2(nullptr),cut(-1),itemid(-1) {}
		bintree(std::shared_ptr<bintree> p1,std::shared_ptr<bintree> p2,int c): p1(p1), p2(p2), cut(c), itemid(-1) {}

		void print()
		{
			int c=0;
			std::shared_ptr<bintree> p1_(p1);
			std::shared_ptr<bintree> p2_(p2);
			std::cout<<"bin "<<c<<std::endl;
			pr(p1_);
			c++;
			while(p2_!=nullptr)
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
			if(p!=nullptr)
			{
				if(p->cut!=-1)
				{
					std::cout<<"cut "<<p->cut<<std::endl;
				} else 
				{
					std::cout<<"itemid: "<<p->itemid<<std::endl;
				}
				/* if(p->itemid!=-1)
				{
					
				} */
			}
			if(p->p1!=nullptr){pr(p->p1);}
			if(p->p2!=nullptr){pr(p->p2);}
			/* pr(p->p1);
			pr(p->p2); */
		}
	};

	//TODO: remove unnecesery information;
	struct sol 
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
	};	

	struct fsub
	{
		sol so;
		rectangle rect; //the rectangle on the other side of the cut
		int cut;
		fsub(const RekSolver::sol& sol, const RekSolver::rectangle& rect, int cut): so(sol),rect(rect), cut(cut) {}
	};

	std::vector<sol> rek(const subproblem& sub);
	std::list<int> consideredCuts(const subproblem& sub,bool vertical);
	//cuts up the ractangle into two
	std::pair<rectangle, rectangle> cutUp(const subproblem& sub, int cut, bool vertical, bool& success);
	std::vector<sol> fit(const subproblem& sub); //solves the special case where no more cuts are allowed 
	//i.e. checking if sub.rect matches any upcomming rectangles
	void solve()
	{
		std::vector<int> ss(stacks.size(),0);
		std::vector<sol> ret=rek(subproblem(ss,0));
		ret[0].b->print();
	}
	//removes but the top k solutions
	void selectTopK(std::vector<sol>& l, int k);
	void selectTopK(std::vector<fsub> &v, int k);
	//checks if a solution is reached
	bool finished(const std::vector<int>& ss);
};

