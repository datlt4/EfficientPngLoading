#include "parsing_chunks.h"
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <zlib.h> // For CRC32 calculation

bool parse_ihdr_chunk(std::ifstream &stream, uint32_t chunk_length, IHDR_t &ihdr)
{
    // IHDR chunk must be 13 bytes long (this is specified by the PNG standard)
    assert(chunk_length == 13);

    // Prepare a buffer to store the chunk data (13 bytes) and the CRC value (4 bytes)
    uint8_t buffer[chunk_length + 4]; // 13 bytes for IHDR data, 4 bytes for CRC
    stream.read(reinterpret_cast<char *>(buffer), chunk_length + 4);

    // Extract the width and height from the buffer (both are 4-byte big-endian integers)
    // PNG data is stored in big-endian, so we manually convert it to little-endian here
    ihdr.width = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | (buffer[3] << 0);
    ihdr.height = (buffer[4] << 24) | (buffer[5] << 16) | (buffer[6] << 8) | (buffer[7] << 0);

    // // PNG is stored in big-endian, convert from big-endian to little-endian if necessary
    // ihdr.width = __builtin_bswap32(ihdr.width);
    // ihdr.height = __builtin_bswap32(ihdr.height);

    // Read bit depth (1 byte)
    ihdr.bit_depth = buffer[8];

    // Read color type (1 byte)
    ihdr.color_type = buffer[9];

    // Read compression method (1 byte)
    ihdr.compression_method = buffer[10];

    // Read filter method (1 byte)
    ihdr.filter_method = buffer[11];

    // Read interlace method (1 byte)
    ihdr.interlace_method = buffer[12];

    // Determine the number of channels based on the color type
    switch (ihdr.color_type)
    {
    case 0: // Grayscale
        ihdr.channels = 1;
        break;
    case 2: // Truecolor (RGB)
        ihdr.channels = 3;
        break;
    case 3: // Indexed-color (Palette)
        ihdr.channels = 1;
        break;
    case 4: // Grayscale with alpha
        ihdr.channels = 2;
        break;
    case 6: // Truecolor with alpha (RGBA)
        ihdr.channels = 4;
        break;
    default:
        std::cerr << "Error: Parse IHDR chunk - CRC mismatch!" << std::endl;
        ihdr.channels = 0; // Invalid color type
        break;
    }
    // Ensure that the color type is valid and channels is not zero
    assert(ihdr.channels != 0);

    // Extract the CRC value from the buffer (last 4 bytes)
    uint32_t crc_value = buffer[chunk_length] << 24 | buffer[chunk_length + 1] << 16 | buffer[chunk_length + 2] << 8 | buffer[chunk_length + 3] << 0;

    // Calculate the CRC for the IHDR chunk data (including the chunk type "IHDR")
    // CRC-32 computed over the chunk type and chunk data, but not the length.
    const char chunk_type[4] = {'I', 'H', 'D', 'R'};
    uint32_t crc_integrity = crc32(0L, (uint8_t *)chunk_type, 4);          // Start with "IHDR" type
    crc_integrity = crc32(crc_integrity, (uint8_t *)buffer, chunk_length); // Continue with chunk data

    if (crc_integrity != crc_value)
    {
        std::cerr << "Error: Parse IHDR chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse IHDR chunk successfully!" << std::endl;
    return true;
}

bool parse_plte_chunk(std::ifstream &stream, uint32_t chunk_length, std::vector<RGB_t> &palette)
{
    // PLTE chunk must be a multiple of 3 (since each color is represented by 3 bytes: R, G, B)
    assert(chunk_length % 3 == 0);

    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the RGB values and store them in the palette vector
    for (uint32_t i = 0; i < chunk_length; i += 3)
    {
        palette.emplace_back(buffer[i], buffer[i + 1], buffer[i + 2]);
    }

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "PLTE" chunk type)
    const char chunk_type[4] = {'P', 'L', 'T', 'E'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "PLTE"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse PLTE chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse PLTE chunk successfully!" << std::endl;
    return true;
}

bool parse_idat_chunk(std::ifstream &stream, uint32_t chunk_length, std::vector<uint8_t> &compressed_data)
{
    assert(chunk_length > 0);
    // Read the compressed data
    std::vector<uint8_t> buffer(chunk_length + 4);
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Append the current chunk's data to the compressed_data vector
    compressed_data.insert(compressed_data.end(), buffer.begin(), buffer.end() - 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "IDAT" chunk type)
    const char chunk_type[4] = {'I', 'D', 'A', 'T'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "IDAT"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse IDAT chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse IDAT chunk successfully! - "
              << "chunk_lenght: " << chunk_length << std::endl;
    return true;
}

std::vector<uint8_t> decompress_idat_data(const std::vector<uint8_t> &compressed_data)
{
    // Set up the zlib stream
    z_stream zlib_stream;
    zlib_stream.zalloc = Z_NULL;
    zlib_stream.zfree = Z_NULL;
    zlib_stream.opaque = Z_NULL;
    zlib_stream.avail_in = compressed_data.size();
    zlib_stream.next_in = const_cast<uint8_t *>(compressed_data.data());

    // Initialize zlib for decompression
    if (inflateInit(&zlib_stream) != Z_OK)
    {
        std::cerr << "Error initializing zlib." << std::endl;
        return {};
    }

    // Output buffer for decompressed data
    std::vector<uint8_t> decompressed_data(1024 * 1024); // 1 MB initial buffer size (resize later if necessary)

    // Decompress the data
    int ret;
    do
    {
        zlib_stream.avail_out = decompressed_data.size() - zlib_stream.total_out;
        zlib_stream.next_out = decompressed_data.data() + zlib_stream.total_out;

        ret = inflate(&zlib_stream, Z_NO_FLUSH);
        if (ret == Z_BUF_ERROR)
        {
            // Increase buffer size if necessary
            decompressed_data.resize(decompressed_data.size() * 2);
        }
    } while (ret != Z_STREAM_END && ret != Z_MEM_ERROR);

    // Resize the buffer to the actual decompressed size
    decompressed_data.resize(zlib_stream.total_out);

    // Clean up zlib resources
    inflateEnd(&zlib_stream);

    if (ret != Z_STREAM_END)
    {
        std::cerr << "Error during decompression." << std::endl;
        return {};
    }

    return decompressed_data;
}

bool parse_iend_chunk(std::ifstream &stream, uint32_t chunk_length)
{
    // The IEND chunk should always have a length of 0
    assert(chunk_length == 0);

    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "IEND" chunk type)
    const char chunk_type[4] = {'I', 'E', 'N', 'D'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4); // Start with the chunk type "IEND"

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse IEND chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse IEND chunk successfully!" << std::endl;
    return true;
}

bool parse_bkgd_chunk(std::ifstream &stream, uint32_t chunk_length, bKGD_t &bkgd_color)
{
    assert((chunk_length == 1) || (chunk_length == 6));
    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "bKGD" chunk type)
    const char chunk_type[4] = {'b', 'K', 'G', 'D'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "bKGD"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse bKGD chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }

    // Parse the background color data based on the chunk length
    if (chunk_length == 1) // Indexed-color image
    {
        bkgd_color.index = buffer[0];
        bkgd_color.is_indexed = true;
        std::cout << "Background color index: " << static_cast<int>(bkgd_color.index) << std::endl;
    }
    else if (chunk_length == 6) // Truecolor image
    {
        bkgd_color.red = buffer[0];
        bkgd_color.green = buffer[1];
        bkgd_color.blue = buffer[2];
        bkgd_color.is_indexed = false;
        std::cout << "Background color (RGB): (" << static_cast<int>(bkgd_color.red) << ", " << static_cast<int>(bkgd_color.green) << ", " << static_cast<int>(bkgd_color.blue) << ")" << std::endl;
    }
    else
    {
        std::cerr << "Error: Invalid bKGD chunk length!" << std::endl;
        return false; // Invalid length for bKGD chunk
    }

    // If everything is correct, return true
    std::cout << "Parse bKGD chunk successfully!" << std::endl;
    return true;
}

