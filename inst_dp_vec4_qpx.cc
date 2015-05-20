#include <stdlib.h>
#include <stdio.h>
#include "instructions.h"

#if PRECISION == 2 && VECLEN == 4
#pragma message "Using BGQ double Precision"

#define FVECTYPE "vector4double"
using namespace std;


const string fullIntMask("0xF");
const string fullMask("0xF");

string DeclareOffsets::serialize() const
{
  return "";
}
FVec::FVec(const string& name_) : name(name_), type(FVECTYPE) {}

string DeclareFVec::serialize() const
{
    return v.getType() + " " + v.getName() + " = vec_splats(0.0);";
}

string InitFVec::serialize() const
{
    //return v.getName()+ " = vec_splats(0.0);";
    return "";
}


string DeclareMask::serialize() const
{
    ostringstream outbuf;

    if(value.empty()) {
        outbuf << FVECTYPE << " " << name << ";" << endl;
    }
    else {
        outbuf << FVECTYPE << " " << name << " = " << value << ";" << endl;
    }

    return outbuf.str();
}

string IntToMask::serialize() const
{
    ostringstream outbuf;
    outbuf << mask << " = _v4d_int2mask(" << value << ");" << endl;
    return outbuf.str();
}

string IfAllOneCond::serialize() const
{
    return "if((" + condition + " & " + fullIntMask + ") ""== " + fullIntMask + ") { ";
}

string LoadFVec::serialize() const
{
    std::ostringstream buf;

    if(mask.empty()) {
        if(!a->isHalfType()) {
     	    buf << v.getName() << " = vec_ld(0, const_cast<double *>(" 
                << a->serialize() << "));" 
                << endl;
        }
        else {
	    buf << v.getName() << " = vec_ld(0, const_cast<float *>(" 
                << a->serialize() << "));" 
                << endl;
        }
    }
    else {
        if(!a->isHalfType()) {
        
            /* Define a zero vector. */
            buf << v.getType() << " zeroVec = vec_splats(0.0);" << endl; 
            
            /* Load the vector from memory. */
            buf << v.getName() << " = vec_ld(0,const_cast<double *>(" 
                << a->serialize() << "));" 
                << endl;
                
            /* Blend with the zero vector. Retain the elements that have the 
             * corresponding high bit set in the mask. 
             */
            buf << v.getName() << " = vec_perm( zeroVec, " << v.getName() 
                << "," << mask << ");" << endl;
        }
        else {

            /* Define a zero vector. */
            buf << v.getType() << " zeroVec = vec_splats(0.0);" << endl; 
            
            /* Load the vector from memory. */
            buf << v.getName() << " = vec_ld(0,const_cast<float *>(" 
                << a->serialize() << "));" 
                << endl;
                
            /* Blend with the zero vector. Retain the elements that have the 
             * corresponding high bit set in the mask. 
             */
            buf << v.getName() << " = vec_perm( zeroVec, " << v.getName() 
                << "," << mask << ");" << endl;

        }
    }

    return buf.str();

}

string StoreFVec::serialize() const
{
    ostringstream buf;
    //int streaming = isStreaming;
    //TODO : Does QPX have an equivalent of AVX _mm256_stream_pd?
    if(!a->isHalfType()) {
      buf << "vec_sta("  << v.getName() << ", 0, const_cast<double *>(" << a->serialize() << "));" 
          << endl;
    }
    else {
      buf << "vec_sta("  << v.getName() << ", 0, const_cast<float *>(" << a->serialize() << "));" 
          << endl;

    }
    return buf.str();
}

string LoadBroadcast::serialize() const
{
    std::ostringstream buf;

    if(!a->isHalfType()) {
        buf << v.getName() << " = vec_splats((double)(*(const_cast<double *>(" << a->serialize() << "))));" 
            << endl;
    }
    else {
      buf << v.getName() << " = vec_splats((double)(*(const_cast<float *>(" << a->serialize() << "))));" 
            << endl;

    }

    return buf.str();
}

