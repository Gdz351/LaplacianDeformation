#version 330 core
out vec4 FragColor;


in float gVertexID;
in float gState;
uniform int gObjectID;

void main()
{

    FragColor=vec4(float(gObjectID+1),float(gl_PrimitiveID+1),float(gVertexID+1),float(gState+1));
}