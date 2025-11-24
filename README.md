# InputRedir-Draw
## Description: Little C program to remotely draw images on the 3DS screen from a BMP file using the InputRedirection protocol.

### Prerequisites
Prepare a BMP file with the correct format (see [Image Requirements](#image-requirements)), compile the program, and execute it (see [Usage](#usage)), it will draw the BMP contents to your screen with a pretty good accuracy.

### Usage
Compile the program by using the `make` command, or grab the binary from the [Releases tab](https://github.com/AtexBg/inputredir-draw/releases) (Linux **x86_64** and **ARM64** are supported), and execute it with the following command-line arguments :
```bash
     ./sender <ip> [-d]
```
-  `<ip>`: The console's local IP address (ex. 192.168.1.15)
- [-d]: DEBUG flag, add it to see debug verbose output on your terminal

### Image Requirements
BMP images should respect the following requirements:
| Required | Value |
|:---|:---:|
| Name       | image.bmp    |
| Resolution | 320×240      |
| BPP        | 24 bits      |
| Size       | 230454 bytes |
| Padding    | None         |

If all requirements above are right, the image should be *EXACTLY* ``230454`` (``0x38436``) bytes in size.

You can use any screenshot taken with Luma3DS on your console (bottom screen only), the provided image, or generate a valid blank BMP by using `make gen` and launch "*./generator*".

> /!\\ Only use black and white pixels for drawing, any other color may produce unwanted touches.

### Credits
Thanks to [TuxSH](https://github.com/TuxSH) for the network/packets code from [InputRedirectionClient-Qt](https://github.com/TuxSH/InputRedirectionClient-Qt).

© 2025 AtexBg