#include "Draw.h"
#include "AllInc.h"
#include "AllDef.h"
#include <Math.h>
#include "CaDepo.h"
#include "globals.h"
#include "Table.h"
std::vector<ivec> CD_depo; // списки депо для каждого астроцита
ivec CD_astr;//списки астроцитов для каждого депо

vec3 GetIntirePoint(Geometry*g)
{
	vec3 pt;
	while(1)
	{
		pt.set(RND01*(g->box2.x-g->box1.x)+g->box1.x, RND01*(g->box2.y-g->box1.y)+g->box1.y, RND01*(g->box2.z-g->box1.z)+g->box1.z);
		if(g->Inside(pt))break;
		
	}
	return pt;
}

void CalcERtoSVR(int as_id,float rad)
{
	int op_num=0;
	Geometry*g = &neuron[0][as_id];
	vec3 pt;

	float max_svr=40;
	int svr_num=40,er_num=20;
	float *all_vl2=new float[svr_num];
	int *all_vl2_num=new int[svr_num];
	int *hist=new int[svr_num*er_num],svr_summ=0;
	memset(all_vl2,0,svr_num*sizeof(float));
	memset(all_vl2_num,0,svr_num*sizeof(int));
	memset(hist,0,svr_num*er_num*sizeof(int));
	int hist_num=0;
	
	Table tbl(3,svr_num+2);
	Table tbl2(er_num+2,svr_num+2);
		
	tbl.SetValue("SVR",0,0);
	tbl.SetValue("Ver/Vas",0,0);
	for(int i=0;i<svr_num;i++)	
		tbl.SetValue((i+1)*max_svr/svr_num,0,i+1);
	tbl2.SetValue("Ver/Vas",1,0);
	tbl2.SetValue("%svr",2,0);
	
	for(int i=0;i<svr_num;i++)	
		tbl2.SetValue((i+1)*max_svr/svr_num,0,i+1);
	for(int i=0;i<er_num;i++)	
		tbl2.SetValue((i+1)/(float)er_num,1+i,0);

	int iter=0;
	Geometry*g0=new Geometry();
	while(1)
	{
		pt = GetIntirePoint(g);
		g0->renull();
		GetInSphere0(g,pt,rad,g0);
		
		float V_as = mc_CalcVolume(g0,pt,rad,pt,1);
		float S_as = mc_CalcArea(g0,pt,rad);
		float V_er = 0;
		
		for(int i=0;i<CD_depo[as_id].size();i++)
		{
			Geometry* g_er = &neuron[4][CD_depo[as_id][i]];
			if(!g_er->tr.size())continue;
			g0->renull();
			GetInSphere0(g_er,pt,rad,g0);
			//printf("(%d)",g0->tr.size());
			
			bool ins_er = g_er->Inside(pt);
			V_er += g_er->color.y*mc_CalcVolume(g0,pt,rad,pt,ins_er);
			
		}

		float svr = S_as/V_as;
		float vv = V_er/V_as;

		int hi = er_num*vv;
		int hj = svr_num*(svr/max_svr);
		if(hj>=0 && hj<svr_num)
		{
			all_vl2[hj] = (vv + (all_vl2_num[hj]*all_vl2[hj]))/(all_vl2_num[hj]+1);
			all_vl2_num[hj]++;
			svr_summ++;

			if(hi>=0 && hi<er_num)
			{
				hist[hi+er_num*hj]++;
				hist_num++;
			}
		}

		iter++;
		printf("%d) v svr: %g %g\n",iter,vv,svr);
//
		if(!(iter%5))
		{
			for(int j=0;j<svr_num;j++)
			{
				tbl.SetValue((float)(all_vl2[j]),1,j+1);
				tbl.SetValue((all_vl2_num[j]/(float)svr_summ),2,j+1);
			}
			tbl.StoreToFile("results\\er_svr_astr"+str::ToString(as_id)+"_Radius"+str::ToString(int(rad*1000))+"nm.txt");
		//
			for(int i=0;i<er_num;i++)
			for(int j=0;j<svr_num;j++)
			{
				tbl2.SetValue((float)(hist[i+j*er_num])/hist_num,i+1,j+1);
			}
			tbl.StoreToFile("results\\er_svr_astr"+str::ToString(as_id)+"_Radius"+str::ToString(int(rad*1000))+"nm.txt");
			tbl2.StoreToFile("results\\HIST_er_svr_astr"+str::ToString(as_id)+"_Radius"+str::ToString(int(rad*1000))+"nm.txt");
			
			if(iter>=5000)break;
		}

	}
	
	delete[]all_vl2;
	delete[]all_vl2_num;
	delete[]hist;
	
}

