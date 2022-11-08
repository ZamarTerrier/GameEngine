#include "transform.h"

#include "e_resource.h"

#include "e_math.h"

#include "gameObject2D.h"
#include "gameObject3D.h"

void Transform2DInit(Transform2D* transform){

    transform->position.x = transform->positionOrig.x = 0;
    transform->position.y = transform->positionOrig.y = 0;

    transform->scale.x = 1;
    transform->scale.y = 1;

    transform->scaleOrig.x = WIDTH;
    transform->scaleOrig.y = HEIGHT;

    transform->img.origin.x = 0;
    transform->img.origin.y = 0;

    transform->img.offset.x = 0;
    transform->img.offset.y = 0;

    transform->img.scale.x = 1;
    transform->img.scale.y = 1;
}

void Transform3DInit(Transform3D* transform){

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

    temp->transform.position.x = x;
    temp->transform.position.y = y;
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
    temp->transform.position.x = temp->transform.positionOrig.x / (WIDTH);
    temp->transform.position.y = temp->transform.positionOrig.y / (HEIGHT);
}

vec2 Transform2DGetPosition(struct GameObject2D* go)
{
    GameObject2D *temp = (GameObject2D *)go;

    vec2 pos = temp->transform.position;

    pos.x *= WIDTH;
    pos.y *= HEIGHT;

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

    temp->transform.scale.x = x;
    temp->transform.scale.y = y;
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
    temp->transform.scale.x = temp->transform.scaleOrig.x / (WIDTH);
    temp->transform.scale.y = temp->transform.scaleOrig.y / (HEIGHT);
}

vec2 Transform2DGetScale(struct GameObject2D* go)
{
    GameObject2D *temp = (GameObject2D *)go;

    vec2 size;
    size.x = temp->transform.scale.x * (WIDTH);
    size.y = temp->transform.scale.y * (HEIGHT);

    return size;
}

//3D Transforms
void Transform3DSetPositionT(Transform3D* transform, vec3 pos)
{

    transform->position = pos;
}

void Transform3DSetPosition(struct GameObject3D* go, vec3 pos)
{

    GameObject3D *temp = (GameObject3D *)go;

    temp->transform.position = pos;
}

vec3 Transform3DGetPosition(struct GameObject3D* go)
{
    GameObject3D *temp = (GameObject3D *)go;

    return temp->transform.position;
}

void Transform3DSetScaleT(Transform3D* transform, vec3 scale)
{

    transform->scale = scale;
}

void Transform3DSetScale(struct GameObject3D* go, vec3 scale)
{

    GameObject3D *temp = (GameObject3D *)go;

    temp->transform.scale = scale;
}

vec3 Transform3DGetScale(struct GameObject3D* go)
{
    GameObject3D *temp = (GameObject3D *)go;
    return temp->transform.scale;
}

void Transform3DSetRotateT(Transform3D* transform, vec3 rotate)
{

    transform->rotation = rotate;
}

void Transform3DSetRotate(struct GameObject3D* go, vec3 rotate){

    GameObject3D *temp = (GameObject3D *)go;

    /*rotate.x = rotate.x > 360 ? 0 : rotate.x;
    rotate.x = rotate.x < -360 ? 0 : rotate.x;

    rotate.y = rotate.y > 360 ? 0 : rotate.y;
    rotate.y = rotate.y < -360 ? 0 : rotate.y;

    rotate.z = rotate.z > 360 ? 0 : rotate.z;
    rotate.z = rotate.z < -360 ? 0 : rotate.z;*/

    temp->transform.rotation = rotate;

}

vec3 Transform3DGetRotate(struct GameObject3D* go){

    GameObject3D *temp = (GameObject3D *)go;

    return temp->transform.rotation;
}
