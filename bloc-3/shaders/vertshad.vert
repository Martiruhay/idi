#version 330 core

in vec3 vertex;
in vec3 normal;

in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;

//OUT
out mat4 viewF;
out mat4 TGF;

out vec3 vertexF;
out vec3 normalF;

out vec3 matambF;
out vec3 matdiffF;
out vec3 matspecF;
out float matshinF;

void main(){

    viewF = view;
    TGF = TG;
    vertexF = vertex;
    normalF = normal;
    matambF = matamb;
    matdiffF = matdiff;
    matspecF = matspec;
    matshinF = matshin;
    
    //fcolor = matdiff;
    gl_Position = proj * view * TG * vec4 (vertex,1);

}
