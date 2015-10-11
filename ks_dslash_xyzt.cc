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
    void (*CVecFuncTop2)(InstVector& ivector, FVec *r, FVec *s1, FVec *s2, FVec &beta_vec, string &mask);
} recons_ops;

string dirname[2] = {"back", "forw"};
string dimchar[4] = {"X", "Y", "Z", "T"};
// Defines which dimensions are involved in SIMD blocking
// Currently just X and Y
bool requireAllOneCheck[4] = {true, true, false, false};
//bool requireAllOneCheck[4] = {false, false, false, false};

string basename = "neighs";
string basename3rd = "neighs3";
//string basenames[8] = {"xyBase", "xyBase", "xyBase", "xyBase", "zbBase", "zfBase", "tbBase", "tfBase"};
//string offsnames[8] = {"xbOffs", "xfOffs", "ybOffs", "yfOffs", "offs",   "offs",   "offs",   "offs"  };
//string basenames3rd[8] = {"xyBase", "xyBase", "xyBase", "xyBase", "z3bBase", "z3fBase", "t3bBase", "t3fBase"};
//string offsnames3rd[8] = {"x3bOffs", "x3fOffs", "y3bOffs", "y3fOffs", "offs",   "offs",   "offs",   "offs"  };

string outBase("oBase");
//string outOffs("offs");
//string gBase("gBase");
//string gOffs("gOffs");

FVec b_KS[3][2] = {
    {FVec("b_C0_RE"), FVec("b_C0_IM")}, {FVec("b_C1_RE"), FVec("b_C1_IM")}, {FVec("b_C2_RE"), FVec("b_C2_IM")}
};

FVec ub_KS[3][2] = {
    {FVec("ub_C0_RE"), FVec("ub_C0_IM")}, {FVec("ub_C1_RE"), FVec("ub_C1_IM")}, { FVec("ub_C2_RE"), FVec("ub_C2_IM") }
};

FVec out_KS[3][2] = {
    {FVec("out_C0_RE"), FVec("out_C0_IM")}, {FVec("out_C1_RE"), FVec("out_C1_IM")}, { FVec("out_C2_RE"), FVec("out_C2_IM") }
};

FVec u_gauge[3][3][2] = {
    { {FVec("u_00_re"), FVec("u_00_im")}, {FVec("u_01_re"), FVec("u_01_im")}, {FVec("u_02_re"), FVec("u_02_im")} },
    { {FVec("u_10_re"), FVec("u_10_im")}, {FVec("u_11_re"), FVec("u_11_im")}, {FVec("u_12_re"), FVec("u_12_im")} },
    { {FVec("u_20_re"), FVec("u_20_im")}, {FVec("u_21_re"), FVec("u_21_im")}, {FVec("u_22_re"), FVec("u_22_im")} }
};

FVec u1_gauge[3][3][2] = {
    { {FVec("u1_00_re"), FVec("u1_00_im")}, {FVec("u1_01_re"), FVec("u1_01_im")}, {FVec("u1_02_re"), FVec("u1_02_im")} },
    { {FVec("u1_10_re"), FVec("u1_10_im")}, {FVec("u1_11_re"), FVec("u1_11_im")}, {FVec("u1_12_re"), FVec("u1_12_im")} },
    { {FVec("u1_20_re"), FVec("u1_20_im")}, {FVec("u1_21_re"), FVec("u1_21_im")}, {FVec("u1_22_re"), FVec("u1_22_im")} }
};

FVec u2_gauge[3][3][2] = {
    { {FVec("u2_00_re"), FVec("u2_00_im")}, {FVec("u2_01_re"), FVec("u2_01_im")}, {FVec("u2_02_re"), FVec("u2_02_im")} },
    { {FVec("u2_10_re"), FVec("u2_10_im")}, {FVec("u2_11_re"), FVec("u2_11_im")}, {FVec("u2_12_re"), FVec("u2_12_im")} },
    { {FVec("u2_20_re"), FVec("u2_20_im")}, {FVec("u2_21_re"), FVec("u2_21_im")}, {FVec("u2_22_re"), FVec("u2_22_im")} }
};

