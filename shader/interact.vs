#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 5) in float aState;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out float gVertexID;
out float gState;

void main()
{
		gl_Position = projection * view * model * vec4(aPos, 1.0);
	  gVertexID = gl_VertexID;
	  gState=aState;
	  
}