#version 150

uniform sampler2D texture;

// Constants for the Ray Marching Algorithm
const float MAX_DISTANCE = 500.;
const float TOLERANCE = 0.01;
const int MAX_STEPS = 10000;
const float PI = 3.14159265359;

vec4 difCol = vec4(1., 1., 1., 1.);

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

uniform vec3 camPosition = vec3(0, 1, 0);
uniform vec3 camRotation = vec3(0);

uniform float time = 0;

// Used for checking and returning the distance to the scene
// and the color at that point in a nice package
struct SceneInfo {
    float distToScene;
    vec4 color;
};

struct Shape {
    vec3 position;
    vec3 rotation;
    vec4 color;
};

struct Sphere {
    Shape base;
    float radius;
};

struct Box {
    Shape base;
    vec3 size;
};

uniform Sphere spheres[10];
uniform int numSpheres = 0;

uniform Box boxes[10];
uniform int numBoxes = 0;

float sphereSDF(vec3 p, vec3 pos, vec3 rot, float r) {
    return length(rotateXYZ(p - pos, rot)) - r;
}

float boxSDF(vec3 p, vec3 pos, vec3 rot, vec3 size) {
    vec3 q = abs(rotateXYZ(p - pos, rot)) - size;
    return length(max(q, 0.)) + min(max(q.x, max(q.y, q.z)), 0.);
}

SceneInfo CheckScene(SceneInfo scene, SceneInfo shape) {
    if (shape.distToScene < scene.distToScene) {
        scene.distToScene = shape.distToScene;
        scene.color = shape.color;
    }

    return scene;
}

SceneInfo SceneSDF(vec3 p) {

    SceneInfo scene;

    scene.distToScene = p.y;
    scene.color = vec4(1, 1, 0, 1);

    SceneInfo shape;

    // Upper plane
    shape.distToScene = -p.y + 60;
    shape.color = vec4(0.7, 0.9, 1., 1.);

    scene = CheckScene(scene, shape);

    for (int i = 0; i < numSpheres; i++) {
        shape.distToScene =  sphereSDF(p, spheres[i].base.position, spheres[i].base.rotation, spheres[i].radius);
        shape.color = spheres[i].base.color;

        scene = CheckScene(scene, shape);
    }

    for (int i = 0; i < numBoxes; i++) {
        shape.distToScene =  boxSDF(p, boxes[i].base.position, boxes[i].base.rotation, boxes[i].size);
        shape.color = boxes[i].base.color;

        scene = CheckScene(scene, shape);
    }

	return scene;
}

float RayMarch(vec3 ro, vec3 rd, out vec4 dCol) {
	float distTotal = 0.;

    vec4 accCol = vec4(0.);
    
    for (int i = 0; i < MAX_STEPS; i++)
    {   
        // March away from origin
        vec3 p = ro + rd * distTotal;
        
        // Get distance to the scene
        SceneInfo scene = SceneSDF(p);
        float dist = scene.distToScene;

        // Check if the ray has hit
        if (dist < TOLERANCE)
        {
           
            accCol.rgb += scene.color.rgb * scene.color.a * (1. - accCol.a);

            accCol.a += scene.color.a * (1. - accCol.a);

            if (scene.color.a <= 1 - TOLERANCE) {
                distTotal += 1.5 * TOLERANCE;
                
            }

            if (accCol.a >= 1. - TOLERANCE) {
                accCol.a = 1.;

                dCol = accCol;
                return distTotal;
             }
        }
        
        // Update travelled distance if no hit
        distTotal += dist;
        
        if (distTotal > MAX_DISTANCE)
        {
            // Color of sky
            dCol = vec4(0.7, 0.9, 1., 1.);
            return distTotal;
        }
    }

    dCol = vec4(0., 0., 0., 1.);
    return distTotal;
}

vec3 getNormal(vec3 p) {
    float dist = SceneSDF(p).distToScene;
    vec2 e = vec2(TOLERANCE, 0);

    vec3 n = dist - vec3(
        SceneSDF(p-e.xyy).distToScene,
        SceneSDF(p-e.yxy).distToScene,
        SceneSDF(p-e.yyx).distToScene
    );

    return normalize(n);
}

vec4 getLight(vec3 p, vec4 color) {
    vec3 lightPos = vec3(p.x, 20., p.z);
    vec3 l = normalize(lightPos - p);

    vec3 n = getNormal(p);

    float dif = smoothstep(0., 1., dot(n, l));

    // Just an empty variable for the sake of using the
    // RayMarch function
    vec4 col = vec4(1.);

    float dist = RayMarch(p + n * TOLERANCE * 2, l, col);

    if (dist < length(lightPos - p))
    {
        dif *= 0.5;
    }

    return vec4(color.rgb * dif, color.a);
}

void main() {
    vec2 dim = vec2(800, 600);

    vec2 uv = (vec2(gl_FragCoord.xy) / dim) - 0.5;

    vec3 rd = normalize(vec3(uv.x, uv.y, 1));
    rd = rotateXYZ(rd, camRotation);

    float dist = RayMarch(camPosition, rd, difCol);

    vec3 pos = camPosition + rd * dist;

    vec4 col = getLight(pos, difCol);

	gl_FragColor = col * difCol;
}