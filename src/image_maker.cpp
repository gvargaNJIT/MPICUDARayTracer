#include "../include/image_maker.h"

void image_ppm(const std::string& filename, const std::vector<unsigned char>& data, int width, int height){
    std::ofstream out(filename, std::ios::binary);

    if (!out) {
        std::cerr << "Error: cannot open file for writing\n";
        return;
    }
    
    out << "P6\n" << width << ' ' << height << "\n255\n";

    out.write(reinterpret_cast<const char*>(data.data()), data.size());
}