FVec u3_gauge[3][3][2] = {
    { {FVec("u3_00_re"), FVec("u3_00_im")}, {FVec("u3_01_re"), FVec("u3_01_im")}, {FVec("u3_02_re"), FVec("u3_02_im")} },
    { {FVec("u3_10_re"), FVec("u3_10_im")}, {FVec("u3_11_re"), FVec("u3_11_im")}, {FVec("u3_12_re"), FVec("u3_12_im")} },
    { {FVec("u3_20_re"), FVec("u3_20_im")}, {FVec("u3_21_re"), FVec("u3_21_im")}, {FVec("u3_22_re"), FVec("u3_22_im")} }
};

FVec zero("zero");
FVec alpha_vec("alpha_vec");
FVec mOne("mOne");

FVec tmp_1_re("tmp_1_re");
FVec tmp_1_im("tmp_1_im");
FVec tmp_2_re("tmp_2_re");
FVec tmp_2_im("tmp_2_im");

FVec tmp[4] = {FVec("tmp_1_re"), FVec("tmp_1_im"), FVec("tmp_2_re"), FVec("tmp_2_im")};

void declare_misc(InstVector& ivector) {
    declareFVecFromFVec(ivector, tmp_1_re);
    declareFVecFromFVec(ivector, tmp_1_im);
    declareFVecFromFVec(ivector, tmp_2_re);
    declareFVecFromFVec(ivector, tmp_2_im);

    declareFVecFromFVec(ivector, zero);
    declareFVecFromFVec(ivector, mOne);
    setZero(ivector, zero);
    set1Const(ivector, mOne, -1.0);
}

// Generate all L2 prefetches
void generatePlainDslashL2Prefetches(InstVector& ivector, bool compress12)
{
    PrefetchL2FullKSDirIn(ivector, "xyBase", "siprefdist1");
    PrefetchL2FullKSDirIn(ivector, "pfBase2", "siprefdist2");
    PrefetchL2FullKSDirIn(ivector, "pfBase3", "siprefdist3");
    PrefetchL2FullKSDirIn(ivector, "pfBase4", "siprefdist4");
    PrefetchL2FullGaugeIn(ivector, "gBase", "gprefdist", compress12);
    PrefetchL2FullKSOut(ivector, outBase, "siprefdist4");
}

// Generate all L2 prefetches
void generateLongDslashL2Prefetches(InstVector& ivector, bool compress12, bool compress12ll)
{
    PrefetchL2FullKSDirIn(ivector, "xyBase", "siprefdist1");
    PrefetchL2FullKSDirIn(ivector, "pfBase2", "siprefdist2");
    PrefetchL2FullKSDirIn(ivector, "pfBase3", "siprefdist3");
    PrefetchL2FullKSDirIn(ivector, "pfBase4", "siprefdist4");
    PrefetchL2FullGaugeIn(ivector, "gBase", "gprefdist", compress12);
    PrefetchL2FullGaugeIn(ivector, "gllBase", "gprefdist", compress12);
    PrefetchL2FullKSOut(ivector, outBase, "siprefdist4");
}

// Generate L2 prefetches for rephase
void generateRephaseL2Prefetches(InstVector& ivector, bool compress12)
{
    PrefetchL2FullGaugeIn(ivector, "gBase", "gprefdist", compress12, 2 /* Ex */);
}

// Generate L2 prefetches for load_longlinks
void generateLoadLongLinkL2Prefetches(InstVector& ivector, bool compress12)
{
    PrefetchL2FullGaugeIn(ivector, "gBase", "gprefdist", compress12);
    PrefetchL2FullGaugeIn(ivector, "gpfBase1", "giprefdist1", compress12);
    PrefetchL2FullGaugeIn(ivector, "gpfBase2", "giprefdist2", compress12);
#ifndef ENABLE_STREAMING_STORES
    PrefetchL2FullGaugeIn(ivector, "gllBase", "gprefdist", compress12);
#endif
}

void generateFacePackL2Prefetches(InstVector& ivector, int dir)
{
    //TODO: Add prefetching logic
    //PrefetchL2HalfSpinorDir(ivector, "outbuf", "hsprefdist", dir, true, 2 /* Ex*/);
    //PrefetchL2FullSpinorDirIn(ivector, "xyBase", "offs", "si_prefdist");
}

