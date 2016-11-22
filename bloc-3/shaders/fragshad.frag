#version 330 core

//in vec3 fcolorF;
out vec4 FragColor;
in mat4 viewF;
in mat4 TGF;

in vec3 vertexF;
in vec3 normalF;

in vec3 matambF;
in vec3 matdiffF;
in vec3 matspecF;
in float matshinF;

uniform vec3 colFocus;// = vec3(0.8, 0.8, 0.8);
vec3 llumAmbient = vec3(0.2, 0.2, 0.2);
uniform vec4 posFocus;// = vec3(0, 1, 1);  // en SCA


vec3 Lambert (vec3 NormSCO, vec3 L) 
{
    // S'assumeix que els vectors que es reben com a paràmetres estan normalitzats

    // Inicialitzem color a component ambient
    vec3 colRes = llumAmbient * matambF;

    // Afegim component difusa, si n'hi ha
    if (dot (L, NormSCO) > 0)
      colRes = colRes + colFocus * matdiffF * dot (L, NormSCO);
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

    if ((dot(R, V) < 0) || (matshinF == 0))
      return colRes;  // no hi ha component especular
    
    // Afegim la component especular
    float shine = pow(max(0.0, dot(R, V)), matshinF);
    return (colRes + matspecF * colFocus * shine); 
}

void main(){
    vec4 vertex_sco = viewF*TGF * vec4(vertexF,1);
    mat3 NormalMatrix = inverse(transpose(mat3(viewF*TGF)));
    vec3 normal_sco = NormalMatrix * normalF;
    normal_sco = normalize(normal_sco);
    
    vec4 L = posFocus - vertex_sco;
    L = normalize(L);
    
    FragColor = vec4(Phong(normal_sco, L.xyz, vertex_sco), 1);

    //FragColor = vec4(fcolor,1);
}
