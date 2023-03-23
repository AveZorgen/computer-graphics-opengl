#version 330

in vec3 glPosition;

struct SCamera {
    vec3 Position;
    vec3 View;
    vec3 Up;
    vec3 Side;
    // отношение сторон выходного изображения
    vec2 Scale;
};
struct SRay {
    vec3 Origin;
    vec3 Direction;
};

#define EPSILON 0.001
#define BIG 1000000.0
const int DIFFUSE = 1;
const int REFLECTION = 2;
const int REFRACTION = 3;

struct SSphere {
    vec3 Center;
    float Radius;
    int MaterialIdx;
};
struct STriangle {
    vec3 v1;
    vec3 v2;
    vec3 v3;
    int MaterialIdx;
};

struct SIntersection {
    float Time;
    vec3 Point;
    vec3 Normal;
    vec3 Color;
 // ambient, diffuse and specular coeffs
    vec4 LightCoeffs;
 // 0 - non-reflection, 1 - mirror
    float ReflectionCoef;
    float RefractionCoef;
    int MaterialType;
};

struct SLight {
    vec3 Position;
};

struct SMaterial {
 //diffuse color
    vec3 Color;
 // ambient, diffuse and specular coeffs
    vec4 LightCoeffs;
 // 0 - non-reflection, 1 - mirror
    float ReflectionCoef;
    float RefractionCoef;
    int MaterialType;
};

SCamera uCamera;
STriangle triangles[10];
SSphere spheres[2];
SLight light;
SMaterial materials[6];

SRay GenerateRay(SCamera uCamera) {
    vec2 coords = glPosition.xy * uCamera.Scale;
    vec3 direction = uCamera.View + uCamera.Side * coords.x + uCamera.Up * coords.y;
    return SRay(uCamera.Position, normalize(direction));
}

void initializeDefaultCamera() {
    uCamera.Position = vec3(0.0, 0.0, -8.0);
    uCamera.View = vec3(0.0, 0.0, 1.0);
    uCamera.Up = vec3(0.0, 1.0, 0.0);
    uCamera.Side = vec3(1.0, 0.0, 0.0);
    uCamera.Scale = vec2(1.0);
}

void initializeDefaultScene(out STriangle triangles[10], out SSphere spheres[2]) {
    /** TRIANGLES **/
    /* left wall */
    triangles[0].v1 = vec3(-5.0, -5.0, -5.0);
    triangles[0].v2 = vec3(-5.0, 5.0, 5.0);
    triangles[0].v3 = vec3(-5.0, 5.0, -5.0);
    triangles[0].MaterialIdx = 0;
    triangles[1].v1 = vec3(-5.0, -5.0, -5.0);
    triangles[1].v2 = vec3(-5.0, -5.0, 5.0);
    triangles[1].v3 = vec3(-5.0, 5.0, 5.0);
    triangles[1].MaterialIdx = 0;
    /* back wall */
    triangles[2].v1 = vec3(-5.0, -5.0, 5.0);
    triangles[2].v2 = vec3(5.0, -5.0, 5.0);
    triangles[2].v3 = vec3(-5.0, 5.0, 5.0);
    triangles[2].MaterialIdx = 0;
    triangles[3].v1 = vec3(5.0, 5.0, 5.0);
    triangles[3].v2 = vec3(-5.0, 5.0, 5.0);
    triangles[3].v3 = vec3(5.0, -5.0, 5.0);
    triangles[3].MaterialIdx = 0;
    /* Самостоятельно добавьте треугольники так, чтобы получился куб */
    /** SPHERES **/
    spheres[0].Center = vec3(-1.0, -1.0, -2.0);
    spheres[0].Radius = 2.0;
    spheres[0].MaterialIdx = 0;
    spheres[1].Center = vec3(2.0, 1.0, 2.0);
    spheres[1].Radius = 1.0;
    spheres[1].MaterialIdx = 0;
}

void initializeDefaultLightMaterials(out SLight light, out SMaterial materials[6]) {
 //** LIGHT **//
    light.Position = vec3(0.0, 2.0, -4.0f);
 /** MATERIALS **/
    vec4 lightCoefs = vec4(0.4, 0.9, 0.0, 512.0);
    materials[0].Color = vec3(0.0, 1.0, 0.0);
    materials[0].LightCoeffs = vec4(lightCoefs);
    materials[0].ReflectionCoef = 0.5;
    materials[0].RefractionCoef = 1.0;
    materials[0].MaterialType = DIFFUSE;
    materials[1].Color = vec3(0.0, 0.0, 1.0);
    materials[1].LightCoeffs = vec4(lightCoefs);
    materials[1].ReflectionCoef = 0.5;
    materials[1].RefractionCoef = 1.0;
    materials[1].MaterialType = DIFFUSE;
}

bool IntersectSphere(SSphere sphere, SRay ray, float start, float final, out float time) {
    ray.Origin -= sphere.Center;
    float A = dot(ray.Direction, ray.Direction);
    float B = dot(ray.Direction, ray.Origin);
    float C = dot(ray.Origin, ray.Origin) - sphere.Radius * sphere.Radius;
    float D = B * B - A * C;
    if(D > 0.0) {
        D = sqrt(D);
        //time = min ( max ( 0.0, ( -B - D ) / A ), ( -B + D ) / A );
        float t1 = (-B - D) / A;
        float t2 = (-B + D) / A;
        if(t1 < 0 && t2 < 0)
            return false;

        if(min(t1, t2) < 0) {
            time = max(t1, t2);
            return true;
        }
        time = min(t1, t2);
        return true;
    }
    return false;
}

