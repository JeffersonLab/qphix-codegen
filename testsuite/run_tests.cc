#include <iostream>
#include <iomanip> 
#include <cmath>
using namespace std;

#include "run_tests.h"

#ifdef QPX
#include "qpx_utils.h"
#else
#include <immintrin.h>
#include "avx_utils.h"
#endif

#if PRECISION == 1
#define FVecBaseType float
#define tol   (1.0e-6)
#else
#define FVecBaseType double
#define tol   (1.0e-13)
#endif

/* Generated functions that use the compiler intrinsics. */
#include "test_fns/gen_ld_st.h"
#include "test_fns/gen_setzero.h"
#include "test_fns/gen_add.h"
#include "test_fns/gen_sub.h"
#include "test_fns/gen_mul.h"
#include "test_fns/gen_fnMadd.h"
#include "test_fns/gen_fMadd.h"
#include "test_fns/gen_loadSplitSOAFVec.h"
#include "test_fns/gen_packFVec.h"

/** Testing the load-store */
int 
RunTests::testLoadStore(bool withMask=false)
{
    FVecBaseType ld[VECLEN];
    FVecBaseType st[VECLEN];
    
    for(int i=0; i < VECLEN; i++) {
        ld[i] = (FVecBaseType)(i+1);
    }   
     
    /* Call generated function */
    if(withMask) {
        cout << setw(10) << left << "Load/Store" << setw(10) << "(masked)" << ": " ;    

        for(unsigned int msk = 0; msk < RunTests::NUM_MASKS; ++msk) {
            testMaskedLoadStoreFVec(st, ld, msk);
            
            /* Do it manually */
            FVecBaseType chk[VECLEN];
            for (unsigned int i = 0; i < VECLEN; ++i) {
                chk[i] = (msk & 1 << i) > 0 ? ld[i] : 0;    
            }
            
            /* Check if the results match */
            for(int i = 0; i < VECLEN; i++) { 
                double diff = st[i] - chk[i];
                double rel_diff = diff/ld[2];
                if( fabs(rel_diff) > tol) { 
                    std::cout << "FAIL: For mask value : " << msk << " i = " << i 
                              << " desired = " << st[i] 
                              << " generated=" << chk[i] << endl;
                    return 0;
                }
            }
        }    
    }
    else {
        cout << setw(10) << left << "Load/Store" << setw(10) <<"(no mask)" << ": " ;    
        testLoadStoreFVec(st, ld);
        /* Check if the results match */
        for(int i = 0; i < VECLEN; i++) { 
            double diff = ld[i]-st[i];
            double rel_diff = diff/ld[2];
            if( fabs(rel_diff) > tol) { 
                std::cout << "FAIL: i= " << i 
                          << " desired = " << ld[i] 
                          << " generated=" << st[i] << endl;
                return 0;
            }
        }
    }  
    std::cout << "PASS" << left << endl;
    return 1;    
}

/** Test the add function */
int 
RunTests::testSetZero()
{
    FVecBaseType ret[VECLEN];
    FVecBaseType ret2[VECLEN];
    
    cout << setw(20) << left << "SetZero" << ": " ;
    /* Call generated function */
    testSetZeroGenerated(ret);

    /* Do it manually */
    for(int i=0; i < VECLEN; i++) { 
        ret2[i] = 0.0;
    }
    
    for(int i=0; i < VECLEN; i++) { 
        double diff = ret2[i]-ret[i];
        double rel_diff = diff/ret[2];
        if( fabs(rel_diff) > tol) { 
            std::cout << "FAIL: i= " << i << " desired = " 
                  << ret2[i] << " generated=" << ret[i] << endl;
            return 0;
        }
    }
 
    std::cout << "PASS" << endl;
    return 1;
}