// FIXME : Letting this be so that the code compiles. For BGQ these hints
// are meaningless. We also emit an empty string on serialization, so should
// be harmless.
//
// TODO: Recode the L1 prefetcher in the BGQ way.
PrefetchL1::PrefetchL1( const Address* a_, int type) : a(a_)
{
    // Type: 0 - none, 1 - NT, 2 - Ex, 3 - NT+Ex
    switch (type) {
    case 0:
        hint = "_MM_HINT_T0";
        break;

    case 1:
        hint = "_MM_HINT_NTA";
        break;

    case 2:
        hint = "_MM_HINT_T0";
        break;

    case 3:
        hint = "_MM_HINT_NTA";
        break;
    }
}


// FIXME : Letting this be so that the code compiles. For BGQ these hints
// are meaningless. We also emit an empty string on serialization, so should
// be harmless.
//
// TODO: Recode the L2 prefetcher in the BGQ way.
PrefetchL2::PrefetchL2( const Address* a_, int type) : a(a_)
{
    // Type: 0 - none, 1 - NT, 2 - Ex, 3 - NT+Ex
    switch(type) {
    case 0:
        hint = "_MM_HINT_T1";
        break;

    case 1:
        hint = "_MM_HINT_T2";
        break;

    case 2:
        hint = "_MM_HINT_T1";
        break;

    case 3:
        hint = "_MM_HINT_T2";
        break;
    }
}

string SetZero::serialize() const
{
    return  ret.getName() + " = vec_splats(0.0); ";
}

string Mul::serialize() const
{
    if(mask.empty()) {
        return  ret.getName() + " = vec_mul(" 
                              + a.getName() + ", " 
                              + b.getName() + ");";
    }
    else {
        return  ret.getName() + " = vec_perm(" + ret.getName() 
                              + ", vec_mul( "
                              + a.getName()+" , "
                              + b.getName()+"), " + mask + ");";
    }
}

string FnMAdd::serialize() const
{
    if(mask.empty()) {
        return  ret.getName()+ " = vec_nmsub(" 
                             + a.getName() + " , "
                             + b.getName() + " , " 
                             + c.getName() + " );";
    }
    else {
        return  ret.getName()+" = vec_perm(" + ret.getName() 
                             + ", vec_nmsub("+a.getName()+" ,"
                             + b.getName() + " ,"
                             + c.getName() + " ) ," + mask + ");";
    }

}

string FMAdd::serialize() const
{
    if(mask.empty()) {
        return  ret.getName() + " = vec_madd(" 
                              + a.getName() 
                              + " ," 
                              + b.getName()
                              + " ,"
                              + c.getName() + ");";
    }
    else {
        return  ret.getName() + " = vec_perm(" 
                              + ret.getName() 
                              + ", vec_madd("
                              + a.getName()
                              + " ,"
                              + b.getName() + " ," 
                              + c.getName() + " )," 
                              + mask + ");" ;
    }
}

string Add::serialize() const
{
    if(mask.empty()) {
        return  ret.getName() + " = vec_add( "+a.getName() 
                              + " , "+b.getName()+" );";
    }
    else {
        return  ret.getName() + " = vec_perm(" + ret.getName() 
                              + ", vec_add("+a.getName()
                              + " , "+b.getName()+"), " + mask + ");";
    }
}

string Sub::serialize() const
{
    if(mask.empty()) {
        return  ret.getName() + " = vec_sub("+a.getName() 
                              + " , "+b.getName()+" );";
    }
    else {
        return  ret.getName() + " = vec_perm(" + ret.getName() 
                              + ", vec_sub(" + a.getName()+", " + b.getName()
                              + "), " + mask + ");";
    }
}

string MovFVec::serialize() const
{
    if(mask.empty()) {
        return  ret.getName()+" = " + a.getName()+";" ;
    }
    else {
        return ret.getName() + " = vec_perm(" + ret.getName() 
                             + ", " + a.getName() + ", " + mask + ");";
    }
}
//FIXME : Add support for Perm64x2 once SOALEN support is added to QPX backend
/*
class Perm64x2 : public Instruction
{
public:
    Perm64x2(const FVec& ret_
           , const FVec& a_
           , const FVec& b_
           , int imm_) : ret(ret_), a(a_), b(b_), imm(imm_) {}
           
    string serialize() const
    {
        ostringstream stream;
        stream << ret.getName() << " = _mm256_permute2f128_pd(" 
               << a.getName() << ", "  << b.getName() << ", " << imm << ");" ;
        return stream.str();
    }
    
    int numArithmeticInst() const
    {
        return 0;
    }
private:
    const FVec ret;
    const FVec a;
    const FVec b;
    int imm;
};
*/

