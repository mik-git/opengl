#include "camera.h"

#include <QtMath>

static const float kCameraSpeed = 0.1f;
static const float kSensitivity = 0.05f;

Camera::Camera()
{

}

Camera::Camera(QVector3D cameraPos, QVector3D cameraFront) :
  cameraPos_(cameraPos),
  cameraFront_(cameraFront)
{

}

QMatrix4x4 Camera::getView()
{
  QMatrix4x4 view;
  view.lookAt(cameraPos_, cameraPos_ + cameraFront_, up_);
  return view;
}

void Camera::goForward()
{
  cameraPos_ += cameraFront_ * kCameraSpeed;
}

void Camera::goBack()
{
  cameraPos_ -= cameraFront_ * kCameraSpeed;
}

void Camera::goLeft()
{
  auto right = QVector3D::crossProduct(cameraFront_, up_);
  right.normalize();
  cameraPos_ -= right * kCameraSpeed;
}

void Camera::goRight()
{
  auto right = QVector3D::crossProduct(cameraFront_, up_);
  right.normalize();
  cameraPos_ += right * kCameraSpeed;
}

void Camera::rotateCamera(QPoint diff )
{
  yaw_   += diff.x() * kSensitivity;
  pitch_ += diff.y() * kSensitivity;

  if(pitch_ > 89.0f) {
    pitch_ = 89.0f;
  }
  if(pitch_ < -89.0f) {
    pitch_ = -89.0f;
  }
  cameraFront_.setX( cos(qDegreesToRadians(yaw_)) * cos(qDegreesToRadians(pitch_)) );
  cameraFront_.setY( sin(qDegreesToRadians(pitch_)) );
  cameraFront_.setZ( sin(qDegreesToRadians(yaw_)) * cos(qDegreesToRadians(pitch_)) );
  cameraFront_.normalize();
}