/** Test the add function */
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
    
    /* Call generated function */
    if(withMask) {
        cout << setw(10) << left << "Add" << setw(10) << "(masked)" << ": " ;    

        for(unsigned int msk = 0; msk < RunTests::NUM_MASKS; ++msk) {
            testMaskedAddGenerated(ret, a, b, msk);
            for(int i = 0; i < VECLEN; ++i) { 
                ret2[i] = (msk & 1 << i) > 0 ? a[i] + b[i] : 0;
            }
          
            /* Check if the results match */
            for(int i = 0; i < VECLEN; i++) { 
                double diff = ret2[i]-ret[i];
                double rel_diff = diff/ret[2];
                if( fabs(rel_diff) > tol) { 
                    std::cout << "FAIL: For mask value : " << msk << " i= " << i 
                              << " desired = " << ret2[i] 
                              << " generated=" << ret[i] << endl;
                    return 0;
                }
            }
        }    
    }
    else {
        cout << setw(10) << left << "Add" << setw(10) <<"(no mask)" << ": " ;    
        testAddGenerated(ret,a,b);
        /* Do it manually */
        for(int i = 0; i < VECLEN; i++) { 
            ret2[i] = a[i] + b[i];
        }
        /* Check if the results match */
        for(int i = 0; i < VECLEN; i++) { 
            double diff = ret2[i]-ret[i];
            double rel_diff = diff/ret[2];
            if( fabs(rel_diff) > tol) { 
                std::cout << "FAIL: i= " << i << " desired = " << ret2[i] 
                          << " generated=" << ret[i] << endl;
                return 0;
            }
        }
    }  
    std::cout << "PASS" << left << endl;
    return 1;
}

/** Test the sub function */
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

    /* Call generated function */
    if(withMask) {
        cout << setw(10) << left << "Sub" << setw(10) << "(masked)" << ": " ;    
        for(int msk = 0; msk < RunTests::NUM_MASKS; ++msk) {
            testMaskedSubGenerated(ret, a, b, msk);
            for(int i = 0; i < VECLEN; ++i) { 
                ret2[i] = (msk & 1 << i) > 0 ? a[i] - b[i] : 0;
            }
          
            /* Check if the results match */
            for(int i = 0; i < VECLEN; i++) { 
                double diff = ret2[i]-ret[i];
                double rel_diff = diff/ret[2];
                if( fabs(rel_diff) > tol) { 
                    std::cout << "FAIL: For mask value : " << msk << " i= " << i 
                              << " desired = " << ret2[i] 
                              << " generated=" << ret[i] << endl;
                    return 0;
                }
            }
        }
    }
    else {
      cout << setw(10) << left << "Sub" << setw(10) <<"(no mask)" << ": " ;    
      testSubGenerated(ret,a,b);
      /* Do it manually */
      for(int i = 0; i < VECLEN; i++) { 
          ret2[i] = a[i] - b[i];
      }
      for(int i = 0; i < VECLEN; i++) { 
          double diff = ret2[i]-ret[i];
          double rel_diff = diff/ret[2];
          if( fabs(rel_diff) > tol) { 
              std::cout << "FAIL: i= " << i << " desired = " 
                        << ret2[i] << " generated=" << ret[i] << endl;
              return 0;
          }
      }
    }  

    std::cout << "PASS" << endl;
    return 1;
}

/** Test the mul function */
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
 
    /* Call generated function */
    if(withMask) {
        cout << setw(10) << left << "Mul" << setw(10) << "(masked)" << ": " ;    
        for(int msk = 0; msk < RunTests::NUM_MASKS; ++msk) {
            testMaskedMulGenerated(ret, a, b, msk);
            for(int i = 0; i < VECLEN; ++i) { 
                ret2[i] = (msk & 1 << i) > 0 ? a[i] * b[i] : 0;
            }
          
            /* Check if the results match */
            for(int i = 0; i < VECLEN; i++) { 
                double diff = ret2[i]-ret[i];
                double rel_diff = diff/ret[2];
                if( fabs(rel_diff) > tol) { 
                    std::cout << "FAIL: For mask value : " << msk << " i= " << i 
                              << " desired = " << ret2[i] 
                              << " generated=" << ret[i] << endl;
                    return 0;
                }
            }
        }
    }
    else {
        cout << setw(10) << left << "Mul" << setw(10) <<"(no mask)" << ": " ;    
        testMulGenerated(ret,a,b);
        /* Do it manually */
        for(int i=0; i < VECLEN; i++) { 
            ret2[i] = a[i] * b[i];
        }
        
        for(int i=0; i < VECLEN; i++) { 
            double diff = ret2[i]-ret[i];
            double rel_diff = diff/ret[2];
            if( fabs(rel_diff) > tol) { 
                std::cout << "FAIL: i= " << i << " desired = " 
                  << ret2[i] << " generated=" << ret[i] << endl;
                return 0;
            }
        }
    }  
  
    std::cout << "PASS" << endl;
    return 1;
}


