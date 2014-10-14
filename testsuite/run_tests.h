#ifndef __RUN_TESTS_H__
#define __RUN_TESTS_H__

class RunTests
{
private:
    /* The mask values that we would use to test the blend operations.
     * We would test from 0 - NUM_MASK -1.
     */ 
    static const int NUM_MASKS = 16;

public:
    int testLoadStore(bool withMask);    
    int testSetZero();
    int testAdd(bool withMask);
    int testSub(bool withMask);
    int testMul(bool withMask);
    int testFMadd(bool withMask);
    int testFnMadd(bool withMask);
    int testLoadSplitSOAFVec(int soalen, int precision);
};

#endif