bool parse_chrm_chunk(std::ifstream &stream, uint32_t chunk_length, cHRM_t &chrm)
{
    assert(chunk_length == 32);

    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "cHRM" chunk type)
    const char chunk_type[4] = {'c', 'H', 'R', 'M'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "cHRM"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse cHRM chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }

    // Extract the chromaticity coordinates (converting from big-endian to uint32_t)
    chrm.red_x = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
    chrm.red_y = (buffer[4] << 24) | (buffer[5] << 16) | (buffer[6] << 8) | buffer[7];
    chrm.green_x = (buffer[8] << 24) | (buffer[9] << 16) | (buffer[10] << 8) | buffer[11];
    chrm.green_y = (buffer[12] << 24) | (buffer[13] << 16) | (buffer[14] << 8) | buffer[15];
    chrm.blue_x = (buffer[16] << 24) | (buffer[17] << 16) | (buffer[18] << 8) | buffer[19];
    chrm.blue_y = (buffer[20] << 24) | (buffer[21] << 16) | (buffer[22] << 8) | buffer[23];
    chrm.white_x = (buffer[24] << 24) | (buffer[25] << 16) | (buffer[26] << 8) | buffer[27];
    chrm.white_y = (buffer[28] << 24) | (buffer[29] << 16) | (buffer[30] << 8) | buffer[31];

    // If everything is correct, return true
    std::cout << "Parse cHRM chunk successfully!" << std::endl;
    return true;
}

