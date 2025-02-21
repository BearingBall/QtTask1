#pragma once
#ifndef ICOSPHERE_H
#define ICOSPHERE_H

#endif // ICOSPHERE_H


#define _CRT_SECURE_NO_WARNINGS

#define EXPORT_OBJS 0
#define EXPORT_IMAGES 0
#define PRINT_STATS 1
#define PRINT_CREATION_TIMES 0

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <map>
#include <random>
#include <string>
#include <iostream>
#include <fstream>

#if _WIN32
#include <Windows.h>
#else
typedef uint64_t LARGE_INTEGER;
#endif

namespace ico
{


struct Vector3
{
    double x, y, z;
    Vector3(double v) : x(v), y(v), z(v) {}
    Vector3(double ix, double iy, double iz) : x(ix), y(iy), z(iz) {}
    Vector3 operator +(const Vector3 &other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
    Vector3 operator -(const Vector3 &other) const { return Vector3(x - other.x, y - other.y, z - other.z); }
    Vector3 operator *(const Vector3 &other) const { return Vector3(x * other.x, y * other.y, z * other.z); }
};

double dot(const Vector3 &a, const Vector3 &b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vector3 cross(const Vector3 &a, const Vector3 &b)
{
    return Vector3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x);
}

double length(const Vector3 &a)
{
    return std::sqrt(dot(a, a));
}

Vector3 normalize(const Vector3 &a)
{
    const double lrcp = 1.0 / std::sqrt(dot(a, a));
    return Vector3(a.x * lrcp, a.y * lrcp, a.z * lrcp);
}

struct Mesh
{
public:
    std::vector<Vector3> vertices;
    std::vector<uint32_t> triangles;

    uint32_t triangleCount() const { return triangles.size() / 3; }

    void addTriangle(uint32_t a, uint32_t b, uint32_t c)
    {
        triangles.emplace_back(a);
        triangles.emplace_back(b);
        triangles.emplace_back(c);
    }

