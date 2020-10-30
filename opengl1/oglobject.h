#ifndef OGLOBJECT_H
#define OGLOBJECT_H

#include <memory>

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

#include "structs.h"
#include "material.h"
#include "mesh.h"

class OGLObject
{
public:
  OGLObject( const QString& path );
  OGLObject(QVector<Vertex>& vertexes, QVector<GLuint>& indexes );

  void load( const QString& path );
  void create(QVector<Vertex>& vertexes, QVector<GLuint>& indexes  );
  void draw( QOpenGLShaderProgram& shader );

private:
  void loadMtl(const QString& path);

private:
  QOpenGLBuffer VBO_;
  QOpenGLBuffer EBO_{QOpenGLBuffer::IndexBuffer};
  QMap<QString, std::shared_ptr<Material> > materialMap_;
  QVector< std::shared_ptr<Mesh> > meshs_;


};

#endif // OGLOBJECT_H
