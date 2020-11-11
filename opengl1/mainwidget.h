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
  void setFowSlot(double fow);
  void setNearPlaneSlot(double nearPlane);
  void setFarPlaneSlot(double farPlane);
  void setLightParamSlot();
  void showColorDialogSlot();
  void setRotateFlagSlot(int flag);
  void setPaintCubeMapSlot(int flag);
  void setPaintCubesSlot(int flag);
  void setPaintCustomObjectSlot(int flag);
  void chooseCustomObjectFileSlot();

private:
  void initValue();
  void setCustomObjectPath(QString path);

private:
  Ui::MainWidget *ui_ = nullptr;
  OpenglWidget* opengl_ = nullptr;
  QPointF prevPos_;
  QColor color_{Qt::black};
};

#endif // MAINWIDGET_H
