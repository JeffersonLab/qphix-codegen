#include "instructions.h"
#include "data_types.h"
#include "generate_test.h"

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>  // exit
#include <limits.h> // PATH_MAX
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cassert>
using namespace std;

#if PRECISION == 1
#define FVecBaseType "float"
#else
#define FVecBaseType "double"
#endif

/** Override the default constructor */
GenerateTestFns::GenerateTestFns()
{
    createTestDir();
}

/**
 * Load packed floating point elements from memory to VPU. Then store the loaded
 * vector elements to a different memory location. We are trying to test if the
 * load and store works as expected. 
 * The interesting test is to ensure the blended load is working as expected.
 */ 
void 
GenerateTestFns::generateLoadStoreFVec(bool useMask)
{
    string _tmp(outPath); 
    ofstream ofs;
    ofs.open(_tmp.append("/gen_ld_st.h").c_str(), ofstream::out | ofstream::app);    

    InstVector ivector;  
    FVec ldvec("ldvec");
    GenericAddress addr_ld("ld",0);
    GenericAddress addr_st("st",0);

    declareFVecFromFVec(ivector,ldvec);

    if(!useMask)
      loadFVec(ivector, ldvec, &addr_ld, "");
    else {
      string mask = "maskVal";
      declareMask(ivector, mask);
      intToMask(ivector, mask, "msk");
      loadFVec(ivector, ldvec, &addr_ld, mask);
    } 
       
    storeFVec(ivector, ldvec, &addr_st, 0);    
    
    if(!useMask)
      ofs << "void testLoadStoreFVec( ";
    else
      ofs << "void testMaskedLoadStoreFVec( ";    
      
    ofs << FVecBaseType <<" *st, "
        << FVecBaseType <<" *ld";

    if(!useMask)       
      ofs << " )" << endl;
    else
      ofs << ", unsigned int msk )" << endl;
      
    ofs << "{" << endl;

    for(unsigned int i=0; i < ivector.size(); i++) {
        ofs << ivector[i]->serialize() << endl;
    }

    ofs << "}" << endl;
    ofs.close();    
}

/**
 * Multiplication of packed double-precision (64-bit) floating-point 
 * elements in a and b and add the negated intermediate result to packed 
 * elements in c, and store the results in dst. 
 */
void 
GenerateTestFns::generateFnMadd(bool useMask)
{
    string _tmp(outPath); 
    ofstream ofs;
    ofs.open(_tmp.append("/gen_fnMadd.h").c_str(), ofstream::out | ofstream::app);

    InstVector ivector;  
    FVec avec("avec");
    FVec bvec("bvec");
    FVec cvec("cvec"); 
    FVec retvec("retvec");

    GenericAddress addr_a("a",0);
    GenericAddress addr_b("b",0);
    GenericAddress addr_c("c",0);
    GenericAddress addr_ret("ret",0);
    GenericAddress addr_mask("m",0);
 
    declareFVecFromFVec(ivector,avec);
    declareFVecFromFVec(ivector,bvec);
    declareFVecFromFVec(ivector,cvec);
    declareFVecFromFVec(ivector,retvec);

    loadFVec(ivector, avec, &addr_a, "");
    loadFVec(ivector, bvec, &addr_b, "");
    loadFVec(ivector, cvec, &addr_c, "");

    if(!useMask)
      fnmaddFVec(ivector, retvec, avec, bvec, cvec);
    else {
      string mask = "maskVal";
      declareMask(ivector, mask);
      intToMask(ivector, mask, "msk");
      fnmaddFVec(ivector, retvec, avec, bvec, cvec, mask);
    }

    storeFVec(ivector, retvec, &addr_ret, 0);

    if(!useMask)
      ofs << "void testFnMaddGenerated( " ;
    else
      ofs << "void testMaskedFnMaddGenerated( ";
    
    ofs << FVecBaseType <<" *ret, "
        << FVecBaseType <<" *a, "
        << FVecBaseType <<" *b, " 
        << FVecBaseType <<" *c";

    if(!useMask)       
      ofs << " )" << endl;
    else
      ofs << ", unsigned int msk )" << endl;

    ofs << "{" << endl;
    
    for(unsigned int i=0; i < ivector.size(); i++) {
      ofs <<"    " << ivector[i]->serialize() << endl;
    }

    ofs << "}" << endl << endl;
    ofs.close();
    
}

/**
 * Multiplication of packed double-precision (64-bit) floating-point 
 * elements in a and b and add the intermediate result to packed 
 * elements in c, and store the results in dst. 
 */