void generatePlainDslashFaceUnpackL2Prefetches(InstVector& ivector, int dir, bool compress12)
{
    //TODO: Add prefetching logic
    //PrefetchL2HalfSpinorDir(ivector, "inbuf", "hsprefdist", dir, false, 0 /* None*/);
    //PrefetchL2FullGaugeDirIn(ivector, "gBase", "gOffs", dir, "gprefdist", compress12);
    //PrefetchL2FullSpinorDirIn(ivector, outBase, outOffs, "soprefdist");
}

void generateLongDslashFaceUnpackL2Prefetches(InstVector& ivector, int dir, bool compress12, bool compress12ll)
{
    //TODO: Add prefetching logic
    //PrefetchL2HalfSpinorDir(ivector, "inbuf", "hsprefdist", dir, false, 0 /* None*/);
    //PrefetchL2FullGaugeDirIn(ivector, "gBase", "gOffs", dir, "gprefdist", compress12);
    //PrefetchL2FullGaugeDirIn(ivector, "gllBase", "gOffs", dir, "gprefdist", compress12);
    //PrefetchL2FullSpinorDirIn(ivector, outBase, outOffs, "soprefdist");
}

// Generate face L2 prefetches for load_longlinks
void generateLoadLongLinkFacePackL2Prefetches(InstVector& ivector, bool compress12, int dim)
{
    PrefetchL2FullGaugeDirIn(ivector, "gBase", 2*dim, "gprefdist", compress12);
    PrefetchL2FullGaugeDirIn(ivector, "gBase", 2*dim+1, "gprefdist", compress12);
    //TODO: Add prefetching logic
}

void generateLoadLongLinkFaceUnpackL2Prefetches(InstVector& ivector, bool compress12, int dim)
{
    //TODO: Add prefetching logic
}



void project(InstVector& ivector, string base, int dir)
{
    //PrefetchL1FullKSDirIn(ivector, base, dir);
    for(int c = 0; c < 3; c++) {
        LoadKSElement(ivector, b_KS[c][RE], base, c, RE, dir);
        LoadKSElement(ivector, b_KS[c][IM], base, c, IM, dir);
    }
}

void recons_add(InstVector& ivector, int dir, FVec out_KS[3][2])
{
	string mask;
    for(int c = 0; c < 3; c++) {
        if(dir == 0)
            subCVec(ivector, out_KS[c], out_KS[c], ub_KS[c], mask);
        else
            addCVec(ivector, out_KS[c], out_KS[c], ub_KS[c], mask);
    }
}

void zeroResult(InstVector& ivector, FVec *out_KS)
{
    for(int i=0; i < 6; i++) {
        setZero(ivector, out_KS[i]);
    }
}

void invertGaugeSign(InstVector& ivector, FVec u_mat[3][3][2], int startRow, bool compress12, string mask)
{
    int endRow = (compress12 ? 2 : 3);
    if(startRow == 2) endRow = 3;
    for(int r = startRow; r < endRow; r++) {
        for(int c = 0; c < 3; c++) {
            mulFVec(ivector, u_mat[r][c][RE], u_gauge[r][c][RE], mOne, mask);
            mulFVec(ivector, u_mat[r][c][IM], u_gauge[r][c][IM], mOne, mask);
        }
    }
}

void reconstructGaugeSign(InstVector& ivector, FVec u_mat[3][3][2], int dim, int startRow, bool compress12)
{
    int soaMsk = 0, elseSoaMsk = 0;
    string mask("signMask");
    switch(dim) {
    case 0:
        ifStatement(ivector, "t & 1" );
        invertGaugeSign(ivector, u_mat, startRow, compress12, "");
        endScope(ivector);
        break;
    case 1:
        ifStatement(ivector, "(t + x) & 1" );
        invertGaugeSign(ivector, u_mat, startRow, compress12, "");
        endScope(ivector);
        break;
    case 2:
        ifStatement(ivector, "(t + x + y) & 1" );
        invertGaugeSign(ivector, u_mat, startRow, compress12, "");
        endScope(ivector);
        break;
    case 3:
        break;
    }
}

