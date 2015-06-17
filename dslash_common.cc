#include "dslash_common.h"
using namespace std;

#if PRECISION == 1
#if VECLEN == 16
#ifdef AVX512
std::string ARCH_NAME="avx512";
#else
std::string ARCH_NAME="mic";
#endif
#elif VECLEN == 8
#ifdef AVX2
std::string ARCH_NAME="avx2";
#else
std::string ARCH_NAME="avx";
#endif
#elif VECLEN == 4
std::string ARCH_NAME="sse";
#elif VECLEN == 1
std::string ARCH_NAME="scalar";
#endif
#elif PRECISION == 2
#if VECLEN == 8
#ifdef AVX512
std::string ARCH_NAME="avx512";
#else
std::string ARCH_NAME="mic";
#endif
#elif VECLEN == 4
#ifdef AVX2
std::string ARCH_NAME="avx2";
#else
std::string ARCH_NAME="avx";
#endif
#elif VECLEN == 2
std::string ARCH_NAME="sse";
#elif VECLEN == 1
std::string ARCH_NAME="scalar";
#endif
#endif //PRECISION

void declare_HalfSpinor(InstVector& ivector, FVec h_spinor[2][3][2])
{
    for(int s=0; s < 2; s++) {
        for(int c = 0; c < 3; c++) {
            declareFVecFromFVec(ivector, h_spinor[s][c][RE]);
            declareFVecFromFVec(ivector, h_spinor[s][c][IM]);
        }
    }
}

void declare_Gauge(InstVector& ivector, FVec u_gauge[3][3][2]) {
    for(int c1=0; c1 < 3; c1++) {
        for(int c2 = 0; c2 < 3; c2++) {
            declareFVecFromFVec(ivector, u_gauge[c1][c2][RE]);
            declareFVecFromFVec(ivector, u_gauge[c1][c2][IM]);
        }
    }
}

void declare_WilsonSpinor(InstVector& ivector, FVec spinor[4][3][2]) {
    for(int s=0; s < 4; s++) {
        for(int c = 0; c < 3; c++) {
            declareFVecFromFVec(ivector, spinor[s][c][RE]);
            declareFVecFromFVec(ivector, spinor[s][c][IM]);
        }
    }
}

void declare_KSSpinor(InstVector& ivector, FVec ks_spinor[3][2]) {
    for(int c = 0; c < 3; c++) {
        declareFVecFromFVec(ivector, ks_spinor[c][RE]);
        declareFVecFromFVec(ivector, ks_spinor[c][IM]);
    }
}

void declare_Clover(InstVector& ivector, FVec diag[6], FVec offdiag[15][2]) {
    for(int s=0; s < 6; s++) {
        declareFVecFromFVec(ivector, diag[s]);
    }
    for(int s=0; s < 15; s++) {
        declareFVecFromFVec(ivector, offdiag[s][RE]);
        declareFVecFromFVec(ivector, offdiag[s][IM]);
    }
}

void movCVec(InstVector& ivector, FVec *r, FVec *s1, string &mask)
{
    movFVec(ivector, r[RE], s1[RE], mask);
    movFVec(ivector, r[IM], s1[IM], mask);
}

void addCVec(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, string &mask)
{
    addFVec(ivector, r[RE], s1[RE], s2[RE], mask);
    addFVec(ivector, r[IM], s1[IM], s2[IM], mask);
}

void subCVec(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, string &mask)
{
    subFVec(ivector, r[RE], s1[RE], s2[RE], mask);
    subFVec(ivector, r[IM], s1[IM], s2[IM], mask);
}

void addiCVec(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, string &mask)
{
    subFVec(ivector, r[RE], s1[RE], s2[IM], mask);
    addFVec(ivector, r[IM], s1[IM], s2[RE], mask);
}

void subiCVec(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, string &mask)
{
    addFVec(ivector, r[RE], s1[RE], s2[IM], mask);
    subFVec(ivector, r[IM], s1[IM], s2[RE], mask);
}

// r[RE] = s1[RE]-beta_vec*s2[RE] = fnmadd(beta_vec,s2[RE],s1[RE])
// r[IM] = s1[IM]-beta_vec*s2[IM] = fnamdd(beta_vec,s2[IM],s1[IM])
void addCVec_mbeta(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask)
{
    fnmaddFVec(ivector, r[RE], beta_vec, s2[RE], s1[RE], mask);
    fnmaddFVec(ivector, r[IM], beta_vec, s2[IM], s1[IM], mask);
}
// r[RE] = s1[RE] + beta_vec*s2[RE] = fmadd(beta_vec, s2[RE], s1[RE]);
// r[IM] = s1[IM] + beta_vec*s2[IM] = fmadd(beta_vec, s2[IM], s1[IM]);
void subCVec_mbeta(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask)
{
    fmaddFVec(ivector, r[RE], beta_vec, s2[RE], s1[RE], mask);
    fmaddFVec(ivector, r[IM], beta_vec, s2[IM], s1[IM], mask);
}

