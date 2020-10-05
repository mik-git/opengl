#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent) :
  QWidget(parent),
  ui_(new Ui::MainWidget),
  opengl_(new OpenglWidget)
{
  ui_->setupUi(this);
  ui_->openglLayout->insertWidget(0, opengl_);
  QObject::connect(ui_->settingsButton,SIGNAL(clicked()), SLOT(changeVisibleSettingsSlot()));
  QObject::connect(ui_->fowSpinBox, SIGNAL(valueChanged(double)), SLOT(sendFowSlot(double)));
  QObject::connect(ui_->nearPlaneSpinBox, SIGNAL(valueChanged(double)), SLOT(sendNearPlaneSlot(double)));
  QObject::connect(ui_->farPlaneSpinBox, SIGNAL(valueChanged(double)), SLOT(sendFarPlaneSlot(double)));
}

MainWidget::~MainWidget()
{
  delete ui_;
  delete opengl_;
}

void MainWidget::changeVisibleSettingsSlot()
{
  ui_->settingsBox->setVisible(!ui_->settingsBox->isVisible());
}

void MainWidget::sendFowSlot( double fow)
{
  opengl_->setFow(float(fow));
}

void MainWidget::sendNearPlaneSlot( double nearPlane)
{
  opengl_->setNearPlane(float(nearPlane));
}

void MainWidget::sendFarPlaneSlot( double farPlane )
{
  opengl_->setFarPlane(float(farPlane));
}
