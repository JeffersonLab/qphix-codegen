#include <iostream>
#include <iomanip> 
#include <cmath>
using namespace std;

// FIXME - Include the correct header based on the arch.
#include <immintrin.h>

#if PRECISION == 1
#define FVecBaseType float
#define tol   (1.0e-6)
#else
#define FVecBaseType double
#define tol   (1.0e-13)
#endif


// Generated functions that use the compiler intrinsics
#include "intrinsic_fns/gen_setzero.h"
#include "intrinsic_fns/gen_add.h"
#include "intrinsic_fns/gen_sub.h"
#include "intrinsic_fns/gen_mul.h"
#include "intrinsic_fns/gen_fnMadd.h"
#include "intrinsic_fns/gen_fMadd.h"


// Test the add function
int test_setZero()
{
  FVecBaseType ret[VECLEN];
  FVecBaseType ret2[VECLEN];
  
  cout << setw(10) << left << "SetZero" << ": " ;
  // Call generated function
  testSetZeroGenerated(ret);

  // Do it manually
  for(int i=0; i < VECLEN; i++) { 
    ret2[i] = 0.0;
  }
  
  for(int i=0; i < VECLEN; i++) { 
    double diff = ret2[i]-ret[i];
    double rel_diff = diff/ret[2];
    if( fabs(rel_diff) > tol ) { 
      std::cout << "FAIL: i= " << i << " desired = " 
                << ret2[i] << " generated=" << ret[i] << endl;
      return 0;
    }
  }
 
  std::cout << "PASS" << endl;
  return 1;
}

// Test the add function
int test_add()
{
  FVecBaseType a[VECLEN];
  FVecBaseType b[VECLEN];
  FVecBaseType ret[VECLEN];
  FVecBaseType ret2[VECLEN];
  
  for(int i=0; i < VECLEN; i++) {
    a[i] = (FVecBaseType)(i+1);
    b[i] = (FVecBaseType)(VECLEN*i+2);
  }
  
  cout << setw(10) << left << "Add" << ": ";
  // Call generated function
  testAddGenerated(ret,a,b);

  // Do it manually
  for(int i=0; i < VECLEN; i++) { 
    ret2[i] = a[i] + b[i];
  }
  
  for(int i=0; i < VECLEN; i++) { 
    double diff = ret2[i]-ret[i];
    double rel_diff = diff/ret[2];
    if( fabs(rel_diff) > tol ) { 
      std::cout << "FAIL: i= " << i << " desired = " 
                << ret2[i] << " generated=" << ret[i] << endl;
      return 0;
    }
  }
 
  std::cout << "PASS" << left << endl;
  return 1;
}

// Test the add function
int test_sub()
{
  FVecBaseType a[VECLEN];
  FVecBaseType b[VECLEN];
  FVecBaseType ret[VECLEN];
  FVecBaseType ret2[VECLEN];
  
  for(int i=0; i < VECLEN; i++) {
    a[i] = (FVecBaseType)(i+1);
    b[i] = (FVecBaseType)(VECLEN*i+2);
  }
  
  cout << setw(10) << left << "Sub" << ": " ;
  // Call generated function
  testSubGenerated(ret,a,b);

  // Do it manually
  for(int i=0; i < VECLEN; i++) { 
    ret2[i] = a[i] - b[i];
  }
  
  for(int i=0; i < VECLEN; i++) { 
    double diff = ret2[i]-ret[i];
    double rel_diff = diff/ret[2];
    if( fabs(rel_diff) > tol ) { 
      std::cout << "FAIL: i= " << i << " desired = " 
                << ret2[i] << " generated=" << ret[i] << endl;
      return 0;
    }
  }
 
  std::cout << "PASS" << endl;
  return 1;
}

// Test the add function
int test_mul()
{
  FVecBaseType a[VECLEN];
  FVecBaseType b[VECLEN];
  FVecBaseType ret[VECLEN];
  FVecBaseType ret2[VECLEN];
  
  for(int i=0; i < VECLEN; i++) {
    a[i] = (FVecBaseType)(i+1);
    b[i] = (FVecBaseType)(VECLEN*i+2);
  }
  
  cout << setw(10) << left << "Mul" << ": " ;
  // Call generated function
  testMulGenerated(ret,a,b);

  // Do it manually
  for(int i=0; i < VECLEN; i++) { 
    ret2[i] = a[i] * b[i];
  }
  
  for(int i=0; i < VECLEN; i++) { 
    double diff = ret2[i]-ret[i];
    double rel_diff = diff/ret[2];
    if( fabs(rel_diff) > tol ) { 
      std::cout << "FAIL: i= " << i << " desired = " 
                << ret2[i] << " generated=" << ret[i] << endl;
      return 0;
    }
  }
 
  std::cout << "PASS" << endl;
  return 1;
}


// Testing the fnMadd function
int test_fnMadd()
{
  FVecBaseType a[VECLEN];
  FVecBaseType b[VECLEN];
  FVecBaseType c[VECLEN];
  FVecBaseType ret[VECLEN];
  FVecBaseType ret2[VECLEN];

  cout << setw(10) << left << "FnMAdd" << ": " ;

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
      std::cout << "FAIL: i= " << i << " desired = " 
                << ret2[i] << " generated=" << ret[i] << endl;
      return 0;
    }
  }
 
  std::cout << "PASS" << endl;
  return 1;
}


// Testing the fnMadd function
int test_fMadd()
{
  FVecBaseType a[VECLEN];
  FVecBaseType b[VECLEN];
  FVecBaseType c[VECLEN];
  FVecBaseType ret[VECLEN];
  FVecBaseType ret2[VECLEN];

  cout << setw(10) << left << "FMAdd" << ": " ;

  for(int i=0; i < VECLEN; i++) {
    a[i] = (FVecBaseType)(i+1);
    b[i] = (FVecBaseType)(VECLEN*i+2);
    c[i] = (FVecBaseType)(2*VECLEN*i+3);
  }
  
  // Call generated function
  testFMaddGenerated(ret,a,b,c);

  // Do it manually
  for(int i=0; i < VECLEN; i++) { 
    ret2[i] = c[i] + (a[i]*b[i]);
  }
  
  for(int i=0; i < VECLEN; i++) { 
    double diff = ret2[i]-ret[i];
    double rel_diff = diff/ret[2];
    if( fabs(rel_diff) > tol ) { 
      std::cout << "FAIL: i= " << i << " desired = " 
                << ret2[i] << " generated=" << ret[i] << endl;
      return 0;
    }
  }
 
  std::cout << "PASS" << endl;
  return 1;
}

int main(int argc, char *argv[]) 
{
  test_setZero();
  test_add();
  test_sub();
  test_mul();
  test_fnMadd();
  test_fMadd();

  // if ( ! test_fnMadd() ) { 
  //    std::cout << "TEST FAILED" << endl; abort(); 
  //}
}
