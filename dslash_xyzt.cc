#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <typeinfo>
#include <string>

using namespace std;

#include "dslash_common.h"
#include "data_types_xyzt.h"

typedef struct {
    const char *name;
    int s[2][2];
    void (*CVecFunc[2])(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, string &mask);
} proj_ops;

typedef struct {
    const char *name;
    int s2;
    int s3;
    void (*CVecFuncTop2)(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask);
    void (*CVecFunc1)(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask);
    void (*CVecFunc2)(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask);
} recons_ops;

proj_ops proj_ops_plus[] = {
    {"plus_X_back", {{0,3},{1,2}},{addiCVec,addiCVec}},
    {"minus_X_forw",{{0,3},{1,2}},{subiCVec,subiCVec}},
    {"plus_Y",      {{0,3},{1,2}},{subCVec, addCVec}},
    {"minus_Y",     {{0,3},{1,2}},{addCVec, subCVec}},
    {"plus_Z",      {{0,2},{1,3}},{addiCVec,subiCVec}},
    {"minus_Z",     {{0,2},{1,3}},{subiCVec,addiCVec}},
    {"plus_T",      {{0,2},{1,3}},{addCVec, addCVec}},
    {"minus_T",     {{0,2},{1,3}},{subCVec, subCVec}}
};

proj_ops proj_ops_minus[] = {
    {"minus_X_back",{{0,3},{1,2}},{subiCVec,subiCVec}},
    {"plus_X_forw", {{0,3},{1,2}},{addiCVec,addiCVec}},
    {"minus_Y",     {{0,3},{1,2}},{addCVec, subCVec}},
    {"plus_Y",      {{0,3},{1,2}},{subCVec, addCVec}},
    {"minus_Z",     {{0,2},{1,3}},{subiCVec,addiCVec}},
    {"plus_Z",      {{0,2},{1,3}},{addiCVec,subiCVec}},
    {"minus_T",     {{0,2},{1,3}},{subCVec, subCVec}},
    {"plus_T",      {{0,2},{1,3}},{addCVec, addCVec}}
};

recons_ops rec_plus_pbeta_ops[] = {
    {"recons_plus_X_pbeta", 1,0, addCVec_pbeta, subiCVec_pbeta, subiCVec_pbeta},
    {"recons_plus_Y_pbeta", 1,0, addCVec_pbeta, addCVec_pbeta,  subCVec_pbeta},
    {"recons_plus_Z_pbeta", 0,1, addCVec_pbeta, subiCVec_pbeta, addiCVec_pbeta},
    {"recons_plus_T_pbeta", 0,1, addCVec_pbeta, addCVec_pbeta, addCVec_pbeta},
};

recons_ops rec_minus_pbeta_ops[] = {
    {"recons_minus_X", 1,0, addCVec_pbeta, addiCVec_pbeta, addiCVec_pbeta},
    {"recons_minus_Y", 1,0, addCVec_pbeta, subCVec_pbeta,  addCVec_pbeta},
    {"recons_minus_Z", 0,1, addCVec_pbeta, addiCVec_pbeta, subiCVec_pbeta},
    {"recons_minus_T", 0,1, addCVec_pbeta, subCVec_pbeta,  subCVec_pbeta},
};

recons_ops rec_plus_mbeta_ops[] = {
    {"recons_plus_X_mbeta", 1,0, addCVec_mbeta, subiCVec_mbeta, subiCVec_mbeta},
    {"recons_plus_Y_mbeta", 1,0, addCVec_mbeta, addCVec_mbeta,  subCVec_mbeta},
    {"recons_plus_Z_mbeta", 0,1, addCVec_mbeta, subiCVec_mbeta, addiCVec_mbeta},
    {"recons_plus_T_mbeta", 0,1, addCVec_mbeta, addCVec_mbeta, addCVec_mbeta},
};

recons_ops rec_minus_mbeta_ops[] = {
    {"recons_minus_X", 1,0, addCVec_mbeta, addiCVec_mbeta, addiCVec_mbeta},
    {"recons_minus_Y", 1,0, addCVec_mbeta, subCVec_mbeta,  addCVec_mbeta},
    {"recons_minus_Z", 0,1, addCVec_mbeta, addiCVec_mbeta, subiCVec_mbeta},
    {"recons_minus_T", 0,1, addCVec_mbeta, subCVec_mbeta,  subCVec_mbeta},
};

string dirname[2] = {"back", "forw"};
string dimchar[4] = {"X", "Y", "Z", "T"};

string basename = "neighs";
string beta_names[8] = {"coeff_s", "coeff_s", "coeff_s", "coeff_s", "coeff_s", "coeff_s", "coeff_t_b", "coeff_t_f"};

string beta_name("beta");
string alpha_name("alpha");
string outBase("oBase");
string gBase("gBase");
string chiBase("chiBase");
string clBase("clBase");