class LoadSplitSOAFVec : public MemRefInstruction
{
public:
    LoadSplitSOAFVec( const FVec& v_
                    , const Address* a1_
                    , const Address* a2_
                    , const int soanum_
                    , const int soalen_
                    , int forward_) 
                    : v(v_), a1(a1_), a2(a2_) , soalen(soalen_)
                   , soanum(soanum_), forward(forward_) {}
                    
    string serialize() const
    {
        std::ostringstream buf;

        if(!a1->isHalfType()) {
            if(forward) {
                if(soalen == 4) {
		  buf   << "pctl1 = vec_lvsl(0, const_cast<double *> (" << a1->serialize() << "));\n"
                        << "pctl2 = vec_lvsl(0, const_cast<double *> (" << a2->serialize() << "));\n"

                        << "v1    =  vec_ld(0, const_cast<double *> (" << a1->serialize() << "));\n"
	                << "v2    =  vec_ld(0, const_cast<double *> (" << a2->serialize() << "));\n"
	
                        << v.getName() << " = vec_perm(vec_perm(v1, v2, pctl1)"
                                                    ", vec_perm(v2, v1, pctl2)"
                                                    ", vec_gpci(00124));"
		        << endl;
                }
                else {
                     buf << "#error\"soalen 2 is not supported in "
                            "the QPX backend\"" <<endl;
                     printf("FIXME: soalen 2 is not supported in "
                            "the QPX backend\n");
                     exit(1);
                }
            }
            else {
                if(soalen == 4) {
                  buf   << "pctl1 = vec_lvsl(0, const_cast<double *> (" << a1->serialize() << "));\n"
                        << "pctl2 = vec_lvsl(0, const_cast<double *> (" << a2->serialize() << "));\n"

                        << "v1    =  vec_ld(0, const_cast<double *> (" << a1->serialize() << "));\n"
	                << "v2    =  vec_ld(0, const_cast<double *> (" << a2->serialize() << "));\n"
	
                        << v.getName() << " = vec_perm(vec_perm(v1, v2, pctl1)"
                                                    ", vec_perm(v2, v1, pctl2)"
                                                    ", vec_gpci(00456));"
                        << endl;
                }
                //FIXME
                else {
                    buf << "#error\"soalen 2 is not supported in " 
                           "the QPX backend\"" 
                        << endl;
                    printf("FIXME: soalen 2 is not supported in " 
                           "the QPX backend\n");
                    exit(1);
                }
            }
        }
        else {
            if(forward) {
                if(soalen == 4) {
		  buf   << "pctl1 = vec_lvsl(0, const_cast<float *> (" << a1->serialize() << "));\n"
                        << "pctl2 = vec_lvsl(0, const_cast<float *> (" << a2->serialize() << "));\n"

                        << "v1    =  vec_ld(0, const_cast<float *> (" << a1->serialize() << "));\n"
	                << "v2    =  vec_ld(0, const_cast<float *> (" << a2->serialize() << "));\n"
	
                        << v.getName() << " = vec_perm(vec_perm(v1, v2, pctl1)"
                                                    ", vec_perm(v2, v1, pctl2)"
                                                    ", vec_gpci(00124));"
		        << endl;
                }
                else {
                     buf << "#error\"soalen 2 is not supported in "
                            "the QPX backend\"" <<endl;
                     printf("FIXME: soalen 2 is not supported in "
                            "the QPX backend\n");
                     exit(1);
                }
            }
            else {
                if(soalen == 4) {
                  buf   << "pctl1 = vec_lvsl(0, const_cast<float *> (" << a1->serialize() << "));\n"
                        << "pctl2 = vec_lvsl(0, const_cast<float *> (" << a2->serialize() << "));\n"

                        << "v1    =  vec_ld(0, const_cast<float *> (" << a1->serialize() << "));\n"
	                << "v2    =  vec_ld(0, const_cast<float *> (" << a2->serialize() << "));\n"
	
                        << v.getName() << " = vec_perm(vec_perm(v1, v2, pctl1)"
                                                    ", vec_perm(v2, v1, pctl2)"
                                                    ", vec_gpci(00456));"
                        << endl;
                }
                //FIXME
                else {
                    buf << "#error\"soalen 2 is not supported in " 
                           "the QPX backend\"" 
                        << endl;
                    printf("FIXME: soalen 2 is not supported in " 
                           "the QPX backend\n");
                    exit(1);
                }
            }
        }
        return buf.str();
    }
    
