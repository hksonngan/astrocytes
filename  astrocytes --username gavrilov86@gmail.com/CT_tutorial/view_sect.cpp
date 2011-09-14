#include "globals.h"

	void seg_BuildBox()
	{

		neuron[2][0].renull();
		neuron[2][1].renull();
		AddBox(&neuron[2][0],vec3(-1),vec3(1));
		//AddSphere(&neuron[2][0],1,25,25);
		neuron[2][0].color.set(1,1,1,1);
		vec3 vv(1,3,2);
		vv.normalize();
		mat34 mt(1,vv);
		//neuron[2][0].Transform(mt);
		neuron[2][0].RebuildTrBB();

	}

#define CUT_NUM 6
	void seg_Cut(ivec2 a,ivec2 b)
	{
		Ray ray[4];
		ray[0] = cam.GetRay(a.x,a.y);
		ray[1] = cam.GetRay(b.x,a.y);
		ray[2] = cam.GetRay(b.x,b.y);
		ray[3] = cam.GetRay(a.x,b.y);
		vec3 vv[CUT_NUM*4];
		for(int i=0;i<4;i++)
		for(int j=0;j<CUT_NUM;j++)
		{
			vv[i+4*j] = ray[i].pos + ray[i].nav*(0.5f+j);
		}

		Geometry sect,res;
		Geometry*g = &sect;
		//Geometry*g = &neuron[2][0];
		for(int i=0;i<4;i++)
		for(int j=0;j<CUT_NUM-1;j++)
			g->AddQuad(vv[i+4*j],vv[i+4*j+4],vv[(i+1)%4+4*j+4],vv[(i+1)%4+4*j]);

		g->AddQuad(vv[0],vv[1],vv[2],vv[3]);
		g->AddQuad(vv[CUT_NUM*4-1],vv[CUT_NUM*4-2],vv[CUT_NUM*4-3],vv[CUT_NUM*4-4]);
		g->RebuildTrBB();
		//g->RebuildTrBB2();

		GetAnotB(&neuron[2][0],g,&res);
		neuron[2][0].Copy(res);



		neuron[2][0].BuildRep2();
		//neuron[2][0].Optimize2();
		neuron[2][0].BuildRep1();
		neuron[2][0].RebuildTrBB();
		//neuron[2][0].RebuildTrBB2();
		neuron[2][0].color.set(1,1,1,1);

//		neuron[2][1].Copy(res);
//		neuron[2][1].color.set(1,0,0,1);
//		neuron[2][1].InvertNormals();
	}
	
void view_sect()
{
	float pp=0;
	if(section.size()!=1)return;
	flat sc_f(psd[cur_psd].fl.d, section[0].v[1]-section[0].v[0]);
	section.clear();
	double start = glfwGetTime ( );
	for(int as=0;as<neuron[0].size();as++)
	{
		FlatCrossGeometry(neuron[0][as],sc_f,section,&pp);
	}
	start = glfwGetTime ( )-start;
	//printf("\n---\n%g\n",start);
	//printf("perimeter = %g\n",pp);


	/*
			neuron[2].resize(2);
			
			Geometry* gg = &neuron[2][0];
			Geometry* gg1 = &neuron[2][1];

			gg1->color.set(0.6,1,0.6,1.0);
			gg->color.set(0.8,0,0,1.0);

			double start = glfwGetTime ( );

			edist[cur_psd].mesh.BuildRep1();
			edist[cur_psd].mesh.RebuildTrBB2();
			for(int as=0;as<neuron[0].size();as++)
			{
				GetSection(&edist[cur_psd].mesh,&neuron[0][as],gg,1,0);
				GetSection(&neuron[0][as],&edist[cur_psd].mesh,gg1,1,0);
			}
			gg->BuildRep2();
			gg1->BuildRep2();
			start = glfwGetTime ( )-start;
			printf("\n---\n%g\n",start);
*/

}

void view_sect2(flat sc_f,vec3 fd,bool fill)
{
//			flat sc_f(psd[cur_psd].fl.d+psd[cur_psd].fl.n*psd_z, psd[cur_psd].fl.n);
	
	vec3 front,left;
	sc_f.n.MakeOrtDop(front,left);
	front.normalize();
	left.normalize();

	section.clear();

	for(int as=0;as<neuron[0].size();as++)
		FlatCrossGeometry(neuron[0][as],sc_f,section,0);

	section2d.clear();
	ProectOnFlat(section,fd,front*(-1),left,section2d);

	
	//iv2vec crossed;
	//Lines2 l0,l1,l2,cl1,cl2;
	//float rad=2;
	//CutByCircle(section2d, rad,&cl1,0, &crossed);
	//FillCircleGaps(rad,cl1, &crossed,cl1,1);
	//section2d=cl1;

	section.clear();
	for(int i=0;i<section2d.size();i++)
	{
		Line ll;
		for(int j=0;j<2;j++)
			ll.v[j] = fd + left*section2d[i].v[j].y - front*section2d[i].v[j].x;
		section.push_back(ll);
	}
	///
	if(!fill)return;
				
				
	v2vec vv;
	v3vec vv3;
	iv2vec ii,ii3;
	ConvertLines(section2d,vv,ii);
	ConvertLines(section,vv3,ii3);
	iv3vec tt;
	MeshLines(vv,ii,tt,1);

	Geometry*g = &neuron[2][0];

	g->renull();
	g->color.set(0,1,0,1);
	for(int i=0;i<tt.size();i++)
	{
		if(tt[i].x<vv.size() &&tt[i].y<vv.size() &&tt[i].z<vv.size())
		{
			vec3 v1 = vec3(vv[tt[i].x].x,vv[tt[i].x].y,0);
			vec3 v2 = vec3(vv[tt[i].y].x,vv[tt[i].y].y,0);
			vec3 v3 = vec3(vv[tt[i].z].x,vv[tt[i].z].y,0);
			g->AddTriangle(v1,v3,v2);
			g->AddTriangle(v1,v2,v3);
		}else {printf("F2");return;}
		if(tt[i].x<vv3.size() &&tt[i].y<vv3.size() &&tt[i].z<vv3.size())
		{
			g->AddTriangle(vv3[tt[i].x],vv3[tt[i].y],vv3[tt[i].z]);
			g->AddTriangle(vv3[tt[i].y],vv3[tt[i].x],vv3[tt[i].z]);
		}else {printf("F3");return;}

	}
}