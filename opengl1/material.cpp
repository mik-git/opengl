#include "material.h"
#include <QDebug>

Material::Material(const QString& name) :
  name_{name}
{

}

Material::~Material()
{
  delete tAlbedo_;
  delete tNormal_;
  delete tSpecular_;
  delete tMetallic_;
  delete tRoughness_;
  delete tAO_;
}

bool Material::hasTextureAlbedo()
{
  return ( tAlbedo_ != nullptr);
}

bool Material::hasTextureNormal()
{
  return ( tNormal_ != nullptr);
}

bool Material::hasTextureSpecular()
{
  return ( tSpecular_ != nullptr);
}

bool Material::hasTextureMetallic()
{
  return ( tMetallic_ != nullptr);
}

bool Material::hasTextureRoughness()
{
  return ( tRoughness_ != nullptr);
}

bool Material::hasTextureAmbientOcclusion()
{
  return ( tAO_ != nullptr);
}

void Material::loadTextureAlbedo(const QString& path)
{
  tAlbedo_ = loadTexture(path);
}

void Material::loadTextureNormal(const QString& path)
{
  tNormal_ = loadTexture(path);
}

void Material::loadTextureSpecular(const QString& path)
{
  tSpecular_ = loadTexture(path);
}

void Material::loadTextureMetallic(const QString& path)
{
  qDebug() << "Load metallic";
  tMetallic_ = loadTexture(path);
}

void Material::loadTextureRoughness(const QString& path)
{
  qDebug() << "Load Roughenss";
  tRoughness_ = loadTexture(path);
}

void Material::loadTextureAmbientOcclusion(const QString& path)
{
  qDebug() << "Load AO";
  tAO_ = loadTexture(path);
}

QOpenGLTexture* Material::loadTexture(const QString& path)
{
  QOpenGLTexture* texture = new QOpenGLTexture(QImage(path));
  texture->setMinificationFilter(QOpenGLTexture::Nearest);
  texture->setMagnificationFilter(QOpenGLTexture::Linear);
  texture->setWrapMode(QOpenGLTexture::Repeat);
  return texture;
}
