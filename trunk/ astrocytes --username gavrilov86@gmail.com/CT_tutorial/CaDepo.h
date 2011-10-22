#ifndef CADEPO_H
#define CADEPO_H

#include "Geometry.h"

void CalcERtoSVR(int as_id,float rad);
void CalcER();

extern std::vector<ivec> CD_depo; // списки депо для каждого астроцита
extern ivec CD_astr;//списки астроцитов для каждого депо

#endif