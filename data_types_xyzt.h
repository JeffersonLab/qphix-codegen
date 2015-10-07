
#ifndef _DATA_TYPES_H_
#define _DATA_TYPES_H_

#include "instructions.h"

#if PRECISION == 1
#define FPTYPE float
#define LPTYPE short
#elif PRECISION == 2
#define FPTYPE double
#define LPTYPE float
#else
#error "Invalid FP PRECISION"
#endif

#ifndef USE_LP_SPINOR
typedef FPTYPE SpinorBaseType;
#define SpinorType	0
#else
typedef LPTYPE SpinorBaseType;
#define SpinorType	1
#endif

#ifndef USE_LP_GAUGE
typedef FPTYPE GaugeBaseType;
#define GaugeType	0
#else
typedef LPTYPE GaugeBaseType;
#define GaugeType	1
#endif

#ifndef USE_LP_CLOVER
typedef FPTYPE CloverBaseType;
#define CloverType	0
#else
typedef LPTYPE CloverBaseType;
#define CloverType	1
#endif

template<typename FT, int VL, bool compress12>
class data_types
{
public:
    typedef FT Spinor[4][3][2][VL];
    typedef FT Gauge[8][(compress12? 2 : 3)][3][2][VL];
    typedef struct {
        FT diag1[6][VL];
        FT off_diag1[15][2][VL];
        FT diag2[6][VL];
        FT off_diag2[15][2][VL];
    } Clover;
};

void LoadSpinorElement(InstVector& ivector, const FVec& ret, const string& base, int spin, int col, int reim, int dir = -1);
void LoadFullSpinor(InstVector& ivector, const FVec ret[4][3][2], const string& base);
void StoreFullSpinor(InstVector& ivector, const FVec ret[4][3][2], const string& base, int isStreaming = 0);
void StreamFullSpinor(InstVector& ivector, const FVec ret[4][3][2], const string& base);
void PackHalfSpinor(InstVector& ivector, const FVec ret[2][3][2], const string& lBase, const string& rBase, int dir);
void UnpackHalfSpinor(InstVector& ivector, const FVec ret[2][3][2], const string& lBase, const string& rBase, int dir);
void LoadFullGaugeDir(InstVector& ivector, const FVec ret[3][3][2], const string& base, int dir, bool compress12);
void LoadFullCloverBlock(InstVector& ivector, const FVec diag[6], const FVec off_diag[15][2], const string& base, int block);
void LoadKSElement(InstVector& ivector, const FVec& ret, const string& base, int col, int reim, int dir);
void LoadFullKS(InstVector& ivector, const FVec ret[3][2], const string& base);
void StoreFullKS(InstVector& ivector, const FVec ret[3][2], const string& base, int isStreaming=0);
void StreamFullKS(InstVector& ivector, const FVec ret[3][2], const string& base);
void PackKSSpinor(InstVector& ivector, const FVec ret[3][2], const string& lBase, const string& rBase, int dir);
void UnpackKSSpinor(InstVector& ivector, const FVec ret[3][2], const string& lBase, const string& rBase, int dir);

void PrefetchL1FullSpinorDirIn(InstVector& ivector, const string& base, int dir = -1, int type = 0);
void PrefetchL1FullSpinorOut(InstVector& ivector, const string& base);
void PrefetchL2FullSpinorDirIn(InstVector& ivector, const string& base, const string& pref_dist, int dir=-1);
void PrefetchL2FullSpinorOut(InstVector& ivector, const string& base, const string& pref_dist);
void PrefetchL1FullKSDirIn(InstVector& ivector, const string& base, int dir=-1, int type=0);
void PrefetchL1FullKSOut(InstVector& ivector, const string& base);
void PrefetchL2FullKSDirIn(InstVector& ivector, const string& base, const string& pref_dist, int dir=-1);
void PrefetchL2FullKSOut(InstVector& ivector, const string& base, const string& pref_dist);
void PrefetchL1FullGaugeDirIn(InstVector& ivector, const string& base, int dir, bool compress12, int type = 0);
void PrefetchL2FullGaugeDirIn(InstVector& ivector, const string& base, int dir, const string& pref_dist, bool compress12, int type = 0);
void PrefetchL2FullGaugeIn(InstVector& ivector, const string& base, const string& pref_dist, bool compress12, int type = 0);
void PrefetchL1FullCloverBlockIn(InstVector& ivector, const string& base, int block);
void PrefetchL2FullCloverIn(InstVector& ivector, const string& base, const string& pref_dist);
void PrefetchL1HalfSpinorDir(InstVector& ivector, const string& lBase, const string& rBase, int dir, bool isPrefforWrite, int type);
void PrefetchL2HalfSpinorDir(InstVector& ivector, const string& lBase, const string& rBase, const string& pref_dist, int dir, bool isPrefforWrite, int type);
#endif // _DATA_TYPES_H_