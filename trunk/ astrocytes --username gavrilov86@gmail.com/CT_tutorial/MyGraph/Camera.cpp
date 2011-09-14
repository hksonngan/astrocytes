#include "AllInc.h"

#include "Camera.h"
#include "AllDef.h"

#include <Math.h>

#include "mat34.h"
#include "Draw.h"

float Camera::CameraZoom2D = 0.3f;

Camera::Camera(): dist(3),center(0),is_perspective(1)
{
	nav.set(0,0,1);
	left.set(1,0,0);
	top.set(0,1,0);
	UpdatePos();
}

void Camera::SetCenter(vec3 c)
{
	center=c;
	UpdatePos();
}
float Camera::GetDistance()
{
	return dist;
}
void Camera::SetDistance(float d)
{
	dist=d;
	UpdatePos();

}
void Camera::SetProjection(bool mode)
{
	is_perspective = mode;
	
	if(is_perspective)
		SetupProjection(45,zNear,zFar,0,0,_width,_height);
	UpdatePos();

}
void Camera::SetupPosition()
{
	if(!is_perspective)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		
		glOrtho(-dist*CameraZoom2D*_width/(double)_height,dist*CameraZoom2D*_width/(double)_height,-CameraZoom2D*dist,CameraZoom2D*dist,zNear,zFar);
		//gluOrtho2D(-dist*CameraZoom2D*_width/(double)_height,dist*CameraZoom2D*_width/(double)_height,-CameraZoom2D*dist,CameraZoom2D*dist);
		glMatrixMode(GL_MODELVIEW);
	}

	glLoadIdentity();
/*	if(stereo_mode)
		glTranslated(eye_distance*(left_eye?1:-1),0,-dist);
	else
*/
		glTranslated(0.0, 0.0, -dist);
	
	//glRotated(-xRot * PI180 , 1.0, 0.0, 0.0);
	//glRotated(-yRot * PI180 +180, 0.0, 1.0, 0.0);
	CS3 cs(vec3(0),vec3(left.x,top.x,-nav.x),vec3(left.y,top.y,-nav.y),vec3(left.z,top.z,-nav.z));
	ChangeCoordSystem(cs);
	glTranslated(-center.x, -center.y, -center.z);
}

void Camera::SetupProjection(float angle,float mynear,float myfar,int x,int y,int width,int height)
{
	zNear=mynear;
	zFar=myfar;
	_angle = angle;
	_width=width;
	_height=height;
	glViewport(x,y,width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(_angle,width/(double)height,mynear,myfar);
    glMatrixMode(GL_MODELVIEW);
}
void Camera::SetupProjection(int x,int y,int width,int height)
{
	
	_width=width;
	_height=height;
	glViewport(x,y,width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	gluPerspective(_angle,width/(double)height,zNear,zFar);
    glMatrixMode(GL_MODELVIEW);
}
void Camera::UpdatePos()
{
	pos = nav*(-dist) + center;

}

void Camera::Rotate(float angle,vec3 axis)
{
	mat34 mr(cos(angle),sin(angle),axis);
	nav = mr.MultRot(nav);
	top = mr.MultRot(top);
	left = mr.MultRot(left);

	//для избавления от накапливающейся погрешности
	static int cnt=100;
	cnt--;
	if(!cnt)
	{
		cnt=100;
		nav.normalize();
		left = left.proected_on(nav).normalized();
		top = vec3::vect_mult(left,nav);
	}

	UpdatePos();
}
vec3 Camera::GetPosition()
{
	//if(!stereo_mode)
		return pos;
	//return pos+left*(left_eye?-1.0f:1.0f)*eye_distance;
}

Ray Camera::GetRay(int x,int y)
{
	Ray res;
	if(is_perspective)
	{
		vec3 ll=left*((x-_width/2)/float(_height));
		vec3 uu=top*((y-_height/2)/float(_height));
		res.nav = nav-(uu-ll)*(2*TAN_22_5);
		res.pos = pos;
	}else
	{
		res.nav = nav;
		res.pos = pos+left*( (2*x-_width)*dist*CameraZoom2D/(float)_height ) - top*( (2*y/(float)_height-1)*CameraZoom2D*dist );

		//gluOrtho2D(-dist*CameraZoom2D*_width/(double)_height,dist*CameraZoom2D*_width/(double)_height,-CameraZoom2D*dist,CameraZoom2D*dist);
	}
	return res;

}

void Camera::ToCoordSystem(CS3 cs)
{
//	cs = cs._1();
	center = cs.GetCoords(center);
	left = cs.GetCoordsRot(left);
	top = cs.GetCoordsRot(top);
	nav = cs.GetCoordsRot(nav);
	UpdatePos();

}

void Camera::ToCoordSystem2(CS3 cs)
{
//	cs = cs._1();
	center = cs.center;
	left = cs.x;
	top = cs.y;
	nav = cs.z;
	dist=1;
	UpdatePos();

}