#include "AllInc.h"
#include <Math.h>
#include "Geometry.h"
#include "mat34.h"
#include "AllDef.h"
#include "vec3.h"
#include "Draw.h"


bool CrossBoxes(vec3 a1,vec3 a2,vec3 b1,vec3 b2) //пересечение коробок a и b
{
	if((a1.x>=b1.x && a1.x<=b2.x) || (a2.x>=b1.x && a2.x<=b2.x) || (a1.x<=b1.x && a2.x>=b2.x))
	if((a1.y>=b1.y && a1.y<=b2.y) || (a2.y>=b1.y && a2.y<=b2.y) || (a1.y<=b1.y && a2.y>=b2.y))
	if((a1.z>=b1.z && a1.z<=b2.z) || (a2.z>=b1.z && a2.z<=b2.z) || (a1.z<=b1.z && a2.z>=b2.z))
		return true;
	return false;
}

bool FlatCrossBox(Line box,flat& fl) //пересечение коробки box c плоскостью
{
	float sg=0;
	box.v[0] -= fl.d;
	box.v[1] -= fl.d;
	for(int i=0;i<2;i++)
	for(int j=0;j<2;j++)
	for(int k=0;k<2;k++)
	{
		vec3 v(box.v[i].x,box.v[j].y,box.v[k].z);
		float tmpf = vec3::dot(v,fl.n);
		if(tmpf*sg<0)return true;
		sg = tmpf;
	}
	return false;
}
void AddToBoxes(Line& bb,int id,std::vector<Boxes>& boxes,float size_limit)
{
	
	Line ll;
	for(int i=0;i<boxes.size();i++)
	{
		ll.v[0] = vec3::Min(bb.v[0],boxes[i].v[0]);
		ll.v[1] = vec3::Max(bb.v[1],boxes[i].v[1]);
		if(max(ll.v[1].x-ll.v[0].x,max(ll.v[1].y-ll.v[0].y,ll.v[1].z-ll.v[0].z))<size_limit)
		{
			boxes[i].v[0] = ll.v[0];
			boxes[i].v[1] = ll.v[1];
			boxes[i].boxes.push_back(id);
			return;
		}
	}
	Boxes n_b;
	n_b.v[0] = bb.v[0];
	n_b.v[1] = bb.v[1];
	n_b.boxes.push_back(id);
	boxes.push_back(n_b);

}
void Geometry::RebuildTrBB()
{
	Line ll;
	tr_bb.clear();
	for(int i=0;i<tr.size();i++)
	{
		ll.v[0] = vec3::Min(tr[i].v[2], vec3::Min(tr[i].v[0],tr[i].v[1]));
		ll.v[1] = vec3::Max(tr[i].v[2], vec3::Max(tr[i].v[0],tr[i].v[1]));
		tr_bb.push_back(ll);
	}
	boxes.clear();
	//printf("T %d ",boxes.size());
	//RebuildTrBB2();

}
void Geometry::RebuildTrBB2()
{
	UpdateBox();
	//float lim = max(box2.x-box1.x,max(box2.y-box1.y,box2.z-box1.z))/6;
	float lim = 1;
	boxes.clear();
	for(int i=0;i<tr_bb.size();i++)
	{
		AddToBoxes(tr_bb[i],i,boxes,lim);
	}
	//printf("T %d ",boxes.size());

}

/*
Geometry::Geometry(const Geometry& g):	tr(g.tr), 
										box_updated(g.box_updated),	
										box1(g.box1), 
										box2(g.box2), 
										color(g.color),
										vert(g.vert),
										face(g.face),
										norm(g.norm),
										tr_bb(g.tr_bb),
										boxes(g.boxes)
{
}
*/
void Geometry::AddTriangle(Triangle&tr1)
{
	tr.push_back(tr1);
	box_updated = 0;
}

void Geometry::AddTriangle(vec3 a,vec3 b,vec3 c)
{
	Triangle tt;
	tt.v[0] = a;
	tt.v[1] = b;
	tt.v[2] = c;
	getnorm(&a.x,&b.x,&c.x,&tt.norm.x);
	//if(abs(tt.norm.x)<0.000001 && abs(tt.norm.y)<0.000001 && abs(tt.norm.z)<0.000001)	{printf(" BT ");return;}
	tt.norm.normalize();
	tr.push_back(tt);

	box_updated = 0;
	
}

