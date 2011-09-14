
varying vec3 vertex,normal;	
/*����������� ������� � �������			
�� ������� �������� ���, � ������� ������������ ��� ��������, ��� �������������� ������� ������ ������� ��������������*/



uniform float level1, step_length;			//���-�������� � ����� ���� ����
uniform vec3 box1,box2;						//���������� ������ �������������� �������

uniform vec3 pos;							//������� �����������
uniform vec3 nav;							//����������� ������� ����������� (�� ������ ������)


uniform sampler3D f_text;					//�������� ��� �������� �������� ������
//uniform sampler3D f_text_i;				//�������� ��� �������� ���������� ���������

											// ��� ������� (������� 3�3 ��� ���������� �� �������������� ����. ������������ ��� ������-������ � ��������)
uniform vec3 RFrom;
uniform vec3 GFrom;
uniform vec3 BFrom;

const vec3 level_color1 = vec3(0.6);		//���� ��������������

											// ������������ ��� ���������� ������� ��� ���������� ������ �������
const float z_near=0.3;
const float z_far=60.0;


											//	���������� �������� ���� � �����
float Equ(in vec3 arg)
{
	return texture3D(f_text, arg).x;
}



const float delta=1.0/256.0;

//	���������� �������������� ��������� (�.�. ������� � ��������������)
vec3 GradEqu(in vec3 arg)
{
	vec3 res;
	res.x = Equ(vec3(arg.x+delta,arg.y,arg.z))-Equ(vec3(arg.x-delta,arg.y,arg.z));
	res.y = Equ(vec3(arg.x,arg.y+delta,arg.z))-Equ(vec3(arg.x,arg.y-delta,arg.z));
	res.z = Equ(vec3(arg.x,arg.y,arg.z+delta))-Equ(vec3(arg.x,arg.y,arg.z-delta));
	return normalize(res);
}

#define K_A 0.1
#define K_D 0.9
#define K_S 0.1
#define P 23.0

// ���������� ����� (��������� �� �����) � ����� point c �������� normal � ������������ ����� color
vec3 Phong ( vec3 point, vec3 normal, vec3 color)
{
   	vec3 light = normalize( pos - point ); // �������, ��� �������� ����� ������ � ������� �����������
	float diffuse = dot ( light, normal );
	vec3 reflect = reflect ( -light, normal );
	float specular = pow ( max (0.0, dot ( reflect, light ) ), P );
	return K_A * vec3(1.0) + diffuse * ( K_D * color + K_S * specular * vec3(1.0) );
}

// ���������� ����� ����������� � �������� (b1,b2) ���� �� ����� pos � ������������ nav. ���������� t, ��� ����� ����������� ��� pos+nav*t
float IntersectBox ( vec3 b1, vec3 b2, vec3 pos, vec3 nav)
{
	vec3 OMAX = ( b1 - pos ) / nav;
   
	vec3 OMIN = ( b2 - pos ) / nav;
	
	vec3 MAX = max ( OMAX, OMIN );
	
	float res = min ( MAX.x, min ( MAX.y, MAX.z ) );
	return res;
}

//���� �����
vec3 GetLevelColor(float l)
{
	return vec3(sin(l*5.0));
}

// ���������� ������� � ����� ps
float GetDepth(vec3 ps)
{
	return z_far/(z_far-z_near) - (z_far*z_near/(z_far-z_near))/(dot(ps-pos,nav));
}
void main()
{
	float alpha = 0.0;									//���� ������� �������������� ������������
	vec3 color = vec3(0.0);								//... ����
	vec3 norm = normal, ray = normalize(vertex-pos);	// �������, ����������� �������� ����
	vec3 step  = ray*step_length;						// ��� ����

	
	vec3 ps = vertex;									//������� ����

/*		
// � ������ ��� � ������� �����, ��������� ������ �������
	vec2 e2;
	vec3 stepi  = ray*0.006;
	do
	{
		ps+=stepi;
		e2 = texture3D(f_text_i, ps).xy;
	}while((ps == clamp(ps,box1,box2)) && (level1>e2.y));
	ps-=stepi;
*/	

	float e = Equ(ps),e0;	//�������� ���� �� ������� � ���������� �����
	ps+=step;

	if(e >= level1)			//���� ��� ������ ��������������, �� ������� ����
	{
		color = GetLevelColor(e);
		alpha = 1.0;
		gl_FragDepth = GetDepth(ps);
	}else
	{
			gl_FragDepth=1.0;
			while(true)
			{
				e0=e;
				e = Equ(ps);
				
				if(e >= level1)
				{
					ps += step*((level1-e)/(e-e0));
					norm = -GradEqu(ps);
					color = level_color1;
					alpha = 1.0;
					gl_FragDepth = GetDepth(ps);
					break;
				}
				ps+=step;
				

				if(ps != clamp(ps,box1,box2))
				{
					ps-=step;
					float ll = IntersectBox(box1,box2,ps,step);
					ps += step*ll;

					e = Equ(ps);
					if(e >= level1)
					{
						norm = -GradEqu(ps);
						color = level_color1;
						alpha = 1.0;
						gl_FragDepth = GetDepth(ps);
					}
					break;
				}
					


			}
			
	}
	
	
	
	mat3 m_anag = mat3(RFrom,GFrom,BFrom);
	gl_FragColor = vec4(Phong(ps,norm,color)*m_anag,alpha);	
}
