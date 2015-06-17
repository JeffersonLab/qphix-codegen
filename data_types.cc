
#include "data_types.h"
#include "instructions.h"

#if 0
typedef SpinorBaseType Spinor[4][3][2][SOALEN];
#ifdef USE_PACKED_GAUGES
typedef GaugeBaseType Gauge[8][3][3][2][VECLEN];
#else
typedef GaugeBaseType Gauge[8][3][3][2][SOALEN];
#endif
#ifdef USE_PACKED_CLOVER
typedef struct {
    CloverBaseType diag1[6][VECLEN];
    CloverBaseType off_diag1[15][2][VECLEN];
    CloverBaseType diag2[6][VECLEN];
    CloverBaseType off_diag2[15][2][VECLEN];
} Clover;
#else
typedef struct {
    CloverBaseType diag1[6][SOALEN];
    CloverBaseType off_diag1[15][2][SOALEN];
    CloverBaseType diag2[6][SOALEN];
    CloverBaseType off_diag2[15][2][SOALEN];
} Clover;
#endif

string serialize_data_types(bool compress12)
{
    std::ostringstream buf;
#ifdef USE_PACKED_GAUGES
    buf << "#define USE_PACKED_GAUGES" << endl;
#else
    buf << "// USE_PACKED_GAUGES not defined" << endl;
#endif

#ifdef USE_PACKED_CLOVER
    buf << "#define USE_PACKED_CLOVER" << endl;
#else
    buf << "// USE_PACKED_CLOVER not defined" << endl;
#endif

    buf << "#define PRECISION	" << PRECISION << endl;
    buf << "#define SOALEN	" << SOALEN << endl;
    buf << "#define VECLEN	" << VECLEN << endl;

    return buf.str();
}
#endif

void readFVecSpecialized(InstVector& ivector, const FVec& ret, GatherAddress *a, string mask)
{
#if (SOALEN == VECLEN) && defined(USE_SHUFFLES)
    loadFVec(ivector, ret, a->getAddr(0), mask);
#else
#ifndef USE_LDUNPK
    gatherFVec(ivector, ret, a, mask);
#else
    initFVec(ivector, ret);

    for(int i = 0; i < VECLEN; i += SOALEN) {
        loadSOAFVec(ivector, ret, a->getAddr(i), i/SOALEN, SOALEN);
    }

#endif
#endif
}

void writeFVecSpecialized(InstVector& ivector, const FVec& ret, GatherAddress *a, int isStreaming)
{
#if (SOALEN == VECLEN) && defined(USE_SHUFFLES)
    storeFVec(ivector, ret, a->getAddr(0), isStreaming);
#else
#ifndef USE_PKST
    scatterFVec(ivector, ret, a);
#else

    for(int i = 0; i < VECLEN; i += SOALEN) {
        storeSOAFVec(ivector, ret, a->getAddr(i), i/SOALEN, SOALEN);
    }

#endif
#endif
}

void readUFVec(InstVector& ivector, const FVec& ret, GatherAddress *a, int forward, const string &mask)
{
    ivector.push_back(new InitFVec(ret));

    for(int i = 0; i < VECLEN; i += SOALEN) {
        loadSplitSOAFVec(ivector, ret, a->getAddr(i), a->getAddr(i+(forward ? SOALEN-1 : 1)), i/SOALEN, SOALEN, forward);
    }
}

void writeUFVec(InstVector& ivector, const FVec& ret, GatherAddress *a, int forward, const string &mask)
{
    for(int i = 0; i < VECLEN; i += SOALEN) {
        storeSplitSOAFVec(ivector, ret, a->getAddr(i), a->getAddr(i+(forward ? SOALEN-1 : 1)), i/SOALEN, SOALEN, forward);
    }
}

void readU3FVec(InstVector& ivector, const FVec& ret, GatherAddress *a, int forward, const string &mask)
{
    ivector.push_back(new InitFVec(ret));
    for(int i = 0; i < VECLEN; i += SOALEN) {
        if(SOALEN > 2)
            loadSplit3SOAFVec(ivector, ret, a->getAddr(i), a->getAddr(i+(forward ? SOALEN-2 : 1)), a->getAddr(i+(forward ? SOALEN-1 : 2)), i/SOALEN, SOALEN, forward);
        else
            loadSplitSOAFVec(ivector, ret, a->getAddr(i), a->getAddr(i+1), i/SOALEN, SOALEN, forward);
    }
}

void readFVecSpinor(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int spin, int color, int reim, const string& mask)
{
    readFVecSpecialized(ivector, ret, new GatherAddress(new SpinorAddress(base,spin,color,reim,SpinorType), offset), mask);
}

void readFVecSpinorXB(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int spin, int color, int reim, const string& mask)
{
#if defined(USE_LDUNPK) || ((SOALEN == VECLEN) && defined(USE_SHUFFLES))
    readUFVec(ivector, ret, new GatherAddress(new SpinorAddress(base,spin,color,reim,SpinorType), offset ), 0, mask);
#else
    gatherFVec(ivector, ret, new GatherAddress(new SpinorAddress(base,spin,color,reim,SpinorType), offset), mask);
#endif
}

void readFVecSpinorXF(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int spin, int color, int reim, const string& mask)
{
#if defined(USE_LDUNPK) || ((SOALEN == VECLEN) && defined(USE_SHUFFLES))
    readUFVec(ivector, ret, new GatherAddress(new SpinorAddress(base,spin,color,reim,SpinorType), offset ), 1, mask);
#else
    gatherFVec(ivector, ret, new GatherAddress(new SpinorAddress(base,spin,color,reim,SpinorType), offset), mask);
#endif
}

void writeFVecSpinor(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int spin, int color, int reim, int isStreaming)
{
    writeFVecSpecialized(ivector, ret, new GatherAddress(new SpinorAddress(base,spin,color,reim,SpinorType), offset), isStreaming);
}

void readFVecKS(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int color, int reim, const string& mask)
{
    readFVecSpecialized(ivector, ret, new GatherAddress(new KSAddress(base,color,reim,SpinorType), offset), mask);
}

void readFVecKSXB(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int color, int reim, const string& mask)
{
#if defined(USE_LDUNPK) || ((SOALEN == VECLEN) && defined(USE_SHUFFLES))
    readUFVec(ivector, ret, new GatherAddress(new KSAddress(base,color,reim,SpinorType), offset ), 0, mask);
#else
    gatherFVec(ivector, ret, new GatherAddress(new KSAddress(base,color,reim,SpinorType), offset), mask);
#endif
}

void readFVecKSXF(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int color, int reim, const string& mask)
{
#if defined(USE_LDUNPK) || ((SOALEN == VECLEN) && defined(USE_SHUFFLES))
    readUFVec(ivector, ret, new GatherAddress(new KSAddress(base,color,reim,SpinorType), offset ), 1, mask);
#else
    gatherFVec(ivector, ret, new GatherAddress(new KSAddress(base,color,reim,SpinorType), offset), mask);
#endif
}

void readFVecKSXBTh(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int color, int reim, const string& mask)
{
#if defined(USE_LDUNPK) || ((SOALEN == VECLEN) && defined(USE_SHUFFLES))
    readU3FVec(ivector, ret, new GatherAddress(new KSAddress(base,color,reim,SpinorType), offset ), 0, mask);
#else
    gatherFVec(ivector, ret, new GatherAddress(new KSAddress(base,color,reim,SpinorType), offset), mask);
#endif
}

