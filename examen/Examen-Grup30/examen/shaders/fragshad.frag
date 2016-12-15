#version 330 core

in vec3 fcolor;
out vec4 FragColor;

in vec3 vertexf;
in vec3 normalf;
in vec3 matambf;
in vec3 matdifff;
in vec3 matspecf;
in float matshinf;

vec3 colFocus = vec3(1, 1, 1);
vec3 llumAmbient = vec3(0.2, 0.2, 0.2);
vec3 posFocus = vec3(0, 0, 0);  // en SCA

uniform mat4 view;
uniform mat4 TG;

vec3 Lambert (vec3 NormSCO, vec3 L) 
{
    // S'assumeix que els vectors que es reben com a paràmetres estan normalitzats

    // Inicialitzem color a component ambient
    vec3 colRes = llumAmbient * matambf;

    // Afegim component difusa, si n'hi ha
    if (dot (L, NormSCO) > 0)
      colRes = colRes + colFocus * matdifff * dot (L, NormSCO);
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

    if ((dot(R, V) < 0) || (matshinf == 0))
      return colRes;  // no hi ha component especular
    
    // Afegim la component especular
    float shine = pow(max(0.0, dot(R, V)), matshinf);
    return (colRes + matspecf * colFocus * shine); 
}

void main()
{	
	//FragColor = vec4(fcolor,1);
	
    vec4 vertex_sco = view*TG * vec4(vertexf,1);
    mat3 NormalMatrix = inverse(transpose(mat3(view*TG)));
    vec3 normal_sco = NormalMatrix * normalf;
    normal_sco = normalize(normal_sco);
    
    vec3 L = posFocus - vertex_sco.xyz;
    L = normalize(L);
    
    FragColor = vec4(Phong(normal_sco, L, vertex_sco),1);
}
