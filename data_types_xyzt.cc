
#include "data_types.h"
#include "instructions.h"

#if 0
typedef SpinorBaseType Spinor[4][3][2][VECLEN];
typedef GaugeBaseType Gauge[8][3][3][2][VECLEN];
typedef struct {
    CloverBaseType diag1[6][VECLEN];
    CloverBaseType off_diag1[15][2][VECLEN];
    CloverBaseType diag2[6][VECLEN];
    CloverBaseType off_diag2[15][2][VECLEN];
} Clover;
#endif


void readFVecSpinor(InstVector& ivector, const FVec& ret, const string& base, int spin, int color, int reim)
{
    loadFVec(ivector, ret, new SpinorAddress(base,spin,color,reim,SpinorType), string(""));
}

void writeFVecSpinor(InstVector& ivector, const FVec& ret, const string& base, int spin, int color, int reim, int isStreaming)
{
    storeFVec(ivector, ret, new SpinorAddress(base,spin,color,reim,SpinorType), isStreaming);
}

void readFVecKS(InstVector& ivector, const FVec& ret, const string& base, int color, int reim)
{
    loadFVec(ivector, ret, new KSAddress(base,color,reim,SpinorType), string(""));
}

void writeFVecKS(InstVector& ivector, const FVec& ret, const string& base, int color, int reim, int isStreaming)
{
    storeFVec(ivector, ret, new KSAddress(base,color,reim,SpinorType), isStreaming);
}

void readFVecGauge(InstVector& ivector, const FVec& ret, const string& base, int dir, int c1, int c2, int reim)
{
    loadFVec(ivector, ret, new GaugeAddress(base,dir,c1,c2,reim,GaugeType), string(""));
}

void readFVecClovDiag(InstVector& ivector, const FVec& ret, const string& base, int block, int c)
{
    loadFVec(ivector, ret, new ClovDiagAddress(base,block,c,CloverType), string(""));
}

void readFVecClovOffDiag(InstVector& ivector, const FVec& ret, const string& base, int block, int c, int reim)
{
    loadFVec(ivector, ret, new ClovOffDiagAddress(base,block,c,reim,CloverType), string(""));
}

void LoadSpinorElement(InstVector& ivector, const FVec& ret, const string& base, int spin, int col, int reim, int dir)
{
    readFVecSpinor(ivector, ret, base, spin, col, reim);
}

void LoadFullSpinor(InstVector& ivector, const FVec ret[4][3][2], const string& base)
{
    for(int col=0; col < 3; col++) {
        for(int spin=0; spin < 4; spin++) {
            LoadSpinorElement(ivector, ret[spin][col][RE], base, spin, col, RE, -1);
            LoadSpinorElement(ivector, ret[spin][col][IM], base, spin, col, IM, -1);
        }
    }
}

void StoreFullSpinor(InstVector& ivector, const FVec ret[4][3][2], const string& base, int isStreaming)
{

#ifndef ENABLE_STREAMING_STORES
    isStreaming = 0;
#endif

    for(int spin=0; spin < 4; spin++) {
        for(int col=0; col < 3; col++) {
            writeFVecSpinor(ivector, ret[spin][col][RE], base, spin, col, RE, isStreaming);
            writeFVecSpinor(ivector, ret[spin][col][IM], base, spin, col, IM, isStreaming);
        }
    }
}

void StreamFullSpinor(InstVector& ivector, const FVec ret[4][3][2], const string& base)
{
    StoreFullSpinor(ivector, ret, base, 1);
}

void PackHalfSpinor(InstVector& ivector, const FVec ret[2][3][2], const string& lBase, const string& rBase, int dir)
{
	int lpackoffset = 0, rpackoffset = 0;
    for(int spin=0; spin < 2; spin++) {
        for(int col=0; col < 3; col++) {
			int rsz = packXYZTFVec(ivector, ret[spin][col], new AddressImm(new GenericAddress(lBase, SpinorType), lpackoffset),
				new AddressImm(new GenericAddress(rBase, SpinorType), rpackoffset), dir);
			rpackoffset += rsz;
			if(rsz == VECLEN) lpackoffset += rsz;
		}
	}
}

void UnpackHalfSpinor(InstVector& ivector, const FVec ret[2][3][2], const string& lBase, const string& rBase, int dir)
{
	int lunpackoffset = 0, runpackoffset = 0;
    for(int spin=0; spin < 2; spin++) {
        for(int col=0; col < 3; col++) {
			int rsz = unpackXYZTFVec(ivector, ret[spin][col], new AddressImm(new GenericAddress(lBase, SpinorType), lunpackoffset),
				new AddressImm(new GenericAddress(rBase, SpinorType), runpackoffset), dir);
			runpackoffset += rsz;
			if(rsz == VECLEN) lunpackoffset += rsz;
		}
	}
}

