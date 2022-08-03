#version 330

uniform sampler2D tex;
uniform sampler2D skybox;
uniform sampler2D buff;

out vec4 FragColor;

// Constants for the Ray Marching Algorithm
const float MAX_DISTANCE = 1000.;
const float TOLERANCE = 0.001;
const int MAX_STEPS = 500;
const float PI = 3.14159265359;
const float GAMMA = 2.5;
const int MAX_BOUNCES = 2;
const int MAX_SAMPLES = 1;
const float SHADOW_STRENGTH = 0.5;
const int AO_STEP_SIZE = 1;

// Shape types
const int SPHERE = 1;
const int BOX = 2;
const int CAPSULE = 3;
const int PLANE = 4;

// SDF Ops
const int UNION = 1;
const int INTERSECTION = 2;
const int SUBTRACT = 3;
const int SMOOTH_UNION = 4;
const int SMOOTH_INTERSECTION = 5;
const int SMOOTH_SUBTRACT = 6;

vec4 difCol = vec4(1., 1., 1., 1.);

mat3 rotateXYZ(vec3 rot) {
    mat3 rotation;
    rotation[0] = vec3(
        cos(rot.z) * cos(rot.y),
        sin(rot.z) * cos(rot.y),
        -sin(rot.y)
    );
    rotation[1] = vec3(
        cos(rot.z) * sin(rot.y) * sin(rot.x) - sin(rot.z) * cos(rot.x),
        sin(rot.z) * sin(rot.y) * sin(rot.x) + cos(rot.z) * cos(rot.x),
        cos(rot.y) * sin(rot.x)
    );
    rotation[2] = vec3(
        cos(rot.z) * sin(rot.y) * cos(rot.x) + sin(rot.z) * sin(rot.x),
        sin(rot.z) * sin(rot.y) * cos(rot.x) - cos(rot.z) * sin(rot.x),
        cos(rot.y) * cos(rot.x)
    );

    return rotation;
}

uniform vec2 windowDimensions = vec2(800, 600);
uniform vec3 camPosition = vec3(0, 1, 0);
uniform vec3 camRotation = vec3(0);

uniform float time = 0;
uniform float deltaTime = 0;

// Used for checking and returning the distance to the scene
// and the color at that point in a nice package

struct Light {
    vec3 position;
    float radius;
    vec4 color;
};

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
    float roughness;
    bool emissive;
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

uniform vec3 lights[2] = { vec3(0, 1000., 0), vec3(-5, 2, 3) };

// Copied from https://www.shadertoy.com/view/Ml3Gz8
float smoothMin(float a, float b, float k) {
    float h = clamp(0.5 + 0.5*(b-a)/k, 0.0, 1.0);
    return mix(b, a, h) - k*h*(1.0-h);
}

float smoothMax(float a, float b, float k) {
    float h = clamp(0.5 - 0.5*(b-a)/k, 0.0, 1.0);
    return mix(b, a, h) + k*h*(1.0-h);
}

vec4 smoothColor(float d1, float d2, vec4 a, vec4 b, float k) {
    float h = clamp(0.5 + 0.5*(d2 - d1)/k, 0.0, 1.0);
    return mix(b, a, h) - k*h*(1.0-h);
}

// Signed Distance Fields
float sphereSDF(vec3 p, float r) {
    return length(p) - r;
}

float boxSDF(vec3 p, vec3 size) {
    vec3 q = abs(p) - size;
    return length(max(q, 0.)) + min(max(q.x, max(q.y, q.z)), 0.);
}

float capsuleSDF(vec3 p, vec3 pos1, vec3 pos2, float r) {
    vec3 pa = p - pos1;
    vec3 ba = pos2 - pos1;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length(pa - ba * h) - r;
}

float planeSDF(vec3 p, vec3 n, float h) {
    n = normalize(n);
    return dot(p, n) + h;
}

