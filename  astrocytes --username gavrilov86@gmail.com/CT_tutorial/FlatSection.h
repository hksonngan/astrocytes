#ifndef FlatSection_H
#define FlatSection_H
#include "ivec2.h"

class SimText3D;

class FlatSection
{
public:
	FlatSection();
	~FlatSection();
	void Draw();
	void SetValues(float*n_dt,ivec2 n_size);
	vec3 d,x,y;
	vec2 bc;
private:
	ivec2 dt_size;
	SimText3D*txt;

};



#endif