
varying vec3 vertex,normal;	

uniform float min_level,max_level, step_length;
uniform vec3 box1,box2;
uniform vec3 txt_box1,txt_box2;
uniform vec3 pos;		
uniform vec3 nav;	


uniform sampler3D f_text;
//uniform sampler3D f_text_i;			

uniform vec3 RFrom;
uniform vec3 GFrom;
uniform vec3 BFrom;


const vec3 level_color1 = vec3(0.6);	


const float z_near=0.3;
const float z_far=60.0;


float Equ(in vec3 arg)
{
	return texture3D(f_text, (arg-txt_box1)/(txt_box2-txt_box1)).x;
}



const float delta=1.0/256.0;


vec3 GradEqu(in vec3 arg)
{

	vec3 res;
	res.x = Equ(vec3(arg.x+delta,arg.y,arg.z))-Equ(vec3(arg.x-delta,arg.y,arg.z));
	res.y = Equ(vec3(arg.x,arg.y+delta,arg.z))-Equ(vec3(arg.x,arg.y-delta,arg.z));
	res.z = Equ(vec3(arg.x,arg.y,arg.z+delta))-Equ(vec3(arg.x,arg.y,arg.z-delta));
	return normalize(res);
	
	return normalize(res);
}


#define K_A 0.1
#define K_D 0.9
#define K_S 0.1
#define P 23.0

vec3 Phong ( vec3 point, vec3 normal, vec3 color)
{
   	vec3 light = normalize( pos - point ); 
	float diffuse = dot ( light, normal );
	vec3 reflect = reflect ( -light, normal );
	float specular = pow ( max (0.0, dot ( reflect, light ) ), P );
	return K_A * vec3(1.0) + diffuse * ( K_D * color + K_S * specular * vec3(1.0) );
}



bool IntersectBox ( vec3 Origin,vec3 Direction, vec3 minimum, vec3 maximum, out float start, out float final )
{
	vec3 OMAX = ( minimum - Origin ) / Direction;
	vec3 OMIN = ( maximum - Origin ) / Direction;
	vec3 MAX = max ( OMAX, OMIN );
	vec3 MIN = min ( OMAX, OMIN );
	final = min ( MAX.x, min ( MAX.y, MAX.z ) );
	start = max ( max ( MIN.x, 0.0), max ( MIN.y, MIN.z ) );	
	return final > start;
}


float GetDepth(vec3 ps)
{
	return z_far/(z_far-z_near) - (z_far*z_near/(z_far-z_near))/(dot(ps-pos,nav));
}

float rand(vec4 co)
{
	return fract(sin(dot(co,vec4(1123.0,2330.5,10.1,10.3))) * 1000.0);
}
void main()
{
	float alpha = 0.0;
	vec3 color = vec3(0.0);
	vec3 norm, ray = normalize(vertex-pos);
	vec4 step  = vec4(ray*step_length,step_length);
	vec4 ps = vec4(pos,0.0);

	float e=0.0,e0, start,final;	
		gl_FragDepth=1.0;

	IntersectBox (ps.xyz,ray,box1,box2,start,final );
	{

		ps = ps + vec4(ray,1.0)*(start);
		//ps = ps - step*(rand(vec4(vertex,ps.w)));
		e = Equ(ps.xyz);
		/*if(e>min_level)
		{
			norm = normalize(GradEqu(ps.xyz));
			if(dot(norm,ray)>0)norm=-norm;
				
							
			//color = vec3(1.0,0.0,0.0);
			color = Phong(ps.xyz,norm,vec3(0.8));
			alpha=1.0;
			gl_FragDepth = GetDepth(ps.xyz);
		}
		else*/
		{
			do
			{
				e0=e;
				e = Equ(ps.xyz);
				ps += step;
			}while((ps.w <final) && (e<=min_level));
			

			if(e>min_level)
			{

				ps += step*clamp( ((min_level-e)/(e-e0)-1.0) , -2.0,-1.0);
				norm = normalize(GradEqu(ps.xyz));
				if(dot(norm,ray)>0)norm=-norm;
				
							
				color = Phong(ps.xyz,norm,vec3(0.8));
				alpha=1.0;
				gl_FragDepth = GetDepth(ps.xyz);
			}
		}
		mat3 m_anag = mat3(RFrom,GFrom,BFrom);
		gl_FragColor = vec4(color*m_anag,alpha);
		
	}
	/*else
	{
		gl_FragColor = vec4(0.0,0.0,0.0,0.0);
		gl_FragDepth=1.0;
	}*/
	
	
	
}
