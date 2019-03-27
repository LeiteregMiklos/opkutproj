#include <string>
#include <vector>
#include <list>

struct Coord
{
	int x, y;
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

	struct stackstate //stackstate
	{
		std::vector<int> stacks;
	};

	struct subproblem //subproblem
	{
		rectangle rect;
		int depth;
		bool first; //the left/upper sude of the first cut in a depth must create a subproblem with depth+=1
		//the other side can still be cut in the same depth/same orientation allowing for "parallel" cuts;
		stackstate ss;
		//stackstate
	};
	
	struct sol //TODO: remove unnecesery information;
	{
		subproblem s;
		stackstate from;
		stackstate to;
	};
	//typedef typename std::list<sol> LoS;
	std::list<sol> rek(subproblem sub);
	std::list<int> consideredCuts(subproblem sub);
	std::pair<rectangle, rectangle> cutUp(subproblem sub, int cut);
	std::list<sol> fit(subproblem sub); //solves the special case where no more cuts are allowed 
	//i.e. checking it sub.rect matches any upcomming rectangles
};