#include "AllInc.h"
#include "wxIncludes.h"
#include <GL/glfw.h>
#include <stdio.h>
#include <stdlib.h>
#include "AllDef.h"
#include "Mouse.h"
#include "Camera.h"
#include "Draw.h"
#include "output.h"
#include <Math.h>
#include <ivec3.h>
#include <vec4.h>
#include "Draw2D.h"
#include "vrml_loader.h"
#include "Edist.h"
#include "MeshSection.h"
#include "globals.h"
#include "table.h"
#include "painting.h"


Mouse mouse;
Camera cam;


ivec2 old_mouse_pos;
int keyboard[255];
int width = 1000, height = 700;
extern vec4 light;
int stereo_on=0, anag=0;
void Calc();
void view_sect();
void view_sect2(flat sc_f,vec3 fd,bool fill);

void FullDraw();


std::vector<std::vector<Geometry>> neuron;//0-astr,1-psd,2-for testing, 3-dendrits
//std::vector<Edist> edist;
std::vector<PSD> psd;
std::vector<Line> section;
std::vector<Line2> section2d;

std::vector<FlatSection> flat_section;

int cur_psd=0;
void seg_BuildBox();
void seg_Cut(ivec2 a,ivec2 b);


// ����������� � ����� �� ������ � ������������ x,y
vec3 GetNav(int x,int y)
{
	vec3 ll=cam.GetLeft()*((x-width/2)/float(height));
	vec3 uu=cam.GetTop()*((y-height/2)/float(height));
	
	
	return cam.GetNav()-(uu-ll)*(2*TAN_22_5);

}

// ��������� ����������� ����� dot � ����� � ��������� ������������ x,y
vec3 MoveDot(int x,int y,vec3 dot)
{
	vec3 n = cam.GetNav();
	float mx = max(abs(n.x),max(abs(n.y),abs(n.z)));
	flat f(dot-cam.GetPosition(),vec3(mx==abs(n.x),mx==abs(n.y),mx==abs(n.z)));
	return flat_cross_line(f,GetNav(x,y))+cam.GetPosition();
}

//���������� �������� ����
void MouseMove ( int x, int y )
{
	mouse.Move(x,y);
light.set(-cam.GetNav().x,-cam.GetNav().y,-cam.GetNav().z,0);
	{
		
		if(keyboard['R'])
		{
			//psd[cur_psd].mediator_radius += (mouse.dx+mouse.dy)*0.01f;
			
			sect_radius += (mouse.dx+mouse.dy)*0.01;

			if(sect_radius<0.01)sect_radius=0.01;
			printf("%g\n",sect_radius);
			
			
		}
		if(keyboard['W'])
		{
			if(flat_section.size())
			{
				vec2 bc = flat_section[0].bc;
				bc += vec2(-mouse.dy-mouse.dx,-mouse.dy+mouse.dx)*0.01;
				for(int i=0;i<flat_section.size();i++)
					flat_section[i].bc = bc;
			}
		}

	}

	if(mouse.btn&2 || mouse.btn&1)
	{
		vec3 old_pos = cam.GetPosition();
		vec3 a_dx = cam.GetTop();			float an_x = -PI*mouse.oldy/float(height)+HALF_PI;			mat34 mrx(cos(an_x),sin(an_x),cam.GetLeft());			a_dx = mrx.MultRot(a_dx);			cam.Rotate(mouse.dx*0.004,a_dx);
		vec3 a_dy = cam.GetLeft();			float an_y = -PI*mouse.oldx/float(width)+HALF_PI;			mat34 mry(cos(an_y),sin(an_y),cam.GetTop());			a_dy = mry.MultRot(a_dy);			cam.Rotate(mouse.dy*0.004,a_dy);

		//light.set(-cam.GetNav().x,-cam.GetNav().y,-cam.GetNav().z,0);

	}	
	if( mouse.btn&4)
	{cam.SetCenter(cam.GetCenter() + (cam.GetLeft() *(float)(-mouse.dx) + cam.GetTop() * (float)(mouse.dy))*(2*cam.GetDistance()*cam.CameraZoom2D/height));}
}

