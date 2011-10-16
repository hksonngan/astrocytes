
#include "globals.h"
#include "table.h"

#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range2d.h"
#include "tbb/tick_count.h"
#include "tbb/mutex.h"

using namespace tbb;

//float *squares,*perimeters;

float d0 = 0.02f;
float d1 = 2.0f;
float d_step = 0.02f;

float d_angle = 1;
int ang_it_num = 360/d_angle;

int it_num = int((d1-d0)/d_step);

int opers_left,opers_total;
double start;
mutex* progress_mutex;
/*
void InitProc()
{
	static tbb::task_scheduler_init init;
	progress_mutex = new mutex();
}

void UnInitProc()
{
	delete progress_mutex;
}
void Calc(int psd_id);
void StoreResultsToFile();

class tbb_Calc
{

public:
	tbb_Calc(){ };

public:
    void operator()(const blocked_range <size_t>& r) const
    {        
        for (size_t i=r.begin(); i!=r.end(); i++) 
        {
			Calc(i);
        }
    }
};


void Calc()
{
	start = glfwGetTime ( );
	opers_left =opers_total= it_num*psd.size();
	squares=new float[opers_left];
	perimeters=new float[opers_left];
	memset(squares,0,opers_left*sizeof(float));
	memset(perimeters,0,opers_left*sizeof(float));
	


	InitProc();
	parallel_for(blocked_range<size_t>(0,psd.size(),1),	tbb_Calc() );
	UnInitProc();

	start = glfwGetTime ( )-start;
	printf("\n---\n%gmin\n",start/60);
	
	
	str::StringPairs str_pr;
	str::AddPair(str_pr,".",",");
	str::ReplaceInFile("output2.txt","_output2.txt",str_pr);

	delete[]squares;
	delete[]perimeters;
}*/
void Calc(int psd_id1)
{
	opers_left =opers_total= it_num*psd.size();
	float *svrs=new float[it_num*psd.size()];
	
	memset(svrs,0,it_num*psd.size()*sizeof(float));
	
	float cur_d;
	Geometry *gg0 = new Geometry(),*gg = new Geometry();
	Geometry*gs = new Geometry();

	for(int psd_id=0;psd_id<psd.size();psd_id++)
	{

		cur_d=d0;

		gs->renull();
		AddSphere(gs,d1+d_step,20,20);
		gs->Move(psd[psd_id].fl.d);
		gs->RebuildTrBB();
		gs->RebuildTrBB2();
		gg0->renull();
		for(int as=0;as<neuron[0].size();as++)
		{
			GetSection(gs,&neuron[0][as],gg0,1,0);
			GetSection(&neuron[0][as],gs,gg0,1,0);
		}
		gg0->RebuildTrBB();
		gg0->RebuildTrBB2();
			
		for(int i=0;i<it_num;i++)
		{
			float pp=0;
			gg->renull();
			gs->renull();
			AddSphere(gs,cur_d,20,20);
			gs->Move(psd[psd_id].fl.d);
			gs->RebuildTrBB();
			gs->RebuildTrBB2();

			GetSection(gs,gg0,gg,1,0,&pp);
			float ss = gg->CalcArea();
			float svr = ss ? pp/ss:0;
						
			opers_left--;
		
			svrs[i+psd_id*it_num]=svr;
			cur_d += d_step;
			int sc=((opers_left)*(glfwGetTime ( )-start)/(opers_total-opers_left))/60;
						
			printf("%dpsd	%dit	%d%	%d:%d\n",psd_id,opers_left,(opers_left*100)/opers_total,sc/60,sc%60);
						
		}

				
	//}
//
	Table tbl1(it_num+1,2+psd.size());
	tbl1.SetValue("Distance",0,0);
	tbl1.SetValue("avarage SVR",0,1);
	for(int i=0;i<psd.size();i++)	
		tbl1.SetValue("PSD_"+str::ToString(i+1),0,i+2);
	{
		float cur_d = d0;
		for(int i=0;i<it_num;i++)
		{
			tbl1.SetValue(cur_d,i+1,0);
			cur_d += d_step;		
		}
	}
	for(int i=0;i<it_num;i++)
	{
		//tbl1.SetValue(svrs[i],i+1,1);
		for(int j=0;j<psd.size();j++)
		{
			tbl1.SetValue(svrs[i+j*it_num],i+1,2+j);
		}
	}
	
	tbl1.StoreToFile("results\\psd_svr.txt");
	
	}
//
			
		
	delete gg;
	delete gg0;
	delete gs;
	delete[]svrs;

}