// r[RE] = s1[RE] + beta_vec * s2[IM] = fmadd(beta_vec,s2[IM], s1[RE])
// r[IM] = s1[IM] - beta_vec * s2[RE] = fnmadd(beta_vec, s2[RE], s1[IM])

void addiCVec_mbeta(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask)
{
    fmaddFVec(ivector, r[RE], beta_vec, s2[IM], s1[RE], mask);
    fnmaddFVec(ivector, r[IM], beta_vec, s2[RE], s1[IM], mask);
}

// r[RE] = s1[RE] - beta_vec*s2[IM] = fnmadd( beta_vec, s2[IM], s1[RE]);
// r[IM] = s1[IM] + beta_vec*s2[RE] = fmadd ( beta_vec, s2[RE], s1[IM]);
void subiCVec_mbeta(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask)
{
    fnmaddFVec(ivector, r[RE], beta_vec, s2[IM], s1[RE], mask);
    fmaddFVec(ivector, r[IM], beta_vec, s2[RE], s1[IM], mask);
}

// r[RE] = s1[RE]+beta_vec*s2[RE] = fmadd(beta_vec,s2[RE],s1[RE])
// r[IM] = s1[IM]+beta_vec*s2[IM] = fmadd(beta_vec,s2[IM],s1[IM])
void addCVec_pbeta(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask)
{
    fmaddFVec(ivector, r[RE], beta_vec, s2[RE], s1[RE], mask);
    fmaddFVec(ivector, r[IM], beta_vec, s2[IM], s1[IM], mask);
}
// r[RE] = s1[RE] - beta_vec*s2[RE] = fnmadd(beta_vec, s2[RE], s1[RE]);
// r[IM] = s1[IM] - beta_vec*s2[IM] = fnmadd(beta_vec, s2[IM], s1[IM]);
void subCVec_pbeta(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask)
{
    fnmaddFVec(ivector, r[RE], beta_vec, s2[RE], s1[RE], mask);
    fnmaddFVec(ivector, r[IM], beta_vec, s2[IM], s1[IM], mask);
}

// r[RE] = s1[RE] - beta_vec * s2[IM] = fnmadd(beta_vec,s2[IM], s1[RE])
// r[IM] = s1[IM] + beta_vec * s2[RE] = fmadd(beta_vec, s2[RE], s1[IM])
void addiCVec_pbeta(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask)
{
    fnmaddFVec(ivector, r[RE], beta_vec, s2[IM], s1[RE], mask);
    fmaddFVec(ivector, r[IM], beta_vec, s2[RE], s1[IM], mask);
}

// r[RE] = s1[RE] + beta_vec*s2[IM] = fmadd( beta_vec, s2[IM], s1[RE]);
// r[IM] = s1[IM] - beta_vec*s2[RE] = fnmadd ( beta_vec, s2[RE], s1[IM]);
void subiCVec_pbeta(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask)
{
    fmaddFVec(ivector, r[RE], beta_vec, s2[IM], s1[RE], mask);
    fnmaddFVec(ivector, r[IM], beta_vec, s2[RE], s1[IM], mask);
}

// r = s1*s2
void mulCVec(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, string &mask)
{
    mulFVec(ivector, r[RE], s1[RE], s2[RE], mask);
    fnmaddFVec(ivector, r[RE], s1[IM], s2[IM], r[RE], mask);
    mulFVec(ivector, r[IM], s1[RE], s2[IM], mask);
    fmaddFVec(ivector, r[IM], s1[IM], s2[RE], r[IM], mask);
}

// r = s1*s2+s3
void fmaddCVec(InstVector& ivector, FVec *r, FVec *s1, FVec *s2,  FVec *s3, string &mask)
{
    fmaddFVec(ivector, r[RE], s1[RE], s2[RE], s3[RE], mask);
    fnmaddFVec(ivector, r[RE], s1[IM], s2[IM], r[RE], mask);
    fmaddFVec(ivector, r[IM], s1[RE], s2[IM], s3[IM], mask);
    fmaddFVec(ivector, r[IM], s1[IM], s2[RE], r[IM], mask);
}

