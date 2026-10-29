# Compressor

A file compression and decompression tool written in C++ using Huffman Coding.

## Features

- Compress files using Huffman Coding.
- Decompress previously compressed files.
- Store the Huffman tree in the compressed file.
- Write encoded data as bits instead of storing each code as a separate string.
- Support binary file input and output.

## Requirements

- C++ compiler with C++20 support.
- CMake.
- Make.

## Build

Clone the repository and build the project:

```bash
git clone https://github.com/youssef-elbassiuny/compreesor.git
cd compressor

cmake -S . -B build
cmake --build build
```

## Usage

### Compress a file

```bash
./build/compressor input.txt encode
```

This generates `output.bin` in the current working directory.

### Decompress a file

```bash
./build/compressor output.bin decode
```

This generates `decoded.txt` in the current working directory.

## Testing

To verify that compression and decompression preserve the original file:

```bash
cmp input.txt decoded.txt
```

If the files are identical, `cmp` exits successfully without printing anything.

## How It Works

1. Count the frequency of each byte in the input file.
2. Build a Huffman tree using a priority queue.
3. Generate a binary code for each byte.
4. Serialize the Huffman tree and encode the input data.
5. During decompression, reconstruct the tree and decode the original data.

## License

Add a license if you intend to distribute this project as open source.