void loadGaugeDir(InstVector& ivector, string gBase, int dir, bool compress12) {
    string mask;

    //PrefetchL1FullGaugeDirIn(ivector, gBase, dir, compress12);
    LoadFullGaugeDir(ivector, u_gauge, gBase, dir, compress12);

    decompressGauge(ivector, u_gauge, compress12, mask);

    if(compress12) reconstructGaugeSign(ivector, u_gauge, dir/2, 2, compress12);
}

void matMultVec(InstVector& ivector, bool adjMul)
{
    string mask;
    matMultVec(ivector, ub_KS, u_gauge, b_KS, adjMul, mask);
}

void dslash_body(InstVector& ivector, string bname, string gBase, bool compress12, bool isLong, FVec out_KS[3][2])
{
    for(int dim = 0; dim < 4; dim++) {
        for(int dir = 0; dir < 2; dir++) {
            int d = dim * 2 + dir;
            stringstream d_str;
            d_str << d;
            bool adjMul;

            adjMul = (dir == 0 ? true : false);

            string accumulate = (isLong ? "accumulate3" : "accumulate");
            string isBoundary = (isLong ? "isBoundary3" : "isBoundary");

            ifStatement(ivector, accumulate + "[" + d_str.str() + "]");
            {
                project(ivector, bname + "[" + d_str.str() + "]",  d);
				ifStatement(ivector, isBoundary + "[" + d_str.str() + "]");
				{
					for(int c = 0; c < 3; c++) 
						for(int ri = 0; ri < 2; ri++) 
							permuteXYZTFVec(ivector, b_KS[c][ri], b_KS[c][ri], d);
				}
                endScope(ivector);
                loadGaugeDir(ivector, gBase, d, compress12);
                matMultVec(ivector, adjMul);
                recons_add(ivector, dir, out_KS);
            }
            endScope(ivector);
        }
    }
}

#if 0
// (A*B+)*C2  n_miu -->
void path_product(InstVector& ivector, string gBase, string gOffs, string nBase, string nOffs, string gllBase, string gllnBase, int dim, bool compress12, bool isFace=false )
{
    string in("inbuf");
    string out("outbuf");

    FVec (*A)[3][2] = u1_gauge;

    int dir = 2*dim+1; // forward in dim

    PrefetchL1FullGaugeDirIn(ivector, gBase, gOffs, dir, compress12);
    LoadFullGaugeDir(ivector, A, gBase, gOffs, dir, compress12);

    //Prefetch in backward dim
    PrefetchL1FullGaugeDirIn(ivector, gBase, gOffs, 2*dim, compress12);

    // Load gauge field B & C
    if(!isFace) {
        PrefetchL1FullGaugeDirIn(ivector, nBase, nOffs, 2*dim+0, compress12);  // backward link
        PrefetchL1FullGaugeDirIn(ivector, nBase, nOffs, 2*dim+1, compress12);  // forward link
    }

    decompressGauge(ivector, A, compress12, "");
    if(compress12) reconstructGaugeSign(ivector, A, dir/2, 2, compress12);

    FVec (*B)[3][2] = u2_gauge;
    FVec (*C)[3][2] = u3_gauge;
    FVec (*U)[3][2] = u_gauge;
    int c1, c2;

    // Gather gauge field B
    if(!isFace)
        LoadFullNeighborGaugeDirLink(ivector, B, nBase, nOffs, dir, 2*dim, compress12);
    else
        LoadOrUnpackFullNeighborGaugeDirLinkBuffer(ivector, B, nBase, nOffs, dir, 2*dim, in, 0, compress12);

    decompressGauge(ivector, B, compress12, "");
    if(compress12) reconstructGaugeSign(ivector, B, dir/2, 2, compress12);

    // U = A*B+
    matMultMat(ivector, U, A, B, true, "");

    // Gather gauge field C in to A
    if(!isFace)
        LoadFullNeighborGaugeDirLink(ivector, A, nBase, nOffs, dir, 2*dim+1, compress12);
    else
        LoadOrUnpackFullNeighborGaugeDirLinkBuffer(ivector, A, nBase, nOffs, dir, 2*dim+1, in, 1, compress12);

    decompressGauge(ivector, A, compress12, "");
    if(compress12) reconstructGaugeSign(ivector, A, dir/2, 2, compress12);

    // B = U*A
    matMultMat(ivector, B, U, A, false, "");
    // store result vector
    StreamFullGaugeDir(ivector, B, gllBase, gOffs, dir, compress12);

    // Gather gauge field C (backwards)
    LoadFullGaugeDir(ivector, A, gBase, gOffs, dir-1, compress12);

    decompressGauge(ivector, A, compress12, "");
    if(compress12) reconstructGaugeSign(ivector, A, dir/2, 2, compress12);

    // B = U+*A
    adjMatMultMat(ivector, B, U, A, "");

    // store result vector
    if(!isFace)
        StoreFullNeighborGaugeDirLink(ivector, B, gllnBase, nOffs, dir, 2*dim, compress12);
    else
        StoreOrPackFullNeighborGaugeDirLinkBuffer(ivector, B, gllnBase, nOffs, dir, 2*dim, out, 0, compress12);
}

