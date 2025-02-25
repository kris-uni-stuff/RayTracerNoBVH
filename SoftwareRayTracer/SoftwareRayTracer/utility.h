#pragma once

void CounterEndAndPrint(LARGE_INTEGER StartingTime, LARGE_INTEGER* EndingTime, LARGE_INTEGER Frequency)
{
    QueryPerformanceCounter(EndingTime);

    LARGE_INTEGER ElapsedMicroseconds;
    ElapsedMicroseconds.QuadPart = (*EndingTime).QuadPart - StartingTime.QuadPart;
    ElapsedMicroseconds.QuadPart *= 1000000;
    ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;
    std::cout << (float)ElapsedMicroseconds.QuadPart / (float)1000000 << std::endl;
}

void writeCol(vec3 col, int pixel_x, int pixel_y)
{
    auto r = linear_to_gamma(col.x);
    auto g = linear_to_gamma(col.y);
    auto b = linear_to_gamma(col.z);


    float rc = std::clamp(r, .0f, 1.f);
    float gc = std::clamp(g, .0f, 1.f);
    float bc = std::clamp(b, .0f, 1.f);

    float pixel_r = rc * 255.f;
    float pixel_g = gc * 255.f;
    float pixel_b = bc * 255.f;

    pixelBuffer[(pixel_y * PIXEL_W * 3) + (pixel_x * 3) + 0] = pixel_r;
    pixelBuffer[(pixel_y * PIXEL_W * 3) + (pixel_x * 3) + 1] = pixel_g;
    pixelBuffer[(pixel_y * PIXEL_W * 3) + (pixel_x * 3) + 2] = pixel_b;

}
