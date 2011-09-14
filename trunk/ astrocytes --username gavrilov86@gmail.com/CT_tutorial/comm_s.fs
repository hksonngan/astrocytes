
varying vec3 vertex,normal;	
/*варьируемые вершина и нормаль			
От вершины стартует луч, а нормаль используется для областей, где ограничивающая коробка делает сечение изоповерхности*/



uniform float level1, step_length;			//изо-значение и длина шага луча
uniform vec3 box1,box2;						//координаты концов ограничивающей коробки

uniform vec3 pos;							//позиция наблюдателя
uniform vec3 nav;							//направление взгляда наблюдателя (из центра экрана)


uniform sampler3D f_text;					//текстура для хранения исходных данных
//uniform sampler3D f_text_i;				//текстура для хранения ускоряющей структуры

											// три вектора (матрица 3х3 для домножение на результирующий цвет. Используется для стерео-режима в анаглифе)
uniform vec3 RFrom;
uniform vec3 GFrom;
uniform vec3 BFrom;

const vec3 level_color1 = vec3(0.6);		//цвет изоповерхности

											// используются для вычисления глубины для заполнения буфера глубины
const float z_near=0.3;
const float z_far=60.0;


											//	вычисление значения поля в точке
float Equ(in vec3 arg)
{
	return texture3D(f_text, arg).x;
}



const float delta=1.0/256.0;

//	вычисление нормированного градиента (т.е. нормали к изоповерхности)
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

// вычисление цвета (освещение по Фонгу) в точке point c нормалью normal и собственного цвета color
vec3 Phong ( vec3 point, vec3 normal, vec3 color)
{
   	vec3 light = normalize( pos - point ); // считаем, что источник света всегда в позиции наблюдателя
	float diffuse = dot ( light, normal );
	vec3 reflect = reflect ( -light, normal );
	float specular = pow ( max (0.0, dot ( reflect, light ) ), P );
	return K_A * vec3(1.0) + diffuse * ( K_D * color + K_S * specular * vec3(1.0) );
}

// вычисление точки пересечения с коробкой (b1,b2) луча из точки pos с направлением nav. Возвращает t, где точка пересечения это pos+nav*t
float IntersectBox ( vec3 b1, vec3 b2, vec3 pos, vec3 nav)
{
	vec3 OMAX = ( b1 - pos ) / nav;
   
	vec3 OMIN = ( b2 - pos ) / nav;
	
	vec3 MAX = max ( OMAX, OMIN );
	
	float res = min ( MAX.x, min ( MAX.y, MAX.z ) );
	return res;
}

//цвет среза
vec3 GetLevelColor(float l)
{
	return vec3(sin(l*5.0));
}

// вычисление глубины в точке ps
float GetDepth(vec3 ps)
{
	return z_far/(z_far-z_near) - (z_far*z_near/(z_far-z_near))/(dot(ps-pos,nav));
}
void main()
{
	float alpha = 0.0;									//сюда запишем результирующую прозрачность
	vec3 color = vec3(0.0);								//... цвет
	vec3 norm = normal, ray = normalize(vertex-pos);	// нормаль, направление движения луча
	vec3 step  = ray*step_length;						// шаг луча

	
	vec3 ps = vertex;									//позиция луча

/*		
// в начале идём с большим шагом, пропуская пустые области
	vec2 e2;
	vec3 stepi  = ray*0.006;
	do
	{
		ps+=stepi;
		e2 = texture3D(f_text_i, ps).xy;
	}while((ps == clamp(ps,box1,box2)) && (level1>e2.y));
	ps-=stepi;
*/	

	float e = Equ(ps),e0;	//значение поля на текущем и предыдущем шагах
	ps+=step;

	if(e >= level1)			//если луч внутри изоповерхности, то выводим срез
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