void LoadFullGaugeDir(InstVector& ivector, const FVec ret[3][3][2], const string& base, int dir, bool compress12)
{
    int nrows = 3;

    if(compress12 == true) {
        nrows = 2;
    }

    for(int c1=0; c1 < nrows; c1++) {
        for(int c2=0; c2 < 3; c2++) {
            readFVecGauge(ivector, ret[c1][c2][RE], base, dir, c1, c2, RE);
            readFVecGauge(ivector, ret[c1][c2][IM], base, dir, c1, c2, IM);
        }
    }
}

void LoadFullCloverBlock(InstVector& ivector, const FVec diag[6], const FVec off_diag[15][2], const string& base, int block)
{
    for(int c=0; c < 6; c++) {
        readFVecClovDiag(ivector, diag[c], base, block, c);
    }

    for(int c=0; c < 15; c++) {
        readFVecClovOffDiag(ivector, off_diag[c][RE], base, block, c, RE);
        readFVecClovOffDiag(ivector, off_diag[c][IM], base, block, c, IM);
    }
}


void LoadKSElement(InstVector& ivector, const FVec& ret, const string& base, int col, int reim, int dir)
{
    readFVecKS(ivector, ret, base, col, reim);
}
void LoadFullKS(InstVector& ivector, const FVec ret[3][2], const string& base)
{
    for(int col=0; col < 3; col++) {
        LoadKSElement(ivector, ret[col][RE], base, col, RE, -1);
        LoadKSElement(ivector, ret[col][IM], base, col, IM, -1);
    }
}


void StoreFullKS(InstVector& ivector, const FVec ret[3][2], const string& base, int isStreaming)
{

#ifndef ENABLE_STREAMING_STORES
    isStreaming = 0;
#endif

    for(int col=0; col < 3; col++) {
        writeFVecKS(ivector, ret[col][RE], base, col, RE, isStreaming);
        writeFVecKS(ivector, ret[col][IM], base, col, IM, isStreaming);
    }
}

void StreamFullKS(InstVector& ivector, const FVec ret[3][2], const string& base)
{
    StoreFullKS(ivector, ret, base, 1);
}

void PackKSSpinor(InstVector& ivector, const FVec ret[3][2], const string& lBase, const string& rBase, int dir)
{
	int lpackoffset = 0, rpackoffset = 0;
    for(int col=0; col < 3; col++) {
		int rsz = packXYZTFVec(ivector, ret[col], new AddressImm(new GenericAddress(lBase, SpinorType), lpackoffset),
			new AddressImm(new GenericAddress(rBase, SpinorType), rpackoffset), dir);
		rpackoffset += rsz;
		if(rsz == VECLEN) lpackoffset += rsz;
	}
}

void UnpackKSSpinor(InstVector& ivector, const FVec ret[3][2], const string& lBase, const string& rBase, int dir)
{
	int lunpackoffset = 0, runpackoffset = 0;
    for(int col=0; col < 3; col++) {
		int rsz = unpackXYZTFVec(ivector, ret[col], new AddressImm(new GenericAddress(lBase, SpinorType), lunpackoffset),
			new AddressImm(new GenericAddress(rBase, SpinorType), runpackoffset), dir);
		runpackoffset += rsz;
		if(rsz == VECLEN) lunpackoffset += rsz;
	}
}


// Prefetches

void prefetchL1SpinorIn(InstVector& ivector, string base, int imm, int dir, int type)
{
#ifdef PREF_L1_SPINOR_IN
    PrefetchL1(ivector, new AddressImm(new SpinorAddress(base,0,0,0,SpinorType), imm), type, dir);
#endif
}

void prefetchL2SpinorIn(InstVector& ivector, string base, const string& pref_dist, int imm, int dir)
{
#ifdef PREF_L2_SPINOR_IN
    PrefetchL2(ivector, new AddressImm(new AddressOffset(new SpinorAddress(base,0,0,0,SpinorType), pref_dist), imm));
#endif
}

void prefetchL1SpinorOut(InstVector& ivector, string base, int imm)
{
#ifdef PREF_L1_SPINOR_OUT
    PrefetchL1(ivector, new AddressImm(new SpinorAddress(base,0,0,0,SpinorType), imm), 3 /*NT & EX */);
#endif
}

