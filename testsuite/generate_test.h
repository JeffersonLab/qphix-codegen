#ifndef __GENERATE_TEST_H__
#define __GENERATE_TEST_H__

class GenerateTestFns
{
private:
    /**
     * Private helper function to get the path of the executable. 
     */
    string getExecutableDirectory();
    /** 
     * Create a directory under the same path as the "generate_tests.exe" 
     * executable and save the generated test functions there.
     */
    void createTestDir();
    /**
     * Holds the path where the generated test functions are to be saved.
     */ 
    string outPath;

public:
    /** Overide the default constructor */
    GenerateTestFns();
    /**
     * Multiplication of packed double-precision (64-bit) floating-point 
     * elements in a and b and add the negated intermediate result to packed 
     * elements in c, and store the results in dst. 
     */
    void generateFnMadd(bool useMask); 
    /**
     * Multiplication of packed double-precision (64-bit) floating-point 
     * elements in a and b and add the intermediate result to packed 
     * elements in c, and store the results in dst. 
     */
    void generateFMadd(bool useMask);    
    /**
     * Subtract packed double-precision (64-bit) floating-point elements in b 
     * from packed double-precision (64-bit) floating-point elements in a, 
     * and store the results in dst.
     */
    void generateSub(bool useMask);
    /**
     * Add packed double-precision (64-bit) floating-point elements in 
     * a and b, and store the results in dst.
     */
    void generateAdd(bool useMask);
    /**
     * Multiply packed double-precision (64-bit) floating-point elements in a 
     * and b, and store the results in dst. 
     */
    void generateMul(bool useMask);
    /**
     * Return vector with all elements set to zero.
     */
    void generateSetZero();
    /**
     * Load packed floating point elements from memory to VPU. Then store the 
     * loaded vector elements to a different memory location. We are trying to 
     * test if the load and store works as expected. 
     * The interesting test is to ensure the blended load and store are working 
     * as expected.
     */ 
    void generateLoadStoreFVec(bool useMask);
    /**
     * Test the loadSplitSOAFVec function
     */
    void generateLoadSplitSOAFVec(int precision, int soalen);
};

#endif
