#ifndef vrml_loader_h
#define vrml_loader_h

#include "Geometry.h"

void vrml_LoadMesh(Geometry& g, std::string fn);


template<class T>
void SaveItem(std::ofstream& fs,T& item)
{
	fs.write( (char*)&item, sizeof(T));
}
template<class T>
void OpenItem(std::ifstream& fs,T& item)
{
	fs.read( (char*)&item, sizeof(T));
}
template<class T>
void SaveVector(std::ofstream& fs,std::vector<T>&v)
{
	int num = v.size();
	//fs << num;
	fs.write( (char*)&num,sizeof(int));
	for(int i=0;i<num;i++)
	{
		SaveItem(fs,v[i]);
	}
}
template<class T>
void OpenVector(std::ifstream& fs,std::vector<T>&v)
{
	int num=0;
	//fs >> num;
	fs.read( (char*)&num,sizeof(int));
	v.clear();
	if(!num)return;
	for(int i=0;i<num;i++)
	{
		T tmp;
		OpenItem(fs,tmp);
		v.push_back(tmp);
	}
}

void SaveString(std::ofstream& fs,std::string& str);
void OpenString(std::ifstream& fs,std::string& str);


#endif