void Geometry::AddQuad(vec3 a,vec3 b,vec3 c,vec3 d)
{
	AddTriangle(a,b,c);
	AddTriangle(a,c,d);
}
//возвращает 0, если pt вне пирамиды с вершиной в начале координат и основанием tr, 1 - внутри
bool InsidePiramide(Triangle& tr,vec3 pt)
{
	vec3 cr;
	if(!flat_cross_line2(flat(tr.v[0],tr.norm), pt, &cr))
		return 0;

	for(int i=0;i<3;i++)
	{
		vec3 tmpv = pt - tr.v[i], vv = tr.v[(i+1)%3] - tr.v[i];
		vec3 nn1 = vec3::vect_mult(vv,tr.norm);
		if(vec3::dot(nn1,tmpv)<0) return 0;
	}
	return 1;
}

//объём пирамиды (умноженной на 6) с вершиной в начале координат и основанием tr
float PiramideVolume(Triangle& tr)
{
	vec3 mu = vec3::vect_mult(tr.v[0],tr.v[1]);
	return vec3::dot(mu,tr.v[2]);
}
float TriangleArea(Triangle& tr)
{
	vec3 nn;
	getnorm(&tr.v[0].x,&tr.v[1].x,&tr.v[2].x,&nn.x);
	return vec3::dot(nn,tr.norm);
}

float Geometry::CalcVolume()
{
	float vol=0;
	for(int i=0;i<tr.size();i++)
	{
		vol += PiramideVolume(tr[i]);
	}
	return -vol/6;
}
float Geometry::CalcArea()
{
	float s=0;
	for(int i=0;i<tr.size();i++)
	{
		s += TriangleArea(tr[i]);
	}
	return s/2;
}

bool Geometry::Inside(vec3 pt)
{
	int sm=0;
	vec3 pt2 = pt+vec3(0,0,100),cr;
//	vec2 v1,v2;
	for(int i=0;i<tr.size();i++)
	{
		/*
		int j=0;
		for(;j<3;j++)
		{
			v1.set(pt.x-tr[i].v[j].x,pt.y-tr[i].v[j].y);
			v2.set(tr[i].v[(j+1)%3].y-tr[i].v[j].y,tr[i].v[j].x-tr[i].v[(j+1)%3].x);

			if(vec2::dot(v1,v2)<0)break;
		}
		if(j==3)
		*/
		if(pt.x>=tr_bb[i].v[0].x && pt.x<=tr_bb[i].v[1].x && pt.y>=tr_bb[i].v[0].y && pt.y<=tr_bb[i].v[1].y && pt.z<=tr_bb[i].v[1].z)
		if(triangle_cross_line(tr[i], pt, pt2,0))
		{
			sm++;
		}

	}
	return (sm&1);

}

bool Geometry::CrossLine(vec3 a,vec3 b,vec3* res)
{
	for(int i=0;i<tr.size();i++)
	{
		if(triangle_cross_line(tr[i], a,b,res))return 1;
	}
	return 0;
}
int Geometry::CrossLineTimes(vec3 a,vec3 b)
{
	int res=0;
	for(int i=0;i<tr.size();i++)
	{
		if(triangle_cross_line(tr[i], a,b,0))res++;
	}
	return res;
}

bool Geometry::Cross(Geometry& g)
{
	if(!box_updated)	UpdateBox();
	
	if(!g.box_updated)	g.UpdateBox();


	if(!CrossBoxes(box1,box2,g.box1,g.box2))return false;
	return CrossTrianglesSet(tr,g.tr);


}
void Geometry::renull()
{
	box_updated = 0;
	tr.clear();
	vert.clear();
	face.clear();
	norm.clear();

}

void Geometry::SetNormOutOf(vec3 c)
{
	for(int i=0;i<(int)tr.size();i++)
	{
		if(vec3::dot(tr[i].norm,(tr[i].v[0]-c))<0)
			tr[i].norm.Inv();
	}

}
void Geometry::Transform(mat34& mat)
{
	box_updated = 0;
	for(int i=0;i<(int)tr.size();i++)
	{
		mat.Mult(tr[i]);
	}
}
void Geometry::Transform2(mat34& mat)
{
	box_updated = 0;
	for(int i=0;i<(int)vert.size();i++)
	{
		vert[i] = mat.Mult(vert[i]);
		//norm[i] = mat.Mult(vert[i]);
	}
}
void Geometry::Transform2(CS3 cs)
{
	box_updated = 0;
	
	
	for(int i=0;i<(int)vert.size();i++)
	{
		vec3 cc = vert[i];
		vert[i] = cs.center+cs.x*cc.x+cs.y*cc.y+cs.z*cc.z;
		if(norm.size())norm[i] = cs.x*cc.x+cs.y*cc.y+cs.z*cc.z;
	}
	
}

Geometry::Geometry():box_updated(0),visible(1)
{
}
Geometry::Geometry(vec4 cl):box_updated(0),visible(1)
{
	color = cl;
}

