#include "MeshSection.h"
#include "AllDef.h"
#include "vec2.h"
#include <Math.h>
#include "AllInc.h"


void GetIntersection(Triangle& t1,Triangle& t2,Lines& res,std::vector<LineConf>* lines_conf,float*perimeter)
{
	Line ll;
	LineConf cc;
	if(CrossTriangles(t1,t2,&ll,cc.v))
	{
		if(!vecs_near(ll.v[0],ll.v[1]))
		{
			res.push_back(ll);
			if(lines_conf)lines_conf->push_back(cc);
			if(perimeter)*perimeter += ll.v[0].length(ll.v[1]);
		}//else printf(" BL ");
	}
}



//заносит результаты пересечений (отрезки) треугольников из g с треугольником tr (tr_bb - is a triangle's bounding box)
void GetCrTriangles(Geometry& g,Triangle& tr,Lines& res,std::vector<LineConf>* lines_conf,Line tr_bb,float*perimeter)
{
	if(!g.boxes.size())
	{
	for(int i=0;i<g.tr.size();i++)
		if(CrossBoxes(g.tr_bb[i].v[0],g.tr_bb[i].v[1],tr_bb.v[0],tr_bb.v[1]))
			GetIntersection(g.tr[i],tr,res,lines_conf,perimeter);
	}else
	for(int b=0;b<g.boxes.size();b++)
		if(CrossBoxes(g.boxes[b].v[0],g.boxes[b].v[1],tr_bb.v[0],tr_bb.v[1]))
			for(int i=0;i<g.boxes[b].boxes.size();i++)
			{
				int box_id = g.boxes[b].boxes[i];
				if(CrossBoxes(g.tr_bb[box_id].v[0],g.tr_bb[box_id].v[1],tr_bb.v[0],tr_bb.v[1]))
					GetIntersection(g.tr[box_id],tr,res,lines_conf,perimeter);
			}
			
}

//заносит результаты пересечений (отрезки) треугольников из g с плоскостью fl 
void FlatCrossGeometry(Geometry& g,flat fl,Lines& res,float*perimeter)
{
	Line ll;
	for(int b=0;b<g.boxes.size();b++)
		if(FlatCrossBox(Line(g.boxes[b].v[0],g.boxes[b].v[1]),fl))
			for(int i=0;i<g.boxes[b].boxes.size();i++)
			{
				int box_id = g.boxes[b].boxes[i];
				if(FlatCrossTriangle(g.tr[box_id],fl,&ll))
					if(!vecs_near(ll.v[0],ll.v[1]))
					{
						//для единообразной ориентации отрезков
						if(vec3::dot(fl.n,vec3::vect_mult(g.tr[box_id].norm,ll.v[0]-ll.v[1]))<0)swap(ll.v[0],ll.v[1]);
						
						res.push_back(ll);
						if(perimeter)*perimeter += ll.v[0].length(ll.v[1]);
					}//else printf(" BL ");
			}
}
//g2 must be with boxes
void GetIntersection(Geometry*g1,Geometry*g2,Lines& res)
{
	Line bb1,bb2;
	for(int i=0;i<g1->tr.size();i++)
	{
		GetCrTriangles(*g2,g1->tr[i],res,0,g1->tr_bb[i],0);
	}

		
}


void GetSection(Geometry*target,Geometry*section,Geometry*res,bool inside_section,bool back_faces,float*perimeter)
{
	Triangle cur_tr;
	Lines lines;
	std::vector<LineConf> lines_conf;


	for(int i=0;i<target->tr.size();i++){
		/*
	for(int b=0;b<target->boxes.size();b++)
		if(CrossBoxes(target->boxes[b].v[0],target->boxes[b].v[1],section->box1,section->box2))
			for(int bi=0;bi<target->boxes[b].boxes.size();bi++)
			{int i=target->boxes[b].boxes[bi];*/

		if(CrossBoxes(target->tr_bb[i].v[0],target->tr_bb[i].v[1],section->box1,section->box2))
	{
		cur_tr = target->tr[i];
		lines.clear();
		lines_conf.clear();
		GetCrTriangles(*section, cur_tr, lines,&lines_conf, target->tr_bb[i],perimeter);
		
		bool ins = section->Inside(cur_tr.v[0]);
		ins = xor(inside_section,ins);
		
		if(lines.size())
		{
			//static int cc=0;
			//cc++;

			CutTriangle(cur_tr,lines,lines_conf,ins,res,!back_faces);
			//if(cc==2)return;
		}else
		{
			if(ins)
			{
				if(back_faces){swap(cur_tr.v[0],cur_tr.v[1]);cur_tr.norm.Inv();}
				res->AddTriangle(cur_tr);
			}//else
		
		}

	}
	}
	//printf("\n\nSection's triangles:%d\n",res->tr.size());

}

