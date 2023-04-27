# Chained Filter

Membership query is a fundamental problem across databases, networks and security. Although Bloom Filters and their variants, such as Bloomier Filters, have been considered as promising solutions, their effectiveness varies depending on scenarios. In this paper, we propose the chain rule theory which allows us to factorize a membership query problem into several sub-problems without losing information. Based on this theory, we present a simple yet space-efficient algorithm called \algoname{}, which combines different elementary filters to work together. Our evaluation results demonstrate that \algoname{} performs well in diverse applications: (1) it requires only 26\% additional space over the theoretical lower bound to encode an implicit static dictionary, (2) it requires only 0.22 additional bit per item over the theoretical lower bound for lossless data compression, (3) it reduces up to 31\% external memory access for Cuckoo Hashing, and (4) it reduces up to 36\% \texttt{P99} tail point query latency in RocksDB database.

## Descriptions
- `src` We provide the C++ code implementation of Algorithm 1 and Algorithm 2 in the paper in `src`, you can refer to this part of the code to help you understand the paper.
- `Static Dictionary` We provide the code related to Static Dictionary mentioned in Section 5.1 of the paper in this folder.
- `Random Access Huffman Coding` We provide the code related to Random Access Huffman Coding mentioned in Section 5.2 of the paper in this folder.
- `Self-Adaptive Hashing` We provide the code related to  Self-Adaptive Hashing mentioned in Section 5.3 of the paper in this folder.
## Requirements
g++, C++17, CMake: We implement all the codes with C++ and build them with g++ 7.5.0 (Ubuntu 7.5.0-6ubuntu2) and -O3 option.

## How to run
Under the folder of each task, you can run `make` and get an executable file `main`, and run `main` to get the experimental data in the paper.

We also write the generation of the data set in `main`, you can refer to `main.cpp`, if you need to test our algorithm on other data sets, you can also modify it in this file.