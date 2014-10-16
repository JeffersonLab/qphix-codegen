#include <iostream>
#include <fstream>
#include "instructions.h"
#include "address_types.h"

using namespace std;


void dumpIVector(InstVector& ivector, string filename)
{
    ofstream outfile(filename.c_str());

    for(unsigned int i=0; i < ivector.size(); i++) {
        outfile << ivector[i]->serialize() << endl;
    }

    outfile.close();
}

void generate(InstVector& ivector)
{
  FVec avec("avec");
  FVec xvec("xvec");
  FVec yvec("yvec");
  FVec zvec("zvec");

  declareFVecFromFVec(ivector, avec); // Generate declarations
  declareFVecFromFVec(ivector, xvec); 
  declareFVecFromFVec(ivector, yvec);
  declareFVecFromFVec(ivector, zvec);

  // Function prototype will provide us with pointers
  GenericAddress* xptr=new GenericAddress(string("xptr"),0); // We need to define xptr in the function prototype
  GenericAddress* yptr=new GenericAddress(string("yptr"),0); // We need to define yptr in the function prototype
  GenericAddress* zptr=new GenericAddress(string("zptr"),0); // We need to define yptr in the function prototype

  // Generate code:
  loadBroadcastScalar(ivector, avec, "a");  // generate load a into avec and broadcast it
  loadFVec(ivector, xvec, xptr,string("")); //  generate load xvec from xptr
  loadFVec(ivector, yvec, yptr,string("")); //  generate load yvec from yptr

  fmaddFVec(ivector, zvec, avec, xvec, yvec, string("")); // generate zvec = avec*xvec+yvec 
  storeFVec(ivector, zvec, zptr, 1);   // generate streaming store 

}

int main(int argc, char *argv[]) 
{
   InstVector ivector;
   ivector.resize(0); // empty the ivector
   generate(ivector);
   dumpIVector(ivector, "axpy.h");
}
