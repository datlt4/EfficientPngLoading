#ifndef __PARSING_CHUNKS__
#define __PARSING_CHUNKS__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "png_properties.h"

bool parse_png_header(const char *filename, IHDR_t *props);

// Parse the IHDR chunk
bool parse_ihdr_chunk(std::ifstream &stream, uint32_t chunk_length, IHDR_t &ihdr);

// Parse the PLTE chunk
bool parse_plte_chunk(std::ifstream &stream, uint32_t chunk_length, std::vector<RGB_t> &palette);

// Parse the IDAT chunk
bool parse_idat_chunk(std::ifstream &stream, uint32_t chunk_length, std::vector<uint8_t> &compressed_data);

// Function to decompress the concatenated IDAT data
std::vector<uint8_t> decompress_idat_data(const std::vector<uint8_t> &compressed_data);

// Parse the IEND chunk
bool parse_iend_chunk(std::ifstream &stream, uint32_t chunk_length);

// Parse the bKGD chunk
bool parse_bkgd_chunk(std::ifstream &stream, uint32_t chunk_length, bKGD_t &bkgd_color);

// Parse the cHRM chunk
bool parse_chrm_chunk(std::ifstream &stream, uint32_t chunk_length, cHRM_t &chrm);

// Parse the cICP chunk
bool parse_cicp_chunk(std::ifstream &stream, uint32_t chunk_length);

// Parse the dSIG chunk
bool parse_dsig_chunk(std::ifstream &stream, uint32_t chunk_length);

// Parse the eXIf chunk
bool parse_exif_chunk(std::ifstream &stream, uint32_t chunk_length);

// Parse the gAMA chunk
bool parse_gama_chunk(std::ifstream &stream, uint32_t chunk_length);

// Parse the hIST chunk
bool parse_hist_chunk(std::ifstream &stream, uint32_t chunk_length);

// Parse the iCCP chunk
bool parse_iccp_chunk(std::ifstream &stream, uint32_t chunk_length);

// Parse the iTXt chunk
bool parse_itxt_chunk(std::ifstream &stream, uint32_t chunk_length);

// Parse the pHYs chunk
bool parse_phys_chunk(std::ifstream &stream, uint32_t chunk_length, pHYs_t &phys);

// Parse the sBIT chunk
bool parse_sbit_chunk(std::ifstream &stream, uint32_t chunk_length);

// Parse the sPLT chunk
bool parse_splt_chunk(std::ifstream &stream, uint32_t chunk_length);

// Parse the sRGB chunk
bool parse_srgb_chunk(std::ifstream &stream, uint32_t chunk_length);

// Parse the sTER chunk
bool parse_ster_chunk(std::ifstream &stream, uint32_t chunk_length);

// Parse the tEXt chunk
bool parse_text_chunk(std::ifstream &stream, uint32_t chunk_length);

// Parse the tIME chunk
bool parse_time_chunk(std::ifstream &stream, uint32_t chunk_length);

// Parse the tRNS chunk
bool parse_trns_chunk(std::ifstream &stream, uint32_t chunk_length);

// Parse the zTXt chunk
bool parse_ztxt_chunk(std::ifstream &stream, uint32_t chunk_length);

#endif // __PARSING_CHUNKS__