void readFVecKSXFTh(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int color, int reim, const string& mask)
{
#if defined(USE_LDUNPK) || ((SOALEN == VECLEN) && defined(USE_SHUFFLES))
    readU3FVec(ivector, ret, new GatherAddress(new KSAddress(base,color,reim,SpinorType), offset ), 1, mask);
#else
    gatherFVec(ivector, ret, new GatherAddress(new KSAddress(base,color,reim,SpinorType), offset), mask);
#endif
}

void writeFVecKS(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int color, int reim, int isStreaming)
{
    writeFVecSpecialized(ivector, ret, new GatherAddress(new KSAddress(base,color,reim,SpinorType), offset), isStreaming);
}

void readFVecGauge(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int dir, int c1, int c2, int reim)
{
#ifndef USE_PACKED_GAUGES
    readFVecSpecialized(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset), string(""));
#else
    loadFVec(ivector, ret, new GaugeAddress(base,dir,c1,c2,reim,GaugeType), string(""));
#endif
}

void readFVecGaugeXB(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int dir, int c1, int c2, int reim)
{
#ifndef USE_PACKED_GAUGES
#if defined(USE_LDUNPK) || ((SOALEN == VECLEN) && defined(USE_SHUFFLES))
    readUFVec(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset ), 0, string(""));
#else
    gatherFVec(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset), string(""));
#endif
#else
    // FIXME: we can potentially use load and shuffles but for time being keep it simple
#if defined(USE_LDUNPK) || ((SOALEN == VECLEN) && defined(USE_SHUFFLES))
    readUFVec(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset ), 0, string(""));
#else
    gatherFVec(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset), string(""));
#endif
#endif
}

void readFVecGaugeXF(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int dir, int c1, int c2, int reim)
{
#ifndef USE_PACKED_GAUGES
#if defined(USE_LDUNPK) || ((SOALEN == VECLEN) && defined(USE_SHUFFLES))
    readUFVec(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset ), 1, string(""));
#else
    gatherFVec(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset), string(""));
#endif
#else
    // FIXME: we can potentially use load and shuffles but for time being keep it simple
#if defined(USE_LDUNPK) || ((SOALEN == VECLEN) && defined(USE_SHUFFLES))
    readUFVec(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset ), 1, string(""));
#else
    gatherFVec(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset), string(""));
#endif
#endif
}

void readFVecGaugeY(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int dir, int c1, int c2, int reim)
{
#ifndef USE_PACKED_GAUGES
    readFVecSpecialized(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset), string(""));
#else
    if(VECLEN/SOALEN <= 2) {
        loadFVec(ivector, ret, new GaugeAddress(base,dir,c1,c2,reim,GaugeType), string(""));
    }
    else {
#ifndef USE_LDUNPK
        gatherFVec(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset), string(""));
#else
    initFVec(ivector, ret);
    GatherAddress *a = new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset);
    for(int i = 0; i < VECLEN; i += 2*SOALEN) {
        loadSOAFVec(ivector, ret, a->getAddr(i), i/(2*SOALEN), 2*SOALEN);
    }
#endif
    }

#endif
}


void unpackFVecGauge(InstVector& ivector, const FVec& ret, const string &inBuf, int unpackInd)
{
    loadFVec(ivector, ret, new AddressImm(new GenericAddress(inBuf, GaugeType), unpackInd), string(""));
}

void readOrUnpackFVecGaugeXB(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int dir, const string &inBuf, int unpackInd, int c1, int c2, int reim)
{
    GatherAddress *a = new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset);
#ifndef USE_PACKED_GAUGES
    ivector.push_back(new InitFVec(ret));
    for(int i = 0; i < VECLEN; i += SOALEN) {
        int unpackOffset = unpackInd + i/SOALEN;
#if SOALEN > 1
        loadSplitSOAFVec(ivector, ret, new AddressImm(new GenericAddress(inBuf, GaugeType), unpackOffset), a->getAddr(i+1), i/SOALEN, SOALEN, 0);
#else
        loadSOAFVec(ivector, ret, new AddressImm(new GenericAddress(inBuf, GaugeType), unpackOffset), i/SOALEN, SOALEN);
#endif
    }
#else
    // FIXME: we can potentially use load and shuffles but for time being keep it simple
    ivector.push_back(new InitFVec(ret));
    for(int i = 0; i < VECLEN; i += SOALEN) {
        int unpackOffset = unpackInd + i/SOALEN;
#if SOALEN > 1
        loadSplitSOAFVec(ivector, ret, new AddressImm(new GenericAddress(inBuf, GaugeType), unpackOffset), a->getAddr(i+1), i/SOALEN, SOALEN, 0);
#else
    loadSOAFVec(ivector, ret, new AddressImm(new GenericAddress(inBuf, GaugeType), unpackOffset), i/SOALEN, SOALEN);
#endif
    }
#endif
}

void readOrUnpackFVecGaugeXF(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int dir, const string &inBuf, int unpackInd, int c1, int c2, int reim)
{
    GatherAddress *a = new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset);
#ifndef USE_PACKED_GAUGES
    ivector.push_back(new InitFVec(ret));
    for(int i = 0; i < VECLEN; i += SOALEN) {
        int unpackOffset = unpackInd + i/SOALEN;
#if SOALEN > 1
        loadSplitSOAFVec(ivector, ret, a->getAddr(i), new AddressImm(new GenericAddress(inBuf, GaugeType), unpackOffset), i/SOALEN, SOALEN, 1);
#else
        loadSOAFVec(ivector, ret, new AddressImm(new GenericAddress(inBuf, GaugeType), unpackOffset), i/SOALEN, SOALEN);
#endif
    }
#else
    // FIXME: we can potentially use load and shuffles but for time being keep it simple
    ivector.push_back(new InitFVec(ret));
    for(int i = 0; i < VECLEN; i += SOALEN) {
        int unpackOffset = unpackInd + i/SOALEN;
#if SOALEN > 1
        loadSplitSOAFVec(ivector, ret, a->getAddr(i), new AddressImm(new GenericAddress(inBuf, GaugeType), unpackOffset), i/SOALEN, SOALEN, 1);
#else
    loadSOAFVec(ivector, ret, new AddressImm(new GenericAddress(inBuf, GaugeType), unpackOffset), i/SOALEN, SOALEN);
#endif
    }
#endif
}

void readOrUnpackFVecGaugeYB(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int dir, const string &inBuf, int unpackInd, int c1, int c2, int reim)
{
    if(VECLEN/SOALEN <= 2) {
        unpackFVecGauge(ivector, ret, inBuf, unpackInd);
    }
    else {
        initFVec(ivector, ret);
        GatherAddress *a = new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset);
        loadSOAFVec(ivector, ret, new AddressImm(new GenericAddress(inBuf, GaugeType), unpackInd), 0, 2*SOALEN);
        for(int i = 2*SOALEN; i < VECLEN; i += 2*SOALEN) {
            loadSOAFVec(ivector, ret, a->getAddr(i), i/(2*SOALEN), 2*SOALEN);
        }
    }
}

