#include "transform.h"

#include "e_math.h"

#include "gameObject2D.h"
#include "gameObject3D.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"

void Transform2DInit(Transform2D* transform){

    memset(transform, 0, sizeof(Transform2D));

    transform->scale.x = 1;
    transform->scale.y = 1;

    transform->scaleOrig.x = WIDTH;
    transform->scaleOrig.y = HEIGHT;

    transform->img.scale.x = 1;
    transform->img.scale.y = 1;
}

void Transform3DInit(Transform3D* transform){

    memset(transform, 0, sizeof(Transform3D));

    transform->scale.x = 1;
    transform->scale.y = 1;
    transform->scale.z = 1;

    transform->img.scale.x = 1;
    transform->img.scale.y = 1;
}

//2D Transforms
void Transform2DSetPosition(struct GameObject2D* go, float x, float y)
{

    GameObject2D *temp = (GameObject2D *)go;

    temp->transform.positionOrig.x = x;
    temp->transform.positionOrig.y = y;

    if(x != 0 )
        x /= WIDTH;

    if(y != 0)
        y /= HEIGHT;

    temp->transform.position.x = x / 2;
    temp->transform.position.y = y / 2;
}

void Transform2DSetImageOrigin(struct GameObject2D* go, float x, float y)
{
    GameObject2D *temp = (GameObject2D *)go;

    temp->transform.img.origin.x = x / temp->image->imgWidth;
    temp->transform.img.origin.y = y / temp->image->imgHeight;
}

vec2 Transform2DGetImageOrigin(struct GameObject2D* go)
{
    GameObject2D *temp = (GameObject2D *)go;

    vec2 origin;
    origin.x = temp->image->imgWidth * temp->transform.img.origin.x;
    origin.y = temp->image->imgHeight * temp->transform.img.origin.y;

    return origin;
}

void Transform2DReposition(struct GameObject2D* go)
{
    GameObject2D *temp = (GameObject2D *)go;
    temp->transform.position.x = temp->transform.positionOrig.x / (WIDTH) / 2;
    temp->transform.position.y = temp->transform.positionOrig.y / (HEIGHT) / 2;
}

vec2 Transform2DGetPosition(struct GameObject2D* go)
{
    GameObject2D *temp = (GameObject2D *)go;

    vec2 pos = temp->transform.position;

    pos.x *= WIDTH * 2;
    pos.y *= HEIGHT * 2;

    return pos;
}


void Transform2DSetImageOffset(struct GameObject2D* go, float x, float y)
{
    GameObject2D *temp = (GameObject2D *)go;

    temp->transform.img.offset.x = x / temp->image->imgWidth;
    temp->transform.img.offset.y = y / temp->image->imgHeight;
}

vec2 Transform2DGetImageOffset(struct GameObject2D* go)
{
    GameObject2D *temp = (GameObject2D *)go;

    vec2 offset;
    offset.x = temp->image->imgWidth * temp->transform.img.offset.x;
    offset.y = temp->image->imgHeight * temp->transform.img.offset.y;

    return offset;
}

void Transform2DSetScale(struct GameObject2D* go, float x, float y)
{
    GameObject2D *temp = (GameObject2D *)go;
    temp->transform.scaleOrig.x = x;
    temp->transform.scaleOrig.y = y;

    x /= WIDTH;
    y /= HEIGHT;

    temp->transform.scale.x = x / 2;
    temp->transform.scale.y = y / 2;
}

void Transform2DSetImageScale(struct GameObject2D* go, float x, float y)
{
    GameObject2D *temp = (GameObject2D *)go;

    temp->transform.img.scale.x = x / temp->image->imgWidth;
    temp->transform.img.scale.y = y / temp->image->imgHeight;
}

vec2 Transform2DGetImageScale(struct GameObject2D* go)
{
    GameObject2D *temp = (GameObject2D *)go;

    vec2 scale;
    scale.x = temp->image->imgWidth * temp->transform.img.scale.x;
    scale.y = temp->image->imgHeight * temp->transform.img.scale.y;

    return scale;
}

void Transform2DRescale(struct GameObject2D* go)
{
    GameObject2D *temp = (GameObject2D *)go;
    temp->transform.scale.x = temp->transform.scaleOrig.x / (WIDTH) / 2;
    temp->transform.scale.y = temp->transform.scaleOrig.y / (HEIGHT) / 2;
}

vec2 Transform2DGetScale(struct GameObject2D* go)
{
    GameObject2D *temp = (GameObject2D *)go;

    vec2 size;
    size.x = temp->transform.scale.x * (WIDTH) * 2;
    size.y = temp->transform.scale.y * (HEIGHT) * 2;

    return size;
}

//3D Transforms
void Transform3DSetPositionT(Transform3D* transform, float x, float y, float z)
{
    transform->position.x = x;
    transform->position.y = y;
    transform->position.z = z;
}

void Transform3DSetPosition(struct GameObject3D* go, float x, float y, float z)
{
    GameObject3D *temp = (GameObject3D *)go;

    temp->transform.position.x = x;
    temp->transform.position.y = y;
    temp->transform.position.z = z;
}

vec3 Transform3DGetPosition(struct GameObject3D* go)
{
    GameObject3D *temp = (GameObject3D *)go;

    return temp->transform.position;
}

void Transform3DSetScaleT(Transform3D* transform, float x, float y, float z)
{
    transform->scale.x = x;
    transform->scale.y = y;
    transform->scale.z = z;
}

void Transform3DSetScale(struct GameObject3D* go, float x, float y, float z)
{
    GameObject3D *temp = (GameObject3D *)go;

    temp->transform.scale.x = x;
    temp->transform.scale.y = y;
    temp->transform.scale.z = z;
}

vec3 Transform3DGetScale(struct GameObject3D* go)
{
    GameObject3D *temp = (GameObject3D *)go;
    return temp->transform.scale;
}

void Transform3DSetRotateT(Transform3D* transform, float x, float y, float z)
{
    transform->rotation.x = x;
    transform->rotation.y = y;
    transform->rotation.z = z;
}

void Transform3DSetRotate(struct GameObject3D* go, float x, float y, float z){

    GameObject3D *temp = (GameObject3D *)go;

    x = x > 360 ? 0 : x;
    x = x < -360 ? 0 : x;

    y = y > 360 ? 0 : y;
    y = y < -360 ? 0 : y;

    z = z > 360 ? 0 : z;
    z = z < -360 ? 0 : z;

    temp->transform.rotation.x = x;
    temp->transform.rotation.y = y;
    temp->transform.rotation.z = z;
}

vec3 Transform3DGetRotate(struct GameObject3D* go){

    GameObject3D *temp = (GameObject3D *)go;

    return temp->transform.rotation;
}
