# Image-Steganography-using-LSB-Encoding-and-Decoding
C-based image steganography application that hides and extracts secret messages using LSB techniques.

This project implements Image Steganography in C using the Least Significant Bit (LSB) technique to securely hide and retrieve secret messages within image files. The application allows users to embed text data into an image without causing noticeable visual changes and later extract the hidden information accurately.

The project demonstrates low-level programming concepts such as bit manipulation, binary file handling, memory management, and data encoding/decoding techniques.

Features

Hide secret messages inside image files using LSB encoding
Extract hidden messages using LSB decoding
Preserve image quality after data embedding
Efficient binary file processing
Secure message recovery without data loss
Command-line based implementation in C

Technologies Used

C Programming
Bit Manipulation
Binary File I/O
File Handling
Data Encoding and Decoding

How It Works

Encoding
Reads the source image and secret message.
Stores message bits in the least significant bits of image pixel data.
Generates a new image containing the hidden message.

Decoding
Reads the encoded image.
Extracts the least significant bits from image data.
Reconstructs and displays the original hidden message.

Learning Outcomes

Understanding of steganography concepts
Practical use of bitwise operators
Binary file manipulation in C
Data encoding and decoding techniques
Low-level memory and file operations

Future Enhancements

Password-protected message extraction
Support for larger file formats
Encryption before embedding
GUI-based interface
Support for hiding multiple files
