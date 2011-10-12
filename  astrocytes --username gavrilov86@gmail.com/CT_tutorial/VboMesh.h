#ifndef VBO_MESH_INC
#define VBO_MESH_INC

#include "GeomFunc.h"
#include "mat34.h"
#include "vec4.h"
#include "vec.h"
#include "CS3.h"
#include "AllInc.h"
#include "wxIncludes.h"

class VboMesh
{
public:

	VboMesh();
	~VboMesh();
	void Build(v3vec& vert, v3vec& norm, v4vec& col, iv3vec& id);
	void Clear();
	void Draw();
	bool Enabled(){return vertexID;};
private:
	unsigned vertexID,colorID,normalID, indexID, iNum;
	
};

#endif