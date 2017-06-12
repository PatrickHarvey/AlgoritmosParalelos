#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include <cuda.h>
#include <math.h>
#include <fstream>

using namespace std;

#define T 2 // max threads x bloque

#define N 512

#define BLUR_SIZE 1


__global__
   void blurKernel(const char * in, unsigned char * out, int ancho, int altura) {
     int columna = blockIdx.x * blockDim.x + threadIdx.x;
     int fila = blockIdx.y * blockDim.y + threadIdx.y;
     if (columna < ancho && fila < altura) {
       int pixVal = 0;
       int pixels = 0;
       for(int blurRow = -BLUR_SIZE; blurRow < BLUR_SIZE+1; ++blurRow) {
         for(int blurCol = -BLUR_SIZE; blurCol < BLUR_SIZE+1; ++blurCol) {
           int curRow = fila + blurRow;
           int curCol = columna + blurCol;
           if(curRow > -1 && curRow < altura && curCol > -1 && curCol < ancho){
             pixVal += in[curRow * ancho + curCol];
             pixels++;
           }
         }
       }
       out[fila * ancho + columna] = (int)(pixVal / pixels);
    }
 }


int main(int argc, char** argv) {

  int entrada[512][512*3];
  int salida[512][512];

  unsigned char *m_entrada, *m_salida;

  const char * archivo_entrada="lena30.jpg";

  cudaMalloc((void**) m_entrada, N * N * 3 *sizeof(int));
  cudaMalloc((void**) m_salida, N * N * sizeof(int));

  cudaMemcpy(m_entrada, entrada, N * N * 3 * sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(m_salida, salida, N * N * sizeof(int), cudaMemcpyHostToDevice);

  int B = (int) ceil((float) N / (float) T);

  dim3 dimBloques(B, B);
  dim3 dimThreadsBloque(T, T);

  int ancho=N;
  int altura=N;
  blurKernel<<<dimBloques, dimThreadsBloque>>>(archivo_entrada, m_salida, ancho, altura);

  cudaMemcpy(salida, m_salida, N * N * sizeof(int), cudaMemcpyDeviceToHost);

  cudaFree(m_entrada);
  cudaFree(m_salida);

 ofstream ficheroSalida;
 ficheroSalida.open ("blur.jpg");
 ficheroSalida << m_salida;
 ficheroSalida.close();

        return 0;
}