bool parse_cicp_chunk(std::ifstream &stream, uint32_t chunk_length)
{
    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "cICP" chunk type)
    const char chunk_type[4] = {'c', 'I', 'C', 'P'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "cICP"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse cICP chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse cICP chunk successfully!" << std::endl;
    return true;
}

bool parse_dsig_chunk(std::ifstream &stream, uint32_t chunk_length)
{
    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "dSIG" chunk type)
    const char chunk_type[4] = {'d', 'S', 'I', 'G'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "dSIG"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse dSIG chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse dSIG chunk successfully!" << std::endl;
    return true;
}

bool parse_exif_chunk(std::ifstream &stream, uint32_t chunk_length)
{
    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "eXIf" chunk type)
    const char chunk_type[4] = {'e', 'X', 'I', 'f'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "eXIf"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse eXIf chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse eXIf chunk successfully!" << std::endl;
    return true;
}

bool parse_gama_chunk(std::ifstream &stream, uint32_t chunk_length)
{
    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "gAMA" chunk type)
    const char chunk_type[4] = {'g', 'A', 'M', 'A'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "gAMA"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse gAMA chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse gAMA chunk successfully!" << std::endl;
    return true;
}

bool parse_hist_chunk(std::ifstream &stream, uint32_t chunk_length)
{
    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "hIST" chunk type)
    const char chunk_type[4] = {'h', 'I', 'S', 'T'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "hIST"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse hIST chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse hIST chunk successfully!" << std::endl;
    return true;
}

bool parse_iccp_chunk(std::ifstream &stream, uint32_t chunk_length)
{
    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "iCCP" chunk type)
    const char chunk_type[4] = {'i', 'C', 'C', 'P'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "iCCP"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse iCCP chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse iCCP chunk successfully!" << std::endl;
    return true;
}

bool parse_itxt_chunk(std::ifstream &stream, uint32_t chunk_length)
{
    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "iTXt" chunk type)
    const char chunk_type[4] = {'i', 'T', 'X', 't'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "iTXt"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse iTXt chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse iTXt chunk successfully!" << std::endl;
    return true;
}