float nearest_epr(vec3 pt)
{
	float res=1000;
	for(int i=0;i<neuron[4].size();i++)
	{
		Geometry* g_er = &neuron[4][i];
		if(!g_er->tr.size())continue;
		float ll = pt.lengthSQR(g_er->vert[GetNearest(g_er->vert,pt)]);
		if(res>ll)res=ll;
	}
	return sqrt(res);
}

void CalcER()
{
		//*'
printf("\n");
for(int i=0;i<psd.size();i++)
		printf("\n%g ",nearest_epr(psd[i].fl.d));
	return;// */
float d0 = 0.01f;
float d1 = 0.6f;
float d_step = 0.01f;
int it_num = int((d1-d0)/d_step);
int opers_left,opers_total;
double start = glfwGetTime ( );

	int psd_num = psd.size();
	opers_left =opers_total= it_num*psd_num;
	float *Vas=new float[it_num*psd_num];
	float *Ver=new float[it_num*psd_num];
	
	memset(Vas,0,it_num*sizeof(float)*psd_num);
	memset(Ver,0,it_num*sizeof(float)*psd_num);
	float *Vas1=new float[it_num];
	float *Ver1=new float[it_num];
	
	memset(Vas1,0,it_num*sizeof(float));
	memset(Ver1,0,it_num*sizeof(float));
	
	float cur_d;
	Geometry *gg0 = new Geometry(),*gg = new Geometry();
	Geometry*gs = new Geometry();

	for(int psd_id=0;psd_id<psd_num;psd_id++)
	{
		float offset=nearest_epr(psd[psd_id].fl.d);

		cur_d=d0+offset;

		gs->renull();
		AddSphere(gs,d1+d_step+offset,40,40);
		mat34 mt(12,vec3(1).normalized());
		gs->Transform(mt);
		gs->Move(psd[psd_id].fl.d);
		gs->RebuildTrBB();
		gs->RebuildTrBB2();
		gg0->renull();
		for(int as=0;as<2;as++)
		{
			GetSection(gs,&neuron[0][as],gg0,1,0);
			GetSection(&neuron[0][as],gs,gg0,1,0);
		}
		gg0->RebuildTrBB();
		gg0->RebuildTrBB2();
			
		for(int i=0;i<it_num;i++)
		{
			
			gg->renull();
			gs->renull();
			AddSphere(gs,cur_d,40,40);
			mat34 mt(12,vec3(1).normalized());
			gs->Transform(mt);
			gs->Move(psd[psd_id].fl.d);
			gs->RebuildTrBB();
			gs->RebuildTrBB2();

			GetSection(gs,gg0,gg,1,0,0);
			float ss = gg->CalcArea();

			float ss1=0;
			for(int as_id=0;as_id<2;as_id++)
			for(int ii=0;ii<CD_depo[as_id].size();ii++)
			{
				Geometry* g_er = &neuron[4][CD_depo[as_id][ii]];
				if(!g_er->tr.size())continue;
				gg->renull();
				GetSection(gs,g_er,gg,1,0,0);
				ss1 += gg->CalcArea()*g_er->color.y;
				
			}
					
			opers_left--;
		

			Vas1[i] = ss/(4*PI*cur_d*cur_d);
			Ver1[i] = ss1/(4*PI*cur_d*cur_d);
			Vas[i+psd_id*it_num] = Vas1[i];
			Ver[i+psd_id*it_num] = Ver1[i];

			cur_d += d_step;
			int sc=((opers_left)*(glfwGetTime ( )-start)/(opers_total-opers_left))/60;
						
			//printf("%dpsd	%dit	%d%	%d:%d\n",psd_id,opers_left,(opers_left*100)/opers_total,sc/60,sc%60);
			printf("%dpsd	%dit	%g %g\n",psd_id,opers_left,ss,ss1);
				
			
		}

	}

				
	//}
//
	Table tbl1(it_num+1,1+3*psd_num);
	tbl1.SetValue("Distance",0,0);
	for(int i=0;i<psd_num;i++)
	{
		tbl1.SetValue("%Vas"+str::ToString(i),0,i+1);
		tbl1.SetValue("%Ver"+str::ToString(i),0,i+1+psd_num);
		tbl1.SetValue("Ver/Vas"+str::ToString(i),0,i+1+psd_num*2);
	}
	
	{
		float cur_d = d0;
		for(int i=0;i<it_num;i++)
		{
			tbl1.SetValue(str::ToString(int((cur_d+0.0001f)*1000))+"nm",i+1,0);
			cur_d += d_step;		
		}
	}
	for(int i=0;i<psd_num;i++)
	for(int j=0;j<it_num;j++)
	{
		tbl1.SetValue(Vas[j+i*it_num],j+1,i+1);
		tbl1.SetValue(Ver[j+i*it_num],j+1,i+1+psd_num);
		if(Vas[j+i*it_num])tbl1.SetValue(Ver[j+i*it_num]/Vas[j+i*it_num],j+1,i+1+psd_num*2);
		
	}
	
	tbl1.StoreToFile("results\\ER_central.txt");
	
	
//
			
		
	delete gg;
	delete gg0;
	delete gs;
	delete[]Vas;
	delete[]Ver;
	delete[]Vas1;
	delete[]Ver1;

}