// Shape operations
Shape combine(Shape s1, Shape s2) {
    Shape returned = s1.signedDistance < s2.signedDistance ? s1 : s2;
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

Shape smoothCombine(Shape s1, Shape s2) {
    Shape returned = s1.signedDistance < s2.signedDistance ? s1 : s2;
    returned.signedDistance = smoothMin(s1.signedDistance, s2.signedDistance, 0.2);
    returned.color = smoothColor(s1.signedDistance, s2.signedDistance, s1.color, s2.color, 0.2);
    returned.checkShape = true;
    return returned;
}

Shape smoothIntersection(Shape s1, Shape s2) {
    Shape returned = s1.signedDistance > s2.signedDistance ? s1 : s2;
    returned.signedDistance = smoothMax(s1.signedDistance, s2.signedDistance, 0.2);
    returned.color = smoothColor(s1.signedDistance, s2.signedDistance, s1.color, s2.color, 0.2);
    returned.checkShape = true;
    return returned;
}

Shape smoothSubtract(Shape s1, Shape s2) {
    Shape negS1 = s1;
    negS1.signedDistance = -s1.signedDistance;

    Shape returned = negS1.signedDistance > s2.signedDistance ? negS1 : s2;
    returned.signedDistance = smoothMax(negS1.signedDistance, s2.signedDistance, 0.2);
    returned.color = smoothColor(s1.signedDistance, s2.signedDistance, s1.color, s2.color, 0.2);
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
            inverse(rotateXYZ(s.rotation)) * (p - s.position),
            s.param1.x
        );
    }

    // Box
    if (s.type == BOX) {
        return boxSDF(
            inverse(rotateXYZ(s.rotation)) * (p - s.position),
            s.param1
        );
    }

    // Capsule
    if (s.type == CAPSULE) {
        return capsuleSDF(
            p,
            s.position,
            s.param1,
            s.param2.r
        );
    }

    if (s.type == PLANE) {
        return planeSDF(
            inverse(rotateXYZ(s.rotation)) * (p - s.position),
            s.param1,
            s.param2.x
        );
    }

    return sdf;
}

Shape operateSDF(Shape s1, Shape s2) {

    if (s1.operation == UNION) {
        return combine(s1, s2);
    } else if (s1.operation == SUBTRACT) {
        return subtract(s2, s1);
    } else if (s1.operation == INTERSECTION) {
        return intersection(s1, s2);
    } else if (s1.operation == SMOOTH_UNION) {
        return smoothCombine(s1, s2);
    } else if (s1.operation == SMOOTH_SUBTRACT) {
        return smoothSubtract(s2, s1);
    } else if (s1.operation == SMOOTH_INTERSECTION) {
        return smoothIntersection(s1, s2);
    }

    return s1;
}

// Random number generation - byteblacksmith
float rand(vec2 co) {
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt = dot(co.xy, vec2(a, b));
    highp float sn = mod(dt, 3.14);
    return fract(sin(sn) * c);
}

