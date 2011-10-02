#include "PSD.h"
#include "Draw.h"
#include "AllInc.h"
#include <Math.h>

/*

flat CalcFlat(iv3vec face,v3vec vert)
{
	flat res;
	float www=0;
	res.d.set(0,0,0);
	res.n.set(0,0,0);
	
	for(int i=0;i<face.size();i++)
	{
		vec3 b = vert[face[i].x]-vert[face[i].y];
		vec3 a = vert[face[i].x]-vert[face[i].z];
		vec3 c = (vert[face[i].x]+vert[face[i].y]+vert[face[i].z])/3;
		vec3 tmpv = vec3::vect_mult(a,b);
		float ww = tmpv.lengthSQR();
		www+=ww;
		if(tmpv.z<0)tmpv.Inv();
		res.n += tmpv;
		res.d += c*ww;

	}
	res.n.normalize();
	res.d /=www;
	

	return res;
}
*/

flat CalcFlat(v3vec vert)
{
	flat res;
	float www=0;
	res.d.set(0,0,0);
	res.n.set(0,0,0);
	//printf("%d+++",vert.size());
	if(vert.size()>2)
	//for(int i=0;i<vert.size()-2;i++)
	//for(int j=i+1;j<vert.size()-1;j++)
	//for(int k=j+1;k<vert.size();k++)
	for(int p=0;p<100;p++)
	{
		int i=rand()%vert.size(),j=i,k=i;
		while(i==j)j=rand()%vert.size();
		while(i==k || k==j)k=rand()%vert.size();

		vec3 b = vert[i]-vert[k];
		vec3 a = vert[i]-vert[j];
		vec3 c = (vert[i]+vert[j]+vert[k])/3;
		vec3 tmpv = vec3::vect_mult(a,b);
		if(vec3::dot(tmpv,res.n)<0)tmpv.Inv();
		float ww = tmpv.lengthSQR();
		www+=ww;
		
		res.n += tmpv;
		res.d += c*ww;

	}
	res.n.normalize();
	res.d /=www;
	

	return res;
}
PSD::PSD(Geometry* n_target):target(n_target)
{
	fl = CalcFlat(target->vert);
	fl.n.MakeOrtDop(front,left);
	front.normalize();
	left.normalize();

	mediator_radius=0;
	for(int i=0;i<target->vert.size();i++)
	{
		float ll=fl.d.lengthSQR(target->vert[i]);
		if(mediator_radius<ll)mediator_radius=ll;
	}
	mediator_radius = sqrt(mediator_radius);
	
}
void PSD::Draw()
{
	glDisable(GL_LIGHTING);
	
	/*
	vec3 l1=fl.d;
	vec3 l2=fl.d+fl.n;

	glColor4d(0,0,0,1);
	glBegin(GL_LINES);
	glLineWidth(4);
	glVertex3fv(&l1.x);
	glVertex3fv(&l2.x);
	glEnd();
	DrawQuadFrame(fl.d+(front+left)*mediator_radius,fl.d+(left-front)*mediator_radius,fl.d-(left+front)*mediator_radius,fl.d+(front-left)*mediator_radius);
*/
	glPushMatrix();
	ChangeCoordSystem(fl.d,front,left,fl.n);

	glBegin(GL_LINES);
	//glLineWidth(4);

	float grid_size=2;
	glLineWidth(3);
	glColor4d(0.1,0.1,0.1,0.9);
	glVertex2f(-grid_size,0);
	glVertex2f(grid_size,0);
	glVertex2f(0,-grid_size);
	glVertex2f(0,grid_size);
	glLineWidth(1);

	glColor4d(0.1,0.1,0.1,0.5);

	float grid_step=0.1;
	for(float i=-grid_size;i<=grid_size;i+=grid_step)
	{
		glVertex2f(-grid_size,i);
		glVertex2f(grid_size,i);
		glVertex2f(i,-grid_size);
		glVertex2f(i,grid_size);
	}

	glEnd();
	glPopMatrix();

//

	//glLineWidth(1);
	glEnable(GL_LIGHTING);
	glColor3d(0.5,0.5,0.5);
	//DrawSphere(fl.d,mediator_radius);
}