void readOrUnpackFVecGaugeYF(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int dir, const string &inBuf, int unpackInd, int c1, int c2, int reim)
{
    if(VECLEN/SOALEN <= 2) {
        unpackFVecGauge(ivector, ret, inBuf, unpackInd);
    }
    else {
        initFVec(ivector, ret);
        GatherAddress *a = new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset);
        for(int i = 0; i < VECLEN-2*SOALEN; i += 2*SOALEN) {
            loadSOAFVec(ivector, ret, a->getAddr(i), i/(2*SOALEN), 2*SOALEN);
        }
        loadSOAFVec(ivector, ret, new AddressImm(new GenericAddress(inBuf, GaugeType), unpackInd), VECLEN/SOALEN/2-1, 2*SOALEN);
    }
}

void writeFVecGauge(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int dir, int c1, int c2, int reim, int isStreaming)
{
#ifndef USE_PACKED_GAUGES
    writeFVecSpecialized(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset), isStreaming);
#else
    storeFVec(ivector, ret, new GaugeAddress(base,dir,c1,c2,reim,GaugeType), isStreaming);
#endif
}

void writeFVecGaugeXB(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int dir, int c1, int c2, int reim, int isStreaming)
{
    string mask;
#ifndef USE_PACKED_GAUGES
#if defined(USE_PKST) || ((SOALEN == VECLEN) && defined(USE_SHUFFLES))
    writeUFVec(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset ), 0, mask);
#else
    scatterFVec(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset));
#endif
#else
    // FIXME: we can potentially use load and shuffles but for time being keep it simple
#if defined(USE_PKST) || ((SOALEN == VECLEN) && defined(USE_SHUFFLES))
    writeUFVec(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset ), 0, mask);
#else
    scatterFVec(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset));
#endif
#endif
}

void writeFVecGaugeXF(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int dir, int c1, int c2, int reim, int isStreaming)
{
    string mask;
#ifndef USE_PACKED_GAUGES
#if defined(USE_PKST) || ((SOALEN == VECLEN) && defined(USE_SHUFFLES))
    writeUFVec(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset ), 1, mask);
#else
    scatterFVec(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset));
#endif
#else
    // FIXME: we can potentially use load and shuffles but for time being keep it simple
#if defined(USE_PKST) || ((SOALEN == VECLEN) && defined(USE_SHUFFLES))
    writeUFVec(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset ), 1, mask);
#else
    scatterFVec(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset));
#endif
#endif
}

void writeFVecGaugeY(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int dir, int c1, int c2, int reim, int isStreaming)
{
#ifndef USE_PACKED_GAUGES
    writeFVecSpecialized(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset), isStreaming);
#else
    if(VECLEN/SOALEN <= 2) {
        storeFVec(ivector, ret, new GaugeAddress(base,dir,c1,c2,reim,GaugeType), isStreaming);
    }
    else {
#ifndef USE_PKST
        scatterFVec(ivector, ret, new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset));
#else
    GatherAddress *a = new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset);
    for(int i = 0; i < VECLEN; i += 2*SOALEN) {
        storeSOAFVec(ivector, ret, a->getAddr(i), i/(2*SOALEN), 2*SOALEN);
    }
#endif
    }

#endif
}

void packFVecGauge(InstVector& ivector, const FVec& ret, const string &outBuf, int packInd)
{
    storeFVec(ivector, ret, new AddressImm(new GenericAddress(outBuf, GaugeType), packInd), 1);
}

void writeOrPackFVecGaugeXB(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int dir, const string &outBuf, int packInd, int c1, int c2, int reim)
{
    GatherAddress *a = new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset);
#ifndef USE_PACKED_GAUGES
    for(int i = 0; i < VECLEN; i += SOALEN) {
        int packOffset = packInd + i/SOALEN;
        storeSplitSOAFVec(ivector, ret, new AddressImm(new GenericAddress(outBuf, GaugeType), packOffset), a->getAddr(i+1), i/SOALEN, SOALEN, 0);
    }
#else
    // FIXME: we can potentially use load and shuffles but for time being keep it simple
    for(int i = 0; i < VECLEN; i += SOALEN) {
        int packOffset = packInd + i/SOALEN;
        storeSplitSOAFVec(ivector, ret, new AddressImm(new GenericAddress(outBuf, GaugeType), packOffset), a->getAddr(i+1), i/SOALEN, SOALEN, 0);
    }
#endif
}

void writeOrPackFVecGaugeXF(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int dir, const string &outBuf, int packInd, int c1, int c2, int reim)
{
    GatherAddress *a = new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset);
#ifndef USE_PACKED_GAUGES
    for(int i = 0; i < VECLEN; i += SOALEN) {
        int packOffset = packInd + i/SOALEN;
        storeSplitSOAFVec(ivector, ret, a->getAddr(i), new AddressImm(new GenericAddress(outBuf, GaugeType), packOffset), i/SOALEN, SOALEN, 1);
    }
#else
    // FIXME: we can potentially use load and shuffles but for time being keep it simple
    for(int i = 0; i < VECLEN; i += SOALEN) {
        int packOffset = packInd + i/SOALEN;
        storeSplitSOAFVec(ivector, ret, a->getAddr(i), new AddressImm(new GenericAddress(outBuf, GaugeType), packOffset), i/SOALEN, SOALEN, 1);
    }
#endif
}

void writeOrPackFVecGaugeYB(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int dir, const string &outBuf, int packInd, int c1, int c2, int reim)
{
    if(VECLEN/SOALEN <= 2) {
        packFVecGauge(ivector, ret, outBuf, packInd);
    }
    else {
        GatherAddress *a = new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset);
        storeSOAFVec(ivector, ret, new AddressImm(new GenericAddress(outBuf, GaugeType), packInd), 0, 2*SOALEN);
        for(int i = 2*SOALEN; i < VECLEN; i += 2*SOALEN) {
            storeSOAFVec(ivector, ret, a->getAddr(i), i/(2*SOALEN), 2*SOALEN);
        }
    }
}

void writeOrPackFVecGaugeYF(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int dir, const string &outBuf, int packInd, int c1, int c2, int reim)
{
    if(VECLEN/SOALEN <= 2) {
        packFVecGauge(ivector, ret, outBuf, packInd);
    }
    else {
        GatherAddress *a = new GatherAddress(new GaugeAddress(base,dir,c1,c2,reim,GaugeType), offset);
        for(int i = 0; i < VECLEN-2*SOALEN; i += 2*SOALEN) {
            storeSOAFVec(ivector, ret, a->getAddr(i), i/(2*SOALEN), 2*SOALEN);
        }
        storeSOAFVec(ivector, ret, new AddressImm(new GenericAddress(outBuf, GaugeType), packInd), VECLEN/SOALEN/2-1, 2*SOALEN);
    }
}

void readFVecClovDiag(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int block, int c)
{
#ifndef USE_PACKED_CLOVER
    readFVecSpecialized(ivector, ret, new GatherAddress(new ClovDiagAddress(base,block,c,CloverType), offset), string(""));
#else
    loadFVec(ivector, ret, new ClovDiagAddress(base,block,c,CloverType), string(""));
#endif
}