#endif

// need siBase to specify input spinor
// need l_out and r_out or output half spinor's local and remote parts
void pack_face_vec(InstVector& ivector, int dir)
{
    std::string l_out("lBuf");
    std::string r_out("rBuf");
    //PrefetchL1KSSpinorDir(ivector, out, dir, true, 2 /*Exclusive*/);

    project(ivector, "siBase", dir);

    // This will write it to outbufs
    PackKSSpinor(ivector, b_KS, l_out, r_out, dir);
}

// need inbuf pointer to packed KS spinor
// need gBaseto point to gauge
// need obase to point to spinor to accumulate and scatter.
void plain_dslash_face_unpack_from_dir_dim_vec(InstVector& ivector, bool compress12, int dir, int dim)
{

    std::string l_in("lBuf");
    std::string r_in("rBuf");
    string gBase("gBase");

    int gauge_index = dim * 2 + dir;
    bool adjMul = (dir == 0);

    declare_KSSpinor(ivector, b_KS);
    declare_KSSpinor(ivector, ub_KS);
    declare_Gauge(ivector, u_gauge);
    declare_misc(ivector);

    declare_KSSpinor(ivector, out_KS);
    //PrefetchL1KSDir(ivector, in, dir, false, 0 /*None*/);
    // Gather in the partial result
    PrefetchL1FullKSDirIn(ivector, outBase, -1);
    LoadFullKS(ivector, out_KS, outBase);

    // load b-from inbuf
    UnpackKSSpinor(ivector, b_KS, l_in, r_in, gauge_index);

    loadGaugeDir(ivector, gBase, gauge_index, compress12);
    matMultVec(ivector, adjMul);
    recons_add(ivector, dir, out_KS);

    // scatter it out
    StoreFullKS(ivector, out_KS, outBase);
}

// need inbuf pointer to packed KS spinor
// need gBase, gllBase and goffs to point to gauge
// need obase and offs to point to spinor to accumulate and scatter.
void long_dslash_face_unpack_from_dir_dim_vec(InstVector& ivector, bool compress12, bool compress12ll, int dir, int dim)
{

    std::string l_in("lBuf");
    std::string r_in("rBuf");
    std::string l_in_ll("lBuf_ll");
    std::string r_in_ll("rBuf_ll");
    string gBase("gBase"), gllBase("gllBase");

    int gauge_index = dim * 2 + dir;
    bool adjMul = (dir == 0);

    declare_KSSpinor(ivector, b_KS);
    declare_KSSpinor(ivector, ub_KS);
    declare_Gauge(ivector, u_gauge);
    declare_misc(ivector);

    declare_KSSpinor(ivector, out_KS);

    //PrefetchL1KSDir(ivector, in, dir, false, 0 /*None*/);
    // Gather in the partial result
    //PrefetchL1FullKSDirIn(ivector, outBase, outOffs, -1);
    LoadFullKS(ivector, out_KS, outBase);

    // load b-from inbuf
    UnpackKSSpinor(ivector, b_KS, l_in, r_in, gauge_index);

    loadGaugeDir(ivector, gBase, gauge_index, compress12);
    matMultVec(ivector, adjMul);
    recons_add(ivector, dir, out_KS);

    // load b-from inbuf
    UnpackKSSpinor(ivector, b_KS, l_in_ll, r_in_ll, gauge_index);

    loadGaugeDir(ivector, gllBase, gauge_index, compress12ll);
    matMultVec(ivector, adjMul);
    recons_add(ivector, dir, out_KS);

    // scatter it out
    StoreFullKS(ivector, out_KS, outBase);
}

