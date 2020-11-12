#include "mesh.h"

Mesh::Mesh()
{

}

Mesh::Mesh(QVector<Vertex>& vertexes, QVector<GLuint>& indexes)
{
  create(vertexes, indexes);
}

void Mesh::draw(QOpenGLShaderProgram& shader)
{
    if (!shader.isLinked() || !VBO_.isCreated() || !EBO_.isCreated() ) {
      return;
    }
    shader.bind();
    if ( material_->hasTextureAlbedo() ) {
      material_->textureAlbedo()->bind(0);
      shader.setUniformValue("albedo0",0);
      shader.setUniformValue("useAlbedoMap",true);
    }
    else {
      shader.setUniformValue("useAlbedoMap", false);
    }
    if ( material_->hasTextureNormal() ) {
      material_->textureNormal()->bind(1);
      shader.setUniformValue("normal0",1);
      shader.setUniformValue("useNormalMap",true);
    }
    else {
      shader.setUniformValue("useNormalMap",false);
    }
    if ( material_->hasTextureMetallic()){
      material_->textureMetallic()->bind(2);
      shader.setUniformValue("metallic0",2);
      shader.setUniformValue("useMetallicMap",true);
    }
    else {
      shader.setUniformValue("material.metallic", material_->metallic() );
      shader.setUniformValue("useMetallicMap",false);
    }
    if( material_->hasTextureRoughness() ) {
      material_->textureRoughness()->bind(3);
      shader.setUniformValue("roughness0",3);
      shader.setUniformValue("useRoughnessMap",true);
    }
    else {
      shader.setUniformValue("material.roughness", material_->roughness() );
      shader.setUniformValue("useRoughnessMap",false);
    }
    if ( material_->hasTextureAmbientOcclusion() ) {
      material_->textureAmbientOcclusion()->bind(4);
      shader.setUniformValue("ao0",4);
      shader.setUniformValue("useAOMap",true);
    }
    else {
      shader.setUniformValue("material.ao", material_->ao() );
      shader.setUniformValue("useAOMap",false);
    }
    shader.setUniformValue("material.specularExponent", material_->specularColor() );
    shader.setUniformValue("material.ambientColor", material_->ambientColor() );
    shader.setUniformValue("material.diffuseColor", material_->diffuseColor() );
    shader.setUniformValue("material.specularColor", material_->specularColor() );


    VBO_.bind();

    int offset = 0;

    auto vertLoc = shader.attributeLocation("inPos");
    shader.enableAttributeArray(vertLoc);
//    qDebug() << vertLoc;
    shader.setAttributeBuffer(vertLoc, GL_FLOAT, offset, 3, sizeof(Vertex));

    offset += sizeof (QVector3D);

    auto texLoc = shader.attributeLocation("inTexCoord");
    shader.enableAttributeArray(texLoc);
//    qDebug() << texLoc;
    shader.setAttributeBuffer(texLoc, GL_FLOAT, offset, 2, sizeof(Vertex));

    offset += sizeof (QVector2D);

    auto normalLoc = shader.attributeLocation("inNormal");
    shader.enableAttributeArray(normalLoc);
//    qDebug() << normalLoc;
    shader.setAttributeBuffer(normalLoc, GL_FLOAT, offset, 3, sizeof(Vertex));

    offset += sizeof (QVector3D);

    auto tangentLoc = shader.attributeLocation("inTangent");
    shader.enableAttributeArray(tangentLoc);
    shader.setAttributeBuffer(tangentLoc, GL_FLOAT, offset, 3, sizeof(Vertex));

    offset += sizeof (QVector3D);

    auto bitangentLoc = shader.attributeLocation("inBitangent");
    shader.enableAttributeArray(bitangentLoc);
    shader.setAttributeBuffer(bitangentLoc, GL_FLOAT, offset, 3, sizeof(Vertex));


    EBO_.bind();
    glDrawElements(GL_TRIANGLES, EBO_.size(), GL_UNSIGNED_INT, nullptr);

    EBO_.release();
    VBO_.release();
    if ( material_->hasTextureAlbedo()) {
      material_->textureAlbedo()->release();
    }
    if ( material_->hasTextureNormal()) {
      material_->textureNormal()->release();
    }
    if (material_->hasTextureMetallic()) {
      material_->textureMetallic()->release();
    }
    if (material_->hasTextureSpecular()) {
      material_->textureSpecular()->release();
    }
    if (material_->hasTextureRoughness()) {
      material_->textureRoughness()->release();
    }
    if (material_->hasTextureAmbientOcclusion()) {
      material_->textureAmbientOcclusion()->release();
    }
    shader.release();
}

