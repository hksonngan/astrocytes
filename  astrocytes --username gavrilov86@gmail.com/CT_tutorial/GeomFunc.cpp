#include "GeomFunc.h"
#include "AllDef.h"
#include "vec.h"

#include <math.h>

vec3 last_triangles_cross;

void getnorm (float a[3],float b[3],float c[3],float *n)
{
	n[0]=(b[1]-a[1])*(c[2]-a[2])-(b[2]-a[2])*(c[1]-a[1]);
	n[1]=(c[0]-a[0])*(b[2]-a[2])-(b[0]-a[0])*(c[2]-a[2]);
	n[2]=(b[0]-a[0])*(c[1]-a[1])-(c[0]-a[0])*(b[1]-a[1]);
}

void ToSphereCoord(vec3 center, Lines& lines)
{
	for(int i=0;i<lines.size();i++)
		for(int j=0;j<2;j++)
		{
			vec3* vv = lines[i].v+j;
			vec3 v = *vv-center;
			if(!v.x)v.x=0.00001f;
			float l = sqrt(v.x*v.x + v.y*v.y)+0.00001f;
			
			vv->x = atan(v.y/v.x);
			if(v.x<0) vv->x += PI*sgn(v.y);
			vv->y = atan(v.z/l);
			vv->z = 0;

		}

}
Triangle::Triangle(const Triangle& tr)
{
	v[0]=tr.v[0];
	v[1]=tr.v[1];
	v[2]=tr.v[2];
	norm=tr.norm;
}
//точка t вне треугольника tr?
bool OutsideTriangle3d(vec3 t,Triangle& tr)
{
	
	vec3 t0 = (tr.v[0]+tr.v[1]+tr.v[2])/3.0f , nv=t0+tr.norm,f;

	getnorm(&tr.v[0].x,&tr.v[1].x,&nv.x,&f.x);	
	if((vec3::dot(f,(t-nv)))*(vec3::dot(f,(t0-nv)))<0)return true;
	
	getnorm(&tr.v[0].x,&tr.v[2].x,&nv.x,&f.x);	
	if((vec3::dot(f,(t-nv)))*(vec3::dot(f,(t0-nv)))<0)return true;

	getnorm(&tr.v[1].x,&tr.v[2].x,&nv.x,&f.x);
	if((vec3::dot(f,(t-nv)))*(vec3::dot(f,(t0-nv)))<0)return true;

	return false;
}
//void AddPoint(vec3);
//пересечение отрезка с плоскостью
bool CrossFlatLine(vec3 a,vec3 b,vec3 d,vec3 n,vec3*x0)
{
	float t=vec3::dot((b-a),n);
	if(t==0)return 0;
	t=(vec3::dot(d-a,n))/t;
	if(t<0||t>1)return 0;
	*x0=a+(b-a)*t;
	last_triangles_cross = *x0;
	//AddPoint(*x0);
	return 1;

}

bool CrossTriangles(Triangle& t1,Triangle& t2)
{
	vec3 x0;
	if(CrossFlatLine(t2.v[0],t2.v[1], t1.v[0],t1.norm,&x0))if(!OutsideTriangle3d(x0,t1))return true;
	if(CrossFlatLine(t2.v[0],t2.v[2], t1.v[0],t1.norm,&x0))if(!OutsideTriangle3d(x0,t1))return true;
	if(CrossFlatLine(t2.v[1],t2.v[2], t1.v[0],t1.norm,&x0))if(!OutsideTriangle3d(x0,t1))return true;

	if(CrossFlatLine(t1.v[0],t1.v[1], t2.v[0],t2.norm,&x0))if(!OutsideTriangle3d(x0,t2))return true;
	if(CrossFlatLine(t1.v[0],t1.v[2], t2.v[0],t2.norm,&x0))if(!OutsideTriangle3d(x0,t2))return true;
	if(CrossFlatLine(t1.v[1],t1.v[2], t2.v[0],t2.norm,&x0))if(!OutsideTriangle3d(x0,t2))return true;

	return false;

}
//res_conf[4]: 
//0-номер ребра треугольника, на котором лежит 1-ая вершина отрезка
//1-номер этого треугольника
//2-номер ребра треугольника, на котором лежит 2-ая вершина отрезка
//3-номер этого треугольника

