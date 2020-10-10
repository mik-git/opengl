#include "openglwidget.h"
#include "ui_openglwidget.h"

#include <QDebug>
#include <QKeyEvent>
#include <QtMath>
#include <QDateTime>

static const float kCubeWidth = 1.0f;
static const float kFloorWidth = 10.0f;
static const QString kWoodContainer = QString(":/textures/woodcontainer.png");
static const QString kWoodFloor = QString(":/textures/wood.png");
static const QVector<QString> kSkyBoxPaths{":/textures/cubes/skybox/right.jpg",
                                           ":/textures/cubes/skybox/left.jpg",
                                           ":/textures/cubes/skybox/top.jpg",
                                           ":/textures/cubes/skybox/bottom.jpg",
                                           ":/textures/cubes/skybox/front.jpg",
                                           ":/textures/cubes/skybox/back.jpg"};

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
  QObject::connect(&timer_, SIGNAL(timeout()), SLOT(changeLightPosSlot()));
  timer_.start(5);
}

OpenglWidget::~OpenglWidget()
{
  makeCurrent();
  delete ui_;
  delete tWoodContainer_;
  delete tFloor_;
  delete tCubeMap_;
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

void OpenglWidget::rotateCamera(const QPoint& diff)
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

  initScene();
  initShaders();
}

void OpenglWidget::initScene()
{
  tWoodContainer_ = loadTexture(kWoodContainer);
  tFloor_ = loadTexture(kWoodFloor);
  tCubeMap_ = loadCubeMap(kSkyBoxPaths);
  initCube(kCubeWidth);
  initFloor(kFloorWidth);
  initCubeMap();
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

  paintScene();

}

void OpenglWidget::initShaders()
{
  initObjectShader();
  initLightShader();
  initNormalShader();
  initSkyBoxShader();
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

}

void OpenglWidget::initSkyBoxShader()
{
  if ( skyBoxShader_.isLinked() ) { return;}
  qDebug() << "init skyBox shader";
  if (!skyBoxShader_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vSkyBoxShader.vert")) {
    qDebug() << "Error vertex shader";
    close();
  }
  if (!skyBoxShader_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fSkyBoxShader.frag")) {
    qDebug() << "Error fragment shader";
    close();
  }
  if (!skyBoxShader_.link()) {
    qDebug() << "Error link shader program";
    close();
  }

}

