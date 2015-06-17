#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <typeinfo>
#include <string>
#include <map>

#include <cstdio>
#include <cstdlib>

using namespace std;

#include "instructions.h"
#include "data_types.h"


extern std::string ARCH_NAME;


void declare_HalfSpinor(InstVector& ivector, FVec h_spinor[2][3][2]);
void declare_Gauge(InstVector& ivector, FVec u_gauge[3][3][2]);
void declare_WilsonSpinor(InstVector& ivector, FVec spinor[4][3][2]);
void declare_KSSpinor(InstVector& ivector, FVec ks_spinor[3][2]);
void declare_Clover(InstVector& ivector, FVec diag[6], FVec offdiag[15][2]);
void movCVec(InstVector& ivector, FVec *r, FVec *s1, string &mask);
void addCVec(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, string &mask);
void subCVec(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, string &mask);
void addiCVec(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, string &mask);
void subiCVec(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, string &mask);
// r[RE] = s1[RE]-beta_vec*s2[RE] = fnmadd(beta_vec,s2[RE],s1[RE])
// r[IM] = s1[IM]-beta_vec*s2[IM] = fnamdd(beta_vec,s2[IM],s1[IM])
void addCVec_mbeta(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask);
// r[RE] = s1[RE] + beta_vec*s2[RE] = fmadd(beta_vec, s2[RE], s1[RE]);
// r[IM] = s1[IM] + beta_vec*s2[IM] = fmadd(beta_vec, s2[IM], s1[IM]);
void subCVec_mbeta(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask);
// r[RE] = s1[RE] + beta_vec * s2[IM] = fmadd(beta_vec,s2[IM], s1[RE]) 
// r[IM] = s1[IM] - beta_vec * s2[RE] = fnmadd(beta_vec, s2[RE], s1[IM])

void addiCVec_mbeta(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask);
// r[RE] = s1[RE] - beta_vec*s2[IM] = fnmadd( beta_vec, s2[IM], s1[RE]);
// r[IM] = s1[IM] + beta_vec*s2[RE] = fmadd ( beta_vec, s2[RE], s1[IM]);
void subiCVec_mbeta(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask);
// r[RE] = s1[RE]+beta_vec*s2[RE] = fmadd(beta_vec,s2[RE],s1[RE])
// r[IM] = s1[IM]+beta_vec*s2[IM] = fmadd(beta_vec,s2[IM],s1[IM])
void addCVec_pbeta(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask);
// r[RE] = s1[RE] - beta_vec*s2[RE] = fnmadd(beta_vec, s2[RE], s1[RE]);
// r[IM] = s1[IM] - beta_vec*s2[IM] = fnmadd(beta_vec, s2[IM], s1[IM]);
void subCVec_pbeta(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask);
// r[RE] = s1[RE] - beta_vec * s2[IM] = fnmadd(beta_vec,s2[IM], s1[RE]) 
// r[IM] = s1[IM] + beta_vec * s2[RE] = fmadd(beta_vec, s2[RE], s1[IM])
void addiCVec_pbeta(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask);
// r[RE] = s1[RE] + beta_vec*s2[IM] = fmadd( beta_vec, s2[IM], s1[RE]);
// r[IM] = s1[IM] - beta_vec*s2[RE] = fnmadd ( beta_vec, s2[RE], s1[IM]);
void subiCVec_pbeta(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask);
// r = s1*s2
void mulCVec(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, string &mask);
// r = s1*s2+s3
void fmaddCVec(InstVector& ivector, FVec *r, FVec *s1, FVec *s2,  FVec *s3, string &mask);
// r = s3-s1*s2
//r[RE] = (s3[RE]-s1[RE]*s2[RE])+(s1[IM]*s2[IM])
//r[IM] = (s3[IM]-s1[RE]*s2[IM])-(s1[IM]*s2[RE])
void fnmaddCVec(InstVector& ivector, FVec *r, FVec *s1, FVec *s2,  FVec *s3, string &mask);
// r = (s1*s2-s3*s4)'
//r[RE] = (s1[RE]*s2[RE])-(s1[IM]*s2[IM])-(s3[RE]*s4[RE])+(s3[IM]*s4[IM])
//r[IM] = (s3[RE]*s4[IM])+(s3[IM]*s4[RE])-(s1[RE]*s2[IM])-(s1[IM]*s2[RE])
void Conj_CrossProd(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec *s3, FVec *s4, string &mask);
// r = s1'*s2
//r[RE] = (s1[RE]*s2[RE])+(s1[IM]*s2[IM])
//r[IM] = (s1[RE]*s2[IM])-(s1[IM]*s2[RE])
void mulConjCVec(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, string &mask);
// r = s1'*s2+s3
//r[RE] = (s3[RE]+s1[RE]*s2[RE])+(s1[IM]*s2[IM])
//r[IM] = (s3[IM]+s1[RE]*s2[IM])-(s1[IM]*s2[RE])
void fmaddConjCVec(InstVector& ivector, FVec *r, FVec *s1, FVec *s2,  FVec *s3, string &mask);
// r[1][3] = s_vec[1][3] * u_mat[3][3](')
// Transposed matrix vector multiplication
void matMultVecT(InstVector& ivector, FVec r[3][2], FVec s_vec[3][2], FVec u_mat[3][3][2], bool adjMul, string mask);
// r[3][1] = u_mat[3][3](') * s_vec[3][1]
void matMultVec(InstVector& ivector, FVec r[3][2], FVec u_mat[3][3][2], FVec s_vec[3][2], bool adjMul, string mask);
// u_ret = u_mat1 * u_mat2(')
void matMultMat(InstVector& ivector, FVec u_ret[3][3][2], FVec u_mat1[3][3][2], FVec u_mat2[3][3][2], bool adjMat, string mask);
// u_ret = u_mat1' * u_mat2
void adjMatMultMat(InstVector& ivector, FVec u_ret[3][3][2], FVec u_mat1[3][3][2], FVec u_mat2[3][3][2], string mask);
void decompressGauge(InstVector& ivector, FVec u_gauge[3][3][2], bool compress12, string mask);
// Merge L2 prefetches with another instruction stream
void mergeIvectorWithL2Prefetches(InstVector& ivector, InstVector& l2prefs);
// Dump an instruction stream into a file
void dumpIVector(InstVector& ivector, string filename);