bool CrossTriangles(Triangle& t1,Triangle& t2,Line*res,char* res_conf)
{
	vec3 x0;
	int id=0;
	
	if(CrossFlatLine(t2.v[0],t2.v[1], t1.v[0],t1.norm,&x0))if(!OutsideTriangle3d(x0,t1))
		{res->v[id] = x0;res_conf[0]=0;res_conf[1]=1;id++;}
	if(CrossFlatLine(t2.v[0],t2.v[2], t1.v[0],t1.norm,&x0))if(!OutsideTriangle3d(x0,t1))
		{res->v[id] = x0;res_conf[id*2]=2;res_conf[id*2+1]=1;id++;if(id==2)return true;}
	if(CrossFlatLine(t2.v[1],t2.v[2], t1.v[0],t1.norm,&x0))if(!OutsideTriangle3d(x0,t1))
		{res->v[id] = x0;res_conf[id*2]=1;res_conf[id*2+1]=1;id++;if(id==2)return true;}

	if(CrossFlatLine(t1.v[0],t1.v[1], t2.v[0],t2.norm,&x0))if(!OutsideTriangle3d(x0,t2))
		{res->v[id] = x0;res_conf[id*2]=0;res_conf[id*2+1]=0;id++;if(id==2)return true;}
	if(CrossFlatLine(t1.v[0],t1.v[2], t2.v[0],t2.norm,&x0))if(!OutsideTriangle3d(x0,t2))
		{res->v[id] = x0;res_conf[id*2]=2;res_conf[id*2+1]=0;id++;if(id==2)return true;}
	if(CrossFlatLine(t1.v[1],t1.v[2], t2.v[0],t2.norm,&x0))if(!OutsideTriangle3d(x0,t2))
		{res->v[id] = x0;res_conf[id*2]=1;res_conf[id*2+1]=0;id++;if(id==2)return true;}

	return false;

}
/*
bool CrossTriangles(Triangle& a,Triangle& b)
{
	return CrossTriangles(a.v[0],a.v[1],a.v[2],b.v[0],b.v[1],b.v[2],a.norm,b.norm);
}
*/
bool CrossTrianglesSet(Triangles& a,Triangles& b)
{
	int i,j,n=a.size(),m=b.size();
	for(i=0;i<n;i++)
		for(j=0;j<m;j++)
			if(CrossTriangles(a[i],b[j]))return true;
	return false;
}


void Triangle::CalcNorm()
{
	getnorm(&v[0].x,&v[1].x,&v[2].x,&norm.x);
	norm.normalize();
	
}

