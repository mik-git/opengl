#ifndef MESH_H
#define MESH_H

#include <memory>

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include "structs.h"
#include "material.h"

class Mesh
{
public:
  Mesh();
  Mesh(QVector<Vertex>& vertexes, QVector<GLuint>& indexes );

  void draw(QOpenGLShaderProgram& shader);
  void create(QVector<Vertex>& vertexes, QVector<GLuint>& indexes);
  void setMaterial( const std::shared_ptr<Material>& material) { material_ = material; }
  void clear();

private:
  void calculateTBN(QVector<Vertex>& vertexes);

private:
  QOpenGLBuffer VBO_;
  QOpenGLBuffer EBO_{QOpenGLBuffer::IndexBuffer};
  std::shared_ptr<Material> material_ = nullptr;
};

#endif // MESH_H
