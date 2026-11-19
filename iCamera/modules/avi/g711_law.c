#include <stdio.h>

/* G711A编码，即PCM编码转化为G711A */
static unsigned char ALawEncode(signed short pcm16)
{
    int p = pcm16;
    unsigned a;  // A-law value we are forming
    if(p < 0)
    {
        // -ve value
        // Note, ones compliment is used here as this keeps encoding symetrical
        // and equal spaced around zero cross-over, (it also matches the standard).
        p = ~p;
        a = 0x00; // sign = 0
    }
    else
    {
        // +ve value
        a = 0x80; // sign = 1
    }

    // Calculate segment and interval numbers
    p >>= 4;
    if(p >= 0x20)
    {
        if(p >= 0x100)
        {
            p >>= 4;
            a += 0x40;
        }
        if(p >= 0x40)
        {
            p >>= 2;
            a += 0x20;
        }
        if(p >= 0x20)
        {
            p >>= 1;
            a += 0x10;
        }
    }
    // a&0x70 now holds segment value and 'p' the interval number

    a += p;  // a now equal to encoded A-law value

    return (a ^ 0x55);	// A-law has alternate bits inverted for transmission
}

/* G711A解码，即G711A编码转化为PCM */
static int ALawDecode(unsigned char alaw)
{
    alaw ^= 0x55;  // A-law has alternate bits inverted for transmission

    unsigned sign = alaw & 0x80;

    int linear = alaw & 0x1f;
    linear <<= 4;
    linear += 8;  // Add a 'half' bit (0x08) to place PCM value in middle of range

    alaw &= 0x7f;
    if (alaw >= 0x20)
    {
        linear |= 0x100;  // Put in MSB
        unsigned shift = (alaw >> 4) - 1;
        linear <<= shift;
    }

    if (!sign)
        return (-linear);
    else
        return (linear);
}

/* G711U编码，即PCM编码转化为G711U */
static unsigned char ULawEncode(signed short pcm16)
{
    int p = pcm16;
    unsigned u;  // u-law value we are forming

    if (p < 0)
    {
        // -ve value
        // Note, ones compliment is used here as this keeps encoding symetrical
        // and equal spaced around zero cross-over, (it also matches the standard).
        p = ~p;
        u = 0x80 ^ 0x10 ^ 0xff;  // Sign bit = 1 (^0x10 because this will get inverted later) ^0xff to invert final u-Law code
    }
    else
    {
        // +ve value
        u = 0x00 ^ 0x10 ^ 0xff;  // Sign bit = 0 (-0x10 because this amount extra will get added later) ^0xff to invert final u-Law code
    }

    p += 0x84; // Add uLaw bias

    if (p > 0x7f00)
    {
        p = 0x7f00;  // Clip to 15 bits
    }
    // Calculate segment and interval numbers
    p >>= 3;	// Shift down to 13bit
    if (p >= 0x100)
    {
        p >>= 4;
        u ^= 0x40;
    }

    if (p >= 0x40)
    {
        p >>= 2;
        u ^= 0x20;
    }

    if (p >= 0x20)
    {
        p >>= 1;
        u ^= 0x10;
    }
    // (u^0x10)&0x70 now equal to the segment value and 'p' the interval number (^0x10)

    u ^= p; // u now equal to encoded u-law value (with all bits inverted)

    return (u);
}

/* G711U解码，即G711U编码转化为PCM */
static int ULawDecode(unsigned char ulaw)
{
    ulaw ^= 0xff;  // u-law has all bits inverted for transmission

    int linear = ulaw & 0x0f;
    linear <<= 3;
    linear |= 0x84;  // Set MSB (0x80) and a 'half' bit (0x04) to place PCM value in middle of range

    unsigned shift = ulaw>>4;
    shift &= 7;
    linear <<= shift;

    linear -= 0x84; // Subract uLaw bias

    if (ulaw & 0x80)
    {
        return (-linear);
    }
    else
    {
        return (linear);
    }
}

/* G711A转化为G711U */
static signed char ALawToULaw(unsigned char alaw)
{
    unsigned char sign = alaw & 0x80;
    alaw ^= sign;
    alaw ^= 0x55;
    unsigned ulaw;

    if (alaw < 45)
    {
        if (alaw < 24)
        {
            ulaw = (alaw<8) ? (alaw<<1)+1 : alaw+8;
        }
        else
        {
            ulaw = (alaw<32) ? (alaw>>1)+20 : alaw+4;
        }
    }
    else
    {
        if (alaw < 63)
        {
            ulaw = (alaw<47) ? alaw+3 : alaw+2;
        }
        else
        {
            ulaw = (alaw<79) ? alaw+1 : alaw;
        }
    }

    ulaw ^= sign;
    
    return (ulaw ^ 0x7f);
}

/* G711U转化为G711A */
static unsigned char ULawToALaw(unsigned char ulaw)
{
    unsigned char sign = ulaw & 0x80;
    ulaw ^= sign;
    ulaw ^= 0x7f;
    unsigned alaw;

    if (ulaw < 48)
    {
        if (ulaw <= 32)
        {
            alaw = (ulaw<=15) ? ulaw>>1 : ulaw-8;
        }
        else
        {
            alaw = (ulaw<=35) ? (ulaw<<1)-40 : ulaw-4;
        }
    }
    else
    {
        if (ulaw <= 63)
        {
            alaw = (ulaw==48) ? ulaw-3 : ulaw-2;
        }
        else
        {
            alaw = (ulaw<=79) ? ulaw-1 : ulaw;
        }
    }

    alaw ^= sign;

    return (alaw ^ 0x55);
}

/* 原始PCM数据编码为G711A */
signed PCM_ENCODE_G711A(unsigned char* dst, signed short* src, size_t srcSize)
{
    srcSize >>= 1;
    unsigned char* end = dst + srcSize;

    while(dst < end)
    {
        *dst++ = ALawEncode(*src++);
    }

    return (srcSize);
}

/* G711A转化为PCM */
signed G711A_DECODE_PCM(signed short* dst, const unsigned char* src, size_t srcSize)
{
    signed short* end = dst + srcSize;

    while(dst < end)
    {
        *dst++ = ALawDecode(*src++);
    }

    return (srcSize << 1);
}

/* 原始PCM数据编码为G711U */
unsigned PCM_ENCODE_G711U(unsigned char* dst, signed short* src, size_t srcSize)
{
    srcSize >>= 1;
    unsigned char* end = dst + srcSize;

    while(dst < end)
    {
        *dst++ = ULawEncode(*src++);
    }
    return (srcSize);
}

/* G711U数据解码为PCM */
unsigned G711U_DECODE_PCM(signed short* dst, const unsigned char* src, size_t srcSize)
{
    signed short* end = dst + srcSize;

    while(dst < end)
    {
        *dst++ = ULawDecode(*src++);
    }
    return (srcSize << 1);
}

/* G711A转化为G711U */
unsigned G711A_TO_G711U(unsigned char* dst, const unsigned char* src, size_t srcSize)
{
    unsigned char* end = dst + srcSize;

    while(dst < end)
    {
        *dst++ = ALawToULaw(*src++);
    }
    return (srcSize);
}

/* G711U转化为G711A */
unsigned G711U_TO_G711A(unsigned char* dst, const unsigned char* src, size_t srcSize)
{
    unsigned char* end = dst + srcSize;

    while(dst < end)
    {
        *dst++ = ULawToALaw(*src++);
    }
    return (srcSize);
}