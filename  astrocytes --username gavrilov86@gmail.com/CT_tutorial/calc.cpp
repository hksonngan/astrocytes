
#include "globals.h"
#include "table.h"

#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range2d.h"
#include "tbb/tick_count.h"
#include "tbb/mutex.h"

using namespace tbb;

float *squares,*perimeters;

float d0 = 0.02f;
float d1 = 2.0f;
float d_step = 0.02f;

float d_angle = 1;
int ang_it_num = 360/d_angle;

int it_num = int((d1-d0)/d_step);

int opers_left,opers_total;
double start;
mutex* progress_mutex;

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
}
void Calc(int psd_id)
{
	/*
			float cur_d;

			Geometry *gg0 = new Geometry(),*gg = new Geometry();

		
				gg0->renull();
				gg->renull();
				cur_d=d0;
				edist[psd_id].SetDistance(d1+d_step);
				edist[psd_id].mesh.BuildRep1();
				edist[psd_id].mesh.RebuildTrBB2();
				for(int as=0;as<neuron[0].size();as++)
				{
					GetSection(&edist[psd_id].mesh,&neuron[0][as],gg0,1,0);
					GetSection(&neuron[0][as],&edist[psd_id].mesh,gg0,1,0);
				}
				gg0->RebuildTrBB();
				gg0->RebuildTrBB2();
		
				for(int i=0;i<it_num;i++)
				{
					int id = i+psd_id*it_num;
					float pp=0;
					gg->renull();
					edist[psd_id].SetDistance(cur_d);
					edist[psd_id].mesh.BuildRep1();
					GetSection(&edist[psd_id].mesh,gg0,gg,1,0,&pp);
					float ss = gg->CalcArea();
		
					float psps = ss ? pp/ss:0;
					
					progress_mutex->lock();
					opers_left--;
					squares[id]=ss;
					perimeters[id]=pp;
					cur_d += d_step;
					int sc=((opers_left)*(glfwGetTime ( )-start)/(opers_total-opers_left))/60;
					
					printf("%d	%d%	%d:%d\n",opers_left,(opers_left*100)/opers_total,sc/60,sc%60);
					
					progress_mutex->unlock();
				}

			
			StoreResultsToFile();
			
		
			delete gg;
			delete gg0;
			*/

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

extern bool fill_gaps_error;
//пучком плоскостей
void Calc2(int psd_id)
{
	double start = glfwGetTime ( );

	Table tbl[3];
	
	std::string fn;
	float *dt1=new float[ang_it_num*it_num];
	float *dt2=new float[ang_it_num*it_num];
	float *dt3=new float[ang_it_num*it_num];
	memset(dt1,0,ang_it_num*it_num*sizeof(float));
	memset(dt2,0,ang_it_num*it_num*sizeof(float));
	memset(dt3,0,ang_it_num*it_num*sizeof(float));
	fn = "results\\areas"+str::ToString(psd_id)+".bin";
	std::ofstream fs_1(fn.c_str(),std::ios::out | std::ios::binary);
	fn = "results\\volumes"+str::ToString(psd_id)+".bin";
	std::ofstream fs_2(fn.c_str(),std::ios::out | std::ios::binary);
	fn = "results\\volume_to_surface"+str::ToString(psd_id)+".bin";
	std::ofstream fs_3(fn.c_str(),std::ios::out | std::ios::binary);
	fs_1.write((char*)&ang_it_num,sizeof(int));
	fs_1.write((char*)&it_num,sizeof(int));
	fs_2.write((char*)&ang_it_num,sizeof(int));
	fs_2.write((char*)&it_num,sizeof(int));
	fs_3.write((char*)&ang_it_num,sizeof(int));
	fs_3.write((char*)&it_num,sizeof(int));
	
	for(int t=0;t<3;t++)
	{
		tbl[t].SetSize(it_num+1,ang_it_num+1);
		
		tbl[t].SetValue("angle\\distance",0,0);
	
		for(int ang=0;ang<ang_it_num;ang++)
			tbl[t].SetValue(ang*(int)d_angle,0,ang+1);
		for(int i=0;i<it_num;i++)
			tbl[t].SetValue(d0+i*d_step,i+1,0);
	}
	
	vec3 norm_sect = psd[psd_id].left;
	vec3 vx = psd[psd_id].front;
	vec3 vy = psd[psd_id].fl.n;
	vec3 d = psd[psd_id].fl.d;
	float med_rd = psd[psd_id].mediator_radius;

	mat34 mr(cos(d_angle*PI/180),sin(d_angle*PI/180),vy);
	
	int as_num=neuron[0].size();
	std::vector<Lines> lines;
	lines.resize(neuron[0].size());
	for(int ang_id=0;ang_id<ang_it_num/2;ang_id++)
	{
		if(ang_id)
		{
			norm_sect = mr.MultRot(norm_sect);
			vx = mr.MultRot(vx);
		}
		
		std::vector<Lines2> l0,l1[2],contour0,contour1;
		l0.resize(as_num);
		l1[0].resize(as_num);
		l1[1].resize(as_num);
		contour0.resize(as_num);
		contour1.resize(as_num);
		for(int as=0;as<as_num;as++)
		{
			Lines lines;
			FlatCrossGeometry(neuron[0][as],flat(d, norm_sect),lines,0);
			ProectOnFlat(lines,d,vx,vy,l0[as]);
			CutByX(l0[as],0);
		}
			
		for(int k=0;k<2;k++)
		{
			std::vector<iv2vec> crossed0,crossed1;
			crossed0.resize(as_num);
			crossed1.resize(as_num);
			for(int as=0;as<as_num;as++)
			{
				l1[k][as]=l0[as];
				MoveLinesX(l1[k][as],med_rd*(k?(1):-1));
				
				Lines2 tmpl;
				CutByCircle(l1[k][as], d1,&tmpl,0,&crossed0[as]);
				l1[k][as]=tmpl;
			}
			float cur_d = d1;
			float ss0=0,vv0=0;
			for(int i=0;i<it_num;i++)
			{
				bool was_error=0;
				
				for(int as=0;as<as_num;as++)
				{
					contour1[as].clear();
					FillCircleGaps(cur_d,l1[k][as], &crossed0[as],contour1[as],1);//связываем внешние разрывы остатка сегмента l1[k] - теперь он вместе с contour1 - замкнутые контура
					
				}
				cur_d -= d_step;
				float ss=0,vv=0,v1=0,v2=0,v3=0;
				bool ov=0;
				for(int as=0;as<as_num;as++)
				{
					crossed0[as].clear();
					crossed1[as].clear();
					Lines2 tmpl,l_segment;
					CutByCircle(l1[k][as], cur_d,&tmpl,&l_segment,&crossed0[as],&crossed1[as]);
					l1[k][as]=tmpl;
					contour0[as].clear();
					FillCircleGaps(cur_d,l_segment, &crossed1[as],contour0[as],0);

					MoveLinesX(l_segment,med_rd*(k?(-1):(1)));
					Lines2 contour_0=contour0[as],contour_1=contour1[as];
					MoveLinesX(contour_0,med_rd*(k?(-1):(1)));
					MoveLinesX(contour_1,med_rd*(k?(-1):(1)));

					ss += RotationSurfaceArea(l_segment);
					float vv1 = RotationSurfaceVolume(l_segment);
					float vv2 = RotationSurfaceVolume(contour_0);
					float vv3 = RotationSurfaceVolume(contour_1);
					
					if(vv1>0)v1=vv1;if(vv2>0)v2=vv2;if(vv3>0)v3=vv3;
					vv+=v1+v2+v3;
						
					if(vv1+vv2+vv3<0){printf("V");ov=1;}
					//if(fill_gaps_error){fill_gaps_error=0;was_error=1;printf("{%d,%d,%d,%d,%g}",ang_id,k,i,as,cur_d);ShowCount(l_segment,contour_0,contour_1,vx,vy,d);}
					
					
					
					
				}
				
				if(ov)vv=vv0;
				vv0=vv;
			
				float sv = vv?(ss/vv):0;
				tbl[0].SetValue(ss,it_num-i,ang_id+1+k*(ang_it_num/2));
				//if(!was_error)
				{
					tbl[1].SetValue(vv,it_num-i,ang_id+1+k*(ang_it_num/2));
					tbl[2].SetValue(sv,it_num-i,ang_id+1+k*(ang_it_num/2));
				}
			
				dt1[it_num-i-1+it_num*(ang_id+k*(ang_it_num/2))]=ss;
				dt2[it_num-i-1+it_num*(ang_id+k*(ang_it_num/2))]=vv;
				dt3[it_num-i-1+it_num*(ang_id+k*(ang_it_num/2))]=sv;
				

				
			
			}
		}
	}
	
	tbl[0].StoreToFile("results\\areas"+str::ToString(psd_id)+".txt");
	tbl[1].StoreToFile("results\\volumes"+str::ToString(psd_id)+".txt");
	tbl[2].StoreToFile("results\\volume_to_surface"+str::ToString(psd_id)+".txt");

	fs_1.write((char*)dt1,sizeof(float)*ang_it_num*it_num);
	fs_2.write((char*)dt2,sizeof(float)*ang_it_num*it_num);
	fs_3.write((char*)dt3,sizeof(float)*ang_it_num*it_num);
	delete[]dt1;
	delete[]dt2;
	delete[]dt3;

	fs_1.close();
	fs_2.close();
	fs_3.close();
}

//с какой стороны ближе астроцит?
	

void Calc3()
{
	Table*tbl=new Table(4,psd.size()+1);
	tbl->SetValue("above_the_flat",1,0);
	tbl->SetValue("distance",2,0);
	tbl->SetValue("astrocyte_ID",3,0);
	for(int psd_id=0;psd_id<psd.size();psd_id++)
	{
		
		tbl->SetValue("PSD"+str::ToString(psd_id),0,psd_id+1);
		float min_l=10000;
		int as_id=0,v_id=0;
		vec3 mvv,d=psd[psd_id].fl.d;
		for(int as=0;as<neuron[0].size();as++)
		{
			for(int i=0;i<neuron[0][as].vert.size();i++)
			{
				vec3 vv=neuron[0][as].vert[i]-d;
				float ll=vv.lengthSQR();
				if(ll<min_l)
				{
					min_l=ll;
					mvv=vv;
					as_id=as;
					v_id=i;

				}
			}
		}
		
		bool up=vec3::dot(psd[psd_id].fl.n,mvv)>0;
		tbl->SetValueB(up,1,psd_id+1);
		tbl->SetValue(sqrt(min_l),2,psd_id+1);
		tbl->SetValue(as_id,3,psd_id+1);

	}
	printf("end");
	tbl->StoreToFile("results\\astr.txt");
	delete tbl;
}

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
void CalcAreas(vec3 d,vec3 x,vec3 y,float rad,float*res,ivec2 res_size)
{
	InitProc();
	parallel_for(blocked_range<size_t>(0,res_size.x*res_size.y,1),	tbb_Calc2(d, x, y, rad,res, res_size) );
	UnInitProc();

}

void StoreResultsToFile()
{
			std::ofstream fs12("output2.txt",std::ios::out | std::ios::binary);
			fs12 << "Distance	";
			for(int psd_id=0;psd_id<psd.size();psd_id++)
				fs12 << "Perimeter"<< psd_id <<"	Square"<< psd_id <<"	PS" << psd_id<<"	";
			fs12 << std::endl;
			float cur_d=d0;
			for(int i=0;i<it_num;i++)
			{
				fs12 << cur_d <<"	";
				for(int psd_id=0;psd_id<psd.size();psd_id++)
				{
					int id = i+psd_id*it_num;
					float psps = squares[id] ? perimeters[id]/squares[id]:0;
					fs12 << perimeters[id] <<"	"<< squares[id] <<"	"<< psps<< "	";

				}
				fs12 << std::endl;
				cur_d+=d_step;
			}
			fs12.close();
}

float NearestPSD(vec3 pt)
{
	float minl;
	for(int i=0;i<psd.size();i++)
	{
		float ll = pt.lengthSQR(psd[i].fl.d);
		if(ll<minl || !i)minl=ll;
	}
	return minl;
}
void CalcHist1()
{
	ivec ass;
	ivec trs;
	int rad_num=20;
	float *all_vl=new float[rad_num];
	memset(all_vl,0,rad_num*sizeof(float));
	int svr_num=30;
	float max_rad=0;
	float max_svr=0;

	for(int as=0;as<neuron[0].size();as++)
	{
		Geometry*g = &neuron[0][as];
		for(int i=0;i<g->face.size();i++)
		{
			ivec3 ff=g->face[i];
			if((g->vert_val[ff.x]>0)&&(g->vert_val[ff.y]>0)&&(g->vert_val[ff.z]>0))
			{
				for(int j=0;j<3;j++)
				{
					float rr = NearestPSD(g->vert[ff.GetByID(j)]);
					if(rr>max_rad)max_rad=rr;
					float ss = g->vert_val[ff.GetByID(j)];
					if(ss>max_svr)max_svr=ss;
				}
			}
		}
	}
	
	max_rad=sqrt(max_rad);
	printf("\n\n%g %g |",max_rad,max_svr);
	


	float *hist=new float[rad_num*svr_num];
	memset(hist,0,rad_num*svr_num*sizeof(float));
	for(int as=0;as<neuron[0].size();as++)
	{
		Geometry*g = &neuron[0][as];
		for(int i=0;i<g->face.size();i++)
		{
			ivec3 ff=g->face[i];
			if((g->vert_val[ff.x]>0)&&(g->vert_val[ff.y]>0)&&(g->vert_val[ff.z]>0))
			{
//				Triangle tr = g->tr[i];
//				float s = TriangleArea(tr);
				vec3 v[3];
				float svrs[3];
				for(int ii=0;ii<3;ii++)
				{
					v[ii]=g->vert[ff.GetByID(ii)];
					svrs[ii] = g->vert_val[ff.GetByID(ii)];
				}
				svrs[1]-=svrs[0];
				svrs[2]-=svrs[0];
				v[1] -=v[0];
				v[2] -=v[0];
				float s = vec3::vect_mult(v[2],v[1]).length();

				for(int jj=0;jj<30;jj++)
				{
					
					float t1=RND01,t2=RND01;
					while(t1+t2>1){t2=RND01;t1=RND01;}
					vec3 pt1 = v[0] + v[1]*t1 + v[2]*t2;
					float ss = svrs[0] + svrs[1]*t1 + svrs[2]*t2;

					float rr = sqrt(NearestPSD(pt1));
					
					int hi = rad_num*(rr/max_rad);
					int hj = svr_num*(ss/max_svr);

					//printf("%d %d =",hi,hj);
					if(hi>=0 && hi<rad_num && hj>=0 && hj<svr_num)
					{
						hist[hi+hj*rad_num]+=s;
						all_vl[hi]+=s;
						
					}

				}
			}
		}
	}
	
	Table tbl(rad_num+1,svr_num+1);
	tbl.SetValue("SVR\\distance",0,0);
	for(int i=0;i<rad_num;i++)
		tbl.SetValue((i+1)*max_rad/rad_num,i+1,0);
	for(int i=0;i<svr_num;i++)	
		tbl.SetValue((i+1)*max_svr/svr_num,0,i+1);

	for(int i=0;i<rad_num;i++)
	if(all_vl[i])
	for(int j=0;j<svr_num;j++)
	{
		tbl.SetValue((hist[i+j*rad_num]*100)/float(all_vl[i]),i+1,j+1);
	}
	tbl.StoreToFile("hist.txt");
	
	delete[]hist;
	delete[]all_vl;
}

float h_tmp[40*40];
float max_svr = 100;
float max_rad = 1;
void CalcHist2(int psd_id)
{
	vec3 psd_c = psd[psd_id].fl.d;
	ivec ass;
	ivec trs;
	int rad_num=20;
	float *all_vl=new float[rad_num];
	memset(all_vl,0,rad_num*sizeof(float));
	int svr_num=30;
	

	float *hist=new float[rad_num*svr_num];
	memset(hist,0,rad_num*svr_num*sizeof(float));
	for(int as=0;as<neuron[0].size();as++)
	{
		Geometry*g = &neuron[0][as];
		for(int i=0;i<g->face.size();i++)
		{
			ivec3 ff = g->face[i];
			if(g->vert[ff.x].lengthSQR(psd_c)<max_rad*max_rad || g->vert[ff.y].lengthSQR(psd_c)<max_rad*max_rad || g->vert[ff.z].lengthSQR(psd_c)<max_rad*max_rad)
			if((g->vert_val[ff.x]>0)&&(g->vert_val[ff.y]>0)&&(g->vert_val[ff.z]>0))
			{
//				Triangle tr = g->tr[i];
//				float s = TriangleArea(tr);
				vec3 v[3];
				float svrs[3];
				for(int ii=0;ii<3;ii++)
				{
					v[ii]=g->vert[ff.GetByID(ii)];
					svrs[ii] = g->vert_val[ff.GetByID(ii)];
				}
				svrs[1]-=svrs[0];
				svrs[2]-=svrs[0];
				v[1] -=v[0];
				v[2] -=v[0];
				float s = vec3::vect_mult(v[2],v[1]).length();

				for(int jj=0;jj<30;jj++)
				{
					
					float t1=RND01,t2=RND01;
					while(t1+t2>1){t2=RND01;t1=RND01;}
					vec3 pt1 = v[0] + v[1]*t1 + v[2]*t2;
					float ss = svrs[0] + svrs[1]*t1 + svrs[2]*t2;

					float rr = pt1.lengthSQR(psd_c);
					//if(NearestPSD(pt1)<rr)continue;
					rr=sqrt(rr);
					
					int hi = rad_num*(rr/max_rad);
					int hj = svr_num*(ss/max_svr);

					//printf("%d %d =",hi,hj);
					if(hi>=0 && hi<rad_num && hj>=0 && hj<svr_num)
					{
						hist[hi+hj*rad_num]+=s;
						all_vl[hi]+=s;
						
					}

				}
			}
		}
	}
	memcpy(h_tmp,hist,rad_num*svr_num*sizeof(float));
	
	Table tbl(rad_num+1,svr_num+1);
	tbl.SetValue("SVR\\distance",0,0);
	for(int i=0;i<rad_num;i++)
		tbl.SetValue((i+1)*max_rad/rad_num,i+1,0);
	for(int i=0;i<svr_num;i++)	
		tbl.SetValue((i+1)*max_svr/svr_num,0,i+1);

	for(int i=0;i<rad_num;i++)
	if(all_vl[i])
	for(int j=0;j<svr_num;j++)
	{
		tbl.SetValue((hist[i+j*rad_num]*100)/float(all_vl[i]),i+1,j+1);
	}
	tbl.StoreToFile("hist"+str::ToString(psd_id)+".txt");
	
	delete[]hist;
	delete[]all_vl;
}
	

	
void CalcHist3() //summ for psd's
{
	int rad_num=20;
	int svr_num=30;

	float *hist=new float[rad_num*svr_num];
	memset(hist,0,rad_num*svr_num*sizeof(float));
	//
	for(int i=0;i<psd.size();i++)
	{
		CalcHist2(i);
		for(int j=0;j<rad_num*svr_num;j++)
			hist[j]+=h_tmp[j];
	}
	for(int j=0;j<rad_num*svr_num;j++)			hist[j]/=psd.size();
	//
	Table tbl(rad_num+1,svr_num+1);
	tbl.SetValue("SVR\\distance",0,0);
	for(int i=0;i<rad_num;i++)
		tbl.SetValue((i+1)*max_rad/rad_num,i+1,0);
	for(int i=0;i<svr_num;i++)	
		tbl.SetValue((i+1)*max_svr/svr_num,0,i+1);

	for(int i=0;i<rad_num;i++)
	for(int j=0;j<svr_num;j++)
	{
		tbl.SetValue(hist[i+j*rad_num],i+1,j+1);
	}
	tbl.StoreToFile("hist_summ.txt");
	
	delete[]hist;
}
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