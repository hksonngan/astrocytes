#ifndef CADEPO_H
#define CADEPO_H

#include "Geometry.h"

void CalcERtoSVR(int as_id,float rad);
void CalcER();

extern std::vector<ivec> CD_depo; // ������ ���� ��� ������� ���������
extern ivec CD_astr;//������ ���������� ��� ������� ����

#endif