//���������� �������, ���������� � �������� ����
void MouseButton ( int button, int state )
{
	
	mouse.Button(button,state);
	if(button==0)
		{
			if(state)
			{
				old_mouse_pos = ivec2(mouse.oldx,mouse.oldy);
			}else
			{
				//boolean//seg_Cut(ivec2(min(mouse.oldx,old_mouse_pos.x),min(mouse.oldy,old_mouse_pos.y)),ivec2(max(mouse.oldx,old_mouse_pos.x),max(mouse.oldy,old_mouse_pos.y)));

			}
		}
}
void Calc2(int psd_id);
void Calc3();
void CalcAreas(vec3 d,vec3 x,vec3 y,float rad,float*res,ivec2 res_size);

void CalcHist1();
void CalcHist2(int psd_id);
void CalcHist3();

//���������� ����������
void KeyButton ( int key, int state )
{
	keyboard[key]=state;

	
	if(state)
	{
		//boolean//if(key=='B')seg_BuildBox();
		if(key=='L')light.set(-cam.GetNav().x,-cam.GetNav().y,-cam.GetNav().z,0);
		if(key=='.')
		{
			cur_psd = (cur_psd+1)%psd.size();
			
			//cam.ToCoordSystem2(CS3(psd[cur_psd].fl.d,psd[cur_psd].front,psd[cur_psd].left,psd[cur_psd].fl.n*(-1)));
			//cam.SetDistance(2);
			cam.SetCenter(psd[cur_psd].fl.d);
			printf("\n[%d] ",cur_psd);
		}
		if(key==',')
		{
			cur_psd = (cur_psd-1+psd.size())%psd.size();
			//cam.ToCoordSystem2(CS3(psd[cur_psd].fl.d,psd[cur_psd].front,psd[cur_psd].left,psd[cur_psd].fl.n*(-1)));
			//cam.SetDistance(2);
			cam.SetCenter(psd[cur_psd].fl.d);
			printf("\n[%d] ",cur_psd);
		}

		static float rad=0.5;
		if(key=='T')rad=psd[cur_psd].mediator_radius;
		if(key=='Q')PaintTrue(psd[cur_psd].mediator_radius);
		if(key=='H')
		{
			CalcHist1();
			//CalcHist3();
			
		}
		if(key=='Z')PaintAs(cam.GetPosition(),0.6);
		if(key=='Y')
		{
			Geometry*g0=&neuron[2][0];
			g0->renull();
			g0->color.set(1,1,1,1);
			
			for(int as=0;as<neuron[0].size();as++)
			{
				GetInSphere(&neuron[0][as],cam.GetCenter(),sect_radius,g0);
				
			}
			
		}
		if(key=='X')
		{
			
			double start = glfwGetTime ( );
			/*
			int psd_id=cur_psd;

			vec3 cen = psd[psd_id].fl.d;
			float max_vl=0,rd=psd[psd_id].mediator_radius;
			for(int as=0;as<neuron[0].size();as++)
			{
				Geometry*g = &neuron[0][as];
				g->vert_col.resize(g->vert.size());
				for(int i=0;i<g->vert_col.size();i++)
					if(g->vert[i].lengthSQR(cen)<rd*rd)
					{
						float ar = CalcSV(g->vert[i],rad);
						//float ar = CalcArea(g->vert[i],rad);
						if(ar>max_vl)max_vl=ar;
						g->vert_col[i].x = ar;
						
					}else
						g->vert_col[i].x = 0;
			}
			for(int as=0;as<neuron[0].size();as++)
			{
				Geometry*g = &neuron[0][as];
				for(int i=0;i<g->vert_col.size();i++)
				{
					float vl = g->vert_col[i].x/max_vl;
					if(vl)
						g->vert_col[i].set(vl,0,1-vl,1);
					else
						g->vert_col[i]=g->color;
				}

			}
			*/
			Geometry*g0;
			//int psd_id=cur_psd;
			vec3 c = cam.GetCenter();
			float rad = sect_radius;

			
			Geometry*g=&neuron[2][0];
			Geometry*g1=&neuron[2][2];
			g0=new Geometry();
			g->renull();
			g->color.set(1,1,1,1);
			g1->renull();
			g1->color.set(1,0,0,1);
			AddSphere(g0,rad,20,20);
			g0->Move(c+vec3(RND11,RND11,RND11)*0.00001f);
			g0->UpdateBox();
			g0->RebuildTrBB();
			//g0->RebuildTrBB2();

			for(int as=0;as<neuron[0].size();as++)
			{
				GetSection(&neuron[0][as],g0,g,1,0);
				GetSection(g0,&neuron[0][as],g1,1,0);
			}
			float s1=g->CalcArea();
			float v1 = g1->CalcVolume()+g->CalcVolume();

			delete g0;
			
			

			printf("\n%g sec\n white surface square = %g mcm2\n section volume = %g mcm3\n S/V = %g mcm-1\n\n",glfwGetTime ( )-start,s1,v1,s1/v1);
			
			g->BuildRep2();
			g1->BuildRep2();
			/*
			
			start = glfwGetTime ( );
			g0=new Geometry();
			
			for(int as=0;as<neuron[0].size();as++)
			{
				GetInSphere(&neuron[0][as],c,rad,g0);
			}

			s1 = mc_CalcArea(g0,c,rad);
			v1 = mc_CalcVolume(g0,c,rad,c,0);
			float s2 = mc_CalcArea(g0,c,rad);
			float v2 = mc_CalcVolume(g0,c,rad,c,0);

			delete g0;
			printf("\n%g sec S(%g) V(%g|%g) %g|  %g",glfwGetTime ( )-start,s1,v1,v1-v2,s1/v1,s1/v1-s2/v2);
			*/
			//
			

		}
		if(key=='I')
		{
//				FullDraw();
			static int id=0, dd=int(glfwGetTime ( )*1000)%10000;
			id++;
			MakeScreenShot("screenshots//"+str::ToString(dd)+"_"+str::ToString(id)+".png");

		}
		if(key=='P')
		{
			cam.SetCenter(psd[cur_psd].fl.d);
		}
		if(key=='O')
		{
			cam.SetCenter(vec3(0));
		}

		if(key=='S')smoothing=!smoothing;
		if(key==']'){SaveApp("saving");}
		if(key=='['){LoadApp("saving");}

		if(key=='B')draw_boxes=!draw_boxes;
		if(key=='D')
		{
			
		}
		
		if(key=='C' || key=='V')
		{
			if(!flat_section.size()){flat_section.push_back(FlatSection());}
			int psd_id=cur_psd;
			double start = glfwGetTime ( );
			int sz=(key=='V')?8:128;
			float sizef=2;
			float *dt=new float[sz*sz];
			vec3 offset = (psd[psd_id].front+psd[psd_id].left)*(-0.5f*sizef);
			CalcAreas(psd[psd_id].fl.d+offset,psd[psd_id].front*sizef,psd[psd_id].left*sizef,psd[psd_id].mediator_radius,dt,ivec2(sz));

//			for(int i=0;i<sz;i++)			for(int j=0;j<sz;j++)				dt[i+j*sz]=RND01;
			
			flat_section[0].SetValues(dt,ivec2(sz));
			flat_section[0].d = psd[psd_id].fl.d+offset;
			flat_section[0].x = psd[psd_id].front*sizef;
			flat_section[0].y = psd[psd_id].left*sizef;
//

			delete[]dt;
			printf("\n%g sec ",glfwGetTime ( )-start);
		}
		if(key=='J')
			//if(0)
	{
		
		double start = glfwGetTime ( );
		//while(1)
			for(int rd=5;rd<25;rd++)
			{
				for(int i=0;i<psd.size();i++)
				//for(int i=0;i<1;i++)
				{
					printf("\n------------ %d PSD Rad=%d\n",i,rd);
					PaintAs(psd[i].fl.d,rd*0.1f);
				}
				CalcHist1();
				
			}
			
		
		printf("\n%g min ",(glfwGetTime ( )-start)/60);
	}
		
		if(key>='0' && key<='9')
		{
			int id = key-'0';
			if(id<neuron.size())
			{
				for(int i=0;i<neuron[id].size();i++)
					neuron[id][i].visible=!neuron[id][i].visible;
			}
		}
	}
	
	
}

