#ifndef CADEPO_H
#define CADEPO_H

#include "Geometry.h"

void CalcERtoSVR(int as_id,float rad);
void CalcER();
void CalcMonteEPR_as(vec3 c,float rad,float& perS_astr,float& perS_epr);

extern std::vector<ivec> CD_depo; // ������ ���� ��� ������� ���������
extern ivec CD_astr;//������ ���������� ��� ������� ����

#endif