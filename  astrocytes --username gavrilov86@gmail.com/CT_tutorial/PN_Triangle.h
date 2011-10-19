#ifndef PN_Triangle_FUNC
#define PN_Triangle_FUNC
#include "Geometry.h"

class PN_Triangle
{
public:
	PN_Triangle(vec3 v1,vec3 v2,vec3 v3,vec3 tn1,vec3 tn2,vec3 tn3,vec3 n1,vec3 n2,vec3 n3,vec4 c1,vec4 c2,vec4 c3);
	vec3 GetVertex(float u,float v);
	vec3 GetNormal(float u,float v);
	vec4 GetColor(float u,float v);
private:
	vec3 b300,b030,b003;
	vec3 b210,b120,b021,b012,b201,b102,b111;
	vec3 n200,n020,n002,n110,n101,n011;//normals for shading
	vec4 c100,c010,c001;


};

void MakeSmoothed(Geometry*src_g,Geometry*dst_g,int order);

#endif