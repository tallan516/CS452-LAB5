#version 130

uniform sampler2D texture; //texture object from application

in vec4 pass_color;
in vec2 texCoord; //texure coordinate from rasterizer

out vec4 out_color;

void main()
{
	out_color = pass_color * texture2D(texture, texCoord);
}
