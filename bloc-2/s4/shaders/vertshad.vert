#version 330 core

in vec3 vertex;
in vec3 color;

uniform mat4 TG, PT, VT;

out vec3 fcolor;

void main()  {
    fcolor = color;
    gl_Position =  PT * VT * TG * vec4 (vertex, 1.0);
}