    const Address* getAddress() const
    {
        return a1;
    }
    
    MemRefType getType() const
    {
        return LOAD_MASKED_VEC;
    }
private:
    const FVec v;
    const Address* a1;
    const Address* a2;
    const int soalen, soanum;
    const int forward;
};

class CondInsertFVecElement : public MemRefInstruction
{
public:
    CondInsertFVecElement( const FVec& v_
                         , const Address* a_
                         , const string mask_
                         , int pos_
                         , bool skipCond_
                         ) 
                         : v(v_), a(a_), mask(mask_), pos(pos_)
                         , skipCond(skipCond_) {}
                         
    string serialize() const
    {
        std::ostringstream buf;

        if(!skipCond) {
            buf << "if(" << mask << " & " << (1 << pos) << ") ";
        }

        if(!a->isHalfType()) {
            
            buf << v.getName() << " = vec_perm(" << v.getName() 
                << ", vec_splats( (double)(*(const_cast<double *>(" << a->serialize() << "))))" 
                << ", _v4d_int2mask(" << (1 << pos) << ")"
                << ");" << endl;
        }
        else {
            buf << v.getName() << " = vec_perm(" << v.getName() 
                << ", vec_splats( (double)(*(const_cast<float *>(" << a->serialize() << "))))" 
                << ", _v4d_int2mask(" << (1 << pos) << ")"
                << ");" << endl;
        }

        return buf.str();
    }
    const Address* getAddress() const
    {
        return a;
    }
    MemRefType getType() const
    {
        return STORE_MASKED_VEC;
    }
private:
    const FVec v;
    const Address* a;
    const string mask;
    const int pos;
    const bool skipCond;
};

class CondExtractFVecElement : public MemRefInstruction
{
public:
    CondExtractFVecElement( const FVec& v_
                          , const Address* a_
                          , const string mask_
                          , int pos_
                          , bool skipCond_) 
                          : v(v_), a(a_), mask(mask_)
                          , pos(pos_), skipCond(skipCond_) {}
    string serialize() const
    {
        std::ostringstream buf;

        if(!skipCond) {
            buf << "if(" << mask << " & " << (1 << pos) << ") ";
        }

        if(!a->isHalfType()) {
            if(pos % 2 == 0) {
                int p =  ((pos/2)&1)?2:0;
                buf << "vec_sts(" 
                    << "vec_promote(" 
                        << "vec_extract(" << v.getName() << "," << p  << ")"
                        << ", 0)"
                    << ", 0"
                    << ", const_cast<double *>(" << a->serialize()
                    << "));"
                    << endl;
            }
            else {
                int p =  ((pos/2)&1)?3:1;
                buf << "vec_sts(" 
                    << "vec_promote(" 
                        << "vec_extract(" << v.getName() << "," << p  << ")"
                        << ", 0)"
                    << ", 0"
                    << ", const_cast<double *>(" << a->serialize()
                    << "));"
                    << endl;
            }
             
        }
        else {
            if(pos % 2 == 0) {
                int p =  ((pos/2)&1)?2:0;
                buf << "vec_sts(" 
                    << "vec_promote(" 
                        << "vec_extract(" << v.getName() << "," << p  << ")"
                        << ", 0)"
                    << ", 0"
                    << ", const_cast<float *>(" << a->serialize()
                    << "));"
                    << endl;
            }
            else {
                int p =  ((pos/2)&1)?3:1;
                buf << "vec_sts(" 
                    << "vec_promote(" 
                        << "vec_extract(" << v.getName() << "," << p  << ")"
                        << ", 0)"
                    << ", 0"
                    << ", const_cast<float *>(" << a->serialize()
                    << "));"
                    << endl;
            }

        }

        return buf.str();
    }
    const Address* getAddress() const
    {
        return a;
    }
    MemRefType getType() const
    {
        return STORE_MASKED_VEC;
    }
private:
    const FVec v;
    const Address* a;
    const string mask;
    const int pos;
    const bool skipCond;
};

