#include "openglwidget.h"
#include "ui_openglwidget.h"

#include <QDebug>
#include <QKeyEvent>
#include <QtMath>

static const float kWIDTH = 1.0f;
static const QString kWoodContainer = QString(":/textures/woodcontainer.png");

Vertex::Vertex(QVector3D position, QVector2D texturePosition, QVector3D normal) :
  position_(position),
  texturePosition_(texturePosition),
  normal_(normal)
{

}

OpenglWidget::OpenglWidget(QWidget *parent) :
  QOpenGLWidget(parent),
  ui_(new Ui::OpenglWidget)
{
  ui_->setupUi(this);
  camera_.setCameraPosition( QVector3D{0.0f, 0.0f, 3.0f} );
  camera_.setCameraFront( QVector3D{0.0f, 0.0f, -1.0f} );
  QSurfaceFormat glFormat;
  glFormat.setVersion(3, 3);
  glFormat.setProfile(QSurfaceFormat::CoreProfile);
  QSurfaceFormat::setDefaultFormat(glFormat);
}

OpenglWidget::~OpenglWidget()
{
  makeCurrent();
  delete ui_;
  delete texture_;
}

void OpenglWidget::setFow(float fow)
{
  if ( 0 == fow ) { return; }
  fow_ = fow;
  updateParametrs();
}

void OpenglWidget::setNearPlane(float nearPlane)
{
  if ( 0 == nearPlane ) { return; }
  nearPlane_ = nearPlane;
  updateParametrs();
}

void OpenglWidget::setFarPlane(float farPlane)
{
  if ( 0 == farPlane ) { return; }
  farPlane_ = farPlane;
  updateParametrs();
}

void OpenglWidget::goForward()
{
  camera_.goForward();
  updateParametrs();
}

void OpenglWidget::goBack()
{
  camera_.goBack();
  updateParametrs();
}

void OpenglWidget::goLeft()
{
  camera_.goLeft();
  updateParametrs();
}

void OpenglWidget::goRight()
{
  camera_.goRight();
  updateParametrs();
}

void OpenglWidget::rotateCamera(QPoint diff)
{
  camera_.rotateCamera(diff);
  updateParametrs();
}

void OpenglWidget::switchLamp()
{
  lamp_ = !lamp_;
  updateParametrs();
}

void OpenglWidget::initializeGL()
{
  qDebug() << "initGL";
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_CULL_FACE);

  loadTexture(kWoodContainer);
  initCube(kWIDTH);
  initShaders();
}

void OpenglWidget::resizeGL(int w, int h)
{
  qDebug() << "resizeGL";
  if ( h == 0 ) { h = 1; }
  float aspect = w / float(h);
  projection_.setToIdentity();
  projection_.perspective(fow_, aspect, nearPlane_, farPlane_);
}

void OpenglWidget::paintGL()
{
  qDebug() << "paint";
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  QVector3D lightPos{-3.0, 3.0, 0.0};
  QVector3D lightColor{1.0, 1.0, 1.0};

  paintLight(lightPos, lightColor, 0.25f);

  QVector3D containerPos1{0.0, 0.0, 0.0};
  paintWoodContainer(containerPos1, 1.0f, lightPos, lightColor);
  QVector3D containerPos2{2.0, 0.0, -2.0};
  paintWoodContainer(containerPos2, 1.0f, lightPos, lightColor);

}

void OpenglWidget::initShaders()
{
  initObjectShader();
  initLightShader();
}

void OpenglWidget::initObjectShader()
{
  if ( objectShader_.isLinked() ) { return;}
  qDebug() << "init object shader";
  if (!objectShader_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vObjectShader.vert")) {
    qDebug() << "Error vertex shader";
    close();
  }
  if (!objectShader_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fObjectShader.frag")) {
    qDebug() << "Error fragment shader";
    close();
  }
  if (!objectShader_.link()) {
    qDebug() << "Error link shader program";
    close();
  }
  objectShader_.bind();

  cubeVBO_.bind();

  int offset = 0;

  auto vertLoc = objectShader_.attributeLocation("inPos");
  objectShader_.enableAttributeArray(vertLoc);
  objectShader_.setAttributeBuffer(vertLoc, GL_FLOAT, offset, 3, sizeof(Vertex));

  offset += sizeof (QVector3D);

  auto texLoc = objectShader_.attributeLocation("inTexCoord");
  objectShader_.enableAttributeArray(texLoc);
  objectShader_.setAttributeBuffer(texLoc, GL_FLOAT, offset, 2, sizeof(Vertex));

  offset += sizeof (QVector2D);

  auto normalLoc = objectShader_.attributeLocation("inNormal");
  objectShader_.enableAttributeArray(normalLoc);
  objectShader_.setAttributeBuffer(normalLoc, GL_FLOAT, offset, 2, sizeof(Vertex));
}

