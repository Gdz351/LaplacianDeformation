#version 330 core
out vec4 FragColor;


struct Lighting {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  


in vec3 FragPos;  
in vec3 Normal;  
in float gState;

uniform vec3 viewPos;

vec3 CalcDirLight(Lighting light,vec3 obj_color, vec3 normal, vec3 viewDir);


void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 obj_color=vec3(1.0);
	if(gState>1){
    	obj_color = vec3(0.6,0.0,0.0);
    }
    else if(gState>0){
    	obj_color = vec3(0.0,0.6,0.0);
    }
	Lighting dirLight;
	dirLight.direction =vec3(-2.2f, -1.0f, -2.3f);
	dirLight.ambient =vec3(0.3,0.3,0.3);
	dirLight.diffuse =vec3(0.8,0.8,0.8);
	dirLight.specular =vec3(0.5,0.5,0.5);
	
    vec3 result = CalcDirLight(dirLight,obj_color, norm, viewDir); 
    FragColor = vec4(result, 1.0);
	//FragColor = vec4(1,1,1, 1.0);
}

vec3 CalcDirLight(Lighting light,vec3 obj_color, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 500);

    vec3 ambient = light.ambient * obj_color;
    vec3 diffuse = light.diffuse * diff * obj_color;
    vec3 specular = light.specular * spec * obj_color;
    return (ambient + diffuse + specular);
}