void ShowCount(Lines2&l_segment,Lines2&contour_0,Lines2&contour_1,vec3 vx,vec3 vy,vec3 d)
{
for(int ii=0;ii<l_segment.size();ii++)
					{
						Line lll;
						for(int jj=0;jj<2;jj++)
							lll.v[jj] = vx*l_segment[ii].v[jj].x+vy*l_segment[ii].v[jj].y+d;
						section.push_back(lll);
					}
					
					for(int ii=0;ii<contour_0.size();ii++)
					{
						Line lll;
						for(int jj=0;jj<2;jj++)
							lll.v[jj] = vx*contour_0[ii].v[jj].x+vy*contour_0[ii].v[jj].y+d;
						section.push_back(lll);
					}
					
					for(int ii=0;ii<contour_1.size();ii++)
					{
						Line lll;
						for(int jj=0;jj<2;jj++)
							lll.v[jj] = vx*contour_1[ii].v[jj].x+vy*contour_1[ii].v[jj].y+d;
						section.push_back(lll);
					}
}

//с какой стороны ближе астроцит?

float CalcArea(vec3 pt,float rad)
{
	Geometry*g = new Geometry();
	Geometry*g0 = new Geometry();
	AddSphere(g0,rad,16,16);
	mat34 mt(12,vec3(1).normalized());
	g0->Transform(mt);
	g0->Move(pt);
	g0->UpdateBox();
	g0->RebuildTrBB();

	for(int as=0;as<neuron[0].size();as++)
	{
		GetSection(&neuron[0][as],g0,g,1,0);
	}
	float s = g->CalcArea();

	delete g0;
	delete g;
	return s;
}
/*
float CalcSV(vec3 pt,float rad)
{
	Geometry *g0=new Geometry();
	for(int as=0;as<neuron[0].size();as++)
	{
		GetInSphere(&neuron[0][as],pt,rad,g0);
	}
	float s = CalcArea(g0,pt,rad);
	float v = CalcVolume(g0,pt,rad);
	if(!v) s = 0;else s=s/v;
	delete g0;
	
	return s;
}
*/
float CalcSV(vec3 pt,float rad)
{
	Geometry*g = new Geometry();
	Geometry*g1 = new Geometry();
	Geometry*g0 = new Geometry();
	AddSphere(g0,rad,16,16);
	mat34 mt(12,vec3(1).normalized());
	g0->Transform(mt);
	g0->Move(pt);
	g0->UpdateBox();
	g0->RebuildTrBB();
	//g0->RebuildTrBB2();

	for(int as=0;as<neuron[0].size();as++)
	{
		GetSection(&neuron[0][as],g0,g,1,0);
		GetSection(g0,&neuron[0][as],g1,1,0);
	}
	float s = g->CalcArea();
	float v = g->CalcVolume()+g1->CalcVolume();
	if(!v) s = 0;else s=s/v;

	delete g0;
	delete g;
	delete g1;
	return s;
}
int iters;
class tbb_Calc2
{

public:
	tbb_Calc2(vec3 d0,vec3 x0,vec3 y0,float rad0,float*res0,ivec2 res_size0):d(d0),x(x0),y(y0),rad(rad0),res(res0),res_size(res_size0){iters=res_size.x*res_size.y; };

	
	vec3 d,x, y;
	float rad;
	float*res;
	ivec2 res_size;
public:
    void operator()(const blocked_range <size_t>& r) const
    {       
		vec3 x0=x;
		vec3 y0=y;
		vec3 d0=d;
        for (size_t id=r.begin(); id!=r.end(); id++) 
        {
			int i = id%res_size.x;
			int j = id/res_size.x;
			
			res[id] = CalcArea(d0+x0*(i/float(res_size.x))+y0*(j/float(res_size.y)),rad);
			
			iters--;
			printf("\n%d ",iters);
        }
    }
};
/*
void CalcAreas(vec3 d,vec3 x,vec3 y,float rad,float*res,ivec2 res_size)
{
	for(int i=0;i<res_size.x;i++)
		for(int j=0;j<res_size.y;j++)
		{
			res[i+j*res_size.x] = CalcArea(d+x*(i/float(res_size.x))+y*(j/float(res_size.y)),rad);
		}
}
*/
/*
void CalcAreas(vec3 d,vec3 x,vec3 y,float rad,float*res,ivec2 res_size)
{
	InitProc();
	parallel_for(blocked_range<size_t>(0,res_size.x*res_size.y,1),	tbb_Calc2(d, x, y, rad,res, res_size) );
	UnInitProc();

}
*/