FVec b_spinor[2][3][2] = {
    { {FVec("b_S0_C0_RE"), FVec("b_S0_C0_IM")}, {FVec("b_S0_C1_RE"), FVec("b_S0_C1_IM")}, {FVec("b_S0_C2_RE"), FVec("b_S0_C2_IM")} },
    { {FVec("b_S1_C0_RE"), FVec("b_S1_C0_IM")}, {FVec("b_S1_C1_RE"), FVec("b_S1_C1_IM")}, {FVec("b_S1_C2_RE"), FVec("b_S1_C2_IM")} }
};

FVec ub_spinor[2][3][2] = {
    { {FVec("ub_S0_C0_RE"), FVec("ub_S0_C0_IM")}, {FVec("ub_S0_C1_RE"), FVec("ub_S0_C1_IM")}, { FVec("ub_S0_C2_RE"), FVec("ub_S0_C2_IM") } },
    { {FVec("ub_S1_C0_RE"), FVec("ub_S1_C0_IM")}, {FVec("ub_S1_C1_RE"), FVec("ub_S1_C1_IM")}, { FVec("ub_S1_C2_RE"), FVec("ub_S1_C2_IM") } }
};

FVec out_spinor[4][3][2] = {
    { {FVec("out_S0_C0_RE"), FVec("out_S0_C0_IM")}, {FVec("out_S0_C1_RE"), FVec("out_S0_C1_IM")}, { FVec("out_S0_C2_RE"), FVec("out_S0_C2_IM") } },
    { {FVec("out_S1_C0_RE"), FVec("out_S1_C0_IM")}, {FVec("out_S1_C1_RE"), FVec("out_S1_C1_IM")}, { FVec("out_S1_C2_RE"), FVec("out_S1_C2_IM") } },
    { {FVec("out_S2_C0_RE"), FVec("out_S2_C0_IM")}, {FVec("out_S2_C1_RE"), FVec("out_S2_C1_IM")}, { FVec("out_S2_C2_RE"), FVec("out_S2_C2_IM") } },
    { {FVec("out_S3_C0_RE"), FVec("out_S3_C0_IM")}, {FVec("out_S3_C1_RE"), FVec("out_S3_C1_IM")}, { FVec("out_S3_C2_RE"), FVec("out_S3_C2_IM") } }
};

FVec clout_spinor[2][6][2] = {
    {   {FVec("out_S0_C0_RE"), FVec("out_S0_C0_IM")}, {FVec("out_S0_C1_RE"), FVec("out_S0_C1_IM")}, { FVec("out_S0_C2_RE"), FVec("out_S0_C2_IM") },
        {FVec("out_S1_C0_RE"), FVec("out_S1_C0_IM")}, {FVec("out_S1_C1_RE"), FVec("out_S1_C1_IM")}, { FVec("out_S1_C2_RE"), FVec("out_S1_C2_IM") }
    },
    {   {FVec("out_S2_C0_RE"), FVec("out_S2_C0_IM")}, {FVec("out_S2_C1_RE"), FVec("out_S2_C1_IM")}, { FVec("out_S2_C2_RE"), FVec("out_S2_C2_IM") },
        {FVec("out_S3_C0_RE"), FVec("out_S3_C0_IM")}, {FVec("out_S3_C1_RE"), FVec("out_S3_C1_IM")}, { FVec("out_S3_C2_RE"), FVec("out_S3_C2_IM") }
    }
};

FVec chi_spinor[4][3][2] = {
    { {FVec("chi_S0_C0_RE"), FVec("chi_S0_C0_IM")}, {FVec("chi_S0_C1_RE"), FVec("chi_S0_C1_IM")}, { FVec("chi_S0_C2_RE"), FVec("chi_S0_C2_IM") } },
    { {FVec("chi_S1_C0_RE"), FVec("chi_S1_C0_IM")}, {FVec("chi_S1_C1_RE"), FVec("chi_S1_C1_IM")}, { FVec("chi_S1_C2_RE"), FVec("chi_S1_C2_IM") } },
    { {FVec("chi_S2_C0_RE"), FVec("chi_S2_C0_IM")}, {FVec("chi_S2_C1_RE"), FVec("chi_S2_C1_IM")}, { FVec("chi_S2_C2_RE"), FVec("chi_S2_C2_IM") } },
    { {FVec("chi_S3_C0_RE"), FVec("chi_S3_C0_IM")}, {FVec("chi_S3_C1_RE"), FVec("chi_S3_C1_IM")}, { FVec("chi_S3_C2_RE"), FVec("chi_S3_C2_IM") } }
};