void dslash_plain_body(InstVector& ivector, bool compress12)
{
    declare_KSSpinor(ivector, b_KS);
    declare_KSSpinor(ivector, ub_KS);
    declare_Gauge(ivector, u_gauge);
    declare_misc(ivector);

    declare_KSSpinor(ivector, out_KS);

    zeroResult(ivector, out_KS[0]);

    dslash_body(ivector, basename, "gBase", compress12, false, out_KS);

    // Store
    StreamFullKS(ivector, out_KS, outBase);
}

void dslash_long_body(InstVector& ivector, bool compress12, bool compress12ll)
{
    declare_KSSpinor(ivector, b_KS);
    declare_KSSpinor(ivector, ub_KS);
    declare_Gauge(ivector, u_gauge);
    declare_misc(ivector);

    declare_KSSpinor(ivector, out_KS);

    zeroResult(ivector, out_KS[0]);

    dslash_body(ivector, basename, "gBase", compress12, false, out_KS);
    dslash_body(ivector, basename3rd, "gllBase", compress12ll, true, out_KS);

    // Store
    StreamFullKS(ivector, out_KS, outBase);
}

#if 0
void rephase_body( InstVector& ivector, bool compress12 )
{
    declare_Gauge(ivector, u_gauge);
    declare_misc(ivector);

    string gBase("gBase");

    // We are updating rephased gauges in place so no need to read/write in T-dim
    for(int dir = 0; dir < 6; dir++) {
        PrefetchL1FullGaugeDirIn(ivector, gBase, dir, compress12, 3 /* Type = NT & Ex */);
        LoadFullGaugeDir(ivector, u_gauge, gBase, dir, compress12);
        reconstructGaugeSign(ivector, u_gauge, dir/2, 0, compress12);
        StreamFullGaugeDir(ivector, u_gauge, gBase, dir, compress12);
    }
}
// Assuming gauge fields contain phase
void load_longlinks_body(InstVector& ivector, bool compress12)
{
    declare_Gauge(ivector, u_gauge);
    declare_Gauge(ivector, u1_gauge);
    declare_Gauge(ivector, u2_gauge);
    declare_Gauge(ivector, u3_gauge);
    declare_misc(ivector);

    ifStatement(ivector, "dirmask[0]");
    path_product(ivector, "gBase", "gOffs", "gBase", "gxfOffs", "gllBase", "gllBase", 0, compress12);
    endScope(ivector);
    ifStatement(ivector, "dirmask[1]");
    path_product(ivector, "gBase", "gOffs", "gBase", "gyfOffs", "gllBase", "gllBase", 1, compress12);
    endScope(ivector);
    ifStatement(ivector, "dirmask[2]");
    path_product(ivector, "gBase", "gOffs", "gzfBase", "gOffs", "gllBase", "gllzfBase", 2, compress12);
    endScope(ivector);
    ifStatement(ivector, "dirmask[3]");
    path_product(ivector, "gBase", "gOffs", "gtfBase", "gOffs", "gllBase", "glltfBase", 3, compress12);
    endScope(ivector);
}
#endif

void face_pack_to_dir_dim_vec(InstVector& ivector, int dir, int dim)
{
    declare_KSSpinor(ivector, b_KS);
    declare_misc(ivector);

    pack_face_vec(ivector, 2*dim+dir);
}