/** Testing the fnMadd function */
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
    
    /* Call generated function */
    if(withMask) {
        cout << setw(10) << left << "FnMAdd" << setw(10) << "(masked)" << ": " ;
        for(int msk = 0; msk < RunTests::NUM_MASKS; ++msk) {
            testMaskedFnMaddGenerated(ret,a,b,c, msk);
            /* Do it manually */
            for(int i=0; i < VECLEN; ++i) { 
                ret2[i] = (msk & 1 << i) > 0 ? c[i] - (a[i]*b[i]) : 0;
            }
            
            /* Check if the results match */
            for(int i = 0; i < VECLEN; i++) { 
                double diff = ret2[i]-ret[i];
                double rel_diff = diff/ret[2];
                if( fabs(rel_diff) > tol) { 
                    std::cout << "FAIL: For mask value : " << msk << " i= " << i 
                              << " desired = " << ret2[i] 
                              << " generated=" << ret[i] << endl;
                    return 0;
                }
            }       
        }    
    }
    else {
        cout << setw(10) << left << "FnMAdd" << setw(10) << "(no mask)" << ": " ;    
        testFnMaddGenerated(ret,a,b,c);
        /* Do it manually */
        for(int i=0; i < VECLEN; i++) { 
            ret2[i] = c[i] - (a[i]*b[i]);
        }
        
        for(int i=0; i < VECLEN; i++) { 
            double diff = ret2[i]-ret[i];
            double rel_diff = diff/ret[2];
            if( fabs(rel_diff) > tol) { 
            std::cout << "FAIL: i= " << i << " desired = " 
                      << ret2[i] << " generated=" << ret[i] << endl;
            return 0;
            }
        }
    }
 
    std::cout << "PASS" << endl;
    return 1;
}


/** Testing the fMadd function */
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
    
    /* Call generated function */
    if(withMask) {
        cout << setw(10) << left << "FMAdd" << setw(10) << "(masked)" << ": ";
        for(int msk = 0; msk < RunTests::NUM_MASKS; ++msk) {
            testMaskedFMaddGenerated(ret,a,b,c, msk);
            /* Do it manually */
            for(int i=0; i < VECLEN; ++i) { 
                ret2[i] = (msk & 1 << i) > 0 ? c[i] + (a[i]*b[i]) : 0;
            }
            
            /* Check if the results match */
            for(int i = 0; i < VECLEN; i++) { 
                double diff = ret2[i]-ret[i];
                double rel_diff = diff/ret[2];
                if( fabs(rel_diff) > tol) { 
                    std::cout << "FAIL: For mask value : " << msk << " i= " << i 
                              << " desired = " << ret2[i] 
                              << " generated=" << ret[i] << endl;
                    return 0;
                }
            }       
        }     
    }
    else {
        cout << setw(10) << left << "FMAdd" << setw(10) << "(no mask)" << ": " ;    
        testFMaddGenerated(ret,a,b,c);
        /* Do it manually */
        for(int i=0; i < VECLEN; i++) { 
            ret2[i] = c[i] + (a[i]*b[i]);
        }
        
        for(int i=0; i < VECLEN; i++) { 
            double diff = ret2[i]-ret[i];
            double rel_diff = diff/ret[2];
            if( fabs(rel_diff) > tol) { 
                std::cout << "FAIL: i= " << i << " desired = " 
                  << ret2[i] << " generated=" << ret[i] << endl;
            return 0;
            }
        }
    }
 
    std::cout << "PASS" << endl;
    return 1;
}


