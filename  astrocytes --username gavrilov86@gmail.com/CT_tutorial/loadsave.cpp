#include "loadsave.h"
#include "globals.h"

void SaveApp(std::string fn)
{
	std::ofstream fs1(fn.c_str(),std::ios::out | std::ios::binary);
	
	for(int i=0;i<neuron.size();i++)
		for(int j=0;j<neuron[i].size();j++)
		{
			Geometry*g=&neuron[i][j];
			SaveVector(fs1,g->vert_col);
			SaveVector(fs1,g->vert);
			SaveVector(fs1,g->norm);
			SaveVector(fs1,g->tr);
			SaveItem(fs1,g->color);
		}

	fs1.close();
}
void LoadApp(std::string fn)
{

	std::ifstream fs1(fn.c_str(),std::ios::in | std::ios::binary);
	for(int i=0;i<neuron.size();i++)
		for(int j=0;j<neuron[i].size();j++)
		{
			Geometry*g=&neuron[i][j];
			OpenVector(fs1,g->vert_col);
			OpenVector(fs1,g->vert);
			OpenVector(fs1,g->norm);
			OpenVector(fs1,g->tr);
			OpenItem(fs1,g->color);
		}
	fs1.close();
}