FVec dout_spinor[4][3][2] = {
    { {FVec("dout_S0_C0_RE"), FVec("dout_S0_C0_IM")}, {FVec("dout_S0_C1_RE"), FVec("dout_S0_C1_IM")}, { FVec("dout_S0_C2_RE"), FVec("dout_S0_C2_IM") } },
    { {FVec("dout_S1_C0_RE"), FVec("dout_S1_C0_IM")}, {FVec("dout_S1_C1_RE"), FVec("dout_S1_C1_IM")}, { FVec("dout_S1_C2_RE"), FVec("dout_S1_C2_IM") } },
    { {FVec("dout_S2_C0_RE"), FVec("dout_S2_C0_IM")}, {FVec("dout_S2_C1_RE"), FVec("dout_S2_C1_IM")}, { FVec("dout_S2_C2_RE"), FVec("dout_S2_C2_IM") } },
    { {FVec("dout_S3_C0_RE"), FVec("dout_S3_C0_IM")}, {FVec("dout_S3_C1_RE"), FVec("dout_S3_C1_IM")}, { FVec("dout_S3_C2_RE"), FVec("dout_S3_C2_IM") } }
};

FVec clov_diag[6] = { FVec("cl_diag_0"), FVec("cl_diag_1"), FVec("cl_diag_2"), FVec("cl_diag_3"), FVec("cl_diag_4"), FVec("cl_diag_5") };

FVec clov_offdiag[15][2]= {
    { FVec("cl_offdiag_0_RE"), FVec("cl_offdiag_0_IM") },
    { FVec("cl_offdiag_1_RE"), FVec("cl_offdiag_1_IM") },
    { FVec("cl_offdiag_2_RE"), FVec("cl_offdiag_2_IM") },
    { FVec("cl_offdiag_3_RE"), FVec("cl_offdiag_3_IM") },
    { FVec("cl_offdiag_4_RE"), FVec("cl_offdiag_4_IM") },
    { FVec("cl_offdiag_5_RE"), FVec("cl_offdiag_5_IM") },
    { FVec("cl_offdiag_6_RE"), FVec("cl_offdiag_6_IM") },
    { FVec("cl_offdiag_7_RE"), FVec("cl_offdiag_7_IM") },
    { FVec("cl_offdiag_8_RE"), FVec("cl_offdiag_8_IM") },
    { FVec("cl_offdiag_9_RE"), FVec("cl_offdiag_9_IM") },
    { FVec("cl_offdiag_10_RE"), FVec("cl_offdiag_10_IM") },
    { FVec("cl_offdiag_11_RE"), FVec("cl_offdiag_11_IM") },
    { FVec("cl_offdiag_12_RE"), FVec("cl_offdiag_12_IM") },
    { FVec("cl_offdiag_13_RE"), FVec("cl_offdiag_13_IM") },
    { FVec("cl_offdiag_14_RE"), FVec("cl_offdiag_14_IM") }
};

FVec zero("zero");
FVec alpha_vec("alpha_vec");
FVec beta_vec("beta_vec");

FVec psi[2][2] = { {FVec("psi_S0_RE"), FVec("psi_S0_IM")}, {FVec("psi_S1_RE"), FVec("psi_S1_IM")} };

FVec tmp_1_re("tmp_1_re");
FVec tmp_1_im("tmp_1_im");
FVec tmp_2_re("tmp_2_re");
FVec tmp_2_im("tmp_2_im");

FVec tmp[4] = {FVec("tmp_1_re"), FVec("tmp_1_im"), FVec("tmp_2_re"), FVec("tmp_2_im")};

FVec u_gauge[3][3][2] = {
    { {FVec("u_00_re"), FVec("u_00_im")}, {FVec("u_01_re"), FVec("u_01_im")}, {FVec("u_02_re"), FVec("u_02_im")} },
    { {FVec("u_10_re"), FVec("u_10_im")}, {FVec("u_11_re"), FVec("u_11_im")}, {FVec("u_12_re"), FVec("u_12_im")} },
    { {FVec("u_20_re"), FVec("u_20_im")}, {FVec("u_21_re"), FVec("u_21_im")}, {FVec("u_22_re"), FVec("u_22_im")} }
};

void declare_misc(InstVector& ivector) {
    declareFVecFromFVec(ivector, psi[0][0]);
    declareFVecFromFVec(ivector, psi[0][1]);
    declareFVecFromFVec(ivector, psi[1][0]);
    declareFVecFromFVec(ivector, psi[1][1]);

    declareFVecFromFVec(ivector, tmp_1_re);
    declareFVecFromFVec(ivector, tmp_1_im);
    declareFVecFromFVec(ivector, tmp_2_re);
    declareFVecFromFVec(ivector, tmp_2_im);

    declareFVecFromFVec(ivector, zero);
    setZero(ivector, zero);
}

