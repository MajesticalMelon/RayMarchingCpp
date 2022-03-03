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

uniform vec3 camPosition = vec3(0, 1, 0);

uniform float shapeTypes[100];
Sphere spheres[30];

// Constants for the Ray Marching Algorithm
const float MAX_DISTANCE = 30.;
const float TOLERANCE = 0.001;
const int MAX_STEPS = 1000;

vec4 difCol = vec4(1., 1., 1., 1.);

<<<<<<< HEAD
vec3 rotateX(vec3 p, float theta) {
    return vec3(
        p.x,
        p.y * cos(theta) - p.z * sin(theta),
        p.y * sin(theta) + p.z * cos(theta)
    );
}

vec3 rotateY(vec3 p, float theta) {
    return vec3(
        p.x * cos(theta) + p.z * sin(theta),
        p.y,
        -p.x * sin(theta) + p.z * cos(theta)
    );
}

vec3 rotateZ(vec3 p, float theta) {
    return vec3(
        p.x * cos(theta) - p.y * sin(theta),
        p.x * sin(theta) + p.y * cos(theta),
        p.z
    );
}

vec3 rotateXYZ(vec3 p, vec3 rot) {
    vec3 rotated = p;

    rotated = rotateX(rotated, rot.x);
    rotated = rotateY(rotated, rot.y);
    rotated = rotateZ(rotated, rot.z);

    return rotated;
}

=======
>>>>>>> Input
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

<<<<<<< HEAD
    Box box;
    box.base.position = vec3 (0, 1, 6);
    box.base.rotation = vec3(5, 2, 0);
    box.base.color = vec4(0, 1, 0, 1);
    box.base.type = 2;
    box.size = vec3(1, 1, 1);
    vec3 q = abs(rotateXYZ(p - box.base.position, box.base.rotation)) - box.size;
    box.base.signedDistance = length(max(q, 0.)) + min(max(q.x, max(q.y, q.z)), 0.);

//    for (int i = 0; i < 30; i++) {
//        if (shapeTypes[i] == -1) {
//            break;
//        }
//
//        if (shapeTypes[i] == 1) {
//            if (spheres[i].base.signedDistance < distToScene) {
//                distToScene = spheres[i].base.signedDistance;
//                col = spheres[i].base.color;
//            }
//        }
//    }

    if (box.base.signedDistance < distToScene) {
        distToScene = box.base.signedDistance;
        col = box.base.color;
=======
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
>>>>>>> Input
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

vec3 getNormal(vec3 p) {
    vec4 r;
    float dist = SceneSDF(p, r);
    vec2 e = vec2(.01, 0);

    vec3 n = dist - vec3(
        SceneSDF(p-e.xyy, r),
        SceneSDF(p-e.yxy, r),
        SceneSDF(p-e.yyx, r)
    );

    return normalize(n);
}

float getLight(vec3 p) {
    vec3 lightPos = vec3(-5, 2, -1);
    vec3 l = normalize(lightPos - p);

    vec3 n = getNormal(p);

    float dif = clamp(dot(n, l), 0., 1.);

    return dif;
}

void main() {
    vec2 dim = vec2(800, 600);

    vec2 uv = (vec2(gl_FragCoord.xy) / vec2(800, 600)) - 0.5;

    vec3 rd = normalize(vec3(uv.x, uv.y, 1));

    float dist = RayMarch(camPosition, rd, difCol);

    vec3 pos = camPosition + rd * dist;

    float dif = getLight(pos);
    vec3 col = vec3(dif);

	gl_FragColor = vec4(col.x, col.y, col.z, 1);
}