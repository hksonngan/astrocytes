#include "globals.h"
#include "table.h"

#define RAD_NUM 60
#define SVR_NUM 60

extern int painting_rad_nm;

float NearestPSD(vec3 pt,int& psd_id)
{
	float minl;
	for(int i=0;i<psd.size();i++)
	{
		float ll = pt.lengthSQR(psd[i].fl.d);
		if(ll<minl || !i){minl=ll;psd_id=i;}
	}
	return minl;
}

float NearestPSD_sp(vec3 pt,int psd_id)
{
	float minl;
	Geometry*g = &neuron[1][psd_id];//psd[psd_id].target;//
	for(int j=0;j<g->vert.size();j++)
	{
		float ll = pt.lengthSQR(g->vert[j]);
		if(ll<minl || !j){minl=ll;}
	}
	return minl;
}
void CalcSVRHist()
{
	int svr_num=400,psd_id;
	float max_svr=40,max_rad=120.5;
	float *all_vl2=new float[svr_num];
	memset(all_vl2,0,svr_num*sizeof(float));

	for(int as=0;as<neuron[0].size();as++)
	{printf("+");
		Geometry*g = &neuron[0][as];
		for(int i=0;i<g->face.size();i++)
		{
			ivec3 ff=g->face[i];
			if((g->vert_val[ff.x]>0)&&(g->vert_val[ff.y]>0)&&(g->vert_val[ff.z]>0))
			if(NearestPSD(g->vert[ff.x],psd_id)<=max_rad || NearestPSD(g->vert[ff.y],psd_id)<=max_rad || NearestPSD(g->vert[ff.z],psd_id)<=max_rad)
			{
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

				for(int jj=0;jj<500;jj++)
				{
					
					float t1=RND01,t2=RND01;
					while(t1+t2>1){t2=RND01;t1=RND01;}
					vec3 pt1 = v[0] + v[1]*t1 + v[2]*t2;
					float ss = svrs[0] + svrs[1]*t1 + svrs[2]*t2;
					float rr = sqrt(NearestPSD(pt1,psd_id));					
					int hj = svr_num*(ss/max_svr);
					if(rr<max_rad)
					if(hj>=0 && hj<svr_num)
					{
						all_vl2[hj]+=s;
					}
					

				}
			}
		}
		
	}
	
	
	Table tbl(2,svr_num+2);
	
	tbl.SetValue("SVR",1,0);
	tbl.SetValue("R="+str::ToString(painting_rad_nm),0,0);
	for(int i=0;i<svr_num;i++)	
		tbl.SetValue((i+1)*max_svr/svr_num,1,i+1);

	float svr_summ=0;
	for(int j=0;j<svr_num;j++)
		svr_summ+=all_vl2[j];

	for(int j=0;j<svr_num;j++)
	{
		tbl.SetValue((float)(all_vl2[j]/svr_summ),0,j+1);
	}
	tbl.StoreToFile("results\\hist_svr_"+str::ToString(painting_rad_nm)+".txt");

	delete[]all_vl2;
}
void CalcHist1()
{
	
	int psd_id;
	ivec ass;
	ivec trs;
	int rad_num=RAD_NUM;
	float *all_vl=new float[rad_num];
	float *all_vl2=new float[SVR_NUM];
	
	float svr_mid[RAD_NUM];
	int svr_n[RAD_NUM];

	float *svr_mid1=new float[psd.size()*RAD_NUM];
	int *svr_n1=new int[psd.size()*RAD_NUM];

	memset(all_vl,0,rad_num*sizeof(float));
	memset(all_vl2,0,SVR_NUM*sizeof(float));
	memset(svr_mid,0,rad_num*sizeof(float));
	memset(svr_n,0,rad_num*sizeof(int));
	memset(svr_mid1,0,rad_num*sizeof(float)*psd.size());
	memset(svr_n1,0,rad_num*sizeof(int)*psd.size());
	
	int svr_num=SVR_NUM;
	float max_rad=0;
	float max_svr=60;
/*
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
					float rr = NearestPSD(g->vert[ff.GetByID(j)],psd_id);
					if(rr>max_rad)max_rad=rr;
					float ss = g->vert_val[ff.GetByID(j)];
					if(ss>max_svr)max_svr=ss;
				}
			}
		}
	}
	*/
	max_rad = 0.6;//sqrt(max_rad);
	printf("\n\n%g %g |",max_rad,max_svr);
	


	float *hist=new float[rad_num*svr_num];
	memset(hist,0,rad_num*svr_num*sizeof(float));
	for(int as=0;as<neuron[0].size();as++)
	{printf("+");
		Geometry*g = &neuron[0][as];
		for(int i=0;i<g->face.size();i++)
		{
			ivec3 ff=g->face[i];
			if((g->vert_val[ff.x]>0)&&(g->vert_val[ff.y]>0)&&(g->vert_val[ff.z]>0))
			if(NearestPSD(g->vert[ff.x],psd_id)<=max_rad || NearestPSD(g->vert[ff.y],psd_id)<=max_rad || NearestPSD(g->vert[ff.z],psd_id)<=max_rad)
			{
				int pID = psd_id;
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

				for(int jj=0;jj<160;jj++)
				{
					
					float t1=RND01,t2=RND01;
					while(t1+t2>1){t2=RND01;t1=RND01;}
					vec3 pt1 = v[0] + v[1]*t1 + v[2]*t2;
					float ss = svrs[0] + svrs[1]*t1 + svrs[2]*t2;
					

					float rr = sqrt(NearestPSD_sp(pt1,pID));
					
					int hi = rad_num*(rr/max_rad);
					int hj = 0;//svr_num*(ss/max_svr);

					//printf("%d %d =",hi,hj);
					if(hi>=0 && hi<rad_num && hj>=0 && hj<svr_num)
					{
						//hist[hi+hj*rad_num]+=s;
						for(int k=0;k<svr_num;k++)
						{
							hist[hi] = ((ss>k+1)*100.0f + all_vl[hi]*hist[hi])/(all_vl[hi]+1);
							all_vl[hi]++;
							
						}
						//all_vl[hi]+=s;
						all_vl2[hj]+=s;

						//svr_mid[hi] = (ss + svr_n[hi]*svr_mid[hi])/(svr_n[hi]+1);
						svr_mid[hi] = ((ss>20)*100.0f + svr_n[hi]*svr_mid[hi])/(svr_n[hi]+1);
						svr_n[hi]++;

					}
					
					if(hj>=0 && hj<svr_num)
					for(psd_id=0;psd_id<psd.size();psd_id++)
					{
						float rr1 = psd[psd_id].fl.d.length(pt1);
						
						int hi1 = rad_num*(rr1/max_rad);
						
						//printf("%d %d =",hi,hj);
						if(hi1>=0 && hi1<rad_num)
						{
							
							float *tmpf = svr_mid1+hi+psd_id*rad_num;
							int *tmpi = svr_n1+hi+psd_id*rad_num;

							*tmpf = (ss + *tmpi * *tmpf)/(*tmpi+1);
							*tmpi++;
						}
					}

				}
			}
		}
		
	}
	
	
	Table tbl(rad_num+2,svr_num+2);
	tbl.SetValue("SVR\\distance",0,0);
	for(int i=0;i<rad_num;i++)
		tbl.SetValue((i+1)*max_rad/rad_num,i+1,0);
	for(int i=0;i<svr_num;i++)	
		tbl.SetValue((i+1)*max_svr/svr_num,0,i+1);

	for(int i=0;i<rad_num;i++)
	if(all_vl[i])
	for(int j=0;j<svr_num;j++)
	{
		tbl.SetValue((float)((hist[i+j*rad_num]*100)/(all_vl[i])),i+1,j+1);
	}
	tbl.StoreToFile("results\\hist_"+str::ToString(painting_rad_nm)+".txt");
	
	for(int j=0;j<svr_num;j++)
	for(int i=0;i<rad_num;i++)
		tbl.SetValue("",i+1,j+1);

	float svr_summ=0;
	for(int j=0;j<svr_num;j++)
		svr_summ+=all_vl2[j];

	for(int j=0;j<svr_num;j++)
	//if(all_vl2[j])
	{
		for(int i=0;i<rad_num;i++)
		{
			tbl.SetValue((float)((hist[i+j*rad_num])),i+1,j+1);
		}
		//tbl.SetValue((float)(all_vl2[j]/svr_summ),rad_num+1,j+1);
	}
	tbl.StoreToFile("results\\hist2_"+str::ToString(painting_rad_nm)+".txt");

//
	Table tbl1(rad_num+1,2+psd.size());
	tbl1.SetValue("Distance",0,0);
	tbl1.SetValue("avarage SVR",0,1);
	for(int i=0;i<psd.size();i++)	
		tbl1.SetValue(i+1,0,i+2);
	for(int i=0;i<rad_num;i++)
		tbl1.SetValue((i+1)*max_rad/rad_num,i+1,0);
	
	for(int i=0;i<rad_num;i++)
	{
		tbl1.SetValue((float)svr_mid[i],i+1,1);
		for(int j=0;j<psd.size();j++)
		{
//			printf("%d %d |",i,j);
			tbl1.SetValue((float)svr_mid1[i+j*rad_num],i+1,2+j);
		}
	}
	
	tbl1.StoreToFile("results\\graphic1_"+str::ToString(painting_rad_nm)+".txt");
printf("g");
//
	delete[]hist;
	printf("a");
	delete[]all_vl;
	printf("b");
	delete[]all_vl2;
	printf("c");
	delete[]svr_n1;
	printf("d");
//delete[]svr_mid1;
	printf("e");

}


float h_tmp[SVR_NUM*RAD_NUM];
float max_svr = 100;
float max_rad = 1;
void CalcHist2(int psd_id)
{
	int psd_id1;
	vec3 psd_c = psd[psd_id].fl.d;
	ivec ass;
	ivec trs;
	int rad_num=RAD_NUM;
	float *all_vl=new float[rad_num];
	memset(all_vl,0,rad_num*sizeof(float));
	int svr_num=SVR_NUM;
	

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
					if(NearestPSD(pt1,psd_id1)<rr)continue;
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
	tbl.StoreToFile("results\\hist"+str::ToString(psd_id)+".txt");
	
	delete[]hist;
	delete[]all_vl;
}
	

	
void CalcHist3() //summ for psd's
{
	int rad_num=RAD_NUM;
	int svr_num=SVR_NUM;

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
	tbl.StoreToFile("results\\hist_summ.txt");
	
	delete[]hist;
}