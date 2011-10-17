#ifndef PSD_H
#define PSD_H

#include "Geometry.h"

class PSD
{
public:
	PSD(){target=0;}
	PSD(Geometry* n_target);
	void Draw();

	flat fl;
	float mediator_radius;
	vec3 front,left;

//private:
	Geometry* target;
	

};

#endif