varying vec3 vertex,normal;
varying vec4 color;


uniform vec3 RFrom;
uniform vec3 GFrom;
uniform vec3 BFrom;

uniform vec3 pos;
uniform vec3 LightDir;


#define K_A 0.1
#define K_D 0.9
#define K_S 0.5
#define P 64.0

vec3 Phong ( vec3 point, vec3 normal, vec3 color)
{
	float diffuse = dot ( LightDir, normal )*0.5+0.5;
	vec3 reflect = reflect ( -LightDir, normal );
	float specular = pow ( max (0.0, dot ( reflect, normalize(pos-point) ) ), P );
	return K_A * vec3(1.0) + diffuse * ( K_D * color + K_S * specular * vec3(1.0) );
}

void main()
{
	
	mat3 m_anag = mat3(RFrom,GFrom,BFrom);
	/*
	if(abs(int(color.x/5.0)*5.0-color.x)<0.5f)color=vec4(1.0,1.0,1.0,1.0);else
	{
	if(color.x<=0)color=vec4(0.0,0.0,0.0,1.0);else
	for(float lm=5.0;lm<40.0;lm+=5.0)
	{
		if(color.x<lm){color=vec4(lm/40.0,0.0,1.0-lm/40.0,1.0);break;}
	}
	}
	*/
	gl_FragColor = vec4(Phong(vertex,normal,color.xyz)*m_anag,color.w);
	
}
