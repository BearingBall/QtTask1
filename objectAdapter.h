#ifndef OBJECTADAPTER_H
#define OBJECTADAPTER_H

#endif // OBJECTADAPTER_H

#include <vector>



#include <QGuiApplication>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QScreen>
#include <QtMath>
#include <QColor>
#include "cube.h"
#include "icosphere.h"


class Objects final
{
public:
    std::vector<size_t> primitiveSize;
    std::vector<GLfloat*> primitives;
    std::vector<GLfloat*> fillColors;
    std::vector<GLfloat*> edgeColors;

    size_t current = 0;

    Objects()
    {
        float cubeSize = 1;
        Cube cube;
        primitiveSize.push_back(cube.vertex.size()*3);
        primitives.push_back( new GLfloat[ primitiveSize[0]]);
        fillColors.push_back( new GLfloat[ primitiveSize[0]]);
        edgeColors.push_back( new GLfloat[ primitiveSize[0]]);

        for(size_t i = 0; i< primitiveSize[0]/3; ++i)
        {
            primitives[0][i*3] = cube.vertex[i].coordinates[0] * cubeSize  - cubeSize/2;
            primitives[0][i*3+1] = cube.vertex[i].coordinates[1] * cubeSize - cubeSize/2;
            primitives[0][i*3+2] = cube.vertex[i].coordinates[2] * cubeSize - cubeSize/2;
        }

        for(size_t i = 0; i < primitiveSize[0]/3; ++i)
        {
            fillColors.back()[i*3] =     0.1f;
            fillColors.back()[i*3+1] =   0.5f + 0.5f / static_cast<double>(primitiveSize[0]) * i;
            fillColors.back()[i*3+2] =   0.1f;
        }

        for(size_t i = 0; i < primitiveSize[0]/3; ++i)
        {
            edgeColors.back()[i*3] =     0.1f;
            edgeColors.back()[i*3+1] =   0.3f;
            edgeColors.back()[i*3+2] =   0.1f;
        }


        ico::Mesh m;
        ico::Icosahedron(m);
        for (size_t j = 0; j<3; ++j)
        {
            primitiveSize.push_back(m.triangles.size()*3);
            primitives.push_back( new GLfloat[ primitiveSize[j+1]]);
            fillColors.push_back( new GLfloat[ primitiveSize[j+1]]);
             edgeColors.push_back( new GLfloat[ primitiveSize[j+1]]);
            for(size_t i = 0; i< primitiveSize[j+1]/3; ++i)
            {
                primitives[j+1][i*3] = m.vertices[m.triangles[i]].x;
                primitives[j+1][i*3+1] = m.vertices[m.triangles[i]].y;
                primitives[j+1][i*3+2] = m.vertices[m.triangles[i]].z;
            }

            for(size_t i = 0; i < primitiveSize[j+1]/3; ++i)
            {
                fillColors[j+1][i*3] =     0.1f;
                fillColors[j+1][i*3+1] =   0.5f + 0.5f / static_cast<double>(m.triangles.size()) * i;
                fillColors[j+1][i*3+2] =   0.1f;
            }

            for(size_t i = 0; i < primitiveSize[j+1]/3; ++i)
            {
                edgeColors[j+1][i*3] =     0.1f;
                edgeColors[j+1][i*3+1] =   0.3f;
                edgeColors[j+1][i*3+2] =   0.1f;
            }

            ico::Mesh m2 = m;
            if (j+1 != 3)
                ico::SubdivideMesh(m2, m);
        }
    }

    void setColor(const QColor& color)
    {
        for(size_t i = 0; i < primitiveSize[current]/3;++i)
        {
            qreal r,g,b;
            color.getRgbF( &r,&g,&b);
            fillColors[current][i*3] = r;
            fillColors[current][i*3+1] = g;
            fillColors[current][i*3+2] = b;
        }
    }


    ~Objects()
    {
        for(size_t i=0; i< primitives.size(); ++i)
        {
            delete[] primitives[i];
            delete[] fillColors[i];
            delete[] edgeColors[i];
        }
    }
};