void OpenglWidget::initLightShader()
{
  if ( lightShader_.isLinked() ) { return;}
  qDebug() << "init light shader";
  if (!lightShader_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vLightShader.vert")) {
    qDebug() << "Error vertex shader";
    close();
  }
  if (!lightShader_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fLightShader.frag")) {
    qDebug() << "Error fragment shader";
    close();
  }
  if (!lightShader_.link()) {
    qDebug() << "Error link shader program";
    close();
  }
  lightShader_.bind();

  cubeVBO_.bind();

  int offset = 0;

  auto vertLoc = lightShader_.attributeLocation("inPos");
  lightShader_.enableAttributeArray(vertLoc);
  lightShader_.setAttributeBuffer(vertLoc, GL_FLOAT, offset, 3, sizeof(Vertex));

}

void OpenglWidget::initCube(float width)
{
  qDebug() << "init Cube";
  float halfWidth = width/2;
  //qDebug() << halfWidth << "half";
  QVector<Vertex> vertexes;
  vertexes.append(Vertex(QVector3D( -halfWidth,  halfWidth,  halfWidth), QVector2D(0.0f, 1.0f), QVector3D(0.0f,0.0f,1.0f)));
  vertexes.append(Vertex(QVector3D( -halfWidth, -halfWidth,  halfWidth), QVector2D(0.0f, 0.0f), QVector3D(0.0f,0.0f,1.0f)));
  vertexes.append(Vertex(QVector3D(  halfWidth,  halfWidth,  halfWidth), QVector2D(1.0f, 1.0f), QVector3D(0.0f,0.0f,1.0f)));

  vertexes.append(Vertex(QVector3D(  halfWidth,  halfWidth,  halfWidth), QVector2D(1.0f, 1.0f), QVector3D(0.0f,0.0f,1.0f)));
  vertexes.append(Vertex(QVector3D( -halfWidth, -halfWidth,  halfWidth), QVector2D(0.0f, 0.0f), QVector3D(0.0f,0.0f,1.0f)));
  vertexes.append(Vertex(QVector3D(  halfWidth, -halfWidth,  halfWidth), QVector2D(1.0f, 0.0f), QVector3D(0.0f,0.0f,1.0f)));

  vertexes.append(Vertex(QVector3D(  halfWidth,  halfWidth,  halfWidth), QVector2D(0.0f, 1.0f), QVector3D(1.0f,0.0f,0.0f)));
  vertexes.append(Vertex(QVector3D(  halfWidth, -halfWidth,  halfWidth), QVector2D(0.0f, 0.0f), QVector3D(1.0f,0.0f,0.0f)));
  vertexes.append(Vertex(QVector3D(  halfWidth,  halfWidth, -halfWidth), QVector2D(1.0f, 1.0f), QVector3D(1.0f,0.0f,0.0f)));

  vertexes.append(Vertex(QVector3D(  halfWidth,  halfWidth, -halfWidth), QVector2D(1.0f, 1.0f), QVector3D(1.0f,0.0f,0.0f)));
  vertexes.append(Vertex(QVector3D(  halfWidth, -halfWidth,  halfWidth), QVector2D(0.0f, 0.0f), QVector3D(1.0f,0.0f,0.0f)));
  vertexes.append(Vertex(QVector3D(  halfWidth, -halfWidth, -halfWidth), QVector2D(1.0f, 0.0f), QVector3D(1.0f,0.0f,0.0f)));

  vertexes.append(Vertex(QVector3D(  halfWidth,  halfWidth,  halfWidth), QVector2D(0.0f, 1.0f), QVector3D(0.0f,1.0f,0.0f)));
  vertexes.append(Vertex(QVector3D(  halfWidth,  halfWidth, -halfWidth), QVector2D(0.0f, 0.0f), QVector3D(0.0f,1.0f,0.0f)));
  vertexes.append(Vertex(QVector3D( -halfWidth,  halfWidth,  halfWidth), QVector2D(1.0f, 1.0f), QVector3D(0.0f,1.0f,0.0f)));

  vertexes.append(Vertex(QVector3D( -halfWidth,  halfWidth,  halfWidth), QVector2D(1.0f, 1.0f), QVector3D(0.0f,1.0f,0.0f)));
  vertexes.append(Vertex(QVector3D(  halfWidth,  halfWidth, -halfWidth), QVector2D(0.0f, 0.0f), QVector3D(0.0f,1.0f,0.0f)));
  vertexes.append(Vertex(QVector3D( -halfWidth,  halfWidth, -halfWidth), QVector2D(1.0f, 0.0f), QVector3D(0.0f,1.0f,0.0f)));

  vertexes.append(Vertex(QVector3D(  halfWidth,  halfWidth, -halfWidth), QVector2D(0.0f, 1.0f), QVector3D(0.0f,0.0f,-1.0f)));
  vertexes.append(Vertex(QVector3D(  halfWidth, -halfWidth, -halfWidth), QVector2D(0.0f, 0.0f), QVector3D(0.0f,0.0f,-1.0f)));
  vertexes.append(Vertex(QVector3D( -halfWidth,  halfWidth, -halfWidth), QVector2D(1.0f, 1.0f), QVector3D(0.0f,0.0f,-1.0f)));

  vertexes.append(Vertex(QVector3D( -halfWidth,  halfWidth, -halfWidth), QVector2D(1.0f, 1.0f), QVector3D(0.0f,0.0f,-1.0f)));
  vertexes.append(Vertex(QVector3D(  halfWidth, -halfWidth, -halfWidth), QVector2D(0.0f, 0.0f), QVector3D(0.0f,0.0f,-1.0f)));
  vertexes.append(Vertex(QVector3D( -halfWidth, -halfWidth, -halfWidth), QVector2D(1.0f, 0.0f), QVector3D(0.0f,0.0f,-1.0f)));

  vertexes.append(Vertex(QVector3D( -halfWidth,  halfWidth,  halfWidth), QVector2D(0.0f, 1.0f), QVector3D(-1.0f,0.0f,0.0f)));
  vertexes.append(Vertex(QVector3D( -halfWidth,  halfWidth, -halfWidth), QVector2D(0.0f, 0.0f), QVector3D(-1.0f,0.0f,0.0f)));
  vertexes.append(Vertex(QVector3D( -halfWidth, -halfWidth,  halfWidth), QVector2D(1.0f, 1.0f), QVector3D(-1.0f,0.0f,0.0f)));

  vertexes.append(Vertex(QVector3D( -halfWidth, -halfWidth,  halfWidth), QVector2D(1.0f, 1.0f), QVector3D(-1.0f,0.0f,0.0f)));
  vertexes.append(Vertex(QVector3D( -halfWidth,  halfWidth, -halfWidth), QVector2D(0.0f, 0.0f), QVector3D(-1.0f,0.0f,0.0f)));
  vertexes.append(Vertex(QVector3D( -halfWidth, -halfWidth, -halfWidth), QVector2D(1.0f, 0.0f), QVector3D(-1.0f,0.0f,0.0f)));

  vertexes.append(Vertex(QVector3D( -halfWidth, -halfWidth,  halfWidth), QVector2D(0.0f, 1.0f), QVector3D(0.0f,-1.0f,0.0f)));
  vertexes.append(Vertex(QVector3D( -halfWidth, -halfWidth, -halfWidth), QVector2D(0.0f, 0.0f), QVector3D(0.0f,-1.0f,0.0f)));
  vertexes.append(Vertex(QVector3D(  halfWidth, -halfWidth,  halfWidth), QVector2D(1.0f, 1.0f), QVector3D(0.0f,-1.0f,0.0f)));

  vertexes.append(Vertex(QVector3D(  halfWidth, -halfWidth,  halfWidth), QVector2D(1.0f, 1.0f), QVector3D(0.0f,-1.0f,0.0f)));
  vertexes.append(Vertex(QVector3D( -halfWidth, -halfWidth, -halfWidth), QVector2D(0.0f, 0.0f), QVector3D(0.0f,-1.0f,0.0f)));
  vertexes.append(Vertex(QVector3D(  halfWidth, -halfWidth, -halfWidth), QVector2D(1.0f, 0.0f), QVector3D(0.0f,-1.0f,0.0f)));


  cubeVBO_.create();
  cubeVBO_.bind();
  cubeVBO_.allocate(vertexes.constData(), vertexes.size() *  sizeof(Vertex));
  cubeVBO_.release();
}