void readFVecClovOffDiag(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int block, int c, int reim)
{
#ifndef USE_PACKED_CLOVER
    readFVecSpecialized(ivector, ret, new GatherAddress(new ClovOffDiagAddress(base,block,c,reim,CloverType), offset), string(""));
#else
    loadFVec(ivector, ret, new ClovOffDiagAddress(base,block,c,reim,CloverType), string(""));
#endif
}


void LoadSpinorElement(InstVector& ivector, const FVec& ret, const string& base, const string& offsets, int spin, int col, int reim, bool isFace, string mask, int dir)
{
    void (*readFunc)(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int spin, int color, int reim, const string& mask);

    readFunc = readFVecSpinor;

    if(isFace == false) {
        if(dir == 0) readFunc = readFVecSpinorXB;
        if(dir == 1) readFunc = readFVecSpinorXF;
    }

    readFunc(ivector, ret, base, offsets, spin, col, reim, mask);
}

void LoadFullSpinor(InstVector& ivector, const FVec ret[4][3][2], const string& base, const string& offsets, string mask)
{
    for(int col=0; col < 3; col++) {
        for(int spin=0; spin < 4; spin++) {
            LoadSpinorElement(ivector, ret[spin][col][RE], base, offsets, spin, col, RE, false, mask, -1);
            LoadSpinorElement(ivector, ret[spin][col][IM], base, offsets, spin, col, IM, false, mask, -1);
        }
    }
}

void StoreFullSpinor(InstVector& ivector, const FVec ret[4][3][2], const string& base, const string& offsets, int isStreaming)
{
    int useShuffles = 0;

#ifndef ENABLE_STREAMING_STORES
    isStreaming = 0;
#endif

#if defined(USE_SHUFFLES)
    useShuffles = 1;
#endif

    if(isStreaming) {
        useShuffles = 1;
    }

#if SOALEN == VECLEN
    useShuffles = 0;
#endif

    extern FVec tmp[];
    FVec in[24] = {
        ret[0][0][0], ret[0][0][1], ret[1][0][0], ret[1][0][1], ret[2][0][0], ret[2][0][1], ret[3][0][0], ret[3][0][1],
        ret[0][1][0], ret[0][1][1], ret[1][1][0], ret[1][1][1], ret[2][1][0], ret[2][1][1], ret[3][1][0], ret[3][1][1],
        ret[0][2][0], ret[0][2][1], ret[1][2][0], ret[1][2][1], ret[2][2][0], ret[2][2][1], ret[3][2][0], ret[3][2][1]
    };

    if(useShuffles) {
        GatherAddress *ga = new GatherAddress(new SpinorAddress(base,0,0,0,SpinorType), offsets );

        for(int i = 0; i < ((24*SOALEN)/VECLEN); i++) {
            transpose(ivector, &tmp[0], &in[i*(VECLEN/SOALEN)], SOALEN);

            for(int j = 0; j < (VECLEN/SOALEN); j++) {
                storeFVec(ivector, tmp[j], new AddressImm(ga->getAddr(j*SOALEN), i*VECLEN), isStreaming);
            }
        }
    }
    else {
        for(int spin=0; spin < 4; spin++) {
            for(int col=0; col < 3; col++) {
                writeFVecSpinor(ivector, ret[spin][col][RE], base, offsets, spin, col, RE, isStreaming);
                writeFVecSpinor(ivector, ret[spin][col][IM], base, offsets, spin, col, IM, isStreaming);
            }
        }
    }
}

void StreamFullSpinor(InstVector& ivector, const FVec ret[4][3][2], const string& base, const string& offsets)
{
    StoreFullSpinor(ivector, ret, base, offsets, 1);
}

void PackHalfSpinorElement(InstVector& ivector, const FVec& ret, const string& base, int packoffset, int dir, string mask)
{
    if(dir >= 4 || (dir >= 2 && SOALEN == VECLEN)) {
        storeFVec(ivector, ret, new AddressImm(new GenericAddress(base, SpinorType), packoffset), 1);
    }
    else if(dir == 2) {
        storeSOAFVec(ivector, ret, new AddressImm(new GenericAddress(base, SpinorType), packoffset), 0, SOALEN);
    }
    else if(dir == 3) {
        storeSOAFVec(ivector, ret, new AddressImm(new GenericAddress(base, SpinorType), packoffset), (VECLEN/SOALEN)-1, SOALEN);
    }
    else {
        int possibleMsk = 0;
        int soaMsk = (dir == 0 ? 1 : (1 << (SOALEN-1)));

        for(int i = 0; i < VECLEN; i += SOALEN) {
            possibleMsk |= (soaMsk << i);
        }

        packFVec(ivector, ret, new AddressImm(new GenericAddress(base, SpinorType), packoffset), mask, possibleMsk);
    }
}

void PackHalfSpinor(InstVector& ivector, const FVec ret[2][3][2], const string& base, int dir, string mask)
{
    int nActiveLanes = VECLEN;
    int ind = 0;

    if(dir == 0 || dir == 1) {
        nActiveLanes = (VECLEN/SOALEN+1)/2;    // +1 to avoid making it 0 when SOALEN=VECLEN
    }

    if(dir == 2 || dir == 3) {
        nActiveLanes = SOALEN;
    }

    for(int spin=0; spin < 2; spin++) {
        for(int col=0; col < 3; col++) {
            PackHalfSpinorElement(ivector, ret[spin][col][RE], base, (ind+0)*nActiveLanes, dir, mask);
            PackHalfSpinorElement(ivector, ret[spin][col][IM], base, (ind+1)*nActiveLanes, dir, mask);
            ind += 2;
        }
    }
}

void UnpackHalfSpinorElement(InstVector& ivector, const FVec& ret, const string& base, int unpackoffset, int dir, string mask)
{
    if(dir >= 4 || (dir >= 2 && SOALEN == VECLEN)) {
        loadFVec(ivector, ret, new AddressImm(new GenericAddress(base, SpinorType), unpackoffset), string(""));
    }
    else if(dir == 2) {
        loadSOAFVec(ivector, ret, new AddressImm(new GenericAddress(base, SpinorType), unpackoffset), 0, SOALEN);
    }
    else if(dir == 3) {
        loadSOAFVec(ivector, ret, new AddressImm(new GenericAddress(base, SpinorType), unpackoffset), (VECLEN/SOALEN)-1, SOALEN);
    }
    else {
        int possibleMsk = 0;
        int soaMsk = (dir == 0 ? 1 : (1 << (SOALEN-1)));

        for(int i = 0; i < VECLEN; i += SOALEN) {
            possibleMsk |= (soaMsk << i);
        }

        unpackFVec(ivector, ret, new AddressImm(new GenericAddress(base, SpinorType), unpackoffset), mask, possibleMsk);
    }
}

void UnpackHalfSpinor(InstVector& ivector, const FVec ret[2][3][2], const string& base, int dir, string mask)
{
    int nActiveLanes = VECLEN;
    int ind = 0;

    if(dir == 0 || dir == 1) {
        nActiveLanes = (VECLEN/SOALEN+1)/2;    // +1 to avoid making it 0 when SOALEN=VECLEN
    }

    if(dir == 2 || dir == 3) {
        nActiveLanes = SOALEN;
    }

    for(int spin=0; spin < 2; spin++) {
        for(int col=0; col < 3; col++) {
            UnpackHalfSpinorElement(ivector, ret[spin][col][RE], base, (ind+0)*nActiveLanes, dir, mask);
            UnpackHalfSpinorElement(ivector, ret[spin][col][IM], base, (ind+1)*nActiveLanes, dir, mask);
            ind += 2;
        }
    }
}

