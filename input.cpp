#include <iostream>
#include <fstream>
#include <cstdio>
#include "solver.h"
#include <cstdlib>
#include <algorithm>

std::vector<std::string> parse(std::string line, std::string delim)
{
	std::vector<std::string> parsed;
	std::string str;
	while (str != line)
	{
		str = line.substr(0, line.find(delim));
		line = line.substr(line.find(delim) + 1);
		parsed.push_back(str);
	}
	return parsed;
};

void Solver::load(std::string filename)
{
	std::string file1 = filename + "_batch.csv";
	std::fstream ifs1(file1.c_str());
	std::string line;
	std::string delim = ";";

	std::getline(ifs1, line);
	//std::cout << line << std::endl;

	for (; std::getline(ifs1, line); !ifs1.eof())
	{
		std::vector<std::string> parsed = parse(line, delim);
		Item new_item;
		new_item.id = std::atoi(parsed[0].c_str());
		new_item.size.x = std::atoi(parsed[1].c_str());
		new_item.size.y = std::atoi(parsed[2].c_str());
		new_item.stack = std::atoi(parsed[3].c_str());
		new_item.seq = std::atoi(parsed[4].c_str());
		items.push_back(new_item);
	}

	std::string file2 = filename + "_defects.csv";
	std::ifstream ifs2(file2.c_str());

	std::getline(ifs2, line);
	//std::cout << line << std::endl;

	for (; std::getline(ifs2, line); !ifs2.eof())
	{
		std::vector<std::string> parsed = parse(line, delim);
		int bin_num = std::atoi(parsed[1].c_str());
		int def_num = std::atoi(parsed[0].c_str());
		Coord pos;
		Coord size;
		pos.x = std::atoi(parsed[2].c_str());
		pos.y = std::atoi(parsed[3].c_str());
		size.x = std::atoi(parsed[4].c_str());
		size.y = std::atoi(parsed[5].c_str());
		if ((int)bins.size() <= bin_num)
		{
			bins.resize(bin_num + 1);
		}
		Solver::Defect new_def;
		new_def.id = def_num;
		new_def.pos = pos;
		new_def.size = size;
		bins[bin_num].defects.push_back(new_def);
	}
}

void Solver::init()
{
	for (auto i : items)
	{
		if (i.stack >= (int)stacks.size())
		{
			stacks.resize(i.stack+1);
		}
		stacks[i.stack].push_back(i);
	}
	for (int i = 0; i < (int)stacks.size(); i++)
	{
		std::sort(stacks[i].begin(), stacks[i].end(), [](Item a, Item b) {return a.seq < b.seq;}  );
	}
}