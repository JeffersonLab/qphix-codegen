#ifndef __RUN_TESTS_H__
#define __RUN_TESTS_H__

class RunTests
{
private:
/* Define static const int values for the MASK. Right now we are only 
 * defining values for the double precision case (0-15), if required this 
 * can be expanded for the single precision testcases (0-255).
 */

    static const int NUM_MASKS = 16;
    //static const int MASKS[NUM_MASKS];
    /*static int const _M00 = 0;
    static int const _M01 = 1;
    static int const _M02 = 2;    
    static int const _M03 = 3;
    static int const _M04 = 4;
    static int const _M05 = 5;
    static int const _M06 = 6;
    static int const _M07 = 7; 
    static int const _M08 = 8;
    static int const _M09 = 9;
    static int const _M10 = 10;
    static int const _M11 = 11;
    static int const _M12 = 12;
    static int const _M13 = 13;
    static int const _M14 = 14;
    static int const _M15 = 15;  
    */                                             
public:
    int testLoadStore(bool withMask);    
    int testSetZero();
    int testAdd(bool withMask);
    int testSub(bool withMask);
    int testMul(bool withMask);
    int testFMadd(bool withMask);
    int testFnMadd(bool withMask);

};

#endif