void LoadFullGaugeDir(InstVector& ivector, const FVec ret[3][3][2], const string& base, const string& offsets, int dir, bool compress12)
{
    int nrows = 3;

    if(compress12 == true) {
        nrows = 2;
    }

    for(int c1=0; c1 < nrows; c1++) {
        for(int c2=0; c2 < 3; c2++) {
            readFVecGauge(ivector, ret[c1][c2][RE], base, offsets, dir, c1, c2, RE);
            readFVecGauge(ivector, ret[c1][c2][IM], base, offsets, dir, c1, c2, IM);
        }
    }
}

void LoadFullNeighborGaugeDirLink(InstVector& ivector, const FVec ret[3][3][2], const string& base, const string& offsets, int dir, int link_dir, bool compress12)
{
    void (*readFunc)(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int dir, int c1, int c2, int reim);

    readFunc = readFVecGauge;
    if(dir == 0) readFunc = readFVecGaugeXB;
    if(dir == 1) readFunc = readFVecGaugeXF;
    if(dir == 2 || dir == 3) readFunc = readFVecGaugeY;

    int nrows = 3;
    if(compress12 == true) nrows = 2;

    for(int c1=0; c1 < nrows; c1++) {
        for(int c2=0; c2 < 3; c2++) {
            readFunc(ivector, ret[c1][c2][RE], base, offsets, link_dir, c1, c2, RE);
            readFunc(ivector, ret[c1][c2][IM], base, offsets, link_dir, c1, c2, IM);
        }
    }
}

void LoadOrUnpackFullNeighborGaugeDirLinkBuffer(InstVector& ivector, const FVec ret[3][3][2], const string& base, const string& offsets, int dir, int link_dir, const string &inBuf, int bufLinkInd, bool compress12)
{
    int packedSites = VECLEN;
    if(dir == 0 || dir == 1) packedSites = VECLEN/SOALEN;
    if(dir == 2 || dir == 3) packedSites = (VECLEN <= SOALEN*2 ? VECLEN : SOALEN*2);

    int elemsPerSite = (compress12 ? 12 : 18);

    void (*readFunc)(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int link_dir, const string &inBuf, int unpackInd, int c1, int c2, int reim);

    if(dir == 0) readFunc = readOrUnpackFVecGaugeXB;
    if(dir == 1) readFunc = readOrUnpackFVecGaugeXF;
    if(dir == 2) readFunc = readOrUnpackFVecGaugeYB;
    if(dir == 3) readFunc = readOrUnpackFVecGaugeYF;

    int nrows = 3;
    if(compress12 == true) nrows = 2;

    for(int c1=0; c1 < nrows; c1++) {
        for(int c2=0; c2 < 3; c2++) {
            for(int reim = 0; reim < 2; reim++) {
                int elemInd = 6*c1+2*c2+reim;
                int unpackInd = bufLinkInd*(packedSites*elemsPerSite) + packedSites*elemInd;
                if(dir < 4)
                    readFunc(ivector, ret[c1][c2][reim], base, offsets, link_dir, inBuf, unpackInd, c1, c2, reim);
                else
                    unpackFVecGauge(ivector, ret[c1][c2][reim], inBuf, unpackInd);
            }
        }
    }
}

void PackGaugeDir(InstVector &ivector, const FVec ret[3][3][2], int dir, const string& outBuf, int bufLinkInd, bool compress12)
{
    int packedSites = VECLEN;
    if(dir == 0 || dir == 1) packedSites = VECLEN/SOALEN;
    if(dir == 2 || dir == 3) packedSites = (VECLEN <= SOALEN*2 ? VECLEN : SOALEN*2);

    int elemsPerSite = (compress12 ? 12 : 18);

    int nrows = 3;
    if(compress12 == true) nrows = 2;

    for(int c1=0; c1 < nrows; c1++) {
        for(int c2=0; c2 < 3; c2++) {
            for(int reim = 0; reim < 2; reim++) {
                int elemInd = 6*c1+2*c2+reim;
                int packInd = bufLinkInd*(packedSites*elemsPerSite) + packedSites*elemInd;
                if(dir >= 4 || (dir >= 2 && VECLEN <= 2*SOALEN)) {
                    packFVecGauge(ivector, ret[c1][c2][reim], outBuf, packInd);
                }
                else if(dir == 2) {
                    storeSOAFVec(ivector, ret[c1][c2][reim], new AddressImm(new GenericAddress(outBuf, GaugeType), packInd), 0, 2*SOALEN);
                }
                else if(dir == 3) {
                    storeSOAFVec(ivector, ret[c1][c2][reim], new AddressImm(new GenericAddress(outBuf, GaugeType), packInd), (VECLEN/SOALEN/2)-1, 2*SOALEN);
                }
                else {
                    int possibleMsk = 0;
                    int soaMsk = (dir == 0 ? 1 : (1 << (SOALEN-1)));

                    for(int i = 0; i < VECLEN; i += SOALEN)
                        possibleMsk |= (soaMsk << i);

                    packFVec(ivector, ret[c1][c2][reim], new AddressImm(new GenericAddress(outBuf, SpinorType), packInd), possibleMsk, possibleMsk);
                }
            }
        }
    }
}

void StoreFullGaugeDir(InstVector& ivector, const FVec ret[3][3][2], const string& base, const string& offsets, int dir, bool compress12, int isStreaming)
{
    int nrows = 3;
    if(compress12 == true) nrows = 2;

#ifndef USE_PACKED_GAUGES
    int useShuffles = 0;

#ifndef ENABLE_STREAMING_STORES
    isStreaming = 0;
#endif

#if defined(USE_SHUFFLES)
    useShuffles = 1;
#endif

    if(isStreaming) useShuffles = 1;

#if SOALEN == VECLEN
    useShuffles = 0;
#endif

    if(compress12 == false) {
        isStreaming = 0;
        useShuffles = 0;
    }

    extern FVec tmp[];
    FVec in[18] = {
        ret[0][0][0], ret[0][0][1], ret[0][1][0], ret[0][1][1], ret[0][2][0], ret[0][2][1],
        ret[1][0][0], ret[1][0][1], ret[1][1][0], ret[1][1][1], ret[1][2][0], ret[1][2][1],
        ret[2][0][0], ret[2][0][1], ret[2][1][0], ret[2][1][1], ret[2][2][0], ret[2][2][1]
    };
    if(useShuffles) {
        GatherAddress *ga = new GatherAddress(new GaugeAddress(base,dir,0,0,0,GaugeType), offsets );
        for(int i = 0; i < ((12*SOALEN)/VECLEN); i++) {
            transpose(ivector, &tmp[0], &in[i*(VECLEN/SOALEN)], SOALEN);
            for(int j = 0; j < (VECLEN/SOALEN); j++) {
                storeFVec(ivector, tmp[j], new AddressImm(ga->getAddr(j*SOALEN), i*VECLEN), isStreaming);
            }
        }
    }
    else {
        for(int c1=0; c1 < nrows; c1++) {
            for(int c2=0; c2 < 3; c2++) {
                writeFVecSpecialized(ivector, ret[c1][c2][RE], new GatherAddress(new GaugeAddress(base,dir,c1,c2,RE,GaugeType), offset));
                writeFVecSpecialized(ivector, ret[c1][c2][IM], new GatherAddress(new GaugeAddress(base,dir,c1,c2,IM,GaugeType), offset));
            }
        }
    }
#else
    for(int c1=0; c1 < nrows; c1++) {
        for(int c2=0; c2 < 3; c2++) {
            storeFVec(ivector, ret[c1][c2][RE], new GaugeAddress(base,dir,c1,c2,RE,GaugeType), isStreaming);
            storeFVec(ivector, ret[c1][c2][IM], new GaugeAddress(base,dir,c1,c2,IM,GaugeType), isStreaming);
        }
    }
#endif
}

