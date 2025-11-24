/*
------------------- main.c --------------------
----------- InputRedir-Draw Project -----------
---------------- AtexBg @ 2025 ----------------
-- https://github.com/atexbg/inputredir-draw --
-----------------------------------------------
---------- Main file for the project -----------
*/

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h> //open
#include <unistd.h> //close
#include <stdlib.h> //malloc
#include <time.h>

#define WIDTH 320
#define HEIGHT 240
#define FILE_SIZE 0x38436 //BMP file size in bytes
#define HEADER_SIZE 0x36
#define HALF_DELAY_MS 50 

void sendPacket(int DEBUG, const char *ip, int pressed, int px, int py, int packetsSent)
{
    //Unused values, only for packet integrity
    uint32_t hidPad = 0xFFF;
    uint32_t circlePadState = 0x7ff7ff;    
    uint32_t cppState = 0x80800081;        
    uint32_t interfaceButtons = 0;         

    //Packet for state and coords of touch point
    uint32_t touchScreenState;

    if (pressed) {
        if (px < 0) px = 0;
        if (py < 0) py = 0;
        if (px > WIDTH) px = WIDTH;
        if (py > HEIGHT) py = HEIGHT;

        //coords are mapped in a 4095*4095 memory space, but are still 320*240px for the console
        //TODO: FIX DEAD LINES AND COORDS SKIPPING
        uint32_t x = (px * 4095) / WIDTH;  
        uint32_t y = (py * 4095) / HEIGHT; 

        touchScreenState = (1u << 24) | (y << 12) | x;
    } else {
        //release: same coords, but no bit 24
        uint32_t x = (px * 4095) / WIDTH;
        uint32_t y = (py * 4095) / HEIGHT;

        touchScreenState = (y << 12) | x;
    }

    //build packet
    uint8_t packet[20];
    memcpy(packet,&hidPad,4);
    memcpy(packet+4,&touchScreenState,4);
    memcpy(packet+8,&circlePadState,4);
    memcpy(packet+12,&cppState,4);
    memcpy(packet+16,&interfaceButtons,4);

    //yeah, 1 socket for every pixel, i have to fix this...
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(4950);
    inet_pton(AF_INET, ip, &addr.sin_addr);

    sendto(sock, packet, sizeof(packet), 0, (struct sockaddr*)&addr, sizeof(addr));

    if(DEBUG){printf("\x1b[32m[DEBUG]\x1b[0m Packet \x1b[34m");
        for(int i = 0; i < 20; i++) {
            printf("%02X ", packet[i]); //display array values as hex
        } printf("\x1b[0m(%d) sent to \x1b[33m%s\x1b[0m\n", packetsSent, ip);
        if(!pressed){printf("(\x1b[32mRELEASE\x1b[0m packet sent.)\n");}
    }
    close(sock);
}


int main(int argc, char *argv[]){

    if (argc < 2) {
        printf("Usage: %s <ip> [-d]\n", argv[0]);
        return 1;
    }

    time_t start = time(NULL);
    int DEBUG = 0;
    int packetsSent = 0;
    char* ip = argv[1]; //init IP from argv

    if (argc > 2 && strcmp(argv[2], "-d") == 0) {
        DEBUG = 1; //enable debug verbose
    }

    if(!ip){printf("Please specifiy an IP address.\n"); printf("Example: %s 192.168.1.1\n", argv[0]); return 1;}

    //open and read file
    FILE *file = fopen("image.bmp", "rb");
    if(!file){perror("Error opening 'image.bmp'"); return 1;}
    uint8_t *fileData = malloc(FILE_SIZE);
    size_t bytes = fread(fileData, sizeof(uint8_t), FILE_SIZE, file);
    if(bytes != FILE_SIZE){printf("Unexpected size of %zu bytes, program may fail.", bytes);}

    if(DEBUG){printf("\x1b[32m[DEBUG]\x1b[0m BMP Size: %zu bytes\n", bytes);}
    
    //check for black pixels
    for(int i=0; i < HEIGHT; i++){
        for(int X=0; X < WIDTH; X++){
            if(fileData[X*3 + (i*WIDTH*3) + HEADER_SIZE] == 0x00){
                int Y = HEIGHT - 1 - i; //Y-axis fix
                if(DEBUG){printf("\x1b[32m[DEBUG]\x1b[0m Black pixel found at X=\x1b[35m%d\x1b[0m, Y=\x1b[35m%d\x1b[0m\n", X, Y);}

                sendPacket(DEBUG, ip, 1, X, Y, packetsSent); //press
                usleep(HALF_DELAY_MS * 1000); //sleep HALF_DELAY_MS twice so the console can process the inputs
                sendPacket(DEBUG, ip, 0, X, Y, packetsSent); //release
                usleep(HALF_DELAY_MS * 1000);
                packetsSent += 2;
            }
        }
    }

    time_t end = time(NULL);
    printf("Packets sent: \x1b[32m%d\x1b[0m\n", packetsSent);
    printf("Time elapsed: \x1b[32m%d\x1b[0m s.\n", end - start);
    free(fileData);
    return 0;
}
