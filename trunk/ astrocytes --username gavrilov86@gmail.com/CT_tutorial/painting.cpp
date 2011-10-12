
#include "globals.h"
#include "table.h"

#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range2d.h"
#include "tbb/tick_count.h"


float CalcSVR(vec3 cc,float rr)
{
	Geometry*gs = new Geometry();
	Geometry* g1 = new Geometry();
	Geometry* g2 = new Geometry();
	AddSphere(gs,rr,20,20);
	//mat34 mt(RND11*5,vec3(1,1,1).normalized());
	//gs->Transform(mt);
	gs->Move(cc+vec3(RND11,RND11,RND11)*0.00001f);
	gs->UpdateBox();
	gs->RebuildTrBB();
	//gs->RebuildTrBB2();
			
	for(int as=0;as<neuron[0].size();as++)
	{
		GetSection(gs,&neuron[0][as],g1,1,0);
		GetSection(&neuron[0][as],gs,g2,1,0);
	}
	delete gs;

	float ss = g2->CalcArea();
	float vv = g2->CalcVolume() + g1->CalcVolume();
	delete g1;
	delete g2;
	float svr = (vv>0)?ss/vv:0;
	//printf("  (%g %g %g)  ",ss,vv,svr);
	return svr;
}

float mc_CalcSVR(vec3 cc,float rr, vec3 c1,bool ins)
{
	Geometry*g0=new Geometry();
		
	for(int as=0;as<neuron[0].size();as++)
	{
		GetInSphere(&neuron[0][as],cc,rr,g0);
	}

	float ss = mc_CalcArea(g0,cc,rr);
	float vv = mc_CalcVolume(g0,cc,rr,c1,ins);
	float svr = (vv>0)?ss/vv:0;

	delete g0;

	return svr;
}

float CalcSVR_safe(vec3 center,float rr)
{
	float old_c;
	int ct=0,nct=5;
	while(ct!=nct)
	{
		float cc = CalcSVR( center, rr);
		if(cc>0)
		{
			if(ct && abs(cc-old_c)>1)
			{
				printf("[%g,%g]",cc,old_c);
				ct=0;

			}else
			{
				old_c=cc;
				ct++;
			}

		}else printf("=");

	}
	return old_c;

}
void SavePainting(std::string fn)
{
	std::ofstream fs1(fn.c_str(),std::ios::out | std::ios::binary);
	for(int as=0;as<neuron[0].size();as++)
	{
		Geometry*g=&neuron[0][as];

		SaveVector(fs1,g->vert_val);
	}
	fs1.close();
}
void LoadPainting(std::string fn)
{
	std::ifstream fs1(fn.c_str(),std::ios::in | std::ios::binary);
	if(!fs1)return;
	for(int as=0;as<neuron[0].size();as++)
	{
		Geometry*g=&neuron[0][as];

		OpenVector(fs1,g->vert_val);
	}
	fs1.close();

}

void PaintTrue(float brightness)
{
	float max_svr=0;
	LoadPainting("painting_svr");
	for(int as=0;as<neuron[0].size();as++)
	{
		Geometry*g=&neuron[0][as];

		for(int i=0;i<g->vert_val.size();i++)
			if(g->vert_val[i]>=0)
			{
				if(max_svr<g->vert_val[i])max_svr=g->vert_val[i];
			}
	}
	printf("max svr: %g ",max_svr);
	for(int as=0;as<neuron[0].size();as++)
	{
		Geometry*g=&neuron[0][as];
		g->vert_col.resize(g->vert.size());

		for(int i=0;i<g->vert_col.size();i++)
		if(g->vert_val[i]>=0)
		{
			float cl = g->vert_val[i];
			cl = min(1,cl*brightness);
			g->vert_col[i].set(cl,0,1-cl,1);
/*
			if(cl<5)g->vert_col[i].set(0,0,1,1);else
			if(cl<10)g->vert_col[i].set(0.5,0.5,1,1);else
			if(cl<15)g->vert_col[i].set(0,0.5,0,1);else
			if(cl<20)g->vert_col[i].set(1,1,0,1);else
			g->vert_col[i].set(1,0,0,1);
			*/
			//g->vert_col[i].set(cl,0,0,1);
		}else
			//g->vert_col[i] = g->color;
			g->vert_col[i].set(0.3,0.3,0.3,1);


		Geometry g1;
		g1.BuildSmoothed(g);
		
		g->vbo_mesh.Build(g1.vert,g1.norm,g1.vert_col,g1.face);
	}
}
#define MAX_SVR 100
void PaintAs(vec3 cc,float rr)
{
	srand(glfwGetTime());
	int total_iters=0,it_l;
	
	LoadPainting("painting_svr");
	
	for(int as=0;as<neuron[0].size();as++)
	{
		Geometry*g=&neuron[0][as];
		if(g->vert_val.size()!=g->vert.size()) 
		{
			g->vert_val.resize(g->vert.size());
			
			for(int i=0;i<g->vert.size();i++)
			{
				g->vert_val[i]=-1;
			}
		}

		for(int i=0;i<g->vert.size();i++)
			if(g->vert[i].lengthSQR(cc)<rr*rr)
				if(g->vert_val[i]<0)
					total_iters++;
	}
	it_l = total_iters;

	double start1 = glfwGetTime();
	for(int as=0;as<neuron[0].size();as++)
	{
		Geometry*g=&neuron[0][as];
		for(int i=0;i<g->vert.size();i++)
			if(g->vert[i].lengthSQR(cc)<rr*rr)
			if(g->vert_val[i]<0)
			{
				//if(g->vert_val[i]==0 || g->vert_val[i]>MAX_SVR)printf("-");

				float svr = mc_CalcSVR(g->vert[i],0.6f ,g->vert[i]+g->norm[i]*0.0001f,0);
				//float svr1 = mc_CalcSVR(g->vert[i],0.6f,g->vert[i]+g->norm[i]*0.0001f,0);
				if(svr>0)g->vert_val[i] = svr;

				it_l--;
				if(!(it_l%10))SavePainting("painting_svr");
					
				printf("%g min {%g}\n",(it_l*(glfwGetTime ( )-start1)/(total_iters-it_l))/60,svr);
			}

	}
	SavePainting("painting_svr");
	//PaintTrue(1);
}