#if 1
void generateFacePackL2Prefetches(InstVector& ivector, int dir)
{
    PrefetchL2HalfSpinorDir(ivector, "lBuf", "rBuf", "hsprefdist", dir, true, 2 /* Ex*/);
    PrefetchL2FullSpinorDirIn(ivector, "siBase", "si_prefdist");
}

void generateFaceUnpackL2Prefetches(InstVector& ivector, int dir, bool compress12, bool clover)
{
    PrefetchL2HalfSpinorDir(ivector, "lBuf", "rBuf", "hsprefdist", dir, false, 0 /* None*/);
    PrefetchL2FullGaugeDirIn(ivector, "gBase", dir, "gprefdist", compress12);

    if(clover)	{
        PrefetchL2FullCloverIn(ivector, "clBase", "clprefdist");
    }

    PrefetchL2FullSpinorDirIn(ivector, outBase, "soprefdist");
}


// Generate all L2 prefetches
void generateL2Prefetches(InstVector& ivector, bool compress12, bool chi, bool clover)
{
    PrefetchL2FullSpinorDirIn(ivector, "siBase", "siprefdist1");
    //PrefetchL2FullSpinorDirIn(ivector, "pfBase1", "offs", "siprefdist1");
    PrefetchL2FullSpinorDirIn(ivector, "pfBase2", "siprefdist2");
    PrefetchL2FullSpinorDirIn(ivector, "pfBase3", "siprefdist3");
    PrefetchL2FullSpinorDirIn(ivector, "pfBase4", "siprefdist4");

    if(clover)	{
        PrefetchL2FullCloverIn(ivector, "clBase", "clprefdist");
    }

    if(chi) {
        PrefetchL2FullSpinorDirIn(ivector, "pfBaseChi", "chiprefdist");
    }

    PrefetchL2FullGaugeIn(ivector, "gBase", "gprefdist", compress12);
    PrefetchL2FullSpinorOut(ivector, outBase, "siprefdist4");
}
#endif

// offset and isFace not used
void project(InstVector& ivector, string base, proj_ops& ops, int dir)
{
    string tmask("");
    //PrefetchL1FullSpinorDirIn(ivector, base, offset, dir);
    for(int s = 0; s < 2; s++) {
        for(int c = 0; c < 3; c++) {
            LoadSpinorElement(ivector, psi[0][RE], base, ops.s[s][0], c, RE, dir);
            LoadSpinorElement(ivector, psi[0][IM], base, ops.s[s][0], c, IM, dir);
            LoadSpinorElement(ivector, psi[1][RE], base, ops.s[s][1], c, RE, dir);
            LoadSpinorElement(ivector, psi[1][IM], base, ops.s[s][1], c, IM, dir);

            ops.CVecFunc[s](ivector, b_spinor[s][c], psi[0], psi[1], tmask); // Not using mask here
        }
    }
}

// Serial Spin version
// offset and isFace not used
void project(InstVector& ivector, string base, proj_ops& ops, int dir, int s)
{
    string tmask("");
    //if(s==0) PrefetchL1FullSpinorDirIn(ivector, base, offset, dir);
    for(int c = 0; c < 3; c++) {
        LoadSpinorElement(ivector, psi[0][RE], base, ops.s[s][0], c, RE, dir);
        LoadSpinorElement(ivector, psi[0][IM], base, ops.s[s][0], c, IM, dir);
        LoadSpinorElement(ivector, psi[1][RE], base, ops.s[s][1], c, RE, dir);
        LoadSpinorElement(ivector, psi[1][IM], base, ops.s[s][1], c, IM, dir);

        ops.CVecFunc[s](ivector, b_spinor[s][c], psi[0], psi[1], /*mask*/ tmask); // Not using mask here
    }
}

void recons_add(InstVector& ivector, recons_ops& ops, FVec outspinor[4][3][2])
{
	string mask;
    for(int s=0; s < 2; s++) {
        for(int c = 0; c < 3; c++) {
            ops.CVecFuncTop2(ivector, outspinor[s][c], outspinor[s][c], ub_spinor[s][c], beta_vec, mask);
        }
        if(ops.s2 == s) {
            for(int c=0; c < 3; c++) {
                ops.CVecFunc1(ivector, outspinor[2][c], outspinor[2][c], ub_spinor[s][c], beta_vec, mask);
            }
        }
        else {
            for(int c=0; c < 3; c++) {
                ops.CVecFunc2(ivector, outspinor[3][c], outspinor[3][c], ub_spinor[s][c], beta_vec, mask);
            }
        }
    }
}