Shape SceneSDF(vec3 p) {

    Shape scene;

    scene.signedDistance = 1000;
    scene.color = vec4(1, 1, 1, 1);
    scene.type = 0;
    scene.metallic = 0;
    scene.roughness = 0;

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

    if (scene.type == BOX) {
        scene.roughness = 0.2;
        scene.metallic = 0.2;
    } else if (scene.type == PLANE) {
        scene.roughness = 0.;
        scene.metallic = 0.5;
    } else if (scene.type != 0) {
        scene.roughness = 0.7;
        scene.metallic = 0.9;
    }

    scene.emissive = false;

	return scene;
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

// Used for traversing through the scene until an object is hit
float RayMarch(vec3 ro, vec3 rd, out vec4 dCol) {
    float distTotal = 0;
    vec4 accCol = vec4(0, 0, 0, 1);

    for (int i = 0; i < MAX_STEPS; i++) {
        vec3 p = ro + rd * distTotal;
        Shape scene = SceneSDF(p);
        float dist = scene.signedDistance;

        if (dist < TOLERANCE) {
            accCol.rgb += scene.color.rgb * (accCol.a * scene.color.a);
            accCol.a *= (1 - scene.color.a);
                
            if (scene.color.a < 1 - TOLERANCE)
                distTotal += TOLERANCE * 10;

            if (accCol.a < 0.05) {
                dCol = accCol;
                return distTotal;
            }
        }

        distTotal += abs(dist);

        if (distTotal > MAX_DISTANCE) {
            vec2 uv;
            uv.x = 0.5 + atan(rd.x, rd.z)/(2 * PI);
            uv.y = 0.5 - asin(rd.y) / PI;
            dCol = texture2D(skybox, uv);
            vec4 mapped = dCol / (dCol + vec4(1.));
            mapped = pow(mapped, vec4(1 / GAMMA));
            accCol.rgb += mapped.rgb * (accCol.a * mapped.a);
            accCol.a *= (1 - mapped.a);
            dCol = accCol;
            return distTotal;
        }
    }

    dCol = accCol;
    dCol.a = -1;
    return distTotal;
}

// Used for proper lighting & shading
vec3 lightMarch(vec3 ro, vec3 rd, int lightID, float k) {
    float distTotal = 0;
    float res = 1.;
    vec4 accCol = vec4(0, 0, 0, 1);
    vec4 baseCol = SceneSDF(ro).color;
    float m = SceneSDF(ro).metallic;
    float alpha;

    for (int i = 0; i < MAX_STEPS; i++) {
        Shape scene = SceneSDF(ro + rd * distTotal);
        float dist = scene.signedDistance;

        if (dist < TOLERANCE) {
            return vec3(SHADOW_STRENGTH);
            if (scene.color.a > 1 - TOLERANCE) {
                
            }

            // Colored shadows (WIP)
            accCol.rgb += scene.color.rgb * (accCol.a * scene.color.a);
            accCol.a *= (1 - scene.color.a);
                
            if (scene.color.a <= 1 - TOLERANCE)
                distTotal += TOLERANCE * 10;

            if (accCol.a < TOLERANCE) {
                return baseCol.rgb * (1. - scene.color.a) + scene.color.a * accCol.rgb;
            }

            alpha = scene.color.a;
        }

        dist = max(abs(dist), TOLERANCE);
        res = min(res, k * dist / distTotal + SHADOW_STRENGTH);
        distTotal += abs(dist);

        if (distTotal > length(lights[lightID] - ro)) {
            return vec3(res);
        }

        //if (distTotal > MAX_DISTANCE) break;
    }

    return vec3(res);
}

vec4 getLight(vec3 p, int lightID, vec4 color) {
    // Allows the skybox to be unaffected by lighting
    if (length(p - camPosition) > MAX_DISTANCE - TOLERANCE) {
        return color;
    }
    
    vec3 lightPos = vec3(p.x, 100, p.z);
    lightPos = rotateXYZ(vec3(0, 0, PI / 12)) * lightPos;
    lightPos = rotateXYZ(vec3(0, mod(time, 2 * PI), 0)) * lightPos;
    vec3 l = normalize(lightPos - p);
    vec3 n = getNormal(p);
    float dif = clamp(dot(n, l), SHADOW_STRENGTH, 1.);
    
    // Diffuse lighting and shadows
    float m = SceneSDF(p).metallic;
    vec3 lightCol = lightMarch(p + n * TOLERANCE * 2, l, lightID, 28);
    color.rgb = lightCol * dif;
    color.a = 1; // Just in case

    return color;
}

float aoMarch(vec3 p) {
    float sum = 0;
    float maxSum = 0;
    vec3 n = getNormal(p);
    for (int i = 0; i < MAX_STEPS / 50; i++) {
        vec3 pos = p + n * (i+1) * AO_STEP_SIZE;
        sum    += 1. / pow(2., i) * abs(SceneSDF(pos).signedDistance);
        maxSum += 1. / pow(2., i) * (i+1) * AO_STEP_SIZE;
    }

    return sum / maxSum;
}

void main() {
    vec2 uv = (2 * gl_FragCoord.xy - windowDimensions.xy) / windowDimensions.y;

    vec3 rd = normalize(vec3(uv.x, uv.y, 1.5));
    rd = rotateXYZ(camRotation) * rd;

    float dist = RayMarch(camPosition, rd, difCol);

    vec3 pos = camPosition + rd * dist;

    if (dist > MAX_DISTANCE - TOLERANCE || difCol.a < 0) {
        difCol.a = 1.;
        FragColor = difCol;
        return;
    }

    float ao = aoMarch(pos);
    vec4 shade = getLight(pos, 0, difCol);

    Shape scene = SceneSDF(pos);

    // Indirect illumination (Need to implement progressive rendering to get a better look)
    // Doesn't put reflections on transparent objects yet
    vec4 accCol = vec4(0);
    vec4 indCol = vec4(0);
    vec3 sn = getNormal(pos);
        vec3 refd = reflect(rd, sn);
    for (int i = 0; i < MAX_SAMPLES && scene.metallic > TOLERANCE; i++) {
         // Don't need multiple samples if roughness is 0
        if (scene.roughness < TOLERANCE) {
            dist = RayMarch(pos + sn * TOLERANCE * 2, refd, indCol);
            accCol += indCol * getLight(pos + refd * dist, 0, indCol);
            accCol *= MAX_SAMPLES;
            break;
        }

        refd = sn + (refd - sn) * scene.roughness;
        vec3 rot = vec3(
            rand(vec2(time / pos.x, deltaTime)), 
            rand(vec2(pos.x / pos.y * deltaTime, time / deltaTime)), 
            rand(vec2(deltaTime * pos.z, length(pos)))
        ) - 0.5;

        // Scale between -PI / 2 and PI / 2 then by roughness
        rot *= 2;
        rot *= PI / 2;
        rot *= scene.roughness;
        vec3 randd = normalize(rotateXYZ(rot) * refd);
        dist = RayMarch(pos + sn * TOLERANCE * 2, randd, indCol);

        if (indCol.a < 0) {
            indCol = scene.color;
            indCol.a = 1;
        }

        vec3 refpos = pos + randd * dist;
        accCol += indCol * getLight(refpos, 0, indCol);
    }

    accCol /= MAX_SAMPLES;
    difCol = difCol * (1 - scene.metallic) + accCol * scene.metallic;
    difCol *= shade * ao;

    vec4 bufCol = texture(buff, gl_FragCoord.xy / windowDimensions);
    difCol.rgb = mix(difCol.rgb, bufCol.rgb, 0.5);

    difCol.a = 1;
	FragColor = difCol;
}