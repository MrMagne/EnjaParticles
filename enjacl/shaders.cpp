#define STRINGIFY(A) #A

//this code heavily based off NVIDIA's oclParticle example from the OpenCL SDK

const char* vertex_shader_source = STRINGIFY(

uniform float pointRadius;  // point size in world space
//uniform float pointScale;   // scale to calculate size in pixels
//uniform float densityScale;
//uniform float densityOffset;
varying vec3 posEye;        // position of center in eye space

void main()
{
    posEye = vec3(gl_ModelViewMatrix * vec4(gl_Vertex.xyz, 1.0));
    float dist = length(posEye);
    //gl_PointSize = pointRadius * (pointScale / dist);
    gl_PointSize = pointRadius * (1.0f / dist);

    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);

    gl_FrontColor = gl_Color;
}


);

const char* fragment_shader_source = STRINGIFY(

uniform float pointRadius;  // point size in world space
varying vec3 posEye;        // position of center in eye space

void main()
{
    const vec3 lightDir = vec3(0.577, 0.577, 0.577);
    const float shininess = 40.0;

    // calculate normal from texture coordinates
    vec3 n;
    n.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    float mag = dot(n.xy, n.xy);
    if (mag > 1.0) discard;   // kill pixels outside circle
    n.z = sqrt(1.0-mag);

    // point on surface of sphere in eye space
    vec3 spherePosEye = posEye + n*pointRadius;

    // calculate lighting
    float diffuse = max(0.0, dot(lightDir, n));
    
    vec3 v = normalize(-spherePosEye);
    vec3 h = normalize(lightDir + v);
    float specular = pow(max(0.0, dot(n, h)), shininess);
    gl_FragColor = gl_Color * diffuse + specular;
    //want to play with this later
    gl_FragColor.w = 1.f;
    
    //gl_FragColor = gl_Color * diffuse;
}


);