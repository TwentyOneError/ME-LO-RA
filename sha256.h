#ifndef SHA256_H
#define SHA256_H

#include <Arduino.h>
#include <WString.h>

class SHA256 {
public:
    static String hash(const String& input);
    static String hash(const uint8_t* data, size_t length);

private:
    typedef uint32_t WORD;
    typedef uint64_t UINT64;
    typedef uint8_t BYTE;
    
    static const uint32_t K[64];
    static const uint32_t H0[8];
    
    static WORD rotr(WORD x, int n);
    static WORD ch(WORD x, WORD y, WORD z);
    static WORD maj(WORD x, WORD y, WORD z);
    static WORD sigma0(WORD x);
    static WORD sigma1(WORD x);
    static WORD theta0(WORD x);
    static WORD theta1(WORD x);
    static void transform(WORD state[8], const BYTE block[64]);
};

#endif
