#version 150

uniform sampler2D texture;

// Constants for the Ray Marching Algorithm
const float MAX_DISTANCE = 100.;
const float TOLERANCE = 0.001;
const int MAX_STEPS = 10000;
const float PI = 3.14159265359;

// Shape types
const int SPHERE = 1;
const int BOX = 2;

// SDF Ops
const int UNION = 1;
const int INTERSECTION = 2;
const int SUBTRACT = 3;

vec4 difCol = vec4(0., 0., 0., 1.);

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

uniform vec2 windowDimensions = vec2(800, 600);
uniform vec3 camPosition = vec3(0, 1, 0);
uniform vec3 camRotation = vec3(0);

uniform float time = 0;

// Used for checking and returning the distance to the scene
// and the color at that point in a nice package

struct Shape {
    vec3 position;
    vec3 rotation;
    vec4 color;
    float signedDistance;
    vec3 param1;
    vec3 param2;

    // Used for combining, subtracting, intersecting, etc. two shapes
    int operation;
    int operandIndex;
    bool checkShape;

    int type;

    // material properties
    float metallic;
};

struct Sphere {
    Shape base;
    float radius;
};

struct Box {
    Shape base;
    vec3 size;
};

uniform Shape shapes[20];

uniform Sphere spheres[10];
uniform int numSpheres = 0;

uniform Box boxes[10];
uniform int numBoxes = 0;

// Copied from https://www.shadertoy.com/view/Ml3Gz8
float smoothMin(float a, float b, float k) {
    float h = clamp(0.5 + 0.5*(a-b)/k, 0.0, 1.0);
    return mix(a, b, h) - k*h*(1.0-h);
}

vec4 smoothColor(float d1, float d2, vec4 a, vec4 b, float k) {
    float h = clamp(0.5 + 0.5*(d2 - d1)/k, 0.0, 1.0);
    return mix(b, a, h) - k*h*(1.0-h);
}

// Signed Distance Fields
float sphereSDF(vec3 p, vec3 pos, vec3 rot, float r) {
    return length(rotateXYZ(p - pos, rot)) - r;
}

float boxSDF(vec3 p, vec3 pos, vec3 rot, vec3 size) {
    vec3 q = abs(rotateXYZ(p - pos, rot)) - size;
    return length(max(q, 0.)) + min(max(q.x, max(q.y, q.z)), 0.);
}

// Shape operations
Shape combine(Shape s1, Shape s2) {
    Shape returned = s1.signedDistance < s2.signedDistance ? s1 : s2;
    returned.signedDistance = smoothMin(s1.signedDistance, s2.signedDistance, 0.2);
    returned.color = smoothColor(s1.signedDistance, s2.signedDistance, s1.color, s2.color, 0.2);
    returned.checkShape = true;
    return returned;
}

Shape intersection(Shape s1, Shape s2) {
    Shape returned = s1.signedDistance > s2.signedDistance ? s1 : s2;
    returned.checkShape = true;
    return returned;
}

Shape subtract(Shape s1, Shape s2) {
    Shape negS1 = s1;
    negS1.signedDistance = -s1.signedDistance;

    Shape returned = negS1.signedDistance > s2.signedDistance ? negS1 : s2;
    returned.checkShape = true;
    return returned;
}

// Compares two objects in the scene
Shape CheckScene(Shape scene, Shape check) {
    return check.signedDistance < scene.signedDistance ? check : scene;
}

float assignSDF(vec3 p, Shape s) {
    float sdf = 0;

    // Sphere
    if (s.type == SPHERE) {
        return sphereSDF(
            p,
            s.position,
            s.rotation,
            s.param1.x
        );
    }

    // Box
    if (s.type == BOX) {
        return boxSDF(
            p,
            s.position,
            s.rotation,
            s.param1
        );
    }

    return sdf;
}

Shape operateSDF(Shape s1, Shape s2) {

    if (s1.operation == UNION) {
        return combine(s1, s2);
    } else if (s1.operation == INTERSECTION) {
        return intersection(s1, s2);
    } else if (s1.operation == SUBTRACT) {
        return subtract(s1, s2);
    }

    return s1;
}

Shape SceneSDF(vec3 p) {

    Shape scene;

    scene.signedDistance = p.y;
    scene.color = vec4(1, 1, 1, 1);
    scene.metallic = 0;

    Shape check;

    for (int i = 0; i < 20; i++) {
        check = shapes[i];

        if (!check.checkShape) {
            continue;
        }

        if (check.type <= 0) {
            break;
        }

        // Assign sdf of this shape
        check.signedDistance = assignSDF(p, check);

        // SDF operations
        if (check.operation > 0) {
            Shape opd = shapes[check.operandIndex];
            opd.signedDistance = assignSDF(p, opd);

            check = operateSDF(check, opd);
        }

        scene = CheckScene(scene, check);
    }

	return scene;
}

