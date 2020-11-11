#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QColorDialog>
#include <QDebug>
#include <QFileDialog>

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
  QObject::connect(ui_->plainSceneCheckBox, SIGNAL(stateChanged(int)), SLOT(setPaintCubesSlot(int)) );
  QObject::connect(ui_->rotateCheckBox, SIGNAL(stateChanged(int)), SLOT(setRotateFlagSlot(int)));
  QObject::connect(ui_->cubeMapCheckBox,SIGNAL(stateChanged(int)), SLOT(setPaintCubeMapSlot(int)));
  QObject::connect(ui_->customObjectCheckBox, SIGNAL(stateChanged(int)), SLOT(setPaintCustomObjectSlot(int)));
  QObject::connect(ui_->fileButton, SIGNAL(clicked()), SLOT(chooseCustomObjectFileSlot()));
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
    case ( Qt::Key::Key_Escape ): //TODO question for escape
    {
      close();
      break;
    }
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

void MainWidget::setRotateFlagSlot(int flag)
{
  opengl_->setRotate(bool(flag));
}

void MainWidget::setPaintCubeMapSlot(int flag)
{
  opengl_->setPaintCubeMap(bool(flag));
}

void MainWidget::setPaintCubesSlot(int flag)
{
  opengl_->setPaintCubes(bool(flag));
}

void MainWidget::setPaintCustomObjectSlot(int flag)
{
  opengl_->setPaintCustomObject(bool(flag));
}

void MainWidget::chooseCustomObjectFileSlot()
{
  auto fileName = QFileDialog::getOpenFileName(this, tr("Open Obj File"), QDir::homePath(), tr("Object Files (*.obj)"));
  qDebug() << "filename " <<  fileName;
  if (!fileName.isEmpty()) {
    setCustomObjectPath(fileName);
  }
}

void MainWidget::setCustomObjectPath(QString path)
{
  ui_->filePath->setText(path);
  opengl_->initCustomObject(path);
}

void MainWidget::initValue()
{
  ui_->fowSpinBox->setValue(45.0);
  ui_->nearPlaneSpinBox->setValue(0.1);
  ui_->farPlaneSpinBox->setValue(100.0);
  ui_->cubeMapCheckBox->setChecked(true);
  ui_->plainSceneCheckBox->setChecked(true);
}