void prefetchL2SpinorOut(InstVector& ivector, string base, const string& pref_dist, int imm)
{
#ifdef PREF_L2_SPINOR_OUT
    PrefetchL2(ivector, new AddressImm(new AddressOffset(new SpinorAddress(base,0,0,0,SpinorType), pref_dist), imm), 3 /*NT & EX */);
#endif
}

void prefetchL1KSIn(InstVector& ivector, string base, int imm, int dir, int type)
{
#ifdef PREF_L1_SPINOR_IN
    PrefetchL1(ivector, new AddressImm(new KSAddress(base,0,0,SpinorType), imm), type, dir);
#endif
}

void prefetchL2KSIn(InstVector& ivector, string base, const string& pref_dist, int imm, int dir)
{
#ifdef PREF_L2_SPINOR_IN
    PrefetchL2(ivector, new AddressImm(new AddressOffset(new KSAddress(base,0,0,SpinorType), pref_dist), imm));
#endif
}

void prefetchL1KSOut(InstVector& ivector, string base, int imm)
{
#ifdef PREF_L1_SPINOR_OUT
    PrefetchL1(ivector, new AddressImm(new KSAddress(base,0,0,SpinorType), imm), 3 /*NT & EX */);
#endif
}

void prefetchL2KSOut(InstVector& ivector, string base, const string& pref_dist, int imm)
{
#ifdef PREF_L2_SPINOR_OUT
    PrefetchL2(ivector, new AddressImm(new AddressOffset(new KSAddress(base,0,0,SpinorType), pref_dist), imm), 3 /*NT & EX */);
#endif
}

void prefetchL1GuageDirIn(InstVector& ivector, string base, int dir, int imm, int type)
{
#ifdef PREF_L1_GAUGE
    prefetchL1(ivector, new AddressImm(new GaugeAddress(base,dir,0,0,0,GaugeType), imm), type);
#endif
}

void prefetchL2GuageDirIn(InstVector& ivector, string base, int dir, const string& pref_dist, int imm, int type)
{
#ifdef PREF_L2_GAUGE
    prefetchL2(ivector, new AddressImm(new AddressOffset(new GaugeAddress(base,dir,0,0,0,GaugeType), pref_dist), imm), type);
#endif
}

void prefetchL1CloverBlockIn(InstVector& ivector, string base, int block, int imm)
{
#ifdef PREF_L1_CLOVER
    prefetchL1(ivector, new AddressImm(new ClovDiagAddress(base,block,0,CloverType), imm), 0);
#endif
}

void prefetchL2CloverBlockIn(InstVector& ivector, string base, int block, const string& pref_dist, int imm)
{
#ifdef PREF_L2_CLOVER
    prefetchL2(ivector, new AddressImm(new AddressOffset(new ClovDiagAddress(base,block,0,CloverType), pref_dist), imm), 0);
#endif
}

