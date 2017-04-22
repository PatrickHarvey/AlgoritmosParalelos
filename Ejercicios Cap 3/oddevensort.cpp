#include <iostream>
#include <pthread.h>

using namespace std;

int main()
{
int * vector;
int n=10;

for(int i=0;i<n;i++)
  vector[i]=n-i;

int exch=1, start=0, i;
int tmp;

while(exch||start)
{
exch=0;

#pragma omp parallel for private(tmp)
  for(i = start;i<n-1;i+=2)
   {
    if(vector[i]>vector[i+1])
      {
       tmp= vector[i];
       vector[i] = vector[i+1];
       vector[i+1]= tmp;
       exch=1;
      }
   }

if(start==0)
   start = 1;
else
   start = 0;
}

for(int i=0;i<n;i++)
 cout << vector[i] << " " << endl;

return 0;
}
