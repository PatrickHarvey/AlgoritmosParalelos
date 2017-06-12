#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include <cuda.h>
#include <math.h>
#include <string>
#include <fstream>

using namespace std;

#define T 2 
#define N 512

#define CHANNELS 3

__global__
void colorToGray(const char * rgbImage, unsigned char * grayImage, int ancho, int altura)
{
    int columna = threadIdx.x + blockIdx.x * blockDim.x;
    int fila    = threadIdx.y + blockIdx.y * blockDim.y;

    if (columna < ancho && fila < altura)
     {

      int greyOffset = fila*(ancho) + columna;
      int rgbOffset = greyOffset * CHANNELS;
        unsigned char r = rgbImage[rgbOffset];
        unsigned char g = rgbImage[rgbOffset + 2];
        unsigned char b = rgbImage[rgbOffset + 3];
      grayImage[greyOffset] = 0.21f*r + 0.71f*g + 0.07f*b;
    }
}

int main(int argc, char** argv) {

  int entrada[512][512*3];
  int salida[512][512];

  unsigned char *m_entrada, *m_salida;
  
  const char * archivo_entrada="lena30.jpg";

  cudaMalloc((void**) &m_entrada, N * CHANNELS * N * sizeof(int));
  cudaMalloc((void**) &m_salida, N * N * sizeof(int));

  cudaMemcpy(m_entrada, entrada, N * CHANNELS * N * sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(m_salida, salida, N * N * sizeof(int), cudaMemcpyHostToDevice);

  int B = (int) ceil((float) N / (float) T);

  dim3 dimBloques(B, B);
  dim3 dimThreadsBloque(T, T);

  int ancho=N;
  int altura=N;
  colorToGray<<<dimBloques, dimThreadsBloque>>>(archivo_entrada, m_salida, ancho, altura);

  cudaMemcpy(salida, m_salida, N * N * sizeof(int), cudaMemcpyDeviceToHost);

  cudaFree(m_entrada);
  cudaFree(m_salida);

 ofstream ficheroSalida;
 ficheroSalida.open ("gris.jpg");
 ficheroSalida << m_salida;
 ficheroSalida.close();
 
 return 0;
}