//считает процент поверхности на сфере внутри астроцитов (perS_astr) и EPR (perS_epr)
void CalcMonteEPR_as(vec3 c,float rad,float& perS_astr,float& perS_epr)
{
	int npp=3000;
	int n_as=0,n_epr=0;

	for(int i=0;i<npp;i++)
	{
		vec3 pt;
		do{pt.set(RND11,RND11,RND11);}while(pt.lengthSQR()>1);
		pt.normalize();
		pt = c + pt*rad;
	
		for(int as_id=0;as_id<2;as_id++)
			if(neuron[0][as_id].Inside(pt))
			{
				n_as++;
				for(int ii=0;ii<CD_depo[as_id].size();ii++)
				{
					Geometry* g_er = &neuron[4][CD_depo[as_id][ii]];
					if(!g_er->tr.size())continue;
					if(g_er->Inside(pt)){n_epr++;break;}
					
					
				}
				break;
			}
		//if(i%100==0)printf("\n%g %g\n",n_as/(float)i,n_epr/(float)i);
	}

	
	perS_astr = n_as/(float)npp;
	perS_astr = n_epr/(float)npp;
}


void CalcER1()
{
	/*
for(int i=0;i<psd.size();i++)
		printf("\n%g ",nearest_epr(psd[i].fl.d));
	return;*/
	
float d0 = 0.01f;
float d1 = 0.5f;
float d_step = 0.04f;
int it_num = int((d1-d0)/d_step);
int opers_left,opers_total;
double start = glfwGetTime ( );

	int psd_num = 3;//psd.size();
	opers_left =opers_total= it_num*psd_num;
	float *Vas=new float[it_num*psd_num];
	float *Ver=new float[it_num*psd_num];
	
	memset(Vas,0,it_num*sizeof(float)*psd_num);
	memset(Ver,0,it_num*sizeof(float)*psd_num);
	float *Vas1=new float[it_num];
	float *Ver1=new float[it_num];
	
	memset(Vas1,0,it_num*sizeof(float));
	memset(Ver1,0,it_num*sizeof(float));
	
	float cur_d;

	for(int psd_id=0;psd_id<psd_num;psd_id++)
	{
		float offset=nearest_epr(psd[psd_id].fl.d);
		cur_d=d0;

		for(int i=0;i<it_num;i++)
		{
			float perS_astr,perS_epr;
			CalcMonteEPR_as(psd[psd_id].fl.d,cur_d+offset,perS_astr,perS_epr);
			

					
			opers_left--;
		

			Vas1[i] = perS_astr;
			Ver1[i] = perS_epr;
			Vas[i+psd_id*it_num] = Vas1[i];
			Ver[i+psd_id*it_num] = Ver1[i];

			cur_d += d_step;
			int sc=((opers_left)*(glfwGetTime ( )-start)/(opers_total-opers_left))/60;
						
			printf("%dpsd	%dit	%d%	%d:%d %g\n",psd_id,opers_left,(opers_left*100)/opers_total,sc/60,sc%60,perS_astr);
				
			
		}

		
	}

				
	//}
//
	Table tbl1(it_num+1,1+3*psd_num);
	tbl1.SetValue("Distance",0,0);
	for(int i=0;i<psd_num;i++)
	{
		tbl1.SetValue("%Vas"+str::ToString(i),0,i+1);
		tbl1.SetValue("%Ver"+str::ToString(i),0,i+1+psd_num);
		tbl1.SetValue("Ver/Vas"+str::ToString(i),0,i+1+psd_num*2);
	}
	
	{
		float cur_d = d0;
		for(int i=0;i<it_num;i++)
		{
			tbl1.SetValue(str::ToString(int((cur_d+0.0001f)*1000))+"nm",i+1,0);
			cur_d += d_step;		
		}
	}
	for(int i=0;i<psd_num;i++)
	for(int j=0;j<it_num;j++)
	{
		tbl1.SetValue(Vas[j+i*it_num],j+1,i+1);
		tbl1.SetValue(Ver[i+i*it_num],j+1,i+1+psd_num);
		if(Vas[i+i*it_num])tbl1.SetValue(Ver[i+i*it_num]/Vas[i+i*it_num],j+1,i+1+psd_num*2);
		
	}
	
	tbl1.StoreToFile("results\\ER_central.txt");
	
	
//
			
		
	delete[]Vas;
	delete[]Ver;
	delete[]Vas1;
	delete[]Ver1;

}
