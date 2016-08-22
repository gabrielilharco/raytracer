#ifndef RAYTRACER_PLANE_H
#define RAYTRACER_PLANE_H

#include "Object.h"

class Plane : public Object {
public:
    Vector3 getFirstIntersection (const Ray& ray) override;
    Vector3 getNormalAt(const Vector3 v) override;
};


#endif //RAYTRACER_PLANE_H