void OpenglWidget::initNormalShader()
{
  if ( normalShader_.isLinked() ) { return;}
  qDebug() << "init normal shader";
  if (!normalShader_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vNormalObjectShader.vert")) {
    qDebug() << "Error vertex shader";
    close();
  }
  if (!normalShader_.addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shaders/gNormalObjectShader.geom")) {
    qDebug() << "Error vertex shader";
    close();
  }
  if (!normalShader_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fNormalObjectShader.frag")) {
    qDebug() << "Error fragment shader";
    close();
  }
  if (!normalShader_.link()) {
    qDebug() << "Error link shader program";
    close();
  }

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

void OpenglWidget::initFloor(float width)
{
  qDebug() << "init Floor";
  float halfWidth = width/2;
  //qDebug() << halfWidth << "half";
  QVector<Vertex> vertexes;
  vertexes.append(Vertex(QVector3D(  halfWidth, 0.0f,  halfWidth), QVector2D(0.0f, 1.0f), QVector3D(0.0f,1.0f,0.0f)));
  vertexes.append(Vertex(QVector3D( -halfWidth, 0.0f,  halfWidth), QVector2D(0.0f, 0.0f), QVector3D(0.0f,1.0f,0.0f)));
  vertexes.append(Vertex(QVector3D(  halfWidth, 0.0f, -halfWidth), QVector2D(1.0f, 1.0f), QVector3D(0.0f,1.0f,0.0f)));

  vertexes.append(Vertex(QVector3D(  halfWidth, 0.0f, -halfWidth), QVector2D(1.0f, 1.0f), QVector3D(0.0f,1.0f,0.0f)));
  vertexes.append(Vertex(QVector3D( -halfWidth, 0.0f,  halfWidth), QVector2D(0.0f, 0.0f), QVector3D(0.0f,1.0f,0.0f)));
  vertexes.append(Vertex(QVector3D( -halfWidth, 0.0f, -halfWidth), QVector2D(1.0f, 0.0f), QVector3D(0.0f,1.0f,0.0f)));

  floorVBO_.create();
  floorVBO_.bind();
  floorVBO_.allocate(vertexes.constData(), vertexes.size() *  sizeof(Vertex));
  floorVBO_.release();
}

void OpenglWidget::initCubeMap()
{

  float skyboxVertices[]{
      // Координаты
      -1.0f,  1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,

      -1.0f, -1.0f,  1.0f,
      -1.0f, -1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f,  1.0f,
      -1.0f, -1.0f,  1.0f,

       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,

      -1.0f, -1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f, -1.0f,  1.0f,
      -1.0f, -1.0f,  1.0f,

      -1.0f,  1.0f, -1.0f,
       1.0f,  1.0f, -1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f, -1.0f,

      -1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f,  1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f,  1.0f,
       1.0f, -1.0f,  1.0f
  };
  cubeMapVBO_.create();
  cubeMapVBO_.bind();
  cubeMapVBO_.allocate(skyboxVertices, sizeof(skyboxVertices));
  cubeMapVBO_.release();
}

QOpenGLTexture* OpenglWidget::loadTexture(const QString& path)
{
  QOpenGLTexture* texture = new QOpenGLTexture(QImage(path).mirrored());
  texture->setMinificationFilter(QOpenGLTexture::Nearest);
  texture->setMagnificationFilter(QOpenGLTexture::Linear);
  texture->setWrapMode(QOpenGLTexture::Repeat);
  return texture;
}

QOpenGLTexture* OpenglWidget::loadCubeMap(const QVector<QString> &paths)
{
  QOpenGLTexture* texture = new QOpenGLTexture( QOpenGLTexture::TargetCubeMap );
  texture->create();
  const QImage posx = QImage(paths.first()).convertToFormat(QImage::Format_RGBA8888);
  texture->setSize(posx.width(), posx.height(), posx.depth());
  texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
  texture->allocateStorage();
  texture->setData(0, 0, QOpenGLTexture::CubeMapPositiveX,
                   QOpenGLTexture::RGBA, QOpenGLTexture::UInt8,
                   posx.constBits(), Q_NULLPTR);
  auto it = paths.begin() + 1;
  for ( int i = 1; it < paths.end() ; it++, i++  ) {
    const QImage image = QImage(*it).convertToFormat(QImage::Format_RGBA8888);
    QOpenGLTexture::CubeMapFace face = QOpenGLTexture::CubeMapFace(QOpenGLTexture::CubeMapPositiveX+i);
    texture->setData(0, 0, face,  QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, image.constBits(), Q_NULLPTR);
  }

  texture->setWrapMode(QOpenGLTexture::ClampToEdge);
  texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
  texture->setMagnificationFilter(QOpenGLTexture::LinearMipMapLinear);
  return texture;
}

void OpenglWidget::paintScene()
{
  paintCubeMap();
  //  QVector3D lightPos{1.0, 3.0, 0.0};
  QVector3D lightColor{1.0, 1.0, 1.0};

  paintLight(lightPos_, lightColor, 0.25f);

  QVector3D containerPos1{1.0, 0.0, -3.0};
  paintWoodContainer(containerPos1, 1.0f, lightPos_, lightColor);
  QVector3D containerPos2{2.0, 0.0, -2.0};
  paintWoodContainer(containerPos2, 1.0f, lightPos_, lightColor);
  //    paintNormalWoodContainer(containerPos1);
  //    paintNormalWoodContainer(containerPos2);
  paintFloor(lightPos_,lightColor);
}

void OpenglWidget::paintWoodContainer( const QVector3D& position, float scale, const QVector3D& lightPos, const QVector3D& lightColor)
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
  tWoodContainer_->bind(0);
  objectShader_.setUniformValue("texture0", 0);

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
  objectShader_.setAttributeBuffer(normalLoc, GL_FLOAT, offset, 3, sizeof(Vertex));

  glDrawArrays(GL_TRIANGLES, 0, cubeVBO_.size());
}

