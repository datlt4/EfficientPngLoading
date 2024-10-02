#ifndef __PNG_PROPERTIES_H__
#define __PNG_PROPERTIES_H__

// Structure to store PNG properties
#include <iostream>
#include <vector>

typedef struct _IHDR
{
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint8_t bit_depth;
    uint8_t color_type;
    uint8_t compression_method;
    uint8_t filter_method;
    uint8_t interlace_method;
} IHDR_t;

// Structure to store pHYs values
typedef struct _RGB
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;

    // Constructor to initialize the RGB values
    _RGB(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b)
    {
    }
} RGB_t;

// Structure to store physical pixel dimensions
typedef struct _pHYs
{
    uint32_t pixels_per_unit_x;
    uint32_t pixels_per_unit_y;
    uint8_t unit_specifier;
} pHYs_t;

// Additional structure to hold background color
typedef struct _bKGD
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t index;   // For indexed images
    bool is_indexed; // Flag to indicate if this is an indexed color
} bKGD_t;

// Extract the chromaticity information
typedef struct _cHRM
{
    uint32_t red_x;
    uint32_t red_y;
    uint32_t green_x;
    uint32_t green_y;
    uint32_t blue_x;
    uint32_t blue_y;
    uint32_t white_x;
    uint32_t white_y;
} cHRM_t;

// Png file properties
typedef struct _png_properties
{
    IHDR_t ihdr;
    pHYs_t phys;
    bKGD_t bkgd; // Background color for non-indexed images
    cHRM_t chrm; // Chromaticity information
    std::vector<RGB_t> palette;
    std::vector<uint8_t> compressed_data;
    std::vector<uint8_t> decompressed_data;
} png_properties_t;

// Overload the << operator
std::ostream &operator<<(std::ostream &os, const IHDR_t &ihdr);
std::ostream &operator<<(std::ostream &os, const pHYs_t &phys);
std::ostream &operator<<(std::ostream &os, const bKGD_t &bkgd);
std::ostream &operator<<(std::ostream &os, const cHRM_t &chrm);
#endif // __PNG_PROPERTIES_H__