void OpenglWidget::loadTexture(QString path)
{
  texture_ = new QOpenGLTexture(QImage(path).mirrored());
  texture_->setMinificationFilter(QOpenGLTexture::Nearest);
  texture_->setMagnificationFilter(QOpenGLTexture::Linear);
  texture_->setWrapMode(QOpenGLTexture::Repeat);
}

void OpenglWidget::paintWoodContainer(QVector3D position, float scale, QVector3D lightPos, QVector3D lightColor)
{
  QMatrix4x4 model;
  model.setToIdentity();
  model.translate(position);
  model.scale(scale);

  objectShader_.bind();
  auto view = camera_.getView();

  objectShader_.setUniformValue("model", model);
  objectShader_.setUniformValue("view", view);
  objectShader_.setUniformValue("projection", projection_);
  objectShader_.setUniformValue("lightPos", lightPos);
  objectShader_.setUniformValue("lightColor", lightColor);
  objectShader_.setUniformValue("viewPos", camera_.cameraPosition());
  texture_->bind(0);
  objectShader_.setUniformValue("texture0", 0);

  glDrawArrays(GL_TRIANGLES, 0, cubeVBO_.size());
}

void OpenglWidget::paintLight(QVector3D position, QVector3D color, float scale)
{
  QMatrix4x4 model;
  model.setToIdentity();
  model.translate(position);
  model.scale(scale);

  lightShader_.bind();
  auto view = camera_.getView();
  QMatrix4x4 modelViewProjectionMatrix{projection_ * view * model};
  //qDebug() << modelViewProjectionMatrix;
  lightShader_.setUniformValue("qt_ModelViewProjectionMatrix", modelViewProjectionMatrix);
  lightShader_.setUniformValue("color", color);


  glDrawArrays(GL_TRIANGLES, 0, cubeVBO_.size());
}

void OpenglWidget::updateParametrs()
{
  auto rect = geometry();
  resizeGL(rect.width(), rect.height());
  update();
}

