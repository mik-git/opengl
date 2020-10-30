#include "material.h"

Material::Material(const QString& name) :
  name_{name}
{

}

Material::~Material()
{
 delete tAlbedo_;
 delete tNormal_;
 delete tSpecular_;
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

QOpenGLTexture* Material::loadTexture(const QString& path)
{
  QOpenGLTexture* texture = new QOpenGLTexture(QImage(path));
  texture->setMinificationFilter(QOpenGLTexture::Nearest);
  texture->setMagnificationFilter(QOpenGLTexture::Linear);
  texture->setWrapMode(QOpenGLTexture::Repeat);
  return texture;
}
