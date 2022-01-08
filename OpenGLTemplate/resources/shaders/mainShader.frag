#version 400 core


//in vec3 vColour;			// Interpolated colour using colour calculated in the vertex shader
in vec2 vTexCoord;			// Interpolated texture coordinate using texture coordinate from the vertex shader

in vec3 n;
in vec4 p;

out vec4 vOutputColour;		// The output colour

uniform sampler2D sampler0;  // The texture sampler
uniform samplerCube CubeMapTex;
uniform bool bUseTexture;    // A flag indicating if texture-mapping should be applied
uniform bool renderSkybox;
uniform bool renderSpotLights;
in vec3 worldPosition;

// Structure holding light information:  its position as well as ambient, diffuse, and specular colours
struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
};

struct SpotLightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
	vec3 direction; 
	float exponent; 
	float cutoff;
};


// Structure holding material information:  its ambient, diffuse, and specular colours, and shininess
struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

// Lights and materials passed in as uniform variables from client programme
uniform LightInfo light1; 
uniform SpotLightInfo spotlight1;
uniform MaterialInfo material1; 



vec3 BlinnPhongSpotlightModel(SpotLightInfo light, vec4 p, vec3 n) {
	vec3 s = normalize(vec3(light.position - p)); 
	float angle = acos(dot(-s, light.direction)); 
	float cutoff = radians(clamp(light.cutoff, 0.0, 90.0)); 
	vec3 ambient = light.La * material1.Ma; 
	if (angle < cutoff) { 
		float spotFactor = pow(dot(-s, light.direction), light.exponent); 
		vec3 v = normalize(-p.xyz); 
		vec3 h = normalize(v + s);
		float sDotN = max(dot(s, n), 0.0); 
		vec3 diffuse = light.Ld * material1.Md * sDotN; 
		vec3 specular = vec3(0.0); 
		if (sDotN > 0.0) 
			specular = light.Ls * material1.Ms * pow(max(dot(h, n), 0.0), material1.shininess);
		return ambient + spotFactor * (diffuse + specular); 
	} else
		return ambient;
}

vec3 PhongModel(vec4 p, vec3 n)
{
	vec3 s = normalize(vec3(light1.position - p));
	vec3 v = normalize(-p.xyz);
	vec3 r = reflect(-s, n);
	vec3 ambient = light1.La * material1.Ma;
	float sDotN = max(dot(s, n), 0.0f);
	vec3 diffuse = light1.Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0f);
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (sDotN > 0.0f) 
		specular = light1.Ls * material1.Ms * pow(max(dot(r, v), 0.0f), material1.shininess + eps);
	

	return ambient + diffuse + specular;

}

void main()
{
	vec3 vColour =  PhongModel(p, normalize(n));

	vColour = vColour + BlinnPhongSpotlightModel(spotlight1 , p , normalize(n));	

	if (renderSkybox) {
		vOutputColour = texture(CubeMapTex, worldPosition);

	} else {

		// Get the texel colour from the texture sampler
		vec4 vTexColour = texture(sampler0, vTexCoord);	

		if (bUseTexture)
		{
			vOutputColour = vTexColour*vec4(vColour, 1.0f);	// Combine object colour and texture 
		}
		else
			vOutputColour = vec4(vColour, 1.0f);	// Just use the colour instead
	}
	
	
}
