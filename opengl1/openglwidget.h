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
  void setLightColor(int i, QVector3D color);
  void setLightPosition(int i, QVector3D position);

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

private:
  void initShaders();
  void initObjectShader();
  void initLightShader();
  void initSkyBoxShader();
  void initNormalShader();
  void initScene();
  void initCube(float width);
  void initFloor(float width);
  void initCubeMap();
  QOpenGLTexture* loadTexture( const QString& path );
  QOpenGLTexture* loadCubeMap( const QVector<QString>& paths );
  void paintScene();
  void paintWoodContainer(const QVector3D& translate = QVector3D{0,0,0}, float scale = 1.0f);
  void paintNormalCube( const QVector3D& translate = QVector3D{0,0,0}, float scale = 1.0f);
  void setLightObjectShader();
  void paintLight( const QVector3D& translate = QVector3D{0,0,0}, const QVector3D& color = QVector3D{1.0f,1.0f,1.0f}, float scale = 1.0f );
  void paintFloor();
  void paintCubeMap();
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
  QOpenGLTexture* tWoodContainer_ = nullptr;
  QOpenGLTexture* tFloor_ = nullptr;
  QOpenGLTexture* tCubeMap_ = nullptr;
  QOpenGLBuffer cubeVBO_;
  QOpenGLBuffer floorVBO_;
  QOpenGLBuffer cubeMapVBO_;
  float fow_ = 45.0f;
  float nearPlane_ = 0.1f;
  float farPlane_ = 10.0f;
  bool lamp_ = false;
  Camera camera_;
  QTimer timer_;
  QVector<LightPos> pointLights_;
};

#endif // OPENGLWIDGET_H
