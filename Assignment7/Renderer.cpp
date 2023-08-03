//
// Created by goksu on 2/25/20.
//

#include <fstream>
#include "Scene.hpp"
#include "Renderer.hpp"

#include <thread>
#include <stdlib.h>
#include <pthread.h>
#include <omp.h>
// linux library for getting cpu num
#include "unistd.h"
#include <chrono>
#include <string>
using std::string;

extern std::chrono::_V2::system_clock::time_point start;


inline float deg2rad(const float &deg) { return deg * M_PI / 180.0; }

const float EPSILON = 0.00001;

// The main render function. This where we iterate over all pixels in the image,
// generate primary rays and cast these rays into the scene. The content of the
// framebuffer is saved to a file.
void Renderer::Render(const Scene &scene,const int spp)
{
    std::vector<Vector3f> framebuffer(scene.width * scene.height);

    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(278, 273, -800);
    bool IsUseMultiThread = true;
    string filename = "binary";
    filename+="_"+std::to_string(spp);
    if (!IsUseMultiThread)
    {
        int m = 0;

        // change the spp value to change sample ammount

        std::cout << "SPP: " << spp << "\n";
        for (uint32_t j = 0; j < scene.height; ++j)
        {
            for (uint32_t i = 0; i < scene.width; ++i)
            {
                // generate primary ray direction
                float x = (2 * (i + 0.5) / (float)scene.width - 1) *
                          imageAspectRatio * scale;
                float y = (1 - 2 * (j + 0.5) / (float)scene.height) * scale;

                Vector3f dir = normalize(Vector3f(-x, y, 1));
                for (int k = 0; k < spp; k++)
                {
                    framebuffer[m] += scene.castRay(Ray(eye_pos, dir), 0) / spp;
                }
                m++;
            }
            UpdateProgress(j / (float)scene.height);
        }
        UpdateProgress(1.f);
        filename += "_WithOut_" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count());
    }
    else
    {
        int m = 0;

        // change the spp value to change sample ammount

        std::cout << "SPP: " << spp << "\n";
        int cpuNum = sysconf(_SC_NPROCESSORS_CONF);
        std::cout << "Cpu Num :" << cpuNum << std::endl;
        omp_set_num_threads(cpuNum);
        // int handle[cpuNum];
        float minY = 0.0f, maxY = 0.0f;
        
        int hxw = scene.width * scene.height;
        long long pocess =0;
        #pragma omp parallel for shared(pocess)
        for (uint32_t p = 0; p < hxw; ++p)
        {
            int i = p % scene.height;
            int j = p / scene.height;
            // generate primary ray direction
            float x = (2 * (i + 0.5) / (float)scene.width - 1) *
                      imageAspectRatio * scale;
            float y = (1 - 2 * (j + 0.5) / (float)scene.height) * scale;

            Vector3f dir = normalize(Vector3f(-x, y, 1));
            for (int k = 0; k < spp; k++)
            {
                framebuffer[p] += scene.castRay(Ray(eye_pos, dir), 0) / spp;
            }
            pocess++;
            #pragma omp critical
            UpdateProgress(pocess / (float)(scene.height * scene.width));
        }
        minY = maxY + 1.0f;
        UpdateProgress(1.f);
        filename += "_With_" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count());
    }
    filename += ".ppm";
    // save framebuffer to file
    FILE *fp = fopen(filename.c_str(), "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    for (auto i = 0; i < scene.height * scene.width; ++i)
    {
        static unsigned char color[3];
        color[0] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].x), 0.6f));
        color[1] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].y), 0.6f));
        color[2] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].z), 0.6f));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);
}
