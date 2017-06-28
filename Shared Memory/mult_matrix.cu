#define TILE_WIDTH 16
#include <iostream>

using namespace std;

__global__ void matrixMultiply(float * A, float * B, float * C, int tam)
{
    __shared__ float ds_A[TILE_WIDTH][TILE_WIDTH];
    __shared__ float ds_B[TILE_WIDTH][TILE_WIDTH];

    int bx = blockIdx.x, by = blockIdx.y, tx = threadIdx.x, ty = threadIdx.y;
    int Row = by * TILE_WIDTH + ty, Col = bx * TILE_WIDTH + tx;
    float Pvalue = 0;

    for (int ph = 0; ph < tam/TILE_WIDTH; ++ph)
    {
	ds_A[ty][tx] = A[Row*tam + ph*TILE_WIDTH + tx];

	ds_B[ty][tx] = B[(ph*TILE_WIDTH + ty)*tam + Col];

	__syncthreads();

	for (int k = 0; k < TILE_WIDTH; ++k)
            {
	    Pvalue += ds_A[ty][k] * ds_B[k][tx];
	    }

	__syncthreads();
     }
C[Row*tam + Col] = Pvalue;
}

int main(int argc, char ** argv) {

    float * hostA; // The A matrix
    float * hostB; // The B matrix
    float * hostC; // The output C matrix
    float * deviceA;
    float * deviceB;
    float * deviceC;

    int tam=16;

    hostA = (float *)malloc(sizeof(float) * tam * tam);
    hostB = (float *)malloc(sizeof(float) * tam * tam);
    hostC = (float *)malloc(sizeof(float) * tam * tam);

    

    for(int i=0;i<tam;i++)
	{
	hostA[i]=i;
	hostB[i]=tam-i;
	}

for(int j=0;j<tam;j++)
	cout << hostA[j] << " ";
cout << endl;
for(int k=0;k<tam;k++)
	cout << hostB[k] << " ";
cout << endl;
    cudaMalloc(&deviceA, sizeof(float) * tam * tam);
    cudaMalloc(&deviceB, sizeof(float) * tam * tam);
    cudaMalloc(&deviceC, sizeof(float) * tam * tam);

    cudaMemcpy(deviceA, hostA, sizeof(float) * tam * tam, cudaMemcpyHostToDevice);
    cudaMemcpy(deviceB, hostB, sizeof(float) * tam * tam, cudaMemcpyHostToDevice);

    dim3 dimGrid((tam-1)/TILE_WIDTH+1, (tam-1)/TILE_WIDTH+1, 1);
    dim3 dimBlock(TILE_WIDTH, TILE_WIDTH, 1);

    matrixMultiply<<<dimGrid, dimBlock>>>(deviceA, deviceB, deviceC, tam);

    cudaThreadSynchronize();

    cudaMemcpy(hostC, deviceC, sizeof(float) * tam * tam, cudaMemcpyDeviceToHost);

    for(int k=0;k<tam;k++)
	cout << hostC[k] << " ";
    cout << endl;

    cudaFree(deviceA);
    cudaFree(deviceB);
    cudaFree(deviceC);


    free(hostA);
    free(hostB);
    free(hostC);

    return 0;
}
