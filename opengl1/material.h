#ifndef MATERIAL_H
#define MATERIAL_H

#include <QVector3D>
#include <QOpenGLTexture>

class Material
{
public:
  Material() = default;
  Material(const QString& name);
  Material(const Material& material) = delete;
  ~Material();

  Material& operator=(const Material&) = delete;

  bool hasTextureAlbedo();
  bool hasTextureNormal();
  bool hasTextureSpecular();
  bool hasTextureMetallic();
  bool hasTextureRoughness();
  bool hasTextureAmbientOcclusion();

  void setName( const QString& name ) { name_ = name; }
  void setSpecularExponent( float Ns ) { Ns_ = Ns; }
  void setDensity( float Ni ) { Ni_ = Ni; }
  void setTransparent( float d ) { d_ = d; }
  void setIllum( float illum ) { illum_ = illum; }
  void setAmbientColor( const QVector3D& Ka ) { Ka_ = Ka; }
  void setDiffuseColor( const QVector3D& Kd ) { Kd_ = Kd; }
  void setSpecularColor( const QVector3D& Ks ) { Ks_ = Ks; }
  void setEmissive( const QVector3D& Ke ) { Ke_ = Ke; }
  void setAmbientColor( float x, float y, float z ) { Ka_ = QVector3D{x,y,z}; }
  void setDiffuseColor( float x, float y, float z ) { Kd_ = QVector3D{x,y,z}; }
  void setSpecularColor( float x, float y, float z ) { Ks_ = QVector3D{x,y,z}; }
  void setEmissive( float x, float y, float z ) { Ke_ = QVector3D{x,y,z}; }
  void loadTextureAlbedo(const QString& path);
  void loadTextureNormal(const QString& path);
  void loadTextureSpecular(const QString& path);
  void loadTextureMetallic(const QString& path);
  void loadTextureRoughness(const QString& path);
  void loadTextureAmbientOcclusion(const QString& path);

  QString name() { return name_; }
  float specularExponent() { return Ns_; }
  float density() { return Ni_; }
  float transparent() { return d_; }
  float illum() { return illum_;}
  QVector3D ambientColor() { return Ka_; }
  QVector3D diffuseColor() { return Kd_; }
  QVector3D specularColor() { return Ks_; }
  QVector3D emissive() { return Ke_; }
  QOpenGLTexture* textureAlbedo() { return  tAlbedo_; }
  QOpenGLTexture* textureNormal() { return tNormal_; }
  QOpenGLTexture* textureSpecular() { return tSpecular_; }

  QOpenGLTexture* textureMetallic() { return  tMetallic_; }
  QOpenGLTexture* textureRoughness() { return tRoughness_; }
  QOpenGLTexture* textureAmbientOcclusion() { return tAO_; }
  float metallic() { return metallic_; }
  float roughness() { return roughness_; }
  float ao() { return ao_; }

private:
  QOpenGLTexture* loadTexture(const QString& path);

private:
  QString name_;
  float Ns_;
  float Ni_;
  float d_;
  float illum_;
  float metallic_ = 0.0f;
  float roughness_ = 0.0f;
  float ao_ = 1.0f;
  QVector3D Ka_;
  QVector3D Kd_;
  QVector3D Ks_;
  QVector3D Ke_;
  QOpenGLTexture* tAlbedo_ = nullptr;
  QOpenGLTexture* tNormal_ = nullptr;
  QOpenGLTexture* tSpecular_ = nullptr;
  QOpenGLTexture* tMetallic_ = nullptr;
  QOpenGLTexture* tRoughness_ = nullptr;
  QOpenGLTexture* tAO_ = nullptr;


};

#endif // MATERIAL_H
