#include "vrml_loader.h"
#include "vec.h"
#include "str.h"
#include "globals.h"

int total_tr=0,total_v=0;

bool TryLoadBin(Geometry& g, std::string fn)
{
	
	std::ifstream fs1(fn.c_str(),std::ios::in | std::ios::binary);

	if(!fs1)return 0;

	OpenVector(fs1,g.tr);
	OpenVector(fs1,g.vert);
	OpenVector(fs1,g.face);
	OpenVector(fs1,g.norm);


	//g.UpdateBox();
	
	total_tr += g.tr.size();
	total_v += g.vert.size();
	//float vv = g.CalcVolume();
	//float ss = g.CalcArea();
	//printf("\n(tr %d,vr %d, vol %g, sq %g,sav %g)",g.tr.size(),g.vert.size(),vv,ss,ss/vv);

	return 1;
}
void vrml_LoadMesh(Geometry& g, std::string fn)
{

	char buf[1000];
	buf[999]=0;
	if(TryLoadBin(g, fn+".bin"))return;
	std::ifstream fs(fn.c_str(),std::ios::in | std::ios::binary);
	if(!fs)return;
	vec3 ttt;
	ivec3 ttti;

	int tp=0;
	std::string s1 = "point [";
	std::string s2 = "coordIndex [";

	while(!fs.eof())
	{
		fs.getline(buf,1000);
		std::string ss(buf);
		if(ss.find(']')!=-1)tp=0;

		if(tp)
		{
			svec vals = str::split(ss,"	 ,");
			//if(vals.size())printf("%s,%s,%s|",vals[0].c_str(),vals[1].c_str(),vals[2].c_str());
			if(tp==1)
			{
				ttt.set(str::ToFloat(vals[0]),str::ToFloat(vals[1]),str::ToFloat(vals[2]));
				g.vert.push_back(ttt);
			}
			if(tp==2)
			{
				ttti.set(str::ToInt(vals[0]),str::ToInt(vals[1]),str::ToInt(vals[2]));
				g.face.push_back(ttti);
			}
		}

		if(ss.find(s1)!=-1)tp=1;
		if(ss.find(s2)!=-1)tp=2;

	}
	fs.close();
	printf("A");

	g.norm.resize(g.vert.size());
	for(int i=0;i<g.norm.size();i++)
		g.norm[i].set(0,0,0);
	for(int i=0;i<g.face.size();i++)
	{
		Triangle nt;
		nt.v[0] = g.vert[g.face[i].x];
		nt.v[1] = g.vert[g.face[i].y];
		nt.v[2] = g.vert[g.face[i].z];
		getnorm(&nt.v[0].x,&nt.v[1].x,&nt.v[2].x,&nt.norm.x);
		
		g.norm[g.face[i].x] = g.norm[g.face[i].x] + nt.norm;
		g.norm[g.face[i].y] = g.norm[g.face[i].y] + nt.norm;
		g.norm[g.face[i].z] = g.norm[g.face[i].z] + nt.norm;

		nt.norm.normalize();
		g.AddTriangle(nt);
	}
	printf("b");
	
	for(int i=0;i<g.norm.size();i++)
	{
		g.norm[i].normalize();
	}
	printf("c");
	



	fn = fn + ".bin";

	std::ofstream fs1(fn.c_str(),std::ios::out | std::ios::binary);
	SaveVector(fs1,g.tr);
	SaveVector(fs1,g.vert);
	SaveVector(fs1,g.face);
	SaveVector(fs1,g.norm);
	fs1.close();
	printf("D");
}


void SaveString(std::ofstream& fs,std::string& str)
{
	int num = str.size();
	
	fs.write( (char*)&num,sizeof(int));
	for(int i=0;i<num;i++)
	{
		SaveItem(fs,str[i]);
	}
}
void OpenString(std::ifstream& fs,std::string& str)
{
	int num=0;
	
	fs.read( (char*)&num,sizeof(int));
	str.resize(num);
	if(!num)return;
	for(int i=0;i<num;i++)
	{
		char tmp;
		OpenItem(fs,tmp);
		str[i]=tmp;
	}
}


void AddGeom(std::string fn,vec4 cl,int id)
{
	printf("\n%s\n",fn.c_str());
	neuron[id].push_back(Geometry(cl));
	int g_id = neuron[id].size()-1;
	Geometry*g = &neuron[id][g_id];
	vrml_LoadMesh(*g,fn);
	if(id==0)
	{
		g->RebuildTrBB();
		g->RebuildTrBB2();
	}
	//if(id==1)edist.push_back(Edist(g));
	//if(id==3)
		g->UpdateBox();
	if(id==1)
	{
		psd.push_back(PSD(g));
		int sip_id=0;
		vec3 mvv,dd=psd[psd.size()-1].fl.d;
		float l_min=10000;
		for(int i=0;i<neuron[3].size();i++)
		{
			for(int j=0;j<neuron[3][i].tr.size();j++)
			{
				vec3 vv=neuron[3][i].tr[j].v[0]-dd;
				float ll=vv.lengthSQR();
				if(ll<l_min)
				{
					l_min=ll;
					mvv=vv;
					sip_id=i;
				}
			}
		}
		mvv=(neuron[3][sip_id].box1+neuron[3][sip_id].box2)*0.5f-dd;
		if(vec3::dot(mvv,psd[psd.size()-1].fl.n)>0)psd[psd.size()-1].fl.n.Inv();
	}
}

void LoadNeuron()
{
	neuron.resize(4);

	
	for(int i=0;i<5;i++)	AddGeom("wrl//as"+str::ToString(i+1)+".wrl",		vec4(0.4*RND01,1.0,0.7*RND01,1),0);

	vec4 d_col(0.7,0.7,0.7,1);
	vec4 psd_col(0.4,0.4,0.4,1);
	
//m
	for(int i=1;i<=16;i++)		AddGeom("wrl//m"+str::ToString(i)+".wrl",	d_col	,3);
//t
	for(int i=1;i<=36;i++)		AddGeom("wrl//t"+str::ToString(i)+".wrl",	d_col	,3);

//mp1
	for(int i=1;i<=7;i++)		AddGeom("wrl//m"+str::ToString(i)+"p1.wrl",	psd_col	,1);
//mp
	for(int i=8;i<=16;i++)		AddGeom("wrl//m"+str::ToString(i)+"p.wrl",	psd_col	,1);

//mp2
	for(int i=4;i<=4;i++)		AddGeom("wrl//m"+str::ToString(i)+"p1.wrl",	psd_col	,1);

//tp1
	for(int i=1;i<=19;i++)		AddGeom("wrl//t"+str::ToString(i)+"p1.wrl",	psd_col	,1);

//tp
	for(int i=20;i<=36;i++)		AddGeom("wrl//t"+str::ToString(i)+"p.wrl",	psd_col	,1);


	neuron[2].push_back(Geometry(vec4(1,1,1,1)));
	neuron[2].push_back(Geometry(vec4(1,1,1,1)));
	neuron[2].push_back(Geometry(vec4(0,0,0,0.2)));

	printf("\n{%d,%d}",total_tr,total_v);
}