#include "AllInc.h"

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

#include "globals.h"


vec4 light(1,0,0,0);
bool smoothing=1,draw_boxes=0;
bool shaders_is=1,anag_left=0;

extern ivec2 old_mouse_pos;
float sect_radius=0.6;


void DrawScene()
{
	static ShaderProgram sp("anag.vs","anag.fs");



	cam.SetupPosition();				//домножение текущей матрицы преобразований в соответствии с положением и ориентацией камеры

	
	glLightfv(GL_LIGHT0,GL_POSITION, &light.x);

	
	glDisable(GL_LIGHTING);
	glColor4d(0,0,0,1);
	glBegin(GL_LINES);
	glLineWidth(3);
	for(int i=0;i<section.size();i++)
	{
		//glColor3d(0,0,0);
		glVertex3fv(&section[i].v[0].x);
		//glColor3d(1,1,1);
		glVertex3fv(&section[i].v[1].x);
	}
	for(int i=0;i<section2d.size();i++)
	{
		//glColor3d(0,0,0);
		glVertex2fv(&section2d[i].v[0].x);
		//glColor3d(1,1,1);
		glVertex2fv(&section2d[i].v[1].x);
	}
/*
	float grid_size=2;
	glColor4d(0.1,0.5,0.1,0.7);
	glVertex2f(-grid_size,0);
	glVertex2f(grid_size,0);
	glVertex2f(0,-grid_size);
	glVertex2f(0,grid_size);

	glColor4d(0.1,0.5,0.1,0.3);

	float grid_step=0.1;
	for(float i=-grid_size;i<=grid_size;i+=grid_step)
	{
		glVertex2f(-grid_size,i);
		glVertex2f(grid_size,i);
		glVertex2f(i,-grid_size);
		glVertex2f(i,grid_size);
	}
*/
	glEnd();

	//glColor4d(0.1,0.5,0.1,0.5);	for(int i=1;i<200;i++)		DrawCircle(0.1*i,vec2(0),45);

	psd[cur_psd].Draw();
	
/*
	glPointSize(12);
	glColor4d(0.1,0.5,0.1,0.5);
	
	glBegin(GL_POINTS);
	for(int i=0;i<section2d.size();i++)
	{
		glVertex2fv(&section2d[i].v[0].x);
		glVertex2fv(&section2d[i].v[1].x);
	}
	glEnd();
	glPointSize(1);
	//DrawRectangle(vec2(-PI,-PI*0.5),vec2(PI,PI*0.5));
*/
	glLineWidth(1);
	glEnable(GL_LIGHTING);

	
	if(shaders_is) sp.Use();
	SetAnag(&sp,anag,anag_left);
	sp.SetVar("LightDir",vec3(light));
	sp.SetVar("pos",cam.GetPosition());

	//glEnable(GL_DEPTH_TEST);

	for(int i=0;i<neuron.size();i++)
	for(int j=0;j<neuron[i].size();j++)
		if(neuron[i][j].color.w)
	//		if(i!=1 || j==cur_psd)
	{
		glColor4fv(&neuron[i][j].color.x);
		if(smoothing)
			neuron[i][j].Draw2();
		else
			neuron[i][j].Draw();

		if(draw_boxes)
		{
			if(shaders_is) sp.UnUse();
			neuron[i][j].DrawBoxes();
			if(shaders_is) sp.Use();
		}

	}
	if(shaders_is) sp.UnUse();

	glColor4d(0,0,0,0.2f);
	DrawSphere(cam.GetCenter(),sect_radius);
glCullFace(GL_BACK);	
	DrawSphere(cam.GetCenter(),sect_radius);
glCullFace(GL_FRONT);

	for(int i=0;i<flat_section.size();i++)
		flat_section[i].Draw();

	/*
	if(mouse.btn&1)
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glViewport(0,0,width,height);
		Begin2D(width,height);
		glColor4f(1,1,1,0.6f);
		DrawRectangle(vec2(old_mouse_pos.x,old_mouse_pos.y),vec2(mouse.oldx,mouse.oldy));
		End2D();
		glEnable(GL_DEPTH_TEST);
	}*/

}

// рисуем сцену в соответствии со стерео-режимом
void FullDraw()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawScene();
}