CXX      = g++
NVCC     = nvcc
CXXFLAGS = -O3 -std=c++17 -Iinclude
NVCCFLAGS = -O3 -std=c++17 -Iinclude

COMMON_SRCS = src/mesh_generator.cpp src/image_maker.cpp

serial: $(COMMON_SRCS) main_serial.cpp
	$(CXX) $(CXXFLAGS) -o main_serial main_serial.cpp $(COMMON_SRCS)

cuda: $(COMMON_SRCS) main_cuda.cu
	$(NVCC) $(NVCCFLAGS) -o main_cuda main_cuda.cu $(COMMON_SRCS)

clean:
	rm -f main_serial main_cuda main_mpi_cuda

all: serial cuda mpi_cuda