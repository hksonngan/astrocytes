#include "SimText3D.h"
#include "Draw.h"
#include "FlatSection.h"
#include "globals.h"

FlatSection::FlatSection()
{
	txt=0;
	bc.set(0,1);
}

FlatSection::~FlatSection()
{
	if(txt)delete txt;
}
void FlatSection::Draw()
{
	vec3 vv[4];
	vv[0] = d+x+y;
	vv[1] = d+y;
	vv[2] = d;
	vv[3] = d+x;
	
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glColor3d(0,0,0);
	DrawQuadFrame(vv[0],vv[1],vv[2],vv[3]);
	if(txt)
	{

		static ShaderProgram sp("common_shaders//srf.vs", "common_shaders//view2d.fs");;
		sp.Use();
		sp.SetVar("txt",txt->SetID(1));
		sp.SetVar("min_level",bc.x);
		sp.SetVar("max_level",bc.y);
		sp.SetVar("size",vec2(dt_size.x,dt_size.y));
		sp.SetVar("params",vec4(smoothing,0,0,0));


		DrawQuad(vv[0],vv[1],vv[2],vv[3]);
		sp.UnUse();
	}
	
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
}
void FlatSection::SetValues(float*n_dt,ivec2 n_size)
{
	dt_size = n_size;
	if(txt)delete txt;
	txt = new SimText3D(2,n_size.x,n_size.y,1,1,n_dt,0,1,GL_FLOAT);
}
