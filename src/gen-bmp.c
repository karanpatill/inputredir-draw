/*
----------------- gen-bmp.c -------------------
----------- InputRedir-Draw Project -----------
---------------- AtexBg @ 2025 ----------------
-- https://github.com/atexbg/inputredir-draw --
-----------------------------------------------
--------- Dummy 24BPP BMP generator -----------
*/

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define FILE_SIZE 0x38436
#define HEADER_SIZE 0x36

uint8_t fileData[FILE_SIZE] = { 
    //fileData of 230454 bytes, first 0x36 bytes contains the proper BMP header
    0x42, 0x4D, 0x36, 0x84, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, //header size are in little-endian
    0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x40, 0x01, //0x40010000 = WIDTH  (320)
    0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x01, 0x00, 0x18, 0x00, //0xF0000000 = HEIGHT (240)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x84, 0x03, 0x00, 0x12, 0x0B,
    0x00, 0x00, 0x12, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


int main() {
    FILE *file = fopen("dummy.bmp", "wb");
    
    for(int i = HEADER_SIZE; i < FILE_SIZE; i++){
        fileData[i] = 0xFF; //fill the image data with white pixels (0xFFFFFF, in BGR)
    }
    
    fwrite(fileData, sizeof(uint8_t), FILE_SIZE, file);

    //clean-up
    fclose(file);
    return 0;
}
