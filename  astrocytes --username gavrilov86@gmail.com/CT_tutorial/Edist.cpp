
#include "Edist.h"
#include "Figures.h"
#include "Draw.h"
#include "AllInc.h"

Edist::Edist(Geometry*g)
{
	dist=3;
	SetTarget(g);
}

void Edist::SetTarget(Geometry*g)
{
	target = g;
	target->UpdateBox();
	base.renull();
	AddSphere(&base,0.1,50,50);
	mat34 mt(1,1,vec3(1).normalized());
	base.Transform(mt);
	
	flat fl;// = CalcFlat(target->vert);

	center = fl.d;//(target->box1+target->box2)*0.5f;
	axis0 = fl.n;
	vec3 tmpv;
	base.Move(center);
	base.BuildRep2();
	if(0)
	for(int i=0;i<base.vert.size();i++)
	{
		if(base.CrossLine(center,center+base.norm[i]*10,&tmpv))
		{
			base.vert[i] = tmpv;
		}
		else
			base.vert[i] = center;
	}
	base.BuildRep1();

	mesh.vert.resize(base.vert.size());
	mesh.face = base.face;
	mesh.norm = base.norm;

	SetDistance(dist);
	mesh.color = g->color;
	base.color = g->color;
	mesh.color.w *= 0.5f;
	
}

void Edist::SetDistance(float d)
{
	dist = d;

	for(int i=0;i<mesh.vert.size();i++)		
		mesh.vert[i] = base.vert[i] + base.norm[i]*d;

	
	
	//mesh.BuildRep1();
	
}

Geometry* Edist::GetMesh()
{
	return &mesh;
}
void Edist::Draw()
{
	glColor4fv(&mesh.color.x);
	for(int ff=0;ff<2;ff++)
	{
		if(!ff)			glCullFace(GL_FRONT);		else 			glCullFace(GL_BACK);
		mesh.Draw2();

	}
		
	



	vec3 l1=center-axis0*4;
	vec3 l2=center+axis0*4;
	glDisable(GL_LIGHTING);
	glColor4d(0,0,0,1);
	glBegin(GL_LINES);
	glLineWidth(4);
	glVertex3fv(&l1.x);
	glVertex3fv(&l2.x);
	glEnd();
	glLineWidth(1);
	glEnable(GL_LIGHTING);
}