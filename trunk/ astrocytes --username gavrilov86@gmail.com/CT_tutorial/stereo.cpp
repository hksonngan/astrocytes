#include "Shader.h"

void SetAnag(ShaderProgram*ps,int s, bool left)
{
	vec3 anag[3];

	switch(s)
	{
	case 0: // no stereo
		anag[0].set(1,0,0);
		anag[1].set(0,1,0);
		anag[2].set(0,0,1);
		break;
	case 1: // True anaglyh
		if(left)
		{
			anag[0].set(0.299,0.587,0.114);
			anag[1].set(0,0,0);
			anag[2].set(0,0,0);
		}else
		{
			anag[0].set(0,0,0);
			anag[1].set(0,0,0);
			anag[2].set(0.299,0.587,0.114);
		}
		break;
	case 2: // Gray anaglyh
		if(left)
		{
			anag[0].set(0.299,0.587,0.114);
			anag[1].set(0,0,0);
			anag[2].set(0,0,0);
		}else
		{
			anag[0].set(0,0,0);
			anag[1].set(0.299,0.587,0.114);
			anag[2].set(0.299,0.587,0.114);
		}
		break;
	case 3: // Color anaglyh
		if(left)
		{
			anag[0].set(1,0,0);
			anag[1].set(0,0,0);
			anag[2].set(0,0,0);
		}else
		{
			anag[0].set(0,0,0);
			anag[1].set(0,1,0);
			anag[2].set(0,0,1);
		}
		break;	
	case 4: // Half-Color anaglyh
		if(left)
		{
			anag[0].set(0.299,0.587,0.114);
			anag[1].set(0,0,0);
			anag[2].set(0,0,0);
		}else
		{
			anag[0].set(0,0,0);
			anag[1].set(0,1,0);
			anag[2].set(0,0,1);
		}
		break;	
	case 5: // Optimized anaglyh
		if(left)
		{
			anag[0].set(0,0.7,0.3);
			anag[1].set(0,0,0);
			anag[2].set(0,0,0);
		}else
		{
			anag[0].set(0,0,0);
			anag[1].set(0,1,0);
			anag[2].set(0,0,1);
		}
		break;	
	};

//	ps->Use();
	ps->SetVar("RFrom",anag[0]);
	ps->SetVar("GFrom",anag[1]);
	ps->SetVar("BFrom",anag[2]);
//	ps->UnUse();
}