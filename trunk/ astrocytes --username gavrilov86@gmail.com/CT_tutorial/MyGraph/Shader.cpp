#include "AllInc.h"
#include "Shader.h"
#ifdef USE_GLEW
#pragma comment(lib,"glew32.lib")
#include <windows.h>
#endif

#include "output.h"
bool printInfoLog(GLuint handle);

void ShaderProgram::Use()
{
	if(!times_used)
	glUseProgram(h_program);
		times_used++;
}

void ShaderProgram::UnUse()
{
	times_used--;
	if(!times_used)
		glUseProgram(0);
}
/*
void ShaderProgram::PrintLog()
{
	printInfoLog(h_program);
}*/
ShaderProgram::~ShaderProgram()
{
	glDetachShader(h_program,h_vert);
	glDeleteShader(h_vert);
	glDetachShader(h_program,h_frag);
	glDeleteShader(h_frag);
	glDeleteProgram(h_program);
}
ShaderProgram::ShaderProgram(const GLchar*vs,const GLchar*fs)
{
	times_used=0;
    char * tmp, * tmp2;
	std::ifstream fin, fin2;

	const GLchar* vShader;
	const GLchar* fShader;


    //! vertex shaer
    fin.open(vs);
    if(!fin.is_open()) {
        std::cout<<"Vertex shader File Error"<<std::endl;
        return;
    }
   
    long begin=fin.tellg();
    fin.seekg(0, std::ios::end);
    long end=fin.tellg();
    int size=end-begin;
    tmp = new char[size+1];
    fin.seekg(0, std::ios::beg);

    int ii=0; 
    while(fin.good()) {
        tmp[ii]=(char)fin.get();
        if(!fin.eof())ii++;
    }

    tmp[ii]=0;
    vShader=(const GLchar*)tmp;

    fin.close();

    //! fragment shaer
    fin2.open(fs);
    if(!fin2.is_open()) {
        std::cout<<"Fragment shader File Error"<<std::endl;
        return;
    }
   
    fin2.seekg(0, std::ios::beg);
    begin=fin2.tellg();
    fin2.seekg(0, std::ios::end);
    end=fin2.tellg();
    size=end-begin;
    tmp2=new char[size+1];
    fin2.seekg(0, std::ios::beg);

    ii=0; 
    while(fin2.good()) {
        tmp2[ii]=(char)fin2.get();
        if(!fin2.eof())ii++;
    }

    tmp2[ii]=0;
    fShader=(const GLchar*)tmp2;
    fin2.close();

    const GLchar* source[2] = {vShader, fShader};
    // Check for shader support first
    float version = (float)atof((const char *)glGetString(GL_VERSION));
    std::cout << "OpenGL v" << version;
    if (version < 2) {
        std::cout << " <-- ERROR: Need at least OpenGL v2.0. Does your graphics card support shaders?\n";
        exit(-1);
    }
    else {
        std::cout << " <-- OKAY!\n";
    }

    h_vert = glCreateShader(GL_VERTEX_SHADER);
	std::cout <<("handle for vertex shader=" + str::ToString((int)h_vert));
    glShaderSource(h_vert, 1, (const GLchar**)&source[0], NULL);
    glCompileShader(h_vert);
   printInfoLog(h_vert);

    h_frag = glCreateShader(GL_FRAGMENT_SHADER);
	std::cout<<("handle for fragment shader=" + str::ToString((int)h_frag));
    glShaderSource(h_frag, 1, (const GLchar**)&source[1], NULL);
    glCompileShader(h_frag);
    printInfoLog(h_frag);

    h_program = glCreateProgram();
    std::cout<<("handle for shader program=" + str::ToString((int)h_program));
    glAttachShader(h_program, h_vert);
    glAttachShader(h_program, h_frag);
    glLinkProgram(h_program);
    printInfoLog(h_program);

    std::cout<<("shaders initialized");

	delete[] tmp;
	delete[] tmp2;
}

GLint ShaderProgram::getUniLoc(const GLchar *name)
{
	GLint loc; 
	loc = glGetUniformLocation(h_program,name); 
	if(loc==-1)
	{
	    static int idd=0;	idd++;	if(idd>300)return loc;

		std::cout<<("No such uniform named " + std::string(name));
	}
	
	return loc;
}

bool printInfoLog(GLuint handle)
{
    GLint   lenInfoLog = 0;
    GLint   nWritten = 0;
    GLchar  *infoLog;

    glGetShaderiv ( handle, GL_INFO_LOG_LENGTH, &lenInfoLog );

    if(lenInfoLog)
    {
        infoLog = (GLchar *)malloc(lenInfoLog);
        glGetShaderInfoLog (handle, lenInfoLog, &nWritten, infoLog);
        if(nWritten)
        {
			std::cout<<("glInfoLog: " + std::string(infoLog));
        }
        free(infoLog);
    }
    return true;
}

void checkGLErrors (const char *label) {
	
    GLenum errCode;
    const GLubyte *errStr;
    static int idd=0;	idd++;	if(idd>30)return;

    if ((errCode = glGetError()) != GL_NO_ERROR) {
	errStr = gluErrorString(errCode);
	std::cout<<("OpenGL ERROR: ");
	std::cout<<((char*)errStr);
	std::cout<<(" Label: ");
	std::cout<<(label);
	//delete[]errStr;
	
    }
	
}


void ShaderProgram::SetVar(const GLchar *name,vec2 val)
{
	glUniform2f(getUniLoc(name),val.x,val.y);
}
void ShaderProgram::SetVar(const GLchar *name,vec3 val)
{
	glUniform3f(getUniLoc(name),val.x,val.y,val.z);
}
void ShaderProgram::SetVar(const GLchar *name,ivec3 val)
{
	glUniform3i(getUniLoc(name),val.x,val.y,val.z);
}
void ShaderProgram::SetVar(const GLchar *name,vec4 val)
{
	glUniform4f(getUniLoc(name),val.x,val.y,val.z,val.w);
}
void ShaderProgram::SetVar(const GLchar *name,float val)
{
	glUniform1f(getUniLoc(name),val);
}
void ShaderProgram::SetVar(const GLchar *name,unsigned int val)
{
	glUniform1i(getUniLoc(name),val);
}
void ShaderProgram::SetVar(const GLchar *name,int val)
{
	glUniform1i(getUniLoc(name),val);
}
void ShaderProgram::SetMatrix3(const GLchar *name,float*m)
{
	
	glUniformMatrix3fv(getUniLoc(name),sizeof(float),0,m);
}

void ShaderProgram::SetVar(const GLchar *name,int num, vec4* val)
{
	glUniform4fv(getUniLoc(name),num,&val->x);
}

void ShaderProgram::SetVar(const GLchar *name,int num, float* val)
{
	glUniform1fv(getUniLoc(name),num,val);
}
