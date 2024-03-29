#include "e_resource_shapes.h"

const Vertex3D lineVert[] = {
    {{0, 0.0f, 0}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
};

const Vertex3D triVert[] = {
    {{-1.0f, -1.0f, 0}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, -1.0f, 0}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
    {{0.0f, 1.0f, 0}, {0.0f,-1.0f, 0.0f}, {1.0f, 1.0f}},
};

const Vertex3D quadVert[] = {
    {{-1.0f, 1.0f, 0}, {-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, 1.0f, 0}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.0f, -1.0f, 0}, {1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
    {{-1.0f, -1.0f, 0}, {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
};

const Vertex3D cubeVert[] = {
    //Front
    {{-1.0f, -1.0f, 1.0f}, {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
    {{-1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
    //Back
    {{1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{-1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{-1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
    {{1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
    //Left
    {{-1.0f, -1.0f, 1.0}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{-1.0f, -1.0f, -1.0}, {1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
    {{-1.0f, 1.0f, -1.0}, {-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
    {{-1.0f, 1.0f, 1.0}, {-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    //Right
    {{1.0f, 1.0f, 1.0}, {1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
    {{1.0f, 1.0f, -1.0}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
    {{1.0f, -1.0f, -1.0}, {-1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{1.0f, -1.0f, 1.0}, {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
    //Top
    {{1.0f, 1.0f, 1.0}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
    {{-1.0f, 1.0f, 1.0}, {-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{-1.0f, 1.0f, -1.0}, {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, 1.0f, -1.0}, {1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
    //Down
    {{-1.0f, -1.0f, -1.0}, {-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
    {{-1.0f, -1.0f, 1.0}, {-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{1.0f, -1.0f, 1.0}, {1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
    {{1.0f, -1.0f, -1.0}, {1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
};

const Vertex2D planeVert[] = {
    {{-0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
    {{0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
};

const Vertex2D projPlaneVert[] = {
    {{-1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{-1.0f,  1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{ 1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{ 1.0f, -1.0f}, {0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
};

const uint32_t triIndx[] = {
    0, 1, 2
};

const uint32_t quadIndx[] = {
    0, 1, 2, 2, 3, 0
};

const uint32_t cubeIndx[] = {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4,
    8, 9, 10, 10, 11, 8,//
    12, 13, 14, 14, 15, 12,
    16, 17, 18, 18, 19, 16,
    20, 21, 22, 22, 23, 20//
};

const uint32_t planeIndx[] = {
    0, 1, 2, 2, 3, 0
};

const uint32_t projPlaneIndx[] = {
    0, 1, 2, 2, 3, 0
};
