#include "png_properties.h"

std::ostream &operator<<(std::ostream &os, const IHDR_t &ihdr)
{
    os << "\tWidth: " << ihdr.width << "\n"
       << "\tHeight: " << ihdr.height << "\n"
       << "\tChannels: " << static_cast<int>(ihdr.channels) << "\n"
       << "\tBit Depth: " << static_cast<int>(ihdr.bit_depth) << "\n"
       << "\tColor Type: ";
    //<< static_cast<int>(ihdr.color_type) << "\n"
    switch (ihdr.color_type)
    {
    case 0: // Grayscale
        os << "Grayscale"
           << "\n";
        break;
    case 2: // Truecolor (RGB)
        os << "Truecolor (RGB)"
           << "\n";
        break;
    case 3: // Indexed-color (Palette)
        os << "Indexed-color (Palette)"
           << "\n";
        break;
    case 4: // Grayscale with alpha
        os << "Grayscale with alpha"
           << "\n";
        break;
    case 6: // Truecolor with alpha (RGBA)
        os << "Truecolor with alpha (RGBA)"
           << "\n";
        break;
    default:
        os << "Invalid color type"
           << "\n"; // Invalid color type
        break;
    }
    os << "\tCompression Method: " << static_cast<int>(ihdr.compression_method) << "\n"
       << "\tFilter Method: " << static_cast<int>(ihdr.filter_method) << "\n"
       << "\tInterlace Method: " << static_cast<int>(ihdr.interlace_method) << "\n";
    return os;
}

std::ostream &operator<<(std::ostream &os, const pHYs_t &phys)
{
    os << "\tPixels per unit (X): " << phys.pixels_per_unit_x << " " << (phys.unit_specifier == 0 ? "pixels" : (phys.unit_specifier == 1 ? "inches" : "unknown")) << "\n"
       << "\tPixels per unit (Y): " << phys.pixels_per_unit_y << " " << (phys.unit_specifier == 0 ? "pixels" : (phys.unit_specifier == 1 ? "inches" : "unknown")) << "\n";
    return os;
}

std::ostream &operator<<(std::ostream &os, const bKGD_t &bkgd)
{
    os << "\tBackground Color: R: " << static_cast<int>(bkgd.red) << ", G: " << static_cast<int>(bkgd.green) << ", B: " << static_cast<int>(bkgd.blue) << "\n";
    return os;
}

std::ostream &operator<<(std::ostream &os, const cHRM_t &chrm)
{
    os << "\tWhite X: " << chrm.white_x << ", Y: " << chrm.white_y << "\n"
       << "\tRed X: " << chrm.red_x << ", Y: " << chrm.red_y << "\n"
       << "\tGreen X: " << chrm.green_x << ", Y: " << chrm.green_y << "\n"
       << "\tBlue X: " << chrm.blue_x << ", Y: " << chrm.blue_y << "\n";
    return os;
}
