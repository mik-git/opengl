#ifndef STRUCTS_H
#define STRUCTS_H

#include <QVector2D>
#include <QVector3D>

struct Vertex
{
  Vertex(){}
  Vertex( QVector3D position, QVector2D texturePosition, QVector3D normal );

  QVector3D position;
  QVector2D texturePosition;
  QVector3D normal;
  QVector3D tangent;
  QVector3D bitangent;
};


struct LightPos {
  LightPos(  QVector3D position = QVector3D{0.0f, 0.0f, 0.0f}, QVector3D ambient = QVector3D{0.05f, 0.05f, 0.05f},
             QVector3D diffuse = QVector3D{1.0f, 1.0f, 1.0f}, QVector3D specular = QVector3D{0.1f, 0.1f, 0.1f});
  QVector3D position{0.0f, 0.0f, 0.0f};
  QVector3D ambient{0.05f, 0.05f, 0.05f};
  QVector3D diffuse{1.0f, 1.0f, 1.0f};
  QVector3D specular{0.1f, 0.1f, 0.1f};

  float constant{1.0f};
  float linear{0.09f};
  float quadratic{0.032f};
};


#endif // STRUCTS_H
