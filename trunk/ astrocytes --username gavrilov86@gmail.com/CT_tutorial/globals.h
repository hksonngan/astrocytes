#ifndef GLOBALS_FUNC
#define GLOBALS_FUNC

#include "AllInc.h"
#include "vrml_loader.h"
#include "MeshSection.h"
#include "Figures.h"
#include <GL/glfw.h>
#include "AllDef.h"
#include "Mouse.h"
#include "Camera.h"
#include "Draw.h"
#include "output.h"
#include <Math.h>
#include <ivec3.h>
#include "Draw2D.h"
#include "Geometry.h"
#include "Edist.h"
#include "Shader.h"
#include "PSD.h"
#include "FlatSection.h"
#include "loadsave.h"

void LoadNeuron();
void SetAnag(ShaderProgram*ps,int s, bool left);

float mc_CalcArea(Geometry*g,vec3 c,float r);
float mc_CalcVolume(Geometry*g,vec3 c,float r, vec3 c1,bool ins);


extern int ang_it_num,it_num;

extern Mouse mouse;
extern Camera cam;
extern int keyboard[255];
extern int width, height;

extern std::vector<std::vector<Geometry>> neuron;
//extern std::vector<Edist> edist;
extern std::vector<PSD> psd;
extern std::vector<Line> section;
extern std::vector<Line2> section2d;
extern int cur_psd;

extern int stereo_on, anag;
extern bool smoothing,anag_left,draw_boxes;
extern int total_tr,total_v;

extern std::vector<FlatSection> flat_section;

extern float sect_radius;

extern std::string painting_svr_fn;
void MakeScreenShot(std::string screenshot_dst);

#endif