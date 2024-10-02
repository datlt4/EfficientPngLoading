#ifndef __MAIN_H__
#define __MAIN_H__

#include "parsing_chunks.h"
#include <cstring>
#include <fstream>
#include <iostream>

bool decode_png_file(std::ifstream &stream, png_properties_t &properties);

#endif // __MAIN_H__
