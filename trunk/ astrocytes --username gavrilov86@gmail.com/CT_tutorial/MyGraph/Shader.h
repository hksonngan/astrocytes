#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "AllInc.h"

#include "vec2.h"
#include "vec3.h"
#include "ivec3.h"
#include "vec4.h"
#include <vector>
#include <assert.h>

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <cstring>
#include <fstream>
#include <ctime>



class ShaderProgram
{
public:
	ShaderProgram(const GLchar*vs,const GLchar*fs);
	~ShaderProgram();
	void Use();
	void UnUse();
//	void PrintLog();

	void SetVar(const GLchar *name,vec2 val);
	void SetVar(const GLchar *name,vec3 val);
	void SetVar(const GLchar *name,ivec3 val);
	void SetVar(const GLchar *name,vec4 val);
	void SetVar(const GLchar *name,float val);
	void SetVar(const GLchar *name,unsigned int val);
	void SetVar(const GLchar *name,int val);
	void SetMatrix3(const GLchar *name,float*m);

	void SetVar(const GLchar *name,int num,vec4* val);
	void SetVar(const GLchar *name,int num,float* val);
	
	template <class T>
	void SetVar1(std::string name,T val)
	{
		SetVar(name.c_str(),val);
	}
	template <class T>
	void SetVar(std::string name,int num,T val)
	{
		SetVar(name.c_str(),num,val);
	}


private:
	int times_used;
	GLint getUniLoc(const GLchar *name);
	GLuint h_vert;
    GLuint h_frag;
    GLuint h_program;

};

#endif