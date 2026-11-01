# High-Capacity Reversible Data Hiding in Encrypted Images via Dynamic Queue Simulation and Enhanced Median Edge Detector

Official C++ implementation, LaTeX sources, and experimental evaluation testbed for the reversible data hiding in encrypted images research:

> **A High Capacity Reversible Data Hiding in Encrypted Images Using Multi-Most Significant Bit Prediction and Huffman Coding**  
> *Li-Chiun Lin, Cheng-Ta Huang, and Shiuh-Jeng Wang*  
> *Security and Information Technologies with AI, Internet Computing and Big-data Applications*, Smart Innovation, Systems and Technologies, vol. 314, Springer, Singapore, pp. 36–44, 2023.  
> DOI: 10.1007/978-981-19-5733-8_4

---

## Overview

Reversible data hiding in encrypted images enables secret payload embedding within encrypted visual media while guaranteeing exact, lossless restoration of the original cover container upon decryption and extraction. Conventional Reserving Room Before Encryption (RRBE) schemes exploit spatial pixel correlation but often face trade-offs between reserve overhead, predictor accuracy, and queue underrun risks during extraction.

This repository provides the complete C++ research testbed for high-capacity RRBE reversible data hiding. The architecture features:
1. **Adaptive Predictor Selection:** Dynamic candidate predictor evaluation (Median Edge Detector, average predictor, gradient-adjusted predictor, and modified Median Edge Detector) to maximize embedding capacity with zero per-interval header overhead.
2. **Two-Phase Dynamic Queue Simulation:** Provably sufficient minimum reserve length computation to prevent queue underruns during extraction, providing a formal guarantee of lossless reconstruction under causal prediction.
3. **Multi-Most Significant Bit and Huffman Coding:** Entropy-optimized prediction error representation minimizing auxiliary label map overhead.

Benchmark evaluations across the standard University of Southern California Signal and Image Processing Institute image dataset, BOSSBase, and BOWS-2 databases demonstrate average embedding rates exceeding 3.2 bits per pixel with byte-exact cover image recovery.

---

## Key Features

- **Reversible Room Reservation:** Implements causal pre-encryption room reservation with dynamic bitstream serialization.
- **Enhanced Spatial Predictors:** C++ implementations of the Median Edge Detector, Gradient-Adjusted Predictor, and directional difference models.
- **Huffman Entropy Encoding:** Custom prefix tree serialization and deserialization routines tailored for multi-bit prediction residuals.
- **Full Verification Suite:** Automated pixel-exact recovery verification, Peak Signal-to-Noise Ratio (PSNR), and embedding capacity evaluation across standard 8-bit grayscale image corpora.

---

## Repository Structure

```text
shorter_huffman_code/
├── msb_huffman/                        # Core C++ prediction and Huffman embedding engine
│   ├── msb_huffman.cpp                 # Main reversible room reservation routines
│   └── msb_huffman.vcxproj             # Visual Studio project file
├── shorter_huffman/                    # Dynamic queue simulation and queue length optimization
│   ├── shorter_huffman.cpp             # Queue simulation and reserve validation
│   └── shorter_huffman.vcxproj         # Visual Studio project file
├── lib/                                # Shared image processing and utility primitives
│   ├── lib.cpp / .h                    # Core buffer management and bitstream serializers
│   └── lib.vcxproj                     # Static library project
├── test/                               # Comprehensive experimental test runner
│   ├── test.cpp                        # Batch evaluation across benchmark image datasets
│   └── test.vcxproj                    # Test harness project
├── article/                            # LaTeX manuscript source and evaluation tables
├── SITAIBA2021_/                       # Conference manuscript LaTeX sources
├── authorkit_SITAIBA2021/              # Publisher author guidelines and formatting templates
├── tiff-4.3.0/                         # LibTIFF source distribution
└── shorter_huffman.sln                 # Master Visual Studio solution file
```

---

## Build Prerequisites

- **C++ Compiler:** Microsoft Visual C++ 2019/2022 (Release x64 configuration) or standard C++17 compiler on Linux.
- **LibTIFF:** Grayscale Tagged Image File Format input/output support (bundled in `tiff-4.3.0/`).

---

## Building and Running

### Building with Microsoft Visual Studio
1. Open `shorter_huffman.sln` in Visual Studio.
2. Select configuration **Release** and platform **x64**.
3. Build the solution. The test runner binary will be generated under `x64/Release/`.

### Running Experimental Tests
```bash
# Execute batch evaluation across the benchmark image suite
./test
```
The runner evaluates embedding capacities across test sets, verifies byte-exact reconstruction, and records bit-per-pixel rates and PSNR fidelity statistics.

---

## Citation

If you utilize this implementation or algorithmic models in your research, please cite:

```bibtex
@incollection{lin2023highcapacity,
  author    = {Lin, Li-Chiun and Huang, Cheng-Ta and Wang, Shiuh-Jeng},
  title     = {A High Capacity Reversible Data Hiding in Encrypted Images Using Multi-Most Significant Bit Prediction and Huffman Coding},
  booktitle = {Security and Information Technologies with AI, Internet Computing and Big-data Applications},
  series    = {Smart Innovation, Systems and Technologies},
  volume    = {314},
  pages     = {36--44},
  publisher = {Springer},
  year      = {2023},
  doi       = {10.1007/978-981-19-5733-8_4}
}
```
