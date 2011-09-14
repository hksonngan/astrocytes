#ifndef EDIST_H
#define EDIST_H

#include "Geometry.h"

class Edist
{
public:
	Edist(Geometry*g);
	void Draw();
	void SetTarget(Geometry*g);
	void SetDistance(float d);
	Geometry* GetMesh();
//private:
	Geometry*target,mesh,base;
	float dist;
	vec3 center,axis0;

};

#endif