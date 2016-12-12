#version 330 core

in vec3 vertex;
in vec3 normal;

in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;
//per a saber si pintem la vaca
uniform int vaca;
uniform int escena;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;

// Valors per als components que necessitem dels focus de llum
vec3 colFocus = vec3(0, 1, 1);
vec3 llumAmbient = vec3(0.2, 0.2, 0.2);
vec3 posFocus = vec3(1, 1, 1);  // en SCA

vec3 vacaamb = vec3(0.2,0.2,0.2);
vec3 vacadiff = vec3(0.3,0.3,0.3);
vec3 vacaspec = vec3(1,1,1);
float vacashin = 100;

vec3 amb, diff, spec;
float shin;

out vec3 fcolor;

vec3 Lambert (vec3 NormSCO, vec3 L) 
{
    // S'assumeix que els vectors que es reben com a paràmetres estan normalitzats

    // Inicialitzem color a component ambient
    vec3 colRes = llumAmbient * amb;

    // Afegim component difusa, si n'hi ha
    if (dot (L, NormSCO) > 0)
      colRes = colRes + colFocus * diff * dot (L, NormSCO);
    return (colRes);
}

vec3 Phong (vec3 NormSCO, vec3 L, vec4 vertSCO) 
{
    // Els vectors estan normalitzats

    // Inicialitzem color a Lambert
    vec3 colRes = Lambert (NormSCO, L);

    // Calculem R i V
    if (dot(NormSCO,L) < 0)
      return colRes;  // no hi ha component especular

    vec3 R = reflect(-L, NormSCO); // equival a: normalize (2.0*dot(NormSCO,L)*NormSCO - L);
    vec3 V = normalize(-vertSCO.xyz);

    if ((dot(R, V) < 0) || (shin == 0))
      return colRes;  // no hi ha component especular
    
    // Afegim la component especular
    float shine = pow(max(0.0, dot(R, V)), shin);
    return (colRes + spec * colFocus * shine); 
}

void main(){

    gl_Position = proj * view * TG * vec4 (vertex, 1.0);
    
    ////
    if (vaca != 0){
        amb = vacaamb;
        diff = vacadiff;
        spec = vacaspec;
        shin = vacashin;
    }
    else {
        amb = matamb;
        diff = matdiff;
        spec = matspec;
        shin = matshin;
    }
    ////
    
    vec4 vertex_sco = view*TG * vec4(vertex,1);
    mat3 NormalMatrix = inverse(transpose(mat3(view*TG)));
    vec3 normal_sco = NormalMatrix * normal;
    normal_sco = normalize(normal_sco);
    
    if (escena != 0){ 
        posFocus = (view * vec4(posFocus,1)).xyz;
        colFocus = vec3(1,1,1);
        llumAmbient = vec3(0,0,0);
    }
    
    vec3 L = posFocus - vertex_sco.xyz;
    L = normalize(L);
    
    fcolor = Phong(normal_sco, L, vertex_sco);
}
