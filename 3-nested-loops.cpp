#include <iostream>

using namespace std;

int main()
{
int tam=200;
int a[tam][tam],b[tam][tam],c[tam][tam];

for(int i=0;i<tam;i++)
for(int j=0;j<tam;j++)
   {
    a[i][j]=i;
    b[i][j]=i;
   }

for (int i = 0; i < tam; i++)
      for (int k = 0; k < tam; k++)
          for (int j = 0; j < tam; j++)
              c[i][j] = c[i][j] + a[i][k]*b[k][j];

return 0;
}
