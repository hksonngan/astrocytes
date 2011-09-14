#ifndef FIGURES_INC
#define FIGURES_INC

#include "Geometry.h"

//� ��������� ����� ��������� ������ ������:

void AddBox(Geometry*g, vec3 a,vec3 b);
void AddCylinder(Geometry*g,float h,float r, int sides);
void AddSphere(Geometry*g,float r, int sides_a, int sides_b);
void AddArmDetail(Geometry*g,float w,float r, int sides);

#endif