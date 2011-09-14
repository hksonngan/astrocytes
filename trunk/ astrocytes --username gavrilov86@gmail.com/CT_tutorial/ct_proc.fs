varying vec2 vTexCoord;

uniform sampler3D f_text;
uniform float current_z;


#define SIZE_X 256.0
#define SIZE_Y 256.0
#define SIZE_Z 256.0

#define STEP_X (1.0/SIZE_X)
#define STEP_Y (1.0/SIZE_Y)
#define STEP_Z (1.0/SIZE_Z)

#define EPSILON 0.001
#define KERNEL_SIZE 3.0

float GetProcessed(vec3 v)
{
	float res = 0.0;
	vec3 ker = v - vec3(STEP_X,STEP_Y,STEP_Z)*KERNEL_SIZE, tmp,iv;
	
	for(iv.x=-KERNEL_SIZE,tmp.x=ker.x;iv.x<=KERNEL_SIZE-EPSILON;iv.x++,tmp.x+=STEP_X)
	for(iv.y=-KERNEL_SIZE,tmp.y=ker.y;iv.y<=KERNEL_SIZE-EPSILON;iv.y++,tmp.y+=STEP_Y)
	for(iv.z=-KERNEL_SIZE,tmp.z=ker.z;iv.z<=KERNEL_SIZE-EPSILON;iv.z++,tmp.z+=STEP_Z)
	{
		float ll=length(iv);
		if(ll>0.0)
		res += texture3D(f_text, tmp).x/ll;

	}
	return res;
	
}
void main()
{
	vec4 res;
	vec3 cur_coord = vec3(vTexCoord,current_z);

	for(int i=0;i<4;i++)
	{
		res[i] = GetProcessed(cur_coord);
		cur_coord.x += STEP_X;
	}
	
	gl_FragColor = res;
}