void 
GenerateTestFns::generateFMadd(bool useMask)
{
    string _tmp(outPath); 
    ofstream ofs;
    ofs.open(_tmp.append("/gen_fMadd.h").c_str(), ofstream::out | ofstream::app);

    InstVector ivector;  
    FVec avec("avec");
    FVec bvec("bvec");
    FVec cvec("cvec"); 
    FVec retvec("retvec");
    GenericAddress addr_a("a",0);
    GenericAddress addr_b("b",0);
    GenericAddress addr_c("c",0);
    GenericAddress addr_ret("ret",0);

    declareFVecFromFVec(ivector,avec);
    declareFVecFromFVec(ivector,bvec);
    declareFVecFromFVec(ivector,cvec);
    declareFVecFromFVec(ivector,retvec);

    loadFVec(ivector, avec, &addr_a, "");
    loadFVec(ivector, bvec, &addr_b, "");
    loadFVec(ivector, cvec, &addr_c, "");

    if(!useMask)
      fmaddFVec(ivector, retvec, avec, bvec, cvec);
    else {
      string mask = "maskVal";
      declareMask(ivector, mask);
      intToMask(ivector, mask, "msk");
      fmaddFVec(ivector, retvec, avec, bvec, cvec, mask);
    }

    storeFVec(ivector, retvec, &addr_ret, 0);
    
    if(!useMask)
      ofs << "void testFMaddGenerated( ";
    else
      ofs << "void testMaskedFMaddGenerated( ";
    
    ofs << FVecBaseType <<" *ret, "
        << FVecBaseType <<" *a, "
        << FVecBaseType <<" *b, " 
        << FVecBaseType <<" *c";

    if(!useMask)       
      ofs << " )" << endl;
    else
      ofs << ", unsigned int msk )" << endl;


    ofs << "{" << endl;

    for(unsigned int i=0; i < ivector.size(); i++) {
        ofs << ivector[i]->serialize() << endl;
    }

    ofs << "}" << endl;
    ofs.close();
}

/**
 * Subtract packed double-precision (64-bit) floating-point elements in b from 
 * packed double-precision (64-bit) floating-point elements in a, and store the 
 * results in dst.
 */
void 
GenerateTestFns::generateSub(bool useMask)
{
    string _tmp(outPath); 
    ofstream ofs;
    ofs.open(_tmp.append("/gen_sub.h").c_str(), ofstream::out | ofstream::app);

    InstVector ivector;  
    FVec avec("avec");
    FVec bvec("bvec");
    FVec retvec("retvec");
    GenericAddress addr_a("a",0);
    GenericAddress addr_b("b",0);
    GenericAddress addr_ret("ret",0);

    declareFVecFromFVec(ivector,avec);
    declareFVecFromFVec(ivector,bvec);
    declareFVecFromFVec(ivector,retvec);

    loadFVec(ivector, avec, &addr_a, "");
    loadFVec(ivector, bvec, &addr_b, "");

    if(!useMask)
      subFVec(ivector, retvec, avec, bvec);
    else {
      string mask = "maskVal";
      declareMask(ivector, mask);
      intToMask(ivector, mask, "msk");
      subFVec(ivector, retvec, avec, bvec, mask);
    }
    
    storeFVec(ivector, retvec, &addr_ret, 0);

    if(!useMask)
      ofs << "void testSubGenerated( ";
    else
      ofs << "void testMaskedSubGenerated( ";

    ofs << FVecBaseType <<" *ret, "
        << FVecBaseType <<" *a, "
        << FVecBaseType <<" *b";

    if(!useMask)       
      ofs << " )" << endl;
    else
      ofs << ", unsigned int msk )" << endl;

    ofs << "{" << endl;

    for(unsigned int i=0; i < ivector.size(); i++) {
        ofs << ivector[i]->serialize() << endl;
    }

    ofs << "}" << endl;
    ofs.close();
}

/**
 * Add packed double-precision (64-bit) floating-point elements in a and b, 
 * and store the results in dst.
 */
