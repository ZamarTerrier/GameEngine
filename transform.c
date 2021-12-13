#include "transform.h"

#include "resource.h"


void initTransform(Transform* transform){

    transform->pos.x = 0;
    transform->pos.y = 0;

    transform->scale.x = 1;
    transform->scale.y = 1;

    transform->img.offset.x = 0;
    transform->img.offset.y = 0;

    transform->img.scale.x = 1;
    transform->img.scale.y = 1;
}

void setPosTransform(Transform* transform, vec2 pos)
{
    pos.x /= (viewSize.x * diffSize.x) * transform->scale.x ;
    pos.y /= (viewSize.y * diffSize.y) * transform->scale.y;

    transform->pos= pos;
}

vec2 getPosTransform(Transform* transform)
{
    vec2 pos;
    pos.x = transform->pos.x * (viewSize.x * diffSize.x) * transform->scale.x;
    pos.y = transform->pos.y * (viewSize.y * diffSize.y) * transform->scale.y;
    return pos;
}

void setScaleTransform(Transform* transform, vec2 scale)
{
    transform->scale.x = scale.x / (viewSize.x * diffSize.x);
    transform->scale.y = scale.y / (viewSize.y * diffSize.y);
}

vec2 getScaleTransform(Transform* transform)
{
    vec2 size;
    size.x = transform->scale.x * (viewSize.x * diffSize.x);
    size.y = transform->scale.y * (viewSize.y * diffSize.y);
    return size;
}
