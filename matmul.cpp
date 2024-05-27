#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <CL/cl.h>

#define MATRIX_DIM 1024
#define MATRIX_SIZE (MATRIX_DIM*MATRIX_DIM)
#define MATRIX_SIZE_BYTES (MATRIX_SIZE * sizeof(int))
int mA[MATRIX_SIZE] = { 0 };
int mB[MATRIX_SIZE] = { 0 };
int mC[MATRIX_SIZE] = { 0 };

char* readKernelSource(const char* filename)
{
	char* kernelSource = nullptr;
	long length;
	FILE* f = fopen(filename, "r");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		kernelSource = (char*)calloc(length, sizeof(char));
		if (kernelSource)
			fread(kernelSource, 1, length, f);
		fclose(f);
	}
	return kernelSource;
}


void runMatrixMultiplication()
{
	clock_t start, diff;
	int msec;

	start = clock();

	// Device input buffers
	cl_mem d_a;
	cl_mem d_b;

	// Device output buffer
	cl_mem d_c;

	cl_platform_id cpPlatform;        // OpenCL platform
	cl_device_id device_id;           // device ID
	cl_context context;               // context
	cl_command_queue queue;           // command queue
	cl_program program;               // program
	cl_kernel kernel;                 // kernel

	size_t globalSize[2], localSize[2];
	cl_int err;

	// Number of work items in each local work group
	localSize[0] = localSize[1] = 16;

	// Number of total work items - localSize must be devisor
	globalSize[0] = 1024;
	globalSize[1] = 1024;

	// Bind to platform
	err = clGetPlatformIDs(1, &cpPlatform, NULL);

	// Get ID for the device
	err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);

	// Create a context  
	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);

	// Create a command queue 
	queue = clCreateCommandQueue(context, device_id, 0, &err);

	char* kernelSource = readKernelSource("MatMul.cl");

	// Create the compute program from the source buffer
	program = clCreateProgramWithSource(context, 1, (const char**)&kernelSource, NULL, &err);

	// Build the program executable 
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

	if (err)
	{
		// Determine the size of the log
		size_t log_size;
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

		// Allocate memory for the log
		char* log = (char*)malloc(log_size);

		// Get the log
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

		// Print the log
		printf("%s\n", log);

		free(log);
	}

	// Create the compute kernel in the program we wish to run
	kernel = clCreateKernel(program, "multMatrices", &err);

	size_t kernelWorkGroupSize = 0;
	clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &kernelWorkGroupSize, nullptr);

	// Create the input and output arrays in device memory for our calculation
	d_a = clCreateBuffer(context, CL_MEM_READ_ONLY, MATRIX_SIZE_BYTES, NULL, NULL);
	d_b = clCreateBuffer(context, CL_MEM_READ_ONLY, MATRIX_SIZE_BYTES, NULL, NULL);
	d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, MATRIX_SIZE_BYTES, NULL, NULL);

	// Write our data set into the input array in device memory
	err = clEnqueueWriteBuffer(queue, d_a, CL_TRUE, 0, MATRIX_SIZE_BYTES, mA, 0, NULL, NULL);
	err |= clEnqueueWriteBuffer(queue, d_b, CL_TRUE, 0, MATRIX_SIZE_BYTES, mB, 0, NULL, NULL);

	int matrixDim = MATRIX_DIM;

	// Set the arguments to our compute kernel
	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
	err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
	err |= clSetKernelArg(kernel, 3, sizeof(int), &matrixDim);

	diff = clock() - start;
	msec = diff * 1000 / CLOCKS_PER_SEC;
	printf("1) %ds%dms\n", msec / 1000, msec % 1000);

	start = clock();

	// Execute the kernel over the entire range of the data set  
	err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, localSize, 0, NULL, NULL);

	// Wait for the command queue to get serviced before reading back results
	clFinish(queue);

	diff = clock() - start;
	msec = diff * 1000 / CLOCKS_PER_SEC;
	printf("2) %ds%dms\n", msec / 1000, msec % 1000);

	start = clock();

	// Read the results from the device
	clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0, MATRIX_SIZE_BYTES, mC, 0, NULL, NULL);

	clFinish(queue);

	// release OpenCL resources
	clReleaseMemObject(d_a);
	clReleaseMemObject(d_b);
	clReleaseMemObject(d_c);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	//release host memory
	free(kernelSource);

	diff = clock() - start;
	msec = diff * 1000 / CLOCKS_PER_SEC;
	printf("3) %ds%dms\n", msec / 1000, msec % 1000);
}

int main2(int argc, char* argv[])
{
	FILE* fpA;
	FILE* fpB;

	fpA = fopen("matA", "r");
	fpB = fopen("matB", "r");

	if ((fpA == NULL) || (fpB == NULL))
	{
		printf("Error opening necessary files.");
		return -1;
	}

	int successA = 0;
	int successB = 0;
	for (int i = 0; i < MATRIX_SIZE; i++)
	{
		successA = fscanf(fpA, "%d", &mA[i]);
		successB = fscanf(fpB, "%d", &mB[i]);
	}

	fclose(fpA);
	fclose(fpB);

	runMatrixMultiplication();

	FILE* fpC;
	fpC = fopen("matC", "w");

	if (fpC == NULL)
	{
		printf("Error opening necessary files.");
		return -1;
	}

	for (int i = 0; i < MATRIX_DIM; i++)
	{
		for (int j = 0; j < MATRIX_DIM; j++)
		{
			fprintf(fpC, "%d", mC[i * MATRIX_DIM + j]);
			if (j != MATRIX_DIM - 1)
			{
				fprintf(fpC, " ");
			}
		}
		fprintf(fpC, "\n");
	}
	fclose(fpC);

	return 0;
}