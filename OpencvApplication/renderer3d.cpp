
#include "renderer3d.h"
/*
void renderer3d::deleteChildrenRecursively(Qt3DCore::QNodeVector& vector)
{
    foreach(Qt3DCore::QNode* node, vector){
        Qt3DCore::QEntity* entity = (Qt3DCore::QEntity*)node;
        QList<Qt3DCore::QComponent*> componentsToDelete;
        foreach(Qt3DCore::QComponent* component, entity->components()){
            entity->removeComponent(component);
            delete(component);
            component = NULL;
        }
        auto n = node->childNodes();
        deleteChildrenRecursively(n);
        delete(node);
        node = NULL;
    }
}*/

void renderer3d::deleteChildrenRecursively(Qt3DCore::QNodeVector& vector)
{
    foreach(Qt3DCore::QNode* node, vector){
        Qt3DCore::QEntity* entity = (Qt3DCore::QEntity*)node;
        QList<Qt3DCore::QComponent*> componentsToDelete;
        foreach(Qt3DCore::QComponent* component, entity->components()){
            entity->removeComponent(component);
            delete(component);
            component = NULL;
        }
        auto n = node->childNodes();
        deleteChildrenRecursively(n);
        delete(node);
        node = NULL;
    }
}



void renderer3d::delete_entity(Qt3DCore::QNode *node){
    Qt3DCore::QEntity* entity = dynamic_cast<Qt3DCore::QEntity*>(node);
    if(entity == nullptr){
        return;
    }
    auto components_to_delete = entity->components();

    foreach(auto *component,components_to_delete){
        entity->removeComponent(component);
        delete(component);
        component = nullptr;
    }

    auto children_nodes = entity->childNodes();

    foreach(auto *child_node, children_nodes){
        delete_entity(child_node);
    }
    delete entity;
    entity = nullptr;
}

renderer3d::renderer3d(QObject *parent)
{
    view = new Qt3DExtras::Qt3DWindow;

    view -> defaultFrameGraph()->setClearColor(QColor(117, 120, 118));

    root = new Qt3DCore::QEntity;

    // Camera
    camera = view->camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(15.0f, 0, 5));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(root);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 190.0f );
    camController->setCamera(camera);

    renderScene();

    view->setRootEntity(root);
    view->show();


}

void renderer3d::renderTags(vector<Vec3d> rvecs, vector<Vec3d>tvecs)
{
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(root);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 100.0f );
    camController->setCamera(camera);

    //origin
    drawLine({ 0, 0, 0 }, { 1, 0, 0 }, Qt::red, root); // X
    drawLine({ 0, 0, 0 }, { 0, 1, 0 }, Qt::green, root); // Y
    drawLine({ 0, 0, 0 }, { 0, 0, 1 }, Qt::blue, root); // Z

    //grid
    float a = 5;
    for(int i = 0; i <= 10; i++)
    {
        drawLine({ a, 0, 5 }, { a, 0, -5 }, QColor(0,0,0), root);
        drawLine({ 5, 0, a }, { -5, 0,  a}, QColor(0,0,0), root);

         a-=1;
    }

    for (int i = 0, n = rvecs.size(); i < n; i++)
    {
        Vec3d t = tvecs[i] * 2;
        Vec3d r = rvecs[i];

        QQuaternion q =  QQuaternion::fromEulerAngles(QVector3D(0, 0, 3.14))*QQuaternion::fromEulerAngles(r[0], r[1],r[2]);
        QVector3D s = QVector3D(t[0],-t[1],t[2]);

        QVector3D xe = q * QVector3D( 1, 0, 0 );
        QVector3D ye = q * QVector3D( 0, 1, 0 );
        QVector3D ze = q * QVector3D( 0, 0, -1 );

        drawLine(s, s+xe, Qt::blue, root); // X
        drawLine(s, s+ye, Qt::green, root); // Y
        drawLine(s, s+ze, Qt::red, root); // Z

        drawLine(QVector3D(0,0,0), s, QColor(255, 145, 0), root);
    }
}

void renderer3d::renderScene()
{
    drawLine({ 0, 0, 0 }, { 1, 0, 0 }, Qt::red, root); // X
    drawLine({ 0, 0, 0 }, { 0, 1, 0 }, Qt::green, root); // Y
    drawLine({ 0, 0, 0 }, { 0, 0, 1 }, Qt::blue, root); // Z

    //grid
    float a = 5;
    for(int i = 0; i <= 10; i++)
    {
        drawLine({ a, 0, 5 }, { a, 0, -5 }, QColor(0,0,0), root);
        drawLine({ 5, 0, a }, { -5, 0,  a}, QColor(0,0,0), root);

         a-=1;
    }

}

void renderer3d::drawLine(const QVector3D& start, const QVector3D& end, const QColor& color, Qt3DCore::QEntity *_rootEntity)
{
    auto *geometry = new Qt3DRender::QGeometry(_rootEntity);

    // position vertices (start and end)
    QByteArray bufferBytes;
    bufferBytes.resize(3 * 2 * sizeof(float)); // start.x, start.y, start.end + end.x, end.y, end.z
    float *positions = reinterpret_cast<float*>(bufferBytes.data());
    *positions++ = start.x();
    *positions++ = start.y();
    *positions++ = start.z();
    *positions++ = end.x();
    *positions++ = end.y();
    *positions++ = end.z();

    auto *buf = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, geometry);
    buf->setData(bufferBytes);

    auto *positionAttribute = new Qt3DRender::QAttribute(geometry);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    positionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    positionAttribute->setVertexSize(3);
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(buf);
    positionAttribute->setByteStride(3 * sizeof(float));
    positionAttribute->setCount(2);
    geometry->addAttribute(positionAttribute); // We add the vertices in the geometry

    // connectivity between vertices
    QByteArray indexBytes;
    indexBytes.resize(2 * sizeof(unsigned int)); // start to end
    unsigned int *indices = reinterpret_cast<unsigned int*>(indexBytes.data());
    *indices++ = 0;
    *indices++ = 1;


    auto *indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, geometry);
    indexBuffer->setData(indexBytes);

    auto *indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(2);
    geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

    // mesh
    auto *line = new Qt3DRender::QGeometryRenderer(_rootEntity);
    line->setGeometry(geometry);
    line->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    auto *material = new Qt3DExtras::QPhongMaterial(_rootEntity);
    material->setAmbient(color);

    // entity
    auto *lineEntity = new Qt3DCore::QEntity(_rootEntity);
    lineEntity->addComponent(line);
    lineEntity->addComponent(material);
}
