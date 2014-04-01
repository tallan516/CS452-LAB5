#version 130

uniform mat4 Matrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

in vec3 in_position;
in vec4 in_color;
in vec2 vTexCoord;	//texture coordinate from application 

out vec4 pass_color;
out vec2 texCoord;	//output tex coordinate to be interpolated

void main()
{
	gl_Position = Matrix * viewMatrix * modelMatrix * vec4(in_position, 1.0); 
	pass_color = in_color;
	texCoord = vTexCoord;
}