// Serial Spin version
void recons_add(InstVector& ivector, recons_ops& ops, FVec outspinor[4][3][2], int s)
{
	string mask;
    for(int c = 0; c < 3; c++) {
        ops.CVecFuncTop2(ivector, outspinor[s][c], outspinor[s][c], ub_spinor[s][c], beta_vec, mask);
    }
    if(ops.s2 == s) {
        for(int c=0; c < 3; c++) {
            ops.CVecFunc1(ivector, outspinor[2][c], outspinor[2][c], ub_spinor[s][c], beta_vec, mask);
        }
    }
    else {
        for(int c=0; c < 3; c++) {
            ops.CVecFunc2(ivector, outspinor[3][c], outspinor[3][c], ub_spinor[s][c], beta_vec, mask);
        }
    }
}

void zeroResult(InstVector& ivector, FVec *outspinor)
{
    for(int i=0; i < 24; i++) {
        setZero(ivector,outspinor[i]);
    }
}

void clover_term(InstVector& ivector, FVec in_spinor[4][3][2], bool face)
{
    FVec clout_tmp[2] = {tmp_1_re, tmp_1_im};

    for(int block=0; block < 2; block++) {
        //PrefetchL1FullCloverBlockIn(ivector, clBase, clOffs, block);
        LoadFullCloverBlock(ivector, clov_diag, clov_offdiag, clBase, block);
        for(int c1=0; c1 < 6; c1++) {
            int spin = 2*block+c1/3;
            int col = c1 % 3;
            bool acc = face;
            string mask;
            FVec *clout = out_spinor[spin][col];
            FVec *clin  = in_spinor[spin][col];
            if( acc ) {
                fmaddFVec( ivector, clout[RE], clov_diag[c1], clin[RE], clout[RE], mask);
                fmaddFVec( ivector, clout[IM], clov_diag[c1], clin[IM], clout[IM], mask);
            }
            else {
                mulFVec( ivector,  clout[RE], clov_diag[c1],  clin[RE], mask);
                mulFVec( ivector,  clout[IM], clov_diag[c1],  clin[IM], mask);
            }
            for(int c2=0; c2 < 6; c2++) {
                if(c1 == c2) continue; // diagonal case
                if(c1 < c2) {
                    int od = c2*(c2-1)/2+c1;
                    fmaddConjCVec(ivector, clout, clov_offdiag[od], in_spinor[2*block+c2/3][c2%3], clout, mask);
                }
                else {
                    int od = c1*(c1-1)/2+c2;
                    fmaddCVec(ivector, clout, clov_offdiag[od], in_spinor[2*block+c2/3][c2%3], clout, mask);
                }
            }
        }
    }
}

void achiResult(InstVector& ivector, bool clover)
{
    //PrefetchL1FullSpinorDirIn(ivector, chiBase, chiOffs, -1, 1 /*NTA*/);

    if(!clover) {
        for(int col=0; col < 3; col++) {
            for(int spin=0; spin < 4; spin++) {
                LoadSpinorElement(ivector, tmp_1_re, chiBase, spin, col, RE);
                LoadSpinorElement(ivector, tmp_1_im, chiBase, spin, col, IM);
                mulFVec(ivector, out_spinor[spin][col][RE], alpha_vec, tmp_1_re);
                mulFVec(ivector, out_spinor[spin][col][IM], alpha_vec, tmp_1_im);
            }
        }
    }
    else {
        for(int col=0; col < 3; col++) {
            for(int spin=0; spin < 4; spin++) {
                LoadSpinorElement(ivector, chi_spinor[spin][col][RE], chiBase, spin, col, RE);
                LoadSpinorElement(ivector, chi_spinor[spin][col][IM], chiBase, spin, col, IM);
            }
        }
        // Apply clover term, and store result in out spinor.
        // This is only on the AChi - bDPsi op (achimbdpsi = true)
        // This is only in body kernel (face = false)
        clover_term(ivector, chi_spinor, false);
    }
}

void loadGaugeDir(InstVector& ivector, int dir, bool compress12) {
    string mask;

    //PrefetchL1FullGaugeDirIn(ivector, gBase, dir, compress12);
    LoadFullGaugeDir(ivector, u_gauge, gBase, dir, compress12);

    decompressGauge(ivector, u_gauge, compress12, mask);
}

void matMultVecT(InstVector& ivector, bool adjMul, int s)
{
    string mask;
    matMultVecT(ivector, ub_spinor[s], b_spinor[s], u_gauge, adjMul, mask);
}

void matMultVecT(InstVector& ivector, bool adjMul)
{
    string mask;
    matMultVecT(ivector, ub_spinor[0], b_spinor[0], u_gauge, adjMul, mask);
    matMultVecT(ivector, ub_spinor[1], b_spinor[1], u_gauge, adjMul, mask);
}

