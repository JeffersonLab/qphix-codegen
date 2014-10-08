#include <iostream>
#include <iomanip> 
#include <cmath>
using namespace std;

#include "run_tests.h"

#if ARCH  == avx || ARCH == avx2
#include <immintrin.h>
#include "avx_utils.h"
#elif ARCH == qpx
#include "qpx_utils.h"
#endif

#if PRECISION == 1
#define FVecBaseType float
#define tol   (1.0e-6)
#else
#define FVecBaseType double
#define tol   (1.0e-13)
#endif

/* Generated functions that use the compiler intrinsics. */
#include "test_fns/gen_setzero.h"
#include "test_fns/gen_add.h"
#include "test_fns/gen_sub.h"
#include "test_fns/gen_mul.h"
#include "test_fns/gen_fnMadd.h"
#include "test_fns/gen_fMadd.h"


/* Define the possible mask values */
//const int RunTests::MASKS[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

#define MASK 7
// Testing the load-store
int 
RunTests::testLoadStore(bool withMask=false)
{
// TODO
}

// Test the add function
int 
RunTests::testSetZero()
{
  FVecBaseType ret[VECLEN];
  FVecBaseType ret2[VECLEN];
  
  cout << setw(18) << left << "SetZero" << ": " ;
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
int 
RunTests::testAdd(bool withMask=false)
{
  FVecBaseType a[VECLEN];
  FVecBaseType b[VECLEN];
  FVecBaseType ret[VECLEN];
  FVecBaseType ret2[VECLEN];
  
  for(int i=0; i < VECLEN; i++) {
    a[i] = (FVecBaseType)(i+1);
    b[i] = (FVecBaseType)(VECLEN*i+2);
  }
  
  // Call generated function
  if(withMask) {
    cout << setw(8) << left << "Add" << setw(10) << "(masked)" << ": " ;    

    for(int msk =0; msk < RunTests::NUM_MASKS; ++msk) {
        testMaskedAddGenerated(ret, a, b, msk);
        for(int i=0; i < VECLEN; ++i) { 
            ret2[i] = (msk & 1 << i ) > 0 ? a[i] + b[i] : 0;
        }
        
        /* Check if the results match */
        for(int i=0; i < VECLEN; i++) { 
            double diff = ret2[i]-ret[i];
            double rel_diff = diff/ret[2];
            if( fabs(rel_diff) > tol ) { 
                std::cout << "FAIL: For mask value " << msk << "i= " << i 
                          << " desired = " << ret2[i] 
                          << " generated=" << ret[i] << endl;
                return 0;
            }
        }
    }    
  }
  else {
    cout << setw(8) << left << "Add" << setw(10) <<"(no mask)" << ": " ;    
    testAddGenerated(ret,a,b);
    // Do it manually
    for(int i=0; i < VECLEN; i++) { 
        ret2[i] = a[i] + b[i];
    }
    /* Check if the results match */
    for(int i=0; i < VECLEN; i++) { 
        double diff = ret2[i]-ret[i];
        double rel_diff = diff/ret[2];
        if( fabs(rel_diff) > tol ) { 
            std::cout << "FAIL: i= " << i << " desired = " << ret2[i] 
                      << " generated=" << ret[i] << endl;
            return 0;
        }
    }
  }  
  std::cout << "PASS" << left << endl;
  return 1;
}

// Test the sub function
int 
RunTests::testSub(bool withMask=false)
{
  FVecBaseType a[VECLEN];
  FVecBaseType b[VECLEN];
  FVecBaseType ret[VECLEN];
  FVecBaseType ret2[VECLEN];
  
  for(int i=0; i < VECLEN; i++) {
    a[i] = (FVecBaseType)(i+1);
    b[i] = (FVecBaseType)(VECLEN*i+2);
  }

  // Call generated function
  if(withMask) {
    cout << setw(8) << left << "Sub" << setw(10) << "(masked)" << ": " ;    
    testMaskedSubGenerated(ret, a, b, MASK);
    // Do it manually
    for(int i=0; i < VECLEN; ++i) { 
      ret2[i] = (MASK & 1 << i ) > 0 ? a[i] - b[i] : 0;
    }
  }
  else {
    cout << setw(8) << left << "Sub" << setw(10) <<"(no mask)" << ": " ;    
    testSubGenerated(ret,a,b);
    // Do it manually
    for(int i=0; i < VECLEN; i++) { 
        ret2[i] = a[i] - b[i];
    }
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
int 
RunTests::testMul(bool withMask=false)
{
  FVecBaseType a[VECLEN];
  FVecBaseType b[VECLEN];
  FVecBaseType ret[VECLEN];
  FVecBaseType ret2[VECLEN];
  
  for(int i=0; i < VECLEN; i++) {
    a[i] = (FVecBaseType)(i+1);
    b[i] = (FVecBaseType)(VECLEN*i+2);
  }
 
  // Call generated function
  if(withMask) {
    cout << setw(8) << left << "Mul" << setw(10) << "(masked)" << ": " ;    
    testMaskedMulGenerated(ret, a, b, MASK);
    // Do it manually
    for(int i=0; i < VECLEN; ++i) { 
      ret2[i] = (MASK & 1 << i ) > 0 ? a[i] * b[i] : 0;
    }
  }
  else {
    cout << setw(8) << left << "Mul" << setw(10) <<"(no mask)" << ": " ;    
    testMulGenerated(ret,a,b);
    // Do it manually
    for(int i=0; i < VECLEN; i++) { 
        ret2[i] = a[i] * b[i];
    }
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
int 
RunTests::testFnMadd(bool withMask=false)
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
  if(withMask) {
    cout << setw(8) << left << "FnMAdd" << setw(10) << "(masked)" << ": " ;    
    testMaskedFnMaddGenerated(ret,a,b,c, MASK);
    // Do it manually
    for(int i=0; i < VECLEN; ++i) { 
      ret2[i] = (MASK & 1 << i ) > 0 ? c[i] - (a[i]*b[i]) : 0;
    }
  }
  else {
    cout << setw(8) << left << "FnMAdd" << setw(10) << "(no mask)" << ": " ;    
    testFnMaddGenerated(ret,a,b,c);
    // Do it manually
    for(int i=0; i < VECLEN; i++) { 
      ret2[i] = c[i] - (a[i]*b[i]);
    }
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
int 
RunTests::testFMadd(bool withMask=false)
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
  if(withMask) {
    cout << setw(8) << left << "FMAdd" << setw(10) << "(masked)" << ": " ;    
    testMaskedFMaddGenerated(ret,a,b,c, MASK);
    // Do it manually
    for(int i=0; i < VECLEN; ++i) { 
      ret2[i] = (MASK & 1 << i ) > 0 ? c[i] + (a[i]*b[i]) : 0;
    }
  }
  else {
    cout << setw(8) << left << "FMAdd" << setw(10) << "(no mask)" << ": " ;    
    testFMaddGenerated(ret,a,b,c);
    // Do it manually
    for(int i=0; i < VECLEN; i++) { 
      ret2[i] = c[i] + (a[i]*b[i]);
    }
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
  RunTests runTest;
  runTest.testSetZero();
  runTest.testAdd(false);
  runTest.testAdd(true);
  runTest.testSub(false);
  runTest.testSub(true);
  runTest.testMul(false);
  runTest.testMul(true);  
  runTest.testFnMadd(false);
  runTest.testFnMadd(true);
  runTest.testFMadd(false);
  runTest.testFMadd(true);  
}