void Mesh::create(QVector<Vertex>& vertexes, QVector<GLuint>& indexes)
{
  if (material_) {
    calculateTBN(vertexes);
  }
  if (VBO_.isCreated()) { VBO_.destroy(); }
  VBO_.create();
  VBO_.bind();
  VBO_.allocate(vertexes.constData(), vertexes.size() * sizeof (Vertex));
  VBO_.release();

  if (EBO_.isCreated()) { EBO_.destroy();}
  EBO_.create();
  EBO_.bind();
  EBO_.allocate(indexes.constData(), indexes.size() * sizeof (GLuint));
  EBO_.release();
}

void Mesh::clear()
{
  VBO_.destroy();
  EBO_.destroy();
}

void Mesh::calculateTBN(QVector<Vertex>& vertexes)
{
//  qDebug() << "CalcTbn";
  if ( vertexes.size()%3 != 0 ) {
    qDebug() << QString("size vertexes %1").arg(vertexes.size());
    return;
  }
  for ( int i = 0; i < vertexes.size(); i += 3 ) {
    QVector3D& v1 = vertexes[i].position;
    QVector3D& v2 = vertexes[i+1].position;
    QVector3D& v3 = vertexes[i+2].position;

    QVector2D& uv1 = vertexes[i].texturePosition;
    QVector2D& uv2 = vertexes[i+1].texturePosition;
    QVector2D& uv3 = vertexes[i+2].texturePosition;

    QVector3D deltaPos1{v2 - v1};
    QVector3D deltaPos2{v3 - v1};

    QVector2D deltaUV1{uv2 - uv1};
    QVector2D deltaUV2{uv3 - uv1};

    float r = 1.0f/( deltaUV1.x() * deltaUV2.y() - deltaUV1.y() * deltaUV2.x() );
//    QVector3D tangent{ (deltaPos1 * deltaUV2.y() - deltaPos2 * deltaUV1.y()) * r };
//    QVector3D bitangent{ (deltaPos2 * deltaUV1.x() - deltaPos1 * deltaUV2.x()) * r };
    QVector3D tangent;
    tangent.setX((deltaUV2.y() * deltaPos1.x() - deltaUV1.y() * deltaPos2.x()) * r);
    tangent.setY((deltaUV2.y() * deltaPos1.y() - deltaUV1.y() * deltaPos2.y()) * r);
    tangent.setZ((deltaUV2.y() * deltaPos1.z() - deltaUV1.y() * deltaPos2.z()) * r);
    QVector3D bitangent;
    bitangent.setX((-deltaUV2.y() * deltaPos1.x() + deltaUV1.y() * deltaPos2.x()) * r);
    bitangent.setY((-deltaUV2.y() * deltaPos1.y() + deltaUV1.y() * deltaPos2.y()) * r);
    bitangent.setZ((-deltaUV2.y() * deltaPos1.z() + deltaUV1.y() * deltaPos2.z()) * r);

    vertexes[i].tangent = tangent;
    vertexes[i+1].tangent = tangent;
    vertexes[i+2].tangent = tangent;

    vertexes[i].bitangent = bitangent;
    vertexes[i+1].bitangent = bitangent;
    vertexes[i+2].bitangent = bitangent;
  }
}

