#include "AllInc.h"
#include "table.h"
#include "str.h"
#include "vec.h"

Table::Table()
{
	data=0;
}

void Table::SetSize(int w,int h)
{
	Clear();
	width=w;
	height=h;
	data=new std::string*[width];
	for(int i=0;i<width;i++)
		data[i]=new std::string[height];

}


Table::Table(int w,int h)
{
	data=0;
	SetSize(w,h);

}
Table::~Table()
{
	Clear();
}
void Table::Clear()
{
	if(data)
	{
		for(int i=0;i<width;i++)
			delete[]data[i];
		delete[]data;
		data=0;
	}
}
void Table::StoreToFile(std::string fn)
{
	std::ofstream fs12(fn.c_str(),std::ios::out | std::ios::binary);
	if(data)
	for(int j=0;j<height;j++)
	{
		for(int i=0;i<width;i++)
		{
			fs12 << *(*(data+i)+j) <<"	";
		}
		char ENTER[]={13,10,0};
		fs12 << std::string(ENTER);
		//fs12 << std::endl;
	}
	fs12.close();
}
void Table::OpenFromFile(std::string fn)
{
		
	std::ifstream fs12(fn.c_str(),std::ios::in | std::ios::binary);
	char ENTER[]={13,10,0};
	char buf[10000];
	int j=0;
	while(!fs12.eof())
	{
		memset(buf,0,10000);
		fs12.getline(buf,10000);
		std::string ss(buf);
		svec vls = str::split(ss,"	"+std::string(ENTER));

		for(int i=0;i<vls.size();i++)
		if(i<width)
		{
			//printf("%s",vls[i].c_str());
			*(*(data+i)+j) = vls[i];
			
		}
		j++;
		if(j>=height)break;
		
				//fs12 << *(*(data+i)+j) <<"	";
	}
	fs12.close();
}

float Table::GetFloatValue(int i,int j)
{
	if(i>=0 && j>=0 && i<width && j<height && data)
	{
		std::string ss = *(*(data+i)+j);
		if(1)
		{
			str::StringPairs str_pr;
			str::AddPair(str_pr,",",".");
			ss = str::ReplaceInString(ss,str_pr);
		}

		return str::ToFloat(ss);
	}
		//data[i][j] = val;
	else {printf("\nTable::SetValue error\n");return 0;}
}

void Table::SetValue(std::string val,int i,int j)
{
	if(i>=0 && j>=0 && i<width && j<height && data)
		*(*(data+i)+j) = val;
		//data[i][j] = val;
	else printf("\nTable::SetValue error\n");
		

}

void Table::SetValue(float val,int i,int j,bool undot)
{
	std::string ss = str::ToString(val);

	if(undot)
	{
		str::StringPairs str_pr;
		str::AddPair(str_pr,".",",");
		ss = str::ReplaceInString(ss,str_pr);
	}

	SetValue(ss,i,j);
}

void Table::SetValue(int val,int i,int j)
{
	std::string ss = str::ToString(val);
	SetValue(ss,i,j);
}
void Table::SetValueB(bool val,int i,int j)
{
	if(val)
		SetValue(std::string("+"),i,j);
	else
		SetValue(std::string("-"),i,j);
}
