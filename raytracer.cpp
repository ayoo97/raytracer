#include <iostream>
#include <vector>

#include "color.h"
#include "vec3.h"

int main() {
    
    // image

    const int image_width = 256;
    const int image_height = 256;

    // render

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height ; j >= 0 ; --j) {
        
        // -------------- this is a Progress Indicator -------------------
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        // ---------------------------------------------------------------   

        for (int i = 0 ; i < image_width ; ++i) {
            /*auto r = double(i) / (image_width - 1);
            auto g = double(j) / (image_height - 1);
            auto b = 0.25;

            int ir = static_cast<int>(255.999 * r);
            int ig = static_cast<int>(255.999 * g);
            int ib = static_cast<int>(255.999 * b);

            std::cout << ir << ' ' << ig << ' ' << ib << '\n';*/
            
            // instead of the loop above, use write_color() from color.h
            color pixel_color(double(i) / (image_width - 1), double(j) / (image_height - 1), 0.25);
            write_color(std::cout, pixel_color);
        }
    }

    std::cerr << "\nDone.\n";
}