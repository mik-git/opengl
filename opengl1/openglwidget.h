#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>

#include "camera.h"

struct Vertex
{
  Vertex(){}
  Vertex( QVector3D position, QVector2D texturePosition, QVector3D normal );

  QVector3D position_;
  QVector2D texturePosition_;
  QVector3D normal_;
};

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
  void rotateCamera(QPoint diff );
  void switchLamp();

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

private:
  void initShaders();
  void initObjectShader();
  void initLightShader();
  void initCube(float width);
  void loadTexture(QString path);
  void paintWoodContainer(QVector3D translate = QVector3D{0,0,0}, float scale = 1.0f,
                          QVector3D lightPos = QVector3D{0.0f,0.0f,0.0f}, QVector3D lightColor = QVector3D{0.0f,0.0f,0.0f} );
  void paintLight(QVector3D translate = QVector3D{0,0,0}, QVector3D color = QVector3D{1.0f,1.0f,1.0f}, float scale = 1.0f );
  void updateParametrs();


private:
  Ui::OpenglWidget *ui_ = nullptr;
  QMatrix4x4 projection_;
  QOpenGLShaderProgram objectShader_;
  QOpenGLShaderProgram lightShader_;
  QOpenGLTexture* texture_ = nullptr;
  QOpenGLBuffer cubeVBO_;
  float fow_ = 45.0f;
  float nearPlane_ = 0.1f;
  float farPlane_ = 10.0f;
  bool lamp_ = false;
  Camera camera_;
};

#endif // OPENGLWIDGET_H