void PrefetchL1FullSpinorDirIn(InstVector& ivector, const string& base, int dir, int type)
{
    // for now we ignore direction but it can be used for specialization
    for(int i = 0; i < (24*VECLEN*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL1SpinorIn(ivector, base, i*(64/sizeof(SpinorBaseType)), dir, type);
    }
}

void PrefetchL1FullSpinorOut(InstVector& ivector, const string& base, const string& off)
{
    for(int i = 0; i < (24*VECLEN*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL1SpinorOut(ivector, base, i*(64/sizeof(SpinorBaseType)));
    }
}

void PrefetchL2FullSpinorDirIn(InstVector& ivector, const string& base, const string& pref_dist, int dir)
{
    // for now we ignore direction but itcan be used for specialization
    for(int i = 0; i < (24*VECLEN*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL2SpinorIn(ivector, base, pref_dist, i*(64/sizeof(SpinorBaseType)), dir);
    }
}

void PrefetchL2FullSpinorOut(InstVector& ivector, const string& base, const string& pref_dist)
{
    for(int i = 0; i < (24*VECLEN*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL2SpinorOut(ivector, base, pref_dist, i*(64/sizeof(SpinorBaseType)));
    }
}

void PrefetchL1FullKSDirIn(InstVector& ivector, const string& base, int dir, int type)
{
    // for now we ignore direction but it can be used for specialization
    for(int i = 0; i < (6*VECLEN*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL1KSIn(ivector, base, i*(64/sizeof(SpinorBaseType)), dir, type);
    }
}

void PrefetchL1FullKSOut(InstVector& ivector, const string& base, const string& off)
{
    for(int i = 0; i < (6*VECLEN*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL1KSOut(ivector, base, i*(64/sizeof(SpinorBaseType)));
    }
}

void PrefetchL2FullKSDirIn(InstVector& ivector, const string& base, const string& pref_dist, int dir)
{
    // for now we ignore direction but itcan be used for specialization
    for(int i = 0; i < (6*VECLEN*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL2KSIn(ivector, base, pref_dist, i*(64/sizeof(SpinorBaseType)), dir);
    }
}

void PrefetchL2FullKSOut(InstVector& ivector, const string& base, const string& pref_dist)
{
    for(int i = 0; i < (6*VECLEN*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL2KSOut(ivector, base, pref_dist, i*(64/sizeof(SpinorBaseType)));
    }
}

void PrefetchL1FullGaugeDirIn(InstVector& ivector, const string& base, int dir, bool compress12, int type)
{
    int nSites = VECLEN;
    int g_size=0;

    if( compress12 ) {
        g_size=2*3*2;
    }
    else {
        g_size=3*3*2;
    }

    for(int i = 0; i < ((g_size*nSites*sizeof(GaugeBaseType)+63)/64); i++) {
        prefetchL1GuageDirIn(ivector, base, dir, i*(64/sizeof(GaugeBaseType)), type);
    }
}

void PrefetchL2FullGaugeDirIn(InstVector& ivector, const string& base, int dir, const string& pref_dist, bool compress12, int type)
{
    int nSites = VECLEN;
    int g_size=0;

    if( compress12 ) {
        g_size=2*3*2;
    }
    else {
        g_size=3*3*2;
    }

    for(int i = 0; i < ((g_size*nSites*sizeof(GaugeBaseType)+63)/64); i++) {
        prefetchL2GuageDirIn(ivector, base, dir, pref_dist, i*(64/sizeof(GaugeBaseType)), type);
    }
}

void PrefetchL2FullGaugeIn(InstVector& ivector, const string& base, const string& pref_dist, bool compress12, int type)
{
    for(int dir = 0; dir < 8; dir++) {
        PrefetchL2FullGaugeDirIn(ivector, base, dir, pref_dist, compress12, type);
    }
}

void PrefetchL1FullCloverBlockIn(InstVector& ivector, const string& base, int block)
{
    int nSites = VECLEN;

    for(int i = 0; i < ((36*nSites*sizeof(CloverBaseType)+63)/64); i++) {
        prefetchL1CloverBlockIn(ivector, base, block, i*(64/sizeof(CloverBaseType)));
    }
}

void PrefetchL2FullCloverIn(InstVector& ivector, const string& base, const string& pref_dist)
{
    int nSites = VECLEN;

    for(int i = 0; i < ((2*36*nSites*sizeof(CloverBaseType)+63)/64); i++) {
        prefetchL2CloverBlockIn(ivector, base, 0, pref_dist, i*(64/sizeof(CloverBaseType)));
    }
}

void PrefetchL1HalfSpinorDir(InstVector& ivector, const string& lBase, const string& rBase, int dir, bool isPrefforWrite, int type)
{
	int split = 0;
	if((1 << dir/2) < VECLEN) split = 1;
	int size = (split == 1 ? VECLEN / 2 : VECLEN);

#ifdef ENABLE_STREAMING_STORES

    if(isPrefforWrite) {
        return;
    }

#endif

    for(int i = 0; i < (12*size*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL1(ivector, new AddressImm(new GenericAddress(rBase, SpinorType), i*(64/sizeof(SpinorBaseType))), type);
		if(split == 1) 
	        prefetchL1(ivector, new AddressImm(new GenericAddress(lBase, SpinorType), i*(64/sizeof(SpinorBaseType))), type);
    }
}

void PrefetchL2HalfSpinorDir(InstVector& ivector, const string& lBase, const string& rBase, const string& pref_dist, int dir, bool isPrefforWrite, int type)
{
	int split = 0;
	if((1 << dir/2) < VECLEN) split = 1;
	int size = (split == 1 ? VECLEN / 2 : VECLEN);

#ifdef ENABLE_STREAMING_STORES

    if(isPrefforWrite) {
        return;
    }

#endif

    for(int i = 0; i < (12*size*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL2(ivector, new AddressImm(new AddressOffset(new GenericAddress(rBase, SpinorType), pref_dist), i*(64/sizeof(SpinorBaseType))), type);
		if(split == 1) 
	        prefetchL2(ivector, new AddressImm(new AddressOffset(new GenericAddress(lBase, SpinorType), pref_dist), i*(64/sizeof(SpinorBaseType))), type);
    }
}

