#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QColorDialog>
#include <QDebug>

static const double kSensitivity = 0.05;

MainWidget::MainWidget(QWidget *parent) :
  QWidget(parent),
  ui_(new Ui::MainWidget),
  opengl_(new OpenglWidget)
{
  ui_->setupUi(this);
  ui_->openglLayout->insertWidget(0, opengl_);
  ui_->settingsBox->setVisible(false);
  QPixmap pix{QSize{32,32}};
  pix.fill(color_);
  ui_->colorButton->setIcon(QIcon(pix));
  QObject::connect(ui_->settingsButton,SIGNAL(clicked()), SLOT(changeVisibleSettingsSlot()));
  QObject::connect(ui_->fowSpinBox, SIGNAL(valueChanged(double)), SLOT(setFowSlot(double)));
  QObject::connect(ui_->nearPlaneSpinBox, SIGNAL(valueChanged(double)), SLOT(setNearPlaneSlot(double)));
  QObject::connect(ui_->farPlaneSpinBox, SIGNAL(valueChanged(double)), SLOT(setFarPlaneSlot(double)));
  QObject::connect(ui_->setButton, SIGNAL(clicked()), SLOT(setLightParamSlot()));
  QObject::connect(ui_->colorButton, SIGNAL(clicked()), SLOT(showColorDialogSlot()) );
  initValue();
}

MainWidget::~MainWidget()
{
  delete ui_;
  delete opengl_;
}

void MainWidget::keyPressEvent(QKeyEvent *event)
{
  switch ( event->key()) {
    case ( Qt::Key::Key_W ): {
      opengl_->goForward();
      break;
    }
    case ( Qt::Key::Key_S ): {
      opengl_->goBack();
      break;
    }
    case ( Qt::Key::Key_A ): {
      opengl_->goLeft();
      break;
    }
    case ( Qt::Key::Key_D ): {
      opengl_->goRight();
      break;
    }
    case ( Qt::Key::Key_F ): {
      opengl_->switchLamp();
      break;
    }
//    case ( Qt::Key::Key_Escape ): //TODO question for escape
//    {
//      close();
//      break;
//    }
  }
  QWidget::keyPressEvent(event);
}

void MainWidget::mousePressEvent(QMouseEvent* event)
{

  if ( event->buttons() == Qt::MouseButton::MiddleButton) {
    prevPos_ = event->localPos();
  }
  QWidget::mousePressEvent(event);
}

void MainWidget::mouseMoveEvent(QMouseEvent* event)
{
  if ( event->buttons() != Qt::MouseButton::MiddleButton) {
    return;
  }
  QPointF pos = event->localPos();
  QPointF diff{prevPos_ - pos};
  prevPos_ = pos;
  diff.setX(-diff.x());
  opengl_->rotateCamera(diff.toPoint());

  QWidget::mouseMoveEvent(event);
}

void MainWidget::wheelEvent(QWheelEvent* event)
{
  auto diff = event->delta() * kSensitivity;
  auto value = ui_->fowSpinBox->value() - diff;
  if ( value < 30.0){
    value = 30.0;
  }
  else if ( value > 90.0 ) {
    value = 90.0;
  }
  ui_->fowSpinBox->setValue( value );

}

void MainWidget::changeVisibleSettingsSlot()
{
  ui_->settingsBox->setVisible(!ui_->settingsBox->isVisible());
}

void MainWidget::setFowSlot( double fow)
{
  opengl_->setFow(float(fow));
}

void MainWidget::setNearPlaneSlot( double nearPlane)
{
  opengl_->setNearPlane(float(nearPlane));
}

void MainWidget::setFarPlaneSlot( double farPlane )
{
  opengl_->setFarPlane(float(farPlane));
}

void MainWidget::setLightParamSlot()
{
  float red = float(color_.redF());
  float green = float(color_.greenF());
  float blue = float(color_.blueF());
  qDebug() << QString(" red %1 green %2 blue %3 ").arg(red).arg(green).arg(blue);
  opengl_->setLightColor(ui_->lightNumberBox->currentIndex(), QVector3D{red,green, blue});
  auto x = float(ui_->xSpinBox->value());
  auto y = float(ui_->ySpinBox->value());
  auto z = float(ui_->zSpinBox->value());
  opengl_->setLightPosition(ui_->lightNumberBox->currentIndex(), QVector3D{x, y, z});
}

void MainWidget::showColorDialogSlot()
{
  QColorDialog *dialog = new QColorDialog();
  dialog->show();
  dialog->setCurrentColor(color_);
  if ( dialog->exec() == QColorDialog::Accepted ) {
    color_ = dialog->currentColor();
    QPixmap pix{QSize{32,32}};
    pix.fill(color_);
    ui_->colorButton->setIcon(QIcon(pix));
  }
  dialog->deleteLater();
}

void MainWidget::initValue()
{
  ui_->fowSpinBox->setValue(45.0);
  ui_->nearPlaneSpinBox->setValue(0.1);
  ui_->farPlaneSpinBox->setValue(100.0);
}
