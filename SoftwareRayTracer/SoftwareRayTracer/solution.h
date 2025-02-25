#pragma once

typedef glm::vec3(*closest_hit)(triangle* tri, int depth, glm::vec3 p, glm::vec3 dir); // type for conciseness
void trace(glm::vec3 o, glm::vec3 dir, float& t, glm::vec3& io_col, int depth, triangle* from_tri, closest_hit p_hit);

glm::vec3 Diffuse(glm::vec3 col, glm::vec3 lightDir, glm::vec3 normal)
{
    float dotNL = std::max(dot(normal, lightDir), 0.f);
    glm::vec3 ret = col * dotNL;
    return ret;
}

glm::vec3 DoNothing(triangle* tri, int depth, glm::vec3 p, glm::vec3 dir)
{
    return glm::vec3(0.f);
}


glm::vec3 Shade(triangle* tri, int depth, glm::vec3 p, glm::vec3 dir)
{
    glm::vec3 ret = tri->v1.col * .1f;
    float t = -1;


    //shadow
    glm::vec3 dummy;
    glm::vec3 lightDir = glm::normalize(light_pos - p);
    trace(p, lightDir, t, dummy, 0, tri, DoNothing);
    if (t < 0)
    {
        ret += Diffuse(tri->v1.col, lightDir, tri->v1.nor);
    }


    //reflection
    if (tri->reflect && depth < max_recursion_depth)
    {
        glm::vec3 refl_col(0.f);

        glm::vec3 refl = glm::reflect(dir, tri->v1.nor);

        trace(p, refl, t, refl_col, depth + 1, tri, Shade);
        ret += (refl_col);// *atten);
    }

//    glm::vec3 ret = ;
    //    glm::vec3 ret = tri->v1.col + (refl_col * atten);

    return ret;
}


bool PointInTriangle(glm::vec3 pt, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
{
    glm::vec3 V12 = v2 - v1;
    glm::vec3 V13 = v3 - v1;
    glm::vec3 V1p = pt - v1;
    glm::vec3 C11 = cross(V12, V1p);
    glm::vec3 C12 = cross(V12, V13);
    float d1 = dot(C11, C12);

    glm::vec3 V23 = v3 - v2;
    glm::vec3 V21 = v1 - v2;
    glm::vec3 V2p = pt - v2;
    glm::vec3 C21 = cross(V23, V2p);
    glm::vec3 C22 = cross(V23, V21);
    float d2 = dot(C21, C22);

    glm::vec3 V31 = v1 - v3;
    glm::vec3 V32 = v2 - v3;
    glm::vec3 V3p = pt - v3;
    glm::vec3 C31 = cross(V31, V3p);
    glm::vec3 C32 = cross(V31, V32);
    float d3 = dot(C31, C32);

    if (d1 > 0 && d2 > 0 && d3 > 0)
        return true;

    return false;
}

float RayTriangleIntersection(glm::vec3 o, glm::vec3 dir, triangle* tri, glm::vec3& point)
{
    //on triangle plane?
    glm::vec3 V1 = tri->v2.pos - tri->v1.pos;
    glm::vec3 V2 = tri->v3.pos - tri->v1.pos;
    glm::vec3 n = cross(V2, V1);
    glm::vec3 N = normalize(n);

    glm::vec3 p = tri->v1.pos - o;
    float d1 = dot(p, N);
    float d2 = dot(dir, N);
    if (d2 == 0.f)
    {
        return FLT_MAX;
    }
    float t1 = d1 / d2;

    if (t1 < 0.001f)
        return FLT_MAX;

    glm::vec3 V3 = dir * t1;
    glm::vec3 pop = o + V3;


    //inside triangle?
    float alpha, beta, gamma;
    if (PointInTriangle(pop, tri->v1.pos, tri->v2.pos, tri->v3.pos))
    {
        point = pop;
        return t1;
    }

    return FLT_MAX;
}

void trace(glm::vec3 o, glm::vec3 dir, float& t, glm::vec3& io_col, int depth, triangle* from_tri, closest_hit p_hit)
{
    float closest_t = FLT_MAX;
    int closest_tc = -1;
    glm::vec3 closest_p;
    for (int tc = 0; tc < tris.size(); tc++)
    {
        triangle* tri = &tris[tc];

       // if (from_tri != NULL && tri == from_tri)
        //    continue;

        glm::vec3 p = glm::vec3(0);
        float current_t = RayTriangleIntersection(o, dir, tri, p);

        if (current_t > 0)//  && current_t < f)
        {
            if (current_t < closest_t)
            {
                closest_t = current_t;
                closest_tc = tc;
                closest_p = p;
            }
        }
    }

    if (closest_tc >= 0)
    {
        t = closest_t;
        triangle* closest_tri = &tris[closest_tc];
        io_col = p_hit(closest_tri, depth, closest_p, dir);
        return;
    }

    io_col = bkgd;
    return;
}

vec3 GetRayDirection(float px, float py, int W, int H, float aspect_ratio, float fov)
{
    vec3 X = aspect_ratio * fov * (((2 * (px + .5f)) / W) - 1) * vec3(1, 0, 0);
    vec3 Y = fov * (((2 * (py + .5f)) / H) - 1) * vec3(0, 1, 0);
    vec3 Z = vec3(0, 0, -1);
    return X + Y + Z;
}

void rayTraceImage()
{
    for (int pixel_y = 0; pixel_y < PIXEL_H; ++pixel_y)
    {
        float percf = (float)pixel_y / (float)PIXEL_H;
        int perci = percf * 100;
        std::clog << "\rScanlines done: " << perci << "%" << ' ' << std::flush;

        for (int pixel_x = 0; pixel_x < PIXEL_W; ++pixel_x)
        {
            glm::vec3 pp = GetRayDirection(pixel_x, pixel_y, PIXEL_W, PIXEL_H, aspect, vfov);
            glm::vec3 dir = normalize(pp);
            dir.y = -dir.y;
            //dir.z = -dir.z;

            float t = -1;
            glm::vec3 col(0.f);

            trace(eye, dir, t, col, 0, NULL, Shade);

            writeCol(col, pixel_x, pixel_y);
        }
    }
    std::clog << "\rFinish rendering.           \n";

}