// Used for traversing through the scene until a solid object is hit
float RayMarch(vec3 ro, vec3 rd, out vec4 dCol) {
	float distTotal = 0.;

    vec4 accCol = vec4(0.);
    
    for (int i = 0; i < MAX_STEPS; i++)
    {   
        // March away from origin
        vec3 p = ro + rd * distTotal;
        
        // Get distance to the scene
        Shape scene = SceneSDF(p);
        float dist = scene.signedDistance;

        // Check if the ray has hit
        if (dist < TOLERANCE)
        {
            accCol.rgb += scene.color.rgb * scene.color.a * (1. - accCol.a);

            accCol.a += scene.color.a * (1. - accCol.a);

            if (scene.color.a <= 1 - TOLERANCE) {
                // Add more to the distance to get it through the object
                distTotal += TOLERANCE;
            }

            if (accCol.a >= 1. - TOLERANCE) {
                accCol.a = 1.;

                dCol = accCol;
                return distTotal;
             }
        }
        
        // Update travelled distance if no hit
        distTotal += abs(dist);
        
        if (distTotal > MAX_DISTANCE)
        {
            if (accCol.a < TOLERANCE) {
                dCol = vec4(0, 0, 0, 1);
                return distTotal;
            }

            break;
        }
    }

    accCol.a = 1.;
    dCol = accCol;
    return distTotal;
}

// Used for proper lighting of tranparent objects
float lightMarch(vec3 ro, vec3 rd, vec3 lightPos, out vec4 dCol, out bool hitTransparentObject) {
	float distTotal = 0.;

    vec4 accCol = vec4(0.);

    hitTransparentObject = false;
    
    for (int i = 0; i < MAX_STEPS; i++)
    {   
        // March away from origin
        vec3 p = ro + rd * distTotal;
        
        // Get distance to the scene
        Shape scene = SceneSDF(p);
        float dist = scene.signedDistance;

        // Check if the ray has hit
        if (dist < TOLERANCE)
        {           
            accCol.rgb += scene.color.rgb * scene.color.a * (1. - accCol.a);

            accCol.a += scene.color.a * (1. - accCol.a);

            if (scene.color.a <= 1 - TOLERANCE) {
                // Add more to the distance to get it through the object
                distTotal += TOLERANCE;

                hitTransparentObject = true;
            }

            if (accCol.a >= 1. - TOLERANCE) {
                accCol.a = 1.;

                dCol = accCol;
                return distTotal;
             }
        }
        
        // Update travelled distance if no hit
        distTotal += abs(dist);
        
        if (distTotal > length(lightPos - ro))
        {
            break;
        }
    }

    accCol.a = 1;
    dCol = accCol;
    return distTotal;
}

vec3 getNormal(vec3 p) {
    float dist = SceneSDF(p).signedDistance;
    vec2 e = vec2(TOLERANCE, 0);

    vec3 n = dist - vec3(
        SceneSDF(p-e.xyy).signedDistance,
        SceneSDF(p-e.yxy).signedDistance,
        SceneSDF(p-e.yyx).signedDistance
    );

    return normalize(n);
}

vec4 getLight(vec3 p, vec4 color) {
    vec3 lightPos = vec3(p.x, 20., p.z);
    vec3 l = normalize(lightPos - p);
    l = rotateX(l, 0.4);

    vec3 n = getNormal(p);

    vec3 dif = vec3(1, 1, 1);

    vec4 col = vec4(0., 0., 0., 1.);
    bool hitTransparentObject;

    float dist = lightMarch(p + n * TOLERANCE * 2, l, lightPos, col, hitTransparentObject);

    if (hitTransparentObject) {
        dif *= col.rgb * 0.5;
    } else if (dist < length(lightPos - p) && !hitTransparentObject) {
        dif *= 0.5;
    }

    return vec4(color.rgb * dif, 1.);
}

void main() {
    vec2 uv = (2 * gl_FragCoord.xy - windowDimensions.xy) / windowDimensions.y;

    vec3 rd = normalize(vec3(uv.x, uv.y, 1.5));
    rd = rotateXYZ(rd, camRotation);

    float dist = RayMarch(camPosition, rd, difCol);

    vec3 pos = camPosition + rd * dist;

    vec4 col = getLight(pos, difCol);

	gl_FragColor = col * difCol;
}