#ifdef SERIAL_SPIN
void dslash_body(InstVector& ivector, bool compress12, proj_ops *ops, recons_ops *rec_ops_bw, recons_ops *rec_ops_fw, FVec outspinor[4][3][2])
{
    for(int dim = 0; dim < 4; dim++) {
        for(int dir = 0; dir < 2; dir++) {
            int d = dim * 2 + dir;
            stringstream d_str;
            d_str << d;
            bool adjMul;
            recons_ops rec_op;

            if(dir == 0) {
                adjMul = true;
                rec_op = rec_ops_bw[dim];
            }
            else {
                adjMul = false;
                rec_op = rec_ops_fw[dim];
            }

            ifStatement(ivector,"accumulate[" + d_str.str() + "]");
            {
                declareFVecFromFVec(ivector, beta_vec);
                loadBroadcastScalar(ivector, beta_vec, beta_names[d], SpinorType);

                for(int s = 0; s < 2; s++) {
                    project(ivector, basename + "[" + d_str.str() + "]", ops[d], d, s);
					ifStatement(ivector, "isBoundary[" + d_str.str() + "]");
					{
						for(int c = 0; c < 3; c++) 
							for(int ri = 0; ri < 2; ri++) 
								permuteXYZTFVec(ivector, b_spinor[s][c][ri], b_spinor[s][c][ri], d);
					}
                    endScope(ivector);

                    if(s==0) {
                        loadGaugeDir(ivector, d, compress12);
                    }

                    matMultVecT(ivector, adjMul, s);
                    recons_add(ivector, rec_op, outspinor, s);
                }

            }
            endScope(ivector);
        }
    }
}
#else // NO SERIAL_SPIN
void dslash_body(InstVector& ivector, bool compress12, proj_ops *ops, recons_ops *rec_ops_bw, recons_ops *rec_ops_fw, FVec outspinor[4][3][2])
{
    for(int dim = 0; dim < 4; dim++) {
        for(int dir = 0; dir < 2; dir++) {
            int d = dim * 2 + dir;
            stringstream d_str;
            d_str << d;
            bool adjMul;
            recons_ops rec_op;

            adjMul = (dir == 0 ? true : false);
            rec_op = (dir == 0 ? rec_ops_bw[dim] : rec_ops_fw[dim]);

            ifStatement(ivector,"accumulate[" + d_str.str() + "]");
            {
                declareFVecFromFVec(ivector, beta_vec);
                loadBroadcastScalar(ivector, beta_vec, beta_names[d], SpinorType);

                project(ivector, basename + "[" + d_str.str() + "]", ops[d], d);
				ifStatement(ivector, "isBoundary[" + d_str.str() + "]");
				{
	                for(int s = 0; s < 2; s++)
						for(int c = 0; c < 3; c++) 
							for(int ri = 0; ri < 2; ri++) 
								permuteXYZTFVec(ivector, b_spinor[s][c][ri], b_spinor[s][c][ri], d);
				}
                endScope(ivector);
                loadGaugeDir(ivector, d, compress12);
                matMultVecT(ivector, adjMul);
                recons_add(ivector, rec_op, outspinor);
            }
            endScope(ivector);
        }
    }
}
#endif // SERIAL_SPIN



// need siBase to specify input spinor
// need outbuf for output half spinor
void pack_face_vec(InstVector& ivector, FVec spinor[2][3][2], proj_ops proj[], int dir)
{
    std::string l_out("lBuf");
    std::string r_out("rBuf");
    //PrefetchL1HalfSpinorDir(ivector, out, dir, true, 2 /*Exclusive*/);

    // We need to reverse direction of projection for our neighbor
    int fb = (dir % 2 == 0 ? 1 : -1);
    project(ivector, "siBase", proj[dir+fb], dir);

    // This will write it to outbuf
    PackHalfSpinor(ivector, spinor, l_out, r_out, dir);
}

// need inbuf pointer to half spinor
// need gBase and goffs to point to gauge
// need obase and offs to point to spinor to scatter.
void recons_add_face_vec(InstVector& ivector, bool compress12, bool adjMul, recons_ops rops[], int dir, int dim, bool clover)
{

    std::string l_in("lBuf");
    std::string r_in("rBuf");

    extern FVec out_spinor[4][3][2];
    extern FVec dout_spinor[4][3][2];
    extern FVec b_spinor[2][3][2];

    int gauge_index = dim * 2 + dir;

    declareFVecFromFVec(ivector, beta_vec);
    loadBroadcastScalar(ivector, beta_vec, beta_name, SpinorType);

    FVec (*outspinor)[4][3][2];

    if(clover) {
        outspinor = &dout_spinor;
        zeroResult(ivector, (*outspinor)[0][0]);
    }
    else {
        outspinor = &out_spinor;
    }

    //PrefetchL1HalfSpinorDir(ivector, in, dir, false, 0 /*None*/);
    // Gather in the partial result
    //PrefetchL1FullSpinorDirIn(ivector, outBase, outOffs, -1);
    LoadFullSpinor(ivector, out_spinor, outBase);

    // load b-from inbuf
    UnpackHalfSpinor(ivector, b_spinor, l_in, r_in, gauge_index);

    loadGaugeDir(ivector, gauge_index, compress12);
    matMultVecT(ivector, adjMul);
    recons_add(ivector, rops[dim], *outspinor);

    if(clover) {
        clover_term(ivector, *outspinor, true);
    }

    // scatter it out
    StoreFullSpinor(ivector, out_spinor, outBase);
}