// r = s3-s1*s2
//r[RE] = (s3[RE]-s1[RE]*s2[RE])+(s1[IM]*s2[IM])
//r[IM] = (s3[IM]-s1[RE]*s2[IM])-(s1[IM]*s2[RE])
void fnmaddCVec(InstVector& ivector, FVec *r, FVec *s1, FVec *s2,  FVec *s3, string &mask)
{
    fnmaddFVec(ivector, r[RE], s1[RE], s2[RE], s3[RE], mask);
    fmaddFVec(ivector, r[RE], s1[IM], s2[IM], r[RE], mask);
    fnmaddFVec(ivector, r[IM], s1[RE], s2[IM], s3[IM], mask);
    fnmaddFVec(ivector, r[IM], s1[IM], s2[RE], r[IM], mask);
}

// r = (s1*s2-s3*s4)'
//r[RE] = (s1[RE]*s2[RE])-(s1[IM]*s2[IM])-(s3[RE]*s4[RE])+(s3[IM]*s4[IM])
//r[IM] = (s3[RE]*s4[IM])+(s3[IM]*s4[RE])-(s1[RE]*s2[IM])-(s1[IM]*s2[RE])
void Conj_CrossProd(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec *s3, FVec *s4, string &mask)
{
    mulFVec(ivector, r[RE], s1[RE], s2[RE], mask);
    fnmaddFVec(ivector, r[RE], s1[IM], s2[IM], r[RE], mask);
    fnmaddFVec(ivector, r[RE], s3[RE], s4[RE], r[RE], mask);
    fmaddFVec(ivector, r[RE], s3[IM], s4[IM], r[RE], mask);

    mulFVec(ivector, r[IM], s3[RE], s4[IM], mask);
    fmaddFVec(ivector, r[IM], s3[IM], s4[RE], r[IM], mask);
    fnmaddFVec(ivector, r[IM], s1[RE], s2[IM], r[IM], mask);
    fnmaddFVec(ivector, r[IM], s1[IM], s2[RE], r[IM], mask);
}

// r = s1'*s2
//r[RE] = (s1[RE]*s2[RE])+(s1[IM]*s2[IM])
//r[IM] = (s1[RE]*s2[IM])-(s1[IM]*s2[RE])
void mulConjCVec(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, string &mask)
{
    mulFVec(ivector, r[RE], s1[RE], s2[RE], mask);
    fmaddFVec(ivector, r[RE], s1[IM], s2[IM], r[RE], mask);
    mulFVec(ivector, r[IM], s1[RE], s2[IM], mask);
    fnmaddFVec(ivector, r[IM], s1[IM], s2[RE], r[IM], mask);
}

// r = s1'*s2+s3
//r[RE] = (s3[RE]+s1[RE]*s2[RE])+(s1[IM]*s2[IM])
//r[IM] = (s3[IM]+s1[RE]*s2[IM])-(s1[IM]*s2[RE])
void fmaddConjCVec(InstVector& ivector, FVec *r, FVec *s1, FVec *s2,  FVec *s3, string &mask)
{
    fmaddFVec(ivector, r[RE], s1[RE], s2[RE], s3[RE], mask);
    fmaddFVec(ivector, r[RE], s1[IM], s2[IM], r[RE], mask);
    fmaddFVec(ivector, r[IM], s1[RE], s2[IM], s3[IM], mask);
    fnmaddFVec(ivector, r[IM], s1[IM], s2[RE], r[IM], mask);
}

// r[1][3] = s_vec[1][3] * u_mat[3][3](')
// Transposed matrix vector multiplication
void matMultVecT(InstVector& ivector, FVec r[3][2], FVec s_vec[3][2], FVec u_mat[3][3][2], bool adjMul, string mask)
{
    for(int c1 = 0; c1 < 3; c1++) {
        if(!adjMul) {
            mulCVec(ivector, r[c1], u_mat[0][c1], s_vec[0], mask);
            fmaddCVec(ivector, r[c1], u_mat[1][c1], s_vec[1], r[c1], mask);
            fmaddCVec(ivector, r[c1], u_mat[2][c1], s_vec[2], r[c1], mask);
        }
        else {
            mulConjCVec(ivector, r[c1], u_mat[c1][0], s_vec[0], mask);
            fmaddConjCVec(ivector, r[c1], u_mat[c1][1], s_vec[1], r[c1], mask);
            fmaddConjCVec(ivector, r[c1], u_mat[c1][2], s_vec[2], r[c1], mask);
        }
    }
}

