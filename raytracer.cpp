#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

#include <iostream>
#include <mpi.h>

// Global Scene
// IMAGE
const auto aspect_ratio = 16.0 / 9.0;
const int image_width = 1920;
const int image_height = static_cast<int>(image_width / aspect_ratio);
const int samples_per_pixel = 300;
const int max_depth = 50;

// CAMERA
point3 lookfrom(13, 2, 3);
point3 lookat(0, 0, 0);
vec3 vup(0, 1, 0);
auto dist_to_focus = 10.0;
auto aperture = 0.1;

camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);


// color ray_color(const ray& r, const hittable& world)
// - takes in a ray and a hittable object 
color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);

    // if the ray hits a hittable object, return the resulting ray color
    // because the resulting ray may hit another hittable object, we recursively
    // call the ray_color function - with a max depth so that the ray cannot
    // infinitely bounce
    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return color(0, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);

    // linear interpolation
    // blendedValue = (1 - t) * startValue + (t * endValue)
    return (1.0 - t) * color(1.0, 1.0, 1.0) + (t * color(0.5, 0.7, 1.0));
}

// hittable_list random_scene()
hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

// raytrace(rank, size, rows_per_node)
void raytrace(int rank, int size, int rows_per_node, hittable_list world) {
    double local_image[rows_per_node][image_width][3];
    double u, v;
    ray r;
    MPI_Request request;

    // Progress Print Stmt
    cerr << "Inside Rank #" << rank << endl;

    // "j" represents the pixel row number and is incremented by "size" (number of nodes)
    // Aids in load distribution (explained below in main())
    for (int j = rank ; j < image_height ; j += size) {
        // "i" represents the pixel column number incremented by 1 until end of image is reached
        for (int i = 0 ; i < image_width ; i++) {
            color pixel_color(0, 0, 0);
            for (int s = 0 ; s < samples_per_pixel ; ++s) {
                u = (double)(i + random_double()) / (image_width - 1);
                v = (double)(j + random_double()) / (image_height - 1);
                r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            local_image[j / size][i][0] = pixel_color.x();
            local_image[j / size][i][1] = pixel_color.y();
            local_image[j / size][i][2] = pixel_color.z();
        }
    }

    // Send message to node 0
    MPI_Isend(&local_image, rows_per_node * image_width * 3, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &request);
}

int main(int argc, char *argv[]) {

    // WORLD
    hittable_list world = random_scene();

    // MPI
    // Rank Number (node id) and Rank Size (number of nodes)
    int rank, size;
    int rows_per_node;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Determine how many rows of pixels each node will be assigned
    rows_per_node = image_height / size;

    // Node 0 starts timer.
    double start_time, end_time;
    if (rank == 0) {
        start_time = MPI_Wtime();
    }

    // RENDER
    // 1. Assign image rows (of pixels) to current node depending on its rank and raytrace
    // Nodes are asssigned certain rows of pixels to maintain an efficient load distribution
    // (don't want the same node to render same chunk)
    raytrace(rank, size, rows_per_node, world);

    // 2. In Node 0, send image to output
    if (rank == 0) {
        double whole_image[size][rows_per_node * image_width * 3];
        
        // Image Format
        cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
        
        // Image
        for (int r = 0 ; r < size ; r++) {
            MPI_Recv(&whole_image[r], rows_per_node * image_width * 3, MPI_DOUBLE, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // sample and write pixel colors
        for (int j = image_height - 1 ; j >= 0 ; j--) {
            int n = j % size;
            for (int i = 0 ; i < image_width ; i++) {
                int ind = ((j / size) * image_width + i) * 3;
                color pixel_color(whole_image[n][ind + 0],
                                  whole_image[n][ind + 1],
                                  whole_image[n][ind + 2]);
                write_color(cout, pixel_color, samples_per_pixel);
            }
        }
        // 4. End timer
        end_time = MPI_Wtime();
        cerr << "\nTime Elapsed: " << end_time - start_time << endl;
    }


    MPI_Finalize();
}