//���������� �������� ������� ����
void MouseWheel( int val )
{
	static int oval = val;
	mouse.Wheel(val-oval);
///	cam.MouseWheel(val-oval);
	cam.SetDistance(cam.GetDistance()*((val-oval)<0?1.1f:0.9f));
	oval = val;
}

void SetupGL()
{
	#ifdef USE_GLEW
		glewInit();
	#endif
	glClearColor(0.6,0.7,1,1);
	//glClearColor(0,0,0,1);
	//glClearColor(1,1,1,1);
	//glShadeModel(GL_FLAT);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
	glDisable(GL_LINE_SMOOTH);
	glCullFace(GL_FRONT);
	
	glEnable(GL_BLEND);
	
	
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}
void initIVP()
{
	
	SetupGL();

	wxImage::AddHandler(new wxPNGHandler);
	cam.SetupProjection(45,0.3f,60.0f,0,0,width,height);

	printf("-----\n%s\n-----\n",output::out_string.c_str());	//����� ���������� ����������� ������� �� �������
	output::out_string.clear();

	LoadNeuron();
	//boolean//seg_BuildBox();
	cam.SetCenter(psd[cur_psd].fl.d);
	cam.SetDistance(5.5f);
	
	
/*
	Geometry gg;
	AddSphere(&gg,1,30,30);
	gg.RebuildTrBB();

	float ss = mc_CalcVolume(&gg,vec3(0.0),1.5);
*/	
	
	PaintTrue(1.0f/30);
	
}