void Geometry::Copy(const Geometry& g)
{
	tr=(g.tr), 
	box_updated=(g.box_updated);	
	box1=(g.box1); 
	box2=(g.box2); 
	color=(g.color);
	vert=(g.vert);
	face=(g.face);
	norm=(g.norm);
	tr_bb=(g.tr_bb);
	boxes=(g.boxes);
}
void Geometry::Add(Geometry& g)
{
	box_updated = 0;
	for(int i=0;i<(int)g.tr.size();i++)
	{
		tr.push_back(g.tr[i]);
	}
	
}
int GetVectorID(v3vec& vectors, vec3 pos)
{
	for(int i=0;i<vectors.size();i++)
	{
		if(pos.lengthSQR(vectors[i])<0.0001f)
			return i;
	}
	return -1;
}
void Geometry::BuildRep1()
{
	tr.clear();
	for(int i=0;i<face.size();i++)
	{
		//printf("%d %d %d | ",face[i].x,face[i].y,face[i].z);
		AddTriangle(vert[face[i].x],vert[face[i].y],vert[face[i].z]);
	}
	RebuildTrBB();


}
void Geometry::Optimize2()
{
	/*
	for(int i=0;i<vert.size()-1;i++)
		for(int j=i+1;j<vert.size();j++)
			if(vert[i].lengthSQR(vert[j])<=0.1f)
			{
				for(int k=0;k<face.size();k++)
					if(face[k].x!=-1)
					if((face[k].x==j)||(face[k].y==j)||(face[k].z==j))
					{
						if((face[k].x==i)||(face[k].y==i)||(face[k].z==i))
						{
							face[k].x=-1;
						}else
						{
							if(face[k].x==j)face[k].x=i;else
							if(face[k].y==j)face[k].y=i;else
							if(face[k].z==j)face[k].z=i;

						}
					}
			}
			*/

	int i=0,j;
	while(i<face.size()-1)
	{
		bool b0=0;
		j=i+1;
		if(face[i].x!=-1)
		while(j<face.size()-1)
		{
			if(face[j].x!=-1)
			{
				ivec3 ff[2];
				ff[0] = face[i];
				ff[1] = face[j];
				int sm=0,ids[2][2];
				for(int ii=0;ii<3;ii++)
					for(int jj=0;jj<3;jj++)
						if(ff[0].GetByID(ii)==ff[1].GetByID(jj))
						{
							ids[sm][0]=ii;
							ids[sm][1]=jj;
							sm++;
						}
				if(sm>=2)
				{
					
					if(sm>2)printf("TR>2 ");
					break;
					vec3 aa[2],bb[2];
					int dd[2];
					for(int ii=0;ii<2;ii++)
					{
						aa[ii] = vert[ff[0].GetByID(ids[ii][0])];
						dd[ii]=0;
						if((ids[0][ii]==0 && ids[1][ii]==1)||(ids[0][ii]==1 && ids[1][ii]==0))dd[ii]=2;
						if((ids[0][ii]==0 && ids[1][ii]==2)||(ids[0][ii]==2 && ids[1][ii]==0))dd[ii]=1;

						bb[ii] = vert[ff[ii].GetByID(dd[ii])];
					}
					
					for(int ii=0;ii<2;ii++)
						if(abs(vec3::vect_mult(aa[ii]-bb[0],aa[ii]-bb[1]).lengthSQR())<0.00001f)
						{
							printf(":");
							break;

							face[i].SetByID(ids[ii][0], ff[1].GetByID(dd[ii]));
							face[j].x=-1;
							i=-1;
							j=0;
							b0=1;
							break;


						}
					
					
				}
				if(b0)break;
			}
			j++;
		}

		i++;
	}

	iv3vec face1;
	for(int k=0;k<face.size();k++)
		if(face[k].x!=-1)
			face1.push_back(face[k]);
	face = face1;

	
}
void Geometry::BuildRep2()
{
	
	vert.clear();
	norm.clear();
	face.clear();
	ivec3 id;
	int tmpi;
	
	for(int i=0;i<tr.size();i++)
	{
		for(int j=0;j<3;j++)
		{
			tmpi = GetVectorID(vert, tr[i].v[j]);
			vec3 nrm;
			getnorm(&tr[i].v[0].x,&tr[i].v[1].x,&tr[i].v[2].x,&nrm.x);
			if(tmpi==-1)
			{
				tmpi = vert.size();
				
				vert.push_back(tr[i].v[j]);
				norm.push_back(nrm);
			}else
				norm[tmpi] = norm[tmpi] + nrm;
			id.SetByID(j, tmpi);
		}
		face.push_back(id);
	}
	for(int i=0;i<norm.size();i++)
	{
		norm[i].normalize();
	}
}

