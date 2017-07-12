#define N 512
#define TILE_WIDTH 16
#include <stdio.h>
#include <iostream>

using namespace std;


__global__ void matrix_mult(int* A, int* B, int* C, int ancho)
{
int tmp = 0;
 int columna = blockIdx.x*TILE_WIDTH + threadIdx.x;
 int fila = blockIdx.y*TILE_WIDTH + threadIdx.y;
 if(columna < ancho && fila < ancho) {
  for (int k = 0; k < ancho; k++)
   tmp += A[fila * ancho + k] * B[k * ancho + columna];
  C[fila * ancho + columna] = tmp;
 }
}

int main() {
 int a[N][N], b[N][N], c[N][N];

 int *dev_a, *dev_b, *dev_c;

 int size = N * N * sizeof(int);

 // initialize a and b matrices here
 cudaMalloc((void **) &dev_a, size);
 cudaMalloc((void **) &dev_b, size);
 cudaMalloc((void **) &dev_c, size);

 cudaMemcpy(dev_a, a, size, cudaMemcpyHostToDevice);
 cudaMemcpy(dev_b, b, size, cudaMemcpyHostToDevice);

 dim3 dimBlock(TILE_WIDTH, TILE_WIDTH);
 dim3 dimGrid((int)ceil(N/dimBlock.x), (int)ceil(N/dimBlock.y));

 matrix_mult<<<dimGrid, dimBlock>>>(dev_a, dev_b, dev_c, N);

 cudaMemcpy(c, dev_c, size, cudaMemcpyDeviceToHost);

 cudaFree(dev_a);
 cudaFree(dev_b);
 cudaFree(dev_c);
}

