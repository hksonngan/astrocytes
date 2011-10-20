#include "VboMesh.h"

VboMesh::VboMesh()
{
	vertexID=0;
	indexID=0;
}

VboMesh::~VboMesh()
{
	Clear();
}

void VboMesh::Build(v3vec& vert, v3vec& norm, v4vec& col, iv3vec& id)
{
	Clear();
	iNum = id.size()*3;
	if(!iNum)return;

	glGenBuffersARB (1, &vertexID );
	glGenBuffersARB (1, &colorID );
	glGenBuffersARB (1, &normalID );
	glGenBuffersARB (1, &indexID );
//

	glBindBufferARB ( GL_ARRAY_BUFFER_ARB, vertexID );
	glBufferDataARB ( GL_ARRAY_BUFFER_ARB, vert.size() * sizeof ( vec3 ), &vert[0].x, GL_STATIC_DRAW_ARB );

	glBindBufferARB ( GL_ARRAY_BUFFER_ARB, colorID );
	glBufferDataARB ( GL_ARRAY_BUFFER_ARB, col.size() * sizeof ( vec4 ), &col[0].x, GL_STATIC_DRAW_ARB );

	glBindBufferARB ( GL_ARRAY_BUFFER_ARB, normalID );
	glBufferDataARB ( GL_ARRAY_BUFFER_ARB, norm.size() * sizeof ( vec3 ), &norm[0].x, GL_STATIC_DRAW_ARB );

	glBindBufferARB ( GL_ELEMENT_ARRAY_BUFFER_ARB, indexID );
	glBufferDataARB ( GL_ELEMENT_ARRAY_BUFFER_ARB, id.size() * sizeof ( ivec3 ), &id[0].x, GL_STATIC_DRAW_ARB );
	
//
	glBindBufferARB ( GL_ARRAY_BUFFER_ARB,         0 );
	glBindBufferARB ( GL_ELEMENT_ARRAY_BUFFER_ARB, 0 );	

}

void VboMesh::Clear()
{
	if(indexID)
	{
		glDeleteBuffersARB (1, &vertexID );
		glDeleteBuffersARB (1, &colorID );
		glDeleteBuffersARB (1, &normalID );
		glDeleteBuffersARB (1, &indexID );
	}
	indexID=0;
}

void VboMesh::Draw()
{
	if(!indexID)return;

	glEnableClientState ( GL_VERTEX_ARRAY );
	glEnableClientState ( GL_COLOR_ARRAY );
	glEnableClientState ( GL_NORMAL_ARRAY );

	glBindBufferARB( GL_ARRAY_BUFFER_ARB, vertexID );
	glVertexPointer( 3, GL_FLOAT, 0, 0 );
	
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, normalID );
	glNormalPointer(GL_FLOAT, 0, 0 );
	
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, colorID );
	glColorPointer( 4, GL_FLOAT, 0, 0 );
	
	glBindBufferARB   ( GL_ARRAY_BUFFER_ARB, 0 );
	glBindBufferARB ( GL_ELEMENT_ARRAY_BUFFER_ARB, indexID );
	glIndexPointer( GL_UNSIGNED_INT, 0, 0 );
	glDrawElements( GL_TRIANGLES, iNum, GL_UNSIGNED_INT, 0 );

	glDisableClientState ( GL_VERTEX_ARRAY );
	glDisableClientState ( GL_COLOR_ARRAY );
	glDisableClientState ( GL_NORMAL_ARRAY );
//	exit(0);
}
