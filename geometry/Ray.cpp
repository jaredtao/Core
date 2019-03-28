#include "Ray.h"
#include "Mesh.h"
#include "AttributeArray.h"
#include "IndexBuffer.h"
#include "Vector4.h"
#include "Vector3.h"

namespace Core {

    Bool Ray::intersectMesh(WeakPointer<Mesh> mesh, std::vector<Hit>& hits) const {
        WeakPointer<AttributeArray<Point3rs>> vertexArray = mesh->getVertexPositions();
        Point3rs * vertices = vertexArray->getAttributes();

        UInt32 tCount = vertexArray->getAttributeCount();
        WeakPointer<IndexBuffer> indices;
        if (mesh->isIndexed()) {
            indices = mesh->getIndexBuffer();
            tCount = indices->getSize();
        }

        Hit hit;
        for (UInt32 i = 0; i < tCount; i+=3) {
            Bool wasHit = false;
            Point3r a, b, c;
            if (mesh->isIndexed()) {
                a = *(vertices + indices->getIndex(i));
                b = *(vertices + indices->getIndex(i + 1));
                c = *(vertices + indices->getIndex(i + 2));
            }
            else {
                a = *(vertices + i);
                b = *(vertices + (i + 1));
                c = *(vertices + (i + 2));
            }
            wasHit = this->intersectTriangle(a, b, c, hit);
            if (wasHit) {
                hit.Object = mesh;
                hits.push_back(hit);
            }
        }

        return hits.size() > 0;
    }

    Bool Ray::intersectBox(const Box3& box, Hit& hit) const {

        const Vector3r& max = box.getMax();
        const Vector3r& min = box.getMin();

        for (UInt32 i = 0; i < 3; i++) {
            Real origin = i == 0 ? this->Origin.x : i == 1 ? this->Origin.y : this->Origin.z;
            Real originA = i == 0 ? this->Origin.y : i == 1 ? this->Origin.z : this->Origin.x;
            Real originB = i == 0 ? this->Origin.z : i == 1 ? this->Origin.x : this->Origin.y;
            Real dir = i == 0 ? this->Direction.x : i == 1 ? this->Direction.y : this->Direction.z;
            Real dirA = i == 0 ? this->Direction.y : i == 1 ? this->Direction.z : this->Direction.x;
            Real dirB = i == 0 ? this->Direction.z : i == 1 ? this->Direction.x : this->Direction.y;
            Vector3r hitNormal = i == 0 ? Vector3r(1.0, 0.0, 0.0) : i == 1 ? Vector3r(0.0, 1.0, 0.0) : Vector3r(0.0, 0.0, 1.0);

            Real extreme = 0.0f, aAtExtreme = 0.0f, bAtExtreme = 0.0f;
            Bool potentialIntersect = false;
            if (dir > 0) {
                extreme = i == 0 ? min.x : i == 1 ? min.y : min.z;
                Real toMin = extreme - origin;
                if (toMin > 0) {
                    aAtExtreme = dirA / dir * toMin + originA;
                    bAtExtreme = dirB / dir * toMin + originB;
                    potentialIntersect = true;
                    hitNormal = hitNormal * -1.0f;
                }
            }
            else if(dir < 0) {
                extreme = i == 0 ? max.x : i == 1 ? max.y : max.z;
                Real toMax = extreme - origin;
                if (toMax < 0) {
                    aAtExtreme = dirA / dir * toMax + originA;
                    bAtExtreme = dirB / dir * toMax + originB;
                    potentialIntersect = true;
                }
            }

            if (potentialIntersect) {
                Real x = i == 0 ? extreme : i == 1 ? bAtExtreme : aAtExtreme;
                Real y = i == 0 ? aAtExtreme : i == 1 ? extreme : bAtExtreme;
                Real z = i == 0 ? bAtExtreme : i == 1 ? aAtExtreme : extreme;
                Real epsilon = 0.0001f;
                if (x >= min.x - epsilon && x <= max.x + epsilon &&
                    y >= min.y - epsilon && y <= max.y + epsilon &&
                    z >= min.z - epsilon && z <= max.z + epsilon) {
                       hit.Origin.set(x, y, z); 
                       hit.Normal = hitNormal;
                       return true ;
                    }
            }
        }

        return false;
    }

    Bool Ray::intersectTriangle(const Point3r* p0, const Point3r* p1,
                                const Point3r* p2, Hit& hit, const Vector3r* normal) const {
        this->intersectTriangle(*p0, *p1, *p2, hit, normal); 
    }

    Bool Ray::intersectTriangle(const Point3r& p0, const Point3r& p1,
                                const Point3r& p2, Hit& hit, const Vector3r* normal) const {
        Vector3r q1 = p2 - p0;
        Vector3r q2 = p1 - p0;
        Vector3r _normal;

        if (normal != nullptr) {
            _normal.set(normal->x, normal->y, normal->z);
        }
        else {
            Vector3r::cross(q1, q2, _normal);
            _normal.normalize();
        }

        if (Vector3r::dot(_normal, this->Direction) >= 0) return false;

        Real d = -Vector3r::dot(p0, _normal);
        Vector4r planeEq(_normal.x, _normal.y, _normal.z, d);
        
        Hit planeHit;
        Bool intersectsPlane = this->intersectPlane(planeEq, planeHit);
        if (!intersectsPlane) return false;

        Vector3r r = planeHit.Origin - p0;
        Real rDotQ1 = Vector3r::dot(r, q1);
        Real rDotQ2 = Vector3r::dot(r, q2);

        Real q1Sq = Vector3r::dot(q1, q1);
        Real q2Sq = Vector3r::dot(q2, q2);
        Real q1Dotq2 = Vector3r::dot(q1, q2);

        Real qF = -q1Dotq2 / q2Sq;
        Real w1 = (qF * rDotQ2 + rDotQ1) / (qF * q1Dotq2 + q1Sq);
        Real w2 = (rDotQ2 - (w1 * q1Dotq2)) / q2Sq;
        
        Real w0 = 1.0 - w2 - w1;

        if (w0 < 0 || w1 < 0 || w2 < 0 || w0 > 1.0 || w1 > 1.0 || w2 > 1.0) {
            return false;
        }

        hit.Origin = planeHit.Origin;
        hit.Normal = _normal;

        return true;
    }

    Bool Ray::intersectPlane(Vector4Components<Real>& plane, Hit& hit) const {
        Vector4r rayOrigin(this->Origin.x, this->Origin.y, this->Origin.z, 1.0f);
        Vector4r rayDir(this->Direction.x, this->Direction.y, this->Direction.z, 0.0f);
        Real planeRayDot = Vector4r::dot(plane, rayDir);
        if (planeRayDot == 0.0f) return false;
        Real t = -(Vector4r::dot(plane, rayOrigin) / planeRayDot);
        Point3r intersection = this->Origin + this->Direction * t;

        hit.Origin = intersection;
        hit.Normal.set(plane.x, plane.y, plane.z);
        return true;
    }

}