void Geometry::Draw()
{
	if(!visible)return;

		Triangle* tt;

	if(1)
	{
		glBegin(GL_TRIANGLES);
		for(int i=0;i<(int)tr.size();i++)
		{
			tt = &tr[i];
			glNormal3fv(&tt->norm.x);
			glVertex3fv(&tt->v->x);
			glVertex3fv(&(tt->v+1)->x);
			glVertex3fv(&(tt->v+2)->x);
		}
		glEnd();
	}else
	{
		glDisable(GL_LIGHTING);
		for(int i=0;i<(int)tr.size();i++)
		{
			tt = &tr[i];
		//vec3 tmp=(tt->v[0]+tt->v[1]+tt->v[2])/3.0f;
		glBegin(GL_LINE_LOOP);
			glVertex3fv(&tt->v->x);
			glVertex3fv(&(tt->v+1)->x);
			glVertex3fv(&(tt->v+2)->x);
		glEnd();
		}
		
		glEnable(GL_LIGHTING);
	}

}

void Geometry::DrawBoxes()
{

	//if(0)
	{
	glColor3d(0,0,0);
		glDisable(GL_LIGHTING);
		for(int i=0;i<boxes.size();i++)
		{
			DrawCubeFrame(boxes[i].v[0],boxes[i].v[1]);
		}
		if(!boxes.size())
		//DrawCubeFrame(box1,box2);
		
		glEnable(GL_LIGHTING);
	}
}
void Geometry::UpdateVboMesh()
{
	vbo_mesh.Build(vert,norm,vert_col,face);
}
void Geometry::BuildSmoothed(Geometry*g)
{
	//for(int i=0;)

}
void Geometry::Draw2()
{
	if(vbo_mesh.Enabled())
	{
		vbo_mesh.Draw();
		return;
	}
	bool col = (vert_col.size()==vert.size());
	bool nrm = (norm.size()==vert.size());
	if(!visible)return;
	if(!nrm){glDisable(GL_LIGHTING); glDisable(GL_CULL_FACE);}
	glBegin(GL_TRIANGLES);
	for(int i=0;i<(int)face.size();i++)
	{
		
		//glNormal3fv(&tr[i].norm.x);

		if(nrm)glNormal3fv(&norm[face[i].x].x);
		if(col)glColor4fv(&vert_col[face[i].x].x);
		glVertex3fv(&vert[face[i].x].x);

		if(nrm)glNormal3fv(&norm[face[i].y].x);
		if(col)glColor4fv(&vert_col[face[i].y].x);
		glVertex3fv(&vert[face[i].y].x);

		if(nrm)glNormal3fv(&norm[face[i].z].x);
		if(col)glColor4fv(&vert_col[face[i].z].x);
		glVertex3fv(&vert[face[i].z].x);
	}
	glEnd();
	if(!nrm){glEnable(GL_LIGHTING); glEnable(GL_CULL_FACE);}
/*
	glPointSize(4);
	glBegin(GL_POINTS);
	for(int i=0;i<(int)vert.size();i++)
	{
		glVertex3fv(&vert[i].x);
	}
	glEnd();
	glPointSize(1);
	*/
}

void Geometry::UpdateBox()
{
	box_updated = 1;

	if(!tr.size())return;
	box1 = tr[0].v[0];
	box2 = box1;
	Triangle* tt;
	int i,j,k;
	for(i=0;i<(int)tr.size();i++)
	{
		tt = &tr[i];
		for(j=0;j<3;j++)
		for(k=0;k<3;k++)
		{
			if(VEC_BY_ID(box1,k)>VEC_BY_ID(tt->v[j],k)) VEC_BY_ID(box1,k) = VEC_BY_ID(tt->v[j],k);
			else
			if(VEC_BY_ID(box2,k)<VEC_BY_ID(tt->v[j],k)) VEC_BY_ID(box2,k) = VEC_BY_ID(tt->v[j],k);
		}
	}

}

void Geometry::InvertNormals()
{
	Triangle* tt;
	for(int i=0;i<(int)tr.size();i++)
	{
		tt = &tr[i];

		tt->norm.Inv();
		swap(tt->v[1],tt->v[2]);
	}

}
void Geometry::Move(vec3 d)
{
	Triangle* tt;
	for(int i=0;i<(int)tr.size();i++)
	{
		tt = &tr[i];

		for(int k=0;k<3;k++)
			tt->v[k]+=d;
	}

}
void Geometry::Move2(vec3 d)
{
	for(int i=0;i<(int)vert.size();i++)
	{
		vert[i] += d;
	}

}