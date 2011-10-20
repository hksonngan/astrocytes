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
	int *hist=new int[svr_num*er_num];
	memset(all_vl2,0,svr_num*sizeof(float));
	memset(all_vl2_num,0,svr_num*sizeof(int));
	memset(hist,0,svr_num*er_num*sizeof(int));
	int hist_num=0;
	
	Table tbl(2,svr_num+2);
	Table tbl2(er_num+2,svr_num+2);
		
	tbl.SetValue("SVR",1,0);
	tbl.SetValue("Ver/Vas",0,0);
	for(int i=0;i<svr_num;i++)	
		tbl.SetValue((i+1)*max_svr/svr_num,1,i+1);
	tbl2.SetValue("SVR\\Ver/Vas",0,0);
	
	for(int i=0;i<svr_num;i++)	
		tbl2.SetValue((i+1)*max_svr/svr_num,1,i+1);
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
			V_er += mc_CalcVolume(g0,pt,rad,pt,ins_er);
			
		}

		float svr = S_as/V_as;
		float vv = V_er/V_as;

		int hi = er_num*vv;
		int hj = svr_num*(svr/max_svr);
		if(hj>=0 && hj<svr_num)
		{
			all_vl2[hj] = (vv + (all_vl2_num[hj]*all_vl2[hj]))/(all_vl2_num[hj]+1);
			all_vl2_num[hj]++;

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
				tbl.SetValue((float)(all_vl2[j]),0,j+1);
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