int 
RunTests::testLoadSplitSOAFVec(int soalen, int precision)
{
    FVecBaseType a[VECLEN];
    FVecBaseType b = 0.0;
    FVecBaseType ret[VECLEN];
    FVecBaseType ret2[VECLEN]; 
    
    for(int i=0; i < VECLEN; i++) {
        a[i] = (FVecBaseType)(i+1);
    }       
    
    cout << setw(20) << left << "LoadSplitSOAFVec" << ": " ;    
    
    testLoadSplitSOAFVecGen(ret, a, &b);

    /* Do Manually */        
    for(int i=0; i < VECLEN; i++) { 
        ret2[i] = ((1 << (soalen -1)) & 1 << i) > 0 ? b : a[i];
    }   

    for(int i=0; i < VECLEN; i++) { 
        double diff = ret2[i]-ret[i];
        double rel_diff = diff/ret[2];
        if( fabs(rel_diff) > tol) { 
            std::cout << "FAIL: i= " << i << " desired = " 
              << ret2[i] << " generated=" << ret[i] << endl;
        return 0;
        }
    }

    std::cout << "PASS" << endl;
    return 1;    
}

int 
RunTests::testPackFVec(int possibleMask, int mask)
{
    FVecBaseType a[VECLEN];
    FVecBaseType ret[VECLEN];
    //FVecBaseType ret1[VECLEN];    
    FVecBaseType ret2[VECLEN];
    
    for(int i=0; i < VECLEN; i++) {
        a[i] = (FVecBaseType)(i+1.5);
    }      
    
    /*
    std::cout << "A : " ;
    for(int i = 0; i < VECLEN; ++i)
        std::cout << a[i] << " ";
    std::cout << std::endl;
    */
    
    cout << setw(20) << left << "PackFVec" << ": " ;   
    testpackFVec(ret, a);

    /* Do manually */
    /* The CondExtractFVecElement would extract the FVec element based on
     * two conditions - a) possibleMask and b) mask. 
     *
     * PossibleMask determines two things, depending on which bit position(s) of 
     * the number is set, the lower or upper 128 bits (for 256 bit vectors) of 
     * the FVec are selected. 
     * 
     * In the likelihood of more than one bit being set, the mask comes into 
     * play. Only the bit position that is also set in the mask is then used to
     * select the FVec element.
     */
     
    /* In this test case, specifically possibleMasks that have just a single bit 
     * set are used.
     */

    FVecBaseType _tmp[VECLEN/2];

    int half, pos;

    for(int i = 0; i < VECLEN; ++i) {
        if(possibleMask & (1 << i) && (mask & (1 << i))) {
            half = i / (VECLEN/2);
            pos  = i % (VECLEN/2);
        }
    }
    
    if(half) {
        for(int i = 0; i < VECLEN/2; ++i)
            _tmp[i] = a[VECLEN/2 + i];    
    }  
    else {
        for(int i = 0; i < VECLEN/2; ++i)
            _tmp[i] = a[i];        
    }    
    
    ret2[0] = _tmp[pos];
    
    //__m256 ldvec = _mm256_load_ps((a));
    //((int*)((ret1)+0))[0] = _mm_extract_ps(_mm256_extractf128_ps(ldvec, 0),3);
    
    //std::cout << "RET[0] "  << ret[0]  << std::endl;
    //std::cout << "RET1[0] " << ret1[0] << std::endl;    
    //std::cout << "RET2[0] " << ret2[0] << std::endl;
    
    
    double diff = ret2[0]-ret[0];
    //double rel_diff = diff/ret[2];
    if( fabs(diff) > tol) { 
        std::cout << "FAIL: i = 0 desired = " 
          << ret2[0] << " generated=" << ret[0] << endl;
    return 0;
    }

    std::cout << "PASS" << endl;
    return 1;        

}

int main(int argc, char *argv[]) 
{
    RunTests runTest;
    runTest.testLoadStore(false);
    runTest.testLoadStore(true);    
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
    /* Testing for the case where SOALEN == VECLEN */  
    //runTest.testLoadSplitSOAFVec(VECLEN, PRECISION);
    
    runTest.testPackFVec(VECLEN, VECLEN);
}
