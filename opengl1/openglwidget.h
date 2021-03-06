#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QTimer>

#include "camera.h"
#include "structs.h"
#include "oglobject.h"


namespace Ui {
class OpenglWidget;
}

class OpenglWidget : public QOpenGLWidget
{
  Q_OBJECT

public:
  explicit OpenglWidget(QWidget *parent = nullptr);
  ~OpenglWidget() override;

  void setFow(float fow);
  void setNearPlane(float nearPlane);
  void setFarPlane(float farPlane);

  void goForward();
  void goBack();
  void goLeft();
  void goRight();
  void rotateCamera(const QPoint& diff );
  void switchLamp();
  void setRotate( bool flag );
  void setPaintCubeMap( bool flag );
  void setPaintCubes( bool flag );
  void setPaintCustomObject( bool flag );
  void setLightColor(int i, QVector3D color);
  void setLightPosition(int i, QVector3D position);
  void initCustomObject( QString& path );

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;
  void timerEvent(QTimerEvent* event) override;

private:
  void initShaders();
  void initObjectShader();
  void initLightShader();
  void initSkyBoxShader();
  void initNormalShader();
  void initCustomObjectShader();
  void initPBRShader();
  void initScene();
  void initCube(float width);
  void initFloor(float width);
  void initCubeMap();
  void initTest();
  QOpenGLTexture* loadTexture( const QString& path );
  QOpenGLTexture* loadCubeMap( const QVector<QString>& paths );
  void paintScene();
  void paintWoodContainer(const QVector3D& translate = QVector3D{0,0,0}, float scale = 1.0f);
  void paintNormalCube( const QVector3D& translate = QVector3D{0,0,0}, float scale = 1.0f);
  void setLightShader( QOpenGLShaderProgram& shader );
  void paintLight( const QVector3D& translate = QVector3D{0,0,0}, const QVector3D& color = QVector3D{1.0f,1.0f,1.0f}, float scale = 1.0f );
  void paintFloor();
  void paintCubeMap();
  void paintCustomObject();
  void paintTest(QOpenGLShaderProgram& shader);
  void updateParametrs();
  void defaultPointsLights();

private slots:
  void changeLightPosSlot();


private:
  Ui::OpenglWidget *ui_ = nullptr;
  QMatrix4x4 projection_;
  QOpenGLShaderProgram objectShader_;
  QOpenGLShaderProgram lightShader_;
  QOpenGLShaderProgram normalShader_;
  QOpenGLShaderProgram skyBoxShader_;
  QOpenGLShaderProgram customObjectShader_;
  QOpenGLShaderProgram PBRShader_;
  QOpenGLTexture* tWoodContainer_ = nullptr;
  QOpenGLTexture* tFloor_ = nullptr;
  QOpenGLTexture* tCubeMap_ = nullptr;
  OGLObject* customObject_ = nullptr;
  QOpenGLBuffer cubeVBO_;
  QOpenGLBuffer floorVBO_;
  QOpenGLBuffer cubeMapVBO_;
  QOpenGLBuffer testVBO_;
  QOpenGLBuffer testEBO_{QOpenGLBuffer::IndexBuffer};
  float fow_ = 45.0f;
  float nearPlane_ = 0.1f;
  float farPlane_ = 10.0f;
  bool lamp_ = false;
  bool rotateFlag_ = false;
  bool paintCubes_ = true;
  bool paintLights_ = true;
  bool paintCustomObject_ = false;
  bool paintCubeMap_ = true;
  Camera camera_;
  QVector<LightPos> pointLights_;
  QQuaternion rotate_;
  float angle_ = 0;
};

#endif // OPENGLWIDGET_H
