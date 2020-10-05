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
  ~MainWidget();

private slots:
  void changeVisibleSettingsSlot();
  void sendFowSlot(double fow);
  void sendNearPlaneSlot(double nearPlane);
  void sendFarPlaneSlot(double farPlane);

private:
  Ui::MainWidget *ui_ = nullptr;
  OpenglWidget* opengl_ = nullptr;
};

#endif // MAINWIDGET_H