void 
GenerateTestFns::generateAdd(bool useMask)
{
    string _tmp(outPath); 
    ofstream ofs;
    ofs.open(_tmp.append("/gen_add.h").c_str(), ofstream::out | ofstream::app);

    InstVector ivector;  
    FVec avec("avec");
    FVec bvec("bvec");
    FVec retvec("retvec");
    GenericAddress addr_a("a",0);
    GenericAddress addr_b("b",0);
    GenericAddress addr_ret("ret",0);

    declareFVecFromFVec(ivector,avec);
    declareFVecFromFVec(ivector,bvec);
    declareFVecFromFVec(ivector,retvec);

    loadFVec(ivector, avec, &addr_a, "");
    loadFVec(ivector, bvec, &addr_b, "");
    
    if(!useMask)
      addFVec(ivector, retvec, avec, bvec);
    else {
      string mask = "maskVal";
      declareMask(ivector, mask);
      intToMask(ivector, mask, "msk");
      addFVec(ivector, retvec, avec, bvec, mask);
    }
    
    storeFVec(ivector, retvec, &addr_ret, 0);

    if(!useMask)
      ofs << "void testAddGenerated( ";
    else
      ofs << "void testMaskedAddGenerated( ";    

    ofs << FVecBaseType <<" *ret, "
        << FVecBaseType <<" *a, "
        << FVecBaseType <<" *b";

    if(!useMask)       
      ofs << " )" << endl;
    else
      ofs << ", unsigned int msk )" << endl;

    ofs << "{" << endl;

    for(unsigned int i=0; i < ivector.size(); i++) {
        ofs << ivector[i]->serialize() << endl;
    }

    ofs << "}" << endl;
    ofs.close();
}

/**
 * Multiply packed double-precision (64-bit) floating-point elements in a and b, 
 * and store the results in dst. 
 */
void 
GenerateTestFns::generateMul(bool useMask)
{
    string _tmp(outPath); 
    ofstream ofs;
    ofs.open(_tmp.append("/gen_mul.h").c_str(), ofstream::out | ofstream::app);    

    InstVector ivector;  
    FVec avec("avec");
    FVec bvec("bvec");
    FVec retvec("retvec");
    GenericAddress addr_a("a",0);
    GenericAddress addr_b("b",0);
    GenericAddress addr_ret("ret",0);

    declareFVecFromFVec(ivector,avec);
    declareFVecFromFVec(ivector,bvec);
    declareFVecFromFVec(ivector,retvec);

    loadFVec(ivector, avec, &addr_a, "");
    loadFVec(ivector, bvec, &addr_b, "");

    if(!useMask)
      mulFVec(ivector, retvec, avec, bvec);
    else {
      string mask = "maskVal";
      declareMask(ivector, mask);
      intToMask(ivector, mask, "msk");
      mulFVec(ivector, retvec, avec, bvec, mask);
    }    
    storeFVec(ivector, retvec, &addr_ret, 0);
    
    if(!useMask)
      ofs << "void testMulGenerated( ";
    else
      ofs << "void testMaskedMulGenerated( ";    
      
    ofs << FVecBaseType <<" *ret, "
        << FVecBaseType <<" *a, "
        << FVecBaseType <<" *b";

    if(!useMask)       
      ofs << " )" << endl;
    else
      ofs << ", unsigned int msk )" << endl;
      
    ofs << "{" << endl;

    for(unsigned int i=0; i < ivector.size(); i++) {
        ofs << ivector[i]->serialize() << endl;
    }

    ofs << "}" << endl;
    ofs.close();
}

/**
 * Return vector with all elements set to zero.
 */
void 
GenerateTestFns::generateSetZero()
{
    string _tmp(outPath); 
    ofstream out(_tmp.append("/gen_setzero.h").c_str());

    InstVector ivector;
    FVec retvec("retvec");
    GenericAddress addr_ret("ret",0);

    declareFVecFromFVec(ivector,retvec);

    setZero(ivector, retvec);
    storeFVec(ivector, retvec, &addr_ret, 0);
    
    out << "void testSetZeroGenerated( " 
        << FVecBaseType << " *ret )" << endl;

    out << "{" << endl;

    for(unsigned int i=0; i < ivector.size(); i++) {
        out << ivector[i]->serialize() << endl;
    }

    out << "}" << endl;
    out.close();
}