string getTypeName(size_t s)
{
    if(s == 2) {
        return "half";
    }
    else if(s == 4) {
        return "float";
    }
    else if(s == 8) {
        return "double";
    }
    else {
        return "Unknown";
    }
}

// Uses ub_spinor as implicit input
// Uses psi[][] as temp and b_spinor[][] as implicit output
void dslash_plain_body(InstVector& ivector, bool compress12, bool clover, bool isPlus)
{
    declare_HalfSpinor(ivector, b_spinor);
    declare_HalfSpinor(ivector, ub_spinor);
    declare_Gauge(ivector, u_gauge);
    declare_misc(ivector);

    declare_WilsonSpinor(ivector, out_spinor);
    if(clover) {
        declare_WilsonSpinor(ivector, dout_spinor);
        declare_Clover(ivector, clov_diag, clov_offdiag);
    }

    FVec (*outspinor)[3][2];
    if(clover)
        outspinor = dout_spinor;
    else
        outspinor = out_spinor;

    zeroResult(ivector, outspinor[0][0]);

    proj_ops *p_ops;
    recons_ops *rec_ops_bw;
    recons_ops *rec_ops_fw;

    if(isPlus) {
        p_ops 		= proj_ops_plus;
        rec_ops_bw	= rec_plus_pbeta_ops;
        rec_ops_fw 	= rec_minus_pbeta_ops;
    }
    else {
        p_ops 		= proj_ops_minus;
        rec_ops_bw	= rec_minus_pbeta_ops;
        rec_ops_fw 	= rec_plus_pbeta_ops;
    }

    dslash_body(ivector, compress12, p_ops, rec_ops_bw, rec_ops_fw, outspinor);

    if(clover) clover_term(ivector, outspinor, false);

    // Store
    StreamFullSpinor(ivector, out_spinor, outBase);
}

// ***** ------- a chi - b D psi versions
void dslash_achimbdpsi_body(InstVector& ivector, bool compress12, bool clover, bool isPlus)
{
    declare_HalfSpinor(ivector, b_spinor);
    declare_HalfSpinor(ivector, ub_spinor);
    declare_Gauge(ivector, u_gauge);
    declare_misc(ivector);

    declare_WilsonSpinor(ivector, out_spinor);
    declare_WilsonSpinor(ivector, chi_spinor);

    if(clover) {
        declare_WilsonSpinor(ivector, dout_spinor);
        declare_Clover(ivector, clov_diag, clov_offdiag);
    }
    else {
        declareFVecFromFVec(ivector, alpha_vec);
        loadBroadcastScalar(ivector, alpha_vec, alpha_name, SpinorType);
    }

    // Fill result with a*chi
    achiResult(ivector, clover);

    proj_ops *p_ops;
    recons_ops *rec_ops_bw;
    recons_ops *rec_ops_fw;

    if(isPlus) {
        p_ops 		= proj_ops_plus;
        rec_ops_bw	= rec_plus_mbeta_ops;
        rec_ops_fw 	= rec_minus_mbeta_ops;
    }
    else {
        p_ops 		= proj_ops_minus;
        rec_ops_bw	= rec_minus_mbeta_ops;
        rec_ops_fw 	= rec_plus_mbeta_ops;
    }

    dslash_body(ivector, compress12, p_ops, rec_ops_bw, rec_ops_fw, out_spinor);

    // Store
    StreamFullSpinor(ivector, out_spinor, outBase);
}

void pack_face_to_dir_dim_vec(InstVector& ivector, bool isPlus, int dir, int dim)
{
    declare_HalfSpinor(ivector, b_spinor);
    declare_misc(ivector);

    proj_ops *p_ops = (isPlus == true ? proj_ops_plus : proj_ops_minus);
    pack_face_vec(ivector, b_spinor, p_ops, 2*dim+dir);
}

void recons_add_face_from_dir_dim_vec(InstVector& ivector, bool compress12, bool isPlus, int dir, int dim, bool clover)
{
    declare_HalfSpinor(ivector, b_spinor);
    declare_HalfSpinor(ivector, ub_spinor);
    declare_WilsonSpinor(ivector, out_spinor);
    declare_Gauge(ivector, u_gauge);
    declare_misc(ivector);

    if(clover) {
        declare_WilsonSpinor(ivector, dout_spinor);
        declare_Clover(ivector, clov_diag, clov_offdiag);
    }

    bool isBack = (dir == 0 ? true : false);
    recons_ops *rec_ops;
    if(clover)
        rec_ops = (isPlus == isBack ? rec_plus_pbeta_ops : rec_minus_pbeta_ops);
    else
        rec_ops = (isPlus == isBack ? rec_plus_mbeta_ops : rec_minus_mbeta_ops);

    recons_add_face_vec(ivector, compress12, isBack, rec_ops, dir, dim, clover);
}