bool parse_phys_chunk(std::ifstream &stream, uint32_t chunk_length, pHYs_t &phys)
{
    // pHYs chunk must be 9 bytes long (this is specified by the PNG standard)
    assert(chunk_length == 9);

    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract pixels per unit (X axis) from the first 4 bytes
    phys.pixels_per_unit_x = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];

    // Extract pixels per unit (Y axis) from the next 4 bytes
    phys.pixels_per_unit_y = (buffer[4] << 24) | (buffer[5] << 16) | (buffer[6] << 8) | buffer[7];

    // Extract unit specifier from the 9th byte
    phys.unit_specifier = buffer[8];

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "pHYs" chunk type)
    const char chunk_type[4] = {'p', 'H', 'Y', 's'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "pHYs"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse pHYs chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse pHYs chunk successfully!" << std::endl;
    return true;
}

bool parse_sbit_chunk(std::ifstream &stream, uint32_t chunk_length)
{
    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "sBIT" chunk type)
    const char chunk_type[4] = {'s', 'B', 'I', 'T'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "sBIT"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse sBIT chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse sBIT chunk successfully!" << std::endl;
    return true;
}

bool parse_splt_chunk(std::ifstream &stream, uint32_t chunk_length)
{
    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "sPLT" chunk type)
    const char chunk_type[4] = {'s', 'P', 'L', 'T'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "sPLT"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse sPLT chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse sPLT chunk successfully!" << std::endl;
    return true;
}

bool parse_srgb_chunk(std::ifstream &stream, uint32_t chunk_length)
{
    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "sRGB" chunk type)
    const char chunk_type[4] = {'s', 'R', 'G', 'B'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "sRGB"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse sRGB chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse sRGB chunk successfully!" << std::endl;
    return true;
}

bool parse_ster_chunk(std::ifstream &stream, uint32_t chunk_length)
{
    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "sTER" chunk type)
    const char chunk_type[4] = {'s', 'T', 'E', 'R'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "sTER"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse sTER chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse sTER chunk successfully!" << std::endl;
    return true;
}

bool parse_text_chunk(std::ifstream &stream, uint32_t chunk_length)
{
    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "tEXt" chunk type)
    const char chunk_type[4] = {'t', 'E', 'X', 't'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "tEXt"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse tEXt chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse tEXt chunk successfully!" << std::endl;
    return true;
}

bool parse_time_chunk(std::ifstream &stream, uint32_t chunk_length)
{
    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "tIME" chunk type)
    const char chunk_type[4] = {'t', 'I', 'M', 'E'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "tIME"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse tIME chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse tIME chunk successfully!" << std::endl;
    return true;
}

bool parse_trns_chunk(std::ifstream &stream, uint32_t chunk_length)
{
    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "tRNS" chunk type)
    const char chunk_type[4] = {'t', 'R', 'N', 'S'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "tRNS"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse tRNS chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse tRNS chunk successfully!" << std::endl;
    return true;
}

bool parse_ztxt_chunk(std::ifstream &stream, uint32_t chunk_length)
{
    // Read the palette data (chunk_length bytes)
    std::vector<uint8_t> buffer(chunk_length + 4); // +4 for the CRC
    stream.read(reinterpret_cast<char *>(buffer.data()), chunk_length + 4);

    // Extract the CRC value from the last 4 bytes of the buffer
    uint32_t crc_value = (buffer[chunk_length] << 24) | (buffer[chunk_length + 1] << 16) | (buffer[chunk_length + 2] << 8) | (buffer[chunk_length + 3]);

    // Calculate the CRC of the chunk data (including the "zTXt" chunk type)
    const char chunk_type[4] = {'z', 'T', 'X', 't'};
    uint32_t calculated_crc = crc32(0L, (uint8_t *)chunk_type, 4);       // Start with the chunk type "zTXt"
    calculated_crc = crc32(calculated_crc, buffer.data(), chunk_length); // Include chunk data

    // Check if the calculated CRC matches the one read from the file
    if (calculated_crc != crc_value)
    {
        std::cerr << "Error: Parse zTXt chunk - CRC mismatch!" << std::endl;
        return false; // Return false if there's a CRC mismatch
    }
    // If everything is correct, return true
    std::cout << "Parse zTXt chunk successfully!" << std::endl;
    return true;
}