void loadSOAFVec(InstVector& ivector
               , const FVec& ret
               , const Address *a
               , int soanum
               , int soalen
               , string mask)
{
    if(soalen == 4) {
        ivector.push_back( new LoadFVec(ret, a, string("")));
    } 
    //FIXME : Add support for soalen == 2
    /*
    else if(soalen == 2) {
        ivector.push_back( new LoadHalfFVec(ret, a, soanum));
    }
    */
    else {
        printf("SOALEN = %d not supported\n", soalen);
        exit(1);
    }
}

void storeSOAFVec(InstVector& ivector
                , const FVec& ret
                , const Address *a
                , int soanum
                , int soalen)
{
    if(soalen == 4) {
        ivector.push_back( new StoreFVec(ret, a, 0));
    }
    //FIXME : Add support for soalen == 2
    /*
    else if(soalen == 2) {
        ivector.push_back( new StoreHalfFVec(ret, a, soanum));
    }
    */
    else {
        printf("SOALEN = %d not supported\n", soalen);
        exit(1);
    }
}

void loadSplitSOAFVec(InstVector& ivector
                    , const FVec& ret
                    , const Address *a1
                    , const Address *a2
                    , int soanum
                    , int soalen
                    , int forward
                    , string mask)
{
    ivector.push_back(new LoadSplitSOAFVec(ret,a1,a2,soanum,soalen,forward));
}


void unpackFVec(InstVector& ivector
              , const FVec& ret
              , Address *a
              , string mask
              , int possibleMask)
{
    int pos = 0, nBits = 0;

    for(int i = 0; i < 4; i++) if(possibleMask & (1 << i)) {
            nBits++;
    }

    for(int i = 0; i < 4; i++) {
        if(possibleMask & (1 << i)) {
            ivector.push_back(new CondInsertFVecElement(ret
                                                      , new AddressImm(a, pos)
                                                      , mask
                                                      , i
                                                      , nBits==1));
            //pos++;
        }
    }
}

void packFVec(InstVector& ivector
            , const FVec& ret
            , Address *a
            , string mask
            , int possibleMask)
{
    int pos = 0, nBits = 0;

    for(int i = 0; i < 4; i++) {
        if(possibleMask & (1 << i)) {
            nBits++;
        }
    }

    for(int i = 0; i < 4; i++) {
        if(possibleMask & (1 << i)) {
            ivector.push_back(new CondExtractFVecElement(ret
                                                       , new AddressImm(a, pos)
                                                       , mask
                                                       , i
                                                       , nBits==1));
            //pos++;
        }
    }
}

/*
void fperm64x2(InstVector& ivector
             , const FVec& ret
             , const FVec& a
             , const FVec& b
             , const int imm)
{
    ivector.push_back(new Perm64x2(ret, a, b, imm));
}

void transpose2x2(InstVector& ivector, const FVec r[2], const FVec f[2])
{
    for(int i = 0; i < 2; i++) {
        fperm64x2(ivector, r[i], f[0], f[1], 0x20+i*0x11);
    }
}
*/
void transpose1x1(InstVector& ivector, const FVec r[1], const FVec f[1])
{
    movFVec(ivector, r[0], f[0], string(""));
}

void transpose(InstVector& ivector, const FVec r[], const FVec f[], int soalen)
{
    switch (soalen) {
    case 2:
        // FIXME : Add support for SOALEN 2 
        //transpose2x2(ivector, r, f);
        printf("SOALEN = %d Not Yet supported " 
               "(only SOALEN = 4 supported)\n", soalen); 
        break;

    case 4:
        transpose1x1(ivector, r, f);
        break;

    default:
        printf("SOALEN = %d Not Yet supported "
               "(only SOALEN = 4 supported)\n", soalen); 
    }
}

#endif // PRECISION == 2