int main(void)
{
    InstVector ivector;
    InstVector l2prefs;
    bool compress12;

    const string SpinorTypeName = getTypeName(sizeof(SpinorBaseType));
    const string GaugeTypeName = getTypeName(sizeof(GaugeBaseType));
    const string CloverTypeName = getTypeName(sizeof(CloverBaseType));

    for(int isign=-1; isign<=1; isign+=2) {
        bool isPlus = (isign == 1 ? true : false);
        string plusminus = (isPlus) ? "plus" : "minus";

        for(int clov = 0; clov < 2; clov++) {
            bool clover = (clov == 1 ? true : false);
            string clov_prefix = (clover ? "clov_"+CloverTypeName+"_" : "");

            for(int num_components=12; num_components <=18; num_components+=6) {
                compress12 = ( num_components==12 );

                std::ostringstream filename;
                filename << "./"<<ARCH_NAME<<"/" << clov_prefix << "dslash_"<<plusminus<<"_body_" << SpinorTypeName << "_" << GaugeTypeName << "_v"<< VECLEN <<"_s"<<""<<"_"<< num_components;
                l2prefs.resize(0);
                generateL2Prefetches(l2prefs,compress12, false, clover);
                //dumpIVector(l2prefs, "prefetches.out");

                // Dslash Plus
                cout << "GENERATING dslash_"<<plusminus<<"_vec body" << endl;
                // Flush instruction list
                ivector.resize(0);

                // Generate instructions
                dslash_plain_body(ivector,compress12,clover,isPlus);
                mergeIvectorWithL2Prefetches(ivector, l2prefs);
                dumpIVector(ivector,filename.str());

                filename.str("");
                filename.clear();
                filename << "./"<<ARCH_NAME<<"/" << clov_prefix << "dslash_achimbdpsi_"<<plusminus<<"_body_" << SpinorTypeName << "_" << GaugeTypeName << "_v"<< VECLEN <<"_s"<<""<<"_"<< num_components;

                l2prefs.resize(0);
                generateL2Prefetches(l2prefs,compress12, true, clover);
                //dumpIVector(l2prefs, "prefetches.out");
                cout << "GENERATING dslash_achimbdpsi_"<<plusminus<<"_vec body" << endl;
                // Flush instruction list
                ivector.resize(0);

                // Generate instructions
                dslash_achimbdpsi_body(ivector,compress12,clover,isPlus);
                //mergeIvectorWithL2Prefetches(ivector, l2prefs);
                dumpIVector(ivector,filename.str());

                for(int dir = 0; dir < 2; dir++) {
                    for(int dim = 0; dim < 4; dim++) {
                        std::ostringstream filename;
                        filename << "./"<<ARCH_NAME<<"/" << clov_prefix << "dslash_face_unpack_from_"<<dirname[dir]<<"_"<<dimchar[dim]<<"_" << plusminus <<"_" << SpinorTypeName << "_" << GaugeTypeName << "_v"<< VECLEN <<"_s"<<""<<"_"<<num_components;
                        cout << "GENERATING face unpack file " << filename.str() << endl;
                        ivector.resize(0);
                        recons_add_face_from_dir_dim_vec(ivector, compress12,isPlus, dir, dim, clover);
                        l2prefs.resize(0);
                        generateFaceUnpackL2Prefetches(l2prefs, 2*dim+dir, compress12, clover);
                        //mergeIvectorWithL2Prefetches(ivector, l2prefs);
                        dumpIVector(ivector,filename.str());
                    }
                }
            }
        }

        for(int dir = 0; dir < 2; dir++) {
            for(int dim = 0; dim < 4; dim++) {
                std::ostringstream filename;
                filename << "./"<<ARCH_NAME<<"/dslash_face_pack_to_"<<dirname[dir]<<"_"<<dimchar[dim]<<"_"<<plusminus<<"_" << SpinorTypeName << "_" << GaugeTypeName << "_v"<< VECLEN <<"_s"<<"";
                cout << "GENERATING face pack file " << filename.str() << endl;
                l2prefs.resize(0);
                generateFacePackL2Prefetches(l2prefs, 2*dim+dir);
                ivector.resize(0);
                pack_face_to_dir_dim_vec(ivector,isPlus,dir,dim);
                //mergeIvectorWithL2Prefetches(ivector, l2prefs);
                dumpIVector(ivector,filename.str());
            }
        }
    }

    return 0;
}
