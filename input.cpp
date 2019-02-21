#include <iostream>
#include <fstream>
#include <cstdio>
#include "solver.h"

std::vector<std::string> parse(std::string line, std::string delim)
{
	std::vector<std::string> parsed;
	std::string str;
	while(str != line){
		str = line.substr(0,line.find(delim));
		line = line.substr(line.find(delim) + 1);
		parsed.push_back(str);
	}
	return parsed;
}

void Solver::load(std::string filename)
{
	std::string file1=filename+"_batch.csv";
	std::ifstream ifs1(file1);
  std::string line;
	std::string delim=";";
	
	std::getline ( ifs1, line ); 
  //std::cout << line << std::endl; 

  for(;std::getline(ifs1,line);!ifs1.eof())
  {
		std::vector<std::string> parsed=parse(line,delim);
		Item new_item;
		new_item.id=std::stoi(parsed[0]);
		new_item.size.x=std::stoi(parsed[1]);
		new_item.size.y=std::stoi(parsed[2]);
		new_item.stack=std::stoi(parsed[3]);
		new_item.seq=std::stoi(parsed[4]);
		items.push_back(new_item);
  }
	
	std::string file2=filename+"_defects.csv";
	std::ifstream ifs2(file2);

	std::getline ( ifs2, line ); 
  //std::cout << line << std::endl; 

  for(;std::getline(ifs2,line);!ifs2.eof())
  {
		std::vector<std::string> parsed=parse(line,delim);
		int bin_num=std::stoi(parsed[1]);
		int def_num=std::stoi(parsed[0]);
		Coord pos; Coord size;
		pos.x=std::stoi(parsed[2]);
		pos.y=std::stoi(parsed[3]);
		size.x=std::stoi(parsed[4]);
		size.y=std::stoi(parsed[5]);
		if(bins.size()<=bin_num)
		{
			bins.resize(bin_num+1);
		}
		Bin::Defect new_def;
		new_def.id=def_num;
		new_def.pos=pos;
		new_def.size=size;
		bins[bin_num].defects.push_back(new_def);
	}
}