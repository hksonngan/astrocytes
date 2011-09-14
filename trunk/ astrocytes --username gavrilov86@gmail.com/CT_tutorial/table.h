#ifndef table_h
#define table_h

#include "AllInc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

class Table
{
public:
	Table();
	Table(int w,int h);
	void SetSize(int w,int h);
	void Clear();
	~Table();
	void StoreToFile(std::string fn);
	void SetValue(std::string val,int i,int j);
	void SetValue(float val,int i,int j,bool undot=1);
	void SetValue(int val,int i,int j);
	void SetValueB(bool val,int i,int j);
private:
	std::string **data;
	int width,height;
};

#endif