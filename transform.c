#include "transform.h"

#include "resource.h"

#include "gameObject.h"
#include "gameObject3D.h"

void initTransform2D(Transform2D* transform){

    transform->position.x = 0;
    transform->position.y = 0;

    transform->scale.x = 1;
    transform->scale.y = 1;

    transform->img.offset.x = 0;
    transform->img.offset.y = 0;

    transform->img.scale.x = 1;
    transform->img.scale.y = 1;
}

void initTransform3D(Transform3D* transform){

    transform->position.x = 0;
    transform->position.y = 0;
    transform->position.z = 0;

    transform->rotation.x = 0;
    transform->rotation.y = 0;
    transform->rotation.z = 0;

    transform->scale.x = 1;
    transform->scale.y = 1;
    transform->scale.z = 1;

    transform->img.offset.x = 0;
    transform->img.offset.y = 0;

    transform->img.scale.x = 1;
    transform->img.scale.y = 1;
}

void setPosTransform2D(struct GameObject2D* go, vec2 pos)
{

    GameObject2D *temp = (GameObject2D *)go;

    pos.x /= (viewSize.x * diffSize.x) * temp->transform.scale.x;
    pos.y /= (viewSize.y * diffSize.y) * temp->transform.scale.y;

    temp->transform.position.x = pos.x;
    temp->transform.position.y = pos.y;
}

void setPosTransform3D(struct GameObject3D* go, vec3 pos)
{

    GameObject3D *temp = (GameObject3D *)go;

    pos.x /= (viewSize.x * diffSize.x) * (temp->transform.scale.x / 100);
    pos.y /= (viewSize.y * diffSize.y) * (temp->transform.scale.y / 100);
    pos.z /= (viewSize.y * diffSize.y) * (temp->transform.scale.z / 100);

    temp->transform.position.x = pos.x;
    temp->transform.position.y = pos.y;
    temp->transform.position.z = pos.z;
}

vec2 getPosTransform2D(struct GameObject2D* go)
{
    GameObject2D *temp = (GameObject2D *)go;

    vec2 pos;
    pos.x = temp->transform.position.x * (viewSize.x * diffSize.x) * temp->transform.scale.x;
    pos.y = temp->transform.position.y * (viewSize.y * diffSize.y) * temp->transform.scale.y;
    return pos;
}

vec3 getPosTransform3D(struct GameObject3D* go)
{
    GameObject3D *temp = (GameObject3D *)go;

    vec3 pos;
    pos.x = temp->transform.position.x * (viewSize.x * diffSize.x) * temp->transform.scale.x;
    pos.y = temp->transform.position.y * (viewSize.y * diffSize.y) * temp->transform.scale.y;
    pos.z = temp->transform.position.z * (viewSize.y * diffSize.y) * temp->transform.scale.z;
    return pos;
}

void setOffsetTransform(struct GameObject2D* go, vec2 offset)
{
    GameObject2D *temp = (GameObject2D *)go;

    offset.x /= (viewSize.x * diffSize.x) * temp->transform.scale.x;
    offset.y /= (viewSize.y * diffSize.y) * temp->transform.scale.y;

    temp->transform.img.offset = offset;
}

vec2 getOffsetTransform(struct GameObject2D* go)
{
    GameObject2D *temp = (GameObject2D *)go;

    vec2 offset;
    offset.x = temp->transform.img.offset.x * (viewSize.x * diffSize.x) * temp->transform.scale.x;
    offset.y = temp->transform.img.offset.x * (viewSize.y * diffSize.y) * temp->transform.scale.y;

    return offset;
}

void setScaleTransform2D(struct GameObject2D* go, vec2 scale)
{
    GameObject2D *temp = (GameObject2D *)go;
    temp->transform.scale.x = scale.x / (viewSize.x * diffSize.x);
    temp->transform.scale.y = scale.y / (viewSize.y * diffSize.y);
}

void setScaleTransform3D(struct GameObject3D* go, vec3 scale)
{

    GameObject3D *temp = (GameObject3D *)go;
    temp->transform.scale.x = scale.x / (viewSize.x * diffSize.x);
    temp->transform.scale.y = scale.y / (viewSize.y * diffSize.y);
    temp->transform.scale.z = scale.z / (viewSize.y * diffSize.y);
}

vec2 getScaleTransform2D(struct GameObject2D* go)
{
    GameObject2D *temp = (GameObject2D *)go;

    vec2 size;
    size.x = temp->transform.scale.x * (viewSize.x * diffSize.x);
    size.y = temp->transform.scale.y * (viewSize.y * diffSize.y);
    return size;
}

vec3 getScaleTransform3D(struct GameObject3D* go)
{
    GameObject3D *temp = (GameObject3D *)go;
    vec3 size;
    size.x = temp->transform.scale.x * (viewSize.x * diffSize.x);
    size.y = temp->transform.scale.y * (viewSize.y * diffSize.y);
    size.z = temp->transform.scale.z * (viewSize.y * diffSize.y);
    return size;
}
