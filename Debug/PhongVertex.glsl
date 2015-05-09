#version 330

const int MAX_POINT_LIGHTS = 1;

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;

uniform mat4 fullTransformMatrix;


out vec3 theColor;

uniform vec3 eyePos;
uniform vec3 baseColor;
uniform vec3 ambientLight;

vec3 direction = vec3(1.0,-0.5,-0.5);
vec3 dirColor = vec3(0.1,0.7,0.1);
vec3 LIGHTDIRECTIONYO =vec3(-1.0,-0.5,0.5);

float constant=0.0f;
float linear=0.0f;
float exponent=0.1f;

vec3 calcDirectionalLight(vec3 lightdirection, vec3 normal, vec3 color)
{

	
	
	float diffuseFactor = dot(-lightdirection,normal);

	vec3 diffuseColor = vec3(0,0,0); 
	vec3 specularColor = vec3(0,0,0);

	if(diffuseFactor > 0)
	{
		diffuseColor = vec3(1.0,1.0,1.0) * diffuseFactor * color;


		vec3 directionToEye = normalize(eyePos - position);
		vec3 reflectDirection = normalize(reflect(lightdirection,normal));

		float specularFactor = dot(directionToEye,reflectDirection);
		specularFactor = pow(specularFactor,16);

		if(specularFactor > 0)
		{
		
			specularColor = vec3(1.0,1.0,1.0) * specularFactor;

		}

	
	}

	return diffuseColor  + specularColor;
}

vec3 calcPointLight(vec3 LightPosition, vec3 normal)
{
	vec3 lightDirection = position - LightPosition;
	float distanceToPoint = length(lightDirection);


	if(distanceToPoint > 100)
	{
		return vec3(0,0,0);
	}

	lightDirection = normalize(lightDirection);

	vec3 color = calcDirectionalLight(lightDirection,normal,vec3(0.0,1.0,0.0));

	float attenuation = constant + (linear*distanceToPoint) + (exponent*distanceToPoint*distanceToPoint) + 0.001f;
	
	return color/attenuation;
}




void main()
{
	vec3 totalLight = ambientLight;

	vec3 color = baseColor; //vec3(0.3,0.5,0.4)

	vec4 v = vec4(position,1.0);
	gl_Position = fullTransformMatrix * v;


	totalLight +=  calcDirectionalLight(direction,normal,dirColor) ;

	
	totalLight += calcPointLight(eyePos, normal);
	

	theColor = baseColor * totalLight;

}