#include <iostream>

using namespace std;

void matriz()
{
int tam=200;
int s=200;
int a[tam][tam],b[tam][tam],c[tam][tam];

for(int j=0;j<tam;j+= s){
        for(int k=0;k<tam;k+= s){
                for(int i=0;i<tam;i++){
                        for(int j = j; j<((j+s)>tam?tam:(j+s)); j++){
                                int temp = 0;
                                for(int k = k; k<((k+s)>tam?tam:(k+s)); k++){
                                        temp += a[i][k]*b[k][j];
                                }
                                c[i][j] += temp;
                        }
                }
        }
}
}




int main()
{
matriz();
return 0;
}