bool IntersectTriangle(SRay ray, vec3 v1, vec3 v2, vec3 v3, out float time) {
// // Compute the intersection of ray with a triangle using geometric solution
// Input: // points v0, v1, v2 are the triangle's vertices
// rayOrig and rayDir are the ray's origin (point) and the ray's direction
// Return: // return true is the ray intersects the triangle, false otherwise
// bool intersectTriangle(point v0, point v1, point v2, point rayOrig, vector rayDir) {
// compute plane's normal vector
    time = -1;
    vec3 A = v2 - v1;
    vec3 B = v3 - v1;
// no need to normalize vector
    vec3 N = cross(A, B);
// N
// // Step 1: finding P
// // check if ray and plane are parallel ?
    float NdotRayDirection = dot(N, ray.Direction);
    if(abs(NdotRayDirection) < EPSILON)
        return false;
// they are parallel so they don't intersect !
// compute d parameter using equation 2
    float d = dot(N, v1);
// compute t (equation 3)
    float t = -(dot(N, ray.Origin) - d) / NdotRayDirection;
// check if the triangle is in behind the ray
    if(t < 0)
        return false;
// the triangle is behind
// compute the intersection point using equation 1
    vec3 P = ray.Origin + t * ray.Direction;
// // Step 2: inside-outside test //
    vec3 C;
// vector perpendicular to triangle's plane
// edge 0
    vec3 edge1 = v2 - v1;
    vec3 VP1 = P - v1;
    C = cross(edge1, VP1);
    if(dot(N, C) < 0)
        return false;
// P is on the right side
// edge 1
    vec3 edge2 = v3 - v2;
    vec3 VP2 = P - v2;
    C = cross(edge2, VP2);
    if(dot(N, C) < 0)
        return false;
// P is on the right side
// edge 2
    vec3 edge3 = v1 - v3;
    vec3 VP3 = P - v3;
    C = cross(edge3, VP3);
    if(dot(N, C) < 0)
        return false;
// P is on the right side;
    time = t;
    return true;
// this ray hits the triangle
}

bool Raytrace(
    SRay ray,
    SSphere spheres[2],
    STriangle triangles[10],
    SMaterial materials[6],
    float start,
    float final,
    inout SIntersection intersect
) {
    bool result = false;
    float test = start;
    intersect.Time = final;
//calculate intersect with spheres
    for(int i = 0; i < 2; i++) {
        SSphere sphere = spheres[i];
        if(IntersectSphere(sphere, ray, start, final, test) && test < intersect.Time) {
            intersect.Time = test;
            intersect.Point = ray.Origin + ray.Direction * test;
            intersect.Normal = normalize(intersect.Point - spheres[i].Center);
            intersect.Color = materials[0].Color;
            intersect.LightCoeffs = materials[0].LightCoeffs;
            intersect.ReflectionCoef = materials[0].ReflectionCoef;
            intersect.RefractionCoef = materials[0].RefractionCoef;
            intersect.MaterialType = materials[0].MaterialType;
            result = true;
        }
    }
//calculate intersect with triangles
    for(int i = 0; i < 10; i++) {
        STriangle triangle = triangles[i];
        if(IntersectTriangle(ray, triangle.v1, triangle.v2, triangle.v3, test) && test < intersect.Time) {
            intersect.Time = test;
            intersect.Point = ray.Origin + ray.Direction * test;
            intersect.Normal = normalize(cross(triangle.v1 - triangle.v2, triangle.v3 - triangle.v2));
            intersect.Color = materials[1].Color;
            intersect.LightCoeffs = materials[1].LightCoeffs;
            intersect.ReflectionCoef = materials[1].ReflectionCoef;
            intersect.RefractionCoef = materials[1].RefractionCoef;
            intersect.MaterialType = materials[1].MaterialType;
            result = true;
        }
    }
    return result;
}

float Shadow(SLight currLight, SIntersection intersect) {
 // Point is lighted
    float shadowing = 1.0;
 // Vector to the light source
    vec3 direction = normalize(currLight.Position - intersect.Point);
 // Distance to the light source
    float distanceLight = distance(currLight.Position, intersect.Point);
 // Generation shadow ray for this light source
    SRay shadowRay = SRay(intersect.Point + direction * EPSILON, direction);
 // ...test intersection this ray with each scene object
    SIntersection shadowIntersect;
    shadowIntersect.Time = BIG;
 // trace ray from shadow ray begining to light source position
    if(Raytrace(shadowRay, spheres, triangles, materials, 0, distanceLight, shadowIntersect)) {
 // this light source is invisible in the intercection point
        shadowing = 0.0;
    }
    return shadowing;
}

vec3 Phong(SIntersection intersect, SLight currLight) {
    vec3 light = normalize(currLight.Position - intersect.Point);
    float diffuse = max(dot(light, intersect.Normal), 0.0);
    vec3 view = normalize(uCamera.Position - intersect.Point);
    vec3 reflected = reflect(-view, intersect.Normal);
    float specular = pow(max(dot(reflected, light), 0.0), intersect.LightCoeffs.w);
    return intersect.LightCoeffs.x * intersect.Color +
        intersect.LightCoeffs.y * diffuse * intersect.Color * Shadow(currLight, intersect) +
        intersect.LightCoeffs.z * specular * 42.0;
}

out vec4 FragColor;

void main(void) {
    float start = 0;
    float final = BIG;
    initializeDefaultCamera();
    SRay ray = GenerateRay(uCamera);
    SIntersection intersect;
    intersect.Time = BIG;
    vec3 resultColor = vec3(0, 0, 0);
    initializeDefaultScene(triangles, spheres);
    initializeDefaultLightMaterials(light, materials);
    if(Raytrace(ray, spheres, triangles, materials, start, final, intersect)) {
        resultColor = Phong(intersect, light);
    }
    FragColor = vec4(resultColor, 1.0);
}
