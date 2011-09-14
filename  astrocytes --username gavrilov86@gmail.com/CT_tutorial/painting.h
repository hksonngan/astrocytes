#ifndef PAINTING_INC
#define PAINTING_INC

#include "table.h"


float CalcSVR(vec3 cc,float rr);
void SavePainting(std::string fn);
void LoadPainting(std::string fn);
void PaintTrue(float brightness);

void PaintAs(vec3 cc,float rr);

#endif