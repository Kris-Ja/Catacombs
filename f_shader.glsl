#version 330

out vec4 pixelColor; //Output variable. Almost final pixel color.

uniform sampler2D textureMap0;
uniform sampler2D textureMap1;
uniform sampler2D textureMap2;
uniform sampler2D textureMap3;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform int flashlight; // enabled/disabled flashlight
uniform vec3 lightPos[20]; // positions of light sources in world space
uniform int n_lights; // number of light sources
uniform int obj_type;

//Varying variables
in vec4 l;
in vec4 v;
in vec2 iTexCoord0;
in vec4 pos; // fragment coordinates in world space
in vec4 light[20];

vec2 parallax(vec2 tx, float height, float accuracy, vec4 viewer){
	vec2 tc = tx;
	return tc;
	if(obj_type > 2) return tc;
	vec2 textureincrement = -viewer.xy/accuracy;
	float heightincrement = -viewer.z/accuracy;

	
	float hc = height;

	float ht = height*texture(textureMap2,tc).r;

	if(viewer.z<0.0001){
		discard;
	}else{
		while(hc>ht){
			tc = tc + textureincrement;
			if(tc.y<0){
				tc.y+=1;
			}
			if(tc.y>1){
				tc.y-=1;
			}
			if(tc.x>1){
				tc.x-=1;
			}
			if(tc.x<0){
				tc.x+=1;
			}
			
			if(tc.x<0 || tc.x>1 || tc.y<0 || tc.y>1){
				break;
			}
			hc = hc + heightincrement;
			ht = height * texture(textureMap2,tc).r;
		}
		if(tc.x<0 || tc.x>1 || tc.y<0 || tc.y>1){
			discard;
		}
	}

	return tc;
}

void main(void) {
	//Normalized, interpolated vectors
	vec4 mv = normalize(v);
	vec2 tc = parallax(iTexCoord0,0.1,1000,mv);
	vec4 mn = normalize(vec4(texture(textureMap1, tc).xyz*2-1, 0));

	//Surface parameters
	vec4 kd = texture(textureMap0, tc);
	
	//Lighting model computation
	vec4 pc = 0.02 * vec4(kd.rgb, kd.a); //pixel color (ambient)
	if(obj_type != 7) pc *= texture(textureMap3, tc);
	
	int n = n_lights<20 ? n_lights : 20;
	for(int i=0; i<n; i++){
		float intensity;
		float distance = length(vec4(lightPos[i], 1) - pos);
		float attenuation = 1.0/(1.0 + 0.7 * distance + 1.2 * distance * distance);
		if(obj_type != 4){
			float theta = dot(normalize(lightPos[i].xyz-pos.xyz), vec3(0.0, 1.0, 0.0));
			float epsilon = 0.82 - 0.64;
			intensity = clamp((theta - 0.64)/epsilon, 0.0, 1.0);
		}
		else{
			 intensity = 1.0;
		}
		vec4 ks = vec4(1, 1, 0.7, 1);//vec4(0.9, 0.0, 0.3, 1);
		vec4 ml = normalize(light[i]);
		vec4 mr = reflect(-ml, mn); //Reflected vector
		float nl = clamp(dot(mn, ml), 0, 1);
		float rv = pow(clamp(dot(mr, mv), 0, 1), 60);
		pc += attenuation * intensity * vec4(kd.rgb * nl, kd.a) + 0.7 * attenuation * intensity * vec4(ks.rgb * rv, 0);
	}
	
	if(flashlight == 1){
		vec4 ks = vec4(1, 1, 1, 1);//vec4(0.9, 0.0, 0.3, 1);
		vec4 ml = normalize(l);
		vec4 mr = reflect(-ml, mn); //Reflected vector
		float nl = clamp(dot(mn, ml), 0, 1);
		float rv = pow(clamp(dot(mr, mv), 0, 1), 60);
		float distance = length(inverse(V)*vec4(0, 0, 0, 1) + vec4(0, -0.4, 0, 0) - pos);
		float attenuation = 1.0/(1.0 + 0.4 * distance + 0.4 * distance * distance);
		pc += attenuation * vec4(kd.rgb * nl, kd.a) + 0.7 * attenuation * vec4(ks.rgb * rv, 0);
	}
	pixelColor = pc;
}
