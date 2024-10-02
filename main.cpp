#include "main.h"

int main(int argc, char **argv)
{
    // std::ifstream png_file("./PNG_transparency_demonstration_1.png", std::ios::binary);
    if (argc < 2)
    {
        std::cerr << "Usage:./EfficientPngLoading <input_png_file>" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream png_file(argv[1], std::ios::binary);
    if (!png_file.is_open())
    {
        std::cerr << "Error opening PNG file." << std::endl;
        return 1;
    }

    // Decode png image
    png_properties_t img_properties;

    if (!decode_png_file(png_file, img_properties))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

bool decode_png_file(std::ifstream &stream, png_properties_t &properties)
{

    std::vector<uint8_t> png_header(8);
    stream.read(reinterpret_cast<char *>(png_header.data()), png_header.size());

    // Check if the file is a PNG
    if (png_header[0] != 0x89 || png_header[1] != 'P' || png_header[2] != 'N' || png_header[3] != 'G' || png_header[4] != 0x0d || png_header[5] != 0x0a || png_header[6] != 0x1a ||
        png_header[7] != 0x0a)
    {
        std::cerr << "Error: Not a valid PNG file." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Parse PNG header successfully!" << std::endl;

    // Read chunks
    while (true)
    {
        uint8_t chunk_length_c[4];
        stream.read(reinterpret_cast<char *>(chunk_length_c), sizeof(4));
        if (stream.eof())
            break; // Stop if we can't read 4 bytes (end of file)
        uint32_t chunk_length = chunk_length_c[0] << 24 | chunk_length_c[1] << 16 | chunk_length_c[2] << 8 | chunk_length_c[3];

        char chunk_type[4];
        stream.read(chunk_type, 4);
        if (std::strncmp(chunk_type, "IHDR", 4) == 0)
        {
            if (parse_ihdr_chunk(stream, chunk_length, properties.ihdr))
            {
                std::cout << "Image properties:\n" << properties.ihdr << std::endl;
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "PLTE", 4) == 0)
        {
            if (parse_plte_chunk(stream, chunk_length, properties.palette))
            {
                // Process palette
                std::cout << "Palette: " << properties.palette.size() << std::endl;
                // for (const auto &rgb : palette)
                //{
                //    std::cout << "(" << static_cast<int>(rgb.red) << ", " << static_cast<int>(rgb.green) << ", " << static_cast<int>(rgb.blue) << ")\n";
                //}
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "IDAT", 4) == 0)
        {
            if (parse_idat_chunk(stream, chunk_length, properties.compressed_data))
            {
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "IEND", 4) == 0)
        {
            if (parse_iend_chunk(stream, chunk_length))
            {
                // End reading png image
                // Decompress IDAT data
                std::vector<uint8_t> decompressed_data = decompress_idat_data(properties.compressed_data);

                // Process decompressed data
                // Save the decompressed image to a file, for example
                std::ofstream output_file("decompressed_image.bin", std::ios::binary);
                output_file.write(reinterpret_cast<const char *>(decompressed_data.data()), decompressed_data.size());
                output_file.close();
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "bKGD", 4) == 0)
        {
            if (parse_bkgd_chunk(stream, chunk_length, properties.bkgd))
            {
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "cHRM", 4) == 0)
        {
            if (parse_chrm_chunk(stream, chunk_length, properties.chrm))
            {
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "cICP", 4) == 0)
        {
            if (parse_cicp_chunk(stream, chunk_length))
            {
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "dSIG", 4) == 0)
        {
            if (parse_dsig_chunk(stream, chunk_length))
            {
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "eXIf", 4) == 0)
        {
            if (parse_exif_chunk(stream, chunk_length))
            {
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "gAMA", 4) == 0)
        {
            if (parse_gama_chunk(stream, chunk_length))
            {
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "hIST", 4) == 0)
        {
            if (parse_hist_chunk(stream, chunk_length))
            {
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "iCCP", 4) == 0)
        {
            if (parse_iccp_chunk(stream, chunk_length))
            {
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "iTXt", 4) == 0)
        {
            if (parse_itxt_chunk(stream, chunk_length))
            {
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "pHYs", 4) == 0)
        {
            if (parse_phys_chunk(stream, chunk_length, properties.phys))
            {
                std::cout << "Physical properties:\n" << properties.phys << std::endl;
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "sBIT", 4) == 0)
        {
            if (parse_sbit_chunk(stream, chunk_length))
            {
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "sPLT", 4) == 0)
        {
            if (parse_splt_chunk(stream, chunk_length))
            {
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "sRGB", 4) == 0)
        {
            if (parse_srgb_chunk(stream, chunk_length))
            {
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "sTER", 4) == 0)
        {
            if (parse_ster_chunk(stream, chunk_length))
            {
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "tEXt", 4) == 0)
        {
            if (parse_text_chunk(stream, chunk_length))
            {
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "tIME", 4) == 0)
        {
            if (parse_time_chunk(stream, chunk_length))
            {
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "tRNS", 4) == 0)
        {
            if (parse_trns_chunk(stream, chunk_length))
            {
            }
            else
                return false;
        }
        else if (std::strncmp(chunk_type, "zTXt", 4) == 0)
        {
            if (parse_ztxt_chunk(stream, chunk_length))
            {
            }
            else
                return false;
        }
    }
    return true;
}
