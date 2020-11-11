#include "oglobject.h"
#include "structs.h"

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QVector3D>
#include <QVector2D>

OGLObject::OGLObject( const QString& path)
{
  load(path);
}

OGLObject::OGLObject(QVector<Vertex>& vertexes, QVector<GLuint>& indexes )
{
  create(vertexes, indexes);
}

void OGLObject::load(const QString& path)
{
  QFile file{path};
  if ( !file.exists() ) {
    qDebug() << QString(" file %1 not exists ").arg(path);
    return;
  }
  if ( !file.open(QFile::ReadOnly) ) {
    qDebug() << QString(" file %1 not open ").arg(path);
    return;
  }
  QTextStream stream{&file};
  QVector<QVector3D> coords;
  QVector<QVector2D> texCoords;
  QVector<QVector3D> normals;

  QVector<Vertex> vertexes;
  QVector<GLuint> indexes;
  std::shared_ptr<Mesh> mesh;
  while( !stream.atEnd()) {
    QString line{stream.readLine()};
    QStringList tokenList{line.split(" ")};
    if (tokenList.first() == QString("#")) {
//      qDebug() << QString("Comment string %1").arg(tokenList.first());
    }
    else if ( tokenList.first() == QString("mtllib") ) {
      QFileInfo fInfo{file};
      QStringList mtlNameList{tokenList};
      mtlNameList.removeFirst();
      QString fName{mtlNameList.join(" ")}; // если имя файла содержит пробелы
      fName = fInfo.absolutePath() + QDir::separator() + fName;
      loadMtl( fName );
    }
    else if (  tokenList.first() == QString("o") ) {
      if ( !vertexes.isEmpty() && !indexes.isEmpty() && mesh) {
        mesh->create(vertexes, indexes);
        meshs_.append(mesh);
        vertexes.clear();
        indexes.clear();
      }
      mesh = std::make_shared<Mesh>();
    }
    else if (tokenList.first() == QString("v")) { //вершинные координаты
      coords.append( QVector3D{tokenList.at(1).toFloat(), tokenList.at(2).toFloat(), tokenList.at(3).toFloat()} );
    }
    else if (tokenList.first() == QString("vt")) { // техтурные координаты
      texCoords.append( QVector2D{tokenList.at(1).toFloat(), tokenList.at(2).toFloat()} );
    }
    else if (tokenList.first() == QString("vn")) { // нормали
      normals.append( QVector3D{tokenList.at(1).toFloat(), tokenList.at(2).toFloat(), tokenList.at(3).toFloat()} );
    }
    else if ( tokenList.first() == QString("usemtl")) {
      QStringList matNameList{tokenList};
      matNameList.removeFirst();
      QString materialName{matNameList.join(" ")}; // если имя материала содерит пробелы
      if ( materialMap_.contains(materialName) ) {
       std::shared_ptr<Material> material = materialMap_[materialName];
       mesh->setMaterial(material);
      }
      else {
        qDebug() << QString("Error material %1 not exists").arg(materialName);
      }

    }
    else if (tokenList.first() == QString("f")) { //индексы
      for ( int i = 1; i <= 3; i++ ) {
        auto vertex = tokenList.at(i).split("/");
        //в obj файле порядок индексов идёт с 1. Переводим индексы.
        vertexes.append(Vertex{ coords.at(vertex.at(0).toInt() - 1), texCoords.at(vertex.at(1).toInt() - 1), normals.at(vertex.at(2).toInt() - 1),});
        indexes.append(indexes.size());
      }
    }
  }
  if ( !vertexes.isEmpty() && !indexes.isEmpty() ) {
    mesh->create(vertexes, indexes);
    meshs_.append(mesh);
    vertexes.clear();
    indexes.clear();
  }
  file.close();
}

void OGLObject::create(QVector<Vertex>& vertexes, QVector<GLuint>& indexes)
{
  meshs_.clear();
  auto mesh = std::make_shared<Mesh>();
  mesh->create(vertexes, indexes);
  meshs_.append(mesh);
}

void OGLObject::draw(QOpenGLShaderProgram& shader)
{
  for( auto& mesh: meshs_ ) {
    mesh->draw(shader);
  }
}