void StreamFullGaugeDir(InstVector& ivector, const FVec ret[3][3][2], const string& base, const string& offsets, int dir, bool compress12)
{
    StoreFullGaugeDir(ivector, ret, base, offsets, dir, compress12, 1);
}

void StoreFullNeighborGaugeDirLink(InstVector& ivector, const FVec ret[3][3][2], const string& base, const string& offsets, int dir, int link_dir, bool compress12)
{
    void (*writeFunc)(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int dir, int c1, int c2, int reim, int isStreaming);

    writeFunc = writeFVecGauge;
    if(dir == 0) writeFunc = writeFVecGaugeXB;
    if(dir == 1) writeFunc = writeFVecGaugeXF;
    if(dir == 2 || dir == 3) writeFunc = writeFVecGaugeY;

    int nrows = 3;
    if(compress12 == true) nrows = 2;

    for(int c1=0; c1 < nrows; c1++) {
        for(int c2=0; c2 < 3; c2++) {
            writeFunc(ivector, ret[c1][c2][RE], base, offsets, link_dir, c1, c2, RE, 1);
            writeFunc(ivector, ret[c1][c2][IM], base, offsets, link_dir, c1, c2, IM, 1);
        }
    }
}

void StoreOrPackFullNeighborGaugeDirLinkBuffer(InstVector& ivector, const FVec ret[3][3][2], const string& base, const string& offsets, int dir, int link_dir, const string &outBuf, int bufLinkInd, bool compress12)
{
    int packedSites = VECLEN;
    if(dir == 0 || dir == 1) packedSites = VECLEN/SOALEN;
    if(dir == 2 || dir == 3) packedSites = (VECLEN <= SOALEN*2 ? VECLEN : SOALEN*2);

    int elemsPerSite = (compress12 ? 12 : 18);

    void (*writeFunc)(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int link_dir, const string &outBuf, int packInd, int c1, int c2, int reim);

    if(dir == 0) writeFunc = writeOrPackFVecGaugeXB;
    if(dir == 1) writeFunc = writeOrPackFVecGaugeXF;
    if(dir == 2) writeFunc = writeOrPackFVecGaugeYB;
    if(dir == 3) writeFunc = writeOrPackFVecGaugeYF;

    int nrows = 3;
    if(compress12 == true) nrows = 2;

    for(int c1=0; c1 < nrows; c1++) {
        for(int c2=0; c2 < 3; c2++) {
            for(int reim = 0; reim < 2; reim++) {
                int elemInd = 6*c1+2*c2+reim;
                int packInd = bufLinkInd*(packedSites*elemsPerSite) + packedSites*elemInd;
                if(dir < 4)
                    writeFunc(ivector, ret[c1][c2][reim], base, offsets, link_dir, outBuf, packInd, c1, c2, reim);
                else
                    packFVecGauge(ivector, ret[c1][c2][reim], outBuf, packInd);
            }
        }
    }
}

void LoadFullCloverBlock(InstVector& ivector, const FVec diag[6], const FVec off_diag[15][2], const string& base, const string& offsets, int block)
{
    for(int c=0; c < 6; c++) {
        readFVecClovDiag(ivector, diag[c], base, offsets, block, c);
    }

    for(int c=0; c < 15; c++) {
        readFVecClovOffDiag(ivector, off_diag[c][RE], base, offsets, block, c, RE);
        readFVecClovOffDiag(ivector, off_diag[c][IM], base, offsets, block, c, IM);
    }
}


void LoadKSElement(InstVector& ivector, const FVec& ret, const string& base, const string& offsets, int col, int reim, bool isFace, string mask, int dir, bool isThird)
{
    void (*readFunc)(InstVector& ivector, const FVec& ret, const string& base, const string& offset, int color, int reim, const string& mask);

    readFunc = readFVecKS;
    if(isFace == false) {
        if(dir == 0) readFunc = (isThird ? readFVecKSXBTh : readFVecKSXB);
        if(dir == 1) readFunc = (isThird ? readFVecKSXFTh : readFVecKSXF);
    }
    readFunc(ivector, ret, base, offsets, col, reim, mask);
}
void LoadFullKS(InstVector& ivector, const FVec ret[3][2], const string& base, const string& offsets, string mask)
{
    for(int col=0; col < 3; col++) {
        LoadKSElement(ivector, ret[col][RE], base, offsets, col, RE, false, mask, -1, false);
        LoadKSElement(ivector, ret[col][IM], base, offsets, col, IM, false, mask, -1, false);
    }
}


void StoreFullKS(InstVector& ivector, const FVec ret[3][2], const string& base, const string& offsets, int isStreaming)
{
    int useShuffles = 0;

#ifndef ENABLE_STREAMING_STORES
    isStreaming = 0;
#endif

#if defined(USE_SHUFFLES)
    useShuffles = 1;
#endif

    if(VECLEN / SOALEN > 2) {
        isStreaming = 0;
        useShuffles = 0;
    }

    if(isStreaming) useShuffles = 1;

#if SOALEN == VECLEN
    useShuffles = 0;
#endif

    extern FVec tmp[];
    FVec in[6] = { ret[0][0], ret[0][1], ret[1][0], ret[1][1], ret[2][0], ret[2][1]	};
    if(useShuffles) {
        GatherAddress *ga = new GatherAddress(new KSAddress(base,0,0,SpinorType), offsets );
        for(int i = 0; i < ((6*SOALEN)/VECLEN); i++) {
            transpose(ivector, &tmp[0], &in[i*(VECLEN/SOALEN)], SOALEN);
            for(int j = 0; j < (VECLEN/SOALEN); j++) {
                storeFVec(ivector, tmp[j], new AddressImm(ga->getAddr(j*SOALEN), i*VECLEN), isStreaming);
            }
        }
    }
    else {
        for(int col=0; col < 3; col++) {
            writeFVecKS(ivector, ret[col][RE], base, offsets, col, RE, isStreaming);
            writeFVecKS(ivector, ret[col][IM], base, offsets, col, IM, isStreaming);
        }
    }
}

void StreamFullKS(InstVector& ivector, const FVec ret[3][2], const string& base, const string& offsets)
{
    StoreFullKS(ivector, ret, base, offsets, 1);
}

