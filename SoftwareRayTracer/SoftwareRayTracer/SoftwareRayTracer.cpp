#define _USE_MATH_DEFINES 
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

//#include <SDL.h>
#include <Windows.h>
#undef main

#include <../glm/glm.hpp>

#include "obj.h"

#define PIXEL_W 1920
#define PIXEL_H 1080
float pixelBuffer[PIXEL_W * PIXEL_H * 3];

glm::vec3 bkgd = glm::vec3(.6f, .6f, .6f);

//vector<Object> objs;
std::vector<triangle> tris;


float vfov = tan(glm::radians(90.f / 2.f));
float aspect = (float)PIXEL_W / (float)PIXEL_H;

glm::vec3 light_pos(4.f, 6.f, 4.f);
int max_recursion_depth = 1;
const float atten = 1.f;


glm::vec3 eye = glm::vec3(0.f, 2.5f, 3.0f);
const std::string MODEL_PATH = "objs/cornell2/cornell-box.obj";
//const std::string MODEL_PATH = "objs/bird/textured_quad.obj";
//glm::vec3 eye = glm::vec3(0.f, 0.f, 2.5f);


#include "bmp.h"
#include "verts.h"
#include "utility.h"
#include "solution.h"





int main()
{
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);

    LARGE_INTEGER Render_StartingTime, Render_EndingTime;
    QueryPerformanceCounter(&Render_StartingTime);


    obj_parse(MODEL_PATH.c_str(), &tris, 1.f);
 //   tris = AssemblePrimitives(verts, n_verts);
//    AppendTriangles(&tris, objs);





    rayTraceImage();

    savebitmap("render.bmp", pixelBuffer, PIXEL_W, PIXEL_H);

    CounterEndAndPrint(Render_StartingTime, &Render_EndingTime, Frequency);
}
