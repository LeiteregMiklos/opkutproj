#include <string>
#include <vector>

struct Coord {
  int x,y;
};

class Solver {
 public:

  // std::string plan_id;
  Coord area; 
  
  class Item {
  public:
    Coord size;
    int bin;
		int stack;
		int seq;
    int id;
    Item() : bin(-1) {}
  };

	class Bin
	{
		public:
		class Defect 
		{
		public:
			int id;
			Coord pos;
			Coord size;
		};
		std::vector<Defect> defects;
	};

	std::vector<Item> items;
	std::vector<Bin> bins;

  Solver()
  {
		area.x=6000;
		area.y=3210;
  }

  void load(std::string filename);
  void save(std::string filename);

};
