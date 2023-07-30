#include "Renderer.hpp"
#include "Scene.hpp"
#include "Triangle.hpp"
#include "Vector.hpp"
#include "global.hpp"
#include <chrono>

// In the main function of the program, we create the scene (create objects and
// lights) as well as set the options for the render (image width and height,
// maximum recursion depth, field-of-view, etc.). We then call the render
// function().
int main(int argc, char** argv)
{
    BVHAccel::SplitMethod mode=BVHAccel::SplitMethod::BVH;
    if (argc == 2)
    {
        if (std::string(argv[1]) == "BVH")
            std::cout << "Build with BVH" << std::endl;
        if (std::string(argv[1]) == "SAH")
            mode = BVHAccel::SplitMethod::SAH, std::cout << "Build with SAH" << std::endl;
    }
    else
    {
        std::cout << "Build with BVH" << std::endl;
    }
    if(argc>=2)
        std::cout << "argv[1]" << argv[1] << std::endl;
    Scene scene(1280, 960);

    MeshTriangle bunny("../models/bunny/bunny.obj",mode);

    scene.Add(&bunny);
    scene.Add(std::make_unique<Light>(Vector3f(-20, 70, 20), 1));
    scene.Add(std::make_unique<Light>(Vector3f(20, 70, 20), 1));


    scene.buildBVH(mode);
    std::cout<<"After build"<<std::endl;
    Renderer r;
    std::string fileroad;
    if(mode==BVHAccel::SplitMethod::BVH){
        fileroad="./imag_BVH/";
    }
    else if(mode==BVHAccel::SplitMethod::SAH){
        fileroad="./imag_SAH/";
    }
    auto start = std::chrono::system_clock::now();
    r.Render(scene,fileroad);
    auto stop = std::chrono::system_clock::now();
    std::cout<<"After Render"<<std::endl;
    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>(stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";

    return 0;
}