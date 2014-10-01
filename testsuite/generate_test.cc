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
    if (stat("intrinsic_fns", &st) == -1) {
        mkdir("intrinsic_fns", 0700);
    }
    else {
        remove("intrinsic_fns");
        mkdir("intrinsic_fns", 0700);
    }
}


void generate_FnMadd()
{
    ofstream out("intrinsic_fns/gen_fnMadd.h");

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

void generate_FMadd()
{
    ofstream out("intrinsic_fns/gen_fMadd.h");

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

    fmaddFVec(ivector, retvec, avec, bvec, cvec);
    storeFVec(ivector, retvec, &addr_ret, 0);
    
    out << "void testFMaddGenerated( " 
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

void generate_Sub()
{
    ofstream out("intrinsic_fns/gen_sub.h");

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

    subFVec(ivector, retvec, avec, bvec);
    storeFVec(ivector, retvec, &addr_ret, 0);
    
    out << "void testSubGenerated( " 
        << FVecBaseType <<" *ret, "
        << "const " << FVecBaseType <<" *a, "
        << "const " << FVecBaseType <<" *b )" << endl;

    out << "{" << endl;

    for(int i=0; i < ivector.size(); i++) {
        out << ivector[i]->serialize() << endl;
    }

    out << "}" << endl;
}


void generate_Add()
{
    ofstream out("intrinsic_fns/gen_add.h");

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

    addFVec(ivector, retvec, avec, bvec);
    storeFVec(ivector, retvec, &addr_ret, 0);
    
    out << "void testAddGenerated( " 
        << FVecBaseType <<" *ret, "
        << "const " << FVecBaseType <<" *a, "
        << "const " << FVecBaseType <<" *b )" << endl;

    out << "{" << endl;

    for(int i=0; i < ivector.size(); i++) {
        out << ivector[i]->serialize() << endl;
    }

    out << "}" << endl;
}

void generate_Mul()
{
    ofstream out("intrinsic_fns/gen_mul.h");

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

    mulFVec(ivector, retvec, avec, bvec);
    storeFVec(ivector, retvec, &addr_ret, 0);
    
    out << "void testMulGenerated( " 
        << FVecBaseType <<" *ret, "
        << "const " << FVecBaseType <<" *a, "
        << "const " << FVecBaseType <<" *b )" << endl;

    out << "{" << endl;

    for(int i=0; i < ivector.size(); i++) {
        out << ivector[i]->serialize() << endl;
    }

    out << "}" << endl;
}

void generate_SetZero()
{
    ofstream out("intrinsic_fns/gen_setzero.h");

    InstVector ivector;
    FVec retvec("retvec");
    GenericAddress addr_ret("ret",0);

    declareFVecFromFVec(ivector,retvec);

    setZero(ivector, retvec);
    storeFVec(ivector, retvec, &addr_ret, 0);
    
    out << "void testSetZeroGenerated( " 
        << FVecBaseType <<" *ret )" << endl;

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
    generate_FMadd();
    generate_Sub();
    generate_Add();
    generate_Mul();
    generate_SetZero();
}
