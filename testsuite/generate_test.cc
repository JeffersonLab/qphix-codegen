#include "instructions.h"
#include "data_types.h"

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

#if PRECISION == 1
#define FVecBaseType "float"
#else
#define FVecBaseType "double"
#endif

/** Create the output directory for the generated test scripts.
 */
void create_test_dir()
{
    struct stat st = {0};
    if (stat("testscripts", &st) == -1) {
        mkdir("testscripts", 0700);
    }
    else {
        remove("testscripts");
        mkdir("testscripts", 0700);
    }
}


void generate_FnMadd()
{
    ofstream out("testscripts/test_gen_fnMadd.h");

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

    fnmaddFVec(ivector, retvec, avec, bvec, cvec);
    storeFVec(ivector, retvec, &addr_ret, 0);
    
    out << "void testFnMaddGenerated( " 
        << FVecBaseType <<" *ret, "
        << "const " << FVecBaseType <<" *a, "
        << "const " << FVecBaseType <<" *b, " 
        << "const " << FVecBaseType <<" *c )" << endl;

    out << "{" << endl;

    for(int i=0; i < ivector.size(); i++) {
        out << ivector[i]->serialize() << endl;
    }


    out << "}" << endl;

}

int main(int argc, char *argv[])
{
    create_test_dir();
    generate_FnMadd();
}
