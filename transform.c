#include "transform.h"

#include "e_resource.h"

#include "e_math.h"

#include "gameObject2D.h"
#include "gameObject3D.h"

void Transform2DInit(Transform2D* transform){

    transform->position.x = 0;
    transform->position.y = 0;

    transform->scale.x = 1;
    transform->scale.y = 1;

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

void Transform2DSetPosition(struct GameObject2D* go, vec2 pos)
{

    GameObject2D *temp = (GameObject2D *)go;


    temp->transform.positionOrig.x = pos.x;
    temp->transform.positionOrig.y = pos.y;

    if(pos.x != 0 )
        pos.x /= WIDTH;

    if(pos.y != 0)
        pos.y /= HEIGHT;

    temp->transform.position.x = pos.x;
    temp->transform.position.y = pos.y;
}

void Transform2DReposition(struct GameObject2D* go)
{
    GameObject2D *temp = (GameObject2D *)go;
    temp->transform.position.x =  temp->transform.positionOrig.x / (WIDTH);
    temp->transform.position.y =  temp->transform.positionOrig.y / (HEIGHT);
}

void Transform3DSetPositionT(Transform3D* transform, vec3 pos)
{

    transform->position = pos;
}

void Transform3DSetPosition(struct GameObject3D* go, vec3 pos)
{

    GameObject3D *temp = (GameObject3D *)go;

    temp->transform.position = pos;
}

vec2 Transform2DGetPosition(struct GameObject2D* go)
{
    GameObject2D *temp = (GameObject2D *)go;

    vec2 pos = temp->transform.position;

    pos.x *= WIDTH;
    pos.y *= HEIGHT;

    return pos;
}

vec3 Transform3DGetPosition(struct GameObject3D* go)
{
    GameObject3D *temp = (GameObject3D *)go;

    return temp->transform.position;
}

void Transform2DSetOffset(struct GameObject2D* go, vec2 offset)
{
    GameObject2D *temp = (GameObject2D *)go;

    offset.x /= (viewSize.x * diffSize.x) * temp->transform.scale.x;
    offset.y /= (viewSize.y * diffSize.y) * temp->transform.scale.y;

    temp->transform.img.offset = offset;
}

vec2 Transform2DGetOffset(struct GameObject2D* go)
{
    GameObject2D *temp = (GameObject2D *)go;

    vec2 offset;
    offset.x = temp->transform.img.offset.x * (viewSize.x * diffSize.x) * temp->transform.scale.x;
    offset.y = temp->transform.img.offset.x * (viewSize.y * diffSize.y) * temp->transform.scale.y;

    return offset;
}

void Transform2DSetScale(struct GameObject2D* go, vec2 scale)
{
    GameObject2D *temp = (GameObject2D *)go;
    temp->transform.scaleOrig.x =  scale.x;
    temp->transform.scaleOrig.y =  scale.y;

    scale.x /= WIDTH;
    scale.y /= HEIGHT;

    temp->transform.scale.x =  scale.x;
    temp->transform.scale.y =  scale.y;
}

void Transform2DRescale(struct GameObject2D* go)
{
    GameObject2D *temp = (GameObject2D *)go;
    temp->transform.scale.x =  temp->transform.scaleOrig.x / (WIDTH);
    temp->transform.scale.y =  temp->transform.scaleOrig.y / (HEIGHT);
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

vec2 Transform2DGetScale(struct GameObject2D* go)
{
    GameObject2D *temp = (GameObject2D *)go;

    vec2 size;
    size.x = temp->transform.scale.x * (WIDTH);
    size.y = temp->transform.scale.y * (HEIGHT);

    return size;
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
