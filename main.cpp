/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the documentation of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "openglwindow.h"

#include <QGuiApplication>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QScreen>
#include <QtMath>
#include "icosphere.h"
#include "shaders.h"
#include "objectAdapter.h"
#include <QKeyEvent>
#include <QColor>
#include <QtWidgets>
#include <QColorDialog>


//! [1]
class TriangleWindow : public OpenGLWindow
{
public:
    using OpenGLWindow::OpenGLWindow;

    void initialize() override;
    void render() override;

    TriangleWindow():sliderX(&dialog),sliderY(&dialog),sliderZ(&dialog),zBuf(&dialog),colling(&dialog),zBufLabel(QString("Z Buf"),&dialog),collingLabel(QString("Colling"),&dialog)
    {

    }


    ~TriangleWindow()
    {
    }

    void keyPressEvent(QKeyEvent* key) override;


private:
    Objects objects;
    QColorDialog dialog;
    QSlider sliderX;
    QSlider sliderY;
    QSlider sliderZ;
    QCheckBox zBuf;
    QCheckBox colling;
    QLabel zBufLabel;
    QLabel collingLabel;

    GLint m_posAttr = 0;
    GLint m_colAttr = 0;
    GLint m_matrixUniform = 0;

    QOpenGLShaderProgram *m_program = nullptr;
    int m_frame = 0;
};

void TriangleWindow::keyPressEvent(QKeyEvent* key)
{
    if (key->key() == Qt::Key_Less)
    {
        objects.current = objects.current == 0 ? 0 : objects.current-1;
    }
    if (key->key() == Qt::Key_Greater)
    {
        objects.current = objects.current == objects.primitives.size() - 1 ?  objects.primitives.size()-1 : objects.current + 1;
    }
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QSurfaceFormat format;
    format.setSamples(16);

    TriangleWindow window;
    window.setFormat(format);
    window.resize(640, 480);
    window.show();

    window.setAnimating(true);

    return app.exec();
}

void TriangleWindow::initialize()
{

    dialog.setGeometry(50,50,100,100);
    dialog.show();
    dialog.setOption(QColorDialog::NoButtons);
    sliderX.setOrientation(Qt::Orientation::Horizontal);
    sliderY.setOrientation(Qt::Orientation::Horizontal);
    sliderZ.setOrientation(Qt::Orientation::Horizontal);
    sliderX.setGeometry(10,217,230,16);
    sliderY.setGeometry(10,234,230,16);
    sliderZ.setGeometry(10,251,230,16);

    zBuf.move(110,260);
    zBuf.setCheckState(Qt::CheckState::Checked);
    colling.move(180, 260);
    zBufLabel.setGeometry(130,260,40,30);
    collingLabel.setGeometry(200,260,40,30);


    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    Q_ASSERT(m_posAttr != -1);
    m_colAttr = m_program->attributeLocation("colAttr");
    Q_ASSERT(m_colAttr != -1);
    m_matrixUniform = m_program->uniformLocation("matrix");
    Q_ASSERT(m_matrixUniform != -1);
}

void TriangleWindow::render()
{
    objects.setColor(dialog.currentColor());



    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    m_program->bind();

    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
    matrix.translate(0, 0, -2);
    matrix.rotate(100.0f * m_frame / screen()->refreshRate(), sliderX.value(), sliderY.value(), sliderZ.value());

    m_program->setUniformValue(m_matrixUniform, matrix);



    if (zBuf.checkState() == Qt::CheckState::Checked)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    if (colling.checkState() == Qt::CheckState::Checked)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    }
        else
        glDisable(GL_CULL_FACE);

    /////////////////////////////////////////////////////

    glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, objects.primitives[objects.current]);
    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, objects.edgeColors[objects.current]);

    glDisable(GL_POLYGON_OFFSET_FILL);
    glEnableVertexAttribArray(m_posAttr);
    glEnableVertexAttribArray(m_colAttr);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawArrays(GL_TRIANGLES, 0 , objects.primitiveSize[objects.current]/3);

    glDisableVertexAttribArray(m_colAttr);
    glDisableVertexAttribArray(m_posAttr);
    /////////////////////////////////////////////////////


     glVertexAttribPointer(m_posAttr, 3, GL_FLOAT, GL_FALSE, 0, objects.primitives[objects.current]);
     glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, objects.fillColors[objects.current]);

     glEnable(GL_POLYGON_OFFSET_FILL);
     //glPolygonOffset(1.0f, 1.0f);

     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

     glEnableVertexAttribArray(m_posAttr);
     glEnableVertexAttribArray(m_colAttr);

     glDrawArrays(GL_TRIANGLES, 0, objects.primitiveSize[objects.current]/3);

     glDisableVertexAttribArray(m_colAttr);
     glDisableVertexAttribArray(m_posAttr);


    m_program->release();
    ++m_frame;
}
//! [5]
