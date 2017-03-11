#version 400

layout(location=0) in vec3 vPosition;

out vec4 fragColor;

uniform vec4 lampColor;
uniform mat4 model_lamps;
uniform mat4 view_lamps;
uniform mat4 projection_lamps;

void main()
{
	fragColor = lampColor; //color passed
	gl_Position = projection_lamps * view_lamps * model_lamps * vec4(vPosition, 1.0f);
}