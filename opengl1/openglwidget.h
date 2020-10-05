#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>

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

protected:
  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;

private:
  void initShaders();
  void initCube(float width);
  void updateParametrs();



private:
  Ui::OpenglWidget *ui_ = nullptr;
  QMatrix4x4 projection_;
  QOpenGLShaderProgram shaderProgram_;
  QOpenGLTexture* texture_ = nullptr;
  QOpenGLBuffer arrayBuffer_;
  float fow_ = 45.0f;
  float nearPlane_ = 0.1f;
  float farPlane_ = 10.0f;
};

#endif // OPENGLWIDGET_H
