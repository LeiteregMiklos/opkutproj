#include <string>
#include <vector>
#include <list>

struct Coord
{
	int x, y;
	int area(){return x*y;}
	Coord operator-(Coord rhs)
	{
		Coord c;
		c.x=this->x-rhs.x;
		c.y=this->y-rhs.y;
		return c;
	}
	bool fit( Coord other)
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
		Item() : bin(-1) {} //not in any
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
		area.x = 6000;
		area.y = 3210;
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
		std::list<Defect> defs;
		int depth; //depth of the next cut
	};

	struct subproblem //subproblem
	{
		rectangle rect;
		int depth;
		bool first; //the left/upper side of the first cut in a depth must create a subproblem with depth+=1
		//the other side can still be cut in the same depth/same orientation allowing for "parallel" cuts;
		std::vector<int> ss;
		//stackstate
	};
	
	struct sol //TODO: remove unnecesery information;
	{
		subproblem s;
		std::vector<int> from; //ends included
		std::vector<int> to;
	};
	//bool comp(const sol& lhs, const sol& rhs);
	//typedef typename std::list<sol> LoS;
	std::vector<sol> rek(subproblem sub);
	std::list<int> consideredCuts(subproblem sub,bool vertical);
	std::pair<rectangle, rectangle> cutUp(subproblem sub, int cut, bool vertical, bool& success);
	std::vector<sol> fit(subproblem sub); //solves the special case where no more cuts are allowed 
	void solve()
	{
		subproblem sub;
		rectangle r;
		r.lb.x=0; r.lb.y=0; r.rt.x=10000; r.rt.y=1000;
		r.depth=0;
		sub.rect=r;
		sub.depth=0;
		sub.first=true;
	}
	//i.e. checking it sub.rect matches any upcomming rectangles
	void selectTopK(std::vector<RekSolver::sol>& l, int k);
};