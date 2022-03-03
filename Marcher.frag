#version 150

uniform sampler2D texture;

struct Shape {
    vec3 position;
    vec3 rotation;
    vec4 color;
    int type;

    float signedDistance;
};

struct Sphere {
    Shape base;
    float radius;
};

struct Box {
    Shape base;
    vec3 size;
};

uniform vec3 camPosition = vec3(0, 0, 0);

uniform float shapeTypes[100];
Sphere spheres[30];

// Constants for the Ray Marching Algorithm
const float MAX_DISTANCE = 1000.;
const float TOLERANCE = 0.001;
const int MAX_STEPS = 30;

vec4 difCol = vec4(1., 1., 1., 1.);

void AddShapes(vec3 p) {
    Sphere sphere;
    sphere.base.position = vec3(0, 0, 5);
    sphere.base.rotation = vec3(0);
    sphere.base.color = vec4(1, 0, 0, 1);
    sphere.base.type = 0;
    sphere.radius = 1.;
    sphere.base.signedDistance = length(p - sphere.base.position) - sphere.radius;
    spheres[0] = sphere;
}

float SceneSDF(vec3 p, out vec4 col) {

    AddShapes(p);

    float distToScene = p.y + 1;
    col = vec4(1, 1, 0, 1);

    Sphere sphere;
    sphere.base.position = vec3(0, 0, 4);
    sphere.base.rotation = vec3(0);
    sphere.base.color = vec4(1, 0, 0, 1);
    sphere.base.type = 0;
    sphere.radius = 1.;
    sphere.base.signedDistance = length(p - sphere.base.position) - sphere.radius;

    Box box;
    box.base.position = vec3 (1, 1, 2);
    box.base.rotation = vec3(1, 0, 0);
    box.base.color = vec4(0, 1, 0, 1);
    box.base.type = 2;
    box.size = vec3(1, 1, 1);
    box.base.signedDistance = length(max(abs(p - box.base.position) - box.size, 0.));

    for (int i = 0; i < 30; i++) {
        if (shapeTypes[i] == -1) {
            break;
        }

        if (shapeTypes[i] == 1) {
            if (spheres[i].base.signedDistance < distToScene) {
                distToScene = spheres[i].base.signedDistance;
                col = spheres[i].base.color;
            }
        }
    }

    if (box.base.signedDistance < distToScene) {
        distToScene = box.base.signedDistance;
        col = box.base.color;
    }

	return distToScene;
}

float RayMarch(vec3 ro, vec3 rd, out vec4 dCol) {
    vec4 color;
	float distTotal = 0.;
    
    for (int i = 0; i < MAX_STEPS; i++)
    {   
        // March away from origin
        vec3 p = ro + rd * distTotal;
        
        // Get distance to the scene
        float dist = SceneSDF(p, color);

        // Check if the ray has hit
        if (dist < TOLERANCE)
        {
            dCol = color;
            return distTotal;
        }
        
        // Update travelled distance if no hit
        distTotal += dist;
        
        if (distTotal > MAX_DISTANCE)
        {
            break;
        }
    }

    dCol = vec4(1.);
    return distTotal;
}

void main() {
    vec2 dim = vec2(800, 600);

    vec2 uv = (vec2(gl_FragCoord.xy) / vec2(800, 600)) - 0.5;

    vec3 rd = normalize(vec3(uv.x, uv.y, 1));

    float dist = RayMarch(camPosition, rd, difCol);

    vec3 pos = rd * dist;

	gl_FragColor = 2 * difCol / dist;
}