void
GenerateTestFns::generateLoadSplitSOAFVec(int precision, int soalen)
{
    string _tmp(outPath);
    ofstream out;
    out.open(_tmp.append("/gen_loadSplitSOAFVec.h").c_str(), ofstream::out | ofstream::app);        
    //string genFnName("");
    
    InstVector ivector;
    FVec ldvec("ldvec");
    GenericAddress addr_a1("a1",0);
    GenericAddress addr_a2("a2",0);
    GenericAddress addr_ret("addr_ret",0);
    
    declareFVecFromFVec(ivector,ldvec);
    
    /* We are handling only the cases where the soanum is not being used. */
    if(precision == 1 && soalen == 4) 
    {
        loadSplitSOAFVec(ivector, ldvec, &addr_a1, &addr_a2, 0, 4, 1, "");     
        //genFnName = "testLoadSplitSOAFVecGen_sp_v4";                   
    }
    else if(precision == 1 && soalen == 8)
    {
        loadSplitSOAFVec(ivector, ldvec, &addr_a1, &addr_a2, 0, 8, 1, ""); 
        //genFnName = "testLoadSplitSOAFVecGen_sp_v8";
    }
    else if(precision == 2 && soalen == 4)
    {
        loadSplitSOAFVec(ivector, ldvec, &addr_a1, &addr_a2, 0, 4, 1, ""); 
        //genFnName = "testLoadSplitSOAFVecGen_dp_v4";   
    }      
    else
    {
        cerr << "Error: Incorrect combination of Prec and SOALEN." << endl;
        exit(1);
    }  
    
    storeFVec(ivector, ldvec, &addr_ret, 0);           
      
    out << "void testLoadSplitSOAFVecGen( "
        << FVecBaseType <<" *addr_ret, "
        << FVecBaseType <<" *a1, "
        << FVecBaseType <<" *a2)" 
        << endl;        

    out << "{" << endl;

    for(unsigned int i=0; i < ivector.size(); i++) {
        out << ivector[i]->serialize() << endl;
    }

    out << "}" << endl;   
    out.close(); 
}


void 
GenerateTestFns::generatePackFVec(int possibleMask, string mask)
{
    assert(possibleMask < (1 << VECLEN) && possibleMask >=0);
        
    string _tmp(outPath);
    ofstream out;
    out.open(_tmp.append("/gen_packFVec.h").c_str(), ofstream::out | ofstream::app);        

    InstVector ivector;
    FVec ldvec("ldvec");    
    GenericAddress addr_a("a", 0);
    GenericAddress addr_ret("addr_ret",0);

    declareFVecFromFVec(ivector,ldvec);
    loadFVec(ivector, ldvec, &addr_a, "");    
    packFVec(ivector, ldvec, &addr_ret, mask, possibleMask);
    
    out << "void testpackFVec( "
        << FVecBaseType <<" *addr_ret, "
        << FVecBaseType <<" *a)" 
        << endl;        

    out << "{" << endl;

    for(unsigned int i=0; i < ivector.size(); i++) {
        out << ivector[i]->serialize() << endl;
    }

    out << "}" << endl;   
    out.close();     

}

/************************** Private helper functions **************************/
string 
GenerateTestFns::getExecutableDirectory()
{
  char path[PATH_MAX];
  size_t last_slash;
  /* Read the target of /proc/self/exe.  */
  if (readlink("/proc/self/exe", path, sizeof(path)-1) <= 0) {
    perror("Hmmm... directory of the generate_test.exe could not be deterimed."
           " Abort!");
    exit(1);
  }
  
  string pathStr = string(path);
  
  if ((last_slash = pathStr.find_last_of("/")) == string::npos) {
    perror("Hmmm... the directory path does not have a '\', how so? Abort!");
    exit(1);
  }

  pathStr = pathStr.substr(0, last_slash);
  return pathStr;
}


void 
GenerateTestFns::createTestDir()
{
    string directoryPath =  getExecutableDirectory();
    this->outPath        = directoryPath.append("/test_fns");
    const char * path    = this->outPath.c_str();

    struct stat st = {0};
    int status;

    if (stat(path, &st) == -1) {
        if ((status =  mkdir(path, 0700)) == -1) {
          perror("Could not build directory for the test functions. Abort!");
          exit(1);
        }
    }
    else {
        string remove = string("rm -fr ").append(path);
        system(remove.c_str());
        if ((status =  mkdir(path, 0700)) == -1) {
            perror("Could not build directory for the test functions. Abort!");
            exit(1);
        }
    }
}

/********************************* Main function ******************************/
int main(int argc, char *argv[])
{
    GenerateTestFns genInst;
    genInst.generateLoadStoreFVec(false);
    genInst.generateLoadStoreFVec(true);
    genInst.generateFnMadd(false);
    genInst.generateFnMadd(true);
    genInst.generateFMadd(false);
    genInst.generateFMadd(true);
    genInst.generateSub(false);
    genInst.generateSub(true);
    genInst.generateAdd(false);
    genInst.generateAdd(true);
    genInst.generateMul(false);
    genInst.generateMul(true);
    genInst.generateSetZero();
    
#if PRECISION == 1
    //genInst.generateLoadSplitSOAFVec(1,4);
    genInst.generateLoadSplitSOAFVec(1,8);
#elif PRECISION == 2
    genInst.generateLoadSplitSOAFVec(2,4);
#endif      
    genInst.generatePackFVec(VECLEN, "1");
    //genInst.generatePackFVec(VECLEN - 1, "5");          
}
