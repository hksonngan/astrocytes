
varying vec3 vertex,normal;
varying vec4 color;

void main(void)
{
	//vertex = vec3(gl_ModelViewMatrix * gl_Vertex);
	//normal = normalize(vec3(gl_NormalMatrix*gl_Normal));
	vertex = vec3( gl_Vertex);
	normal = normalize(vec3(gl_Normal));
	color = vec4(gl_Color);
    gl_Position = ftransform();

}
