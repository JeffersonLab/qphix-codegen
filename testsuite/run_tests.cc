#include <iostream>
#include <cmath>
using namespace std;


#include <immintrin.h>

#if PRECISION == 1
#define FVecBaseType float
#define tol   (1.0e-6)
#else
#define FVecBaseType double
#define tol   (1.0e-13)
#endif



#include "test_gen_fnMadd.h"

int test_fnMadd()
{
  FVecBaseType a[VECLEN];
  FVecBaseType b[VECLEN];
  FVecBaseType c[VECLEN];
  FVecBaseType ret[VECLEN];
  FVecBaseType ret2[VECLEN];
  
  for(int i=0; i < VECLEN; i++) {
    a[i] = (FVecBaseType)(i+1);
    b[i] = (FVecBaseType)(VECLEN*i+2);
    c[i] = (FVecBaseType)(2*VECLEN*i+3);
  }
  
  // Call generated function
  testFnMaddGenerated(ret,a,b,c);

  // Do it manually
  for(int i=0; i < VECLEN; i++) { 
    ret2[i] = c[i] - (a[i]*b[i]);
  }
  
  for(int i=0; i < VECLEN; i++) { 
    double diff = ret2[i]-ret[i];
    double rel_diff = diff/ret[2];
    if( fabs(rel_diff) > tol ) { 
      std::cout << "FAIL: i= " << i << " desired = " << ret2[i] << " generated=" << ret[i] << endl;
      return 0;
    }
  }
 
  std::cout << "PASS" << endl;
  return 1;
}


int main(int argc, char *argv[]) 
{
  if ( ! test_fnMadd() ) { std::cout << "TEST FAILED" << endl; abort(); }
}
