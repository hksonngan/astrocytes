#include "globals.h"
#include "table.h"

#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range2d.h"
#include "tbb/tick_count.h"
#include "tbb/mutex.h"

using namespace tbb;


float TriangleAreaInSphere(Triangle& tr,vec3& c,float& r)
{
	float s = TriangleArea(tr);
	vec3 v[3];
	for(int i=0;i<3;i++)
		v[i]=tr.v[i]-c;
	v[1] -=v[0];
	v[2] -=v[0];
	int pp=0,npp=1000;
	for(int i=0;i<npp;i++)
	{
		float t1=RND01,t2=RND01;
		while(t1+t2>1){t2=RND01;t1=RND01;}
		vec3 pt = v[0] + v[1]*t1 + v[2]*t2;

		if(pt.lengthSQR()<r*r)pp++;

	}
	///printf("(%d)",pp);
	return s*(pp/float(npp));

}
float mc_CalcArea(Geometry*g,vec3 c,float r)
{
	
	float s=0;
	
	for(int i=0;i<g->tr.size();i++)
	{
		if(g->tr[i].v[0].lengthSQR(c)>r*r || g->tr[i].v[1].lengthSQR(c)>r*r || g->tr[i].v[2].lengthSQR(c)>r*r)
			s += TriangleAreaInSphere(g->tr[i],c,r);
		else
			s += TriangleArea(g->tr[i]);
	}

	return s/2;
}

float mc_CalcVolume(Geometry*g,vec3 c,float r, vec3 c1,bool ins)
{
	float vol = 4*PI*r*r*r/3;
	int pp=0,npp=5000;
	for(int i=0;i<npp;i++)
	{
		vec3 pt(RND11,RND11,RND11);
		while(pt.lengthSQR()>1)pt.set(RND11,RND11,RND11);
		pt = c + pt*r;
		bool c_ins = g->CrossLineTimes(c1,pt)&1;
		if((ins&&!c_ins)||(!ins&&c_ins))pp++;
		//if(i)if(!(i%1000))printf("\n%g ",vol*(pp/float(i)));
	}

	return vol*(pp/float(npp));
}