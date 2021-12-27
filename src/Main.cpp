#include"Window.h"
#include"Buffer.h"
#include"Camera.h"
#include"Marching.h"

#include<GLFW/glfw3.h>
#include<ctpl_stl.h>

#include<cstring>
#include<cmath>
#include<chrono>
#include<vector>
#include<iostream>

static const int WIDTH = 800;
static const int HEIGHT = 700;

float Floor(float x)
{
    return static_cast<float>(static_cast<int64_t>(x)) - static_cast<float>(static_cast<int>(x < 0.0f));
}

float Mod(float a, float b)
{
    return a - Floor(a / b) * b;
}

int main()
{
    Window window(WIDTH, HEIGHT, "Ray Marching");

    Buffer buffer(WIDTH, HEIGHT);

    Camera camera;
    camera.x = 0.0f;
    camera.y = 2.0f;
    camera.z = 0.0f;
    camera.pitch = 0.0f;
    camera.yaw = 0.0f;
    camera.fov = 1.57f;

    float vx = 0.0f, vz = 0.0f;

    window.SetKeyCallback([&](int key, int action)
    {
        switch (key)
        {
            case GLFW_KEY_LEFT:
                vx = action == GLFW_RELEASE? 0.0f : -1.0f;
                break;
        
            case GLFW_KEY_RIGHT:
                vx = action == GLFW_RELEASE? 0.0f : 1.0f;
                break;
        
            case GLFW_KEY_DOWN:
                vz = action == GLFW_RELEASE? 0.0f : -1.0f;
                break;
        
            case GLFW_KEY_UP:
                vz = action == GLFW_RELEASE? 0.0f : 1.0f;
                break;
        }
    });

    // Sky dir
    float sky_dir_x = 0.2f;
    float sky_dir_y = -0.8f;
    float sky_dir_z = 0.3f;
    float sky_dir_length = sqrt(sky_dir_x * sky_dir_x + sky_dir_y * sky_dir_y + sky_dir_z * sky_dir_z);
    sky_dir_x /= sky_dir_length;
    sky_dir_y /= sky_dir_length;
    sky_dir_z /= sky_dir_length;

    // Sphere
    auto sphere_dist = [](float x, float y, float z) -> float
    {
        x = Mod(x + 1.5f, 3.0f) - 1.5f; z = Mod(z + 1.5f, 3.0f) - 1.5f;
        return sqrtf(x * x + y * y + z * z) - 1.0f;
    };

    auto sphere_normal = [](float x, float y, float z, float dist) -> std::tuple<float, float, float>
    {
        x = Mod(x + 1.5f, 3.0f) - 1.5f; z = Mod(z + 1.5f, 3.0f) - 1.5f;
        float d = dist + 1.0f;
        return { x / d, y / d, z / d };
    };

    // Plane
    auto plane_dist = [](float x, float y, float z) -> float
    {
        float nx = 0.0f, ny = 1.0f, nz = 0.0f;
        float px = 0.0f, py = -1.0f, pz = 0.0f;

        return nx * (x - px) + ny * (y - py) + nz * (z - pz);
    };

    auto plane_normal = [](float x, float y, float z) -> std::tuple<float, float, float>
    {
        float nx = 0.0f, ny = 1.0f, nz = 0.0f;
        return { nx, ny, nz };
    };

    // Thread pool
    unsigned int num_workers = std::thread::hardware_concurrency();
    std::cout << "Number of threads being used: " << num_workers << std::endl;
    ctpl::thread_pool tp(num_workers);
    std::vector<std::future<void>> worker_results(num_workers);

    window.Show();

    std::chrono::high_resolution_clock::time_point t1, t2;
    t1 = t2 = std::chrono::high_resolution_clock::now();

    while (!window.ShouldClose())
    {
        t2 = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration_cast<std::chrono::duration<float>>(t2 - t1).count();
        t1 = t2;

        camera.x += vx * dt;
        camera.z += vz * dt;

        Pixel *pixels = buffer.MapData();
        memset(pixels, 0, WIDTH * HEIGHT * sizeof(Pixel));

        // Divvy up work between threads
        size_t num_pixels = WIDTH * HEIGHT;
        size_t num_pixels_per_thread = num_pixels / num_workers;

        for (size_t i = 0; i < num_workers; ++i)
        {
            worker_results[i] = tp.push([&, i, num_pixels_per_thread](int id)
            {
                int start_p = i * num_pixels_per_thread;
                int end_p = start_p + num_pixels_per_thread;
                for (int p = start_p; p < end_p; ++p)
                {
                    int x = p % WIDTH;
                    int y = p / WIDTH;
                    float tx = static_cast<float>(x) / static_cast<float>(WIDTH);
                    float ty = 1.0f - static_cast<float>(y) / static_cast<float>(HEIGHT);

                    float dx, dy, dz;
                    camera.GenerateRay(tx, ty, dx, dy, dz);

                    float total_dist, dist;
                    float nx, ny, nz;
                    size_t steps;

                    // Sphere
                    steps = MarchRay(camera.x, camera.y, camera.z, dx, dy, dz, sphere_dist, total_dist, dist);
                    float rx = camera.x + total_dist * dx;
                    float ry = camera.y + total_dist * dy;
                    float rz = camera.z + total_dist * dz;
                    std::tie(nx, ny, nz) = sphere_normal(rx, ry, rz, dist);

                    float p_tdist, p_dist;
                    size_t p_steps = MarchRay(camera.x, camera.y, camera.z, dx, dy, dz, plane_dist, p_tdist, p_dist);
                    if (p_steps != -1 && p_tdist < total_dist)
                    {
                        steps = p_steps;
                        total_dist = p_tdist;
                        dist = p_dist;
                        rx = camera.x + total_dist * dx;
                        ry = camera.y + total_dist * dy;
                        rz = camera.z + total_dist * dz;
                        std::tie(nx, ny, nz) = plane_normal(rx, ry, rz);
                    }

                    // Do lighting
                    if (steps != -1)
                    {
                        float diffuse = -(nx * sky_dir_x + ny * sky_dir_y + nz * sky_dir_z);
                        diffuse = diffuse < 0.2f? 0.2f : diffuse;
                        pixels[p] = { diffuse, diffuse, diffuse };
                    }
                }
            });
        }

        for (auto &result : worker_results)
            result.get();
        
        buffer.UnmapData();

        buffer.Render();
        window.SwapBuffers();
    }
}