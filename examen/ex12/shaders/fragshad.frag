#version 330 core

in vec3 fcolor;
out vec4 FragColor;

uniform int ratlles;

void main()
{	
	if (ratlles == 0) FragColor = vec4(fcolor,1);
	else{
        if ((int(gl_FragCoord[1]) % 20) < 10) FragColor = vec4(1,1,1,1);
        else FragColor = vec4(0,0,0,1);
	}
}