int main ( void )
{
	glfwInit ( );
	
//	printf("Do you want to run program in fullscreen mode? [Y/N]\n");
	printf("[S] - toggle smoothing\n");
	printf("[>] - to next PSD\n");
	printf("[<] - to previous PSD\n");
	printf("[X] - perform the astrocytes section by the sphere\n\n");
	printf("[0] - show/hide astrocytes\n");
	printf("[1] - show/hide PSD\n");
	printf("[2] - show/hide the section\n");
	printf("[3] - show/hide dendrite spinuliferous\n\n");
	printf("[R]+mouse movement - change the sphere radius\n");

	int choice = 0;//getchar ( );

	int _width, _height, mode = GLFW_WINDOW;

	//---------------------------------------------------------------------------------------------


	if ( choice == 'Y' || choice == 'y' )
	{
		GLFWvidmode vidmode;

		glfwGetDesktopMode ( &vidmode );

		width = vidmode.Width;
		
		height = vidmode.Height;

		mode = GLFW_FULLSCREEN;
	}

	//---------------------------------------------------------------------------------------------

    if( !glfwOpenWindow ( width, height, 8, 8, 8, 8, 8, 8, mode ) )
    {
        glfwTerminate ( ); exit ( 0 );
	}

	//---------------------------------------------------------------------------------------------

    glfwSwapInterval ( 0 );

	glfwSetMousePosCallback ( MouseMove );
	glfwSetMouseButtonCallback ( MouseButton );
	glfwSetKeyCallback ( KeyButton );
	glfwSetMouseWheelCallback ( MouseWheel );

	//---------------------------------------------------------------------------------------------

	

	bool running = GL_TRUE;

	int frames = 0;

	char caption [100];
	initIVP();
	double start = glfwGetTime ( );
	double frame_time = glfwGetTime ( );
    while ( running )
    {

        double time = glfwGetTime ( );
//            glfwSetWindowTitle ( caption );

		//-----------------------------------------------------------------------------------------

        glfwGetWindowSize ( &_width, &_height );
        _height = max ( _height, 1 );
		if(_height!=height || _width!=width)
		{
			width=_width;
			height=_height;
			cam.SetupProjection(45,0.3f,60.0f,0,0,width,height);

		}

		if(time-frame_time>1/30.0)
		{frame_time=time;
		FullDraw();
        glfwSwapBuffers ( );
		}

		//-----------------------------------------------------------------------------------------
		
		running = !glfwGetKey ( GLFW_KEY_ESC ) && glfwGetWindowParam ( GLFW_OPENED );
	}

    glfwTerminate ( ); 
	

	exit ( 0 );
}