void OGLObject::loadMtl(const QString& path)
{
  qDebug() << "loadMTL" << path;
  QFile file{path};
  if ( !file.exists() ) {
    qDebug() << QString(" file %1 not exists ").arg(path);
    return;
  }
  if ( !file.open(QFile::ReadOnly) ) {
    qDebug() << QString(" file %1 not open ").arg(path);
    return;
  }
  QTextStream stream{&file};
  std::shared_ptr<Material> material;
  QFileInfo fInfo{file};
  QString absolutePath{fInfo.absolutePath()} ;

  auto filePathFromToken = [ &absolutePath = absolutePath ]( QStringList& fileNameList ) {
    fileNameList.removeFirst();
    QString fName{fileNameList.join(" ")};
    fName = absolutePath + QDir::separator() + fName;
    return fName;
  };

  while( !stream.atEnd()) {
    QString line{stream.readLine()};
    QStringList tokenList{line.split(" ")};
    if (tokenList.first() == QString("#")) {
//      qDebug() << QString("Comment string %1").arg(tokenList.first());
    }
    else if (  tokenList.first() == QString("newmtl") ) {
      QStringList mNameList{tokenList};
      mNameList.removeFirst();
      QString mName{mNameList.join(" ")}; //если имя содержит пробелы
      material = std::make_shared<Material>(mName);
      materialMap_.insert(mName, material);
    }
    else if (tokenList.first() == QString("Ns")) {
      if ( material ) {
        material->setSpecularExponent(tokenList.at(1).toFloat());
      }
    }
    else if (tokenList.first() == QString("Ka")) {
      if ( material ) {
        material->setAmbientColor(tokenList.at(1).toFloat(), tokenList.at(2).toFloat(), tokenList.at(3).toFloat());
      }
    }
    else if (tokenList.first() == QString("Kd")) {
      if ( material ) {
        material->setDiffuseColor(tokenList.at(1).toFloat(), tokenList.at(2).toFloat(), tokenList.at(3).toFloat());
      }
    }
    else if (tokenList.first() == QString("Ks")) {
      if ( material ) {
        material->setSpecularColor(tokenList.at(1).toFloat(), tokenList.at(2).toFloat(), tokenList.at(3).toFloat());
      }
    }
    else if (tokenList.first() == QString("Ke")) {
      if ( material ) {
        material->setEmissive(tokenList.at(1).toFloat(), tokenList.at(2).toFloat(), tokenList.at(3).toFloat());
      }
    }
    else if (tokenList.first() == QString("Ni")) {
      if ( material ) {
        material->setDensity(tokenList.at(1).toFloat());
      }
    }
    else if (tokenList.first() == QString("d")) {
      if ( material ) {
        material->setTransparent(tokenList.at(1).toFloat());
      }
    }
    else if (tokenList.first() == QString("illum")) {
      if ( material ) {
        material->setIllum(tokenList.at(1).toFloat());
      }
    }
    else if (tokenList.first() == QString("map_Kd")) { // albedo
      if ( material ) {
        auto tName = filePathFromToken(tokenList);
        material->loadTextureAlbedo(tName);
      }
    }
    else if (tokenList.first() == QString("map_Bump")) {  // normal
      if ( material ) {
        auto tName = filePathFromToken(tokenList);
        material->loadTextureNormal(tName);
      }
    }
    else if (tokenList.first() == QString("map_Ks")) { // specular
      if ( material ) {
        auto tName = filePathFromToken(tokenList);
        material->loadTextureSpecular(tName);
      }
    }
    else if (tokenList.first() == QString("map_Pm")) { // metallic
      if ( material ) {
        auto tName = filePathFromToken(tokenList);
        material->loadTextureMetallic(tName);
      }
    }
    else if (tokenList.first() == QString("map_Pr")) { // roughness
      if ( material ) {
        auto tName = filePathFromToken(tokenList);
        material->loadTextureRoughness(tName);
      }
    }
    else if (tokenList.first() == QString("map_Ka")) { // AO
      if ( material ) {
        auto tName = filePathFromToken(tokenList);
        material->loadTextureAmbientOcclusion(tName);
      }
    }
  }
  file.close();
}
