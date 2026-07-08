# Image Steganography using LSB Encoding and Decoding
C-based image steganography application that hides and extracts secret messages using LSB techniques.


This project implements **Image Steganography** in C using the **Least Significant Bit (LSB)** technique to securely hide and retrieve secret messages within image files. It enables users to embed text data into an image without causing noticeable visual changes and accurately extract the hidden message when needed.

The project demonstrates low-level programming concepts such as bit manipulation, binary file handling, memory management, and data encoding/decoding techniques.

## Features

* Hide secret messages inside image files using LSB encoding
* Extract hidden messages using LSB decoding
* Preserve image quality after data embedding
* Efficient binary file processing
* Accurate and reliable message recovery
* Command-line based implementation

## Technologies Used

* C Programming
* Bit Manipulation
* Binary File I/O
* File Handling
* Data Encoding and Decoding

## How It Works

### Encoding

1. Reads the source image and secret message.
2. Converts the message into binary.
3. Embeds each message bit into the least significant bits of the image pixel data.
4. Generates a new image containing the hidden message.

### Decoding

1. Reads the encoded image.
2. Extracts the least significant bits from the image data.
3. Reconstructs the original secret message.
4. Displays the decoded message.

## Compilation

```bash id="pgjk8o"
gcc *.c -o steganography
```

## Execution

### Encoding

```bash id="4vhj4r"
./steganography -e source.bmp secret.txt output.bmp
```

### Decoding

```bash id="83p7e6"
./steganography -d output.bmp
```

## Learning Outcomes

* Understanding of image steganography concepts
* Practical application of bitwise operators
* Binary file manipulation in C
* Data encoding and decoding techniques
* Low-level memory and file operations

## Future Enhancements

* Password-protected message extraction
* Encryption before embedding data
* Support for larger image formats
* GUI-based interface
* Support for hiding multiple files