    void addQuad(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
    {
        triangles.emplace_back(a);
        triangles.emplace_back(b);
        triangles.emplace_back(c);
        triangles.emplace_back(a);
        triangles.emplace_back(c);
        triangles.emplace_back(d);
    }

    void addQuadAlt(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
    {
        triangles.emplace_back(a);
        triangles.emplace_back(b);
        triangles.emplace_back(d);
        triangles.emplace_back(b);
        triangles.emplace_back(c);
        triangles.emplace_back(d);
    }

    void clear()
    {
        vertices.clear();
        triangles.clear();
    }

    double distance(const Vector3 &p, uint32_t tidx) const
    {
        const uint32_t idx0 = triangles[tidx];
        const uint32_t idx1 = triangles[tidx + 1];
        const uint32_t idx2 = triangles[tidx + 2];
        const Vector3 v0 = vertices[idx0];
        const Vector3 v1 = vertices[idx1];
        const Vector3 v2 = vertices[idx2];
        const Vector3 bv = v0;
        const Vector3 e0 = v1 - v0;
        const Vector3 e1 = v2 - v0;
        const Vector3 dv = bv - p;
        const double a = dot(e0, e0);
        const double b = dot(e0, e1);
        const double c = dot(e1, e1);
        const double d = dot(e0, dv);
        const double e = dot(e1, dv);
      //  const double f = dot(dv, dv);

        const double det = a*c - b*b;
        double s = b*e - c*d;
        double t = b*d - a*e;

        if (s + t <= det)
        {
            if (s < 0.0)
            {
                if (t < 0.0)
                {
                    // region 4
                    if (d < 0.0)
                    {
                        t = 0.0;
                        s = -d >= a ? 1.0 : -d / a;
                    }
                    else
                    {
                        s = 0.0;
                        t = e >= 0.0 ? 0.0 : (-e >= c ? 1.0 : -e / c);
                    }
                }
                else
                {
                    // region 3
                    s = 0.0;
                    t = e >= 0.0 ? 0.0 : (-e >= c ? 1.0 : -e / c);
                }
            }
            else if (t < 0.0)
            {
                // region 5
                s = d >= 0.0 ? 0.0 : (-d >= a ? 1.0 : -d / a);
                t = 0.0;
            }
            else
            {
                // region 0
                const double invDet = 1.0 / det;
                s *= invDet;
                t *= invDet;
            }
        }
        else
        {
            if (s < 0.0)
            {
                // region 2
                const double tmp0 = b + d;
                const double tmp1 = c + e;
                if (tmp1 > tmp0)
                {
                    const double numer = tmp1 - tmp0;
                    const double denom = a - 2.0 * b + c;
                    s = numer >= denom ? 1.0 : numer / denom;
                    t = 1.0 - s;
                }
                else
                {
                    s = 0.0;
                    t = (tmp1 <= 0.0 ? 1.0 : (e >= 0.0 ? 0.0 : -e / c));
                }
            }
            else if (t < 0.0)
            {
                // region 6
                const double tmp0 = b + e;
                const double tmp1 = a + d;
                if (tmp1 > tmp0)
                {
                    const double numer = tmp1 - tmp0;
                    const double denom = a - 2.0 * b + c;
                    t = numer >= denom ? 1.0 : numer / denom;
                    s = 1.0 - t;
                }
                else
                {
                    s = (tmp1 <= 0.0 ? 1.0 : (d >= 0.0 ? 0.0 : -d / a));
                    t = 0.0;
                }
            }
            else
            {
                // region 1
                const double numer = c + e - b - d;
                if (numer <= 0)
                {
                    s = 0.0;
                }
                else
                {
                    const double denom = a - 2.0 * b + c;
                    s = numer >= denom ? 1.0 : numer / denom;
                }
                t = 1.0 - s;
            }
        }

        return length(p - (v0 + Vector3(s) * e0 + Vector3(t) * e1));
    }

    double distance(const Vector3 &p) const
    {
        double min = 10e10;
        for (uint32_t i = 0; i < triangles.size(); i += 3)
        {
            min = std::fmin(min, distance(p, i));
        }
        return min;
    }
};

void Icosahedron(Mesh &mesh)
{
    const double t = (1.0 + std::sqrt(5.0)) / 2.0;

    // Vertices
    mesh.vertices.emplace_back(normalize(Vector3(-1.0,  t, 0.0)));
    mesh.vertices.emplace_back(normalize(Vector3( 1.0,  t, 0.0)));
    mesh.vertices.emplace_back(normalize(Vector3(-1.0, -t, 0.0)));
    mesh.vertices.emplace_back(normalize(Vector3( 1.0, -t, 0.0)));
    mesh.vertices.emplace_back(normalize(Vector3(0.0, -1.0,  t)));
    mesh.vertices.emplace_back(normalize(Vector3(0.0,  1.0,  t)));
    mesh.vertices.emplace_back(normalize(Vector3(0.0, -1.0, -t)));
    mesh.vertices.emplace_back(normalize(Vector3(0.0,  1.0, -t)));
    mesh.vertices.emplace_back(normalize(Vector3( t, 0.0, -1.0)));
    mesh.vertices.emplace_back(normalize(Vector3( t, 0.0,  1.0)));
    mesh.vertices.emplace_back(normalize(Vector3(-t, 0.0, -1.0)));
    mesh.vertices.emplace_back(normalize(Vector3(-t, 0.0,  1.0)));

    // Faces
    mesh.addTriangle(0, 11, 5);
    mesh.addTriangle(0, 5, 1);
    mesh.addTriangle(0, 1, 7);
    mesh.addTriangle(0, 7, 10);
    mesh.addTriangle(0, 10, 11);
    mesh.addTriangle(1, 5, 9);
    mesh.addTriangle(5, 11, 4);
    mesh.addTriangle(11, 10, 2);
    mesh.addTriangle(10, 7, 6);
    mesh.addTriangle(7, 1, 8);
    mesh.addTriangle(3, 9, 4);
    mesh.addTriangle(3, 4, 2);
    mesh.addTriangle(3, 2, 6);
    mesh.addTriangle(3, 6, 8);
    mesh.addTriangle(3, 8, 9);
    mesh.addTriangle(4, 9, 5);
    mesh.addTriangle(2, 4, 11);
    mesh.addTriangle(6, 2, 10);
    mesh.addTriangle(8, 6, 7);
    mesh.addTriangle(9, 8, 1);
}

struct Edge
{
    uint32_t v0;
    uint32_t v1;

    Edge(uint32_t v0, uint32_t v1)
        : v0(v0 < v1 ? v0 : v1)
        , v1(v0 < v1 ? v1 : v0)
    {
    }

    bool operator <(const Edge &rhs) const
    {
        return v0 < rhs.v0 || (v0 == rhs.v0 && v1 < rhs.v1);
    }
};

uint32_t subdivideEdge(uint32_t f0, uint32_t f1, const Vector3 &v0, const Vector3 &v1, Mesh &io_mesh, std::map<Edge, uint32_t> &io_divisions)
{
    const Edge edge(f0, f1);
    auto it = io_divisions.find(edge);
    if (it != io_divisions.end())
    {
        return it->second;
    }

    const Vector3 v = normalize(Vector3(0.5) * (v0 + v1));
    const uint32_t f = io_mesh.vertices.size();
    io_mesh.vertices.emplace_back(v);
    io_divisions.emplace(edge, f);
    return f;
}

void SubdivideMesh(const Mesh &meshIn, Mesh &meshOut)
{
    meshOut.vertices = meshIn.vertices;

    std::map<Edge, uint32_t> divisions; // Edge -> new vertex

    for (uint32_t i = 0; i < meshIn.triangleCount(); ++i)
    {
        const uint32_t f0 = meshIn.triangles[i * 3];
        const uint32_t f1 = meshIn.triangles[i * 3 + 1];
        const uint32_t f2 = meshIn.triangles[i * 3 + 2];

        const Vector3 v0 = meshIn.vertices[f0];
        const Vector3 v1 = meshIn.vertices[f1];
        const Vector3 v2 = meshIn.vertices[f2];

        const uint32_t f3 = subdivideEdge(f0, f1, v0, v1, meshOut, divisions);
        const uint32_t f4 = subdivideEdge(f1, f2, v1, v2, meshOut, divisions);
        const uint32_t f5 = subdivideEdge(f2, f0, v2, v0, meshOut, divisions);

        meshOut.addTriangle(f0, f3, f5);
        meshOut.addTriangle(f3, f1, f4);
        meshOut.addTriangle(f4, f2, f5);
        meshOut.addTriangle(f3, f4, f5);
    }
}

}