#if 0
void load_longlinks_face_pack_to_dim(InstVector& ivector, bool compress12, int dim)
{
    declare_Gauge(ivector, u_gauge);
    declare_misc(ivector);

    LoadFullGaugeDir(ivector, u_gauge, "gBase", "gOffs", 2*dim, compress12);
    PackGaugeDir(ivector, u_gauge, 2*dim+1, "outbuf", 0, compress12);
    LoadFullGaugeDir(ivector, u_gauge, "gBase", "gOffs", 2*dim+1, compress12);
    PackGaugeDir(ivector, u_gauge, 2*dim+1, "outbuf", 1, compress12);
}

void load_longlinks_face_unpack_from_dim(InstVector& ivector, bool compress12, int dim)
{
    declare_Gauge(ivector, u_gauge);
    declare_Gauge(ivector, u1_gauge);
    declare_Gauge(ivector, u2_gauge);
    declare_Gauge(ivector, u3_gauge);
    declare_misc(ivector);

    path_product(ivector, "gBase", "gOffs", "gBase", "gfOffs", "gllBase", "gllBase", dim, compress12, true);
}
#endif

string getTypeName(size_t s)
{
    if(s == 2) return "half";
    else if(s == 4) return "float";
    else if(s == 8) return "double";
    else return "Unknown";
}

