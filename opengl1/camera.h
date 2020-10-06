#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>
#include <QMatrix4x4>

class Camera
{
public:
  Camera();
  Camera(QVector3D cameraPos, QVector3D cameraFront);
  void setCameraPosition(QVector3D pos) { cameraPos_ = pos; }
  void setCameraFront(QVector3D front) { cameraFront_ = front; }
  QMatrix4x4 getView();
  void goForward();
  void goBack();
  void goLeft();
  void goRight();
  void rotateCamera(QPoint diff );

private:
  QVector3D cameraPos_{0.0f, 0.0f, 3.0f};
  QVector3D cameraFront_{0.0f, 0.0f, -1.0f};
  QVector3D up_{0.0f, 1.0f, 0.0f};
  float yaw_ = -90.0f;
  float pitch_ = 0.0f;
};

#endif // CAMERA_H