void OpenglWidget::paintNormalCube( const QVector3D& position, float scale)
{
  QMatrix4x4 model;
  model.setToIdentity();
  model.translate(position);
  model.scale(scale);

  normalShader_.bind();
  auto view = camera_.getView();

  normalShader_.setUniformValue("model", model);
  normalShader_.setUniformValue("view", view);
  normalShader_.setUniformValue("projection", projection_);

  cubeVBO_.bind();

  int offset = 0;

  auto vertLoc = normalShader_.attributeLocation("inPos");
  normalShader_.enableAttributeArray(vertLoc);
  normalShader_.setAttributeBuffer(vertLoc, GL_FLOAT, offset, 3, sizeof(Vertex));

  offset += sizeof (QVector3D);

  auto texLoc = normalShader_.attributeLocation("inTexCoord");
  normalShader_.enableAttributeArray(texLoc);
  normalShader_.setAttributeBuffer(texLoc, GL_FLOAT, offset, 2, sizeof(Vertex));

  offset += sizeof (QVector2D);

  auto normalLoc = normalShader_.attributeLocation("inNormal");
  normalShader_.enableAttributeArray(normalLoc);
  normalShader_.setAttributeBuffer(normalLoc, GL_FLOAT, offset, 3, sizeof(Vertex));

  glDrawArrays(GL_TRIANGLES, 0, cubeVBO_.size());
}

void OpenglWidget::paintLight(const QVector3D& position, const QVector3D& color, float scale)
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

  cubeVBO_.bind();

  int offset = 0;

  auto vertLoc = lightShader_.attributeLocation("inPos");
  lightShader_.enableAttributeArray(vertLoc);
  lightShader_.setAttributeBuffer(vertLoc, GL_FLOAT, offset, 3, sizeof(Vertex));

  glDrawArrays(GL_TRIANGLES, 0, cubeVBO_.size());
}

void OpenglWidget::paintFloor(const QVector3D& lightPos, const QVector3D& lightColor)
{
  QMatrix4x4 model;
  model.setToIdentity();
  model.translate(QVector3D{0.0f, float(-kCubeWidth/2), 0.0f});
  objectShader_.bind();
  auto view = camera_.getView();

  objectShader_.setUniformValue("model", model);
  objectShader_.setUniformValue("view", view);
  objectShader_.setUniformValue("projection", projection_);
  objectShader_.setUniformValue("lightPos", lightPos);
  objectShader_.setUniformValue("lightColor", lightColor);
  objectShader_.setUniformValue("viewPos", camera_.cameraPosition());
  tFloor_->bind(1);
  objectShader_.setUniformValue("texture0", 1);

  floorVBO_.bind();

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
  objectShader_.setAttributeBuffer(normalLoc, GL_FLOAT, offset, 3, sizeof(Vertex));

  glDrawArrays(GL_TRIANGLES, 0, floorVBO_.size());
}

void OpenglWidget::paintCubeMap()
{
  glDepthMask(GL_FALSE);
  skyBoxShader_.bind();
  auto view = camera_.getView();
  skyBoxShader_.setUniformValue("view", view);
  skyBoxShader_.setUniformValue("projection", projection_);
  tCubeMap_->bind(0);
  skyBoxShader_.setUniformValue("skybox", 0);

  cubeMapVBO_.bind();

  int offset = 0;

  auto vertLoc = skyBoxShader_.attributeLocation("inPos");
  skyBoxShader_.enableAttributeArray(vertLoc);
  skyBoxShader_.setAttributeBuffer(vertLoc, GL_FLOAT, offset, 3, sizeof(float)*3);

  glDrawArrays(GL_TRIANGLES, 0, 36);
  glDepthMask(GL_TRUE);
}

void OpenglWidget::updateParametrs()
{
  auto rect = geometry();
  resizeGL(rect.width(), rect.height());
  update();
}

void OpenglWidget::changeLightPosSlot()
{
  double velocity = 0.0001;
  lightPos_ = QVector3D{6.0f * float(sin(QDateTime::currentMSecsSinceEpoch()*velocity * M_PI)), 2.0f, float(cos(QDateTime::currentMSecsSinceEpoch()*velocity  * M_PI)) * 6.0f};
  updateParametrs();
}
