# Conway's Game of Life - OpenCL Implementation

## Overview
This project implements Conway's Game of Life using OpenCL to leverage GPU acceleration for enhanced performance. It's loading its intial state from the image.

## What is Conway's Game of Life?
Conway's Game of Life, devised by mathematician John Horton Conway in 1970, is a cellular automaton. It is a zero-player game, meaning that its evolution is determined by its initial state, with no further input required. The game consists of a grid of cells, each of which can be in one of two states: alive or dead.

### How It Works
The state of the cells evolves over discrete time steps according to a set of simple rules:

1. **Underpopulation:** A living cell with fewer than two living neighbors dies.
2. **Overpopulation:** A living cell with more than three living neighbors dies.
3. **Survival:** A living cell with two or three living neighbors continues to live.
4. **Reproduction:** A dead cell with exactly three living neighbors becomes a living cell.

These rules are applied simultaneously to every cell in the grid, resulting in the next generation of the grid. The game creates complex patterns and behaviors from these simple rules, illustrating how complexity can arise from simplicity.

## OpenCL and GPU Acceleration
OpenCL (Open Computing Language) is a framework for writing programs that execute across heterogeneous platforms, including CPUs, GPUs, and other processors.

### Key Features of OpenCL:
- **Cross-Platform:** Write code once and run it on multiple types of hardware.
- **Parallel Execution:** Exploit data parallelism and task parallelism to improve performance.
- **Flexibility:** Combine different types of processors in a single application.

## Prerequisites
To run this project, you need to have OpenCL installed on your system. This usually involves installing the appropriate drivers for your graphics card. Below are steps for different platforms:

### Windows
1. Install the latest drivers for your GPU from the manufacturer's website (NVIDIA, AMD, or Intel).
2. Download and install the OpenCL SDK corresponding to your GPU.

### Linux
1. Install the appropriate drivers for your GPU. This can often be done through your package manager. For example, for NVIDIA GPUs, you might use:
   ```bash
   sudo apt-get install nvidia-driver-<version>
   ```
2. Install OpenCL ICD (Installable Client Driver) loader, typically available via your package manager.

### macOS
1. Install OpenCL ICD (Installable Client Driver) loader, typically available via your package manager.