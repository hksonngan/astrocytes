#include "AllDef.h"
#include "vec2.h"
#include "vec.h"
#include "Math.h"

float my_func(float& x,vec& p)//m0 s0 m1 s1 alpha
{
	float m;
	float s;
	float e=0;
	
	for(int i=0;i<2;i++)
	{
		m = p[i*2];
		s = p[1+i*2];
		if(s<=0.000001f)
			s=0.000001f;
		float al = p[4];
		if(i)al=1-al;
		e += al*exp(-(x-m)*(x-m)/(2*s*s))/(sqrt(2*PI)*s);
	}

	return e;
}
float CalcError(vec& p,vec& x_vals,vec& y_vals)
{
	float error = 0;
	for(int i=0;i<x_vals.size();i++)
	{
		float er = y_vals[i]*10 - my_func(x_vals[i],p);
		error += er*er;
	}
	return error;
}


//возвращает параметры функции, которой мы аппроксимируем ряд vals
//p_b - отрезки, ограничивающие искомые параметры
vec Approx(vec& x_vals, vec& y_vals, v2vec& p_b)
{
	float betta = 0.5;
	int pop_num = 1700;
	int best_id=0;
	vec *pop = new vec[pop_num];
	float *fit = new float[pop_num];
	for(int i=0;i<pop_num;i++)
	{
		pop[i].resize(p_b.size());
		for(int j=0;j<p_b.size();j++)
			pop[i][j] = RND01*(p_b[j].y-p_b[j].x)+p_b[j].x;
		fit[i] = CalcError(pop[i],x_vals,y_vals);
		if(fit[i]<fit[best_id])best_id=i;

		//for(int i1=0;i1<p_b.size();i1++)			printf(" %g",pop[best_id][i1]);		printf("\n");

	}

	int id[3];

	for(int iter1=0;iter1<400;iter1++)
	for(int iter=0;iter<1000;iter++)
	{
		id[0] = RNDINT(pop_num);
		do{id[1] = RNDINT(pop_num);}while(id[0]==id[1]);
		do{id[2] = RNDINT(pop_num);}while(id[0]==id[2] || id[1]==id[2]);
		
		int poor=(fit[id[0]]<fit[id[1]])?1:0;
		if(fit[id[poor]]<fit[id[2]])poor=2;

		int id1=(poor+1)%3;
		int id2=(poor+2)%3;
		
		poor=id[poor];
		id1=id[id1];
		id2=id[id2];
		

		for(int j=0;j<p_b.size();j++)
		{
			pop[poor][j] = RND01*(pop[id2][j]-pop[id1][j])+pop[id1][j];
			pop[poor][j] = clamp(p_b[j].x,p_b[j].y,pop[poor][j]+RND11*betta*(pop[id2][j]-pop[id1][j]));
		}
		fit[poor] = CalcError(pop[poor],x_vals,y_vals);
		if(fit[poor]<fit[best_id])
		{
			printf("%g\n",fit[best_id]);
			best_id=poor;
//for(int i=0;i<p_b.size();i++)		printf(" %g",pop[best_id][i]);	printf("\n");
			
		}
	}



	vec res = pop[best_id];
	delete[]pop;
	delete[]fit;
	return res;
}