void PackKSSpinor(InstVector& ivector, const FVec ret[3][2], const string& out, int dir, const string &intMask, const string &width)
{
    for(int c = 0; c < 3; c++) {
        for(int reim = 0; reim < 2; reim++) {
            if(dir >=4 || (dir >=2 && VECLEN == SOALEN)) {
                int packoffset = (2*c+reim)*VECLEN;
                storeFVec(ivector, ret[c][reim], new AddressImm(new GenericAddress(out, SpinorType), packoffset), 1);
            }
            else {
                int elemId = (2*c+reim);
                packFVec(ivector, ret[c][reim], new AddressScaledOffset(new GenericAddress(out, SpinorType), width, elemId), intMask);
            }
        }
    }
}

void UnpackKSSpinor(InstVector& ivector, const FVec ret[3][2], const string& in, int dir, const string &intMask, const string &width)
{
    for(int c = 0; c < 3; c++) {
        for(int reim = 0; reim < 2; reim++) {
            if(dir >=4 || (dir >=2 && VECLEN == SOALEN)) {
                int unpackoffset = (2*c+reim)*VECLEN;
                loadFVec(ivector, ret[c][reim], new AddressImm(new GenericAddress(in, SpinorType), unpackoffset), string(""));
            }
            else {
                int elemId = (2*c+reim);
                unpackFVec(ivector, ret[c][reim], new AddressScaledOffset(new GenericAddress(in, SpinorType), width, elemId), intMask);
            }
        }
    }
}

// Prefetches

void PrefetchL1Specialized(InstVector& ivector, GatherAddress *a, int type = 0, int dir = -1)
//void PrefetchL1Specialized(InstVector& ivector, GatherAddress *a, int type, int dir)
{
#ifndef NO_GPREF_L1
    gatherPrefetchL1(ivector, a, type);
#else

    for(int i = 0; i < VECLEN; i += SOALEN) {
        prefetchL1(ivector, a->getAddr(i), type);
    }

#endif
}

void PrefetchL2Specialized(InstVector& ivector, GatherAddress *a, int type = 0, int dir = -1)
//void PrefetchL2Specialized(InstVector& ivector, GatherAddress *a, int type, int dir)
{
#ifndef NO_GPREF_L2
    gatherPrefetchL2(ivector, a, type);
#else

    for(int i = 0; i < VECLEN; i += SOALEN) {
        prefetchL2(ivector, a->getAddr(i), type);
    }

#endif
}

void prefetchL1SpinorIn(InstVector& ivector, string base, string off, int imm, int dir, int type)
{
#ifdef PREF_L1_SPINOR_IN
    PrefetchL1Specialized(ivector, new GatherAddress(new AddressImm(new SpinorAddress(base,0,0,0,SpinorType), imm), off ), type, dir);
#endif
}

void prefetchL2SpinorIn(InstVector& ivector, string base, string off, const string& pref_dist, int imm, int dir)
{
#ifdef PREF_L2_SPINOR_IN
    PrefetchL2Specialized(ivector, new GatherAddress(new AddressImm(new AddressOffset(new SpinorAddress(base,0,0,0,SpinorType), pref_dist), imm), off ));
#endif
}

void prefetchL1SpinorOut(InstVector& ivector, string base, string off, int imm)
{
#ifdef PREF_L1_SPINOR_OUT
    PrefetchL1Specialized(ivector, new GatherAddress(new AddressImm(new SpinorAddress(base,0,0,0,SpinorType), imm), off ), 3 /*NT & EX */);
#endif
}

void prefetchL2SpinorOut(InstVector& ivector, string base, string off, const string& pref_dist, int imm)
{
#ifdef PREF_L2_SPINOR_OUT
    PrefetchL2Specialized(ivector, new GatherAddress(new AddressImm(new AddressOffset(new SpinorAddress(base,0,0,0,SpinorType), pref_dist), imm), off ), 3 /*NT & EX */);
#endif
}

void prefetchL1KSIn(InstVector& ivector, string base, string off, int imm, int dir, int type)
{
#ifdef PREF_L1_SPINOR_IN
    PrefetchL1Specialized(ivector, new GatherAddress(new AddressImm(new KSAddress(base,0,0,SpinorType), imm), off ), type, dir);
#endif
}

void prefetchL2KSIn(InstVector& ivector, string base, string off, const string& pref_dist, int imm, int dir)
{
#ifdef PREF_L2_SPINOR_IN
    PrefetchL2Specialized(ivector, new GatherAddress(new AddressImm(new AddressOffset(new KSAddress(base,0,0,SpinorType), pref_dist), imm), off ));
#endif
}

void prefetchL1KSOut(InstVector& ivector, string base, string off, int imm)
{
#ifdef PREF_L1_SPINOR_OUT
    PrefetchL1Specialized(ivector, new GatherAddress(new AddressImm(new KSAddress(base,0,0,SpinorType), imm), off ), 3 /*NT & EX */);
#endif
}

void prefetchL2KSOut(InstVector& ivector, string base, string off, const string& pref_dist, int imm)
{
#ifdef PREF_L2_SPINOR_OUT
    PrefetchL2Specialized(ivector, new GatherAddress(new AddressImm(new AddressOffset(new KSAddress(base,0,0,SpinorType), pref_dist), imm), off ), 3 /*NT & EX */);
#endif
}

void prefetchL1GuageDirIn(InstVector& ivector, string base, string off, int dir, int imm, int type)
{
#ifdef PREF_L1_GAUGE
#ifndef USE_PACKED_GAUGES
    PrefetchL1Specialized(ivector, new GatherAddress(new AddressImm(new GaugeAddress(base,dir,0,0,0,GaugeType), imm), off ), type);
#else
    prefetchL1(ivector, new AddressImm(new GaugeAddress(base,dir,0,0,0,GaugeType), imm), type);
#endif
#endif
}

void prefetchL2GuageDirIn(InstVector& ivector, string base, string off, int dir, const string& pref_dist, int imm, int type)
{
#ifdef PREF_L2_GAUGE
#ifndef USE_PACKED_GAUGES
    PrefetchL2Specialized(ivector, new GatherAddress(new AddressImm(new AddressOffset(new GaugeAddress(base,dir,0,0,0,GaugeType), pref_dist), imm), off ), type);
#else
    prefetchL2(ivector, new AddressImm(new AddressOffset(new GaugeAddress(base,dir,0,0,0,GaugeType), pref_dist), imm), type);
#endif
#endif
}

void prefetchL1CloverBlockIn(InstVector& ivector, string base, string off, int block, int imm)
{
#ifdef PREF_L1_CLOVER
#ifndef USE_PACKED_CLOVER
    PrefetchL1Specialized(ivector, new GatherAddress(new AddressImm(new ClovDiagAddress(base,block,0,CloverType), imm), off ), 0);
#else
    prefetchL1(ivector, new AddressImm(new ClovDiagAddress(base,block,0,CloverType), imm), 0);
#endif
#endif
}

