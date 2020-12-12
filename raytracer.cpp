#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"

#include <iostream>

// color ray_color(const ray& r, const hittable& world)
// - takes in a ray and a hittable object 
color ray_color(const ray& r, const hittable& world) {
    hit_record rec;
    if (world.hit(r, 0, infinity, rec)) {
        return 0.5 * (rec.normal + color(1, 1, 1));
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);

    // linear interpolation
    // blendedValue = (1 - t) * startValue + (t * endValue)
    return (1.0 - t) * color(1.0, 1.0, 1.0) + (t * color(0.5, 0.7, 1.0));
}

int main() {
    
    // IMAGE
    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;

    // WORLD
    hittable_list world;
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

    // CAMERA
    camera cam;

    // RENDER
    cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    // loops the pixels in each row (height) and column (width) and each
    // sample within each pixel
    for (int j = image_height ; j >= 0 ; --j) {
        cerr << "\rScanlines remaining: " << j << ' ' << flush;         // Progress Indicator
        for (int i = 0 ; i < image_width ; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0 ; s < samples_per_pixel ; ++s) {
                // for each sample in the current pixel increment the pixel_color
                auto u = double(i + random_double()) / (image_width - 1);
                auto v = double(j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world);
            }
            write_color(cout, pixel_color, samples_per_pixel);
        }
    }
    cerr << "\nDone.\n";
}