/*

void Calc()
{
			float d0 = 0.02f,cur_d;
			float d1 = 2.0f;
			float d_step = 0.01f;
			int it_num = int((d1-d0)/d_step);
			float *squares=new float[it_num*edist.size()],*perimeters=new float[it_num*edist.size()];

			double start = glfwGetTime ( );
			double prep_time=0,proc_time=0; 
			std::ofstream fs11("output.txt",std::ios::out | std::ios::binary);
			Geometry *gg0 = new Geometry(),*gg = new Geometry();

			for(cur_psd=0;cur_psd<edist.size();cur_psd++)
			{
				double start0 = glfwGetTime ( );

				gg0->renull();
				gg->renull();
				fs11 << "Distance	Perimeter	Square	P/S	PSD" << cur_psd << std::endl;
				cur_d=d0;
				edist[cur_psd].SetDistance(d1+d_step);
				edist[cur_psd].mesh.BuildRep1();
				edist[cur_psd].mesh.RebuildTrBB2();
				for(int as=0;as<neuron[0].size();as++)
				{
					GetSection(&edist[cur_psd].mesh,&neuron[0][as],gg0,1,0);
					GetSection(&neuron[0][as],&edist[cur_psd].mesh,gg0,1,0);
				}
				gg0->RebuildTrBB();
				gg0->RebuildTrBB2();

				prep_time = ((glfwGetTime ( ) - start0)+cur_psd*prep_time)/(cur_psd+1);
				printf("\n---\n%g sec prepare(%d left)\n",prep_time,edist.size()-cur_psd-1);

				double start1 = glfwGetTime ( );
		
		
		
				for(int i=0;i<it_num;i++)
				{
					int id = i+cur_psd*it_num;
					perimetr=0;
					gg->renull();
					edist[cur_psd].SetDistance(cur_d);
					edist[cur_psd].mesh.BuildRep1();
					GetSection(&edist[cur_psd].mesh,gg0,gg,1,0);
					float ss = gg->CalcArea();
					
					float psps = ss ? perimetr/ss:0;
					
					printf("\n%g	%g	%g	%g",cur_d , perimetr , ss , psps);
					fs11 << cur_d <<"	"<< perimetr <<"	"<< ss <<"	"<< psps << std::endl;
					squares[id]=ss;
					perimeters[id]=perimetr;
					cur_d += d_step;
					if(!cur_psd)proc_time=(1+(d1-cur_d)/(cur_d-d0))*(glfwGetTime ( )-start1);
					printf("	%g min",((d1-cur_d)*(glfwGetTime ( )-start1)/(cur_d-d0) + (prep_time+proc_time)*(edist.size()-cur_psd-1))/60);
				}
				if(!cur_psd)proc_time=0;
				proc_time = ((glfwGetTime ( ) - start1)+cur_psd*proc_time)/(cur_psd+1);
				fs11 << std::endl;
			}
			fs11.close();
			str::StringPairs str_pr;
			str::AddPair(str_pr,".",",");
			str::ReplaceInFile("output.txt","_output.txt",str_pr);
			

			std::ofstream fs12("output2.txt",std::ios::out | std::ios::binary);
			fs12 << "Distance	";
			for(cur_psd=0;cur_psd<edist.size();cur_psd++)
				fs12 << "Perimeter"<< cur_psd <<"	Square"<< cur_psd <<"	PS" << cur_psd<<"	";
			fs12 << std::endl;
			cur_d=d0;
			for(int i=0;i<it_num;i++)
			{
				fs12 << cur_d <<"	";
				for(cur_psd=0;cur_psd<edist.size();cur_psd++)
				{
					int id = i+cur_psd*it_num;
					float psps = squares[id] ? perimeters[id]/squares[id]:0;
					fs12 << perimeters[id] <<"	"<< squares[id] <<"	"<< psps<< "	";

				}
				fs12 << std::endl;
				cur_d+=d_step;
			}
			fs12.close();

			str::ReplaceInFile("output2.txt","_output2.txt",str_pr);


			start = glfwGetTime ( )-start;
			printf("\n---\n%g\n",start/60);
			cur_psd=0;
			delete gg;
			delete gg0;
			delete[] perimeters;
			delete[]squares;

		}
		
	*/