void prefetchL2CloverBlockIn(InstVector& ivector, string base, string off, int block, const string& pref_dist, int imm)
{
#ifdef PREF_L2_CLOVER
#ifndef USE_PACKED_CLOVER
    PrefetchL2Specialized(ivector, new GatherAddress(new AddressImm(new AddressOffset(new ClovDiagAddress(base,block,0,CloverType), pref_dist), imm), off ), 0);
#else
    prefetchL2(ivector, new AddressImm(new AddressOffset(new ClovDiagAddress(base,block,0,CloverType), pref_dist), imm), 0);
#endif
#endif
}

void PrefetchL1FullSpinorDirIn(InstVector& ivector, const string& base, const string& off, int dir, int type)
{
    // for now we ignore direction but it can be used for specialization
    for(int i = 0; i < (24*SOALEN*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL1SpinorIn(ivector, base, off, i*(64/sizeof(SpinorBaseType)), dir, type);
    }
}

void PrefetchL1FullSpinorOut(InstVector& ivector, const string& base, const string& off)
{
    for(int i = 0; i < (24*SOALEN*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL1SpinorOut(ivector, base, off, i*(64/sizeof(SpinorBaseType)));
    }
}

void PrefetchL2FullSpinorDirIn(InstVector& ivector, const string& base, const string& off, const string& pref_dist, int dir)
{
    // for now we ignore direction but itcan be used for specialization
    for(int i = 0; i < (24*SOALEN*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL2SpinorIn(ivector, base, off, pref_dist, i*(64/sizeof(SpinorBaseType)), dir);
    }
}

void PrefetchL2FullSpinorOut(InstVector& ivector, const string& base, const string& off, const string& pref_dist)
{
    for(int i = 0; i < (24*SOALEN*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL2SpinorOut(ivector, base, off, pref_dist, i*(64/sizeof(SpinorBaseType)));
    }
}

void PrefetchL1FullKSDirIn(InstVector& ivector, const string& base, const string& off, int dir, int type)
{
    // for now we ignore direction but it can be used for specialization
    for(int i = 0; i < (6*SOALEN*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL1KSIn(ivector, base, off, i*(64/sizeof(SpinorBaseType)), dir, type);
    }
}

void PrefetchL1FullKSOut(InstVector& ivector, const string& base, const string& off)
{
    for(int i = 0; i < (6*SOALEN*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL1KSOut(ivector, base, off, i*(64/sizeof(SpinorBaseType)));
    }
}

void PrefetchL2FullKSDirIn(InstVector& ivector, const string& base, const string& off, const string& pref_dist, int dir)
{
    // for now we ignore direction but itcan be used for specialization
    for(int i = 0; i < (6*SOALEN*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL2KSIn(ivector, base, off, pref_dist, i*(64/sizeof(SpinorBaseType)), dir);
    }
}

void PrefetchL2FullKSOut(InstVector& ivector, const string& base, const string& off, const string& pref_dist)
{
    for(int i = 0; i < (6*SOALEN*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL2KSOut(ivector, base, off, pref_dist, i*(64/sizeof(SpinorBaseType)));
    }
}

void PrefetchL1FullGaugeDirIn(InstVector& ivector, const string& base, const string& off, int dir, bool compress12, int type)
{
#ifndef USE_PACKED_GAUGES
    int nSites = SOALEN;
#else
    int nSites = VECLEN;
#endif
    int g_size=0;

    if( compress12 ) {
        g_size=2*3*2;
    }
    else {
        g_size=3*3*2;
    }

    for(int i = 0; i < ((g_size*nSites*sizeof(GaugeBaseType)+63)/64); i++) {
        prefetchL1GuageDirIn(ivector, base, off, dir, i*(64/sizeof(GaugeBaseType)), type);
    }
}

void PrefetchL2FullGaugeDirIn(InstVector& ivector, const string& base, const string& off, int dir, const string& pref_dist, bool compress12, int type)
{
#ifndef USE_PACKED_GAUGES
    int nSites = SOALEN;
#else
    int nSites = VECLEN;
#endif
    int g_size=0;

    if( compress12 ) {
        g_size=2*3*2;
    }
    else {
        g_size=3*3*2;
    }

    for(int i = 0; i < ((g_size*nSites*sizeof(GaugeBaseType)+63)/64); i++) {
        prefetchL2GuageDirIn(ivector, base, off, dir, pref_dist, i*(64/sizeof(GaugeBaseType)), type);
    }
}

void PrefetchL2FullGaugeIn(InstVector& ivector, const string& base, const string& off, const string& pref_dist, bool compress12, int type)
{
    for(int dir = 0; dir < 8; dir++) {
        PrefetchL2FullGaugeDirIn(ivector, base, off, dir, pref_dist, compress12, type);
    }
}

void PrefetchL1FullCloverBlockIn(InstVector& ivector, const string& base, const string& off, int block)
{
#ifndef USE_PACKED_CLOVER
    int nSites = SOALEN;
#else
    int nSites = VECLEN;
#endif

    for(int i = 0; i < ((36*nSites*sizeof(CloverBaseType)+63)/64); i++) {
        prefetchL1CloverBlockIn(ivector, base, off, block, i*(64/sizeof(CloverBaseType)));
    }
}

void PrefetchL2FullCloverIn(InstVector& ivector, const string& base, const string& off, const string& pref_dist)
{
#ifndef USE_PACKED_CLOVER
    int nSites = SOALEN;
#else
    int nSites = VECLEN;
#endif

    for(int i = 0; i < ((2*36*nSites*sizeof(CloverBaseType)+63)/64); i++) {
        prefetchL2CloverBlockIn(ivector, base, off, 0, pref_dist, i*(64/sizeof(CloverBaseType)));
    }
}

void PrefetchL1HalfSpinorDir(InstVector& ivector, const string& base, int dir, bool isPrefforWrite, int type)
{
    int nActiveLanes = VECLEN;
    int ind = 0;

    if(dir == 0 || dir == 1) {
        nActiveLanes = (VECLEN/SOALEN+1)/2;    // +1 to avoid making it 0 when SOALEN=VECLEN
    }

    if(dir == 2 || dir == 3) {
        nActiveLanes = SOALEN;
    }

#ifdef ENABLE_STREAMING_STORES

    if(nActiveLanes == VECLEN && isPrefforWrite) {
        return;
    }

#endif

    for(int i = 0; i < (12*nActiveLanes*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL1(ivector, new AddressImm(new GenericAddress(base, SpinorType), i*(64/sizeof(SpinorBaseType))), type);
    }
}

void PrefetchL2HalfSpinorDir(InstVector& ivector, const string& base, const string& pref_dist, int dir, bool isPrefforWrite, int type)
{
    int nActiveLanes = VECLEN;
    int ind = 0;

    if(dir == 0 || dir == 1) {
        nActiveLanes = (VECLEN/SOALEN+1)/2;    // +1 to avoid making it 0 when SOALEN=VECLEN
    }

    if(dir == 2 || dir == 3) {
        nActiveLanes = SOALEN;
    }

#ifdef ENABLE_STREAMING_STORES

    if(nActiveLanes == VECLEN && isPrefforWrite) {
        return;
    }

#endif

    for(int i = 0; i < (12*nActiveLanes*sizeof(SpinorBaseType)+63)/64; i++) {
        prefetchL2(ivector, new AddressImm(new AddressOffset(new GenericAddress(base, SpinorType), pref_dist), i*(64/sizeof(SpinorBaseType))), type);
    }
}