// всё то, что внутри сферы.
void GetInSphere0(Geometry*target,vec3 c,float rad,Geometry*res)
{
	Triangle cur_tr;
	vec3 b1 = c-vec3(rad);
	vec3 b2 = c+vec3(rad);

	for(int b=0;b<target->boxes.size();b++)
		if(CrossBoxes(target->boxes[b].v[0],target->boxes[b].v[1],b1,b2))
			for(int bi=0;bi<target->boxes[b].boxes.size();bi++)
			{
				int i=target->boxes[b].boxes[bi];
				//if(CrossBoxes(target->tr_bb[i].v[0],target->tr_bb[i].v[1],b1,b2))
				{
					cur_tr = target->tr[i];
					if(cur_tr.v[0].lengthSQR(c)<rad*rad || cur_tr.v[1].lengthSQR(c)<rad*rad || cur_tr.v[2].lengthSQR(c)<rad*rad)
						res->AddTriangle(cur_tr);

				}
			}

}
int GetNearest(v3vec& vert,vec3 pt)
{
	int res=0;
	float mll = pt.lengthSQR(vert[0]);
	for(int i=1;i<vert.size();i++)
	{
		float ll = pt.lengthSQR(vert[i]);
		if(ll<mll){mll=ll;res = i;}
	}
	return res;
}
void ChooseComponent(v3vec& src_vert,iv3vec& src_face,v3vec& dst_vert,iv3vec& dst_face,int src_vert_id)
{
	//printf(" %d < %d ",src_vert_id,src_vert.size());
	bvec incl,fincl;
	incl.resize(src_vert.size());
	fincl.resize(src_face.size());
	for(int i=0;i<src_face.size();i++)
		fincl[i]=0;
	for(int i=0;i<src_vert.size();i++)
		incl[i] = (i==src_vert_id);
	dst_vert = src_vert;
	bool was_added;
	while(1)
	{
		was_added=0;
		for(int i=0;i<src_face.size();i++)
		{
			ivec3 ff = src_face[i];
			if((incl[ff.x] || incl[ff.y] || incl[ff.z]) &&(!fincl[i]))
			{
				fincl[i]=1;
				was_added=1;
				incl[ff.x]=incl[ff.y]=incl[ff.z]=1;
				dst_face.push_back(ff);
			}
		}

		if(!was_added)break;
	}
	
}
// всё то, что внутри сферы. центр сферы определяет связную компоненту, еднственно которую оставляем
void GetInSphere(Geometry*target,vec3 c,float rad,Geometry*res)
{
	iv3vec face;
	v3vec vert;
	Geometry tmpg;
	GetInSphere0(target, c, rad,&tmpg);
	tmpg.BuildRep2();

	if(!tmpg.vert.size())return;
	int v_id = GetNearest(tmpg.vert,c);
	
	ChooseComponent(tmpg.vert,tmpg.face,vert,face,v_id);
	for(int i=0;i<face.size();i++)
	{
		
		res->AddTriangle(vert[face[i].x],vert[face[i].y],vert[face[i].z]);
	}

}
void GetAorB(Geometry*a,Geometry*b,Geometry*res)
{
	GetSection(a,b,res,0,0);
	GetSection(b,a,res,0,0);

}
void GetAandB(Geometry*a,Geometry*b,Geometry*res)
{
	GetSection(a,b,res,1,0);
	GetSection(b,a,res,1,0);

}
void GetAnotB(Geometry*a,Geometry*b,Geometry*res)
{
	GetSection(a,b,res,0,0);
	GetSection(b,a,res,1,1);

}