// r[3][1] = u_mat[3][3](') * s_vec[3][1]
void matMultVec(InstVector& ivector, FVec r[3][2], FVec u_mat[3][3][2], FVec s_vec[3][2], bool adjMul, string mask)
{
    for(int c1 = 0; c1 < 3; c1++) {
        if(!adjMul) {
            mulCVec(ivector, r[c1], u_mat[c1][0], s_vec[0], mask);
            fmaddCVec(ivector, r[c1], u_mat[c1][1], s_vec[1], r[c1], mask);
            fmaddCVec(ivector, r[c1], u_mat[c1][2], s_vec[2], r[c1], mask);
        }
        else {
            mulConjCVec(ivector, r[c1], u_mat[0][c1], s_vec[0], mask);
            fmaddConjCVec(ivector, r[c1], u_mat[1][c1], s_vec[1], r[c1], mask);
            fmaddConjCVec(ivector, r[c1], u_mat[2][c1], s_vec[2], r[c1], mask);
        }
    }
}

// u_ret = u_mat1 * u_mat2(')
void matMultMat(InstVector& ivector, FVec u_ret[3][3][2], FVec u_mat1[3][3][2], FVec u_mat2[3][3][2], bool adjMul, string mask)
{
    for(int i = 0; i < 3; i++) {
        matMultVecT(ivector, u_ret[i], u_mat1[i], u_mat2, adjMul, mask);
    }
}

// u_ret = u_mat1' * u_mat2
void adjMatMultMat(InstVector& ivector, FVec u_ret[3][3][2], FVec u_mat1[3][3][2], FVec u_mat2[3][3][2], string mask)
{
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            mulConjCVec(ivector, u_ret[i][j], u_mat1[0][i], u_mat2[0][j], mask);
            fmaddConjCVec(ivector, u_ret[i][j], u_mat1[1][i], u_mat2[1][j], u_ret[i][j], mask);
            fmaddConjCVec(ivector, u_ret[i][j], u_mat1[2][i], u_mat2[2][j], u_ret[i][j], mask);
        }
    }
}

void decompressGauge(InstVector& ivector, FVec u_gauge[3][3][2], bool compress12, string mask)
{
    if( compress12 ) {
        for(int c = 0; c < 3; c++) {
            Conj_CrossProd(ivector, u_gauge[2][c], u_gauge[0][(c+1)%3], u_gauge[1][(c+2)%3], u_gauge[0][(c+2)%3], u_gauge[1][(c+1)%3], mask);
        }
    }
}

// Merge L2 prefetches with another instruction stream
void mergeIvectorWithL2Prefetches(InstVector& ivector, InstVector& l2prefs)
{
    if( l2prefs.size() == 0 ) {
        cout << "No L2 Prefetches. Returning ivector unchanged" << endl;
    }
    else {

        if( ivector.size() == 0 ) {
            cout << "No actual instructions to merge. Returning " << endl;
            return;
        }


        int ivector_size = ivector.size();
        vector<Instruction*>::iterator it;

        // Skip past first declarations
        it=ivector.begin();
        while ( (*it)->numDeclarations() > 0 ) {
            it++;
            ivector_size--;
        }

        int n_prefs = l2prefs.size();

        //cout << "After declarations Ivector size is " << ivector_size << endl;
        //cout << "PrefetchL2 size is " << n_prefs << endl;

        int spacing_factor = ivector_size / n_prefs;
        //cout << "Spacing Factor is  " << spacing_factor << endl;
        int pref=0;
        for(int i=0; i < n_prefs; i++) {
            it = ivector.insert(it, l2prefs[pref]);
            pref++;
            it++;
            int j=spacing_factor;
            while (j > 0) {
                it++;
                j--;
            }
        }
    }
}

// Dump an instruction stream into a file
void dumpIVector(InstVector& ivector, string filename)
{
    // Check if we have any gather instructions and if we need to convert
    // indices in an offset array to an index register before dumping the instructions
    std::map<string,string> offslist;
    for(int i=0; i < ivector.size(); i++) {
        Instruction *inst = ivector[i];
        if ( inst->hasAddress() ) {
            MemRefInstruction* mr = dynamic_cast< MemRefInstruction* >(inst);
            if(mr->hasGSAddress()) {
                const GatherAddress* ga = dynamic_cast<const GatherAddress *>(mr->getAddress());
                string offs = ga->getOffsets(false);
                string voffs = ga->getOffsets ();
                if(offslist.find(offs) == offslist.end())
                    offslist[offs] = voffs;
            }
        }
    }
    for ( map<string,string>::iterator  it = offslist.begin(); it != offslist.end(); ++it ) {
        ivector.insert(ivector.begin(), new DeclareOffsets(it->first, it->second ));
    }

    ofstream outfile(filename.c_str());
    for(int i=0; i < ivector.size(); i++) {
        outfile << ivector[i]->serialize() << endl;
    }
    outfile.close();
}
