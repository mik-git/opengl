#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "openglwidget.h"
#include <QWidget>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
  Q_OBJECT

public:
  explicit MainWidget(QWidget *parent = nullptr);
  ~MainWidget() override;

protected:
  void keyPressEvent(QKeyEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void wheelEvent(QWheelEvent *event) override;

private slots:
  void changeVisibleSettingsSlot();
  void sendFowSlot(double fow);
  void sendNearPlaneSlot(double nearPlane);
  void sendFarPlaneSlot(double farPlane);

private:
  void initValue();

private:
  Ui::MainWidget *ui_ = nullptr;
  OpenglWidget* opengl_ = nullptr;
  QPointF prevPos_;
};

#endif // MAINWIDGET_H