bool FlatCrossTriangle(Triangle& t1,flat&fl ,Line*res)
{
	vec3 x0;
	int id=0;

	if(CrossFlatLine(t1.v[0],t1.v[1], fl.d,fl.n,&x0))
		{res->v[id] = x0;id++;}
	if(CrossFlatLine(t1.v[0],t1.v[2], fl.d,fl.n,&x0))
		{res->v[id] = x0;id++;if(id==2)return true;}
	if(CrossFlatLine(t1.v[1],t1.v[2], fl.d,fl.n,&x0))
		{res->v[id] = x0;id++;if(id==2)return true;}

	return false;

}
// vx должно быть перпендикулярно vy
void ProectOnFlat(Lines& lines,vec3 d,vec3 vx,vec3 vy,Lines2& res)
{
	for(int i=0;i<lines.size();i++)
	{
		Line2 l2;
		for(int j=0;j<2;j++)
		{
			vec3 tmpv = lines[i].v[j]-d;
			l2.v[j].set( vec3::dot(tmpv,vx) , vec3::dot(tmpv,vy) );
		}
		res.push_back(l2);
	}

}
bool fill_gaps_error=0;
void FillCircleSegment(float r,Line2 ll,Lines2&lines,bool inside_circle)
{
	Lines2 tmpll;
	tmpll.push_back(ll);
	int id=0;
	vec2 tmpv=ll.v[1]-ll.v[0];
	bool bb = (vec2::vect_mult(tmpv,ll.v[0])<0);
	if(inside_circle)bb=!bb;
	if(bb)
	{
		vec2 nt = (ll.v[0]+ll.v[1])*(-1);
		nt.normalize();
		nt*=r;
		tmpll.push_back(Line2(nt,ll.v[1]));
		tmpll.push_back(Line2(ll.v[0],nt));
		id++;
	}
	while(id<tmpll.size())
	{
		vec2 a=tmpll[id].v[0];
		vec2 b=tmpll[id].v[1];
		if(a.lengthSQR(b)<r*r/100)
		{
			lines.push_back(tmpll[id]);
		}else
		{
			vec2 nt = (a+b);
			nt.normalize();
			nt*=r;
			tmpll.push_back(Line2(a,nt));
			tmpll.push_back(Line2(nt,b));
		}
		id++;
		if(id>1000)break;
	}
	if(id>1000){printf("!");fill_gaps_error=1;}

}

void FillCircleGaps(float r,Lines2& lines, iv2vec* crossed,Lines2&contour,bool inside_circle)
{
	v2vec vr;
	vec ang;
	bvec to_surf;
	if(crossed->size()<2)return;
	for(int i=0;i<crossed->size();i++)
	{
		ivec2 cc = (*crossed)[i];
		vec2 tmpv2 = lines[cc.x].v[cc.y];
		vr.push_back(tmpv2);
		ang.push_back(tmpv2.GetAngle());
		to_surf.push_back(cc.y);
	}

	for(int i=0;i<vr.size()-1;i++)
		for(int j=i+1;j<vr.size();j++)
		if(ang[i]>ang[j])
		{
			swap(vr[i],vr[j]);
			swap(ang[i],ang[j]);
			swap(to_surf[i],to_surf[j]);
		}

	int i=0;//!to_surf[0];
	while(i<vr.size())
	{
		int i1=i;
		int i2=(i+1)%vr.size();
		if(!inside_circle)swap(i1,i2);
		if(to_surf[i1] && !to_surf[i2])
		{
			
			//lines.push_back(Line2(vr[i1],vr[i2]));
			FillCircleSegment(r,Line2(vr[i1],vr[i2]),contour,inside_circle);
			//i++;

		}else if(to_surf[i1] || !to_surf[i2]){printf("C");fill_gaps_error=1;}
		i++;
	}

}
// сечение кругом. в crossed_inside и crossed_outside записываются (если не переданы нулевые указатели) номера (в результирующих массивах lines_inside и lines_outside) пересечённых кругом отрезков
// x-это номер, а y-сторона отрезка.
void CutByCircle(Lines2& lines, float r,Lines2* lines_inside,Lines2* lines_outside,iv2vec*crossed_inside,iv2vec*crossed_outside)
{
	for(int i=0;i<lines.size();i++)
	{
		vec2 v0=lines[i].v[0];
		vec2 v1=lines[i].v[1];
		vec2 v=v1-v0;
		float a = v.x*v.x+v.y*v.y;
		float b = 2*(v0.x*v.x+v0.y*v.y);
		float c = v0.x*v0.x+v0.y*v0.y-r*r;
		float d = 4*a*c-b*b;
		if(d>=0)
		{
			if(lines_outside)lines_outside->push_back(lines[i]);
		}else 
		{
			d = sqrt(-d);
			float t1 = (-b+d)/(2*a);
			float t2 = (-b-d)/(2*a);
			bool was_swap=0;
			if(t1>t2){swap(t1,t2);was_swap=1;}
			if(t1>1 || t2<0)
				{if(lines_outside)lines_outside->push_back(lines[i]);}
			else
			{
				if(t1<=0 && t2>=1)
					{if(lines_inside)lines_inside->push_back(lines[i]);}
				else
				{
					if(t1>=0)
					{
						if(lines_outside)
						{
							if(crossed_outside)crossed_outside->push_back(ivec2(lines_outside->size(),was_swap));
							lines_outside->push_back(Line2(v0,v0+v*t1));
						}
						
					}else t1=0;
					if(t2<=1)
					{
						if(lines_outside)
						{
							if(crossed_outside)crossed_outside->push_back(ivec2(lines_outside->size(),!was_swap));
							lines_outside->push_back(Line2(v0+v*t2,v1));
						}
					}else t2=1;

					if(lines_inside)
					{
						
						if(crossed_inside)
						{
							if(t1!=0)crossed_inside->push_back(ivec2(lines_inside->size(),!was_swap));
							if(t2!=1)crossed_inside->push_back(ivec2(lines_inside->size(),was_swap));
						}
						
						lines_inside->push_back(Line2(v0+v*t1,v0+v*t2));
					}
				}

			}
		}
	}
	
}

