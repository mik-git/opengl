#include "openglwidget.h"
#include "ui_openglwidget.h"

#include <QDebug>

const float kWIDTH = 1.0f;

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

void OpenglWidget::initializeGL()
{
  qDebug() << "initGL";
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_CULL_FACE);

  initShaders();
  initCube(kWIDTH);
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
  shaderProgram_.bind();


  QMatrix4x4 model;
  model.setToIdentity();
  model.translate(0.0,0.0,0.0);
  QVector3D viewPos{-1.0f, 1.0f, 3.0f};
  QVector3D target{0.0f,0.0f,0.0f};
  QVector3D up{0.0f, 1.0f, 0.0f};
  QMatrix4x4 view;
  view.lookAt(viewPos, target, up);

  QMatrix4x4 modelViewProjectionMatrix{projection_ * view * model};
  //qDebug() << modelViewProjectionMatrix;
  shaderProgram_.setUniformValue("qt_ModelViewProjectionMatrix", modelViewProjectionMatrix);
  shaderProgram_.setUniformValue("texture0", 0);
  texture_->bind(0);
  arrayBuffer_.bind();

  int offset = 0;

  auto vertLoc = shaderProgram_.attributeLocation("inPos");
  shaderProgram_.enableAttributeArray(vertLoc);
  shaderProgram_.setAttributeBuffer(vertLoc, GL_FLOAT, offset, 3, sizeof(Vertex));

  offset += sizeof (QVector3D);

  auto texLoc = shaderProgram_.attributeLocation("inTexCoord");
  shaderProgram_.enableAttributeArray(texLoc);
  shaderProgram_.setAttributeBuffer(texLoc, GL_FLOAT, offset, 2, sizeof(Vertex));

  glDrawArrays(GL_TRIANGLES, 0, arrayBuffer_.size());
}

void OpenglWidget::initShaders()
{
  if ( shaderProgram_.isLinked() ) { return;}
  qDebug() << "init shader";
  if (!shaderProgram_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vshader.vert")) {
    qDebug() << "Error vertex shader";
    close();
  }
  if (!shaderProgram_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fshader.frag")) {
    qDebug() << "Error fragment shader";
    close();
  }
  if (!shaderProgram_.link()) {
    qDebug() << "Error link shader program";
    close();
  }
  shaderProgram_.bind();
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


  arrayBuffer_.create();
  arrayBuffer_.bind();
  arrayBuffer_.allocate(vertexes.constData(), vertexes.size() *  sizeof(Vertex));
  arrayBuffer_.release();


  texture_ = new QOpenGLTexture(QImage(":/textures/woodcontainer.png").mirrored());
  texture_->setMinificationFilter(QOpenGLTexture::Nearest);
  texture_->setMagnificationFilter(QOpenGLTexture::Linear);
  texture_->setWrapMode(QOpenGLTexture::Repeat);

}

void OpenglWidget::updateParametrs()
{
  auto rect = geometry();
  resizeGL(rect.width(), rect.height());
  update();
}