int main(void)
{
    InstVector ivector;
    InstVector l2prefs;
    bool compress12;

    const string SpinorTypeName = getTypeName(sizeof(SpinorBaseType));
    const string GaugeTypeName = getTypeName(sizeof(GaugeBaseType));

    if(SOALEN == VECLEN) requireAllOneCheck[1] = false;

#ifdef NO_MASKS
    for(int i = 0; i < 4; i++) requireAllOneCheck[i] = false;
#endif

    for(int num_components=12; num_components <=18; num_components+=6) {
        compress12 = ( num_components==12 );

        std::ostringstream filename;
        // Dslash
        filename << "./"<<ARCH_NAME<<"/" << "ks_dslash_body_" << SpinorTypeName << "_" << GaugeTypeName << "_v"<< VECLEN <<"_s"<<"_"<< num_components;
        cout << "GENERATING " << filename.str() << endl;
        l2prefs.resize(0);
        ivector.resize(0);
        generatePlainDslashL2Prefetches(l2prefs,compress12);
        dslash_plain_body(ivector,compress12);
        mergeIvectorWithL2Prefetches(ivector, l2prefs);
        dumpIVector(ivector,filename.str());

        filename.str("");
        filename.clear();
        // Long Dslash
        filename << "./"<<ARCH_NAME<<"/" << "ks_long_dslash_body_" << SpinorTypeName << "_" << GaugeTypeName << "_v"<< VECLEN <<"_s"<<"_"<< num_components;
        cout << "GENERATING " << filename.str() << endl;
        l2prefs.resize(0);
        ivector.resize(0);
        generateLongDslashL2Prefetches(l2prefs,false,compress12);
        dslash_long_body(ivector,false,compress12);
        mergeIvectorWithL2Prefetches(ivector, l2prefs);
        dumpIVector(ivector,filename.str());
#if 0
        filename.str("");
        filename.clear();
        // rephase
        filename << "./"<<ARCH_NAME<<"/" << "ks_rephase_body_" << GaugeTypeName << "_v"<< VECLEN <<"_s"<<"_"<< num_components;
        cout << "GENERATING " << filename.str() << endl;
        l2prefs.resize(0);
        ivector.resize(0);
        generateRephaseL2Prefetches(l2prefs,compress12);
        rephase_body(ivector,compress12);
        mergeIvectorWithL2Prefetches(ivector, l2prefs);
        dumpIVector(ivector,filename.str());

		filename.str("");
        filename.clear();
        // load long links
        filename << "./"<<ARCH_NAME<<"/" << "ks_load_longlinks_body_" << GaugeTypeName << "_v"<< VECLEN <<"_s"<<"_"<< num_components;
        cout << "GENERATING " << filename.str() << endl;
        l2prefs.resize(0);
        ivector.resize(0);
        generateLoadLongLinkL2Prefetches(l2prefs,compress12);
        load_longlinks_body(ivector,compress12);
        mergeIvectorWithL2Prefetches(ivector, l2prefs);
        dumpIVector(ivector,filename.str());
#endif

        for(int dim = 0; dim < 4; dim++) {
            std::ostringstream filename;
            for(int dir = 0; dir < 2; dir++) {
                filename.str("");
                filename.clear();
                filename << "./"<<ARCH_NAME<<"/ks_dslash_face_unpack_from_"<<dirname[dir]<<"_"<<dimchar[dim]<<"_" << SpinorTypeName << "_" << GaugeTypeName << "_v"<< VECLEN <<"_s"<<"_"<<num_components;
                cout << "GENERATING " << filename.str() << endl;
                l2prefs.resize(0);
                ivector.resize(0);
                plain_dslash_face_unpack_from_dir_dim_vec(ivector, compress12,dir,dim);
                generatePlainDslashFaceUnpackL2Prefetches(l2prefs, 2*dim+dir, compress12);
                mergeIvectorWithL2Prefetches(ivector, l2prefs);
                dumpIVector(ivector,filename.str());

                filename.str("");
                filename.clear();
                // long dslash
                filename << "./"<<ARCH_NAME<<"/ks_long_dslash_face_unpack_from_"<<dirname[dir]<<"_"<<dimchar[dim]<<"_" << SpinorTypeName << "_" << GaugeTypeName << "_v"<< VECLEN <<"_s"<<"_"<<num_components;
                cout << "GENERATING " << filename.str() << endl;
                l2prefs.resize(0);
                ivector.resize(0);
                long_dslash_face_unpack_from_dir_dim_vec(ivector, false, compress12,dir,dim);
                generateLongDslashFaceUnpackL2Prefetches(l2prefs, 2*dim+dir, false, compress12);
                mergeIvectorWithL2Prefetches(ivector, l2prefs);
                dumpIVector(ivector,filename.str());
            }

#if 0
            filename.str("");
            filename.clear();
            // load long links
            filename << "./"<<ARCH_NAME<<"/" << "ks_load_longlinks_face_pack_to_forw_"<<dimchar[dim]<<"_" << GaugeTypeName << "_v"<< VECLEN <<"_s"<<"_"<< num_components;
            cout << "GENERATING " << filename.str() << endl;
            l2prefs.resize(0);
            ivector.resize(0);
            generateLoadLongLinkFacePackL2Prefetches(l2prefs,compress12,dim);
            load_longlinks_face_pack_to_dim(ivector,compress12,dim);
            mergeIvectorWithL2Prefetches(ivector, l2prefs);
            dumpIVector(ivector,filename.str());

            filename.str("");
            filename.clear();
            // load long links
            filename << "./"<<ARCH_NAME<<"/" << "ks_load_longlinks_face_unpack_from_forw_" <<dimchar[dim]<<"_" << GaugeTypeName << "_v"<< VECLEN <<"_s"<<"_"<< num_components;
            cout << "GENERATING " << filename.str() << endl;
            l2prefs.resize(0);
            ivector.resize(0);
            generateLoadLongLinkFaceUnpackL2Prefetches(l2prefs,compress12,dim);
            load_longlinks_face_unpack_from_dim(ivector,compress12,dim);
            mergeIvectorWithL2Prefetches(ivector, l2prefs);
            dumpIVector(ivector,filename.str());
#endif
		}
    }

    for(int dir = 0; dir < 2; dir++) {
        for(int dim = 0; dim < 4; dim++) {
            std::ostringstream filename;
            filename << "./"<<ARCH_NAME<<"/ks_dslash_face_pack_to_"<<dirname[dir]<<"_"<<dimchar[dim]<<"_" << SpinorTypeName << "_v"<< VECLEN <<"_s";
            cout << "GENERATING " << filename.str() << endl;
            l2prefs.resize(0);
            ivector.resize(0);
            generateFacePackL2Prefetches(l2prefs, 2*dim+dir);
            face_pack_to_dir_dim_vec(ivector,dir,dim);
            mergeIvectorWithL2Prefetches(ivector, l2prefs);
            dumpIVector(ivector,filename.str());
        }
    }
    return 0;
}

