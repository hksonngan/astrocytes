#ifndef GEOM_FUNC
#define GEOM_FUNC

#include "vec.h"
#include "vec2.h"
#include "vec3.h"
#include <stdlib.h>
#include <vector>




struct Triangle
{
	vec3 v[3];
	vec3 norm;

	Triangle(){};
	Triangle(const Triangle& tr);

	void Draw();
	void CalcNorm();
};
struct Line
{
	vec3 v[2];
	Line(){}
	Line(vec3 a,vec3 b){v[0]=a;v[1]=b;}
};
struct Line2
{
	vec2 v[2];
	Line2(){}
	Line2(vec2 a,vec2 b){v[0]=a;v[1]=b;}
};
typedef std::vector<Triangle> Triangles;
typedef std::vector<Triangle*> ptr_Triangles;
typedef std::vector<Line> Lines;
typedef std::vector<Line2> Lines2;

//вычислить нормаль по трём точкам
void getnorm (float a[3],float b[3],float c[3],float *n);

//пересекаются ли треугольники
bool CrossTriangles(Triangle& a,Triangle& b);
bool CrossTriangles(Triangle& t1,Triangle& t2,Line*res,char* res_conf);

//пересекаются ли множества треугольников
bool CrossTrianglesSet(Triangles& a,Triangles& b);
bool FlatCrossTriangle(Triangle& t1,flat&fl ,Line*res);

//здесь хранится точка из пересечения двух треугольников после последнего вызова CrossTriangles
extern vec3 last_triangles_cross;

bool OutsideTriangle3d(vec3 t,Triangle& tr);
void ToSphereCoord(vec3 center, Lines& lines);

//проектирует lines на плоскость с точкой d и ортогональными образующими vx vy
void ProectOnFlat(Lines& lines,vec3 d,vec3 vx,vec3 vy,Lines2& res);
void CutByCircle(Lines2& lines, float r,Lines2* lines_inside,Lines2* lines_outside,iv2vec*crossed_inside=0,iv2vec*crossed_outside=0);
void FillCircleGaps(float r,Lines2& lines, iv2vec* crossed,Lines2&contour,bool inside_circle);

void MoveLinesX(Lines2&lines,float dx);
void CutByX(Lines2&lines,float x0,Lines2* lines_less,Lines2* lines_grater);
void CutByX(Lines2&lines,float x0);

float RotationSurfaceArea(Lines2& lines);
float RotationSurfaceVolume(Lines2& lines);

#endif