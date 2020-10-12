#include "structs.h"

Vertex::Vertex(QVector3D pos, QVector2D textPos, QVector3D norm) :
  position(pos),
  texturePosition(textPos),
  normal(norm)
{

}

LightPos::LightPos(  QVector3D pos, QVector3D a, QVector3D d, QVector3D s) :
  position(pos),
  ambient(a),
  diffuse(d),
  specular(s)
{

}
