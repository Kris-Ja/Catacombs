#version 330

//Uniform variables
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 lightPos[20]; // positions of light sources in world space
uniform int n_lights;

//Attributes
in vec4 vertex; //Vertex coordinates in model space
in vec2 texCoord0;
in vec4 c1;
in vec4 c2;
in vec4 c3;

//Varying variables
out vec4 l;
out vec4 v;
out vec2 iTexCoord0;
out vec4 pos;
out vec4 light[20];

void main(void) {
    vec4 lp = vec4(0, 0, 0, 1);
    mat4 invTBN = mat4(c1, c2, c3,vec4(0,0,0,1));
    l = normalize(invTBN*(inverse(M)*(inverse(V)*lp + vec4(0, -0.4, 0, 0)) - vertex));
    v = normalize(invTBN*(inverse(V*M)*vec4(0,0,0,1)-vertex));
    iTexCoord0 = texCoord0;
    
    int n = n_lights<20 ? n_lights : 20;
    for(int i=0; i<n; i++){
		light[i] = normalize(invTBN*(inverse(M)*vec4(lightPos[i], 1) - vertex));
	}
    
    gl_Position = P * V * M * vertex;
    pos = M * vertex;
}
