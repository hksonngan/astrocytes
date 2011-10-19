#include "PN_Triangle.h"
#include "AllDef.h"


PN_Triangle::PN_Triangle(vec3 v1,vec3 v2,vec3 v3,vec3 n1,vec3 n2,vec3 n3,vec4 c1,vec4 c2,vec4 c3)
{
	b300 = v1;
	b030 = v2;
	b003 = v3;
	
	float w12 = vec3::dot(v2-v1,n1);
	float w21 = vec3::dot(v1-v2,n2);
	float w23 = vec3::dot(v3-v2,n2);
	float w32 = vec3::dot(v2-v3,n3);
	float w31 = vec3::dot(v1-v3,n3);
	float w13 = vec3::dot(v3-v1,n1);

	b210 = (v1*2+v2-n1*w12)/3;
	b120 = (v2*2+v1-n2*w21)/3;
	b021 = (v2*2+v3-n2*w23)/3;
	b012 = (v3*2+v2-n3*w32)/3;
	b102 = (v3*2+v1-n3*w31)/3;
	b201 = (v1*2+v3-n1*w13)/3;
	vec3 e = (b210+b120+b201+b102+b021+b012)/6;
	vec3 v = (v1+v2+v3)/3;
	b111 = e + (e-v)/2;

	n200 = n1;
	n020 = n2;
	n002 = n3;
	float v12 = 2*vec3::dot(v2-v1,n1+n2)/(v2-v1).lengthSQR();
	float v23 = 2*vec3::dot(v3-v2,n3+n2)/(v3-v2).lengthSQR();
	float v31 = 2*vec3::dot(v1-v3,n1+n3)/(v1-v3).lengthSQR();
	n110 = n1+n2-(v2-v1)*v12;
	n011 = n2+n3-(v3-v2)*v23;
	n101 = n3+n1-(v1-v3)*v31;

	n110.normalize();
	n101.normalize();
	n011.normalize();
	c100 = c1;
	c010 = c2;
	c001 = c3;
}

vec3 PN_Triangle::GetVertex(float u,float v)
{
	float w = 1-u-v;
	return	b300*(w*w*w) + b030*(u*u*u) + b003*(v*v*v) + 
			(b210*(w*w*u) + b120*(w*u*u) + b201*(w*w*v) + 
			b021*(u*u*v) + b102*(w*v*v) + b012*(u*v*v))*3 + 
			b111*(v*u*w*6);
}


vec3 PN_Triangle::GetNormal(float u,float v)
{
	float w = 1-u-v;

	return	n200*(w*w)+n020*(u*u)+n002*(v*v)+
			n110*(w*u)+n011*(u*v)+n101*(w*v);
}

vec4 PN_Triangle::GetColor(float u,float v)
{
	float w = 1-u-v;

	return	c100*w+c010*u+c001*v;
}

void MakeSmoothed(Geometry*src_g,Geometry*dst_g,int order)
{
	if(order<2)return;
	for(int i=0;i<src_g->face.size();i++)
	{
		PN_Triangle pnt(src_g->vert[src_g->face[i].x],src_g->vert[src_g->face[i].y],src_g->vert[src_g->face[i].z],
						src_g->norm[src_g->face[i].x],src_g->norm[src_g->face[i].y],src_g->norm[src_g->face[i].z],
						src_g->vert_col[src_g->face[i].x],src_g->vert_col[src_g->face[i].y],src_g->vert_col[src_g->face[i].z]);
		int v_offset = dst_g->vert.size();

		for(int u=0;u<order;u++)
			for(int v=0;v<order-u;v++)
			{
				float fu = u/float(order-1);
				float fv = v/float(order-1);
				dst_g->vert.push_back(pnt.GetVertex(fu,fv));
				dst_g->norm.push_back(pnt.GetNormal(fu,fv));
				dst_g->vert_col.push_back(pnt.GetColor(fu,fv));
			}
		int off0=0,off1=0;
		for(int u=0;u<order-1;u++)
		{
			for(int v=0;v<order-u-1;v++)
			{
				int v00 = v_offset+ u*order+v-off0;
				int v01 = v_offset+ (u+1)*order+v-off1;
				int v10 = v_offset+ (u)*order+v+1-off0;
				int v11 = v_offset+ (u+1)*order+v+1-off1;
				
				dst_g->face.push_back(ivec3(v00,v01,v10));
				if(v!=order-u-2)
					dst_g->face.push_back(ivec3(v01,v11,v10));
			}
			if(u>=0)off1+=u+1;
			if(u>=1)off0+=u;
		}
	}

}