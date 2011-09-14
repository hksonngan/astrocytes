#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "CS3.h"

#define CAMERA_ANGLE 45.0
#define Z_NEAR 0.1
#define Z_FAR 30.0

struct Ray
{
	vec3 pos,nav;
};

class Camera
{
public:
	Camera();
	void SetupPosition();
	void SetupProjection(float angle,float mynear,float myfar,int x,int y,int width,int height);
	void SetupProjection(int x,int y,int width,int height);

	void SetCenter(vec3 c);
	void SetDistance(float d);
	float GetDistance();

	vec3 GetCenter(){return center;}
	vec3 GetPosition();
	vec3 GetNav(){return nav;}
	vec3 GetLeft(){return left;}
	vec3 GetTop(){return top;}

	void SetProjection(bool mode);
	bool GetProjection(){return is_perspective;}

	void Rotate(float angle,vec3 axis);
	
	void ToCoordSystem(CS3 cs);
	void ToCoordSystem2(CS3 cs);

	Ray GetRay(int x,int y);

	static float CameraZoom2D;

private:
	void UpdatePos();

	vec3 pos,nav,top,left,center;
	float dist;

	float zNear,zFar, _angle;
	int _width,_height;
	bool is_perspective;
};
#endif