void CutByX(Lines2&lines,float x0,Lines2* lines_less,Lines2* lines_grater)
{
	for(int i=0;i<lines.size();i++)
	{
		Line2 ll = lines[i];
		if(lines_less)
			if(ll.v[0].x<=x0 && ll.v[1].x<=x0)
			{
				lines_less->push_back(ll);
				continue;
			}
		if(lines_grater)
			if(ll.v[0].x>=x0 && ll.v[1].x>=x0)
			{
				lines_grater->push_back(ll);
				continue;
			}
		//if(ll.v[0].x>ll.v[1].x)			swap(ll.v[0],ll.v[1]);
		float y0=ll.v[0].y + (ll.v[1].y-ll.v[0].y)*(x0-ll.v[0].x)/(ll.v[1].x-ll.v[0].x);
		if(lines_less)
			lines_less->push_back(Line2(ll.v[0],vec2(x0,y0)));
		if(lines_grater)
			lines_grater->push_back(Line2(vec2(x0,y0),ll.v[1]));

		

	}

}

void CutByX(Lines2&lines,float x0)
{
	for(int i=0;i<lines.size();i++)
	{
		Line2 ll = lines[i];
		if((ll.v[0].x>x0 && ll.v[1].x<x0)||(ll.v[0].x<x0 && ll.v[1].x>x0))
		{
			float y0=ll.v[0].y + (ll.v[1].y-ll.v[0].y)*(x0-ll.v[0].x)/(ll.v[1].x-ll.v[0].x);
			lines[i].v[0]=ll.v[0];
			lines[i].v[1].set(x0,y0);
			lines.push_back(Line2(vec2(x0,y0),ll.v[1]));
		}

		

	}

}
void MoveLinesX(Lines2&lines,float dx)
{
	for(int i=0;i<lines.size();i++)
		for(int j=0;j<2;j++)
			lines[i].v[j].x += dx;
}

//возвращает площадь поверхности вращения для контура
float RotationSurfaceArea(Lines2& lines)
{
	float res=0;
	for(int i=0;i<lines.size();i++)
	{
		float x0 = lines[i].v[0].x;
		float x1 = lines[i].v[1].x;
		float ll = lines[i].v[0].length(lines[i].v[1]);
		float ss = abs(x1+x0)*ll;

		res += ss;
		
	}
	return res*PI;
}

//возвращает объём фигуры вращения области внутри контура 
float RotationSurfaceVolume(Lines2& lines)
{
	float res=0;
	for(int i=0;i<lines.size();i++)
	{
		float x0 = lines[i].v[0].x;
		float x1 = lines[i].v[1].x;
		float dx = x1-x0;
		float h = (lines[i].v[0].y-lines[i].v[1].y);

		res += ((x0<0 || x1<0)?1:-1)*(x0*x0+dx*dx/3+dx*x0)*h;

		
	}
	
	return res*PI;
}