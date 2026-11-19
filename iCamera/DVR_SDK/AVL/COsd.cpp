// Copyright (C) 1991 - 1999 Rational Software Corporation
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include "COsd.h"
#include "ch_default_pixel.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <unistd.h>
#include "AVL_global.h"
#include <sys/time.h>
#include "../PAR/CPubFunc.h"
#include "common_pub.h"

using namespace std;


#define STRING_OSD

static int notCheckParam = 0;
static int SetOsdDirect = 0;
static char strvalue[256] = "73:80:67:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0";

typedef struct tagBITMAPINFOHEADER
{
	unsigned int biSize;
	int biWidth;
	int biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
} __attribute__ ((packed)) BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagBITMAPFILEHEADER
{
	unsigned short bfType;
	unsigned int bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;
} __attribute__ ((packed)) BITMAPFILEHEADER, *PBITMAPFILEHEADER;


#define MAX_OSD_FILE_SIZE         (1024*1024)

typedef unsigned short	uint16;
typedef unsigned char	uchar;

const uint16 uni_table[][2]={
	{0x00A4,0xA1E8}, // ¤
	{0x00A7,0xA1EC}, // §
	{0x00A8,0xA1A7}, // ¨
	{0x00B0,0xA1E3}, // °
	{0x00B1,0xA1C0}, // ±
	{0x00B7,0xA1A4}, // ·
	{0x00D7,0xA1C1}, // ×
	{0x00E0,0xA8A4}, // à
	{0x00E1,0xA8A2}, // á
	{0x00E8,0xA8A8}, // è
	{0x00E9,0xA8A6}, // é
	{0x00EA,0xA8BA}, // ê
	{0x00EC,0xA8AC}, // ì
	{0x00ED,0xA8AA}, // í
	{0x00F2,0xA8B0}, // ò
	{0x00F3,0xA8AE}, // ó
	{0x00F7,0xA1C2}, // ÷
	{0x00F9,0xA8B4}, // ù
	{0x00FA,0xA8B2}, // ú
	{0x00FC,0xA8B9}, // ü
	{0x0101,0xA8A1}, // ā
	{0x0113,0xA8A5}, // ē
	{0x011B,0xA8A7}, // ě
	{0x012B,0xA8A9}, // ī
	{0x014D,0xA8AD}, // ō
	{0x016B,0xA8B1}, // ū
	{0x01CE,0xA8A3}, // ǎ
	{0x01D0,0xA8AB}, // ǐ
	{0x01D2,0xA8AF}, // ǒ
	{0x01D4,0xA8B3}, // ǔ
	{0x01D6,0xA8B5}, // ǖ
	{0x01D8,0xA8B6}, // ǘ
	{0x01DA,0xA8B7}, // ǚ
	{0x01DC,0xA8B8}, // ǜ
	{0x02C7,0xA1A6}, // ˇ
	{0x02C9,0xA1A5}, // ˉ
	{0x0391,0xA6A1}, // Α
	{0x0392,0xA6A2}, // Β
	{0x0393,0xA6A3}, // Γ
	{0x0394,0xA6A4}, // Δ
	{0x0395,0xA6A5}, // Ε
	{0x0396,0xA6A6}, // Ζ
	{0x0397,0xA6A7}, // Η
	{0x0398,0xA6A8}, // Θ
	{0x0399,0xA6A9}, // Ι
	{0x039A,0xA6AA}, // Κ
	{0x039B,0xA6AB}, // Λ
	{0x039C,0xA6AC}, // Μ
	{0x039D,0xA6AD}, // Ν
	{0x039E,0xA6AE}, // Ξ
	{0x039F,0xA6AF}, // Ο
	{0x03A0,0xA6B0}, // Π
	{0x03A1,0xA6B1}, // Ρ
	{0x03A3,0xA6B2}, // Σ
	{0x03A4,0xA6B3}, // Τ
	{0x03A5,0xA6B4}, // Υ
	{0x03A6,0xA6B5}, // Φ
	{0x03A7,0xA6B6}, // Χ
	{0x03A8,0xA6B7}, // Ψ
	{0x03A9,0xA6B8}, // Ω
	{0x03B1,0xA6C1}, // α
	{0x03B2,0xA6C2}, // β
	{0x03B3,0xA6C3}, // γ
	{0x03B4,0xA6C4}, // δ
	{0x03B5,0xA6C5}, // ε
	{0x03B6,0xA6C6}, // ζ
	{0x03B7,0xA6C7}, // η
	{0x03B8,0xA6C8}, // θ
	{0x03B9,0xA6C9}, // ι
	{0x03BA,0xA6CA}, // κ
	{0x03BB,0xA6CB}, // λ
	{0x03BC,0xA6CC}, // μ
	{0x03BD,0xA6CD}, // ν
	{0x03BE,0xA6CE}, // ξ
	{0x03BF,0xA6CF}, // ο
	{0x03C0,0xA6D0}, // π
	{0x03C1,0xA6D1}, // ρ
	{0x03C3,0xA6D2}, // σ
	{0x03C4,0xA6D3}, // τ
	{0x03C5,0xA6D4}, // υ
	{0x03C6,0xA6D5}, // φ
	{0x03C7,0xA6D6}, // χ
	{0x03C8,0xA6D7}, // ψ
	{0x03C9,0xA6D8}, // ω
	{0x0401,0xA7A7}, // Ё
	{0x0410,0xA7A1}, // А
	{0x0411,0xA7A2}, // Б
	{0x0412,0xA7A3}, // В
	{0x0413,0xA7A4}, // Г
	{0x0414,0xA7A5}, // Д
	{0x0415,0xA7A6}, // Е
	{0x0416,0xA7A8}, // Ж
	{0x0417,0xA7A9}, // З
	{0x0418,0xA7AA}, // И
	{0x0419,0xA7AB}, // Й
	{0x041A,0xA7AC}, // К
	{0x041B,0xA7AD}, // Л
	{0x041C,0xA7AE}, // М
	{0x041D,0xA7AF}, // Н
	{0x041E,0xA7B0}, // О
	{0x041F,0xA7B1}, // П
	{0x0420,0xA7B2}, // Р
	{0x0421,0xA7B3}, // С
	{0x0422,0xA7B4}, // Т
	{0x0423,0xA7B5}, // У
	{0x0424,0xA7B6}, // Ф
	{0x0425,0xA7B7}, // Х
	{0x0426,0xA7B8}, // Ц
	{0x0427,0xA7B9}, // Ч
	{0x0428,0xA7BA}, // Ш
	{0x0429,0xA7BB}, // Щ
	{0x042A,0xA7BC}, // Ъ
	{0x042B,0xA7BD}, // Ы
	{0x042C,0xA7BE}, // Ь
	{0x042D,0xA7BF}, // Э
	{0x042E,0xA7C0}, // Ю
	{0x042F,0xA7C1}, // Я
	{0x0430,0xA7D1}, // а
	{0x0431,0xA7D2}, // б
	{0x0432,0xA7D3}, // в
	{0x0433,0xA7D4}, // г
	{0x0434,0xA7D5}, // д
	{0x0435,0xA7D6}, // е
	{0x0436,0xA7D8}, // ж
	{0x0437,0xA7D9}, // з
	{0x0438,0xA7DA}, // и
	{0x0439,0xA7DB}, // й
	{0x043A,0xA7DC}, // к
	{0x043B,0xA7DD}, // л
	{0x043C,0xA7DE}, // м
	{0x043D,0xA7DF}, // н
	{0x043E,0xA7E0}, // о
	{0x043F,0xA7E1}, // п
	{0x0440,0xA7E2}, // р
	{0x0441,0xA7E3}, // с
	{0x0442,0xA7E4}, // т
	{0x0443,0xA7E5}, // у
	{0x0444,0xA7E6}, // ф
	{0x0445,0xA7E7}, // х
	{0x0446,0xA7E8}, // ц
	{0x0447,0xA7E9}, // ч
	{0x0448,0xA7EA}, // ш
	{0x0449,0xA7EB}, // щ
	{0x044A,0xA7EC}, // ъ
	{0x044B,0xA7ED}, // ы
	{0x044C,0xA7EE}, // ь
	{0x044D,0xA7EF}, // э
	{0x044E,0xA7F0}, // ю
	{0x044F,0xA7F1}, // я
	{0x0451,0xA7D7}, // ё
	{0x2014,0xA1AA}, // —
	{0x2016,0xA1AC}, // ‖
	{0x2018,0xA1AE}, // ‘
	{0x2019,0xA1AF}, // ’
	{0x201C,0xA1B0}, // “
	{0x201D,0xA1B1}, // ”
	{0x2026,0xA1AD}, // …
	{0x2030,0xA1EB}, // ‰
	{0x2032,0xA1E4}, // ′
	{0x2033,0xA1E5}, // ″
	{0x203B,0xA1F9}, // ※
	{0x2103,0xA1E6}, // ℃
	{0x2116,0xA1ED}, // №
	{0x2160,0xA2F1}, // Ⅰ
	{0x2161,0xA2F2}, // Ⅱ
	{0x2162,0xA2F3}, // Ⅲ
	{0x2163,0xA2F4}, // Ⅳ
	{0x2164,0xA2F5}, // Ⅴ
	{0x2165,0xA2F6}, // Ⅵ
	{0x2166,0xA2F7}, // Ⅶ
	{0x2167,0xA2F8}, // Ⅷ
	{0x2168,0xA2F9}, // Ⅸ
	{0x2169,0xA2FA}, // Ⅹ
	{0x216A,0xA2FB}, // Ⅺ
	{0x216B,0xA2FC}, // Ⅻ
	{0x2190,0xA1FB}, // ←
	{0x2191,0xA1FC}, // ↑
	{0x2192,0xA1FA}, // →
	{0x2193,0xA1FD}, // ↓
	{0x2208,0xA1CA}, // ∈
	{0x220F,0xA1C7}, // ∏
	{0x2211,0xA1C6}, // ∑
	{0x221A,0xA1CC}, // √
	{0x221D,0xA1D8}, // ∝
	{0x221E,0xA1DE}, // ∞
	{0x2220,0xA1CF}, // ∠
	{0x2225,0xA1CE}, // ∥
	{0x2227,0xA1C4}, // ∧
	{0x2228,0xA1C5}, // ∨
	{0x2229,0xA1C9}, // ∩
	{0x222A,0xA1C8}, // ∪
	{0x222B,0xA1D2}, // ∫
	{0x222E,0xA1D3}, // ∮
	{0x2234,0xA1E0}, // ∴
	{0x2235,0xA1DF}, // ∵
	{0x2236,0xA1C3}, // ∶
	{0x2237,0xA1CB}, // ∷
	{0x223D,0xA1D7}, // ∽
	{0x2248,0xA1D6}, // ≈
	{0x224C,0xA1D5}, // ≌
	{0x2260,0xA1D9}, // ≠
	{0x2261,0xA1D4}, // ≡
	{0x2264,0xA1DC}, // ≤
	{0x2265,0xA1DD}, // ≥
	{0x226E,0xA1DA}, // ≮
	{0x226F,0xA1DB}, // ≯
	{0x2299,0xA1D1}, // ⊙
	{0x22A5,0xA1CD}, // ⊥
	{0x2312,0xA1D0}, // ⌒
	{0x2460,0xA2D9}, // ①
	{0x2461,0xA2DA}, // ②
	{0x2462,0xA2DB}, // ③
	{0x2463,0xA2DC}, // ④
	{0x2464,0xA2DD}, // ⑤
	{0x2465,0xA2DE}, // ⑥
	{0x2466,0xA2DF}, // ⑦
	{0x2467,0xA2E0}, // ⑧
	{0x2468,0xA2E1}, // ⑨
	{0x2469,0xA2E2}, // ⑩
	{0x2474,0xA2C5}, // ⑴
	{0x2475,0xA2C6}, // ⑵
	{0x2476,0xA2C7}, // ⑶
	{0x2477,0xA2C8}, // ⑷
	{0x2478,0xA2C9}, // ⑸
	{0x2479,0xA2CA}, // ⑹
	{0x247A,0xA2CB}, // ⑺
	{0x247B,0xA2CC}, // ⑻
	{0x247C,0xA2CD}, // ⑼
	{0x247D,0xA2CE}, // ⑽
	{0x247E,0xA2CF}, // ⑾
	{0x247F,0xA2D0}, // ⑿
	{0x2480,0xA2D1}, // ⒀
	{0x2481,0xA2D2}, // ⒁
	{0x2482,0xA2D3}, // ⒂
	{0x2483,0xA2D4}, // ⒃
	{0x2484,0xA2D5}, // ⒄
	{0x2485,0xA2D6}, // ⒅
	{0x2486,0xA2D7}, // ⒆
	{0x2487,0xA2D8}, // ⒇
	{0x2488,0xA2B1}, // ⒈
	{0x2489,0xA2B2}, // ⒉
	{0x248A,0xA2B3}, // ⒊
	{0x248B,0xA2B4}, // ⒋
	{0x248C,0xA2B5}, // ⒌
	{0x248D,0xA2B6}, // ⒍
	{0x248E,0xA2B7}, // ⒎
	{0x248F,0xA2B8}, // ⒏
	{0x2490,0xA2B9}, // ⒐
	{0x2491,0xA2BA}, // ⒑
	{0x2492,0xA2BB}, // ⒒
	{0x2493,0xA2BC}, // ⒓
	{0x2494,0xA2BD}, // ⒔
	{0x2495,0xA2BE}, // ⒕
	{0x2496,0xA2BF}, // ⒖
	{0x2497,0xA2C0}, // ⒗
	{0x2498,0xA2C1}, // ⒘
	{0x2499,0xA2C2}, // ⒙
	{0x249A,0xA2C3}, // ⒚
	{0x249B,0xA2C4}, // ⒛
	{0x2500,0xA9A4}, // ─
	{0x2501,0xA9A5}, // ━
	{0x2502,0xA9A6}, // │
	{0x2503,0xA9A7}, // ┃
	{0x2504,0xA9A8}, // ┄
	{0x2505,0xA9A9}, // ┅
	{0x2506,0xA9AA}, // ┆
	{0x2507,0xA9AB}, // ┇
	{0x2508,0xA9AC}, // ┈
	{0x2509,0xA9AD}, // ┉
	{0x250A,0xA9AE}, // ┊
	{0x250B,0xA9AF}, // ┋
	{0x250C,0xA9B0}, // ┌
	{0x250D,0xA9B1}, // ┍
	{0x250E,0xA9B2}, // ┎
	{0x250F,0xA9B3}, // ┏
	{0x2510,0xA9B4}, // ┐
	{0x2511,0xA9B5}, // ┑
	{0x2512,0xA9B6}, // ┒
	{0x2513,0xA9B7}, // ┓
	{0x2514,0xA9B8}, // └
	{0x2515,0xA9B9}, // ┕
	{0x2516,0xA9BA}, // ┖
	{0x2517,0xA9BB}, // ┗
	{0x2518,0xA9BC}, // ┘
	{0x2519,0xA9BD}, // ┙
	{0x251A,0xA9BE}, // ┚
	{0x251B,0xA9BF}, // ┛
	{0x251C,0xA9C0}, // ├
	{0x251D,0xA9C1}, // ┝
	{0x251E,0xA9C2}, // ┞
	{0x251F,0xA9C3}, // ┟
	{0x2520,0xA9C4}, // ┠
	{0x2521,0xA9C5}, // ┡
	{0x2522,0xA9C6}, // ┢
	{0x2523,0xA9C7}, // ┣
	{0x2524,0xA9C8}, // ┤
	{0x2525,0xA9C9}, // ┥
	{0x2526,0xA9CA}, // ┦
	{0x2527,0xA9CB}, // ┧
	{0x2528,0xA9CC}, // ┨
	{0x2529,0xA9CD}, // ┩
	{0x252A,0xA9CE}, // ┪
	{0x252B,0xA9CF}, // ┫
	{0x252C,0xA9D0}, // ┬
	{0x252D,0xA9D1}, // ┭
	{0x252E,0xA9D2}, // ┮
	{0x252F,0xA9D3}, // ┯
	{0x2530,0xA9D4}, // ┰
	{0x2531,0xA9D5}, // ┱
	{0x2532,0xA9D6}, // ┲
	{0x2533,0xA9D7}, // ┳
	{0x2534,0xA9D8}, // ┴
	{0x2535,0xA9D9}, // ┵
	{0x2536,0xA9DA}, // ┶
	{0x2537,0xA9DB}, // ┷
	{0x2538,0xA9DC}, // ┸
	{0x2539,0xA9DD}, // ┹
	{0x253A,0xA9DE}, // ┺
	{0x253B,0xA9DF}, // ┻
	{0x253C,0xA9E0}, // ┼
	{0x253D,0xA9E1}, // ┽
	{0x253E,0xA9E2}, // ┾
	{0x253F,0xA9E3}, // ┿
	{0x2540,0xA9E4}, // ╀
	{0x2541,0xA9E5}, // ╁
	{0x2542,0xA9E6}, // ╂
	{0x2543,0xA9E7}, // ╃
	{0x2544,0xA9E8}, // ╄
	{0x2545,0xA9E9}, // ╅
	{0x2546,0xA9EA}, // ╆
	{0x2547,0xA9EB}, // ╇
	{0x2548,0xA9EC}, // ╈
	{0x2549,0xA9ED}, // ╉
	{0x254A,0xA9EE}, // ╊
	{0x254B,0xA9EF}, // ╋
	{0x25A0,0xA1F6}, // ■
	{0x25A1,0xA1F5}, // □
	{0x25B2,0xA1F8}, // ▲
	{0x25B3,0xA1F7}, // △
	{0x25C6,0xA1F4}, // ◆
	{0x25C7,0xA1F3}, // ◇
	{0x25CB,0xA1F0}, // ○
	{0x25CE,0xA1F2}, // ◎
	{0x25CF,0xA1F1}, // ●
	{0x2605,0xA1EF}, // ★
	{0x2606,0xA1EE}, // ☆
	{0x2640,0xA1E2}, // ♀
	{0x2642,0xA1E1}, // ♂
	{0x3000,0xA1A1}, // 　
	{0x3001,0xA1A2}, // 、
	{0x3002,0xA1A3}, // 。
	{0x3003,0xA1A8}, // 〃
	{0x3005,0xA1A9}, // 々
	{0x3008,0xA1B4}, // 〈
	{0x3009,0xA1B5}, // 〉
	{0x300A,0xA1B6}, // 《
	{0x300B,0xA1B7}, // 》
	{0x300C,0xA1B8}, // 「
	{0x300D,0xA1B9}, // 」
	{0x300E,0xA1BA}, // 『
	{0x300F,0xA1BB}, // 』
	{0x3010,0xA1BE}, // 【
	{0x3011,0xA1BF}, // 】
	{0x3013,0xA1FE}, // 〓
	{0x3014,0xA1B2}, // 〔
	{0x3015,0xA1B3}, // 〕
	{0x3016,0xA1BC}, // 〖
	{0x3017,0xA1BD}, // 〗
	{0x3041,0xA4A1}, // ぁ
	{0x3042,0xA4A2}, // あ
	{0x3043,0xA4A3}, // ぃ
	{0x3044,0xA4A4}, // い
	{0x3045,0xA4A5}, // ぅ
	{0x3046,0xA4A6}, // う
	{0x3047,0xA4A7}, // ぇ
	{0x3048,0xA4A8}, // え
	{0x3049,0xA4A9}, // ぉ
	{0x304A,0xA4AA}, // お
	{0x304B,0xA4AB}, // か
	{0x304C,0xA4AC}, // が
	{0x304D,0xA4AD}, // き
	{0x304E,0xA4AE}, // ぎ
	{0x304F,0xA4AF}, // く
	{0x3050,0xA4B0}, // ぐ
	{0x3051,0xA4B1}, // け
	{0x3052,0xA4B2}, // げ
	{0x3053,0xA4B3}, // こ
	{0x3054,0xA4B4}, // ご
	{0x3055,0xA4B5}, // さ
	{0x3056,0xA4B6}, // ざ
	{0x3057,0xA4B7}, // し
	{0x3058,0xA4B8}, // じ
	{0x3059,0xA4B9}, // す
	{0x305A,0xA4BA}, // ず
	{0x305B,0xA4BB}, // せ
	{0x305C,0xA4BC}, // ぜ
	{0x305D,0xA4BD}, // そ
	{0x305E,0xA4BE}, // ぞ
	{0x305F,0xA4BF}, // た
	{0x3060,0xA4C0}, // だ
	{0x3061,0xA4C1}, // ち
	{0x3062,0xA4C2}, // ぢ
	{0x3063,0xA4C3}, // っ
	{0x3064,0xA4C4}, // つ
	{0x3065,0xA4C5}, // づ
	{0x3066,0xA4C6}, // て
	{0x3067,0xA4C7}, // で
	{0x3068,0xA4C8}, // と
	{0x3069,0xA4C9}, // ど
	{0x306A,0xA4CA}, // な
	{0x306B,0xA4CB}, // に
	{0x306C,0xA4CC}, // ぬ
	{0x306D,0xA4CD}, // ね
	{0x306E,0xA4CE}, // の
	{0x306F,0xA4CF}, // は
	{0x3070,0xA4D0}, // ば
	{0x3071,0xA4D1}, // ぱ
	{0x3072,0xA4D2}, // ひ
	{0x3073,0xA4D3}, // び
	{0x3074,0xA4D4}, // ぴ
	{0x3075,0xA4D5}, // ふ
	{0x3076,0xA4D6}, // ぶ
	{0x3077,0xA4D7}, // ぷ
	{0x3078,0xA4D8}, // へ
	{0x3079,0xA4D9}, // べ
	{0x307A,0xA4DA}, // ぺ
	{0x307B,0xA4DB}, // ほ
	{0x307C,0xA4DC}, // ぼ
	{0x307D,0xA4DD}, // ぽ
	{0x307E,0xA4DE}, // ま
	{0x307F,0xA4DF}, // み
	{0x3080,0xA4E0}, // む
	{0x3081,0xA4E1}, // め
	{0x3082,0xA4E2}, // も
	{0x3083,0xA4E3}, // ゃ
	{0x3084,0xA4E4}, // や
	{0x3085,0xA4E5}, // ゅ
	{0x3086,0xA4E6}, // ゆ
	{0x3087,0xA4E7}, // ょ
	{0x3088,0xA4E8}, // よ
	{0x3089,0xA4E9}, // ら
	{0x308A,0xA4EA}, // り
	{0x308B,0xA4EB}, // る
	{0x308C,0xA4EC}, // れ
	{0x308D,0xA4ED}, // ろ
	{0x308E,0xA4EE}, // ゎ
	{0x308F,0xA4EF}, // わ
	{0x3090,0xA4F0}, // ゐ
	{0x3091,0xA4F1}, // ゑ
	{0x3092,0xA4F2}, // を
	{0x3093,0xA4F3}, // ん
	{0x30A1,0xA5A1}, // ァ
	{0x30A2,0xA5A2}, // ア
	{0x30A3,0xA5A3}, // ィ
	{0x30A4,0xA5A4}, // イ
	{0x30A5,0xA5A5}, // ゥ
	{0x30A6,0xA5A6}, // ウ
	{0x30A7,0xA5A7}, // ェ
	{0x30A8,0xA5A8}, // エ
	{0x30A9,0xA5A9}, // ォ
	{0x30AA,0xA5AA}, // オ
	{0x30AB,0xA5AB}, // カ
	{0x30AC,0xA5AC}, // ガ
	{0x30AD,0xA5AD}, // キ
	{0x30AE,0xA5AE}, // ギ
	{0x30AF,0xA5AF}, // ク
	{0x30B0,0xA5B0}, // グ
	{0x30B1,0xA5B1}, // ケ
	{0x30B2,0xA5B2}, // ゲ
	{0x30B3,0xA5B3}, // コ
	{0x30B4,0xA5B4}, // ゴ
	{0x30B5,0xA5B5}, // サ
	{0x30B6,0xA5B6}, // ザ
	{0x30B7,0xA5B7}, // シ
	{0x30B8,0xA5B8}, // ジ
	{0x30B9,0xA5B9}, // ス
	{0x30BA,0xA5BA}, // ズ
	{0x30BB,0xA5BB}, // セ
	{0x30BC,0xA5BC}, // ゼ
	{0x30BD,0xA5BD}, // ソ
	{0x30BE,0xA5BE}, // ゾ
	{0x30BF,0xA5BF}, // タ
	{0x30C0,0xA5C0}, // ダ
	{0x30C1,0xA5C1}, // チ
	{0x30C2,0xA5C2}, // ヂ
	{0x30C3,0xA5C3}, // ッ
	{0x30C4,0xA5C4}, // ツ
	{0x30C5,0xA5C5}, // ヅ
	{0x30C6,0xA5C6}, // テ
	{0x30C7,0xA5C7}, // デ
	{0x30C8,0xA5C8}, // ト
	{0x30C9,0xA5C9}, // ド
	{0x30CA,0xA5CA}, // ナ
	{0x30CB,0xA5CB}, // ニ
	{0x30CC,0xA5CC}, // ヌ
	{0x30CD,0xA5CD}, // ネ
	{0x30CE,0xA5CE}, // ノ
	{0x30CF,0xA5CF}, // ハ
	{0x30D0,0xA5D0}, // バ
	{0x30D1,0xA5D1}, // パ
	{0x30D2,0xA5D2}, // ヒ
	{0x30D3,0xA5D3}, // ビ
	{0x30D4,0xA5D4}, // ピ
	{0x30D5,0xA5D5}, // フ
	{0x30D6,0xA5D6}, // ブ
	{0x30D7,0xA5D7}, // プ
	{0x30D8,0xA5D8}, // ヘ
	{0x30D9,0xA5D9}, // ベ
	{0x30DA,0xA5DA}, // ペ
	{0x30DB,0xA5DB}, // ホ
	{0x30DC,0xA5DC}, // ボ
	{0x30DD,0xA5DD}, // ポ
	{0x30DE,0xA5DE}, // マ
	{0x30DF,0xA5DF}, // ミ
	{0x30E0,0xA5E0}, // ム
	{0x30E1,0xA5E1}, // メ
	{0x30E2,0xA5E2}, // モ
	{0x30E3,0xA5E3}, // ャ
	{0x30E4,0xA5E4}, // ヤ
	{0x30E5,0xA5E5}, // ュ
	{0x30E6,0xA5E6}, // ユ
	{0x30E7,0xA5E7}, // ョ
	{0x30E8,0xA5E8}, // ヨ
	{0x30E9,0xA5E9}, // ラ
	{0x30EA,0xA5EA}, // リ
	{0x30EB,0xA5EB}, // ル
	{0x30EC,0xA5EC}, // レ
	{0x30ED,0xA5ED}, // ロ
	{0x30EE,0xA5EE}, // ヮ
	{0x30EF,0xA5EF}, // ワ
	{0x30F0,0xA5F0}, // ヰ
	{0x30F1,0xA5F1}, // ヱ
	{0x30F2,0xA5F2}, // ヲ
	{0x30F3,0xA5F3}, // ン
	{0x30F4,0xA5F4}, // ヴ
	{0x30F5,0xA5F5}, // ヵ
	{0x30F6,0xA5F6}, // ヶ
	{0x3105,0xA8C5}, // ㄅ
	{0x3106,0xA8C6}, // ㄆ
	{0x3107,0xA8C7}, // ㄇ
	{0x3108,0xA8C8}, // ㄈ
	{0x3109,0xA8C9}, // ㄉ
	{0x310A,0xA8CA}, // ㄊ
	{0x310B,0xA8CB}, // ㄋ
	{0x310C,0xA8CC}, // ㄌ
	{0x310D,0xA8CD}, // ㄍ
	{0x310E,0xA8CE}, // ㄎ
	{0x310F,0xA8CF}, // ㄏ
	{0x3110,0xA8D0}, // ㄐ
	{0x3111,0xA8D1}, // ㄑ
	{0x3112,0xA8D2}, // ㄒ
	{0x3113,0xA8D3}, // ㄓ
	{0x3114,0xA8D4}, // ㄔ
	{0x3115,0xA8D5}, // ㄕ
	{0x3116,0xA8D6}, // ㄖ
	{0x3117,0xA8D7}, // ㄗ
	{0x3118,0xA8D8}, // ㄘ
	{0x3119,0xA8D9}, // ㄙ
	{0x311A,0xA8DA}, // ㄚ
	{0x311B,0xA8DB}, // ㄛ
	{0x311C,0xA8DC}, // ㄜ
	{0x311D,0xA8DD}, // ㄝ
	{0x311E,0xA8DE}, // ㄞ
	{0x311F,0xA8DF}, // ㄟ
	{0x3120,0xA8E0}, // ㄠ
	{0x3121,0xA8E1}, // ㄡ
	{0x3122,0xA8E2}, // ㄢ
	{0x3123,0xA8E3}, // ㄣ
	{0x3124,0xA8E4}, // ㄤ
	{0x3125,0xA8E5}, // ㄥ
	{0x3126,0xA8E6}, // ㄦ
	{0x3127,0xA8E7}, // ㄧ
	{0x3128,0xA8E8}, // ㄨ
	{0x3129,0xA8E9}, // ㄩ
	{0x3220,0xA2E5}, // ㈠
	{0x3221,0xA2E6}, // ㈡
	{0x3222,0xA2E7}, // ㈢
	{0x3223,0xA2E8}, // ㈣
	{0x3224,0xA2E9}, // ㈤
	{0x3225,0xA2EA}, // ㈥
	{0x3226,0xA2EB}, // ㈦
	{0x3227,0xA2EC}, // ㈧
	{0x3228,0xA2ED}, // ㈨
	{0x3229,0xA2EE}, // ㈩
    {0x4E00,0xD2BB}, /* 一: GB2312 Code: 0xD2BB ==> Row:50 Col:27 */
    {0x4E01,0xB6A1}, /* 丁: GB2312 Code: 0xB6A1 ==> Row:22 Col:01 */
    {0x4E03,0xC6DF}, /* 七: GB2312 Code: 0xC6DF ==> Row:38 Col:63 */
    {0x4E07,0xCDF2}, /* 万: GB2312 Code: 0xCDF2 ==> Row:45 Col:82 */
    {0x4E08,0xD5C9}, /* 丈: GB2312 Code: 0xD5C9 ==> Row:53 Col:41 */
    {0x4E09,0xC8FD}, /* 三: GB2312 Code: 0xC8FD ==> Row:40 Col:93 */
    {0x4E0A,0xC9CF}, /* 上: GB2312 Code: 0xC9CF ==> Row:41 Col:47 */
    {0x4E0B,0xCFC2}, /* 下: GB2312 Code: 0xCFC2 ==> Row:47 Col:34 */
    {0x4E0C,0xD8A2}, /* 丌: GB2312 Code: 0xD8A2 ==> Row:56 Col:02 */
    {0x4E0D,0xB2BB}, /* 不: GB2312 Code: 0xB2BB ==> Row:18 Col:27 */
    {0x4E0E,0xD3EB}, /* 与: GB2312 Code: 0xD3EB ==> Row:51 Col:75 */
    {0x4E10,0xD8A4}, /* 丐: GB2312 Code: 0xD8A4 ==> Row:56 Col:04 */
    {0x4E11,0xB3F3}, /* 丑: GB2312 Code: 0xB3F3 ==> Row:19 Col:83 */
    {0x4E13,0xD7A8}, /* 专: GB2312 Code: 0xD7A8 ==> Row:55 Col:08 */
    {0x4E14,0xC7D2}, /* 且: GB2312 Code: 0xC7D2 ==> Row:39 Col:50 */
    {0x4E15,0xD8A7}, /* 丕: GB2312 Code: 0xD8A7 ==> Row:56 Col:07 */
    {0x4E16,0xCAC0}, /* 世: GB2312 Code: 0xCAC0 ==> Row:42 Col:32 */
    {0x4E18,0xC7F0}, /* 丘: GB2312 Code: 0xC7F0 ==> Row:39 Col:80 */
    {0x4E19,0xB1FB}, /* 丙: GB2312 Code: 0xB1FB ==> Row:17 Col:91 */
    {0x4E1A,0xD2B5}, /* 业: GB2312 Code: 0xD2B5 ==> Row:50 Col:21 */
    {0x4E1B,0xB4D4}, /* 丛: GB2312 Code: 0xB4D4 ==> Row:20 Col:52 */
    {0x4E1C,0xB6AB}, /* 东: GB2312 Code: 0xB6AB ==> Row:22 Col:11 */
    {0x4E1D,0xCBBF}, /* 丝: GB2312 Code: 0xCBBF ==> Row:43 Col:31 */
    {0x4E1E,0xD8A9}, /* 丞: GB2312 Code: 0xD8A9 ==> Row:56 Col:09 */
    {0x4E22,0xB6AA}, /* 丢: GB2312 Code: 0xB6AA ==> Row:22 Col:10 */
    {0x4E24,0xC1BD}, /* 两: GB2312 Code: 0xC1BD ==> Row:33 Col:29 */
    {0x4E25,0xD1CF}, /* 严: GB2312 Code: 0xD1CF ==> Row:49 Col:47 */
    {0x4E27,0xC9A5}, /* 丧: GB2312 Code: 0xC9A5 ==> Row:41 Col:05 */
    {0x4E28,0xD8AD}, /* 丨: GB2312 Code: 0xD8AD ==> Row:56 Col:13 */
    {0x4E2A,0xB8F6}, /* 个: GB2312 Code: 0xB8F6 ==> Row:24 Col:86 */
    {0x4E2B,0xD1BE}, /* 丫: GB2312 Code: 0xD1BE ==> Row:49 Col:30 */
    {0x4E2C,0xE3DC}, /* 丬: GB2312 Code: 0xE3DC ==> Row:67 Col:60 */
    {0x4E2D,0xD6D0}, /* 中: GB2312 Code: 0xD6D0 ==> Row:54 Col:48 */
    {0x4E30,0xB7E1}, /* 丰: GB2312 Code: 0xB7E1 ==> Row:23 Col:65 */
    {0x4E32,0xB4AE}, /* 串: GB2312 Code: 0xB4AE ==> Row:20 Col:14 */
    {0x4E34,0xC1D9}, /* 临: GB2312 Code: 0xC1D9 ==> Row:33 Col:57 */
    {0x4E36,0xD8BC}, /* 丶: GB2312 Code: 0xD8BC ==> Row:56 Col:28 */
    {0x4E38,0xCDE8}, /* 丸: GB2312 Code: 0xCDE8 ==> Row:45 Col:72 */
    {0x4E39,0xB5A4}, /* 丹: GB2312 Code: 0xB5A4 ==> Row:21 Col:04 */
    {0x4E3A,0xCEAA}, /* 为: GB2312 Code: 0xCEAA ==> Row:46 Col:10 */
    {0x4E3B,0xD6F7}, /* 主: GB2312 Code: 0xD6F7 ==> Row:54 Col:87 */
    {0x4E3D,0xC0F6}, /* 丽: GB2312 Code: 0xC0F6 ==> Row:32 Col:86 */
    {0x4E3E,0xBED9}, /* 举: GB2312 Code: 0xBED9 ==> Row:30 Col:57 */
    {0x4E3F,0xD8AF}, /* 丿: GB2312 Code: 0xD8AF ==> Row:56 Col:15 */
    {0x4E43,0xC4CB}, /* 乃: GB2312 Code: 0xC4CB ==> Row:36 Col:43 */
    {0x4E45,0xBEC3}, /* 久: GB2312 Code: 0xBEC3 ==> Row:30 Col:35 */
    {0x4E47,0xD8B1}, /* 乇: GB2312 Code: 0xD8B1 ==> Row:56 Col:17 */
    {0x4E48,0xC3B4}, /* 么: GB2312 Code: 0xC3B4 ==> Row:35 Col:20 */
    {0x4E49,0xD2E5}, /* 义: GB2312 Code: 0xD2E5 ==> Row:50 Col:69 */
    {0x4E4B,0xD6AE}, /* 之: GB2312 Code: 0xD6AE ==> Row:54 Col:14 */
    {0x4E4C,0xCEDA}, /* 乌: GB2312 Code: 0xCEDA ==> Row:46 Col:58 */
    {0x4E4D,0xD5A7}, /* 乍: GB2312 Code: 0xD5A7 ==> Row:53 Col:07 */
    {0x4E4E,0xBAF5}, /* 乎: GB2312 Code: 0xBAF5 ==> Row:26 Col:85 */
    {0x4E4F,0xB7A6}, /* 乏: GB2312 Code: 0xB7A6 ==> Row:23 Col:06 */
    {0x4E50,0xC0D6}, /* 乐: GB2312 Code: 0xC0D6 ==> Row:32 Col:54 */
    {0x4E52,0xC6B9}, /* 乒: GB2312 Code: 0xC6B9 ==> Row:38 Col:25 */
    {0x4E53,0xC5D2}, /* 乓: GB2312 Code: 0xC5D2 ==> Row:37 Col:50 */
    {0x4E54,0xC7C7}, /* 乔: GB2312 Code: 0xC7C7 ==> Row:39 Col:39 */
    {0x4E56,0xB9D4}, /* 乖: GB2312 Code: 0xB9D4 ==> Row:25 Col:52 */
    {0x4E58,0xB3CB}, /* 乘: GB2312 Code: 0xB3CB ==> Row:19 Col:43 */
    {0x4E59,0xD2D2}, /* 乙: GB2312 Code: 0xD2D2 ==> Row:50 Col:50 */
    {0x4E5C,0xD8BF}, /* 乜: GB2312 Code: 0xD8BF ==> Row:56 Col:31 */
    {0x4E5D,0xBEC5}, /* 九: GB2312 Code: 0xBEC5 ==> Row:30 Col:37 */
    {0x4E5E,0xC6F2}, /* 乞: GB2312 Code: 0xC6F2 ==> Row:38 Col:82 */
    {0x4E5F,0xD2B2}, /* 也: GB2312 Code: 0xD2B2 ==> Row:50 Col:18 */
    {0x4E60,0xCFB0}, /* 习: GB2312 Code: 0xCFB0 ==> Row:47 Col:16 */
    {0x4E61,0xCFE7}, /* 乡: GB2312 Code: 0xCFE7 ==> Row:47 Col:71 */
    {0x4E66,0xCAE9}, /* 书: GB2312 Code: 0xCAE9 ==> Row:42 Col:73 */
    {0x4E69,0xD8C0}, /* 乩: GB2312 Code: 0xD8C0 ==> Row:56 Col:32 */
    {0x4E70,0xC2F2}, /* 买: GB2312 Code: 0xC2F2 ==> Row:34 Col:82 */
    {0x4E71,0xC2D2}, /* 乱: GB2312 Code: 0xC2D2 ==> Row:34 Col:50 */
    {0x4E73,0xC8E9}, /* 乳: GB2312 Code: 0xC8E9 ==> Row:40 Col:73 */
    {0x4E7E,0xC7AC}, /* 乾: GB2312 Code: 0xC7AC ==> Row:39 Col:12 */
    {0x4E86,0xC1CB}, /* 了: GB2312 Code: 0xC1CB ==> Row:33 Col:43 */
    {0x4E88,0xD3E8}, /* 予: GB2312 Code: 0xD3E8 ==> Row:51 Col:72 */
    {0x4E89,0xD5F9}, /* 争: GB2312 Code: 0xD5F9 ==> Row:53 Col:89 */
    {0x4E8B,0xCAC2}, /* 事: GB2312 Code: 0xCAC2 ==> Row:42 Col:34 */
    {0x4E8C,0xB6FE}, /* 二: GB2312 Code: 0xB6FE ==> Row:22 Col:94 */
    {0x4E8D,0xD8A1}, /* 亍: GB2312 Code: 0xD8A1 ==> Row:56 Col:01 */
    {0x4E8E,0xD3DA}, /* 于: GB2312 Code: 0xD3DA ==> Row:51 Col:58 */
    {0x4E8F,0xBFF7}, /* 亏: GB2312 Code: 0xBFF7 ==> Row:31 Col:87 */
    {0x4E91,0xD4C6}, /* 云: GB2312 Code: 0xD4C6 ==> Row:52 Col:38 */
    {0x4E92,0xBBA5}, /* 互: GB2312 Code: 0xBBA5 ==> Row:27 Col:05 */
    {0x4E93,0xD8C1}, /* 亓: GB2312 Code: 0xD8C1 ==> Row:56 Col:33 */
    {0x4E94,0xCEE5}, /* 五: GB2312 Code: 0xCEE5 ==> Row:46 Col:69 */
    {0x4E95,0xBEAE}, /* 井: GB2312 Code: 0xBEAE ==> Row:30 Col:14 */
    {0x4E98,0xD8A8}, /* 亘: GB2312 Code: 0xD8A8 ==> Row:56 Col:08 */
    {0x4E9A,0xD1C7}, /* 亚: GB2312 Code: 0xD1C7 ==> Row:49 Col:39 */
    {0x4E9B,0xD0A9}, /* 些: GB2312 Code: 0xD0A9 ==> Row:48 Col:09 */
    {0x4E9F,0xD8BD}, /* 亟: GB2312 Code: 0xD8BD ==> Row:56 Col:29 */
    {0x4EA0,0xD9EF}, /* 亠: GB2312 Code: 0xD9EF ==> Row:57 Col:79 */
    {0x4EA1,0xCDF6}, /* 亡: GB2312 Code: 0xCDF6 ==> Row:45 Col:86 */
    {0x4EA2,0xBFBA}, /* 亢: GB2312 Code: 0xBFBA ==> Row:31 Col:26 */
    {0x4EA4,0xBDBB}, /* 交: GB2312 Code: 0xBDBB ==> Row:29 Col:27 */
    {0x4EA5,0xBAA5}, /* 亥: GB2312 Code: 0xBAA5 ==> Row:26 Col:05 */
    {0x4EA6,0xD2E0}, /* 亦: GB2312 Code: 0xD2E0 ==> Row:50 Col:64 */
    {0x4EA7,0xB2FA}, /* 产: GB2312 Code: 0xB2FA ==> Row:18 Col:90 */
    {0x4EA8,0xBAE0}, /* 亨: GB2312 Code: 0xBAE0 ==> Row:26 Col:64 */
    {0x4EA9,0xC4B6}, /* 亩: GB2312 Code: 0xC4B6 ==> Row:36 Col:22 */
    {0x4EAB,0xCFED}, /* 享: GB2312 Code: 0xCFED ==> Row:47 Col:77 */
    {0x4EAC,0xBEA9}, /* 京: GB2312 Code: 0xBEA9 ==> Row:30 Col:09 */
    {0x4EAD,0xCDA4}, /* 亭: GB2312 Code: 0xCDA4 ==> Row:45 Col:04 */
    {0x4EAE,0xC1C1}, /* 亮: GB2312 Code: 0xC1C1 ==> Row:33 Col:33 */
    {0x4EB2,0xC7D7}, /* 亲: GB2312 Code: 0xC7D7 ==> Row:39 Col:55 */
    {0x4EB3,0xD9F1}, /* 亳: GB2312 Code: 0xD9F1 ==> Row:57 Col:81 */
    {0x4EB5,0xD9F4}, /* 亵: GB2312 Code: 0xD9F4 ==> Row:57 Col:84 */
    {0x4EBA,0xC8CB}, /* 人: GB2312 Code: 0xC8CB ==> Row:40 Col:43 */
    {0x4EBB,0xD8E9}, /* 亻: GB2312 Code: 0xD8E9 ==> Row:56 Col:73 */
    {0x4EBF,0xD2DA}, /* 亿: GB2312 Code: 0xD2DA ==> Row:50 Col:58 */
    {0x4EC0,0xCAB2}, /* 什: GB2312 Code: 0xCAB2 ==> Row:42 Col:18 */
    {0x4EC1,0xC8CA}, /* 仁: GB2312 Code: 0xC8CA ==> Row:40 Col:42 */
    {0x4EC2,0xD8EC}, /* 仂: GB2312 Code: 0xD8EC ==> Row:56 Col:76 */
    {0x4EC3,0xD8EA}, /* 仃: GB2312 Code: 0xD8EA ==> Row:56 Col:74 */
    {0x4EC4,0xD8C6}, /* 仄: GB2312 Code: 0xD8C6 ==> Row:56 Col:38 */
    {0x4EC5,0xBDF6}, /* 仅: GB2312 Code: 0xBDF6 ==> Row:29 Col:86 */
    {0x4EC6,0xC6CD}, /* 仆: GB2312 Code: 0xC6CD ==> Row:38 Col:45 */
    {0x4EC7,0xB3F0}, /* 仇: GB2312 Code: 0xB3F0 ==> Row:19 Col:80 */
    {0x4EC9,0xD8EB}, /* 仉: GB2312 Code: 0xD8EB ==> Row:56 Col:75 */
    {0x4ECA,0xBDF1}, /* 今: GB2312 Code: 0xBDF1 ==> Row:29 Col:81 */
    {0x4ECB,0xBDE9}, /* 介: GB2312 Code: 0xBDE9 ==> Row:29 Col:73 */
    {0x4ECD,0xC8D4}, /* 仍: GB2312 Code: 0xC8D4 ==> Row:40 Col:52 */
    {0x4ECE,0xB4D3}, /* 从: GB2312 Code: 0xB4D3 ==> Row:20 Col:51 */
    {0x4ED1,0xC2D8}, /* 仑: GB2312 Code: 0xC2D8 ==> Row:34 Col:56 */
    {0x4ED3,0xB2D6}, /* 仓: GB2312 Code: 0xB2D6 ==> Row:18 Col:54 */
    {0x4ED4,0xD7D0}, /* 仔: GB2312 Code: 0xD7D0 ==> Row:55 Col:48 */
    {0x4ED5,0xCACB}, /* 仕: GB2312 Code: 0xCACB ==> Row:42 Col:43 */
    {0x4ED6,0xCBFB}, /* 他: GB2312 Code: 0xCBFB ==> Row:43 Col:91 */
    {0x4ED7,0xD5CC}, /* 仗: GB2312 Code: 0xD5CC ==> Row:53 Col:44 */
    {0x4ED8,0xB8B6}, /* 付: GB2312 Code: 0xB8B6 ==> Row:24 Col:22 */
    {0x4ED9,0xCFC9}, /* 仙: GB2312 Code: 0xCFC9 ==> Row:47 Col:41 */
    {0x4EDD,0xD9DA}, /* 仝: GB2312 Code: 0xD9DA ==> Row:57 Col:58 */
    {0x4EDE,0xD8F0}, /* 仞: GB2312 Code: 0xD8F0 ==> Row:56 Col:80 */
    {0x4EDF,0xC7AA}, /* 仟: GB2312 Code: 0xC7AA ==> Row:39 Col:10 */
    {0x4EE1,0xD8EE}, /* 仡: GB2312 Code: 0xD8EE ==> Row:56 Col:78 */
    {0x4EE3,0xB4FA}, /* 代: GB2312 Code: 0xB4FA ==> Row:20 Col:90 */
    {0x4EE4,0xC1EE}, /* 令: GB2312 Code: 0xC1EE ==> Row:33 Col:78 */
    {0x4EE5,0xD2D4}, /* 以: GB2312 Code: 0xD2D4 ==> Row:50 Col:52 */
    {0x4EE8,0xD8ED}, /* 仨: GB2312 Code: 0xD8ED ==> Row:56 Col:77 */
    {0x4EEA,0xD2C7}, /* 仪: GB2312 Code: 0xD2C7 ==> Row:50 Col:39 */
    {0x4EEB,0xD8EF}, /* 仫: GB2312 Code: 0xD8EF ==> Row:56 Col:79 */
    {0x4EEC,0xC3C7}, /* 们: GB2312 Code: 0xC3C7 ==> Row:35 Col:39 */
    {0x4EF0,0xD1F6}, /* 仰: GB2312 Code: 0xD1F6 ==> Row:49 Col:86 */
    {0x4EF2,0xD6D9}, /* 仲: GB2312 Code: 0xD6D9 ==> Row:54 Col:57 */
    {0x4EF3,0xD8F2}, /* 仳: GB2312 Code: 0xD8F2 ==> Row:56 Col:82 */
    {0x4EF5,0xD8F5}, /* 仵: GB2312 Code: 0xD8F5 ==> Row:56 Col:85 */
    {0x4EF6,0xBCFE}, /* 件: GB2312 Code: 0xBCFE ==> Row:28 Col:94 */
    {0x4EF7,0xBCDB}, /* 价: GB2312 Code: 0xBCDB ==> Row:28 Col:59 */
    {0x4EFB,0xC8CE}, /* 任: GB2312 Code: 0xC8CE ==> Row:40 Col:46 */
    {0x4EFD,0xB7DD}, /* 份: GB2312 Code: 0xB7DD ==> Row:23 Col:61 */
    {0x4EFF,0xB7C2}, /* 仿: GB2312 Code: 0xB7C2 ==> Row:23 Col:34 */
    {0x4F01,0xC6F3}, /* 企: GB2312 Code: 0xC6F3 ==> Row:38 Col:83 */
    {0x4F09,0xD8F8}, /* 伉: GB2312 Code: 0xD8F8 ==> Row:56 Col:88 */
    {0x4F0A,0xD2C1}, /* 伊: GB2312 Code: 0xD2C1 ==> Row:50 Col:33 */
    {0x4F0D,0xCEE9}, /* 伍: GB2312 Code: 0xCEE9 ==> Row:46 Col:73 */
    {0x4F0E,0xBCBF}, /* 伎: GB2312 Code: 0xBCBF ==> Row:28 Col:31 */
    {0x4F0F,0xB7FC}, /* 伏: GB2312 Code: 0xB7FC ==> Row:23 Col:92 */
    {0x4F10,0xB7A5}, /* 伐: GB2312 Code: 0xB7A5 ==> Row:23 Col:05 */
    {0x4F11,0xD0DD}, /* 休: GB2312 Code: 0xD0DD ==> Row:48 Col:61 */
    {0x4F17,0xD6DA}, /* 众: GB2312 Code: 0xD6DA ==> Row:54 Col:58 */
    {0x4F18,0xD3C5}, /* 优: GB2312 Code: 0xD3C5 ==> Row:51 Col:37 */
    {0x4F19,0xBBEF}, /* 伙: GB2312 Code: 0xBBEF ==> Row:27 Col:79 */
    {0x4F1A,0xBBE1}, /* 会: GB2312 Code: 0xBBE1 ==> Row:27 Col:65 */
    {0x4F1B,0xD8F1}, /* 伛: GB2312 Code: 0xD8F1 ==> Row:56 Col:81 */
    {0x4F1E,0xC9A1}, /* 伞: GB2312 Code: 0xC9A1 ==> Row:41 Col:01 */
    {0x4F1F,0xCEB0}, /* 伟: GB2312 Code: 0xCEB0 ==> Row:46 Col:16 */
    {0x4F20,0xB4AB}, /* 传: GB2312 Code: 0xB4AB ==> Row:20 Col:11 */
    {0x4F22,0xD8F3}, /* 伢: GB2312 Code: 0xD8F3 ==> Row:56 Col:83 */
    {0x4F24,0xC9CB}, /* 伤: GB2312 Code: 0xC9CB ==> Row:41 Col:43 */
    {0x4F25,0xD8F6}, /* 伥: GB2312 Code: 0xD8F6 ==> Row:56 Col:86 */
    {0x4F26,0xC2D7}, /* 伦: GB2312 Code: 0xC2D7 ==> Row:34 Col:55 */
    {0x4F27,0xD8F7}, /* 伧: GB2312 Code: 0xD8F7 ==> Row:56 Col:87 */
    {0x4F2A,0xCEB1}, /* 伪: GB2312 Code: 0xCEB1 ==> Row:46 Col:17 */
    {0x4F2B,0xD8F9}, /* 伫: GB2312 Code: 0xD8F9 ==> Row:56 Col:89 */
    {0x4F2F,0xB2AE}, /* 伯: GB2312 Code: 0xB2AE ==> Row:18 Col:14 */
    {0x4F30,0xB9C0}, /* 估: GB2312 Code: 0xB9C0 ==> Row:25 Col:32 */
    {0x4F32,0xD9A3}, /* 伲: GB2312 Code: 0xD9A3 ==> Row:57 Col:03 */
    {0x4F34,0xB0E9}, /* 伴: GB2312 Code: 0xB0E9 ==> Row:16 Col:73 */
    {0x4F36,0xC1E6}, /* 伶: GB2312 Code: 0xC1E6 ==> Row:33 Col:70 */
    {0x4F38,0xC9EC}, /* 伸: GB2312 Code: 0xC9EC ==> Row:41 Col:76 */
    {0x4F3A,0xCBC5}, /* 伺: GB2312 Code: 0xCBC5 ==> Row:43 Col:37 */
    {0x4F3C,0xCBC6}, /* 似: GB2312 Code: 0xCBC6 ==> Row:43 Col:38 */
    {0x4F3D,0xD9A4}, /* 伽: GB2312 Code: 0xD9A4 ==> Row:57 Col:04 */
    {0x4F43,0xB5E8}, /* 佃: GB2312 Code: 0xB5E8 ==> Row:21 Col:72 */
    {0x4F46,0xB5AB}, /* 但: GB2312 Code: 0xB5AB ==> Row:21 Col:11 */
    {0x4F4D,0xCEBB}, /* 位: GB2312 Code: 0xCEBB ==> Row:46 Col:27 */
    {0x4F4E,0xB5CD}, /* 低: GB2312 Code: 0xB5CD ==> Row:21 Col:45 */
    {0x4F4F,0xD7A1}, /* 住: GB2312 Code: 0xD7A1 ==> Row:55 Col:01 */
    {0x4F50,0xD7F4}, /* 佐: GB2312 Code: 0xD7F4 ==> Row:55 Col:84 */
    {0x4F51,0xD3D3}, /* 佑: GB2312 Code: 0xD3D3 ==> Row:51 Col:51 */
    {0x4F53,0xCCE5}, /* 体: GB2312 Code: 0xCCE5 ==> Row:44 Col:69 */
    {0x4F55,0xBACE}, /* 何: GB2312 Code: 0xBACE ==> Row:26 Col:46 */
    {0x4F57,0xD9A2}, /* 佗: GB2312 Code: 0xD9A2 ==> Row:57 Col:02 */
    {0x4F58,0xD9DC}, /* 佘: GB2312 Code: 0xD9DC ==> Row:57 Col:60 */
    {0x4F59,0xD3E0}, /* 余: GB2312 Code: 0xD3E0 ==> Row:51 Col:64 */
    {0x4F5A,0xD8FD}, /* 佚: GB2312 Code: 0xD8FD ==> Row:56 Col:93 */
    {0x4F5B,0xB7F0}, /* 佛: GB2312 Code: 0xB7F0 ==> Row:23 Col:80 */
    {0x4F5C,0xD7F7}, /* 作: GB2312 Code: 0xD7F7 ==> Row:55 Col:87 */
    {0x4F5D,0xD8FE}, /* 佝: GB2312 Code: 0xD8FE ==> Row:56 Col:94 */
    {0x4F5E,0xD8FA}, /* 佞: GB2312 Code: 0xD8FA ==> Row:56 Col:90 */
    {0x4F5F,0xD9A1}, /* 佟: GB2312 Code: 0xD9A1 ==> Row:57 Col:01 */
    {0x4F60,0xC4E3}, /* 你: GB2312 Code: 0xC4E3 ==> Row:36 Col:67 */
    {0x4F63,0xD3B6}, /* 佣: GB2312 Code: 0xD3B6 ==> Row:51 Col:22 */
    {0x4F64,0xD8F4}, /* 佤: GB2312 Code: 0xD8F4 ==> Row:56 Col:84 */
    {0x4F65,0xD9DD}, /* 佥: GB2312 Code: 0xD9DD ==> Row:57 Col:61 */
    {0x4F67,0xD8FB}, /* 佧: GB2312 Code: 0xD8FB ==> Row:56 Col:91 */
    {0x4F69,0xC5E5}, /* 佩: GB2312 Code: 0xC5E5 ==> Row:37 Col:69 */
    {0x4F6C,0xC0D0}, /* 佬: GB2312 Code: 0xC0D0 ==> Row:32 Col:48 */
    {0x4F6F,0xD1F0}, /* 佯: GB2312 Code: 0xD1F0 ==> Row:49 Col:80 */
    {0x4F70,0xB0DB}, /* 佰: GB2312 Code: 0xB0DB ==> Row:16 Col:59 */
    {0x4F73,0xBCD1}, /* 佳: GB2312 Code: 0xBCD1 ==> Row:28 Col:49 */
    {0x4F74,0xD9A6}, /* 佴: GB2312 Code: 0xD9A6 ==> Row:57 Col:06 */
    {0x4F76,0xD9A5}, /* 佶: GB2312 Code: 0xD9A5 ==> Row:57 Col:05 */
    {0x4F7B,0xD9AC}, /* 佻: GB2312 Code: 0xD9AC ==> Row:57 Col:12 */
    {0x4F7C,0xD9AE}, /* 佼: GB2312 Code: 0xD9AE ==> Row:57 Col:14 */
    {0x4F7E,0xD9AB}, /* 佾: GB2312 Code: 0xD9AB ==> Row:57 Col:11 */
    {0x4F7F,0xCAB9}, /* 使: GB2312 Code: 0xCAB9 ==> Row:42 Col:25 */
    {0x4F83,0xD9A9}, /* 侃: GB2312 Code: 0xD9A9 ==> Row:57 Col:09 */
    {0x4F84,0xD6B6}, /* 侄: GB2312 Code: 0xD6B6 ==> Row:54 Col:22 */
    {0x4F88,0xB3DE}, /* 侈: GB2312 Code: 0xB3DE ==> Row:19 Col:62 */
    {0x4F89,0xD9A8}, /* 侉: GB2312 Code: 0xD9A8 ==> Row:57 Col:08 */
    {0x4F8B,0xC0FD}, /* 例: GB2312 Code: 0xC0FD ==> Row:32 Col:93 */
    {0x4F8D,0xCACC}, /* 侍: GB2312 Code: 0xCACC ==> Row:42 Col:44 */
    {0x4F8F,0xD9AA}, /* 侏: GB2312 Code: 0xD9AA ==> Row:57 Col:10 */
    {0x4F91,0xD9A7}, /* 侑: GB2312 Code: 0xD9A7 ==> Row:57 Col:07 */
    {0x4F94,0xD9B0}, /* 侔: GB2312 Code: 0xD9B0 ==> Row:57 Col:16 */
    {0x4F97,0xB6B1}, /* 侗: GB2312 Code: 0xB6B1 ==> Row:22 Col:17 */
    {0x4F9B,0xB9A9}, /* 供: GB2312 Code: 0xB9A9 ==> Row:25 Col:09 */
    {0x4F9D,0xD2C0}, /* 依: GB2312 Code: 0xD2C0 ==> Row:50 Col:32 */
    {0x4FA0,0xCFC0}, /* 侠: GB2312 Code: 0xCFC0 ==> Row:47 Col:32 */
    {0x4FA3,0xC2C2}, /* 侣: GB2312 Code: 0xC2C2 ==> Row:34 Col:34 */
    {0x4FA5,0xBDC4}, /* 侥: GB2312 Code: 0xBDC4 ==> Row:29 Col:36 */
    {0x4FA6,0xD5EC}, /* 侦: GB2312 Code: 0xD5EC ==> Row:53 Col:76 */
    {0x4FA7,0xB2E0}, /* 侧: GB2312 Code: 0xB2E0 ==> Row:18 Col:64 */
    {0x4FA8,0xC7C8}, /* 侨: GB2312 Code: 0xC7C8 ==> Row:39 Col:40 */
    {0x4FA9,0xBFEB}, /* 侩: GB2312 Code: 0xBFEB ==> Row:31 Col:75 */
    {0x4FAA,0xD9AD}, /* 侪: GB2312 Code: 0xD9AD ==> Row:57 Col:13 */
    {0x4FAC,0xD9AF}, /* 侬: GB2312 Code: 0xD9AF ==> Row:57 Col:15 */
    {0x4FAE,0xCEEA}, /* 侮: GB2312 Code: 0xCEEA ==> Row:46 Col:74 */
    {0x4FAF,0xBAEE}, /* 侯: GB2312 Code: 0xBAEE ==> Row:26 Col:78 */
    {0x4FB5,0xC7D6}, /* 侵: GB2312 Code: 0xC7D6 ==> Row:39 Col:54 */
    {0x4FBF,0xB1E3}, /* 便: GB2312 Code: 0xB1E3 ==> Row:17 Col:67 */
    {0x4FC3,0xB4D9}, /* 促: GB2312 Code: 0xB4D9 ==> Row:20 Col:57 */
    {0x4FC4,0xB6ED}, /* 俄: GB2312 Code: 0xB6ED ==> Row:22 Col:77 */
    {0x4FC5,0xD9B4}, /* 俅: GB2312 Code: 0xD9B4 ==> Row:57 Col:20 */
    {0x4FCA,0xBFA1}, /* 俊: GB2312 Code: 0xBFA1 ==> Row:31 Col:01 */
    {0x4FCE,0xD9DE}, /* 俎: GB2312 Code: 0xD9DE ==> Row:57 Col:62 */
    {0x4FCF,0xC7CE}, /* 俏: GB2312 Code: 0xC7CE ==> Row:39 Col:46 */
    {0x4FD0,0xC0FE}, /* 俐: GB2312 Code: 0xC0FE ==> Row:32 Col:94 */
    {0x4FD1,0xD9B8}, /* 俑: GB2312 Code: 0xD9B8 ==> Row:57 Col:24 */
    {0x4FD7,0xCBD7}, /* 俗: GB2312 Code: 0xCBD7 ==> Row:43 Col:55 */
    {0x4FD8,0xB7FD}, /* 俘: GB2312 Code: 0xB7FD ==> Row:23 Col:93 */
    {0x4FDA,0xD9B5}, /* 俚: GB2312 Code: 0xD9B5 ==> Row:57 Col:21 */
    {0x4FDC,0xD9B7}, /* 俜: GB2312 Code: 0xD9B7 ==> Row:57 Col:23 */
    {0x4FDD,0xB1A3}, /* 保: GB2312 Code: 0xB1A3 ==> Row:17 Col:03 */
    {0x4FDE,0xD3E1}, /* 俞: GB2312 Code: 0xD3E1 ==> Row:51 Col:65 */
    {0x4FDF,0xD9B9}, /* 俟: GB2312 Code: 0xD9B9 ==> Row:57 Col:25 */
    {0x4FE1,0xD0C5}, /* 信: GB2312 Code: 0xD0C5 ==> Row:48 Col:37 */
    {0x4FE3,0xD9B6}, /* 俣: GB2312 Code: 0xD9B6 ==> Row:57 Col:22 */
    {0x4FE6,0xD9B1}, /* 俦: GB2312 Code: 0xD9B1 ==> Row:57 Col:17 */
    {0x4FE8,0xD9B2}, /* 俨: GB2312 Code: 0xD9B2 ==> Row:57 Col:18 */
    {0x4FE9,0xC1A9}, /* 俩: GB2312 Code: 0xC1A9 ==> Row:33 Col:09 */
    {0x4FEA,0xD9B3}, /* 俪: GB2312 Code: 0xD9B3 ==> Row:57 Col:19 */
    {0x4FED,0xBCF3}, /* 俭: GB2312 Code: 0xBCF3 ==> Row:28 Col:83 */
    {0x4FEE,0xD0DE}, /* 修: GB2312 Code: 0xD0DE ==> Row:48 Col:62 */
    {0x4FEF,0xB8A9}, /* 俯: GB2312 Code: 0xB8A9 ==> Row:24 Col:09 */
    {0x4FF1,0xBEE3}, /* 俱: GB2312 Code: 0xBEE3 ==> Row:30 Col:67 */
    {0x4FF3,0xD9BD}, /* 俳: GB2312 Code: 0xD9BD ==> Row:57 Col:29 */
    {0x4FF8,0xD9BA}, /* 俸: GB2312 Code: 0xD9BA ==> Row:57 Col:26 */
    {0x4FFA,0xB0B3}, /* 俺: GB2312 Code: 0xB0B3 ==> Row:16 Col:19 */
    {0x4FFE,0xD9C2}, /* 俾: GB2312 Code: 0xD9C2 ==> Row:57 Col:34 */
    {0x500C,0xD9C4}, /* 倌: GB2312 Code: 0xD9C4 ==> Row:57 Col:36 */
    {0x500D,0xB1B6}, /* 倍: GB2312 Code: 0xB1B6 ==> Row:17 Col:22 */
    {0x500F,0xD9BF}, /* 倏: GB2312 Code: 0xD9BF ==> Row:57 Col:31 */
    {0x5012,0xB5B9}, /* 倒: GB2312 Code: 0xB5B9 ==> Row:21 Col:25 */
    {0x5014,0xBEF3}, /* 倔: GB2312 Code: 0xBEF3 ==> Row:30 Col:83 */
    {0x5018,0xCCC8}, /* 倘: GB2312 Code: 0xCCC8 ==> Row:44 Col:40 */
    {0x5019,0xBAF2}, /* 候: GB2312 Code: 0xBAF2 ==> Row:26 Col:82 */
    {0x501A,0xD2D0}, /* 倚: GB2312 Code: 0xD2D0 ==> Row:50 Col:48 */
    {0x501C,0xD9C3}, /* 倜: GB2312 Code: 0xD9C3 ==> Row:57 Col:35 */
    {0x501F,0xBDE8}, /* 借: GB2312 Code: 0xBDE8 ==> Row:29 Col:72 */
    {0x5021,0xB3AB}, /* 倡: GB2312 Code: 0xB3AB ==> Row:19 Col:11 */
    {0x5025,0xD9C5}, /* 倥: GB2312 Code: 0xD9C5 ==> Row:57 Col:37 */
    {0x5026,0xBEEB}, /* 倦: GB2312 Code: 0xBEEB ==> Row:30 Col:75 */
    {0x5028,0xD9C6}, /* 倨: GB2312 Code: 0xD9C6 ==> Row:57 Col:38 */
    {0x5029,0xD9BB}, /* 倩: GB2312 Code: 0xD9BB ==> Row:57 Col:27 */
    {0x502A,0xC4DF}, /* 倪: GB2312 Code: 0xC4DF ==> Row:36 Col:63 */
    {0x502C,0xD9BE}, /* 倬: GB2312 Code: 0xD9BE ==> Row:57 Col:30 */
    {0x502D,0xD9C1}, /* 倭: GB2312 Code: 0xD9C1 ==> Row:57 Col:33 */
    {0x502E,0xD9C0}, /* 倮: GB2312 Code: 0xD9C0 ==> Row:57 Col:32 */
    {0x503A,0xD5AE}, /* 债: GB2312 Code: 0xD5AE ==> Row:53 Col:14 */
    {0x503C,0xD6B5}, /* 值: GB2312 Code: 0xD6B5 ==> Row:54 Col:21 */
    {0x503E,0xC7E3}, /* 倾: GB2312 Code: 0xC7E3 ==> Row:39 Col:67 */
    {0x5043,0xD9C8}, /* 偃: GB2312 Code: 0xD9C8 ==> Row:57 Col:40 */
    {0x5047,0xBCD9}, /* 假: GB2312 Code: 0xBCD9 ==> Row:28 Col:57 */
    {0x5048,0xD9CA}, /* 偈: GB2312 Code: 0xD9CA ==> Row:57 Col:42 */
    {0x504C,0xD9BC}, /* 偌: GB2312 Code: 0xD9BC ==> Row:57 Col:28 */
    {0x504E,0xD9CB}, /* 偎: GB2312 Code: 0xD9CB ==> Row:57 Col:43 */
    {0x504F,0xC6AB}, /* 偏: GB2312 Code: 0xC6AB ==> Row:38 Col:11 */
    {0x5055,0xD9C9}, /* 偕: GB2312 Code: 0xD9C9 ==> Row:57 Col:41 */
    {0x505A,0xD7F6}, /* 做: GB2312 Code: 0xD7F6 ==> Row:55 Col:86 */
    {0x505C,0xCDA3}, /* 停: GB2312 Code: 0xCDA3 ==> Row:45 Col:03 */
    {0x5065,0xBDA1}, /* 健: GB2312 Code: 0xBDA1 ==> Row:29 Col:01 */
    {0x506C,0xD9CC}, /* 偬: GB2312 Code: 0xD9CC ==> Row:57 Col:44 */
    {0x5076,0xC5BC}, /* 偶: GB2312 Code: 0xC5BC ==> Row:37 Col:28 */
    {0x5077,0xCDB5}, /* 偷: GB2312 Code: 0xCDB5 ==> Row:45 Col:21 */
    {0x507B,0xD9CD}, /* 偻: GB2312 Code: 0xD9CD ==> Row:57 Col:45 */
    {0x507E,0xD9C7}, /* 偾: GB2312 Code: 0xD9C7 ==> Row:57 Col:39 */
    {0x507F,0xB3A5}, /* 偿: GB2312 Code: 0xB3A5 ==> Row:19 Col:05 */
    {0x5080,0xBFFE}, /* 傀: GB2312 Code: 0xBFFE ==> Row:31 Col:94 */
    {0x5085,0xB8B5}, /* 傅: GB2312 Code: 0xB8B5 ==> Row:24 Col:21 */
    {0x5088,0xC0FC}, /* 傈: GB2312 Code: 0xC0FC ==> Row:32 Col:92 */
    {0x508D,0xB0F8}, /* 傍: GB2312 Code: 0xB0F8 ==> Row:16 Col:88 */
    {0x50A3,0xB4F6}, /* 傣: GB2312 Code: 0xB4F6 ==> Row:20 Col:86 */
    {0x50A5,0xD9CE}, /* 傥: GB2312 Code: 0xD9CE ==> Row:57 Col:46 */
    {0x50A7,0xD9CF}, /* 傧: GB2312 Code: 0xD9CF ==> Row:57 Col:47 */
    {0x50A8,0xB4A2}, /* 储: GB2312 Code: 0xB4A2 ==> Row:20 Col:02 */
    {0x50A9,0xD9D0}, /* 傩: GB2312 Code: 0xD9D0 ==> Row:57 Col:48 */
    {0x50AC,0xB4DF}, /* 催: GB2312 Code: 0xB4DF ==> Row:20 Col:63 */
    {0x50B2,0xB0C1}, /* 傲: GB2312 Code: 0xB0C1 ==> Row:16 Col:33 */
    {0x50BA,0xD9D1}, /* 傺: GB2312 Code: 0xD9D1 ==> Row:57 Col:49 */
    {0x50BB,0xC9B5}, /* 傻: GB2312 Code: 0xC9B5 ==> Row:41 Col:21 */
    {0x50CF,0xCFF1}, /* 像: GB2312 Code: 0xCFF1 ==> Row:47 Col:81 */
    {0x50D6,0xD9D2}, /* 僖: GB2312 Code: 0xD9D2 ==> Row:57 Col:50 */
    {0x50DA,0xC1C5}, /* 僚: GB2312 Code: 0xC1C5 ==> Row:33 Col:37 */
    {0x50E6,0xD9D6}, /* 僦: GB2312 Code: 0xD9D6 ==> Row:57 Col:54 */
    {0x50E7,0xC9AE}, /* 僧: GB2312 Code: 0xC9AE ==> Row:41 Col:14 */
    {0x50EC,0xD9D5}, /* 僬: GB2312 Code: 0xD9D5 ==> Row:57 Col:53 */
    {0x50ED,0xD9D4}, /* 僭: GB2312 Code: 0xD9D4 ==> Row:57 Col:52 */
    {0x50EE,0xD9D7}, /* 僮: GB2312 Code: 0xD9D7 ==> Row:57 Col:55 */
    {0x50F3,0xCBDB}, /* 僳: GB2312 Code: 0xCBDB ==> Row:43 Col:59 */
    {0x50F5,0xBDA9}, /* 僵: GB2312 Code: 0xBDA9 ==> Row:29 Col:09 */
    {0x50FB,0xC6A7}, /* 僻: GB2312 Code: 0xC6A7 ==> Row:38 Col:07 */
    {0x5106,0xD9D3}, /* 儆: GB2312 Code: 0xD9D3 ==> Row:57 Col:51 */
    {0x5107,0xD9D8}, /* 儇: GB2312 Code: 0xD9D8 ==> Row:57 Col:56 */
    {0x510B,0xD9D9}, /* 儋: GB2312 Code: 0xD9D9 ==> Row:57 Col:57 */
    {0x5112,0xC8E5}, /* 儒: GB2312 Code: 0xC8E5 ==> Row:40 Col:69 */
    {0x5121,0xC0DC}, /* 儡: GB2312 Code: 0xC0DC ==> Row:32 Col:60 */
    {0x513F,0xB6F9}, /* 儿: GB2312 Code: 0xB6F9 ==> Row:22 Col:89 */
    {0x5140,0xD8A3}, /* 兀: GB2312 Code: 0xD8A3 ==> Row:56 Col:03 */
    {0x5141,0xD4CA}, /* 允: GB2312 Code: 0xD4CA ==> Row:52 Col:42 */
    {0x5143,0xD4AA}, /* 元: GB2312 Code: 0xD4AA ==> Row:52 Col:10 */
    {0x5144,0xD0D6}, /* 兄: GB2312 Code: 0xD0D6 ==> Row:48 Col:54 */
    {0x5145,0xB3E4}, /* 充: GB2312 Code: 0xB3E4 ==> Row:19 Col:68 */
    {0x5146,0xD5D7}, /* 兆: GB2312 Code: 0xD5D7 ==> Row:53 Col:55 */
    {0x5148,0xCFC8}, /* 先: GB2312 Code: 0xCFC8 ==> Row:47 Col:40 */
    {0x5149,0xB9E2}, /* 光: GB2312 Code: 0xB9E2 ==> Row:25 Col:66 */
    {0x514B,0xBFCB}, /* 克: GB2312 Code: 0xBFCB ==> Row:31 Col:43 */
    {0x514D,0xC3E2}, /* 免: GB2312 Code: 0xC3E2 ==> Row:35 Col:66 */
    {0x5151,0xB6D2}, /* 兑: GB2312 Code: 0xB6D2 ==> Row:22 Col:50 */
    {0x5154,0xCDC3}, /* 兔: GB2312 Code: 0xCDC3 ==> Row:45 Col:35 */
    {0x5155,0xD9EE}, /* 兕: GB2312 Code: 0xD9EE ==> Row:57 Col:78 */
    {0x5156,0xD9F0}, /* 兖: GB2312 Code: 0xD9F0 ==> Row:57 Col:80 */
    {0x515A,0xB5B3}, /* 党: GB2312 Code: 0xB5B3 ==> Row:21 Col:19 */
    {0x515C,0xB6B5}, /* 兜: GB2312 Code: 0xB6B5 ==> Row:22 Col:21 */
    {0x5162,0xBEA4}, /* 兢: GB2312 Code: 0xBEA4 ==> Row:30 Col:04 */
    {0x5165,0xC8EB}, /* 入: GB2312 Code: 0xC8EB ==> Row:40 Col:75 */
    {0x5168,0xC8AB}, /* 全: GB2312 Code: 0xC8AB ==> Row:40 Col:11 */
    {0x516B,0xB0CB}, /* 八: GB2312 Code: 0xB0CB ==> Row:16 Col:43 */
    {0x516C,0xB9AB}, /* 公: GB2312 Code: 0xB9AB ==> Row:25 Col:11 */
    {0x516D,0xC1F9}, /* 六: GB2312 Code: 0xC1F9 ==> Row:33 Col:89 */
    {0x516E,0xD9E2}, /* 兮: GB2312 Code: 0xD9E2 ==> Row:57 Col:66 */
    {0x5170,0xC0BC}, /* 兰: GB2312 Code: 0xC0BC ==> Row:32 Col:28 */
    {0x5171,0xB9B2}, /* 共: GB2312 Code: 0xB9B2 ==> Row:25 Col:18 */
    {0x5173,0xB9D8}, /* 关: GB2312 Code: 0xB9D8 ==> Row:25 Col:56 */
    {0x5174,0xD0CB}, /* 兴: GB2312 Code: 0xD0CB ==> Row:48 Col:43 */
    {0x5175,0xB1F8}, /* 兵: GB2312 Code: 0xB1F8 ==> Row:17 Col:88 */
    {0x5176,0xC6E4}, /* 其: GB2312 Code: 0xC6E4 ==> Row:38 Col:68 */
    {0x5177,0xBEDF}, /* 具: GB2312 Code: 0xBEDF ==> Row:30 Col:63 */
    {0x5178,0xB5E4}, /* 典: GB2312 Code: 0xB5E4 ==> Row:21 Col:68 */
    {0x5179,0xD7C8}, /* 兹: GB2312 Code: 0xD7C8 ==> Row:55 Col:40 */
    {0x517B,0xD1F8}, /* 养: GB2312 Code: 0xD1F8 ==> Row:49 Col:88 */
    {0x517C,0xBCE6}, /* 兼: GB2312 Code: 0xBCE6 ==> Row:28 Col:70 */
    {0x517D,0xCADE}, /* 兽: GB2312 Code: 0xCADE ==> Row:42 Col:62 */
    {0x5180,0xBCBD}, /* 冀: GB2312 Code: 0xBCBD ==> Row:28 Col:29 */
    {0x5181,0xD9E6}, /* 冁: GB2312 Code: 0xD9E6 ==> Row:57 Col:70 */
    {0x5182,0xD8E7}, /* 冂: GB2312 Code: 0xD8E7 ==> Row:56 Col:71 */
    {0x5185,0xC4DA}, /* 内: GB2312 Code: 0xC4DA ==> Row:36 Col:58 */
    {0x5188,0xB8D4}, /* 冈: GB2312 Code: 0xB8D4 ==> Row:24 Col:52 */
    {0x5189,0xC8BD}, /* 冉: GB2312 Code: 0xC8BD ==> Row:40 Col:29 */
    {0x518C,0xB2E1}, /* 册: GB2312 Code: 0xB2E1 ==> Row:18 Col:65 */
    {0x518D,0xD4D9}, /* 再: GB2312 Code: 0xD4D9 ==> Row:52 Col:57 */
    {0x5192,0xC3B0}, /* 冒: GB2312 Code: 0xC3B0 ==> Row:35 Col:16 */
    {0x5195,0xC3E1}, /* 冕: GB2312 Code: 0xC3E1 ==> Row:35 Col:65 */
    {0x5196,0xDAA2}, /* 冖: GB2312 Code: 0xDAA2 ==> Row:58 Col:02 */
    {0x5197,0xC8DF}, /* 冗: GB2312 Code: 0xC8DF ==> Row:40 Col:63 */
    {0x5199,0xD0B4}, /* 写: GB2312 Code: 0xD0B4 ==> Row:48 Col:20 */
    {0x519B,0xBEFC}, /* 军: GB2312 Code: 0xBEFC ==> Row:30 Col:92 */
    {0x519C,0xC5A9}, /* 农: GB2312 Code: 0xC5A9 ==> Row:37 Col:09 */
    {0x51A0,0xB9DA}, /* 冠: GB2312 Code: 0xB9DA ==> Row:25 Col:58 */
    {0x51A2,0xDAA3}, /* 冢: GB2312 Code: 0xDAA3 ==> Row:58 Col:03 */
    {0x51A4,0xD4A9}, /* 冤: GB2312 Code: 0xD4A9 ==> Row:52 Col:09 */
    {0x51A5,0xDAA4}, /* 冥: GB2312 Code: 0xDAA4 ==> Row:58 Col:04 */
    {0x51AB,0xD9FB}, /* 冫: GB2312 Code: 0xD9FB ==> Row:57 Col:91 */
    {0x51AC,0xB6AC}, /* 冬: GB2312 Code: 0xB6AC ==> Row:22 Col:12 */
    {0x51AF,0xB7EB}, /* 冯: GB2312 Code: 0xB7EB ==> Row:23 Col:75 */
    {0x51B0,0xB1F9}, /* 冰: GB2312 Code: 0xB1F9 ==> Row:17 Col:89 */
    {0x51B1,0xD9FC}, /* 冱: GB2312 Code: 0xD9FC ==> Row:57 Col:92 */
    {0x51B2,0xB3E5}, /* 冲: GB2312 Code: 0xB3E5 ==> Row:19 Col:69 */
    {0x51B3,0xBEF6}, /* 决: GB2312 Code: 0xBEF6 ==> Row:30 Col:86 */
    {0x51B5,0xBFF6}, /* 况: GB2312 Code: 0xBFF6 ==> Row:31 Col:86 */
    {0x51B6,0xD2B1}, /* 冶: GB2312 Code: 0xD2B1 ==> Row:50 Col:17 */
    {0x51B7,0xC0E4}, /* 冷: GB2312 Code: 0xC0E4 ==> Row:32 Col:68 */
    {0x51BB,0xB6B3}, /* 冻: GB2312 Code: 0xB6B3 ==> Row:22 Col:19 */
    {0x51BC,0xD9FE}, /* 冼: GB2312 Code: 0xD9FE ==> Row:57 Col:94 */
    {0x51BD,0xD9FD}, /* 冽: GB2312 Code: 0xD9FD ==> Row:57 Col:93 */
    {0x51C0,0xBEBB}, /* 净: GB2312 Code: 0xBEBB ==> Row:30 Col:27 */
    {0x51C4,0xC6E0}, /* 凄: GB2312 Code: 0xC6E0 ==> Row:38 Col:64 */
    {0x51C6,0xD7BC}, /* 准: GB2312 Code: 0xD7BC ==> Row:55 Col:28 */
    {0x51C7,0xDAA1}, /* 凇: GB2312 Code: 0xDAA1 ==> Row:58 Col:01 */
    {0x51C9,0xC1B9}, /* 凉: GB2312 Code: 0xC1B9 ==> Row:33 Col:25 */
    {0x51CB,0xB5F2}, /* 凋: GB2312 Code: 0xB5F2 ==> Row:21 Col:82 */
    {0x51CC,0xC1E8}, /* 凌: GB2312 Code: 0xC1E8 ==> Row:33 Col:72 */
    {0x51CF,0xBCF5}, /* 减: GB2312 Code: 0xBCF5 ==> Row:28 Col:85 */
    {0x51D1,0xB4D5}, /* 凑: GB2312 Code: 0xB4D5 ==> Row:20 Col:53 */
    {0x51DB,0xC1DD}, /* 凛: GB2312 Code: 0xC1DD ==> Row:33 Col:61 */
    {0x51DD,0xC4FD}, /* 凝: GB2312 Code: 0xC4FD ==> Row:36 Col:93 */
    {0x51E0,0xBCB8}, /* 几: GB2312 Code: 0xBCB8 ==> Row:28 Col:24 */
    {0x51E1,0xB7B2}, /* 凡: GB2312 Code: 0xB7B2 ==> Row:23 Col:18 */
    {0x51E4,0xB7EF}, /* 凤: GB2312 Code: 0xB7EF ==> Row:23 Col:79 */
    {0x51EB,0xD9EC}, /* 凫: GB2312 Code: 0xD9EC ==> Row:57 Col:76 */
    {0x51ED,0xC6BE}, /* 凭: GB2312 Code: 0xC6BE ==> Row:38 Col:30 */
    {0x51EF,0xBFAD}, /* 凯: GB2312 Code: 0xBFAD ==> Row:31 Col:13 */
    {0x51F0,0xBBCB}, /* 凰: GB2312 Code: 0xBBCB ==> Row:27 Col:43 */
    {0x51F3,0xB5CA}, /* 凳: GB2312 Code: 0xB5CA ==> Row:21 Col:42 */
    {0x51F5,0xDBC9}, /* 凵: GB2312 Code: 0xDBC9 ==> Row:59 Col:41 */
    {0x51F6,0xD0D7}, /* 凶: GB2312 Code: 0xD0D7 ==> Row:48 Col:55 */
    {0x51F8,0xCDB9}, /* 凸: GB2312 Code: 0xCDB9 ==> Row:45 Col:25 */
    {0x51F9,0xB0BC}, /* 凹: GB2312 Code: 0xB0BC ==> Row:16 Col:28 */
    {0x51FA,0xB3F6}, /* 出: GB2312 Code: 0xB3F6 ==> Row:19 Col:86 */
    {0x51FB,0xBBF7}, /* 击: GB2312 Code: 0xBBF7 ==> Row:27 Col:87 */
    {0x51FC,0xDBCA}, /* 凼: GB2312 Code: 0xDBCA ==> Row:59 Col:42 */
    {0x51FD,0xBAAF}, /* 函: GB2312 Code: 0xBAAF ==> Row:26 Col:15 */
    {0x51FF,0xD4E4}, /* 凿: GB2312 Code: 0xD4E4 ==> Row:52 Col:68 */
    {0x5200,0xB5B6}, /* 刀: GB2312 Code: 0xB5B6 ==> Row:21 Col:22 */
    {0x5201,0xB5F3}, /* 刁: GB2312 Code: 0xB5F3 ==> Row:21 Col:83 */
    {0x5202,0xD8D6}, /* 刂: GB2312 Code: 0xD8D6 ==> Row:56 Col:54 */
    {0x5203,0xC8D0}, /* 刃: GB2312 Code: 0xC8D0 ==> Row:40 Col:48 */
    {0x5206,0xB7D6}, /* 分: GB2312 Code: 0xB7D6 ==> Row:23 Col:54 */
    {0x5207,0xC7D0}, /* 切: GB2312 Code: 0xC7D0 ==> Row:39 Col:48 */
    {0x5208,0xD8D7}, /* 刈: GB2312 Code: 0xD8D7 ==> Row:56 Col:55 */
    {0x520A,0xBFAF}, /* 刊: GB2312 Code: 0xBFAF ==> Row:31 Col:15 */
    {0x520D,0xDBBB}, /* 刍: GB2312 Code: 0xDBBB ==> Row:59 Col:27 */
    {0x520E,0xD8D8}, /* 刎: GB2312 Code: 0xD8D8 ==> Row:56 Col:56 */
    {0x5211,0xD0CC}, /* 刑: GB2312 Code: 0xD0CC ==> Row:48 Col:44 */
    {0x5212,0xBBAE}, /* 划: GB2312 Code: 0xBBAE ==> Row:27 Col:14 */
    {0x5216,0xEBBE}, /* 刖: GB2312 Code: 0xEBBE ==> Row:75 Col:30 */
    {0x5217,0xC1D0}, /* 列: GB2312 Code: 0xC1D0 ==> Row:33 Col:48 */
    {0x5218,0xC1F5}, /* 刘: GB2312 Code: 0xC1F5 ==> Row:33 Col:85 */
    {0x5219,0xD4F2}, /* 则: GB2312 Code: 0xD4F2 ==> Row:52 Col:82 */
    {0x521A,0xB8D5}, /* 刚: GB2312 Code: 0xB8D5 ==> Row:24 Col:53 */
    {0x521B,0xB4B4}, /* 创: GB2312 Code: 0xB4B4 ==> Row:20 Col:20 */
    {0x521D,0xB3F5}, /* 初: GB2312 Code: 0xB3F5 ==> Row:19 Col:85 */
    {0x5220,0xC9BE}, /* 删: GB2312 Code: 0xC9BE ==> Row:41 Col:30 */
    {0x5224,0xC5D0}, /* 判: GB2312 Code: 0xC5D0 ==> Row:37 Col:48 */
    {0x5228,0xC5D9}, /* 刨: GB2312 Code: 0xC5D9 ==> Row:37 Col:57 */
    {0x5229,0xC0FB}, /* 利: GB2312 Code: 0xC0FB ==> Row:32 Col:91 */
    {0x522B,0xB1F0}, /* 别: GB2312 Code: 0xB1F0 ==> Row:17 Col:80 */
    {0x522D,0xD8D9}, /* 刭: GB2312 Code: 0xD8D9 ==> Row:56 Col:57 */
    {0x522E,0xB9CE}, /* 刮: GB2312 Code: 0xB9CE ==> Row:25 Col:46 */
    {0x5230,0xB5BD}, /* 到: GB2312 Code: 0xB5BD ==> Row:21 Col:29 */
    {0x5233,0xD8DA}, /* 刳: GB2312 Code: 0xD8DA ==> Row:56 Col:58 */
    {0x5236,0xD6C6}, /* 制: GB2312 Code: 0xD6C6 ==> Row:54 Col:38 */
    {0x5237,0xCBA2}, /* 刷: GB2312 Code: 0xCBA2 ==> Row:43 Col:02 */
    {0x5238,0xC8AF}, /* 券: GB2312 Code: 0xC8AF ==> Row:40 Col:15 */
    {0x5239,0xC9B2}, /* 刹: GB2312 Code: 0xC9B2 ==> Row:41 Col:18 */
    {0x523A,0xB4CC}, /* 刺: GB2312 Code: 0xB4CC ==> Row:20 Col:44 */
    {0x523B,0xBFCC}, /* 刻: GB2312 Code: 0xBFCC ==> Row:31 Col:44 */
    {0x523D,0xB9F4}, /* 刽: GB2312 Code: 0xB9F4 ==> Row:25 Col:84 */
    {0x523F,0xD8DB}, /* 刿: GB2312 Code: 0xD8DB ==> Row:56 Col:59 */
    {0x5240,0xD8DC}, /* 剀: GB2312 Code: 0xD8DC ==> Row:56 Col:60 */
    {0x5241,0xB6E7}, /* 剁: GB2312 Code: 0xB6E7 ==> Row:22 Col:71 */
    {0x5242,0xBCC1}, /* 剂: GB2312 Code: 0xBCC1 ==> Row:28 Col:33 */
    {0x5243,0xCCEA}, /* 剃: GB2312 Code: 0xCCEA ==> Row:44 Col:74 */
    {0x524A,0xCFF7}, /* 削: GB2312 Code: 0xCFF7 ==> Row:47 Col:87 */
    {0x524C,0xD8DD}, /* 剌: GB2312 Code: 0xD8DD ==> Row:56 Col:61 */
    {0x524D,0xC7B0}, /* 前: GB2312 Code: 0xC7B0 ==> Row:39 Col:16 */
    {0x5250,0xB9D0}, /* 剐: GB2312 Code: 0xB9D0 ==> Row:25 Col:48 */
    {0x5251,0xBDA3}, /* 剑: GB2312 Code: 0xBDA3 ==> Row:29 Col:03 */
    {0x5254,0xCCDE}, /* 剔: GB2312 Code: 0xCCDE ==> Row:44 Col:62 */
    {0x5256,0xC6CA}, /* 剖: GB2312 Code: 0xC6CA ==> Row:38 Col:42 */
    {0x525C,0xD8E0}, /* 剜: GB2312 Code: 0xD8E0 ==> Row:56 Col:64 */
    {0x525E,0xD8DE}, /* 剞: GB2312 Code: 0xD8DE ==> Row:56 Col:62 */
    {0x5261,0xD8DF}, /* 剡: GB2312 Code: 0xD8DF ==> Row:56 Col:63 */
    {0x5265,0xB0FE}, /* 剥: GB2312 Code: 0xB0FE ==> Row:16 Col:94 */
    {0x5267,0xBEE7}, /* 剧: GB2312 Code: 0xBEE7 ==> Row:30 Col:71 */
    {0x5269,0xCAA3}, /* 剩: GB2312 Code: 0xCAA3 ==> Row:42 Col:03 */
    {0x526A,0xBCF4}, /* 剪: GB2312 Code: 0xBCF4 ==> Row:28 Col:84 */
    {0x526F,0xB8B1}, /* 副: GB2312 Code: 0xB8B1 ==> Row:24 Col:17 */
    {0x5272,0xB8EE}, /* 割: GB2312 Code: 0xB8EE ==> Row:24 Col:78 */
    {0x527D,0xD8E2}, /* 剽: GB2312 Code: 0xD8E2 ==> Row:56 Col:66 */
    {0x527F,0xBDCB}, /* 剿: GB2312 Code: 0xBDCB ==> Row:29 Col:43 */
    {0x5281,0xD8E4}, /* 劁: GB2312 Code: 0xD8E4 ==> Row:56 Col:68 */
    {0x5282,0xD8E3}, /* 劂: GB2312 Code: 0xD8E3 ==> Row:56 Col:67 */
    {0x5288,0xC5FC}, /* 劈: GB2312 Code: 0xC5FC ==> Row:37 Col:92 */
    {0x5290,0xD8E5}, /* 劐: GB2312 Code: 0xD8E5 ==> Row:56 Col:69 */
    {0x5293,0xD8E6}, /* 劓: GB2312 Code: 0xD8E6 ==> Row:56 Col:70 */
    {0x529B,0xC1A6}, /* 力: GB2312 Code: 0xC1A6 ==> Row:33 Col:06 */
    {0x529D,0xC8B0}, /* 劝: GB2312 Code: 0xC8B0 ==> Row:40 Col:16 */
    {0x529E,0xB0EC}, /* 办: GB2312 Code: 0xB0EC ==> Row:16 Col:76 */
    {0x529F,0xB9A6}, /* 功: GB2312 Code: 0xB9A6 ==> Row:25 Col:06 */
    {0x52A0,0xBCD3}, /* 加: GB2312 Code: 0xBCD3 ==> Row:28 Col:51 */
    {0x52A1,0xCEF1}, /* 务: GB2312 Code: 0xCEF1 ==> Row:46 Col:81 */
    {0x52A2,0xDBBD}, /* 劢: GB2312 Code: 0xDBBD ==> Row:59 Col:29 */
    {0x52A3,0xC1D3}, /* 劣: GB2312 Code: 0xC1D3 ==> Row:33 Col:51 */
    {0x52A8,0xB6AF}, /* 动: GB2312 Code: 0xB6AF ==> Row:22 Col:15 */
    {0x52A9,0xD6FA}, /* 助: GB2312 Code: 0xD6FA ==> Row:54 Col:90 */
    {0x52AA,0xC5AC}, /* 努: GB2312 Code: 0xC5AC ==> Row:37 Col:12 */
    {0x52AB,0xBDD9}, /* 劫: GB2312 Code: 0xBDD9 ==> Row:29 Col:57 */
    {0x52AC,0xDBBE}, /* 劬: GB2312 Code: 0xDBBE ==> Row:59 Col:30 */
    {0x52AD,0xDBBF}, /* 劭: GB2312 Code: 0xDBBF ==> Row:59 Col:31 */
    {0x52B1,0xC0F8}, /* 励: GB2312 Code: 0xC0F8 ==> Row:32 Col:88 */
    {0x52B2,0xBEA2}, /* 劲: GB2312 Code: 0xBEA2 ==> Row:30 Col:02 */
    {0x52B3,0xC0CD}, /* 劳: GB2312 Code: 0xC0CD ==> Row:32 Col:45 */
    {0x52BE,0xDBC0}, /* 劾: GB2312 Code: 0xDBC0 ==> Row:59 Col:32 */
    {0x52BF,0xCAC6}, /* 势: GB2312 Code: 0xCAC6 ==> Row:42 Col:38 */
    {0x52C3,0xB2AA}, /* 勃: GB2312 Code: 0xB2AA ==> Row:18 Col:10 */
    {0x52C7,0xD3C2}, /* 勇: GB2312 Code: 0xD3C2 ==> Row:51 Col:34 */
    {0x52C9,0xC3E3}, /* 勉: GB2312 Code: 0xC3E3 ==> Row:35 Col:67 */
    {0x52CB,0xD1AB}, /* 勋: GB2312 Code: 0xD1AB ==> Row:49 Col:11 */
    {0x52D0,0xDBC2}, /* 勐: GB2312 Code: 0xDBC2 ==> Row:59 Col:34 */
    {0x52D2,0xC0D5}, /* 勒: GB2312 Code: 0xC0D5 ==> Row:32 Col:53 */
    {0x52D6,0xDBC3}, /* 勖: GB2312 Code: 0xDBC3 ==> Row:59 Col:35 */
    {0x52D8,0xBFB1}, /* 勘: GB2312 Code: 0xBFB1 ==> Row:31 Col:17 */
    {0x52DF,0xC4BC}, /* 募: GB2312 Code: 0xC4BC ==> Row:36 Col:28 */
    {0x52E4,0xC7DA}, /* 勤: GB2312 Code: 0xC7DA ==> Row:39 Col:58 */
    {0x52F0,0xDBC4}, /* 勰: GB2312 Code: 0xDBC4 ==> Row:59 Col:36 */
    {0x52F9,0xD9E8}, /* 勹: GB2312 Code: 0xD9E8 ==> Row:57 Col:72 */
    {0x52FA,0xC9D7}, /* 勺: GB2312 Code: 0xC9D7 ==> Row:41 Col:55 */
    {0x52FE,0xB9B4}, /* 勾: GB2312 Code: 0xB9B4 ==> Row:25 Col:20 */
    {0x52FF,0xCEF0}, /* 勿: GB2312 Code: 0xCEF0 ==> Row:46 Col:80 */
    {0x5300,0xD4C8}, /* 匀: GB2312 Code: 0xD4C8 ==> Row:52 Col:40 */
    {0x5305,0xB0FC}, /* 包: GB2312 Code: 0xB0FC ==> Row:16 Col:92 */
    {0x5306,0xB4D2}, /* 匆: GB2312 Code: 0xB4D2 ==> Row:20 Col:50 */
    {0x5308,0xD0D9}, /* 匈: GB2312 Code: 0xD0D9 ==> Row:48 Col:57 */
    {0x530D,0xD9E9}, /* 匍: GB2312 Code: 0xD9E9 ==> Row:57 Col:73 */
    {0x530F,0xDECB}, /* 匏: GB2312 Code: 0xDECB ==> Row:62 Col:43 */
    {0x5310,0xD9EB}, /* 匐: GB2312 Code: 0xD9EB ==> Row:57 Col:75 */
    {0x5315,0xD8B0}, /* 匕: GB2312 Code: 0xD8B0 ==> Row:56 Col:16 */
    {0x5316,0xBBAF}, /* 化: GB2312 Code: 0xBBAF ==> Row:27 Col:15 */
    {0x5317,0xB1B1}, /* 北: GB2312 Code: 0xB1B1 ==> Row:17 Col:17 */
    {0x5319,0xB3D7}, /* 匙: GB2312 Code: 0xB3D7 ==> Row:19 Col:55 */
    {0x531A,0xD8CE}, /* 匚: GB2312 Code: 0xD8CE ==> Row:56 Col:46 */
    {0x531D,0xD4D1}, /* 匝: GB2312 Code: 0xD4D1 ==> Row:52 Col:49 */
    {0x5320,0xBDB3}, /* 匠: GB2312 Code: 0xBDB3 ==> Row:29 Col:19 */
    {0x5321,0xBFEF}, /* 匡: GB2312 Code: 0xBFEF ==> Row:31 Col:79 */
    {0x5323,0xCFBB}, /* 匣: GB2312 Code: 0xCFBB ==> Row:47 Col:27 */
    {0x5326,0xD8D0}, /* 匦: GB2312 Code: 0xD8D0 ==> Row:56 Col:48 */
    {0x532A,0xB7CB}, /* 匪: GB2312 Code: 0xB7CB ==> Row:23 Col:43 */
    {0x532E,0xD8D1}, /* 匮: GB2312 Code: 0xD8D1 ==> Row:56 Col:49 */
    {0x5339,0xC6A5}, /* 匹: GB2312 Code: 0xC6A5 ==> Row:38 Col:05 */
    {0x533A,0xC7F8}, /* 区: GB2312 Code: 0xC7F8 ==> Row:39 Col:88 */
    {0x533B,0xD2BD}, /* 医: GB2312 Code: 0xD2BD ==> Row:50 Col:29 */
    {0x533E,0xD8D2}, /* 匾: GB2312 Code: 0xD8D2 ==> Row:56 Col:50 */
    {0x533F,0xC4E4}, /* 匿: GB2312 Code: 0xC4E4 ==> Row:36 Col:68 */
    {0x5341,0xCAAE}, /* 十: GB2312 Code: 0xCAAE ==> Row:42 Col:14 */
    {0x5343,0xC7A7}, /* 千: GB2312 Code: 0xC7A7 ==> Row:39 Col:07 */
    {0x5345,0xD8A6}, /* 卅: GB2312 Code: 0xD8A6 ==> Row:56 Col:06 */
    {0x5347,0xC9FD}, /* 升: GB2312 Code: 0xC9FD ==> Row:41 Col:93 */
    {0x5348,0xCEE7}, /* 午: GB2312 Code: 0xCEE7 ==> Row:46 Col:71 */
    {0x5349,0xBBDC}, /* 卉: GB2312 Code: 0xBBDC ==> Row:27 Col:60 */
    {0x534A,0xB0EB}, /* 半: GB2312 Code: 0xB0EB ==> Row:16 Col:75 */
    {0x534E,0xBBAA}, /* 华: GB2312 Code: 0xBBAA ==> Row:27 Col:10 */
    {0x534F,0xD0AD}, /* 协: GB2312 Code: 0xD0AD ==> Row:48 Col:13 */
    {0x5351,0xB1B0}, /* 卑: GB2312 Code: 0xB1B0 ==> Row:17 Col:16 */
    {0x5352,0xD7E4}, /* 卒: GB2312 Code: 0xD7E4 ==> Row:55 Col:68 */
    {0x5353,0xD7BF}, /* 卓: GB2312 Code: 0xD7BF ==> Row:55 Col:31 */
    {0x5355,0xB5A5}, /* 单: GB2312 Code: 0xB5A5 ==> Row:21 Col:05 */
    {0x5356,0xC2F4}, /* 卖: GB2312 Code: 0xC2F4 ==> Row:34 Col:84 */
    {0x5357,0xC4CF}, /* 南: GB2312 Code: 0xC4CF ==> Row:36 Col:47 */
    {0x535A,0xB2A9}, /* 博: GB2312 Code: 0xB2A9 ==> Row:18 Col:09 */
    {0x535C,0xB2B7}, /* 卜: GB2312 Code: 0xB2B7 ==> Row:18 Col:23 */
    {0x535E,0xB1E5}, /* 卞: GB2312 Code: 0xB1E5 ==> Row:17 Col:69 */
    {0x535F,0xDFB2}, /* 卟: GB2312 Code: 0xDFB2 ==> Row:63 Col:18 */
    {0x5360,0xD5BC}, /* 占: GB2312 Code: 0xD5BC ==> Row:53 Col:28 */
    {0x5361,0xBFA8}, /* 卡: GB2312 Code: 0xBFA8 ==> Row:31 Col:08 */
    {0x5362,0xC2AC}, /* 卢: GB2312 Code: 0xC2AC ==> Row:34 Col:12 */
    {0x5363,0xD8D5}, /* 卣: GB2312 Code: 0xD8D5 ==> Row:56 Col:53 */
    {0x5364,0xC2B1}, /* 卤: GB2312 Code: 0xC2B1 ==> Row:34 Col:17 */
    {0x5366,0xD8D4}, /* 卦: GB2312 Code: 0xD8D4 ==> Row:56 Col:52 */
    {0x5367,0xCED4}, /* 卧: GB2312 Code: 0xCED4 ==> Row:46 Col:52 */
    {0x5369,0xDAE0}, /* 卩: GB2312 Code: 0xDAE0 ==> Row:58 Col:64 */
    {0x536B,0xCEC0}, /* 卫: GB2312 Code: 0xCEC0 ==> Row:46 Col:32 */
    {0x536E,0xD8B4}, /* 卮: GB2312 Code: 0xD8B4 ==> Row:56 Col:20 */
    {0x536F,0xC3AE}, /* 卯: GB2312 Code: 0xC3AE ==> Row:35 Col:14 */
    {0x5370,0xD3A1}, /* 印: GB2312 Code: 0xD3A1 ==> Row:51 Col:01 */
    {0x5371,0xCEA3}, /* 危: GB2312 Code: 0xCEA3 ==> Row:46 Col:03 */
    {0x5373,0xBCB4}, /* 即: GB2312 Code: 0xBCB4 ==> Row:28 Col:20 */
    {0x5374,0xC8B4}, /* 却: GB2312 Code: 0xC8B4 ==> Row:40 Col:20 */
    {0x5375,0xC2D1}, /* 卵: GB2312 Code: 0xC2D1 ==> Row:34 Col:49 */
    {0x5377,0xBEED}, /* 卷: GB2312 Code: 0xBEED ==> Row:30 Col:77 */
    {0x5378,0xD0B6}, /* 卸: GB2312 Code: 0xD0B6 ==> Row:48 Col:22 */
    {0x537A,0xDAE1}, /* 卺: GB2312 Code: 0xDAE1 ==> Row:58 Col:65 */
    {0x537F,0xC7E4}, /* 卿: GB2312 Code: 0xC7E4 ==> Row:39 Col:68 */
    {0x5382,0xB3A7}, /* 厂: GB2312 Code: 0xB3A7 ==> Row:19 Col:07 */
    {0x5384,0xB6F2}, /* 厄: GB2312 Code: 0xB6F2 ==> Row:22 Col:82 */
    {0x5385,0xCCFC}, /* 厅: GB2312 Code: 0xCCFC ==> Row:44 Col:92 */
    {0x5386,0xC0FA}, /* 历: GB2312 Code: 0xC0FA ==> Row:32 Col:90 */
    {0x5389,0xC0F7}, /* 厉: GB2312 Code: 0xC0F7 ==> Row:32 Col:87 */
    {0x538B,0xD1B9}, /* 压: GB2312 Code: 0xD1B9 ==> Row:49 Col:25 */
    {0x538C,0xD1E1}, /* 厌: GB2312 Code: 0xD1E1 ==> Row:49 Col:65 */
    {0x538D,0xD8C7}, /* 厍: GB2312 Code: 0xD8C7 ==> Row:56 Col:39 */
    {0x5395,0xB2DE}, /* 厕: GB2312 Code: 0xB2DE ==> Row:18 Col:62 */
    {0x5398,0xC0E5}, /* 厘: GB2312 Code: 0xC0E5 ==> Row:32 Col:69 */
    {0x539A,0xBAF1}, /* 厚: GB2312 Code: 0xBAF1 ==> Row:26 Col:81 */
    {0x539D,0xD8C8}, /* 厝: GB2312 Code: 0xD8C8 ==> Row:56 Col:40 */
    {0x539F,0xD4AD}, /* 原: GB2312 Code: 0xD4AD ==> Row:52 Col:13 */
    {0x53A2,0xCFE1}, /* 厢: GB2312 Code: 0xCFE1 ==> Row:47 Col:65 */
    {0x53A3,0xD8C9}, /* 厣: GB2312 Code: 0xD8C9 ==> Row:56 Col:41 */
    {0x53A5,0xD8CA}, /* 厥: GB2312 Code: 0xD8CA ==> Row:56 Col:42 */
    {0x53A6,0xCFC3}, /* 厦: GB2312 Code: 0xCFC3 ==> Row:47 Col:35 */
    {0x53A8,0xB3F8}, /* 厨: GB2312 Code: 0xB3F8 ==> Row:19 Col:88 */
    {0x53A9,0xBEC7}, /* 厩: GB2312 Code: 0xBEC7 ==> Row:30 Col:39 */
    {0x53AE,0xD8CB}, /* 厮: GB2312 Code: 0xD8CB ==> Row:56 Col:43 */
    {0x53B6,0xDBCC}, /* 厶: GB2312 Code: 0xDBCC ==> Row:59 Col:44 */
    {0x53BB,0xC8A5}, /* 去: GB2312 Code: 0xC8A5 ==> Row:40 Col:05 */
    {0x53BF,0xCFD8}, /* 县: GB2312 Code: 0xCFD8 ==> Row:47 Col:56 */
    {0x53C1,0xC8FE}, /* 叁: GB2312 Code: 0xC8FE ==> Row:40 Col:94 */
    {0x53C2,0xB2CE}, /* 参: GB2312 Code: 0xB2CE ==> Row:18 Col:46 */
    {0x53C8,0xD3D6}, /* 又: GB2312 Code: 0xD3D6 ==> Row:51 Col:54 */
    {0x53C9,0xB2E6}, /* 叉: GB2312 Code: 0xB2E6 ==> Row:18 Col:70 */
    {0x53CA,0xBCB0}, /* 及: GB2312 Code: 0xBCB0 ==> Row:28 Col:16 */
    {0x53CB,0xD3D1}, /* 友: GB2312 Code: 0xD3D1 ==> Row:51 Col:49 */
    {0x53CC,0xCBAB}, /* 双: GB2312 Code: 0xCBAB ==> Row:43 Col:11 */
    {0x53CD,0xB7B4}, /* 反: GB2312 Code: 0xB7B4 ==> Row:23 Col:20 */
    {0x53D1,0xB7A2}, /* 发: GB2312 Code: 0xB7A2 ==> Row:23 Col:02 */
    {0x53D4,0xCAE5}, /* 叔: GB2312 Code: 0xCAE5 ==> Row:42 Col:69 */
    {0x53D6,0xC8A1}, /* 取: GB2312 Code: 0xC8A1 ==> Row:40 Col:01 */
    {0x53D7,0xCADC}, /* 受: GB2312 Code: 0xCADC ==> Row:42 Col:60 */
    {0x53D8,0xB1E4}, /* 变: GB2312 Code: 0xB1E4 ==> Row:17 Col:68 */
    {0x53D9,0xD0F0}, /* 叙: GB2312 Code: 0xD0F0 ==> Row:48 Col:80 */
    {0x53DB,0xC5D1}, /* 叛: GB2312 Code: 0xC5D1 ==> Row:37 Col:49 */
    {0x53DF,0xDBC5}, /* 叟: GB2312 Code: 0xDBC5 ==> Row:59 Col:37 */
    {0x53E0,0xB5FE}, /* 叠: GB2312 Code: 0xB5FE ==> Row:21 Col:94 */
    {0x53E3,0xBFDA}, /* 口: GB2312 Code: 0xBFDA ==> Row:31 Col:58 */
    {0x53E4,0xB9C5}, /* 古: GB2312 Code: 0xB9C5 ==> Row:25 Col:37 */
    {0x53E5,0xBEE4}, /* 句: GB2312 Code: 0xBEE4 ==> Row:30 Col:68 */
    {0x53E6,0xC1ED}, /* 另: GB2312 Code: 0xC1ED ==> Row:33 Col:77 */
    {0x53E8,0xDFB6}, /* 叨: GB2312 Code: 0xDFB6 ==> Row:63 Col:22 */
    {0x53E9,0xDFB5}, /* 叩: GB2312 Code: 0xDFB5 ==> Row:63 Col:21 */
    {0x53EA,0xD6BB}, /* 只: GB2312 Code: 0xD6BB ==> Row:54 Col:27 */
    {0x53EB,0xBDD0}, /* 叫: GB2312 Code: 0xBDD0 ==> Row:29 Col:48 */
    {0x53EC,0xD5D9}, /* 召: GB2312 Code: 0xD5D9 ==> Row:53 Col:57 */
    {0x53ED,0xB0C8}, /* 叭: GB2312 Code: 0xB0C8 ==> Row:16 Col:40 */
    {0x53EE,0xB6A3}, /* 叮: GB2312 Code: 0xB6A3 ==> Row:22 Col:03 */
    {0x53EF,0xBFC9}, /* 可: GB2312 Code: 0xBFC9 ==> Row:31 Col:41 */
    {0x53F0,0xCCA8}, /* 台: GB2312 Code: 0xCCA8 ==> Row:44 Col:08 */
    {0x53F1,0xDFB3}, /* 叱: GB2312 Code: 0xDFB3 ==> Row:63 Col:19 */
    {0x53F2,0xCAB7}, /* 史: GB2312 Code: 0xCAB7 ==> Row:42 Col:23 */
    {0x53F3,0xD3D2}, /* 右: GB2312 Code: 0xD3D2 ==> Row:51 Col:50 */
    {0x53F5,0xD8CF}, /* 叵: GB2312 Code: 0xD8CF ==> Row:56 Col:47 */
    {0x53F6,0xD2B6}, /* 叶: GB2312 Code: 0xD2B6 ==> Row:50 Col:22 */
    {0x53F7,0xBAC5}, /* 号: GB2312 Code: 0xBAC5 ==> Row:26 Col:37 */
    {0x53F8,0xCBBE}, /* 司: GB2312 Code: 0xCBBE ==> Row:43 Col:30 */
    {0x53F9,0xCCBE}, /* 叹: GB2312 Code: 0xCCBE ==> Row:44 Col:30 */
    {0x53FB,0xDFB7}, /* 叻: GB2312 Code: 0xDFB7 ==> Row:63 Col:23 */
    {0x53FC,0xB5F0}, /* 叼: GB2312 Code: 0xB5F0 ==> Row:21 Col:80 */
    {0x53FD,0xDFB4}, /* 叽: GB2312 Code: 0xDFB4 ==> Row:63 Col:20 */
    {0x5401,0xD3F5}, /* 吁: GB2312 Code: 0xD3F5 ==> Row:51 Col:85 */
    {0x5403,0xB3D4}, /* 吃: GB2312 Code: 0xB3D4 ==> Row:19 Col:52 */
    {0x5404,0xB8F7}, /* 各: GB2312 Code: 0xB8F7 ==> Row:24 Col:87 */
    {0x5406,0xDFBA}, /* 吆: GB2312 Code: 0xDFBA ==> Row:63 Col:26 */
    {0x5408,0xBACF}, /* 合: GB2312 Code: 0xBACF ==> Row:26 Col:47 */
    {0x5409,0xBCAA}, /* 吉: GB2312 Code: 0xBCAA ==> Row:28 Col:10 */
    {0x540A,0xB5F5}, /* 吊: GB2312 Code: 0xB5F5 ==> Row:21 Col:85 */
    {0x540C,0xCDAC}, /* 同: GB2312 Code: 0xCDAC ==> Row:45 Col:12 */
    {0x540D,0xC3FB}, /* 名: GB2312 Code: 0xC3FB ==> Row:35 Col:91 */
    {0x540E,0xBAF3}, /* 后: GB2312 Code: 0xBAF3 ==> Row:26 Col:83 */
    {0x540F,0xC0F4}, /* 吏: GB2312 Code: 0xC0F4 ==> Row:32 Col:84 */
    {0x5410,0xCDC2}, /* 吐: GB2312 Code: 0xCDC2 ==> Row:45 Col:34 */
    {0x5411,0xCFF2}, /* 向: GB2312 Code: 0xCFF2 ==> Row:47 Col:82 */
    {0x5412,0xDFB8}, /* 吒: GB2312 Code: 0xDFB8 ==> Row:63 Col:24 */
    {0x5413,0xCFC5}, /* 吓: GB2312 Code: 0xCFC5 ==> Row:47 Col:37 */
    {0x5415,0xC2C0}, /* 吕: GB2312 Code: 0xC2C0 ==> Row:34 Col:32 */
    {0x5416,0xDFB9}, /* 吖: GB2312 Code: 0xDFB9 ==> Row:63 Col:25 */
    {0x5417,0xC2F0}, /* 吗: GB2312 Code: 0xC2F0 ==> Row:34 Col:80 */
    {0x541B,0xBEFD}, /* 君: GB2312 Code: 0xBEFD ==> Row:30 Col:93 */
    {0x541D,0xC1DF}, /* 吝: GB2312 Code: 0xC1DF ==> Row:33 Col:63 */
    {0x541E,0xCDCC}, /* 吞: GB2312 Code: 0xCDCC ==> Row:45 Col:44 */
    {0x541F,0xD2F7}, /* 吟: GB2312 Code: 0xD2F7 ==> Row:50 Col:87 */
    {0x5420,0xB7CD}, /* 吠: GB2312 Code: 0xB7CD ==> Row:23 Col:45 */
    {0x5421,0xDFC1}, /* 吡: GB2312 Code: 0xDFC1 ==> Row:63 Col:33 */
    {0x5423,0xDFC4}, /* 吣: GB2312 Code: 0xDFC4 ==> Row:63 Col:36 */
    {0x5426,0xB7F1}, /* 否: GB2312 Code: 0xB7F1 ==> Row:23 Col:81 */
    {0x5427,0xB0C9}, /* 吧: GB2312 Code: 0xB0C9 ==> Row:16 Col:41 */
    {0x5428,0xB6D6}, /* 吨: GB2312 Code: 0xB6D6 ==> Row:22 Col:54 */
    {0x5429,0xB7D4}, /* 吩: GB2312 Code: 0xB7D4 ==> Row:23 Col:52 */
    {0x542B,0xBAAC}, /* 含: GB2312 Code: 0xBAAC ==> Row:26 Col:12 */
    {0x542C,0xCCFD}, /* 听: GB2312 Code: 0xCCFD ==> Row:44 Col:93 */
    {0x542D,0xBFD4}, /* 吭: GB2312 Code: 0xBFD4 ==> Row:31 Col:52 */
    {0x542E,0xCBB1}, /* 吮: GB2312 Code: 0xCBB1 ==> Row:43 Col:17 */
    {0x542F,0xC6F4}, /* 启: GB2312 Code: 0xC6F4 ==> Row:38 Col:84 */
    {0x5431,0xD6A8}, /* 吱: GB2312 Code: 0xD6A8 ==> Row:54 Col:08 */
    {0x5432,0xDFC5}, /* 吲: GB2312 Code: 0xDFC5 ==> Row:63 Col:37 */
    {0x5434,0xCEE2}, /* 吴: GB2312 Code: 0xCEE2 ==> Row:46 Col:66 */
    {0x5435,0xB3B3}, /* 吵: GB2312 Code: 0xB3B3 ==> Row:19 Col:19 */
    {0x5438,0xCEFC}, /* 吸: GB2312 Code: 0xCEFC ==> Row:46 Col:92 */
    {0x5439,0xB4B5}, /* 吹: GB2312 Code: 0xB4B5 ==> Row:20 Col:21 */
    {0x543B,0xCEC7}, /* 吻: GB2312 Code: 0xCEC7 ==> Row:46 Col:39 */
    {0x543C,0xBAF0}, /* 吼: GB2312 Code: 0xBAF0 ==> Row:26 Col:80 */
    {0x543E,0xCEE1}, /* 吾: GB2312 Code: 0xCEE1 ==> Row:46 Col:65 */
    {0x5440,0xD1BD}, /* 呀: GB2312 Code: 0xD1BD ==> Row:49 Col:29 */
    {0x5443,0xDFC0}, /* 呃: GB2312 Code: 0xDFC0 ==> Row:63 Col:32 */
    {0x5446,0xB4F4}, /* 呆: GB2312 Code: 0xB4F4 ==> Row:20 Col:84 */
    {0x5448,0xB3CA}, /* 呈: GB2312 Code: 0xB3CA ==> Row:19 Col:42 */
    {0x544A,0xB8E6}, /* 告: GB2312 Code: 0xB8E6 ==> Row:24 Col:70 */
    {0x544B,0xDFBB}, /* 呋: GB2312 Code: 0xDFBB ==> Row:63 Col:27 */
    {0x5450,0xC4C5}, /* 呐: GB2312 Code: 0xC4C5 ==> Row:36 Col:37 */
    {0x5452,0xDFBC}, /* 呒: GB2312 Code: 0xDFBC ==> Row:63 Col:28 */
    {0x5453,0xDFBD}, /* 呓: GB2312 Code: 0xDFBD ==> Row:63 Col:29 */
    {0x5454,0xDFBE}, /* 呔: GB2312 Code: 0xDFBE ==> Row:63 Col:30 */
    {0x5455,0xC5BB}, /* 呕: GB2312 Code: 0xC5BB ==> Row:37 Col:27 */
    {0x5456,0xDFBF}, /* 呖: GB2312 Code: 0xDFBF ==> Row:63 Col:31 */
    {0x5457,0xDFC2}, /* 呗: GB2312 Code: 0xDFC2 ==> Row:63 Col:34 */
    {0x5458,0xD4B1}, /* 员: GB2312 Code: 0xD4B1 ==> Row:52 Col:17 */
    {0x5459,0xDFC3}, /* 呙: GB2312 Code: 0xDFC3 ==> Row:63 Col:35 */
    {0x545B,0xC7BA}, /* 呛: GB2312 Code: 0xC7BA ==> Row:39 Col:26 */
    {0x545C,0xCED8}, /* 呜: GB2312 Code: 0xCED8 ==> Row:46 Col:56 */
    {0x5462,0xC4D8}, /* 呢: GB2312 Code: 0xC4D8 ==> Row:36 Col:56 */
    {0x5464,0xDFCA}, /* 呤: GB2312 Code: 0xDFCA ==> Row:63 Col:42 */
    {0x5466,0xDFCF}, /* 呦: GB2312 Code: 0xDFCF ==> Row:63 Col:47 */
    {0x5468,0xD6DC}, /* 周: GB2312 Code: 0xD6DC ==> Row:54 Col:60 */
    {0x5471,0xDFC9}, /* 呱: GB2312 Code: 0xDFC9 ==> Row:63 Col:41 */
    {0x5472,0xDFDA}, /* 呲: GB2312 Code: 0xDFDA ==> Row:63 Col:58 */
    {0x5473,0xCEB6}, /* 味: GB2312 Code: 0xCEB6 ==> Row:46 Col:22 */
    {0x5475,0xBAC7}, /* 呵: GB2312 Code: 0xBAC7 ==> Row:26 Col:39 */
    {0x5476,0xDFCE}, /* 呶: GB2312 Code: 0xDFCE ==> Row:63 Col:46 */
    {0x5477,0xDFC8}, /* 呷: GB2312 Code: 0xDFC8 ==> Row:63 Col:40 */
    {0x5478,0xC5DE}, /* 呸: GB2312 Code: 0xC5DE ==> Row:37 Col:62 */
    {0x547B,0xC9EB}, /* 呻: GB2312 Code: 0xC9EB ==> Row:41 Col:75 */
    {0x547C,0xBAF4}, /* 呼: GB2312 Code: 0xBAF4 ==> Row:26 Col:84 */
    {0x547D,0xC3FC}, /* 命: GB2312 Code: 0xC3FC ==> Row:35 Col:92 */
    {0x5480,0xBED7}, /* 咀: GB2312 Code: 0xBED7 ==> Row:30 Col:55 */
    {0x5482,0xDFC6}, /* 咂: GB2312 Code: 0xDFC6 ==> Row:63 Col:38 */
    {0x5484,0xDFCD}, /* 咄: GB2312 Code: 0xDFCD ==> Row:63 Col:45 */
    {0x5486,0xC5D8}, /* 咆: GB2312 Code: 0xC5D8 ==> Row:37 Col:56 */
    {0x548B,0xD5A6}, /* 咋: GB2312 Code: 0xD5A6 ==> Row:53 Col:06 */
    {0x548C,0xBACD}, /* 和: GB2312 Code: 0xBACD ==> Row:26 Col:45 */
    {0x548E,0xBECC}, /* 咎: GB2312 Code: 0xBECC ==> Row:30 Col:44 */
    {0x548F,0xD3BD}, /* 咏: GB2312 Code: 0xD3BD ==> Row:51 Col:29 */
    {0x5490,0xB8C0}, /* 咐: GB2312 Code: 0xB8C0 ==> Row:24 Col:32 */
    {0x5492,0xD6E4}, /* 咒: GB2312 Code: 0xD6E4 ==> Row:54 Col:68 */
    {0x5494,0xDFC7}, /* 咔: GB2312 Code: 0xDFC7 ==> Row:63 Col:39 */
    {0x5495,0xB9BE}, /* 咕: GB2312 Code: 0xB9BE ==> Row:25 Col:30 */
    {0x5496,0xBFA7}, /* 咖: GB2312 Code: 0xBFA7 ==> Row:31 Col:07 */
    {0x5499,0xC1FC}, /* 咙: GB2312 Code: 0xC1FC ==> Row:33 Col:92 */
    {0x549A,0xDFCB}, /* 咚: GB2312 Code: 0xDFCB ==> Row:63 Col:43 */
    {0x549B,0xDFCC}, /* 咛: GB2312 Code: 0xDFCC ==> Row:63 Col:44 */
    {0x549D,0xDFD0}, /* 咝: GB2312 Code: 0xDFD0 ==> Row:63 Col:48 */
    {0x54A3,0xDFDB}, /* 咣: GB2312 Code: 0xDFDB ==> Row:63 Col:59 */
    {0x54A4,0xDFE5}, /* 咤: GB2312 Code: 0xDFE5 ==> Row:63 Col:69 */
    {0x54A6,0xDFD7}, /* 咦: GB2312 Code: 0xDFD7 ==> Row:63 Col:55 */
    {0x54A7,0xDFD6}, /* 咧: GB2312 Code: 0xDFD6 ==> Row:63 Col:54 */
    {0x54A8,0xD7C9}, /* 咨: GB2312 Code: 0xD7C9 ==> Row:55 Col:41 */
    {0x54A9,0xDFE3}, /* 咩: GB2312 Code: 0xDFE3 ==> Row:63 Col:67 */
    {0x54AA,0xDFE4}, /* 咪: GB2312 Code: 0xDFE4 ==> Row:63 Col:68 */
    {0x54AB,0xE5EB}, /* 咫: GB2312 Code: 0xE5EB ==> Row:69 Col:75 */
    {0x54AC,0xD2A7}, /* 咬: GB2312 Code: 0xD2A7 ==> Row:50 Col:07 */
    {0x54AD,0xDFD2}, /* 咭: GB2312 Code: 0xDFD2 ==> Row:63 Col:50 */
    {0x54AF,0xBFA9}, /* 咯: GB2312 Code: 0xBFA9 ==> Row:31 Col:09 */
    {0x54B1,0xD4DB}, /* 咱: GB2312 Code: 0xD4DB ==> Row:52 Col:59 */
    {0x54B3,0xBFC8}, /* 咳: GB2312 Code: 0xBFC8 ==> Row:31 Col:40 */
    {0x54B4,0xDFD4}, /* 咴: GB2312 Code: 0xDFD4 ==> Row:63 Col:52 */
    {0x54B8,0xCFCC}, /* 咸: GB2312 Code: 0xCFCC ==> Row:47 Col:44 */
    {0x54BB,0xDFDD}, /* 咻: GB2312 Code: 0xDFDD ==> Row:63 Col:61 */
    {0x54BD,0xD1CA}, /* 咽: GB2312 Code: 0xD1CA ==> Row:49 Col:42 */
    {0x54BF,0xDFDE}, /* 咿: GB2312 Code: 0xDFDE ==> Row:63 Col:62 */
    {0x54C0,0xB0A7}, /* 哀: GB2312 Code: 0xB0A7 ==> Row:16 Col:07 */
    {0x54C1,0xC6B7}, /* 品: GB2312 Code: 0xC6B7 ==> Row:38 Col:23 */
    {0x54C2,0xDFD3}, /* 哂: GB2312 Code: 0xDFD3 ==> Row:63 Col:51 */
    {0x54C4,0xBAE5}, /* 哄: GB2312 Code: 0xBAE5 ==> Row:26 Col:69 */
    {0x54C6,0xB6DF}, /* 哆: GB2312 Code: 0xB6DF ==> Row:22 Col:63 */
    {0x54C7,0xCDDB}, /* 哇: GB2312 Code: 0xCDDB ==> Row:45 Col:59 */
    {0x54C8,0xB9FE}, /* 哈: GB2312 Code: 0xB9FE ==> Row:25 Col:94 */
    {0x54C9,0xD4D5}, /* 哉: GB2312 Code: 0xD4D5 ==> Row:52 Col:53 */
    {0x54CC,0xDFDF}, /* 哌: GB2312 Code: 0xDFDF ==> Row:63 Col:63 */
    {0x54CD,0xCFEC}, /* 响: GB2312 Code: 0xCFEC ==> Row:47 Col:76 */
    {0x54CE,0xB0A5}, /* 哎: GB2312 Code: 0xB0A5 ==> Row:16 Col:05 */
    {0x54CF,0xDFE7}, /* 哏: GB2312 Code: 0xDFE7 ==> Row:63 Col:71 */
    {0x54D0,0xDFD1}, /* 哐: GB2312 Code: 0xDFD1 ==> Row:63 Col:49 */
    {0x54D1,0xD1C6}, /* 哑: GB2312 Code: 0xD1C6 ==> Row:49 Col:38 */
    {0x54D2,0xDFD5}, /* 哒: GB2312 Code: 0xDFD5 ==> Row:63 Col:53 */
    {0x54D3,0xDFD8}, /* 哓: GB2312 Code: 0xDFD8 ==> Row:63 Col:56 */
    {0x54D4,0xDFD9}, /* 哔: GB2312 Code: 0xDFD9 ==> Row:63 Col:57 */
    {0x54D5,0xDFDC}, /* 哕: GB2312 Code: 0xDFDC ==> Row:63 Col:60 */
    {0x54D7,0xBBA9}, /* 哗: GB2312 Code: 0xBBA9 ==> Row:27 Col:09 */
    {0x54D9,0xDFE0}, /* 哙: GB2312 Code: 0xDFE0 ==> Row:63 Col:64 */
    {0x54DA,0xDFE1}, /* 哚: GB2312 Code: 0xDFE1 ==> Row:63 Col:65 */
    {0x54DC,0xDFE2}, /* 哜: GB2312 Code: 0xDFE2 ==> Row:63 Col:66 */
    {0x54DD,0xDFE6}, /* 哝: GB2312 Code: 0xDFE6 ==> Row:63 Col:70 */
    {0x54DE,0xDFE8}, /* 哞: GB2312 Code: 0xDFE8 ==> Row:63 Col:72 */
    {0x54DF,0xD3B4}, /* 哟: GB2312 Code: 0xD3B4 ==> Row:51 Col:20 */
    {0x54E5,0xB8E7}, /* 哥: GB2312 Code: 0xB8E7 ==> Row:24 Col:71 */
    {0x54E6,0xC5B6}, /* 哦: GB2312 Code: 0xC5B6 ==> Row:37 Col:22 */
    {0x54E7,0xDFEA}, /* 哧: GB2312 Code: 0xDFEA ==> Row:63 Col:74 */
    {0x54E8,0xC9DA}, /* 哨: GB2312 Code: 0xC9DA ==> Row:41 Col:58 */
    {0x54E9,0xC1A8}, /* 哩: GB2312 Code: 0xC1A8 ==> Row:33 Col:08 */
    {0x54EA,0xC4C4}, /* 哪: GB2312 Code: 0xC4C4 ==> Row:36 Col:36 */
    {0x54ED,0xBFDE}, /* 哭: GB2312 Code: 0xBFDE ==> Row:31 Col:62 */
    {0x54EE,0xCFF8}, /* 哮: GB2312 Code: 0xCFF8 ==> Row:47 Col:88 */
    {0x54F2,0xD5DC}, /* 哲: GB2312 Code: 0xD5DC ==> Row:53 Col:60 */
    {0x54F3,0xDFEE}, /* 哳: GB2312 Code: 0xDFEE ==> Row:63 Col:78 */
    {0x54FA,0xB2B8}, /* 哺: GB2312 Code: 0xB2B8 ==> Row:18 Col:24 */
    {0x54FC,0xBADF}, /* 哼: GB2312 Code: 0xBADF ==> Row:26 Col:63 */
    {0x54FD,0xDFEC}, /* 哽: GB2312 Code: 0xDFEC ==> Row:63 Col:76 */
    {0x54FF,0xDBC1}, /* 哿: GB2312 Code: 0xDBC1 ==> Row:59 Col:33 */
    {0x5501,0xD1E4}, /* 唁: GB2312 Code: 0xD1E4 ==> Row:49 Col:68 */
    {0x5506,0xCBF4}, /* 唆: GB2312 Code: 0xCBF4 ==> Row:43 Col:84 */
    {0x5507,0xB4BD}, /* 唇: GB2312 Code: 0xB4BD ==> Row:20 Col:29 */
    {0x5509,0xB0A6}, /* 唉: GB2312 Code: 0xB0A6 ==> Row:16 Col:06 */
    {0x550F,0xDFF1}, /* 唏: GB2312 Code: 0xDFF1 ==> Row:63 Col:81 */
    {0x5510,0xCCC6}, /* 唐: GB2312 Code: 0xCCC6 ==> Row:44 Col:38 */
    {0x5511,0xDFF2}, /* 唑: GB2312 Code: 0xDFF2 ==> Row:63 Col:82 */
    {0x5514,0xDFED}, /* 唔: GB2312 Code: 0xDFED ==> Row:63 Col:77 */
    {0x551B,0xDFE9}, /* 唛: GB2312 Code: 0xDFE9 ==> Row:63 Col:73 */
    {0x5520,0xDFEB}, /* 唠: GB2312 Code: 0xDFEB ==> Row:63 Col:75 */
    {0x5522,0xDFEF}, /* 唢: GB2312 Code: 0xDFEF ==> Row:63 Col:79 */
    {0x5523,0xDFF0}, /* 唣: GB2312 Code: 0xDFF0 ==> Row:63 Col:80 */
    {0x5524,0xBBBD}, /* 唤: GB2312 Code: 0xBBBD ==> Row:27 Col:29 */
    {0x5527,0xDFF3}, /* 唧: GB2312 Code: 0xDFF3 ==> Row:63 Col:83 */
    {0x552A,0xDFF4}, /* 唪: GB2312 Code: 0xDFF4 ==> Row:63 Col:84 */
    {0x552C,0xBBA3}, /* 唬: GB2312 Code: 0xBBA3 ==> Row:27 Col:03 */
    {0x552E,0xCADB}, /* 售: GB2312 Code: 0xCADB ==> Row:42 Col:59 */
    {0x552F,0xCEA8}, /* 唯: GB2312 Code: 0xCEA8 ==> Row:46 Col:08 */
    {0x5530,0xE0A7}, /* 唰: GB2312 Code: 0xE0A7 ==> Row:64 Col:07 */
    {0x5531,0xB3AA}, /* 唱: GB2312 Code: 0xB3AA ==> Row:19 Col:10 */
    {0x5533,0xE0A6}, /* 唳: GB2312 Code: 0xE0A6 ==> Row:64 Col:06 */
    {0x5537,0xE0A1}, /* 唷: GB2312 Code: 0xE0A1 ==> Row:64 Col:01 */
    {0x553C,0xDFFE}, /* 唼: GB2312 Code: 0xDFFE ==> Row:63 Col:94 */
    {0x553E,0xCDD9}, /* 唾: GB2312 Code: 0xCDD9 ==> Row:45 Col:57 */
    {0x553F,0xDFFC}, /* 唿: GB2312 Code: 0xDFFC ==> Row:63 Col:92 */
    {0x5541,0xDFFA}, /* 啁: GB2312 Code: 0xDFFA ==> Row:63 Col:90 */
    {0x5543,0xBFD0}, /* 啃: GB2312 Code: 0xBFD0 ==> Row:31 Col:48 */
    {0x5544,0xD7C4}, /* 啄: GB2312 Code: 0xD7C4 ==> Row:55 Col:36 */
    {0x5546,0xC9CC}, /* 商: GB2312 Code: 0xC9CC ==> Row:41 Col:44 */
    {0x5549,0xDFF8}, /* 啉: GB2312 Code: 0xDFF8 ==> Row:63 Col:88 */
    {0x554A,0xB0A1}, /* 啊: GB2312 Code: 0xB0A1 ==> Row:16 Col:01 */
    {0x5550,0xDFFD}, /* 啐: GB2312 Code: 0xDFFD ==> Row:63 Col:93 */
    {0x5555,0xDFFB}, /* 啕: GB2312 Code: 0xDFFB ==> Row:63 Col:91 */
    {0x5556,0xE0A2}, /* 啖: GB2312 Code: 0xE0A2 ==> Row:64 Col:02 */
    {0x555C,0xE0A8}, /* 啜: GB2312 Code: 0xE0A8 ==> Row:64 Col:08 */
    {0x5561,0xB7C8}, /* 啡: GB2312 Code: 0xB7C8 ==> Row:23 Col:40 */
    {0x5564,0xC6A1}, /* 啤: GB2312 Code: 0xC6A1 ==> Row:38 Col:01 */
    {0x5565,0xC9B6}, /* 啥: GB2312 Code: 0xC9B6 ==> Row:41 Col:22 */
    {0x5566,0xC0B2}, /* 啦: GB2312 Code: 0xC0B2 ==> Row:32 Col:18 */
    {0x5567,0xDFF5}, /* 啧: GB2312 Code: 0xDFF5 ==> Row:63 Col:85 */
    {0x556A,0xC5BE}, /* 啪: GB2312 Code: 0xC5BE ==> Row:37 Col:30 */
    {0x556C,0xD8C4}, /* 啬: GB2312 Code: 0xD8C4 ==> Row:56 Col:36 */
    {0x556D,0xDFF9}, /* 啭: GB2312 Code: 0xDFF9 ==> Row:63 Col:89 */
    {0x556E,0xC4F6}, /* 啮: GB2312 Code: 0xC4F6 ==> Row:36 Col:86 */
    {0x5575,0xE0A3}, /* 啵: GB2312 Code: 0xE0A3 ==> Row:64 Col:03 */
    {0x5576,0xE0A4}, /* 啶: GB2312 Code: 0xE0A4 ==> Row:64 Col:04 */
    {0x5577,0xE0A5}, /* 啷: GB2312 Code: 0xE0A5 ==> Row:64 Col:05 */
    {0x5578,0xD0A5}, /* 啸: GB2312 Code: 0xD0A5 ==> Row:48 Col:05 */
    {0x557B,0xE0B4}, /* 啻: GB2312 Code: 0xE0B4 ==> Row:64 Col:20 */
    {0x557C,0xCCE4}, /* 啼: GB2312 Code: 0xCCE4 ==> Row:44 Col:68 */
    {0x557E,0xE0B1}, /* 啾: GB2312 Code: 0xE0B1 ==> Row:64 Col:17 */
    {0x5580,0xBFA6}, /* 喀: GB2312 Code: 0xBFA6 ==> Row:31 Col:06 */
    {0x5581,0xE0AF}, /* 喁: GB2312 Code: 0xE0AF ==> Row:64 Col:15 */
    {0x5582,0xCEB9}, /* 喂: GB2312 Code: 0xCEB9 ==> Row:46 Col:25 */
    {0x5583,0xE0AB}, /* 喃: GB2312 Code: 0xE0AB ==> Row:64 Col:11 */
    {0x5584,0xC9C6}, /* 善: GB2312 Code: 0xC9C6 ==> Row:41 Col:38 */
    {0x5587,0xC0AE}, /* 喇: GB2312 Code: 0xC0AE ==> Row:32 Col:14 */
    {0x5588,0xE0AE}, /* 喈: GB2312 Code: 0xE0AE ==> Row:64 Col:14 */
    {0x5589,0xBAED}, /* 喉: GB2312 Code: 0xBAED ==> Row:26 Col:77 */
    {0x558A,0xBAB0}, /* 喊: GB2312 Code: 0xBAB0 ==> Row:26 Col:16 */
    {0x558B,0xE0A9}, /* 喋: GB2312 Code: 0xE0A9 ==> Row:64 Col:09 */
    {0x558F,0xDFF6}, /* 喏: GB2312 Code: 0xDFF6 ==> Row:63 Col:86 */
    {0x5591,0xE0B3}, /* 喑: GB2312 Code: 0xE0B3 ==> Row:64 Col:19 */
    {0x5594,0xE0B8}, /* 喔: GB2312 Code: 0xE0B8 ==> Row:64 Col:24 */
    {0x5598,0xB4AD}, /* 喘: GB2312 Code: 0xB4AD ==> Row:20 Col:13 */
    {0x5599,0xE0B9}, /* 喙: GB2312 Code: 0xE0B9 ==> Row:64 Col:25 */
    {0x559C,0xCFB2}, /* 喜: GB2312 Code: 0xCFB2 ==> Row:47 Col:18 */
    {0x559D,0xBAC8}, /* 喝: GB2312 Code: 0xBAC8 ==> Row:26 Col:40 */
    {0x559F,0xE0B0}, /* 喟: GB2312 Code: 0xE0B0 ==> Row:64 Col:16 */
    {0x55A7,0xD0FA}, /* 喧: GB2312 Code: 0xD0FA ==> Row:48 Col:90 */
    {0x55B1,0xE0AC}, /* 喱: GB2312 Code: 0xE0AC ==> Row:64 Col:12 */
    {0x55B3,0xD4FB}, /* 喳: GB2312 Code: 0xD4FB ==> Row:52 Col:91 */
    {0x55B5,0xDFF7}, /* 喵: GB2312 Code: 0xDFF7 ==> Row:63 Col:87 */
    {0x55B7,0xC5E7}, /* 喷: GB2312 Code: 0xC5E7 ==> Row:37 Col:71 */
    {0x55B9,0xE0AD}, /* 喹: GB2312 Code: 0xE0AD ==> Row:64 Col:13 */
    {0x55BB,0xD3F7}, /* 喻: GB2312 Code: 0xD3F7 ==> Row:51 Col:87 */
    {0x55BD,0xE0B6}, /* 喽: GB2312 Code: 0xE0B6 ==> Row:64 Col:22 */
    {0x55BE,0xE0B7}, /* 喾: GB2312 Code: 0xE0B7 ==> Row:64 Col:23 */
    {0x55C4,0xE0C4}, /* 嗄: GB2312 Code: 0xE0C4 ==> Row:64 Col:36 */
    {0x55C5,0xD0E1}, /* 嗅: GB2312 Code: 0xD0E1 ==> Row:48 Col:65 */
    {0x55C9,0xE0BC}, /* 嗉: GB2312 Code: 0xE0BC ==> Row:64 Col:28 */
    {0x55CC,0xE0C9}, /* 嗌: GB2312 Code: 0xE0C9 ==> Row:64 Col:41 */
    {0x55CD,0xE0CA}, /* 嗍: GB2312 Code: 0xE0CA ==> Row:64 Col:42 */
    {0x55D1,0xE0BE}, /* 嗑: GB2312 Code: 0xE0BE ==> Row:64 Col:30 */
    {0x55D2,0xE0AA}, /* 嗒: GB2312 Code: 0xE0AA ==> Row:64 Col:10 */
    {0x55D3,0xC9A4}, /* 嗓: GB2312 Code: 0xC9A4 ==> Row:41 Col:04 */
    {0x55D4,0xE0C1}, /* 嗔: GB2312 Code: 0xE0C1 ==> Row:64 Col:33 */
    {0x55D6,0xE0B2}, /* 嗖: GB2312 Code: 0xE0B2 ==> Row:64 Col:18 */
    {0x55DC,0xCAC8}, /* 嗜: GB2312 Code: 0xCAC8 ==> Row:42 Col:40 */
    {0x55DD,0xE0C3}, /* 嗝: GB2312 Code: 0xE0C3 ==> Row:64 Col:35 */
    {0x55DF,0xE0B5}, /* 嗟: GB2312 Code: 0xE0B5 ==> Row:64 Col:21 */
    {0x55E1,0xCECB}, /* 嗡: GB2312 Code: 0xCECB ==> Row:46 Col:43 */
    {0x55E3,0xCBC3}, /* 嗣: GB2312 Code: 0xCBC3 ==> Row:43 Col:35 */
    {0x55E4,0xE0CD}, /* 嗤: GB2312 Code: 0xE0CD ==> Row:64 Col:45 */
    {0x55E5,0xE0C6}, /* 嗥: GB2312 Code: 0xE0C6 ==> Row:64 Col:38 */
    {0x55E6,0xE0C2}, /* 嗦: GB2312 Code: 0xE0C2 ==> Row:64 Col:34 */
    {0x55E8,0xE0CB}, /* 嗨: GB2312 Code: 0xE0CB ==> Row:64 Col:43 */
    {0x55EA,0xE0BA}, /* 嗪: GB2312 Code: 0xE0BA ==> Row:64 Col:26 */
    {0x55EB,0xE0BF}, /* 嗫: GB2312 Code: 0xE0BF ==> Row:64 Col:31 */
    {0x55EC,0xE0C0}, /* 嗬: GB2312 Code: 0xE0C0 ==> Row:64 Col:32 */
    {0x55EF,0xE0C5}, /* 嗯: GB2312 Code: 0xE0C5 ==> Row:64 Col:37 */
    {0x55F2,0xE0C7}, /* 嗲: GB2312 Code: 0xE0C7 ==> Row:64 Col:39 */
    {0x55F3,0xE0C8}, /* 嗳: GB2312 Code: 0xE0C8 ==> Row:64 Col:40 */
    {0x55F5,0xE0CC}, /* 嗵: GB2312 Code: 0xE0CC ==> Row:64 Col:44 */
    {0x55F7,0xE0BB}, /* 嗷: GB2312 Code: 0xE0BB ==> Row:64 Col:27 */
    {0x55FD,0xCBD4}, /* 嗽: GB2312 Code: 0xCBD4 ==> Row:43 Col:52 */
    {0x55FE,0xE0D5}, /* 嗾: GB2312 Code: 0xE0D5 ==> Row:64 Col:53 */
    {0x5600,0xE0D6}, /* 嘀: GB2312 Code: 0xE0D6 ==> Row:64 Col:54 */
    {0x5601,0xE0D2}, /* 嘁: GB2312 Code: 0xE0D2 ==> Row:64 Col:50 */
    {0x5608,0xE0D0}, /* 嘈: GB2312 Code: 0xE0D0 ==> Row:64 Col:48 */
    {0x5609,0xBCCE}, /* 嘉: GB2312 Code: 0xBCCE ==> Row:28 Col:46 */
    {0x560C,0xE0D1}, /* 嘌: GB2312 Code: 0xE0D1 ==> Row:64 Col:49 */
    {0x560E,0xB8C2}, /* 嘎: GB2312 Code: 0xB8C2 ==> Row:24 Col:34 */
    {0x560F,0xD8C5}, /* 嘏: GB2312 Code: 0xD8C5 ==> Row:56 Col:37 */
    {0x5618,0xD0EA}, /* 嘘: GB2312 Code: 0xD0EA ==> Row:48 Col:74 */
    {0x561B,0xC2EF}, /* 嘛: GB2312 Code: 0xC2EF ==> Row:34 Col:79 */
    {0x561E,0xE0CF}, /* 嘞: GB2312 Code: 0xE0CF ==> Row:64 Col:47 */
    {0x561F,0xE0BD}, /* 嘟: GB2312 Code: 0xE0BD ==> Row:64 Col:29 */
    {0x5623,0xE0D4}, /* 嘣: GB2312 Code: 0xE0D4 ==> Row:64 Col:52 */
    {0x5624,0xE0D3}, /* 嘤: GB2312 Code: 0xE0D3 ==> Row:64 Col:51 */
    {0x5627,0xE0D7}, /* 嘧: GB2312 Code: 0xE0D7 ==> Row:64 Col:55 */
    {0x562C,0xE0DC}, /* 嘬: GB2312 Code: 0xE0DC ==> Row:64 Col:60 */
    {0x562D,0xE0D8}, /* 嘭: GB2312 Code: 0xE0D8 ==> Row:64 Col:56 */
    {0x5631,0xD6F6}, /* 嘱: GB2312 Code: 0xD6F6 ==> Row:54 Col:86 */
    {0x5632,0xB3B0}, /* 嘲: GB2312 Code: 0xB3B0 ==> Row:19 Col:16 */
    {0x5634,0xD7EC}, /* 嘴: GB2312 Code: 0xD7EC ==> Row:55 Col:76 */
    {0x5636,0xCBBB}, /* 嘶: GB2312 Code: 0xCBBB ==> Row:43 Col:27 */
    {0x5639,0xE0DA}, /* 嘹: GB2312 Code: 0xE0DA ==> Row:64 Col:58 */
    {0x563B,0xCEFB}, /* 嘻: GB2312 Code: 0xCEFB ==> Row:46 Col:91 */
    {0x563F,0xBAD9}, /* 嘿: GB2312 Code: 0xBAD9 ==> Row:26 Col:57 */
    {0x564C,0xE0E1}, /* 噌: GB2312 Code: 0xE0E1 ==> Row:64 Col:65 */
    {0x564D,0xE0DD}, /* 噍: GB2312 Code: 0xE0DD ==> Row:64 Col:61 */
    {0x564E,0xD2AD}, /* 噎: GB2312 Code: 0xD2AD ==> Row:50 Col:13 */
    {0x5654,0xE0E2}, /* 噔: GB2312 Code: 0xE0E2 ==> Row:64 Col:66 */
    {0x5657,0xE0DB}, /* 噗: GB2312 Code: 0xE0DB ==> Row:64 Col:59 */
    {0x5658,0xE0D9}, /* 噘: GB2312 Code: 0xE0D9 ==> Row:64 Col:57 */
    {0x5659,0xE0DF}, /* 噙: GB2312 Code: 0xE0DF ==> Row:64 Col:63 */
    {0x565C,0xE0E0}, /* 噜: GB2312 Code: 0xE0E0 ==> Row:64 Col:64 */
    {0x5662,0xE0DE}, /* 噢: GB2312 Code: 0xE0DE ==> Row:64 Col:62 */
    {0x5664,0xE0E4}, /* 噤: GB2312 Code: 0xE0E4 ==> Row:64 Col:68 */
    {0x5668,0xC6F7}, /* 器: GB2312 Code: 0xC6F7 ==> Row:38 Col:87 */
    {0x5669,0xD8AC}, /* 噩: GB2312 Code: 0xD8AC ==> Row:56 Col:12 */
    {0x566A,0xD4EB}, /* 噪: GB2312 Code: 0xD4EB ==> Row:52 Col:75 */
    {0x566B,0xE0E6}, /* 噫: GB2312 Code: 0xE0E6 ==> Row:64 Col:70 */
    {0x566C,0xCAC9}, /* 噬: GB2312 Code: 0xCAC9 ==> Row:42 Col:41 */
    {0x5671,0xE0E5}, /* 噱: GB2312 Code: 0xE0E5 ==> Row:64 Col:69 */
    {0x5676,0xB8C1}, /* 噶: GB2312 Code: 0xB8C1 ==> Row:24 Col:33 */
    {0x567B,0xE0E7}, /* 噻: GB2312 Code: 0xE0E7 ==> Row:64 Col:71 */
    {0x567C,0xE0E8}, /* 噼: GB2312 Code: 0xE0E8 ==> Row:64 Col:72 */
    {0x5685,0xE0E9}, /* 嚅: GB2312 Code: 0xE0E9 ==> Row:64 Col:73 */
    {0x5686,0xE0E3}, /* 嚆: GB2312 Code: 0xE0E3 ==> Row:64 Col:67 */
    {0x568E,0xBABF}, /* 嚎: GB2312 Code: 0xBABF ==> Row:26 Col:31 */
    {0x568F,0xCCE7}, /* 嚏: GB2312 Code: 0xCCE7 ==> Row:44 Col:71 */
    {0x5693,0xE0EA}, /* 嚓: GB2312 Code: 0xE0EA ==> Row:64 Col:74 */
    {0x56A3,0xCFF9}, /* 嚣: GB2312 Code: 0xCFF9 ==> Row:47 Col:89 */
    {0x56AF,0xE0EB}, /* 嚯: GB2312 Code: 0xE0EB ==> Row:64 Col:75 */
    {0x56B7,0xC8C2}, /* 嚷: GB2312 Code: 0xC8C2 ==> Row:40 Col:34 */
    {0x56BC,0xBDC0}, /* 嚼: GB2312 Code: 0xBDC0 ==> Row:29 Col:32 */
    {0x56CA,0xC4D2}, /* 囊: GB2312 Code: 0xC4D2 ==> Row:36 Col:50 */
    {0x56D4,0xE0EC}, /* 囔: GB2312 Code: 0xE0EC ==> Row:64 Col:76 */
    {0x56D7,0xE0ED}, /* 囗: GB2312 Code: 0xE0ED ==> Row:64 Col:77 */
    {0x56DA,0xC7F4}, /* 囚: GB2312 Code: 0xC7F4 ==> Row:39 Col:84 */
    {0x56DB,0xCBC4}, /* 四: GB2312 Code: 0xCBC4 ==> Row:43 Col:36 */
    {0x56DD,0xE0EE}, /* 囝: GB2312 Code: 0xE0EE ==> Row:64 Col:78 */
    {0x56DE,0xBBD8}, /* 回: GB2312 Code: 0xBBD8 ==> Row:27 Col:56 */
    {0x56DF,0xD8B6}, /* 囟: GB2312 Code: 0xD8B6 ==> Row:56 Col:22 */
    {0x56E0,0xD2F2}, /* 因: GB2312 Code: 0xD2F2 ==> Row:50 Col:82 */
    {0x56E1,0xE0EF}, /* 囡: GB2312 Code: 0xE0EF ==> Row:64 Col:79 */
    {0x56E2,0xCDC5}, /* 团: GB2312 Code: 0xCDC5 ==> Row:45 Col:37 */
    {0x56E4,0xB6DA}, /* 囤: GB2312 Code: 0xB6DA ==> Row:22 Col:58 */
    {0x56EB,0xE0F1}, /* 囫: GB2312 Code: 0xE0F1 ==> Row:64 Col:81 */
    {0x56ED,0xD4B0}, /* 园: GB2312 Code: 0xD4B0 ==> Row:52 Col:16 */
    {0x56F0,0xC0A7}, /* 困: GB2312 Code: 0xC0A7 ==> Row:32 Col:07 */
    {0x56F1,0xB4D1}, /* 囱: GB2312 Code: 0xB4D1 ==> Row:20 Col:49 */
    {0x56F4,0xCEA7}, /* 围: GB2312 Code: 0xCEA7 ==> Row:46 Col:07 */
    {0x56F5,0xE0F0}, /* 囵: GB2312 Code: 0xE0F0 ==> Row:64 Col:80 */
    {0x56F9,0xE0F2}, /* 囹: GB2312 Code: 0xE0F2 ==> Row:64 Col:82 */
    {0x56FA,0xB9CC}, /* 固: GB2312 Code: 0xB9CC ==> Row:25 Col:44 */
    {0x56FD,0xB9FA}, /* 国: GB2312 Code: 0xB9FA ==> Row:25 Col:90 */
    {0x56FE,0xCDBC}, /* 图: GB2312 Code: 0xCDBC ==> Row:45 Col:28 */
    {0x56FF,0xE0F3}, /* 囿: GB2312 Code: 0xE0F3 ==> Row:64 Col:83 */
    {0x5703,0xC6D4}, /* 圃: GB2312 Code: 0xC6D4 ==> Row:38 Col:52 */
    {0x5704,0xE0F4}, /* 圄: GB2312 Code: 0xE0F4 ==> Row:64 Col:84 */
    {0x5706,0xD4B2}, /* 圆: GB2312 Code: 0xD4B2 ==> Row:52 Col:18 */
    {0x5708,0xC8A6}, /* 圈: GB2312 Code: 0xC8A6 ==> Row:40 Col:06 */
    {0x5709,0xE0F6}, /* 圉: GB2312 Code: 0xE0F6 ==> Row:64 Col:86 */
    {0x570A,0xE0F5}, /* 圊: GB2312 Code: 0xE0F5 ==> Row:64 Col:85 */
    {0x571C,0xE0F7}, /* 圜: GB2312 Code: 0xE0F7 ==> Row:64 Col:87 */
    {0x571F,0xCDC1}, /* 土: GB2312 Code: 0xCDC1 ==> Row:45 Col:33 */
    {0x5723,0xCAA5}, /* 圣: GB2312 Code: 0xCAA5 ==> Row:42 Col:05 */
    {0x5728,0xD4DA}, /* 在: GB2312 Code: 0xD4DA ==> Row:52 Col:58 */
    {0x5729,0xDBD7}, /* 圩: GB2312 Code: 0xDBD7 ==> Row:59 Col:55 */
    {0x572A,0xDBD9}, /* 圪: GB2312 Code: 0xDBD9 ==> Row:59 Col:57 */
    {0x572C,0xDBD8}, /* 圬: GB2312 Code: 0xDBD8 ==> Row:59 Col:56 */
    {0x572D,0xB9E7}, /* 圭: GB2312 Code: 0xB9E7 ==> Row:25 Col:71 */
    {0x572E,0xDBDC}, /* 圮: GB2312 Code: 0xDBDC ==> Row:59 Col:60 */
    {0x572F,0xDBDD}, /* 圯: GB2312 Code: 0xDBDD ==> Row:59 Col:61 */
    {0x5730,0xB5D8}, /* 地: GB2312 Code: 0xB5D8 ==> Row:21 Col:56 */
    {0x5733,0xDBDA}, /* 圳: GB2312 Code: 0xDBDA ==> Row:59 Col:58 */
    {0x5739,0xDBDB}, /* 圹: GB2312 Code: 0xDBDB ==> Row:59 Col:59 */
    {0x573A,0xB3A1}, /* 场: GB2312 Code: 0xB3A1 ==> Row:19 Col:01 */
    {0x573B,0xDBDF}, /* 圻: GB2312 Code: 0xDBDF ==> Row:59 Col:63 */
    {0x573E,0xBBF8}, /* 圾: GB2312 Code: 0xBBF8 ==> Row:27 Col:88 */
    {0x5740,0xD6B7}, /* 址: GB2312 Code: 0xD6B7 ==> Row:54 Col:23 */
    {0x5742,0xDBE0}, /* 坂: GB2312 Code: 0xDBE0 ==> Row:59 Col:64 */
    {0x5747,0xBEF9}, /* 均: GB2312 Code: 0xBEF9 ==> Row:30 Col:89 */
    {0x574A,0xB7BB}, /* 坊: GB2312 Code: 0xB7BB ==> Row:23 Col:27 */
    {0x574C,0xDBD0}, /* 坌: GB2312 Code: 0xDBD0 ==> Row:59 Col:48 */
    {0x574D,0xCCAE}, /* 坍: GB2312 Code: 0xCCAE ==> Row:44 Col:14 */
    {0x574E,0xBFB2}, /* 坎: GB2312 Code: 0xBFB2 ==> Row:31 Col:18 */
    {0x574F,0xBBB5}, /* 坏: GB2312 Code: 0xBBB5 ==> Row:27 Col:21 */
    {0x5750,0xD7F8}, /* 坐: GB2312 Code: 0xD7F8 ==> Row:55 Col:88 */
    {0x5751,0xBFD3}, /* 坑: GB2312 Code: 0xBFD3 ==> Row:31 Col:51 */
    {0x5757,0xBFE9}, /* 块: GB2312 Code: 0xBFE9 ==> Row:31 Col:73 */
    {0x575A,0xBCE1}, /* 坚: GB2312 Code: 0xBCE1 ==> Row:28 Col:65 */
    {0x575B,0xCCB3}, /* 坛: GB2312 Code: 0xCCB3 ==> Row:44 Col:19 */
    {0x575C,0xDBDE}, /* 坜: GB2312 Code: 0xDBDE ==> Row:59 Col:62 */
    {0x575D,0xB0D3}, /* 坝: GB2312 Code: 0xB0D3 ==> Row:16 Col:51 */
    {0x575E,0xCEEB}, /* 坞: GB2312 Code: 0xCEEB ==> Row:46 Col:75 */
    {0x575F,0xB7D8}, /* 坟: GB2312 Code: 0xB7D8 ==> Row:23 Col:56 */
    {0x5760,0xD7B9}, /* 坠: GB2312 Code: 0xD7B9 ==> Row:55 Col:25 */
    {0x5761,0xC6C2}, /* 坡: GB2312 Code: 0xC6C2 ==> Row:38 Col:34 */
    {0x5764,0xC0A4}, /* 坤: GB2312 Code: 0xC0A4 ==> Row:32 Col:04 */
    {0x5766,0xCCB9}, /* 坦: GB2312 Code: 0xCCB9 ==> Row:44 Col:25 */
    {0x5768,0xDBE7}, /* 坨: GB2312 Code: 0xDBE7 ==> Row:59 Col:71 */
    {0x5769,0xDBE1}, /* 坩: GB2312 Code: 0xDBE1 ==> Row:59 Col:65 */
    {0x576A,0xC6BA}, /* 坪: GB2312 Code: 0xC6BA ==> Row:38 Col:26 */
    {0x576B,0xDBE3}, /* 坫: GB2312 Code: 0xDBE3 ==> Row:59 Col:67 */
    {0x576D,0xDBE8}, /* 坭: GB2312 Code: 0xDBE8 ==> Row:59 Col:72 */
    {0x576F,0xC5F7}, /* 坯: GB2312 Code: 0xC5F7 ==> Row:37 Col:87 */
    {0x5773,0xDBEA}, /* 坳: GB2312 Code: 0xDBEA ==> Row:59 Col:74 */
    {0x5776,0xDBE9}, /* 坶: GB2312 Code: 0xDBE9 ==> Row:59 Col:73 */
    {0x5777,0xBFC0}, /* 坷: GB2312 Code: 0xBFC0 ==> Row:31 Col:32 */
    {0x577B,0xDBE6}, /* 坻: GB2312 Code: 0xDBE6 ==> Row:59 Col:70 */
    {0x577C,0xDBE5}, /* 坼: GB2312 Code: 0xDBE5 ==> Row:59 Col:69 */
    {0x5782,0xB4B9}, /* 垂: GB2312 Code: 0xB4B9 ==> Row:20 Col:25 */
    {0x5783,0xC0AC}, /* 垃: GB2312 Code: 0xC0AC ==> Row:32 Col:12 */
    {0x5784,0xC2A2}, /* 垄: GB2312 Code: 0xC2A2 ==> Row:34 Col:02 */
    {0x5785,0xDBE2}, /* 垅: GB2312 Code: 0xDBE2 ==> Row:59 Col:66 */
    {0x5786,0xDBE4}, /* 垆: GB2312 Code: 0xDBE4 ==> Row:59 Col:68 */
    {0x578B,0xD0CD}, /* 型: GB2312 Code: 0xD0CD ==> Row:48 Col:45 */
    {0x578C,0xDBED}, /* 垌: GB2312 Code: 0xDBED ==> Row:59 Col:77 */
    {0x5792,0xC0DD}, /* 垒: GB2312 Code: 0xC0DD ==> Row:32 Col:61 */
    {0x5793,0xDBF2}, /* 垓: GB2312 Code: 0xDBF2 ==> Row:59 Col:82 */
    {0x579B,0xB6E2}, /* 垛: GB2312 Code: 0xB6E2 ==> Row:22 Col:66 */
    {0x57A0,0xDBF3}, /* 垠: GB2312 Code: 0xDBF3 ==> Row:59 Col:83 */
    {0x57A1,0xDBD2}, /* 垡: GB2312 Code: 0xDBD2 ==> Row:59 Col:50 */
    {0x57A2,0xB9B8}, /* 垢: GB2312 Code: 0xB9B8 ==> Row:25 Col:24 */
    {0x57A3,0xD4AB}, /* 垣: GB2312 Code: 0xD4AB ==> Row:52 Col:11 */
    {0x57A4,0xDBEC}, /* 垤: GB2312 Code: 0xDBEC ==> Row:59 Col:76 */
    {0x57A6,0xBFD1}, /* 垦: GB2312 Code: 0xBFD1 ==> Row:31 Col:49 */
    {0x57A7,0xDBF0}, /* 垧: GB2312 Code: 0xDBF0 ==> Row:59 Col:80 */
    {0x57A9,0xDBD1}, /* 垩: GB2312 Code: 0xDBD1 ==> Row:59 Col:49 */
    {0x57AB,0xB5E6}, /* 垫: GB2312 Code: 0xB5E6 ==> Row:21 Col:70 */
    {0x57AD,0xDBEB}, /* 垭: GB2312 Code: 0xDBEB ==> Row:59 Col:75 */
    {0x57AE,0xBFE5}, /* 垮: GB2312 Code: 0xBFE5 ==> Row:31 Col:69 */
    {0x57B2,0xDBEE}, /* 垲: GB2312 Code: 0xDBEE ==> Row:59 Col:78 */
    {0x57B4,0xDBF1}, /* 垴: GB2312 Code: 0xDBF1 ==> Row:59 Col:81 */
    {0x57B8,0xDBF9}, /* 垸: GB2312 Code: 0xDBF9 ==> Row:59 Col:89 */
    {0x57C2,0xB9A1}, /* 埂: GB2312 Code: 0xB9A1 ==> Row:25 Col:01 */
    {0x57C3,0xB0A3}, /* 埃: GB2312 Code: 0xB0A3 ==> Row:16 Col:03 */
    {0x57CB,0xC2F1}, /* 埋: GB2312 Code: 0xC2F1 ==> Row:34 Col:81 */
    {0x57CE,0xB3C7}, /* 城: GB2312 Code: 0xB3C7 ==> Row:19 Col:39 */
    {0x57CF,0xDBEF}, /* 埏: GB2312 Code: 0xDBEF ==> Row:59 Col:79 */
    {0x57D2,0xDBF8}, /* 埒: GB2312 Code: 0xDBF8 ==> Row:59 Col:88 */
    {0x57D4,0xC6D2}, /* 埔: GB2312 Code: 0xC6D2 ==> Row:38 Col:50 */
    {0x57D5,0xDBF4}, /* 埕: GB2312 Code: 0xDBF4 ==> Row:59 Col:84 */
    {0x57D8,0xDBF5}, /* 埘: GB2312 Code: 0xDBF5 ==> Row:59 Col:85 */
    {0x57D9,0xDBF7}, /* 埙: GB2312 Code: 0xDBF7 ==> Row:59 Col:87 */
    {0x57DA,0xDBF6}, /* 埚: GB2312 Code: 0xDBF6 ==> Row:59 Col:86 */
    {0x57DD,0xDBFE}, /* 埝: GB2312 Code: 0xDBFE ==> Row:59 Col:94 */
    {0x57DF,0xD3F2}, /* 域: GB2312 Code: 0xD3F2 ==> Row:51 Col:82 */
    {0x57E0,0xB2BA}, /* 埠: GB2312 Code: 0xB2BA ==> Row:18 Col:26 */
    {0x57E4,0xDBFD}, /* 埤: GB2312 Code: 0xDBFD ==> Row:59 Col:93 */
    {0x57ED,0xDCA4}, /* 埭: GB2312 Code: 0xDCA4 ==> Row:60 Col:04 */
    {0x57EF,0xDBFB}, /* 埯: GB2312 Code: 0xDBFB ==> Row:59 Col:91 */
    {0x57F4,0xDBFA}, /* 埴: GB2312 Code: 0xDBFA ==> Row:59 Col:90 */
    {0x57F8,0xDBFC}, /* 埸: GB2312 Code: 0xDBFC ==> Row:59 Col:92 */
    {0x57F9,0xC5E0}, /* 培: GB2312 Code: 0xC5E0 ==> Row:37 Col:64 */
    {0x57FA,0xBBF9}, /* 基: GB2312 Code: 0xBBF9 ==> Row:27 Col:89 */
    {0x57FD,0xDCA3}, /* 埽: GB2312 Code: 0xDCA3 ==> Row:60 Col:03 */
    {0x5800,0xDCA5}, /* 堀: GB2312 Code: 0xDCA5 ==> Row:60 Col:05 */
    {0x5802,0xCCC3}, /* 堂: GB2312 Code: 0xCCC3 ==> Row:44 Col:35 */
    {0x5806,0xB6D1}, /* 堆: GB2312 Code: 0xB6D1 ==> Row:22 Col:49 */
    {0x5807,0xDDC0}, /* 堇: GB2312 Code: 0xDDC0 ==> Row:61 Col:32 */
    {0x580B,0xDCA1}, /* 堋: GB2312 Code: 0xDCA1 ==> Row:60 Col:01 */
    {0x580D,0xDCA2}, /* 堍: GB2312 Code: 0xDCA2 ==> Row:60 Col:02 */
    {0x5811,0xC7B5}, /* 堑: GB2312 Code: 0xC7B5 ==> Row:39 Col:21 */
    {0x5815,0xB6E9}, /* 堕: GB2312 Code: 0xB6E9 ==> Row:22 Col:73 */
    {0x5819,0xDCA7}, /* 堙: GB2312 Code: 0xDCA7 ==> Row:60 Col:07 */
    {0x581E,0xDCA6}, /* 堞: GB2312 Code: 0xDCA6 ==> Row:60 Col:06 */
    {0x5820,0xDCA9}, /* 堠: GB2312 Code: 0xDCA9 ==> Row:60 Col:09 */
    {0x5821,0xB1A4}, /* 堡: GB2312 Code: 0xB1A4 ==> Row:17 Col:04 */
    {0x5824,0xB5CC}, /* 堤: GB2312 Code: 0xB5CC ==> Row:21 Col:44 */
    {0x582A,0xBFB0}, /* 堪: GB2312 Code: 0xBFB0 ==> Row:31 Col:16 */
    {0x5830,0xD1DF}, /* 堰: GB2312 Code: 0xD1DF ==> Row:49 Col:63 */
    {0x5835,0xB6C2}, /* 堵: GB2312 Code: 0xB6C2 ==> Row:22 Col:34 */
    {0x5844,0xDCA8}, /* 塄: GB2312 Code: 0xDCA8 ==> Row:60 Col:08 */
    {0x584C,0xCBFA}, /* 塌: GB2312 Code: 0xCBFA ==> Row:43 Col:90 */
    {0x584D,0xEBF3}, /* 塍: GB2312 Code: 0xEBF3 ==> Row:75 Col:83 */
    {0x5851,0xCBDC}, /* 塑: GB2312 Code: 0xCBDC ==> Row:43 Col:60 */
    {0x5854,0xCBFE}, /* 塔: GB2312 Code: 0xCBFE ==> Row:43 Col:94 */
    {0x5858,0xCCC1}, /* 塘: GB2312 Code: 0xCCC1 ==> Row:44 Col:33 */
    {0x585E,0xC8FB}, /* 塞: GB2312 Code: 0xC8FB ==> Row:40 Col:91 */
    {0x5865,0xDCAA}, /* 塥: GB2312 Code: 0xDCAA ==> Row:60 Col:10 */
    {0x586B,0xCCEE}, /* 填: GB2312 Code: 0xCCEE ==> Row:44 Col:78 */
    {0x586C,0xDCAB}, /* 塬: GB2312 Code: 0xDCAB ==> Row:60 Col:11 */
    {0x587E,0xDBD3}, /* 塾: GB2312 Code: 0xDBD3 ==> Row:59 Col:51 */
    {0x5880,0xDCAF}, /* 墀: GB2312 Code: 0xDCAF ==> Row:60 Col:15 */
    {0x5881,0xDCAC}, /* 墁: GB2312 Code: 0xDCAC ==> Row:60 Col:12 */
    {0x5883,0xBEB3}, /* 境: GB2312 Code: 0xBEB3 ==> Row:30 Col:19 */
    {0x5885,0xCAFB}, /* 墅: GB2312 Code: 0xCAFB ==> Row:42 Col:91 */
    {0x5889,0xDCAD}, /* 墉: GB2312 Code: 0xDCAD ==> Row:60 Col:13 */
    {0x5892,0xC9CA}, /* 墒: GB2312 Code: 0xC9CA ==> Row:41 Col:42 */
    {0x5893,0xC4B9}, /* 墓: GB2312 Code: 0xC4B9 ==> Row:36 Col:25 */
    {0x5899,0xC7BD}, /* 墙: GB2312 Code: 0xC7BD ==> Row:39 Col:29 */
    {0x589A,0xDCAE}, /* 墚: GB2312 Code: 0xDCAE ==> Row:60 Col:14 */
    {0x589E,0xD4F6}, /* 增: GB2312 Code: 0xD4F6 ==> Row:52 Col:86 */
    {0x589F,0xD0E6}, /* 墟: GB2312 Code: 0xD0E6 ==> Row:48 Col:70 */
    {0x58A8,0xC4AB}, /* 墨: GB2312 Code: 0xC4AB ==> Row:36 Col:11 */
    {0x58A9,0xB6D5}, /* 墩: GB2312 Code: 0xB6D5 ==> Row:22 Col:53 */
    {0x58BC,0xDBD4}, /* 墼: GB2312 Code: 0xDBD4 ==> Row:59 Col:52 */
    {0x58C1,0xB1DA}, /* 壁: GB2312 Code: 0xB1DA ==> Row:17 Col:58 */
    {0x58C5,0xDBD5}, /* 壅: GB2312 Code: 0xDBD5 ==> Row:59 Col:53 */
    {0x58D1,0xDBD6}, /* 壑: GB2312 Code: 0xDBD6 ==> Row:59 Col:54 */
    {0x58D5,0xBABE}, /* 壕: GB2312 Code: 0xBABE ==> Row:26 Col:30 */
    {0x58E4,0xC8C0}, /* 壤: GB2312 Code: 0xC8C0 ==> Row:40 Col:32 */
    {0x58EB,0xCABF}, /* 士: GB2312 Code: 0xCABF ==> Row:42 Col:31 */
    {0x58EC,0xC8C9}, /* 壬: GB2312 Code: 0xC8C9 ==> Row:40 Col:41 */
    {0x58EE,0xD7B3}, /* 壮: GB2312 Code: 0xD7B3 ==> Row:55 Col:19 */
    {0x58F0,0xC9F9}, /* 声: GB2312 Code: 0xC9F9 ==> Row:41 Col:89 */
    {0x58F3,0xBFC7}, /* 壳: GB2312 Code: 0xBFC7 ==> Row:31 Col:39 */
    {0x58F6,0xBAF8}, /* 壶: GB2312 Code: 0xBAF8 ==> Row:26 Col:88 */
    {0x58F9,0xD2BC}, /* 壹: GB2312 Code: 0xD2BC ==> Row:50 Col:28 */
    {0x5902,0xE2BA}, /* 夂: GB2312 Code: 0xE2BA ==> Row:66 Col:26 */
    {0x5904,0xB4A6}, /* 处: GB2312 Code: 0xB4A6 ==> Row:20 Col:06 */
    {0x5907,0xB1B8}, /* 备: GB2312 Code: 0xB1B8 ==> Row:17 Col:24 */
    {0x590D,0xB8B4}, /* 复: GB2312 Code: 0xB8B4 ==> Row:24 Col:20 */
    {0x590F,0xCFC4}, /* 夏: GB2312 Code: 0xCFC4 ==> Row:47 Col:36 */
    {0x5914,0xD9E7}, /* 夔: GB2312 Code: 0xD9E7 ==> Row:57 Col:71 */
    {0x5915,0xCFA6}, /* 夕: GB2312 Code: 0xCFA6 ==> Row:47 Col:06 */
    {0x5916,0xCDE2}, /* 外: GB2312 Code: 0xCDE2 ==> Row:45 Col:66 */
    {0x5919,0xD9ED}, /* 夙: GB2312 Code: 0xD9ED ==> Row:57 Col:77 */
    {0x591A,0xB6E0}, /* 多: GB2312 Code: 0xB6E0 ==> Row:22 Col:64 */
    {0x591C,0xD2B9}, /* 夜: GB2312 Code: 0xD2B9 ==> Row:50 Col:25 */
    {0x591F,0xB9BB}, /* 够: GB2312 Code: 0xB9BB ==> Row:25 Col:27 */
    {0x5924,0xE2B9}, /* 夤: GB2312 Code: 0xE2B9 ==> Row:66 Col:25 */
    {0x5925,0xE2B7}, /* 夥: GB2312 Code: 0xE2B7 ==> Row:66 Col:23 */
    {0x5927,0xB4F3}, /* 大: GB2312 Code: 0xB4F3 ==> Row:20 Col:83 */
    {0x5929,0xCCEC}, /* 天: GB2312 Code: 0xCCEC ==> Row:44 Col:76 */
    {0x592A,0xCCAB}, /* 太: GB2312 Code: 0xCCAB ==> Row:44 Col:11 */
    {0x592B,0xB7F2}, /* 夫: GB2312 Code: 0xB7F2 ==> Row:23 Col:82 */
    {0x592D,0xD8B2}, /* 夭: GB2312 Code: 0xD8B2 ==> Row:56 Col:18 */
    {0x592E,0xD1EB}, /* 央: GB2312 Code: 0xD1EB ==> Row:49 Col:75 */
    {0x592F,0xBABB}, /* 夯: GB2312 Code: 0xBABB ==> Row:26 Col:27 */
    {0x5931,0xCAA7}, /* 失: GB2312 Code: 0xCAA7 ==> Row:42 Col:07 */
    {0x5934,0xCDB7}, /* 头: GB2312 Code: 0xCDB7 ==> Row:45 Col:23 */
    {0x5937,0xD2C4}, /* 夷: GB2312 Code: 0xD2C4 ==> Row:50 Col:36 */
    {0x5938,0xBFE4}, /* 夸: GB2312 Code: 0xBFE4 ==> Row:31 Col:68 */
    {0x5939,0xBCD0}, /* 夹: GB2312 Code: 0xBCD0 ==> Row:28 Col:48 */
    {0x593A,0xB6E1}, /* 夺: GB2312 Code: 0xB6E1 ==> Row:22 Col:65 */
    {0x593C,0xDEC5}, /* 夼: GB2312 Code: 0xDEC5 ==> Row:62 Col:37 */
    {0x5941,0xDEC6}, /* 奁: GB2312 Code: 0xDEC6 ==> Row:62 Col:38 */
    {0x5942,0xDBBC}, /* 奂: GB2312 Code: 0xDBBC ==> Row:59 Col:28 */
    {0x5944,0xD1D9}, /* 奄: GB2312 Code: 0xD1D9 ==> Row:49 Col:57 */
    {0x5947,0xC6E6}, /* 奇: GB2312 Code: 0xC6E6 ==> Row:38 Col:70 */
    {0x5948,0xC4CE}, /* 奈: GB2312 Code: 0xC4CE ==> Row:36 Col:46 */
    {0x5949,0xB7EE}, /* 奉: GB2312 Code: 0xB7EE ==> Row:23 Col:78 */
    {0x594B,0xB7DC}, /* 奋: GB2312 Code: 0xB7DC ==> Row:23 Col:60 */
    {0x594E,0xBFFC}, /* 奎: GB2312 Code: 0xBFFC ==> Row:31 Col:92 */
    {0x594F,0xD7E0}, /* 奏: GB2312 Code: 0xD7E0 ==> Row:55 Col:64 */
    {0x5951,0xC6F5}, /* 契: GB2312 Code: 0xC6F5 ==> Row:38 Col:85 */
    {0x5954,0xB1BC}, /* 奔: GB2312 Code: 0xB1BC ==> Row:17 Col:28 */
    {0x5955,0xDEC8}, /* 奕: GB2312 Code: 0xDEC8 ==> Row:62 Col:40 */
    {0x5956,0xBDB1}, /* 奖: GB2312 Code: 0xBDB1 ==> Row:29 Col:17 */
    {0x5957,0xCCD7}, /* 套: GB2312 Code: 0xCCD7 ==> Row:44 Col:55 */
    {0x5958,0xDECA}, /* 奘: GB2312 Code: 0xDECA ==> Row:62 Col:42 */
    {0x595A,0xDEC9}, /* 奚: GB2312 Code: 0xDEC9 ==> Row:62 Col:41 */
    {0x5960,0xB5EC}, /* 奠: GB2312 Code: 0xB5EC ==> Row:21 Col:76 */
    {0x5962,0xC9DD}, /* 奢: GB2312 Code: 0xC9DD ==> Row:41 Col:61 */
    {0x5965,0xB0C2}, /* 奥: GB2312 Code: 0xB0C2 ==> Row:16 Col:34 */
    {0x5973,0xC5AE}, /* 女: GB2312 Code: 0xC5AE ==> Row:37 Col:14 */
    {0x5974,0xC5AB}, /* 奴: GB2312 Code: 0xC5AB ==> Row:37 Col:11 */
    {0x5976,0xC4CC}, /* 奶: GB2312 Code: 0xC4CC ==> Row:36 Col:44 */
    {0x5978,0xBCE9}, /* 奸: GB2312 Code: 0xBCE9 ==> Row:28 Col:73 */
    {0x5979,0xCBFD}, /* 她: GB2312 Code: 0xCBFD ==> Row:43 Col:93 */
    {0x597D,0xBAC3}, /* 好: GB2312 Code: 0xBAC3 ==> Row:26 Col:35 */
    {0x5981,0xE5F9}, /* 妁: GB2312 Code: 0xE5F9 ==> Row:69 Col:89 */
    {0x5982,0xC8E7}, /* 如: GB2312 Code: 0xC8E7 ==> Row:40 Col:71 */
    {0x5983,0xE5FA}, /* 妃: GB2312 Code: 0xE5FA ==> Row:69 Col:90 */
    {0x5984,0xCDFD}, /* 妄: GB2312 Code: 0xCDFD ==> Row:45 Col:93 */
    {0x5986,0xD7B1}, /* 妆: GB2312 Code: 0xD7B1 ==> Row:55 Col:17 */
    {0x5987,0xB8BE}, /* 妇: GB2312 Code: 0xB8BE ==> Row:24 Col:30 */
    {0x5988,0xC2E8}, /* 妈: GB2312 Code: 0xC2E8 ==> Row:34 Col:72 */
    {0x598A,0xC8D1}, /* 妊: GB2312 Code: 0xC8D1 ==> Row:40 Col:49 */
    {0x598D,0xE5FB}, /* 妍: GB2312 Code: 0xE5FB ==> Row:69 Col:91 */
    {0x5992,0xB6CA}, /* 妒: GB2312 Code: 0xB6CA ==> Row:22 Col:42 */
    {0x5993,0xBCCB}, /* 妓: GB2312 Code: 0xBCCB ==> Row:28 Col:43 */
    {0x5996,0xD1FD}, /* 妖: GB2312 Code: 0xD1FD ==> Row:49 Col:93 */
    {0x5997,0xE6A1}, /* 妗: GB2312 Code: 0xE6A1 ==> Row:70 Col:01 */
    {0x5999,0xC3EE}, /* 妙: GB2312 Code: 0xC3EE ==> Row:35 Col:78 */
    {0x599E,0xE6A4}, /* 妞: GB2312 Code: 0xE6A4 ==> Row:70 Col:04 */
    {0x59A3,0xE5FE}, /* 妣: GB2312 Code: 0xE5FE ==> Row:69 Col:94 */
    {0x59A4,0xE6A5}, /* 妤: GB2312 Code: 0xE6A5 ==> Row:70 Col:05 */
    {0x59A5,0xCDD7}, /* 妥: GB2312 Code: 0xCDD7 ==> Row:45 Col:55 */
    {0x59A8,0xB7C1}, /* 妨: GB2312 Code: 0xB7C1 ==> Row:23 Col:33 */
    {0x59A9,0xE5FC}, /* 妩: GB2312 Code: 0xE5FC ==> Row:69 Col:92 */
    {0x59AA,0xE5FD}, /* 妪: GB2312 Code: 0xE5FD ==> Row:69 Col:93 */
    {0x59AB,0xE6A3}, /* 妫: GB2312 Code: 0xE6A3 ==> Row:70 Col:03 */
    {0x59AE,0xC4DD}, /* 妮: GB2312 Code: 0xC4DD ==> Row:36 Col:61 */
    {0x59AF,0xE6A8}, /* 妯: GB2312 Code: 0xE6A8 ==> Row:70 Col:08 */
    {0x59B2,0xE6A7}, /* 妲: GB2312 Code: 0xE6A7 ==> Row:70 Col:07 */
    {0x59B9,0xC3C3}, /* 妹: GB2312 Code: 0xC3C3 ==> Row:35 Col:35 */
    {0x59BB,0xC6DE}, /* 妻: GB2312 Code: 0xC6DE ==> Row:38 Col:62 */
    {0x59BE,0xE6AA}, /* 妾: GB2312 Code: 0xE6AA ==> Row:70 Col:10 */
    {0x59C6,0xC4B7}, /* 姆: GB2312 Code: 0xC4B7 ==> Row:36 Col:23 */
    {0x59CA,0xE6A2}, /* 姊: GB2312 Code: 0xE6A2 ==> Row:70 Col:02 */
    {0x59CB,0xCABC}, /* 始: GB2312 Code: 0xCABC ==> Row:42 Col:28 */
    {0x59D0,0xBDE3}, /* 姐: GB2312 Code: 0xBDE3 ==> Row:29 Col:67 */
    {0x59D1,0xB9C3}, /* 姑: GB2312 Code: 0xB9C3 ==> Row:25 Col:35 */
    {0x59D2,0xE6A6}, /* 姒: GB2312 Code: 0xE6A6 ==> Row:70 Col:06 */
    {0x59D3,0xD0D5}, /* 姓: GB2312 Code: 0xD0D5 ==> Row:48 Col:53 */
    {0x59D4,0xCEAF}, /* 委: GB2312 Code: 0xCEAF ==> Row:46 Col:15 */
    {0x59D7,0xE6A9}, /* 姗: GB2312 Code: 0xE6A9 ==> Row:70 Col:09 */
    {0x59D8,0xE6B0}, /* 姘: GB2312 Code: 0xE6B0 ==> Row:70 Col:16 */
    {0x59DA,0xD2A6}, /* 姚: GB2312 Code: 0xD2A6 ==> Row:50 Col:06 */
    {0x59DC,0xBDAA}, /* 姜: GB2312 Code: 0xBDAA ==> Row:29 Col:10 */
    {0x59DD,0xE6AD}, /* 姝: GB2312 Code: 0xE6AD ==> Row:70 Col:13 */
    {0x59E3,0xE6AF}, /* 姣: GB2312 Code: 0xE6AF ==> Row:70 Col:15 */
    {0x59E5,0xC0D1}, /* 姥: GB2312 Code: 0xC0D1 ==> Row:32 Col:49 */
    {0x59E8,0xD2CC}, /* 姨: GB2312 Code: 0xD2CC ==> Row:50 Col:44 */
    {0x59EC,0xBCA7}, /* 姬: GB2312 Code: 0xBCA7 ==> Row:28 Col:07 */
    {0x59F9,0xE6B1}, /* 姹: GB2312 Code: 0xE6B1 ==> Row:70 Col:17 */
    {0x59FB,0xD2F6}, /* 姻: GB2312 Code: 0xD2F6 ==> Row:50 Col:86 */
    {0x59FF,0xD7CB}, /* 姿: GB2312 Code: 0xD7CB ==> Row:55 Col:43 */
    {0x5A01,0xCDFE}, /* 威: GB2312 Code: 0xCDFE ==> Row:45 Col:94 */
    {0x5A03,0xCDDE}, /* 娃: GB2312 Code: 0xCDDE ==> Row:45 Col:62 */
    {0x5A04,0xC2A6}, /* 娄: GB2312 Code: 0xC2A6 ==> Row:34 Col:06 */
    {0x5A05,0xE6AB}, /* 娅: GB2312 Code: 0xE6AB ==> Row:70 Col:11 */
    {0x5A06,0xE6AC}, /* 娆: GB2312 Code: 0xE6AC ==> Row:70 Col:12 */
    {0x5A07,0xBDBF}, /* 娇: GB2312 Code: 0xBDBF ==> Row:29 Col:31 */
    {0x5A08,0xE6AE}, /* 娈: GB2312 Code: 0xE6AE ==> Row:70 Col:14 */
    {0x5A09,0xE6B3}, /* 娉: GB2312 Code: 0xE6B3 ==> Row:70 Col:19 */
    {0x5A0C,0xE6B2}, /* 娌: GB2312 Code: 0xE6B2 ==> Row:70 Col:18 */
    {0x5A11,0xE6B6}, /* 娑: GB2312 Code: 0xE6B6 ==> Row:70 Col:22 */
    {0x5A13,0xE6B8}, /* 娓: GB2312 Code: 0xE6B8 ==> Row:70 Col:24 */
    {0x5A18,0xC4EF}, /* 娘: GB2312 Code: 0xC4EF ==> Row:36 Col:79 */
    {0x5A1C,0xC4C8}, /* 娜: GB2312 Code: 0xC4C8 ==> Row:36 Col:40 */
    {0x5A1F,0xBEEA}, /* 娟: GB2312 Code: 0xBEEA ==> Row:30 Col:74 */
    {0x5A20,0xC9EF}, /* 娠: GB2312 Code: 0xC9EF ==> Row:41 Col:79 */
    {0x5A23,0xE6B7}, /* 娣: GB2312 Code: 0xE6B7 ==> Row:70 Col:23 */
    {0x5A25,0xB6F0}, /* 娥: GB2312 Code: 0xB6F0 ==> Row:22 Col:80 */
    {0x5A29,0xC3E4}, /* 娩: GB2312 Code: 0xC3E4 ==> Row:35 Col:68 */
    {0x5A31,0xD3E9}, /* 娱: GB2312 Code: 0xD3E9 ==> Row:51 Col:73 */
    {0x5A32,0xE6B4}, /* 娲: GB2312 Code: 0xE6B4 ==> Row:70 Col:20 */
    {0x5A34,0xE6B5}, /* 娴: GB2312 Code: 0xE6B5 ==> Row:70 Col:21 */
    {0x5A36,0xC8A2}, /* 娶: GB2312 Code: 0xC8A2 ==> Row:40 Col:02 */
    {0x5A3C,0xE6BD}, /* 娼: GB2312 Code: 0xE6BD ==> Row:70 Col:29 */
    {0x5A40,0xE6B9}, /* 婀: GB2312 Code: 0xE6B9 ==> Row:70 Col:25 */
    {0x5A46,0xC6C5}, /* 婆: GB2312 Code: 0xC6C5 ==> Row:38 Col:37 */
    {0x5A49,0xCDF1}, /* 婉: GB2312 Code: 0xCDF1 ==> Row:45 Col:81 */
    {0x5A4A,0xE6BB}, /* 婊: GB2312 Code: 0xE6BB ==> Row:70 Col:27 */
    {0x5A55,0xE6BC}, /* 婕: GB2312 Code: 0xE6BC ==> Row:70 Col:28 */
    {0x5A5A,0xBBE9}, /* 婚: GB2312 Code: 0xBBE9 ==> Row:27 Col:73 */
    {0x5A62,0xE6BE}, /* 婢: GB2312 Code: 0xE6BE ==> Row:70 Col:30 */
    {0x5A67,0xE6BA}, /* 婧: GB2312 Code: 0xE6BA ==> Row:70 Col:26 */
    {0x5A6A,0xC0B7}, /* 婪: GB2312 Code: 0xC0B7 ==> Row:32 Col:23 */
    {0x5A74,0xD3A4}, /* 婴: GB2312 Code: 0xD3A4 ==> Row:51 Col:04 */
    {0x5A75,0xE6BF}, /* 婵: GB2312 Code: 0xE6BF ==> Row:70 Col:31 */
    {0x5A76,0xC9F4}, /* 婶: GB2312 Code: 0xC9F4 ==> Row:41 Col:84 */
    {0x5A77,0xE6C3}, /* 婷: GB2312 Code: 0xE6C3 ==> Row:70 Col:35 */
    {0x5A7A,0xE6C4}, /* 婺: GB2312 Code: 0xE6C4 ==> Row:70 Col:36 */
    {0x5A7F,0xD0F6}, /* 婿: GB2312 Code: 0xD0F6 ==> Row:48 Col:86 */
    {0x5A92,0xC3BD}, /* 媒: GB2312 Code: 0xC3BD ==> Row:35 Col:29 */
    {0x5A9A,0xC3C4}, /* 媚: GB2312 Code: 0xC3C4 ==> Row:35 Col:36 */
    {0x5A9B,0xE6C2}, /* 媛: GB2312 Code: 0xE6C2 ==> Row:70 Col:34 */
    {0x5AAA,0xE6C1}, /* 媪: GB2312 Code: 0xE6C1 ==> Row:70 Col:33 */
    {0x5AB2,0xE6C7}, /* 媲: GB2312 Code: 0xE6C7 ==> Row:70 Col:39 */
    {0x5AB3,0xCFB1}, /* 媳: GB2312 Code: 0xCFB1 ==> Row:47 Col:17 */
    {0x5AB5,0xEBF4}, /* 媵: GB2312 Code: 0xEBF4 ==> Row:75 Col:84 */
    {0x5AB8,0xE6CA}, /* 媸: GB2312 Code: 0xE6CA ==> Row:70 Col:42 */
    {0x5ABE,0xE6C5}, /* 媾: GB2312 Code: 0xE6C5 ==> Row:70 Col:37 */
    {0x5AC1,0xBCDE}, /* 嫁: GB2312 Code: 0xBCDE ==> Row:28 Col:62 */
    {0x5AC2,0xC9A9}, /* 嫂: GB2312 Code: 0xC9A9 ==> Row:41 Col:09 */
    {0x5AC9,0xBCB5}, /* 嫉: GB2312 Code: 0xBCB5 ==> Row:28 Col:21 */
    {0x5ACC,0xCFD3}, /* 嫌: GB2312 Code: 0xCFD3 ==> Row:47 Col:51 */
    {0x5AD2,0xE6C8}, /* 嫒: GB2312 Code: 0xE6C8 ==> Row:70 Col:40 */
    {0x5AD4,0xE6C9}, /* 嫔: GB2312 Code: 0xE6C9 ==> Row:70 Col:41 */
    {0x5AD6,0xE6CE}, /* 嫖: GB2312 Code: 0xE6CE ==> Row:70 Col:46 */
    {0x5AD8,0xE6D0}, /* 嫘: GB2312 Code: 0xE6D0 ==> Row:70 Col:48 */
    {0x5ADC,0xE6D1}, /* 嫜: GB2312 Code: 0xE6D1 ==> Row:70 Col:49 */
    {0x5AE0,0xE6CB}, /* 嫠: GB2312 Code: 0xE6CB ==> Row:70 Col:43 */
    {0x5AE1,0xB5D5}, /* 嫡: GB2312 Code: 0xB5D5 ==> Row:21 Col:53 */
    {0x5AE3,0xE6CC}, /* 嫣: GB2312 Code: 0xE6CC ==> Row:70 Col:44 */
    {0x5AE6,0xE6CF}, /* 嫦: GB2312 Code: 0xE6CF ==> Row:70 Col:47 */
    {0x5AE9,0xC4DB}, /* 嫩: GB2312 Code: 0xC4DB ==> Row:36 Col:59 */
    {0x5AEB,0xE6C6}, /* 嫫: GB2312 Code: 0xE6C6 ==> Row:70 Col:38 */
    {0x5AF1,0xE6CD}, /* 嫱: GB2312 Code: 0xE6CD ==> Row:70 Col:45 */
    {0x5B09,0xE6D2}, /* 嬉: GB2312 Code: 0xE6D2 ==> Row:70 Col:50 */
    {0x5B16,0xE6D4}, /* 嬖: GB2312 Code: 0xE6D4 ==> Row:70 Col:52 */
    {0x5B17,0xE6D3}, /* 嬗: GB2312 Code: 0xE6D3 ==> Row:70 Col:51 */
    {0x5B32,0xE6D5}, /* 嬲: GB2312 Code: 0xE6D5 ==> Row:70 Col:53 */
    {0x5B34,0xD9F8}, /* 嬴: GB2312 Code: 0xD9F8 ==> Row:57 Col:88 */
    {0x5B37,0xE6D6}, /* 嬷: GB2312 Code: 0xE6D6 ==> Row:70 Col:54 */
    {0x5B40,0xE6D7}, /* 孀: GB2312 Code: 0xE6D7 ==> Row:70 Col:55 */
    {0x5B50,0xD7D3}, /* 子: GB2312 Code: 0xD7D3 ==> Row:55 Col:51 */
    {0x5B51,0xE6DD}, /* 孑: GB2312 Code: 0xE6DD ==> Row:70 Col:61 */
    {0x5B53,0xE6DE}, /* 孓: GB2312 Code: 0xE6DE ==> Row:70 Col:62 */
    {0x5B54,0xBFD7}, /* 孔: GB2312 Code: 0xBFD7 ==> Row:31 Col:55 */
    {0x5B55,0xD4D0}, /* 孕: GB2312 Code: 0xD4D0 ==> Row:52 Col:48 */
    {0x5B57,0xD7D6}, /* 字: GB2312 Code: 0xD7D6 ==> Row:55 Col:54 */
    {0x5B58,0xB4E6}, /* 存: GB2312 Code: 0xB4E6 ==> Row:20 Col:70 */
    {0x5B59,0xCBEF}, /* 孙: GB2312 Code: 0xCBEF ==> Row:43 Col:79 */
    {0x5B5A,0xE6DA}, /* 孚: GB2312 Code: 0xE6DA ==> Row:70 Col:58 */
    {0x5B5B,0xD8C3}, /* 孛: GB2312 Code: 0xD8C3 ==> Row:56 Col:35 */
    {0x5B5C,0xD7CE}, /* 孜: GB2312 Code: 0xD7CE ==> Row:55 Col:46 */
    {0x5B5D,0xD0A2}, /* 孝: GB2312 Code: 0xD0A2 ==> Row:48 Col:02 */
    {0x5B5F,0xC3CF}, /* 孟: GB2312 Code: 0xC3CF ==> Row:35 Col:47 */
    {0x5B62,0xE6DF}, /* 孢: GB2312 Code: 0xE6DF ==> Row:70 Col:63 */
    {0x5B63,0xBCBE}, /* 季: GB2312 Code: 0xBCBE ==> Row:28 Col:30 */
    {0x5B64,0xB9C2}, /* 孤: GB2312 Code: 0xB9C2 ==> Row:25 Col:34 */
    {0x5B65,0xE6DB}, /* 孥: GB2312 Code: 0xE6DB ==> Row:70 Col:59 */
    {0x5B66,0xD1A7}, /* 学: GB2312 Code: 0xD1A7 ==> Row:49 Col:07 */
    {0x5B69,0xBAA2}, /* 孩: GB2312 Code: 0xBAA2 ==> Row:26 Col:02 */
    {0x5B6A,0xC2CF}, /* 孪: GB2312 Code: 0xC2CF ==> Row:34 Col:47 */
    {0x5B6C,0xD8AB}, /* 孬: GB2312 Code: 0xD8AB ==> Row:56 Col:11 */
    {0x5B70,0xCAEB}, /* 孰: GB2312 Code: 0xCAEB ==> Row:42 Col:75 */
    {0x5B71,0xE5EE}, /* 孱: GB2312 Code: 0xE5EE ==> Row:69 Col:78 */
    {0x5B73,0xE6DC}, /* 孳: GB2312 Code: 0xE6DC ==> Row:70 Col:60 */
    {0x5B75,0xB7F5}, /* 孵: GB2312 Code: 0xB7F5 ==> Row:23 Col:85 */
    {0x5B7A,0xC8E6}, /* 孺: GB2312 Code: 0xC8E6 ==> Row:40 Col:70 */
    {0x5B7D,0xC4F5}, /* 孽: GB2312 Code: 0xC4F5 ==> Row:36 Col:85 */
    {0x5B80,0xE5B2}, /* 宀: GB2312 Code: 0xE5B2 ==> Row:69 Col:18 */
    {0x5B81,0xC4FE}, /* 宁: GB2312 Code: 0xC4FE ==> Row:36 Col:94 */
    {0x5B83,0xCBFC}, /* 它: GB2312 Code: 0xCBFC ==> Row:43 Col:92 */
    {0x5B84,0xE5B3}, /* 宄: GB2312 Code: 0xE5B3 ==> Row:69 Col:19 */
    {0x5B85,0xD5AC}, /* 宅: GB2312 Code: 0xD5AC ==> Row:53 Col:12 */
    {0x5B87,0xD3EE}, /* 宇: GB2312 Code: 0xD3EE ==> Row:51 Col:78 */
    {0x5B88,0xCAD8}, /* 守: GB2312 Code: 0xCAD8 ==> Row:42 Col:56 */
    {0x5B89,0xB0B2}, /* 安: GB2312 Code: 0xB0B2 ==> Row:16 Col:18 */
    {0x5B8B,0xCBCE}, /* 宋: GB2312 Code: 0xCBCE ==> Row:43 Col:46 */
    {0x5B8C,0xCDEA}, /* 完: GB2312 Code: 0xCDEA ==> Row:45 Col:74 */
    {0x5B8F,0xBAEA}, /* 宏: GB2312 Code: 0xBAEA ==> Row:26 Col:74 */
    {0x5B93,0xE5B5}, /* 宓: GB2312 Code: 0xE5B5 ==> Row:69 Col:21 */
    {0x5B95,0xE5B4}, /* 宕: GB2312 Code: 0xE5B4 ==> Row:69 Col:20 */
    {0x5B97,0xD7DA}, /* 宗: GB2312 Code: 0xD7DA ==> Row:55 Col:58 */
    {0x5B98,0xB9D9}, /* 官: GB2312 Code: 0xB9D9 ==> Row:25 Col:57 */
    {0x5B99,0xD6E6}, /* 宙: GB2312 Code: 0xD6E6 ==> Row:54 Col:70 */
    {0x5B9A,0xB6A8}, /* 定: GB2312 Code: 0xB6A8 ==> Row:22 Col:08 */
    {0x5B9B,0xCDF0}, /* 宛: GB2312 Code: 0xCDF0 ==> Row:45 Col:80 */
    {0x5B9C,0xD2CB}, /* 宜: GB2312 Code: 0xD2CB ==> Row:50 Col:43 */
    {0x5B9D,0xB1A6}, /* 宝: GB2312 Code: 0xB1A6 ==> Row:17 Col:06 */
    {0x5B9E,0xCAB5}, /* 实: GB2312 Code: 0xCAB5 ==> Row:42 Col:21 */
    {0x5BA0,0xB3E8}, /* 宠: GB2312 Code: 0xB3E8 ==> Row:19 Col:72 */
    {0x5BA1,0xC9F3}, /* 审: GB2312 Code: 0xC9F3 ==> Row:41 Col:83 */
    {0x5BA2,0xBFCD}, /* 客: GB2312 Code: 0xBFCD ==> Row:31 Col:45 */
    {0x5BA3,0xD0FB}, /* 宣: GB2312 Code: 0xD0FB ==> Row:48 Col:91 */
    {0x5BA4,0xCAD2}, /* 室: GB2312 Code: 0xCAD2 ==> Row:42 Col:50 */
    {0x5BA5,0xE5B6}, /* 宥: GB2312 Code: 0xE5B6 ==> Row:69 Col:22 */
    {0x5BA6,0xBBC2}, /* 宦: GB2312 Code: 0xBBC2 ==> Row:27 Col:34 */
    {0x5BAA,0xCFDC}, /* 宪: GB2312 Code: 0xCFDC ==> Row:47 Col:60 */
    {0x5BAB,0xB9AC}, /* 宫: GB2312 Code: 0xB9AC ==> Row:25 Col:12 */
    {0x5BB0,0xD4D7}, /* 宰: GB2312 Code: 0xD4D7 ==> Row:52 Col:55 */
    {0x5BB3,0xBAA6}, /* 害: GB2312 Code: 0xBAA6 ==> Row:26 Col:06 */
    {0x5BB4,0xD1E7}, /* 宴: GB2312 Code: 0xD1E7 ==> Row:49 Col:71 */
    {0x5BB5,0xCFFC}, /* 宵: GB2312 Code: 0xCFFC ==> Row:47 Col:92 */
    {0x5BB6,0xBCD2}, /* 家: GB2312 Code: 0xBCD2 ==> Row:28 Col:50 */
    {0x5BB8,0xE5B7}, /* 宸: GB2312 Code: 0xE5B7 ==> Row:69 Col:23 */
    {0x5BB9,0xC8DD}, /* 容: GB2312 Code: 0xC8DD ==> Row:40 Col:61 */
    {0x5BBD,0xBFED}, /* 宽: GB2312 Code: 0xBFED ==> Row:31 Col:77 */
    {0x5BBE,0xB1F6}, /* 宾: GB2312 Code: 0xB1F6 ==> Row:17 Col:86 */
    {0x5BBF,0xCBDE}, /* 宿: GB2312 Code: 0xCBDE ==> Row:43 Col:62 */
    {0x5BC2,0xBCC5}, /* 寂: GB2312 Code: 0xBCC5 ==> Row:28 Col:37 */
    {0x5BC4,0xBCC4}, /* 寄: GB2312 Code: 0xBCC4 ==> Row:28 Col:36 */
    {0x5BC5,0xD2FA}, /* 寅: GB2312 Code: 0xD2FA ==> Row:50 Col:90 */
    {0x5BC6,0xC3DC}, /* 密: GB2312 Code: 0xC3DC ==> Row:35 Col:60 */
    {0x5BC7,0xBFDC}, /* 寇: GB2312 Code: 0xBFDC ==> Row:31 Col:60 */
    {0x5BCC,0xB8BB}, /* 富: GB2312 Code: 0xB8BB ==> Row:24 Col:27 */
    {0x5BD0,0xC3C2}, /* 寐: GB2312 Code: 0xC3C2 ==> Row:35 Col:34 */
    {0x5BD2,0xBAAE}, /* 寒: GB2312 Code: 0xBAAE ==> Row:26 Col:14 */
    {0x5BD3,0xD4A2}, /* 寓: GB2312 Code: 0xD4A2 ==> Row:52 Col:02 */
    {0x5BDD,0xC7DE}, /* 寝: GB2312 Code: 0xC7DE ==> Row:39 Col:62 */
    {0x5BDE,0xC4AF}, /* 寞: GB2312 Code: 0xC4AF ==> Row:36 Col:15 */
    {0x5BDF,0xB2EC}, /* 察: GB2312 Code: 0xB2EC ==> Row:18 Col:76 */
    {0x5BE1,0xB9D1}, /* 寡: GB2312 Code: 0xB9D1 ==> Row:25 Col:49 */
    {0x5BE4,0xE5BB}, /* 寤: GB2312 Code: 0xE5BB ==> Row:69 Col:27 */
    {0x5BE5,0xC1C8}, /* 寥: GB2312 Code: 0xC1C8 ==> Row:33 Col:40 */
    {0x5BE8,0xD5AF}, /* 寨: GB2312 Code: 0xD5AF ==> Row:53 Col:15 */
    {0x5BEE,0xE5BC}, /* 寮: GB2312 Code: 0xE5BC ==> Row:69 Col:28 */
    {0x5BF0,0xE5BE}, /* 寰: GB2312 Code: 0xE5BE ==> Row:69 Col:30 */
    {0x5BF8,0xB4E7}, /* 寸: GB2312 Code: 0xB4E7 ==> Row:20 Col:71 */
    {0x5BF9,0xB6D4}, /* 对: GB2312 Code: 0xB6D4 ==> Row:22 Col:52 */
    {0x5BFA,0xCBC2}, /* 寺: GB2312 Code: 0xCBC2 ==> Row:43 Col:34 */
    {0x5BFB,0xD1B0}, /* 寻: GB2312 Code: 0xD1B0 ==> Row:49 Col:16 */
    {0x5BFC,0xB5BC}, /* 导: GB2312 Code: 0xB5BC ==> Row:21 Col:28 */
    {0x5BFF,0xCAD9}, /* 寿: GB2312 Code: 0xCAD9 ==> Row:42 Col:57 */
    {0x5C01,0xB7E2}, /* 封: GB2312 Code: 0xB7E2 ==> Row:23 Col:66 */
    {0x5C04,0xC9E4}, /* 射: GB2312 Code: 0xC9E4 ==> Row:41 Col:68 */
    {0x5C06,0xBDAB}, /* 将: GB2312 Code: 0xBDAB ==> Row:29 Col:11 */
    {0x5C09,0xCEBE}, /* 尉: GB2312 Code: 0xCEBE ==> Row:46 Col:30 */
    {0x5C0A,0xD7F0}, /* 尊: GB2312 Code: 0xD7F0 ==> Row:55 Col:80 */
    {0x5C0F,0xD0A1}, /* 小: GB2312 Code: 0xD0A1 ==> Row:48 Col:01 */
    {0x5C11,0xC9D9}, /* 少: GB2312 Code: 0xC9D9 ==> Row:41 Col:57 */
    {0x5C14,0xB6FB}, /* 尔: GB2312 Code: 0xB6FB ==> Row:22 Col:91 */
    {0x5C15,0xE6D8}, /* 尕: GB2312 Code: 0xE6D8 ==> Row:70 Col:56 */
    {0x5C16,0xBCE2}, /* 尖: GB2312 Code: 0xBCE2 ==> Row:28 Col:66 */
    {0x5C18,0xB3BE}, /* 尘: GB2312 Code: 0xB3BE ==> Row:19 Col:30 */
    {0x5C1A,0xC9D0}, /* 尚: GB2312 Code: 0xC9D0 ==> Row:41 Col:48 */
    {0x5C1C,0xE6D9}, /* 尜: GB2312 Code: 0xE6D9 ==> Row:70 Col:57 */
    {0x5C1D,0xB3A2}, /* 尝: GB2312 Code: 0xB3A2 ==> Row:19 Col:02 */
    {0x5C22,0xDECC}, /* 尢: GB2312 Code: 0xDECC ==> Row:62 Col:44 */
    {0x5C24,0xD3C8}, /* 尤: GB2312 Code: 0xD3C8 ==> Row:51 Col:40 */
    {0x5C25,0xDECD}, /* 尥: GB2312 Code: 0xDECD ==> Row:62 Col:45 */
    {0x5C27,0xD2A2}, /* 尧: GB2312 Code: 0xD2A2 ==> Row:50 Col:02 */
    {0x5C2C,0xDECE}, /* 尬: GB2312 Code: 0xDECE ==> Row:62 Col:46 */
    {0x5C31,0xBECD}, /* 就: GB2312 Code: 0xBECD ==> Row:30 Col:45 */
    {0x5C34,0xDECF}, /* 尴: GB2312 Code: 0xDECF ==> Row:62 Col:47 */
    {0x5C38,0xCAAC}, /* 尸: GB2312 Code: 0xCAAC ==> Row:42 Col:12 */
    {0x5C39,0xD2FC}, /* 尹: GB2312 Code: 0xD2FC ==> Row:50 Col:92 */
    {0x5C3A,0xB3DF}, /* 尺: GB2312 Code: 0xB3DF ==> Row:19 Col:63 */
    {0x5C3B,0xE5EA}, /* 尻: GB2312 Code: 0xE5EA ==> Row:69 Col:74 */
    {0x5C3C,0xC4E1}, /* 尼: GB2312 Code: 0xC4E1 ==> Row:36 Col:65 */
    {0x5C3D,0xBEA1}, /* 尽: GB2312 Code: 0xBEA1 ==> Row:30 Col:01 */
    {0x5C3E,0xCEB2}, /* 尾: GB2312 Code: 0xCEB2 ==> Row:46 Col:18 */
    {0x5C3F,0xC4F2}, /* 尿: GB2312 Code: 0xC4F2 ==> Row:36 Col:82 */
    {0x5C40,0xBED6}, /* 局: GB2312 Code: 0xBED6 ==> Row:30 Col:54 */
    {0x5C41,0xC6A8}, /* 屁: GB2312 Code: 0xC6A8 ==> Row:38 Col:08 */
    {0x5C42,0xB2E3}, /* 层: GB2312 Code: 0xB2E3 ==> Row:18 Col:67 */
    {0x5C45,0xBED3}, /* 居: GB2312 Code: 0xBED3 ==> Row:30 Col:51 */
    {0x5C48,0xC7FC}, /* 屈: GB2312 Code: 0xC7FC ==> Row:39 Col:92 */
    {0x5C49,0xCCEB}, /* 屉: GB2312 Code: 0xCCEB ==> Row:44 Col:75 */
    {0x5C4A,0xBDEC}, /* 届: GB2312 Code: 0xBDEC ==> Row:29 Col:76 */
    {0x5C4B,0xCEDD}, /* 屋: GB2312 Code: 0xCEDD ==> Row:46 Col:61 */
    {0x5C4E,0xCABA}, /* 屎: GB2312 Code: 0xCABA ==> Row:42 Col:26 */
    {0x5C4F,0xC6C1}, /* 屏: GB2312 Code: 0xC6C1 ==> Row:38 Col:33 */
    {0x5C50,0xE5EC}, /* 屐: GB2312 Code: 0xE5EC ==> Row:69 Col:76 */
    {0x5C51,0xD0BC}, /* 屑: GB2312 Code: 0xD0BC ==> Row:48 Col:28 */
    {0x5C55,0xD5B9}, /* 展: GB2312 Code: 0xD5B9 ==> Row:53 Col:25 */
    {0x5C59,0xE5ED}, /* 屙: GB2312 Code: 0xE5ED ==> Row:69 Col:77 */
    {0x5C5E,0xCAF4}, /* 属: GB2312 Code: 0xCAF4 ==> Row:42 Col:84 */
    {0x5C60,0xCDC0}, /* 屠: GB2312 Code: 0xCDC0 ==> Row:45 Col:32 */
    {0x5C61,0xC2C5}, /* 屡: GB2312 Code: 0xC2C5 ==> Row:34 Col:37 */
    {0x5C63,0xE5EF}, /* 屣: GB2312 Code: 0xE5EF ==> Row:69 Col:79 */
    {0x5C65,0xC2C4}, /* 履: GB2312 Code: 0xC2C4 ==> Row:34 Col:36 */
    {0x5C66,0xE5F0}, /* 屦: GB2312 Code: 0xE5F0 ==> Row:69 Col:80 */
    {0x5C6E,0xE5F8}, /* 屮: GB2312 Code: 0xE5F8 ==> Row:69 Col:88 */
    {0x5C6F,0xCDCD}, /* 屯: GB2312 Code: 0xCDCD ==> Row:45 Col:45 */
    {0x5C71,0xC9BD}, /* 山: GB2312 Code: 0xC9BD ==> Row:41 Col:29 */
    {0x5C79,0xD2D9}, /* 屹: GB2312 Code: 0xD2D9 ==> Row:50 Col:57 */
    {0x5C7A,0xE1A8}, /* 屺: GB2312 Code: 0xE1A8 ==> Row:65 Col:08 */
    {0x5C7F,0xD3EC}, /* 屿: GB2312 Code: 0xD3EC ==> Row:51 Col:76 */
    {0x5C81,0xCBEA}, /* 岁: GB2312 Code: 0xCBEA ==> Row:43 Col:74 */
    {0x5C82,0xC6F1}, /* 岂: GB2312 Code: 0xC6F1 ==> Row:38 Col:81 */
    {0x5C88,0xE1AC}, /* 岈: GB2312 Code: 0xE1AC ==> Row:65 Col:12 */
    {0x5C8C,0xE1A7}, /* 岌: GB2312 Code: 0xE1A7 ==> Row:65 Col:07 */
    {0x5C8D,0xE1A9}, /* 岍: GB2312 Code: 0xE1A9 ==> Row:65 Col:09 */
    {0x5C90,0xE1AA}, /* 岐: GB2312 Code: 0xE1AA ==> Row:65 Col:10 */
    {0x5C91,0xE1AF}, /* 岑: GB2312 Code: 0xE1AF ==> Row:65 Col:15 */
    {0x5C94,0xB2ED}, /* 岔: GB2312 Code: 0xB2ED ==> Row:18 Col:77 */
    {0x5C96,0xE1AB}, /* 岖: GB2312 Code: 0xE1AB ==> Row:65 Col:11 */
    {0x5C97,0xB8DA}, /* 岗: GB2312 Code: 0xB8DA ==> Row:24 Col:58 */
    {0x5C98,0xE1AD}, /* 岘: GB2312 Code: 0xE1AD ==> Row:65 Col:13 */
    {0x5C99,0xE1AE}, /* 岙: GB2312 Code: 0xE1AE ==> Row:65 Col:14 */
    {0x5C9A,0xE1B0}, /* 岚: GB2312 Code: 0xE1B0 ==> Row:65 Col:16 */
    {0x5C9B,0xB5BA}, /* 岛: GB2312 Code: 0xB5BA ==> Row:21 Col:26 */
    {0x5C9C,0xE1B1}, /* 岜: GB2312 Code: 0xE1B1 ==> Row:65 Col:17 */
    {0x5CA2,0xE1B3}, /* 岢: GB2312 Code: 0xE1B3 ==> Row:65 Col:19 */
    {0x5CA3,0xE1B8}, /* 岣: GB2312 Code: 0xE1B8 ==> Row:65 Col:24 */
    {0x5CA9,0xD1D2}, /* 岩: GB2312 Code: 0xD1D2 ==> Row:49 Col:50 */
    {0x5CAB,0xE1B6}, /* 岫: GB2312 Code: 0xE1B6 ==> Row:65 Col:22 */
    {0x5CAC,0xE1B5}, /* 岬: GB2312 Code: 0xE1B5 ==> Row:65 Col:21 */
    {0x5CAD,0xC1EB}, /* 岭: GB2312 Code: 0xC1EB ==> Row:33 Col:75 */
    {0x5CB1,0xE1B7}, /* 岱: GB2312 Code: 0xE1B7 ==> Row:65 Col:23 */
    {0x5CB3,0xD4C0}, /* 岳: GB2312 Code: 0xD4C0 ==> Row:52 Col:32 */
    {0x5CB5,0xE1B2}, /* 岵: GB2312 Code: 0xE1B2 ==> Row:65 Col:18 */
    {0x5CB7,0xE1BA}, /* 岷: GB2312 Code: 0xE1BA ==> Row:65 Col:26 */
    {0x5CB8,0xB0B6}, /* 岸: GB2312 Code: 0xB0B6 ==> Row:16 Col:22 */
    {0x5CBD,0xE1B4}, /* 岽: GB2312 Code: 0xE1B4 ==> Row:65 Col:20 */
    {0x5CBF,0xBFF9}, /* 岿: GB2312 Code: 0xBFF9 ==> Row:31 Col:89 */
    {0x5CC1,0xE1B9}, /* 峁: GB2312 Code: 0xE1B9 ==> Row:65 Col:25 */
    {0x5CC4,0xE1BB}, /* 峄: GB2312 Code: 0xE1BB ==> Row:65 Col:27 */
    {0x5CCB,0xE1BE}, /* 峋: GB2312 Code: 0xE1BE ==> Row:65 Col:30 */
    {0x5CD2,0xE1BC}, /* 峒: GB2312 Code: 0xE1BC ==> Row:65 Col:28 */
    {0x5CD9,0xD6C5}, /* 峙: GB2312 Code: 0xD6C5 ==> Row:54 Col:37 */
    {0x5CE1,0xCFBF}, /* 峡: GB2312 Code: 0xCFBF ==> Row:47 Col:31 */
    {0x5CE4,0xE1BD}, /* 峤: GB2312 Code: 0xE1BD ==> Row:65 Col:29 */
    {0x5CE5,0xE1BF}, /* 峥: GB2312 Code: 0xE1BF ==> Row:65 Col:31 */
    {0x5CE6,0xC2CD}, /* 峦: GB2312 Code: 0xC2CD ==> Row:34 Col:45 */
    {0x5CE8,0xB6EB}, /* 峨: GB2312 Code: 0xB6EB ==> Row:22 Col:75 */
    {0x5CEA,0xD3F8}, /* 峪: GB2312 Code: 0xD3F8 ==> Row:51 Col:88 */
    {0x5CED,0xC7CD}, /* 峭: GB2312 Code: 0xC7CD ==> Row:39 Col:45 */
    {0x5CF0,0xB7E5}, /* 峰: GB2312 Code: 0xB7E5 ==> Row:23 Col:69 */
    {0x5CFB,0xBEFE}, /* 峻: GB2312 Code: 0xBEFE ==> Row:30 Col:94 */
    {0x5D02,0xE1C0}, /* 崂: GB2312 Code: 0xE1C0 ==> Row:65 Col:32 */
    {0x5D03,0xE1C1}, /* 崃: GB2312 Code: 0xE1C1 ==> Row:65 Col:33 */
    {0x5D06,0xE1C7}, /* 崆: GB2312 Code: 0xE1C7 ==> Row:65 Col:39 */
    {0x5D07,0xB3E7}, /* 崇: GB2312 Code: 0xB3E7 ==> Row:19 Col:71 */
    {0x5D0E,0xC6E9}, /* 崎: GB2312 Code: 0xC6E9 ==> Row:38 Col:73 */
    {0x5D14,0xB4DE}, /* 崔: GB2312 Code: 0xB4DE ==> Row:20 Col:62 */
    {0x5D16,0xD1C2}, /* 崖: GB2312 Code: 0xD1C2 ==> Row:49 Col:34 */
    {0x5D1B,0xE1C8}, /* 崛: GB2312 Code: 0xE1C8 ==> Row:65 Col:40 */
    {0x5D1E,0xE1C6}, /* 崞: GB2312 Code: 0xE1C6 ==> Row:65 Col:38 */
    {0x5D24,0xE1C5}, /* 崤: GB2312 Code: 0xE1C5 ==> Row:65 Col:37 */
    {0x5D26,0xE1C3}, /* 崦: GB2312 Code: 0xE1C3 ==> Row:65 Col:35 */
    {0x5D27,0xE1C2}, /* 崧: GB2312 Code: 0xE1C2 ==> Row:65 Col:34 */
    {0x5D29,0xB1C0}, /* 崩: GB2312 Code: 0xB1C0 ==> Row:17 Col:32 */
    {0x5D2D,0xD5B8}, /* 崭: GB2312 Code: 0xD5B8 ==> Row:53 Col:24 */
    {0x5D2E,0xE1C4}, /* 崮: GB2312 Code: 0xE1C4 ==> Row:65 Col:36 */
    {0x5D34,0xE1CB}, /* 崴: GB2312 Code: 0xE1CB ==> Row:65 Col:43 */
    {0x5D3D,0xE1CC}, /* 崽: GB2312 Code: 0xE1CC ==> Row:65 Col:44 */
    {0x5D3E,0xE1CA}, /* 崾: GB2312 Code: 0xE1CA ==> Row:65 Col:42 */
    {0x5D47,0xEFFA}, /* 嵇: GB2312 Code: 0xEFFA ==> Row:79 Col:90 */
    {0x5D4A,0xE1D3}, /* 嵊: GB2312 Code: 0xE1D3 ==> Row:65 Col:51 */
    {0x5D4B,0xE1D2}, /* 嵋: GB2312 Code: 0xE1D2 ==> Row:65 Col:50 */
    {0x5D4C,0xC7B6}, /* 嵌: GB2312 Code: 0xC7B6 ==> Row:39 Col:22 */
    {0x5D58,0xE1C9}, /* 嵘: GB2312 Code: 0xE1C9 ==> Row:65 Col:41 */
    {0x5D5B,0xE1CE}, /* 嵛: GB2312 Code: 0xE1CE ==> Row:65 Col:46 */
    {0x5D5D,0xE1D0}, /* 嵝: GB2312 Code: 0xE1D0 ==> Row:65 Col:48 */
    {0x5D69,0xE1D4}, /* 嵩: GB2312 Code: 0xE1D4 ==> Row:65 Col:52 */
    {0x5D6B,0xE1D1}, /* 嵫: GB2312 Code: 0xE1D1 ==> Row:65 Col:49 */
    {0x5D6C,0xE1CD}, /* 嵬: GB2312 Code: 0xE1CD ==> Row:65 Col:45 */
    {0x5D6F,0xE1CF}, /* 嵯: GB2312 Code: 0xE1CF ==> Row:65 Col:47 */
    {0x5D74,0xE1D5}, /* 嵴: GB2312 Code: 0xE1D5 ==> Row:65 Col:53 */
    {0x5D82,0xE1D6}, /* 嶂: GB2312 Code: 0xE1D6 ==> Row:65 Col:54 */
    {0x5D99,0xE1D7}, /* 嶙: GB2312 Code: 0xE1D7 ==> Row:65 Col:55 */
    {0x5D9D,0xE1D8}, /* 嶝: GB2312 Code: 0xE1D8 ==> Row:65 Col:56 */
    {0x5DB7,0xE1DA}, /* 嶷: GB2312 Code: 0xE1DA ==> Row:65 Col:58 */
    {0x5DC5,0xE1DB}, /* 巅: GB2312 Code: 0xE1DB ==> Row:65 Col:59 */
    {0x5DCD,0xCEA1}, /* 巍: GB2312 Code: 0xCEA1 ==> Row:46 Col:01 */
    {0x5DDB,0xE7DD}, /* 巛: GB2312 Code: 0xE7DD ==> Row:71 Col:61 */
    {0x5DDD,0xB4A8}, /* 川: GB2312 Code: 0xB4A8 ==> Row:20 Col:08 */
    {0x5DDE,0xD6DD}, /* 州: GB2312 Code: 0xD6DD ==> Row:54 Col:61 */
    {0x5DE1,0xD1B2}, /* 巡: GB2312 Code: 0xD1B2 ==> Row:49 Col:18 */
    {0x5DE2,0xB3B2}, /* 巢: GB2312 Code: 0xB3B2 ==> Row:19 Col:18 */
    {0x5DE5,0xB9A4}, /* 工: GB2312 Code: 0xB9A4 ==> Row:25 Col:04 */
    {0x5DE6,0xD7F3}, /* 左: GB2312 Code: 0xD7F3 ==> Row:55 Col:83 */
    {0x5DE7,0xC7C9}, /* 巧: GB2312 Code: 0xC7C9 ==> Row:39 Col:41 */
    {0x5DE8,0xBEDE}, /* 巨: GB2312 Code: 0xBEDE ==> Row:30 Col:62 */
    {0x5DE9,0xB9AE}, /* 巩: GB2312 Code: 0xB9AE ==> Row:25 Col:14 */
    {0x5DEB,0xCED7}, /* 巫: GB2312 Code: 0xCED7 ==> Row:46 Col:55 */
    {0x5DEE,0xB2EE}, /* 差: GB2312 Code: 0xB2EE ==> Row:18 Col:78 */
    {0x5DEF,0xDBCF}, /* 巯: GB2312 Code: 0xDBCF ==> Row:59 Col:47 */
    {0x5DF1,0xBCBA}, /* 己: GB2312 Code: 0xBCBA ==> Row:28 Col:26 */
    {0x5DF2,0xD2D1}, /* 已: GB2312 Code: 0xD2D1 ==> Row:50 Col:49 */
    {0x5DF3,0xCBC8}, /* 巳: GB2312 Code: 0xCBC8 ==> Row:43 Col:40 */
    {0x5DF4,0xB0CD}, /* 巴: GB2312 Code: 0xB0CD ==> Row:16 Col:45 */
    {0x5DF7,0xCFEF}, /* 巷: GB2312 Code: 0xCFEF ==> Row:47 Col:79 */
    {0x5DFD,0xD9E3}, /* 巽: GB2312 Code: 0xD9E3 ==> Row:57 Col:67 */
    {0x5DFE,0xBDED}, /* 巾: GB2312 Code: 0xBDED ==> Row:29 Col:77 */
    {0x5E01,0xB1D2}, /* 币: GB2312 Code: 0xB1D2 ==> Row:17 Col:50 */
    {0x5E02,0xCAD0}, /* 市: GB2312 Code: 0xCAD0 ==> Row:42 Col:48 */
    {0x5E03,0xB2BC}, /* 布: GB2312 Code: 0xB2BC ==> Row:18 Col:28 */
    {0x5E05,0xCBA7}, /* 帅: GB2312 Code: 0xCBA7 ==> Row:43 Col:07 */
    {0x5E06,0xB7AB}, /* 帆: GB2312 Code: 0xB7AB ==> Row:23 Col:11 */
    {0x5E08,0xCAA6}, /* 师: GB2312 Code: 0xCAA6 ==> Row:42 Col:06 */
    {0x5E0C,0xCFA3}, /* 希: GB2312 Code: 0xCFA3 ==> Row:47 Col:03 */
    {0x5E0F,0xE0F8}, /* 帏: GB2312 Code: 0xE0F8 ==> Row:64 Col:88 */
    {0x5E10,0xD5CA}, /* 帐: GB2312 Code: 0xD5CA ==> Row:53 Col:42 */
    {0x5E11,0xE0FB}, /* 帑: GB2312 Code: 0xE0FB ==> Row:64 Col:91 */
    {0x5E14,0xE0FA}, /* 帔: GB2312 Code: 0xE0FA ==> Row:64 Col:90 */
    {0x5E15,0xC5C1}, /* 帕: GB2312 Code: 0xC5C1 ==> Row:37 Col:33 */
    {0x5E16,0xCCFB}, /* 帖: GB2312 Code: 0xCCFB ==> Row:44 Col:91 */
    {0x5E18,0xC1B1}, /* 帘: GB2312 Code: 0xC1B1 ==> Row:33 Col:17 */
    {0x5E19,0xE0F9}, /* 帙: GB2312 Code: 0xE0F9 ==> Row:64 Col:89 */
    {0x5E1A,0xD6E3}, /* 帚: GB2312 Code: 0xD6E3 ==> Row:54 Col:67 */
    {0x5E1B,0xB2AF}, /* 帛: GB2312 Code: 0xB2AF ==> Row:18 Col:15 */
    {0x5E1C,0xD6C4}, /* 帜: GB2312 Code: 0xD6C4 ==> Row:54 Col:36 */
    {0x5E1D,0xB5DB}, /* 帝: GB2312 Code: 0xB5DB ==> Row:21 Col:59 */
    {0x5E26,0xB4F8}, /* 带: GB2312 Code: 0xB4F8 ==> Row:20 Col:88 */
    {0x5E27,0xD6A1}, /* 帧: GB2312 Code: 0xD6A1 ==> Row:54 Col:01 */
    {0x5E2D,0xCFAF}, /* 席: GB2312 Code: 0xCFAF ==> Row:47 Col:15 */
    {0x5E2E,0xB0EF}, /* 帮: GB2312 Code: 0xB0EF ==> Row:16 Col:79 */
    {0x5E31,0xE0FC}, /* 帱: GB2312 Code: 0xE0FC ==> Row:64 Col:92 */
    {0x5E37,0xE1A1}, /* 帷: GB2312 Code: 0xE1A1 ==> Row:65 Col:01 */
    {0x5E38,0xB3A3}, /* 常: GB2312 Code: 0xB3A3 ==> Row:19 Col:03 */
    {0x5E3B,0xE0FD}, /* 帻: GB2312 Code: 0xE0FD ==> Row:64 Col:93 */
    {0x5E3C,0xE0FE}, /* 帼: GB2312 Code: 0xE0FE ==> Row:64 Col:94 */
    {0x5E3D,0xC3B1}, /* 帽: GB2312 Code: 0xC3B1 ==> Row:35 Col:17 */
    {0x5E42,0xC3DD}, /* 幂: GB2312 Code: 0xC3DD ==> Row:35 Col:61 */
    {0x5E44,0xE1A2}, /* 幄: GB2312 Code: 0xE1A2 ==> Row:65 Col:02 */
    {0x5E45,0xB7F9}, /* 幅: GB2312 Code: 0xB7F9 ==> Row:23 Col:89 */
    {0x5E4C,0xBBCF}, /* 幌: GB2312 Code: 0xBBCF ==> Row:27 Col:47 */
    {0x5E54,0xE1A3}, /* 幔: GB2312 Code: 0xE1A3 ==> Row:65 Col:03 */
    {0x5E55,0xC4BB}, /* 幕: GB2312 Code: 0xC4BB ==> Row:36 Col:27 */
    {0x5E5B,0xE1A4}, /* 幛: GB2312 Code: 0xE1A4 ==> Row:65 Col:04 */
    {0x5E5E,0xE1A5}, /* 幞: GB2312 Code: 0xE1A5 ==> Row:65 Col:05 */
    {0x5E61,0xE1A6}, /* 幡: GB2312 Code: 0xE1A6 ==> Row:65 Col:06 */
    {0x5E62,0xB4B1}, /* 幢: GB2312 Code: 0xB4B1 ==> Row:20 Col:17 */
    {0x5E72,0xB8C9}, /* 干: GB2312 Code: 0xB8C9 ==> Row:24 Col:41 */
    {0x5E73,0xC6BD}, /* 平: GB2312 Code: 0xC6BD ==> Row:38 Col:29 */
    {0x5E74,0xC4EA}, /* 年: GB2312 Code: 0xC4EA ==> Row:36 Col:74 */
    {0x5E76,0xB2A2}, /* 并: GB2312 Code: 0xB2A2 ==> Row:18 Col:02 */
    {0x5E78,0xD0D2}, /* 幸: GB2312 Code: 0xD0D2 ==> Row:48 Col:50 */
    {0x5E7A,0xE7DB}, /* 幺: GB2312 Code: 0xE7DB ==> Row:71 Col:59 */
    {0x5E7B,0xBBC3}, /* 幻: GB2312 Code: 0xBBC3 ==> Row:27 Col:35 */
    {0x5E7C,0xD3D7}, /* 幼: GB2312 Code: 0xD3D7 ==> Row:51 Col:55 */
    {0x5E7D,0xD3C4}, /* 幽: GB2312 Code: 0xD3C4 ==> Row:51 Col:36 */
    {0x5E7F,0xB9E3}, /* 广: GB2312 Code: 0xB9E3 ==> Row:25 Col:67 */
    {0x5E80,0xE2CF}, /* 庀: GB2312 Code: 0xE2CF ==> Row:66 Col:47 */
    {0x5E84,0xD7AF}, /* 庄: GB2312 Code: 0xD7AF ==> Row:55 Col:15 */
    {0x5E86,0xC7EC}, /* 庆: GB2312 Code: 0xC7EC ==> Row:39 Col:76 */
    {0x5E87,0xB1D3}, /* 庇: GB2312 Code: 0xB1D3 ==> Row:17 Col:51 */
    {0x5E8A,0xB4B2}, /* 床: GB2312 Code: 0xB4B2 ==> Row:20 Col:18 */
    {0x5E8B,0xE2D1}, /* 庋: GB2312 Code: 0xE2D1 ==> Row:66 Col:49 */
    {0x5E8F,0xD0F2}, /* 序: GB2312 Code: 0xD0F2 ==> Row:48 Col:82 */
    {0x5E90,0xC2AE}, /* 庐: GB2312 Code: 0xC2AE ==> Row:34 Col:14 */
    {0x5E91,0xE2D0}, /* 庑: GB2312 Code: 0xE2D0 ==> Row:66 Col:48 */
    {0x5E93,0xBFE2}, /* 库: GB2312 Code: 0xBFE2 ==> Row:31 Col:66 */
    {0x5E94,0xD3A6}, /* 应: GB2312 Code: 0xD3A6 ==> Row:51 Col:06 */
    {0x5E95,0xB5D7}, /* 底: GB2312 Code: 0xB5D7 ==> Row:21 Col:55 */
    {0x5E96,0xE2D2}, /* 庖: GB2312 Code: 0xE2D2 ==> Row:66 Col:50 */
    {0x5E97,0xB5EA}, /* 店: GB2312 Code: 0xB5EA ==> Row:21 Col:74 */
    {0x5E99,0xC3ED}, /* 庙: GB2312 Code: 0xC3ED ==> Row:35 Col:77 */
    {0x5E9A,0xB8FD}, /* 庚: GB2312 Code: 0xB8FD ==> Row:24 Col:93 */
    {0x5E9C,0xB8AE}, /* 府: GB2312 Code: 0xB8AE ==> Row:24 Col:14 */
    {0x5E9E,0xC5D3}, /* 庞: GB2312 Code: 0xC5D3 ==> Row:37 Col:51 */
    {0x5E9F,0xB7CF}, /* 废: GB2312 Code: 0xB7CF ==> Row:23 Col:47 */
    {0x5EA0,0xE2D4}, /* 庠: GB2312 Code: 0xE2D4 ==> Row:66 Col:52 */
    {0x5EA5,0xE2D3}, /* 庥: GB2312 Code: 0xE2D3 ==> Row:66 Col:51 */
    {0x5EA6,0xB6C8}, /* 度: GB2312 Code: 0xB6C8 ==> Row:22 Col:40 */
    {0x5EA7,0xD7F9}, /* 座: GB2312 Code: 0xD7F9 ==> Row:55 Col:89 */
    {0x5EAD,0xCDA5}, /* 庭: GB2312 Code: 0xCDA5 ==> Row:45 Col:05 */
    {0x5EB3,0xE2D8}, /* 庳: GB2312 Code: 0xE2D8 ==> Row:66 Col:56 */
    {0x5EB5,0xE2D6}, /* 庵: GB2312 Code: 0xE2D6 ==> Row:66 Col:54 */
    {0x5EB6,0xCAFC}, /* 庶: GB2312 Code: 0xCAFC ==> Row:42 Col:92 */
    {0x5EB7,0xBFB5}, /* 康: GB2312 Code: 0xBFB5 ==> Row:31 Col:21 */
    {0x5EB8,0xD3B9}, /* 庸: GB2312 Code: 0xD3B9 ==> Row:51 Col:25 */
    {0x5EB9,0xE2D5}, /* 庹: GB2312 Code: 0xE2D5 ==> Row:66 Col:53 */
    {0x5EBE,0xE2D7}, /* 庾: GB2312 Code: 0xE2D7 ==> Row:66 Col:55 */
    {0x5EC9,0xC1AE}, /* 廉: GB2312 Code: 0xC1AE ==> Row:33 Col:14 */
    {0x5ECA,0xC0C8}, /* 廊: GB2312 Code: 0xC0C8 ==> Row:32 Col:40 */
    {0x5ED1,0xE2DB}, /* 廑: GB2312 Code: 0xE2DB ==> Row:66 Col:59 */
    {0x5ED2,0xE2DA}, /* 廒: GB2312 Code: 0xE2DA ==> Row:66 Col:58 */
    {0x5ED3,0xC0AA}, /* 廓: GB2312 Code: 0xC0AA ==> Row:32 Col:10 */
    {0x5ED6,0xC1CE}, /* 廖: GB2312 Code: 0xC1CE ==> Row:33 Col:46 */
    {0x5EDB,0xE2DC}, /* 廛: GB2312 Code: 0xE2DC ==> Row:66 Col:60 */
    {0x5EE8,0xE2DD}, /* 廨: GB2312 Code: 0xE2DD ==> Row:66 Col:61 */
    {0x5EEA,0xE2DE}, /* 廪: GB2312 Code: 0xE2DE ==> Row:66 Col:62 */
    {0x5EF4,0xDBC8}, /* 廴: GB2312 Code: 0xDBC8 ==> Row:59 Col:40 */
    {0x5EF6,0xD1D3}, /* 延: GB2312 Code: 0xD1D3 ==> Row:49 Col:51 */
    {0x5EF7,0xCDA2}, /* 廷: GB2312 Code: 0xCDA2 ==> Row:45 Col:02 */
    {0x5EFA,0xBDA8}, /* 建: GB2312 Code: 0xBDA8 ==> Row:29 Col:08 */
    {0x5EFE,0xDEC3}, /* 廾: GB2312 Code: 0xDEC3 ==> Row:62 Col:35 */
    {0x5EFF,0xD8A5}, /* 廿: GB2312 Code: 0xD8A5 ==> Row:56 Col:05 */
    {0x5F00,0xBFAA}, /* 开: GB2312 Code: 0xBFAA ==> Row:31 Col:10 */
    {0x5F01,0xDBCD}, /* 弁: GB2312 Code: 0xDBCD ==> Row:59 Col:45 */
    {0x5F02,0xD2EC}, /* 异: GB2312 Code: 0xD2EC ==> Row:50 Col:76 */
    {0x5F03,0xC6FA}, /* 弃: GB2312 Code: 0xC6FA ==> Row:38 Col:90 */
    {0x5F04,0xC5AA}, /* 弄: GB2312 Code: 0xC5AA ==> Row:37 Col:10 */
    {0x5F08,0xDEC4}, /* 弈: GB2312 Code: 0xDEC4 ==> Row:62 Col:36 */
    {0x5F0A,0xB1D7}, /* 弊: GB2312 Code: 0xB1D7 ==> Row:17 Col:55 */
    {0x5F0B,0xDFAE}, /* 弋: GB2312 Code: 0xDFAE ==> Row:63 Col:14 */
    {0x5F0F,0xCABD}, /* 式: GB2312 Code: 0xCABD ==> Row:42 Col:29 */
    {0x5F11,0xDFB1}, /* 弑: GB2312 Code: 0xDFB1 ==> Row:63 Col:17 */
    {0x5F13,0xB9AD}, /* 弓: GB2312 Code: 0xB9AD ==> Row:25 Col:13 */
    {0x5F15,0xD2FD}, /* 引: GB2312 Code: 0xD2FD ==> Row:50 Col:93 */
    {0x5F17,0xB8A5}, /* 弗: GB2312 Code: 0xB8A5 ==> Row:24 Col:05 */
    {0x5F18,0xBAEB}, /* 弘: GB2312 Code: 0xBAEB ==> Row:26 Col:75 */
    {0x5F1B,0xB3DA}, /* 弛: GB2312 Code: 0xB3DA ==> Row:19 Col:58 */
    {0x5F1F,0xB5DC}, /* 弟: GB2312 Code: 0xB5DC ==> Row:21 Col:60 */
    {0x5F20,0xD5C5}, /* 张: GB2312 Code: 0xD5C5 ==> Row:53 Col:37 */
    {0x5F25,0xC3D6}, /* 弥: GB2312 Code: 0xC3D6 ==> Row:35 Col:54 */
    {0x5F26,0xCFD2}, /* 弦: GB2312 Code: 0xCFD2 ==> Row:47 Col:50 */
    {0x5F27,0xBBA1}, /* 弧: GB2312 Code: 0xBBA1 ==> Row:27 Col:01 */
    {0x5F29,0xE5F3}, /* 弩: GB2312 Code: 0xE5F3 ==> Row:69 Col:83 */
    {0x5F2A,0xE5F2}, /* 弪: GB2312 Code: 0xE5F2 ==> Row:69 Col:82 */
    {0x5F2D,0xE5F4}, /* 弭: GB2312 Code: 0xE5F4 ==> Row:69 Col:84 */
    {0x5F2F,0xCDE4}, /* 弯: GB2312 Code: 0xCDE4 ==> Row:45 Col:68 */
    {0x5F31,0xC8F5}, /* 弱: GB2312 Code: 0xC8F5 ==> Row:40 Col:85 */
    {0x5F39,0xB5AF}, /* 弹: GB2312 Code: 0xB5AF ==> Row:21 Col:15 */
    {0x5F3A,0xC7BF}, /* 强: GB2312 Code: 0xC7BF ==> Row:39 Col:31 */
    {0x5F3C,0xE5F6}, /* 弼: GB2312 Code: 0xE5F6 ==> Row:69 Col:86 */
    {0x5F40,0xECB0}, /* 彀: GB2312 Code: 0xECB0 ==> Row:76 Col:16 */
    {0x5F50,0xE5E6}, /* 彐: GB2312 Code: 0xE5E6 ==> Row:69 Col:70 */
    {0x5F52,0xB9E9}, /* 归: GB2312 Code: 0xB9E9 ==> Row:25 Col:73 */
    {0x5F53,0xB5B1}, /* 当: GB2312 Code: 0xB5B1 ==> Row:21 Col:17 */
    {0x5F55,0xC2BC}, /* 录: GB2312 Code: 0xC2BC ==> Row:34 Col:28 */
    {0x5F56,0xE5E8}, /* 彖: GB2312 Code: 0xE5E8 ==> Row:69 Col:72 */
    {0x5F57,0xE5E7}, /* 彗: GB2312 Code: 0xE5E7 ==> Row:69 Col:71 */
    {0x5F58,0xE5E9}, /* 彘: GB2312 Code: 0xE5E9 ==> Row:69 Col:73 */
    {0x5F5D,0xD2CD}, /* 彝: GB2312 Code: 0xD2CD ==> Row:50 Col:45 */
    {0x5F61,0xE1EA}, /* 彡: GB2312 Code: 0xE1EA ==> Row:65 Col:74 */
    {0x5F62,0xD0CE}, /* 形: GB2312 Code: 0xD0CE ==> Row:48 Col:46 */
    {0x5F64,0xCDAE}, /* 彤: GB2312 Code: 0xCDAE ==> Row:45 Col:14 */
    {0x5F66,0xD1E5}, /* 彦: GB2312 Code: 0xD1E5 ==> Row:49 Col:69 */
    {0x5F69,0xB2CA}, /* 彩: GB2312 Code: 0xB2CA ==> Row:18 Col:42 */
    {0x5F6A,0xB1EB}, /* 彪: GB2312 Code: 0xB1EB ==> Row:17 Col:75 */
    {0x5F6C,0xB1F2}, /* 彬: GB2312 Code: 0xB1F2 ==> Row:17 Col:82 */
    {0x5F6D,0xC5ED}, /* 彭: GB2312 Code: 0xC5ED ==> Row:37 Col:77 */
    {0x5F70,0xD5C3}, /* 彰: GB2312 Code: 0xD5C3 ==> Row:53 Col:35 */
    {0x5F71,0xD3B0}, /* 影: GB2312 Code: 0xD3B0 ==> Row:51 Col:16 */
    {0x5F73,0xE1DC}, /* 彳: GB2312 Code: 0xE1DC ==> Row:65 Col:60 */
    {0x5F77,0xE1DD}, /* 彷: GB2312 Code: 0xE1DD ==> Row:65 Col:61 */
    {0x5F79,0xD2DB}, /* 役: GB2312 Code: 0xD2DB ==> Row:50 Col:59 */
    {0x5F7B,0xB3B9}, /* 彻: GB2312 Code: 0xB3B9 ==> Row:19 Col:25 */
    {0x5F7C,0xB1CB}, /* 彼: GB2312 Code: 0xB1CB ==> Row:17 Col:43 */
    {0x5F80,0xCDF9}, /* 往: GB2312 Code: 0xCDF9 ==> Row:45 Col:89 */
    {0x5F81,0xD5F7}, /* 征: GB2312 Code: 0xD5F7 ==> Row:53 Col:87 */
    {0x5F82,0xE1DE}, /* 徂: GB2312 Code: 0xE1DE ==> Row:65 Col:62 */
    {0x5F84,0xBEB6}, /* 径: GB2312 Code: 0xBEB6 ==> Row:30 Col:22 */
    {0x5F85,0xB4FD}, /* 待: GB2312 Code: 0xB4FD ==> Row:20 Col:93 */
    {0x5F87,0xE1DF}, /* 徇: GB2312 Code: 0xE1DF ==> Row:65 Col:63 */
    {0x5F88,0xBADC}, /* 很: GB2312 Code: 0xBADC ==> Row:26 Col:60 */
    {0x5F89,0xE1E0}, /* 徉: GB2312 Code: 0xE1E0 ==> Row:65 Col:64 */
    {0x5F8A,0xBBB2}, /* 徊: GB2312 Code: 0xBBB2 ==> Row:27 Col:18 */
    {0x5F8B,0xC2C9}, /* 律: GB2312 Code: 0xC2C9 ==> Row:34 Col:41 */
    {0x5F8C,0xE1E1}, /* 後: GB2312 Code: 0xE1E1 ==> Row:65 Col:65 */
    {0x5F90,0xD0EC}, /* 徐: GB2312 Code: 0xD0EC ==> Row:48 Col:76 */
    {0x5F92,0xCDBD}, /* 徒: GB2312 Code: 0xCDBD ==> Row:45 Col:29 */
    {0x5F95,0xE1E2}, /* 徕: GB2312 Code: 0xE1E2 ==> Row:65 Col:66 */
    {0x5F97,0xB5C3}, /* 得: GB2312 Code: 0xB5C3 ==> Row:21 Col:35 */
    {0x5F98,0xC5C7}, /* 徘: GB2312 Code: 0xC5C7 ==> Row:37 Col:39 */
    {0x5F99,0xE1E3}, /* 徙: GB2312 Code: 0xE1E3 ==> Row:65 Col:67 */
    {0x5F9C,0xE1E4}, /* 徜: GB2312 Code: 0xE1E4 ==> Row:65 Col:68 */
    {0x5FA1,0xD3F9}, /* 御: GB2312 Code: 0xD3F9 ==> Row:51 Col:89 */
    {0x5FA8,0xE1E5}, /* 徨: GB2312 Code: 0xE1E5 ==> Row:65 Col:69 */
    {0x5FAA,0xD1AD}, /* 循: GB2312 Code: 0xD1AD ==> Row:49 Col:13 */
    {0x5FAD,0xE1E6}, /* 徭: GB2312 Code: 0xE1E6 ==> Row:65 Col:70 */
    {0x5FAE,0xCEA2}, /* 微: GB2312 Code: 0xCEA2 ==> Row:46 Col:02 */
    {0x5FB5,0xE1E7}, /* 徵: GB2312 Code: 0xE1E7 ==> Row:65 Col:71 */
    {0x5FB7,0xB5C2}, /* 德: GB2312 Code: 0xB5C2 ==> Row:21 Col:34 */
    {0x5FBC,0xE1E8}, /* 徼: GB2312 Code: 0xE1E8 ==> Row:65 Col:72 */
    {0x5FBD,0xBBD5}, /* 徽: GB2312 Code: 0xBBD5 ==> Row:27 Col:53 */
    {0x5FC3,0xD0C4}, /* 心: GB2312 Code: 0xD0C4 ==> Row:48 Col:36 */
    {0x5FC4,0xE2E0}, /* 忄: GB2312 Code: 0xE2E0 ==> Row:66 Col:64 */
    {0x5FC5,0xB1D8}, /* 必: GB2312 Code: 0xB1D8 ==> Row:17 Col:56 */
    {0x5FC6,0xD2E4}, /* 忆: GB2312 Code: 0xD2E4 ==> Row:50 Col:68 */
    {0x5FC9,0xE2E1}, /* 忉: GB2312 Code: 0xE2E1 ==> Row:66 Col:65 */
    {0x5FCC,0xBCC9}, /* 忌: GB2312 Code: 0xBCC9 ==> Row:28 Col:41 */
    {0x5FCD,0xC8CC}, /* 忍: GB2312 Code: 0xC8CC ==> Row:40 Col:44 */
    {0x5FCF,0xE2E3}, /* 忏: GB2312 Code: 0xE2E3 ==> Row:66 Col:67 */
    {0x5FD0,0xECFE}, /* 忐: GB2312 Code: 0xECFE ==> Row:76 Col:94 */
    {0x5FD1,0xECFD}, /* 忑: GB2312 Code: 0xECFD ==> Row:76 Col:93 */
    {0x5FD2,0xDFAF}, /* 忒: GB2312 Code: 0xDFAF ==> Row:63 Col:15 */
    {0x5FD6,0xE2E2}, /* 忖: GB2312 Code: 0xE2E2 ==> Row:66 Col:66 */
    {0x5FD7,0xD6BE}, /* 志: GB2312 Code: 0xD6BE ==> Row:54 Col:30 */
    {0x5FD8,0xCDFC}, /* 忘: GB2312 Code: 0xCDFC ==> Row:45 Col:92 */
    {0x5FD9,0xC3A6}, /* 忙: GB2312 Code: 0xC3A6 ==> Row:35 Col:06 */
    {0x5FDD,0xE3C3}, /* 忝: GB2312 Code: 0xE3C3 ==> Row:67 Col:35 */
    {0x5FE0,0xD6D2}, /* 忠: GB2312 Code: 0xD6D2 ==> Row:54 Col:50 */
    {0x5FE1,0xE2E7}, /* 忡: GB2312 Code: 0xE2E7 ==> Row:66 Col:71 */
    {0x5FE4,0xE2E8}, /* 忤: GB2312 Code: 0xE2E8 ==> Row:66 Col:72 */
    {0x5FE7,0xD3C7}, /* 忧: GB2312 Code: 0xD3C7 ==> Row:51 Col:39 */
    {0x5FEA,0xE2EC}, /* 忪: GB2312 Code: 0xE2EC ==> Row:66 Col:76 */
    {0x5FEB,0xBFEC}, /* 快: GB2312 Code: 0xBFEC ==> Row:31 Col:76 */
    {0x5FED,0xE2ED}, /* 忭: GB2312 Code: 0xE2ED ==> Row:66 Col:77 */
    {0x5FEE,0xE2E5}, /* 忮: GB2312 Code: 0xE2E5 ==> Row:66 Col:69 */
    {0x5FF1,0xB3C0}, /* 忱: GB2312 Code: 0xB3C0 ==> Row:19 Col:32 */
    {0x5FF5,0xC4EE}, /* 念: GB2312 Code: 0xC4EE ==> Row:36 Col:78 */
    {0x5FF8,0xE2EE}, /* 忸: GB2312 Code: 0xE2EE ==> Row:66 Col:78 */
    {0x5FFB,0xD0C3}, /* 忻: GB2312 Code: 0xD0C3 ==> Row:48 Col:35 */
    {0x5FFD,0xBAF6}, /* 忽: GB2312 Code: 0xBAF6 ==> Row:26 Col:86 */
    {0x5FFE,0xE2E9}, /* 忾: GB2312 Code: 0xE2E9 ==> Row:66 Col:73 */
    {0x5FFF,0xB7DE}, /* 忿: GB2312 Code: 0xB7DE ==> Row:23 Col:62 */
    {0x6000,0xBBB3}, /* 怀: GB2312 Code: 0xBBB3 ==> Row:27 Col:19 */
    {0x6001,0xCCAC}, /* 态: GB2312 Code: 0xCCAC ==> Row:44 Col:12 */
    {0x6002,0xCBCB}, /* 怂: GB2312 Code: 0xCBCB ==> Row:43 Col:43 */
    {0x6003,0xE2E4}, /* 怃: GB2312 Code: 0xE2E4 ==> Row:66 Col:68 */
    {0x6004,0xE2E6}, /* 怄: GB2312 Code: 0xE2E6 ==> Row:66 Col:70 */
    {0x6005,0xE2EA}, /* 怅: GB2312 Code: 0xE2EA ==> Row:66 Col:74 */
    {0x6006,0xE2EB}, /* 怆: GB2312 Code: 0xE2EB ==> Row:66 Col:75 */
    {0x600A,0xE2F7}, /* 怊: GB2312 Code: 0xE2F7 ==> Row:66 Col:87 */
    {0x600D,0xE2F4}, /* 怍: GB2312 Code: 0xE2F4 ==> Row:66 Col:84 */
    {0x600E,0xD4F5}, /* 怎: GB2312 Code: 0xD4F5 ==> Row:52 Col:85 */
    {0x600F,0xE2F3}, /* 怏: GB2312 Code: 0xE2F3 ==> Row:66 Col:83 */
    {0x6012,0xC5AD}, /* 怒: GB2312 Code: 0xC5AD ==> Row:37 Col:13 */
    {0x6014,0xD5FA}, /* 怔: GB2312 Code: 0xD5FA ==> Row:53 Col:90 */
    {0x6015,0xC5C2}, /* 怕: GB2312 Code: 0xC5C2 ==> Row:37 Col:34 */
    {0x6016,0xB2C0}, /* 怖: GB2312 Code: 0xB2C0 ==> Row:18 Col:32 */
    {0x6019,0xE2EF}, /* 怙: GB2312 Code: 0xE2EF ==> Row:66 Col:79 */
    {0x601B,0xE2F2}, /* 怛: GB2312 Code: 0xE2F2 ==> Row:66 Col:82 */
    {0x601C,0xC1AF}, /* 怜: GB2312 Code: 0xC1AF ==> Row:33 Col:15 */
    {0x601D,0xCBBC}, /* 思: GB2312 Code: 0xCBBC ==> Row:43 Col:28 */
    {0x6020,0xB5A1}, /* 怠: GB2312 Code: 0xB5A1 ==> Row:21 Col:01 */
    {0x6021,0xE2F9}, /* 怡: GB2312 Code: 0xE2F9 ==> Row:66 Col:89 */
    {0x6025,0xBCB1}, /* 急: GB2312 Code: 0xBCB1 ==> Row:28 Col:17 */
    {0x6026,0xE2F1}, /* 怦: GB2312 Code: 0xE2F1 ==> Row:66 Col:81 */
    {0x6027,0xD0D4}, /* 性: GB2312 Code: 0xD0D4 ==> Row:48 Col:52 */
    {0x6028,0xD4B9}, /* 怨: GB2312 Code: 0xD4B9 ==> Row:52 Col:25 */
    {0x6029,0xE2F5}, /* 怩: GB2312 Code: 0xE2F5 ==> Row:66 Col:85 */
    {0x602A,0xB9D6}, /* 怪: GB2312 Code: 0xB9D6 ==> Row:25 Col:54 */
    {0x602B,0xE2F6}, /* 怫: GB2312 Code: 0xE2F6 ==> Row:66 Col:86 */
    {0x602F,0xC7D3}, /* 怯: GB2312 Code: 0xC7D3 ==> Row:39 Col:51 */
    {0x6035,0xE2F0}, /* 怵: GB2312 Code: 0xE2F0 ==> Row:66 Col:80 */
    {0x603B,0xD7DC}, /* 总: GB2312 Code: 0xD7DC ==> Row:55 Col:60 */
    {0x603C,0xEDA1}, /* 怼: GB2312 Code: 0xEDA1 ==> Row:77 Col:01 */
    {0x603F,0xE2F8}, /* 怿: GB2312 Code: 0xE2F8 ==> Row:66 Col:88 */
    {0x6041,0xEDA5}, /* 恁: GB2312 Code: 0xEDA5 ==> Row:77 Col:05 */
    {0x6042,0xE2FE}, /* 恂: GB2312 Code: 0xE2FE ==> Row:66 Col:94 */
    {0x6043,0xCAD1}, /* 恃: GB2312 Code: 0xCAD1 ==> Row:42 Col:49 */
    {0x604B,0xC1B5}, /* 恋: GB2312 Code: 0xC1B5 ==> Row:33 Col:21 */
    {0x604D,0xBBD0}, /* 恍: GB2312 Code: 0xBBD0 ==> Row:27 Col:48 */
    {0x6050,0xBFD6}, /* 恐: GB2312 Code: 0xBFD6 ==> Row:31 Col:54 */
    {0x6052,0xBAE3}, /* 恒: GB2312 Code: 0xBAE3 ==> Row:26 Col:67 */
    {0x6055,0xCBA1}, /* 恕: GB2312 Code: 0xCBA1 ==> Row:43 Col:01 */
    {0x6059,0xEDA6}, /* 恙: GB2312 Code: 0xEDA6 ==> Row:77 Col:06 */
    {0x605A,0xEDA3}, /* 恚: GB2312 Code: 0xEDA3 ==> Row:77 Col:03 */
    {0x605D,0xEDA2}, /* 恝: GB2312 Code: 0xEDA2 ==> Row:77 Col:02 */
    {0x6062,0xBBD6}, /* 恢: GB2312 Code: 0xBBD6 ==> Row:27 Col:54 */
    {0x6063,0xEDA7}, /* 恣: GB2312 Code: 0xEDA7 ==> Row:77 Col:07 */
    {0x6064,0xD0F4}, /* 恤: GB2312 Code: 0xD0F4 ==> Row:48 Col:84 */
    {0x6067,0xEDA4}, /* 恧: GB2312 Code: 0xEDA4 ==> Row:77 Col:04 */
    {0x6068,0xBADE}, /* 恨: GB2312 Code: 0xBADE ==> Row:26 Col:62 */
    {0x6069,0xB6F7}, /* 恩: GB2312 Code: 0xB6F7 ==> Row:22 Col:87 */
    {0x606A,0xE3A1}, /* 恪: GB2312 Code: 0xE3A1 ==> Row:67 Col:01 */
    {0x606B,0xB6B2}, /* 恫: GB2312 Code: 0xB6B2 ==> Row:22 Col:18 */
    {0x606C,0xCCF1}, /* 恬: GB2312 Code: 0xCCF1 ==> Row:44 Col:81 */
    {0x606D,0xB9A7}, /* 恭: GB2312 Code: 0xB9A7 ==> Row:25 Col:07 */
    {0x606F,0xCFA2}, /* 息: GB2312 Code: 0xCFA2 ==> Row:47 Col:02 */
    {0x6070,0xC7A1}, /* 恰: GB2312 Code: 0xC7A1 ==> Row:39 Col:01 */
    {0x6073,0xBFD2}, /* 恳: GB2312 Code: 0xBFD2 ==> Row:31 Col:50 */
    {0x6076,0xB6F1}, /* 恶: GB2312 Code: 0xB6F1 ==> Row:22 Col:81 */
    {0x6078,0xE2FA}, /* 恸: GB2312 Code: 0xE2FA ==> Row:66 Col:90 */
    {0x6079,0xE2FB}, /* 恹: GB2312 Code: 0xE2FB ==> Row:66 Col:91 */
    {0x607A,0xE2FD}, /* 恺: GB2312 Code: 0xE2FD ==> Row:66 Col:93 */
    {0x607B,0xE2FC}, /* 恻: GB2312 Code: 0xE2FC ==> Row:66 Col:92 */
    {0x607C,0xC4D5}, /* 恼: GB2312 Code: 0xC4D5 ==> Row:36 Col:53 */
    {0x607D,0xE3A2}, /* 恽: GB2312 Code: 0xE3A2 ==> Row:67 Col:02 */
    {0x607F,0xD3C1}, /* 恿: GB2312 Code: 0xD3C1 ==> Row:51 Col:33 */
    {0x6083,0xE3A7}, /* 悃: GB2312 Code: 0xE3A7 ==> Row:67 Col:07 */
    {0x6084,0xC7C4}, /* 悄: GB2312 Code: 0xC7C4 ==> Row:39 Col:36 */
    {0x6089,0xCFA4}, /* 悉: GB2312 Code: 0xCFA4 ==> Row:47 Col:04 */
    {0x608C,0xE3A9}, /* 悌: GB2312 Code: 0xE3A9 ==> Row:67 Col:09 */
    {0x608D,0xBAB7}, /* 悍: GB2312 Code: 0xBAB7 ==> Row:26 Col:23 */
    {0x6092,0xE3A8}, /* 悒: GB2312 Code: 0xE3A8 ==> Row:67 Col:08 */
    {0x6094,0xBBDA}, /* 悔: GB2312 Code: 0xBBDA ==> Row:27 Col:58 */
    {0x6096,0xE3A3}, /* 悖: GB2312 Code: 0xE3A3 ==> Row:67 Col:03 */
    {0x609A,0xE3A4}, /* 悚: GB2312 Code: 0xE3A4 ==> Row:67 Col:04 */
    {0x609B,0xE3AA}, /* 悛: GB2312 Code: 0xE3AA ==> Row:67 Col:10 */
    {0x609D,0xE3A6}, /* 悝: GB2312 Code: 0xE3A6 ==> Row:67 Col:06 */
    {0x609F,0xCEF2}, /* 悟: GB2312 Code: 0xCEF2 ==> Row:46 Col:82 */
    {0x60A0,0xD3C6}, /* 悠: GB2312 Code: 0xD3C6 ==> Row:51 Col:38 */
    {0x60A3,0xBBBC}, /* 患: GB2312 Code: 0xBBBC ==> Row:27 Col:28 */
    {0x60A6,0xD4C3}, /* 悦: GB2312 Code: 0xD4C3 ==> Row:52 Col:35 */
    {0x60A8,0xC4FA}, /* 您: GB2312 Code: 0xC4FA ==> Row:36 Col:90 */
    {0x60AB,0xEDA8}, /* 悫: GB2312 Code: 0xEDA8 ==> Row:77 Col:08 */
    {0x60AC,0xD0FC}, /* 悬: GB2312 Code: 0xD0FC ==> Row:48 Col:92 */
    {0x60AD,0xE3A5}, /* 悭: GB2312 Code: 0xE3A5 ==> Row:67 Col:05 */
    {0x60AF,0xC3F5}, /* 悯: GB2312 Code: 0xC3F5 ==> Row:35 Col:85 */
    {0x60B1,0xE3AD}, /* 悱: GB2312 Code: 0xE3AD ==> Row:67 Col:13 */
    {0x60B2,0xB1AF}, /* 悲: GB2312 Code: 0xB1AF ==> Row:17 Col:15 */
    {0x60B4,0xE3B2}, /* 悴: GB2312 Code: 0xE3B2 ==> Row:67 Col:18 */
    {0x60B8,0xBCC2}, /* 悸: GB2312 Code: 0xBCC2 ==> Row:28 Col:34 */
    {0x60BB,0xE3AC}, /* 悻: GB2312 Code: 0xE3AC ==> Row:67 Col:12 */
    {0x60BC,0xB5BF}, /* 悼: GB2312 Code: 0xB5BF ==> Row:21 Col:31 */
    {0x60C5,0xC7E9}, /* 情: GB2312 Code: 0xC7E9 ==> Row:39 Col:73 */
    {0x60C6,0xE3B0}, /* 惆: GB2312 Code: 0xE3B0 ==> Row:67 Col:16 */
    {0x60CA,0xBEAA}, /* 惊: GB2312 Code: 0xBEAA ==> Row:30 Col:10 */
    {0x60CB,0xCDEF}, /* 惋: GB2312 Code: 0xCDEF ==> Row:45 Col:79 */
    {0x60D1,0xBBF3}, /* 惑: GB2312 Code: 0xBBF3 ==> Row:27 Col:83 */
    {0x60D5,0xCCE8}, /* 惕: GB2312 Code: 0xCCE8 ==> Row:44 Col:72 */
    {0x60D8,0xE3AF}, /* 惘: GB2312 Code: 0xE3AF ==> Row:67 Col:15 */
    {0x60DA,0xE3B1}, /* 惚: GB2312 Code: 0xE3B1 ==> Row:67 Col:17 */
    {0x60DC,0xCFA7}, /* 惜: GB2312 Code: 0xCFA7 ==> Row:47 Col:07 */
    {0x60DD,0xE3AE}, /* 惝: GB2312 Code: 0xE3AE ==> Row:67 Col:14 */
    {0x60DF,0xCEA9}, /* 惟: GB2312 Code: 0xCEA9 ==> Row:46 Col:09 */
    {0x60E0,0xBBDD}, /* 惠: GB2312 Code: 0xBBDD ==> Row:27 Col:61 */
    {0x60E6,0xB5EB}, /* 惦: GB2312 Code: 0xB5EB ==> Row:21 Col:75 */
    {0x60E7,0xBEE5}, /* 惧: GB2312 Code: 0xBEE5 ==> Row:30 Col:69 */
    {0x60E8,0xB2D2}, /* 惨: GB2312 Code: 0xB2D2 ==> Row:18 Col:50 */
    {0x60E9,0xB3CD}, /* 惩: GB2312 Code: 0xB3CD ==> Row:19 Col:45 */
    {0x60EB,0xB1B9}, /* 惫: GB2312 Code: 0xB1B9 ==> Row:17 Col:25 */
    {0x60EC,0xE3AB}, /* 惬: GB2312 Code: 0xE3AB ==> Row:67 Col:11 */
    {0x60ED,0xB2D1}, /* 惭: GB2312 Code: 0xB2D1 ==> Row:18 Col:49 */
    {0x60EE,0xB5AC}, /* 惮: GB2312 Code: 0xB5AC ==> Row:21 Col:12 */
    {0x60EF,0xB9DF}, /* 惯: GB2312 Code: 0xB9DF ==> Row:25 Col:63 */
    {0x60F0,0xB6E8}, /* 惰: GB2312 Code: 0xB6E8 ==> Row:22 Col:72 */
    {0x60F3,0xCFEB}, /* 想: GB2312 Code: 0xCFEB ==> Row:47 Col:75 */
    {0x60F4,0xE3B7}, /* 惴: GB2312 Code: 0xE3B7 ==> Row:67 Col:23 */
    {0x60F6,0xBBCC}, /* 惶: GB2312 Code: 0xBBCC ==> Row:27 Col:44 */
    {0x60F9,0xC8C7}, /* 惹: GB2312 Code: 0xC8C7 ==> Row:40 Col:39 */
    {0x60FA,0xD0CA}, /* 惺: GB2312 Code: 0xD0CA ==> Row:48 Col:42 */
    {0x6100,0xE3B8}, /* 愀: GB2312 Code: 0xE3B8 ==> Row:67 Col:24 */
    {0x6101,0xB3EE}, /* 愁: GB2312 Code: 0xB3EE ==> Row:19 Col:78 */
    {0x6106,0xEDA9}, /* 愆: GB2312 Code: 0xEDA9 ==> Row:77 Col:09 */
    {0x6108,0xD3FA}, /* 愈: GB2312 Code: 0xD3FA ==> Row:51 Col:90 */
    {0x6109,0xD3E4}, /* 愉: GB2312 Code: 0xD3E4 ==> Row:51 Col:68 */
    {0x610D,0xEDAA}, /* 愍: GB2312 Code: 0xEDAA ==> Row:77 Col:10 */
    {0x610E,0xE3B9}, /* 愎: GB2312 Code: 0xE3B9 ==> Row:67 Col:25 */
    {0x610F,0xD2E2}, /* 意: GB2312 Code: 0xD2E2 ==> Row:50 Col:66 */
    {0x6115,0xE3B5}, /* 愕: GB2312 Code: 0xE3B5 ==> Row:67 Col:21 */
    {0x611A,0xD3DE}, /* 愚: GB2312 Code: 0xD3DE ==> Row:51 Col:62 */
    {0x611F,0xB8D0}, /* 感: GB2312 Code: 0xB8D0 ==> Row:24 Col:48 */
    {0x6120,0xE3B3}, /* 愠: GB2312 Code: 0xE3B3 ==> Row:67 Col:19 */
    {0x6123,0xE3B6}, /* 愣: GB2312 Code: 0xE3B6 ==> Row:67 Col:22 */
    {0x6124,0xB7DF}, /* 愤: GB2312 Code: 0xB7DF ==> Row:23 Col:63 */
    {0x6126,0xE3B4}, /* 愦: GB2312 Code: 0xE3B4 ==> Row:67 Col:20 */
    {0x6127,0xC0A2}, /* 愧: GB2312 Code: 0xC0A2 ==> Row:32 Col:02 */
    {0x612B,0xE3BA}, /* 愫: GB2312 Code: 0xE3BA ==> Row:67 Col:26 */
    {0x613F,0xD4B8}, /* 愿: GB2312 Code: 0xD4B8 ==> Row:52 Col:24 */
    {0x6148,0xB4C8}, /* 慈: GB2312 Code: 0xB4C8 ==> Row:20 Col:40 */
    {0x614A,0xE3BB}, /* 慊: GB2312 Code: 0xE3BB ==> Row:67 Col:27 */
    {0x614C,0xBBC5}, /* 慌: GB2312 Code: 0xBBC5 ==> Row:27 Col:37 */
    {0x614E,0xC9F7}, /* 慎: GB2312 Code: 0xC9F7 ==> Row:41 Col:87 */
    {0x6151,0xC9E5}, /* 慑: GB2312 Code: 0xC9E5 ==> Row:41 Col:69 */
    {0x6155,0xC4BD}, /* 慕: GB2312 Code: 0xC4BD ==> Row:36 Col:29 */
    {0x615D,0xEDAB}, /* 慝: GB2312 Code: 0xEDAB ==> Row:77 Col:11 */
    {0x6162,0xC2FD}, /* 慢: GB2312 Code: 0xC2FD ==> Row:34 Col:93 */
    {0x6167,0xBBDB}, /* 慧: GB2312 Code: 0xBBDB ==> Row:27 Col:59 */
    {0x6168,0xBFAE}, /* 慨: GB2312 Code: 0xBFAE ==> Row:31 Col:14 */
    {0x6170,0xCEBF}, /* 慰: GB2312 Code: 0xCEBF ==> Row:46 Col:31 */
    {0x6175,0xE3BC}, /* 慵: GB2312 Code: 0xE3BC ==> Row:67 Col:28 */
    {0x6177,0xBFB6}, /* 慷: GB2312 Code: 0xBFB6 ==> Row:31 Col:22 */
    {0x618B,0xB1EF}, /* 憋: GB2312 Code: 0xB1EF ==> Row:17 Col:79 */
    {0x618E,0xD4F7}, /* 憎: GB2312 Code: 0xD4F7 ==> Row:52 Col:87 */
    {0x6194,0xE3BE}, /* 憔: GB2312 Code: 0xE3BE ==> Row:67 Col:30 */
    {0x619D,0xEDAD}, /* 憝: GB2312 Code: 0xEDAD ==> Row:77 Col:13 */
    {0x61A7,0xE3BF}, /* 憧: GB2312 Code: 0xE3BF ==> Row:67 Col:31 */
    {0x61A8,0xBAA9}, /* 憨: GB2312 Code: 0xBAA9 ==> Row:26 Col:09 */
    {0x61A9,0xEDAC}, /* 憩: GB2312 Code: 0xEDAC ==> Row:77 Col:12 */
    {0x61AC,0xE3BD}, /* 憬: GB2312 Code: 0xE3BD ==> Row:67 Col:29 */
    {0x61B7,0xE3C0}, /* 憷: GB2312 Code: 0xE3C0 ==> Row:67 Col:32 */
    {0x61BE,0xBAB6}, /* 憾: GB2312 Code: 0xBAB6 ==> Row:26 Col:22 */
    {0x61C2,0xB6AE}, /* 懂: GB2312 Code: 0xB6AE ==> Row:22 Col:14 */
    {0x61C8,0xD0B8}, /* 懈: GB2312 Code: 0xD0B8 ==> Row:48 Col:24 */
    {0x61CA,0xB0C3}, /* 懊: GB2312 Code: 0xB0C3 ==> Row:16 Col:35 */
    {0x61CB,0xEDAE}, /* 懋: GB2312 Code: 0xEDAE ==> Row:77 Col:14 */
    {0x61D1,0xEDAF}, /* 懑: GB2312 Code: 0xEDAF ==> Row:77 Col:15 */
    {0x61D2,0xC0C1}, /* 懒: GB2312 Code: 0xC0C1 ==> Row:32 Col:33 */
    {0x61D4,0xE3C1}, /* 懔: GB2312 Code: 0xE3C1 ==> Row:67 Col:33 */
    {0x61E6,0xC5B3}, /* 懦: GB2312 Code: 0xC5B3 ==> Row:37 Col:19 */
    {0x61F5,0xE3C2}, /* 懵: GB2312 Code: 0xE3C2 ==> Row:67 Col:34 */
    {0x61FF,0xDCB2}, /* 懿: GB2312 Code: 0xDCB2 ==> Row:60 Col:18 */
    {0x6206,0xEDB0}, /* 戆: GB2312 Code: 0xEDB0 ==> Row:77 Col:16 */
    {0x6208,0xB8EA}, /* 戈: GB2312 Code: 0xB8EA ==> Row:24 Col:74 */
    {0x620A,0xCEEC}, /* 戊: GB2312 Code: 0xCEEC ==> Row:46 Col:76 */
    {0x620B,0xEAA7}, /* 戋: GB2312 Code: 0xEAA7 ==> Row:74 Col:07 */
    {0x620C,0xD0E7}, /* 戌: GB2312 Code: 0xD0E7 ==> Row:48 Col:71 */
    {0x620D,0xCAF9}, /* 戍: GB2312 Code: 0xCAF9 ==> Row:42 Col:89 */
    {0x620E,0xC8D6}, /* 戎: GB2312 Code: 0xC8D6 ==> Row:40 Col:54 */
    {0x620F,0xCFB7}, /* 戏: GB2312 Code: 0xCFB7 ==> Row:47 Col:23 */
    {0x6210,0xB3C9}, /* 成: GB2312 Code: 0xB3C9 ==> Row:19 Col:41 */
    {0x6211,0xCED2}, /* 我: GB2312 Code: 0xCED2 ==> Row:46 Col:50 */
    {0x6212,0xBDE4}, /* 戒: GB2312 Code: 0xBDE4 ==> Row:29 Col:68 */
    {0x6215,0xE3DE}, /* 戕: GB2312 Code: 0xE3DE ==> Row:67 Col:62 */
    {0x6216,0xBBF2}, /* 或: GB2312 Code: 0xBBF2 ==> Row:27 Col:82 */
    {0x6217,0xEAA8}, /* 戗: GB2312 Code: 0xEAA8 ==> Row:74 Col:08 */
    {0x6218,0xD5BD}, /* 战: GB2312 Code: 0xD5BD ==> Row:53 Col:29 */
    {0x621A,0xC6DD}, /* 戚: GB2312 Code: 0xC6DD ==> Row:38 Col:61 */
    {0x621B,0xEAA9}, /* 戛: GB2312 Code: 0xEAA9 ==> Row:74 Col:09 */
    {0x621F,0xEAAA}, /* 戟: GB2312 Code: 0xEAAA ==> Row:74 Col:10 */
    {0x6221,0xEAAC}, /* 戡: GB2312 Code: 0xEAAC ==> Row:74 Col:12 */
    {0x6222,0xEAAB}, /* 戢: GB2312 Code: 0xEAAB ==> Row:74 Col:11 */
    {0x6224,0xEAAE}, /* 戤: GB2312 Code: 0xEAAE ==> Row:74 Col:14 */
    {0x6225,0xEAAD}, /* 戥: GB2312 Code: 0xEAAD ==> Row:74 Col:13 */
    {0x622A,0xBDD8}, /* 截: GB2312 Code: 0xBDD8 ==> Row:29 Col:56 */
    {0x622C,0xEAAF}, /* 戬: GB2312 Code: 0xEAAF ==> Row:74 Col:15 */
    {0x622E,0xC2BE}, /* 戮: GB2312 Code: 0xC2BE ==> Row:34 Col:30 */
    {0x6233,0xB4C1}, /* 戳: GB2312 Code: 0xB4C1 ==> Row:20 Col:33 */
    {0x6234,0xB4F7}, /* 戴: GB2312 Code: 0xB4F7 ==> Row:20 Col:87 */
    {0x6237,0xBBA7}, /* 户: GB2312 Code: 0xBBA7 ==> Row:27 Col:07 */
    {0x623D,0xECE6}, /* 戽: GB2312 Code: 0xECE6 ==> Row:76 Col:70 */
    {0x623E,0xECE5}, /* 戾: GB2312 Code: 0xECE5 ==> Row:76 Col:69 */
    {0x623F,0xB7BF}, /* 房: GB2312 Code: 0xB7BF ==> Row:23 Col:31 */
    {0x6240,0xCBF9}, /* 所: GB2312 Code: 0xCBF9 ==> Row:43 Col:89 */
    {0x6241,0xB1E2}, /* 扁: GB2312 Code: 0xB1E2 ==> Row:17 Col:66 */
    {0x6243,0xECE7}, /* 扃: GB2312 Code: 0xECE7 ==> Row:76 Col:71 */
    {0x6247,0xC9C8}, /* 扇: GB2312 Code: 0xC9C8 ==> Row:41 Col:40 */
    {0x6248,0xECE8}, /* 扈: GB2312 Code: 0xECE8 ==> Row:76 Col:72 */
    {0x6249,0xECE9}, /* 扉: GB2312 Code: 0xECE9 ==> Row:76 Col:73 */
    {0x624B,0xCAD6}, /* 手: GB2312 Code: 0xCAD6 ==> Row:42 Col:54 */
    {0x624C,0xDED0}, /* 扌: GB2312 Code: 0xDED0 ==> Row:62 Col:48 */
    {0x624D,0xB2C5}, /* 才: GB2312 Code: 0xB2C5 ==> Row:18 Col:37 */
    {0x624E,0xD4FA}, /* 扎: GB2312 Code: 0xD4FA ==> Row:52 Col:90 */
    {0x6251,0xC6CB}, /* 扑: GB2312 Code: 0xC6CB ==> Row:38 Col:43 */
    {0x6252,0xB0C7}, /* 扒: GB2312 Code: 0xB0C7 ==> Row:16 Col:39 */
    {0x6253,0xB4F2}, /* 打: GB2312 Code: 0xB4F2 ==> Row:20 Col:82 */
    {0x6254,0xC8D3}, /* 扔: GB2312 Code: 0xC8D3 ==> Row:40 Col:51 */
    {0x6258,0xCDD0}, /* 托: GB2312 Code: 0xCDD0 ==> Row:45 Col:48 */
    {0x625B,0xBFB8}, /* 扛: GB2312 Code: 0xBFB8 ==> Row:31 Col:24 */
    {0x6263,0xBFDB}, /* 扣: GB2312 Code: 0xBFDB ==> Row:31 Col:59 */
    {0x6266,0xC7A4}, /* 扦: GB2312 Code: 0xC7A4 ==> Row:39 Col:04 */
    {0x6267,0xD6B4}, /* 执: GB2312 Code: 0xD6B4 ==> Row:54 Col:20 */
    {0x6269,0xC0A9}, /* 扩: GB2312 Code: 0xC0A9 ==> Row:32 Col:09 */
    {0x626A,0xDED1}, /* 扪: GB2312 Code: 0xDED1 ==> Row:62 Col:49 */
    {0x626B,0xC9A8}, /* 扫: GB2312 Code: 0xC9A8 ==> Row:41 Col:08 */
    {0x626C,0xD1EF}, /* 扬: GB2312 Code: 0xD1EF ==> Row:49 Col:79 */
    {0x626D,0xC5A4}, /* 扭: GB2312 Code: 0xC5A4 ==> Row:37 Col:04 */
    {0x626E,0xB0E7}, /* 扮: GB2312 Code: 0xB0E7 ==> Row:16 Col:71 */
    {0x626F,0xB3B6}, /* 扯: GB2312 Code: 0xB3B6 ==> Row:19 Col:22 */
    {0x6270,0xC8C5}, /* 扰: GB2312 Code: 0xC8C5 ==> Row:40 Col:37 */
    {0x6273,0xB0E2}, /* 扳: GB2312 Code: 0xB0E2 ==> Row:16 Col:66 */
    {0x6276,0xB7F6}, /* 扶: GB2312 Code: 0xB7F6 ==> Row:23 Col:86 */
    {0x6279,0xC5FA}, /* 批: GB2312 Code: 0xC5FA ==> Row:37 Col:90 */
    {0x627C,0xB6F3}, /* 扼: GB2312 Code: 0xB6F3 ==> Row:22 Col:83 */
    {0x627E,0xD5D2}, /* 找: GB2312 Code: 0xD5D2 ==> Row:53 Col:50 */
    {0x627F,0xB3D0}, /* 承: GB2312 Code: 0xB3D0 ==> Row:19 Col:48 */
    {0x6280,0xBCBC}, /* 技: GB2312 Code: 0xBCBC ==> Row:28 Col:28 */
    {0x6284,0xB3AD}, /* 抄: GB2312 Code: 0xB3AD ==> Row:19 Col:13 */
    {0x6289,0xBEF1}, /* 抉: GB2312 Code: 0xBEF1 ==> Row:30 Col:81 */
    {0x628A,0xB0D1}, /* 把: GB2312 Code: 0xB0D1 ==> Row:16 Col:49 */
    {0x6291,0xD2D6}, /* 抑: GB2312 Code: 0xD2D6 ==> Row:50 Col:54 */
    {0x6292,0xCAE3}, /* 抒: GB2312 Code: 0xCAE3 ==> Row:42 Col:67 */
    {0x6293,0xD7A5}, /* 抓: GB2312 Code: 0xD7A5 ==> Row:55 Col:05 */
    {0x6295,0xCDB6}, /* 投: GB2312 Code: 0xCDB6 ==> Row:45 Col:22 */
    {0x6296,0xB6B6}, /* 抖: GB2312 Code: 0xB6B6 ==> Row:22 Col:22 */
    {0x6297,0xBFB9}, /* 抗: GB2312 Code: 0xBFB9 ==> Row:31 Col:25 */
    {0x6298,0xD5DB}, /* 折: GB2312 Code: 0xD5DB ==> Row:53 Col:59 */
    {0x629A,0xB8A7}, /* 抚: GB2312 Code: 0xB8A7 ==> Row:24 Col:07 */
    {0x629B,0xC5D7}, /* 抛: GB2312 Code: 0xC5D7 ==> Row:37 Col:55 */
    {0x629F,0xDED2}, /* 抟: GB2312 Code: 0xDED2 ==> Row:62 Col:50 */
    {0x62A0,0xBFD9}, /* 抠: GB2312 Code: 0xBFD9 ==> Row:31 Col:57 */
    {0x62A1,0xC2D5}, /* 抡: GB2312 Code: 0xC2D5 ==> Row:34 Col:53 */
    {0x62A2,0xC7C0}, /* 抢: GB2312 Code: 0xC7C0 ==> Row:39 Col:32 */
    {0x62A4,0xBBA4}, /* 护: GB2312 Code: 0xBBA4 ==> Row:27 Col:04 */
    {0x62A5,0xB1A8}, /* 报: GB2312 Code: 0xB1A8 ==> Row:17 Col:08 */
    {0x62A8,0xC5EA}, /* 抨: GB2312 Code: 0xC5EA ==> Row:37 Col:74 */
    {0x62AB,0xC5FB}, /* 披: GB2312 Code: 0xC5FB ==> Row:37 Col:91 */
    {0x62AC,0xCCA7}, /* 抬: GB2312 Code: 0xCCA7 ==> Row:44 Col:07 */
    {0x62B1,0xB1A7}, /* 抱: GB2312 Code: 0xB1A7 ==> Row:17 Col:07 */
    {0x62B5,0xB5D6}, /* 抵: GB2312 Code: 0xB5D6 ==> Row:21 Col:54 */
    {0x62B9,0xC4A8}, /* 抹: GB2312 Code: 0xC4A8 ==> Row:36 Col:08 */
    {0x62BB,0xDED3}, /* 抻: GB2312 Code: 0xDED3 ==> Row:62 Col:51 */
    {0x62BC,0xD1BA}, /* 押: GB2312 Code: 0xD1BA ==> Row:49 Col:26 */
    {0x62BD,0xB3E9}, /* 抽: GB2312 Code: 0xB3E9 ==> Row:19 Col:73 */
    {0x62BF,0xC3F2}, /* 抿: GB2312 Code: 0xC3F2 ==> Row:35 Col:82 */
    {0x62C2,0xB7F7}, /* 拂: GB2312 Code: 0xB7F7 ==> Row:23 Col:87 */
    {0x62C4,0xD6F4}, /* 拄: GB2312 Code: 0xD6F4 ==> Row:54 Col:84 */
    {0x62C5,0xB5A3}, /* 担: GB2312 Code: 0xB5A3 ==> Row:21 Col:03 */
    {0x62C6,0xB2F0}, /* 拆: GB2312 Code: 0xB2F0 ==> Row:18 Col:80 */
    {0x62C7,0xC4B4}, /* 拇: GB2312 Code: 0xC4B4 ==> Row:36 Col:20 */
    {0x62C8,0xC4E9}, /* 拈: GB2312 Code: 0xC4E9 ==> Row:36 Col:73 */
    {0x62C9,0xC0AD}, /* 拉: GB2312 Code: 0xC0AD ==> Row:32 Col:13 */
    {0x62CA,0xDED4}, /* 拊: GB2312 Code: 0xDED4 ==> Row:62 Col:52 */
    {0x62CC,0xB0E8}, /* 拌: GB2312 Code: 0xB0E8 ==> Row:16 Col:72 */
    {0x62CD,0xC5C4}, /* 拍: GB2312 Code: 0xC5C4 ==> Row:37 Col:36 */
    {0x62CE,0xC1E0}, /* 拎: GB2312 Code: 0xC1E0 ==> Row:33 Col:64 */
    {0x62D0,0xB9D5}, /* 拐: GB2312 Code: 0xB9D5 ==> Row:25 Col:53 */
    {0x62D2,0xBEDC}, /* 拒: GB2312 Code: 0xBEDC ==> Row:30 Col:60 */
    {0x62D3,0xCDD8}, /* 拓: GB2312 Code: 0xCDD8 ==> Row:45 Col:56 */
    {0x62D4,0xB0CE}, /* 拔: GB2312 Code: 0xB0CE ==> Row:16 Col:46 */
    {0x62D6,0xCDCF}, /* 拖: GB2312 Code: 0xCDCF ==> Row:45 Col:47 */
    {0x62D7,0xDED6}, /* 拗: GB2312 Code: 0xDED6 ==> Row:62 Col:54 */
    {0x62D8,0xBED0}, /* 拘: GB2312 Code: 0xBED0 ==> Row:30 Col:48 */
    {0x62D9,0xD7BE}, /* 拙: GB2312 Code: 0xD7BE ==> Row:55 Col:30 */
    {0x62DA,0xDED5}, /* 拚: GB2312 Code: 0xDED5 ==> Row:62 Col:53 */
    {0x62DB,0xD5D0}, /* 招: GB2312 Code: 0xD5D0 ==> Row:53 Col:48 */
    {0x62DC,0xB0DD}, /* 拜: GB2312 Code: 0xB0DD ==> Row:16 Col:61 */
    {0x62DF,0xC4E2}, /* 拟: GB2312 Code: 0xC4E2 ==> Row:36 Col:66 */
    {0x62E2,0xC2A3}, /* 拢: GB2312 Code: 0xC2A3 ==> Row:34 Col:03 */
    {0x62E3,0xBCF0}, /* 拣: GB2312 Code: 0xBCF0 ==> Row:28 Col:80 */
    {0x62E5,0xD3B5}, /* 拥: GB2312 Code: 0xD3B5 ==> Row:51 Col:21 */
    {0x62E6,0xC0B9}, /* 拦: GB2312 Code: 0xC0B9 ==> Row:32 Col:25 */
    {0x62E7,0xC5A1}, /* 拧: GB2312 Code: 0xC5A1 ==> Row:37 Col:01 */
    {0x62E8,0xB2A6}, /* 拨: GB2312 Code: 0xB2A6 ==> Row:18 Col:06 */
    {0x62E9,0xD4F1}, /* 择: GB2312 Code: 0xD4F1 ==> Row:52 Col:81 */
    {0x62EC,0xC0A8}, /* 括: GB2312 Code: 0xC0A8 ==> Row:32 Col:08 */
    {0x62ED,0xCAC3}, /* 拭: GB2312 Code: 0xCAC3 ==> Row:42 Col:35 */
    {0x62EE,0xDED7}, /* 拮: GB2312 Code: 0xDED7 ==> Row:62 Col:55 */
    {0x62EF,0xD5FC}, /* 拯: GB2312 Code: 0xD5FC ==> Row:53 Col:92 */
    {0x62F1,0xB9B0}, /* 拱: GB2312 Code: 0xB9B0 ==> Row:25 Col:16 */
    {0x62F3,0xC8AD}, /* 拳: GB2312 Code: 0xC8AD ==> Row:40 Col:13 */
    {0x62F4,0xCBA9}, /* 拴: GB2312 Code: 0xCBA9 ==> Row:43 Col:09 */
    {0x62F6,0xDED9}, /* 拶: GB2312 Code: 0xDED9 ==> Row:62 Col:57 */
    {0x62F7,0xBFBD}, /* 拷: GB2312 Code: 0xBFBD ==> Row:31 Col:29 */
    {0x62FC,0xC6B4}, /* 拼: GB2312 Code: 0xC6B4 ==> Row:38 Col:20 */
    {0x62FD,0xD7A7}, /* 拽: GB2312 Code: 0xD7A7 ==> Row:55 Col:07 */
    {0x62FE,0xCAB0}, /* 拾: GB2312 Code: 0xCAB0 ==> Row:42 Col:16 */
    {0x62FF,0xC4C3}, /* 拿: GB2312 Code: 0xC4C3 ==> Row:36 Col:35 */
    {0x6301,0xB3D6}, /* 持: GB2312 Code: 0xB3D6 ==> Row:19 Col:54 */
    {0x6302,0xB9D2}, /* 挂: GB2312 Code: 0xB9D2 ==> Row:25 Col:50 */
    {0x6307,0xD6B8}, /* 指: GB2312 Code: 0xD6B8 ==> Row:54 Col:24 */
    {0x6308,0xEAFC}, /* 挈: GB2312 Code: 0xEAFC ==> Row:74 Col:92 */
    {0x6309,0xB0B4}, /* 按: GB2312 Code: 0xB0B4 ==> Row:16 Col:20 */
    {0x630E,0xBFE6}, /* 挎: GB2312 Code: 0xBFE6 ==> Row:31 Col:70 */
    {0x6311,0xCCF4}, /* 挑: GB2312 Code: 0xCCF4 ==> Row:44 Col:84 */
    {0x6316,0xCDDA}, /* 挖: GB2312 Code: 0xCDDA ==> Row:45 Col:58 */
    {0x631A,0xD6BF}, /* 挚: GB2312 Code: 0xD6BF ==> Row:54 Col:31 */
    {0x631B,0xC2CE}, /* 挛: GB2312 Code: 0xC2CE ==> Row:34 Col:46 */
    {0x631D,0xCECE}, /* 挝: GB2312 Code: 0xCECE ==> Row:46 Col:46 */
    {0x631E,0xCCA2}, /* 挞: GB2312 Code: 0xCCA2 ==> Row:44 Col:02 */
    {0x631F,0xD0AE}, /* 挟: GB2312 Code: 0xD0AE ==> Row:48 Col:14 */
    {0x6320,0xC4D3}, /* 挠: GB2312 Code: 0xC4D3 ==> Row:36 Col:51 */
    {0x6321,0xB5B2}, /* 挡: GB2312 Code: 0xB5B2 ==> Row:21 Col:18 */
    {0x6322,0xDED8}, /* 挢: GB2312 Code: 0xDED8 ==> Row:62 Col:56 */
    {0x6323,0xD5F5}, /* 挣: GB2312 Code: 0xD5F5 ==> Row:53 Col:85 */
    {0x6324,0xBCB7}, /* 挤: GB2312 Code: 0xBCB7 ==> Row:28 Col:23 */
    {0x6325,0xBBD3}, /* 挥: GB2312 Code: 0xBBD3 ==> Row:27 Col:51 */
    {0x6328,0xB0A4}, /* 挨: GB2312 Code: 0xB0A4 ==> Row:16 Col:04 */
    {0x632A,0xC5B2}, /* 挪: GB2312 Code: 0xC5B2 ==> Row:37 Col:18 */
    {0x632B,0xB4EC}, /* 挫: GB2312 Code: 0xB4EC ==> Row:20 Col:76 */
    {0x632F,0xD5F1}, /* 振: GB2312 Code: 0xD5F1 ==> Row:53 Col:81 */
    {0x6332,0xEAFD}, /* 挲: GB2312 Code: 0xEAFD ==> Row:74 Col:93 */
    {0x6339,0xDEDA}, /* 挹: GB2312 Code: 0xDEDA ==> Row:62 Col:58 */
    {0x633A,0xCDA6}, /* 挺: GB2312 Code: 0xCDA6 ==> Row:45 Col:06 */
    {0x633D,0xCDEC}, /* 挽: GB2312 Code: 0xCDEC ==> Row:45 Col:76 */
    {0x6342,0xCEE6}, /* 捂: GB2312 Code: 0xCEE6 ==> Row:46 Col:70 */
    {0x6343,0xDEDC}, /* 捃: GB2312 Code: 0xDEDC ==> Row:62 Col:60 */
    {0x6345,0xCDB1}, /* 捅: GB2312 Code: 0xCDB1 ==> Row:45 Col:17 */
    {0x6346,0xC0A6}, /* 捆: GB2312 Code: 0xC0A6 ==> Row:32 Col:06 */
    {0x6349,0xD7BD}, /* 捉: GB2312 Code: 0xD7BD ==> Row:55 Col:29 */
    {0x634B,0xDEDB}, /* 捋: GB2312 Code: 0xDEDB ==> Row:62 Col:59 */
    {0x634C,0xB0C6}, /* 捌: GB2312 Code: 0xB0C6 ==> Row:16 Col:38 */
    {0x634D,0xBAB4}, /* 捍: GB2312 Code: 0xBAB4 ==> Row:26 Col:20 */
    {0x634E,0xC9D3}, /* 捎: GB2312 Code: 0xC9D3 ==> Row:41 Col:51 */
    {0x634F,0xC4F3}, /* 捏: GB2312 Code: 0xC4F3 ==> Row:36 Col:83 */
    {0x6350,0xBEE8}, /* 捐: GB2312 Code: 0xBEE8 ==> Row:30 Col:72 */
    {0x6355,0xB2B6}, /* 捕: GB2312 Code: 0xB2B6 ==> Row:18 Col:22 */
    {0x635E,0xC0CC}, /* 捞: GB2312 Code: 0xC0CC ==> Row:32 Col:44 */
    {0x635F,0xCBF0}, /* 损: GB2312 Code: 0xCBF0 ==> Row:43 Col:80 */
    {0x6361,0xBCF1}, /* 捡: GB2312 Code: 0xBCF1 ==> Row:28 Col:81 */
    {0x6362,0xBBBB}, /* 换: GB2312 Code: 0xBBBB ==> Row:27 Col:27 */
    {0x6363,0xB5B7}, /* 捣: GB2312 Code: 0xB5B7 ==> Row:21 Col:23 */
    {0x6367,0xC5F5}, /* 捧: GB2312 Code: 0xC5F5 ==> Row:37 Col:85 */
    {0x6369,0xDEE6}, /* 捩: GB2312 Code: 0xDEE6 ==> Row:62 Col:70 */
    {0x636D,0xDEE3}, /* 捭: GB2312 Code: 0xDEE3 ==> Row:62 Col:67 */
    {0x636E,0xBEDD}, /* 据: GB2312 Code: 0xBEDD ==> Row:30 Col:61 */
    {0x6371,0xDEDF}, /* 捱: GB2312 Code: 0xDEDF ==> Row:62 Col:63 */
    {0x6376,0xB4B7}, /* 捶: GB2312 Code: 0xB4B7 ==> Row:20 Col:23 */
    {0x6377,0xBDDD}, /* 捷: GB2312 Code: 0xBDDD ==> Row:29 Col:61 */
    {0x637A,0xDEE0}, /* 捺: GB2312 Code: 0xDEE0 ==> Row:62 Col:64 */
    {0x637B,0xC4ED}, /* 捻: GB2312 Code: 0xC4ED ==> Row:36 Col:77 */
    {0x6380,0xCFC6}, /* 掀: GB2312 Code: 0xCFC6 ==> Row:47 Col:38 */
    {0x6382,0xB5E0}, /* 掂: GB2312 Code: 0xB5E0 ==> Row:21 Col:64 */
    {0x6387,0xB6DE}, /* 掇: GB2312 Code: 0xB6DE ==> Row:22 Col:62 */
    {0x6388,0xCADA}, /* 授: GB2312 Code: 0xCADA ==> Row:42 Col:58 */
    {0x6389,0xB5F4}, /* 掉: GB2312 Code: 0xB5F4 ==> Row:21 Col:84 */
    {0x638A,0xDEE5}, /* 掊: GB2312 Code: 0xDEE5 ==> Row:62 Col:69 */
    {0x638C,0xD5C6}, /* 掌: GB2312 Code: 0xD5C6 ==> Row:53 Col:38 */
    {0x638E,0xDEE1}, /* 掎: GB2312 Code: 0xDEE1 ==> Row:62 Col:65 */
    {0x638F,0xCCCD}, /* 掏: GB2312 Code: 0xCCCD ==> Row:44 Col:45 */
    {0x6390,0xC6FE}, /* 掐: GB2312 Code: 0xC6FE ==> Row:38 Col:94 */
    {0x6392,0xC5C5}, /* 排: GB2312 Code: 0xC5C5 ==> Row:37 Col:37 */
    {0x6396,0xD2B4}, /* 掖: GB2312 Code: 0xD2B4 ==> Row:50 Col:20 */
    {0x6398,0xBEF2}, /* 掘: GB2312 Code: 0xBEF2 ==> Row:30 Col:82 */
    {0x63A0,0xC2D3}, /* 掠: GB2312 Code: 0xC2D3 ==> Row:34 Col:51 */
    {0x63A2,0xCCBD}, /* 探: GB2312 Code: 0xCCBD ==> Row:44 Col:29 */
    {0x63A3,0xB3B8}, /* 掣: GB2312 Code: 0xB3B8 ==> Row:19 Col:24 */
    {0x63A5,0xBDD3}, /* 接: GB2312 Code: 0xBDD3 ==> Row:29 Col:51 */
    {0x63A7,0xBFD8}, /* 控: GB2312 Code: 0xBFD8 ==> Row:31 Col:56 */
    {0x63A8,0xCDC6}, /* 推: GB2312 Code: 0xCDC6 ==> Row:45 Col:38 */
    {0x63A9,0xD1DA}, /* 掩: GB2312 Code: 0xD1DA ==> Row:49 Col:58 */
    {0x63AA,0xB4EB}, /* 措: GB2312 Code: 0xB4EB ==> Row:20 Col:75 */
    {0x63AC,0xDEE4}, /* 掬: GB2312 Code: 0xDEE4 ==> Row:62 Col:68 */
    {0x63AD,0xDEDD}, /* 掭: GB2312 Code: 0xDEDD ==> Row:62 Col:61 */
    {0x63AE,0xDEE7}, /* 掮: GB2312 Code: 0xDEE7 ==> Row:62 Col:71 */
    {0x63B0,0xEAFE}, /* 掰: GB2312 Code: 0xEAFE ==> Row:74 Col:94 */
    {0x63B3,0xC2B0}, /* 掳: GB2312 Code: 0xC2B0 ==> Row:34 Col:16 */
    {0x63B4,0xDEE2}, /* 掴: GB2312 Code: 0xDEE2 ==> Row:62 Col:66 */
    {0x63B7,0xD6C0}, /* 掷: GB2312 Code: 0xD6C0 ==> Row:54 Col:32 */
    {0x63B8,0xB5A7}, /* 掸: GB2312 Code: 0xB5A7 ==> Row:21 Col:07 */
    {0x63BA,0xB2F4}, /* 掺: GB2312 Code: 0xB2F4 ==> Row:18 Col:84 */
    {0x63BC,0xDEE8}, /* 掼: GB2312 Code: 0xDEE8 ==> Row:62 Col:72 */
    {0x63BE,0xDEF2}, /* 掾: GB2312 Code: 0xDEF2 ==> Row:62 Col:82 */
    {0x63C4,0xDEED}, /* 揄: GB2312 Code: 0xDEED ==> Row:62 Col:77 */
    {0x63C6,0xDEF1}, /* 揆: GB2312 Code: 0xDEF1 ==> Row:62 Col:81 */
    {0x63C9,0xC8E0}, /* 揉: GB2312 Code: 0xC8E0 ==> Row:40 Col:64 */
    {0x63CD,0xD7E1}, /* 揍: GB2312 Code: 0xD7E1 ==> Row:55 Col:65 */
    {0x63CE,0xDEEF}, /* 揎: GB2312 Code: 0xDEEF ==> Row:62 Col:79 */
    {0x63CF,0xC3E8}, /* 描: GB2312 Code: 0xC3E8 ==> Row:35 Col:72 */
    {0x63D0,0xCCE1}, /* 提: GB2312 Code: 0xCCE1 ==> Row:44 Col:65 */
    {0x63D2,0xB2E5}, /* 插: GB2312 Code: 0xB2E5 ==> Row:18 Col:69 */
    {0x63D6,0xD2BE}, /* 揖: GB2312 Code: 0xD2BE ==> Row:50 Col:30 */
    {0x63DE,0xDEEE}, /* 揞: GB2312 Code: 0xDEEE ==> Row:62 Col:78 */
    {0x63E0,0xDEEB}, /* 揠: GB2312 Code: 0xDEEB ==> Row:62 Col:75 */
    {0x63E1,0xCED5}, /* 握: GB2312 Code: 0xCED5 ==> Row:46 Col:53 */
    {0x63E3,0xB4A7}, /* 揣: GB2312 Code: 0xB4A7 ==> Row:20 Col:07 */
    {0x63E9,0xBFAB}, /* 揩: GB2312 Code: 0xBFAB ==> Row:31 Col:11 */
    {0x63EA,0xBEBE}, /* 揪: GB2312 Code: 0xBEBE ==> Row:30 Col:30 */
    {0x63ED,0xBDD2}, /* 揭: GB2312 Code: 0xBDD2 ==> Row:29 Col:50 */
    {0x63F2,0xDEE9}, /* 揲: GB2312 Code: 0xDEE9 ==> Row:62 Col:73 */
    {0x63F4,0xD4AE}, /* 援: GB2312 Code: 0xD4AE ==> Row:52 Col:14 */
    {0x63F6,0xDEDE}, /* 揶: GB2312 Code: 0xDEDE ==> Row:62 Col:62 */
    {0x63F8,0xDEEA}, /* 揸: GB2312 Code: 0xDEEA ==> Row:62 Col:74 */
    {0x63FD,0xC0BF}, /* 揽: GB2312 Code: 0xC0BF ==> Row:32 Col:31 */
    {0x63FF,0xDEEC}, /* 揿: GB2312 Code: 0xDEEC ==> Row:62 Col:76 */
    {0x6400,0xB2F3}, /* 搀: GB2312 Code: 0xB2F3 ==> Row:18 Col:83 */
    {0x6401,0xB8E9}, /* 搁: GB2312 Code: 0xB8E9 ==> Row:24 Col:73 */
    {0x6402,0xC2A7}, /* 搂: GB2312 Code: 0xC2A7 ==> Row:34 Col:07 */
    {0x6405,0xBDC1}, /* 搅: GB2312 Code: 0xBDC1 ==> Row:29 Col:33 */
    {0x640B,0xDEF5}, /* 搋: GB2312 Code: 0xDEF5 ==> Row:62 Col:85 */
    {0x640C,0xDEF8}, /* 搌: GB2312 Code: 0xDEF8 ==> Row:62 Col:88 */
    {0x640F,0xB2AB}, /* 搏: GB2312 Code: 0xB2AB ==> Row:18 Col:11 */
    {0x6410,0xB4A4}, /* 搐: GB2312 Code: 0xB4A4 ==> Row:20 Col:04 */
    {0x6413,0xB4EA}, /* 搓: GB2312 Code: 0xB4EA ==> Row:20 Col:74 */
    {0x6414,0xC9A6}, /* 搔: GB2312 Code: 0xC9A6 ==> Row:41 Col:06 */
    {0x641B,0xDEF6}, /* 搛: GB2312 Code: 0xDEF6 ==> Row:62 Col:86 */
    {0x641C,0xCBD1}, /* 搜: GB2312 Code: 0xCBD1 ==> Row:43 Col:49 */
    {0x641E,0xB8E3}, /* 搞: GB2312 Code: 0xB8E3 ==> Row:24 Col:67 */
    {0x6420,0xDEF7}, /* 搠: GB2312 Code: 0xDEF7 ==> Row:62 Col:87 */
    {0x6421,0xDEFA}, /* 搡: GB2312 Code: 0xDEFA ==> Row:62 Col:90 */
    {0x6426,0xDEF9}, /* 搦: GB2312 Code: 0xDEF9 ==> Row:62 Col:89 */
    {0x642A,0xCCC2}, /* 搪: GB2312 Code: 0xCCC2 ==> Row:44 Col:34 */
    {0x642C,0xB0E1}, /* 搬: GB2312 Code: 0xB0E1 ==> Row:16 Col:65 */
    {0x642D,0xB4EE}, /* 搭: GB2312 Code: 0xB4EE ==> Row:20 Col:78 */
    {0x6434,0xE5BA}, /* 搴: GB2312 Code: 0xE5BA ==> Row:69 Col:26 */
    {0x643A,0xD0AF}, /* 携: GB2312 Code: 0xD0AF ==> Row:48 Col:15 */
    {0x643D,0xB2EB}, /* 搽: GB2312 Code: 0xB2EB ==> Row:18 Col:75 */
    {0x643F,0xEBA1}, /* 搿: GB2312 Code: 0xEBA1 ==> Row:75 Col:01 */
    {0x6441,0xDEF4}, /* 摁: GB2312 Code: 0xDEF4 ==> Row:62 Col:84 */
    {0x6444,0xC9E3}, /* 摄: GB2312 Code: 0xC9E3 ==> Row:41 Col:67 */
    {0x6445,0xDEF3}, /* 摅: GB2312 Code: 0xDEF3 ==> Row:62 Col:83 */
    {0x6446,0xB0DA}, /* 摆: GB2312 Code: 0xB0DA ==> Row:16 Col:58 */
    {0x6447,0xD2A1}, /* 摇: GB2312 Code: 0xD2A1 ==> Row:50 Col:01 */
    {0x6448,0xB1F7}, /* 摈: GB2312 Code: 0xB1F7 ==> Row:17 Col:87 */
    {0x644A,0xCCAF}, /* 摊: GB2312 Code: 0xCCAF ==> Row:44 Col:15 */
    {0x6452,0xDEF0}, /* 摒: GB2312 Code: 0xDEF0 ==> Row:62 Col:80 */
    {0x6454,0xCBA4}, /* 摔: GB2312 Code: 0xCBA4 ==> Row:43 Col:04 */
    {0x6458,0xD5AA}, /* 摘: GB2312 Code: 0xD5AA ==> Row:53 Col:10 */
    {0x645E,0xDEFB}, /* 摞: GB2312 Code: 0xDEFB ==> Row:62 Col:91 */
    {0x6467,0xB4DD}, /* 摧: GB2312 Code: 0xB4DD ==> Row:20 Col:61 */
    {0x6469,0xC4A6}, /* 摩: GB2312 Code: 0xC4A6 ==> Row:36 Col:06 */
    {0x646D,0xDEFD}, /* 摭: GB2312 Code: 0xDEFD ==> Row:62 Col:93 */
    {0x6478,0xC3FE}, /* 摸: GB2312 Code: 0xC3FE ==> Row:35 Col:94 */
    {0x6479,0xC4A1}, /* 摹: GB2312 Code: 0xC4A1 ==> Row:36 Col:01 */
    {0x647A,0xDFA1}, /* 摺: GB2312 Code: 0xDFA1 ==> Row:63 Col:01 */
    {0x6482,0xC1CC}, /* 撂: GB2312 Code: 0xC1CC ==> Row:33 Col:44 */
    {0x6484,0xDEFC}, /* 撄: GB2312 Code: 0xDEFC ==> Row:62 Col:92 */
    {0x6485,0xBEEF}, /* 撅: GB2312 Code: 0xBEEF ==> Row:30 Col:79 */
    {0x6487,0xC6B2}, /* 撇: GB2312 Code: 0xC6B2 ==> Row:38 Col:18 */
    {0x6491,0xB3C5}, /* 撑: GB2312 Code: 0xB3C5 ==> Row:19 Col:37 */
    {0x6492,0xC8F6}, /* 撒: GB2312 Code: 0xC8F6 ==> Row:40 Col:86 */
    {0x6495,0xCBBA}, /* 撕: GB2312 Code: 0xCBBA ==> Row:43 Col:26 */
    {0x6496,0xDEFE}, /* 撖: GB2312 Code: 0xDEFE ==> Row:62 Col:94 */
    {0x6499,0xDFA4}, /* 撙: GB2312 Code: 0xDFA4 ==> Row:63 Col:04 */
    {0x649E,0xD7B2}, /* 撞: GB2312 Code: 0xD7B2 ==> Row:55 Col:18 */
    {0x64A4,0xB3B7}, /* 撤: GB2312 Code: 0xB3B7 ==> Row:19 Col:23 */
    {0x64A9,0xC1C3}, /* 撩: GB2312 Code: 0xC1C3 ==> Row:33 Col:35 */
    {0x64AC,0xC7CB}, /* 撬: GB2312 Code: 0xC7CB ==> Row:39 Col:43 */
    {0x64AD,0xB2A5}, /* 播: GB2312 Code: 0xB2A5 ==> Row:18 Col:05 */
    {0x64AE,0xB4E9}, /* 撮: GB2312 Code: 0xB4E9 ==> Row:20 Col:73 */
    {0x64B0,0xD7AB}, /* 撰: GB2312 Code: 0xD7AB ==> Row:55 Col:11 */
    {0x64B5,0xC4EC}, /* 撵: GB2312 Code: 0xC4EC ==> Row:36 Col:76 */
    {0x64B7,0xDFA2}, /* 撷: GB2312 Code: 0xDFA2 ==> Row:63 Col:02 */
    {0x64B8,0xDFA3}, /* 撸: GB2312 Code: 0xDFA3 ==> Row:63 Col:03 */
    {0x64BA,0xDFA5}, /* 撺: GB2312 Code: 0xDFA5 ==> Row:63 Col:05 */
    {0x64BC,0xBAB3}, /* 撼: GB2312 Code: 0xBAB3 ==> Row:26 Col:19 */
    {0x64C0,0xDFA6}, /* 擀: GB2312 Code: 0xDFA6 ==> Row:63 Col:06 */
    {0x64C2,0xC0DE}, /* 擂: GB2312 Code: 0xC0DE ==> Row:32 Col:62 */
    {0x64C5,0xC9C3}, /* 擅: GB2312 Code: 0xC9C3 ==> Row:41 Col:35 */
    {0x64CD,0xB2D9}, /* 操: GB2312 Code: 0xB2D9 ==> Row:18 Col:57 */
    {0x64CE,0xC7E6}, /* 擎: GB2312 Code: 0xC7E6 ==> Row:39 Col:70 */
    {0x64D0,0xDFA7}, /* 擐: GB2312 Code: 0xDFA7 ==> Row:63 Col:07 */
    {0x64D2,0xC7DC}, /* 擒: GB2312 Code: 0xC7DC ==> Row:39 Col:60 */
    {0x64D7,0xDFA8}, /* 擗: GB2312 Code: 0xDFA8 ==> Row:63 Col:08 */
    {0x64D8,0xEBA2}, /* 擘: GB2312 Code: 0xEBA2 ==> Row:75 Col:02 */
    {0x64DE,0xCBD3}, /* 擞: GB2312 Code: 0xCBD3 ==> Row:43 Col:51 */
    {0x64E2,0xDFAA}, /* 擢: GB2312 Code: 0xDFAA ==> Row:63 Col:10 */
    {0x64E4,0xDFA9}, /* 擤: GB2312 Code: 0xDFA9 ==> Row:63 Col:09 */
    {0x64E6,0xB2C1}, /* 擦: GB2312 Code: 0xB2C1 ==> Row:18 Col:33 */
    {0x6500,0xC5CA}, /* 攀: GB2312 Code: 0xC5CA ==> Row:37 Col:42 */
    {0x6509,0xDFAB}, /* 攉: GB2312 Code: 0xDFAB ==> Row:63 Col:11 */
    {0x6512,0xD4DC}, /* 攒: GB2312 Code: 0xD4DC ==> Row:52 Col:60 */
    {0x6518,0xC8C1}, /* 攘: GB2312 Code: 0xC8C1 ==> Row:40 Col:33 */
    {0x6525,0xDFAC}, /* 攥: GB2312 Code: 0xDFAC ==> Row:63 Col:12 */
    {0x652B,0xBEF0}, /* 攫: GB2312 Code: 0xBEF0 ==> Row:30 Col:80 */
    {0x652E,0xDFAD}, /* 攮: GB2312 Code: 0xDFAD ==> Row:63 Col:13 */
    {0x652F,0xD6A7}, /* 支: GB2312 Code: 0xD6A7 ==> Row:54 Col:07 */
    {0x6534,0xEAB7}, /* 攴: GB2312 Code: 0xEAB7 ==> Row:74 Col:23 */
    {0x6535,0xEBB6}, /* 攵: GB2312 Code: 0xEBB6 ==> Row:75 Col:22 */
    {0x6536,0xCAD5}, /* 收: GB2312 Code: 0xCAD5 ==> Row:42 Col:53 */
    {0x6538,0xD8FC}, /* 攸: GB2312 Code: 0xD8FC ==> Row:56 Col:92 */
    {0x6539,0xB8C4}, /* 改: GB2312 Code: 0xB8C4 ==> Row:24 Col:36 */
    {0x653B,0xB9A5}, /* 攻: GB2312 Code: 0xB9A5 ==> Row:25 Col:05 */
    {0x653E,0xB7C5}, /* 放: GB2312 Code: 0xB7C5 ==> Row:23 Col:37 */
    {0x653F,0xD5FE}, /* 政: GB2312 Code: 0xD5FE ==> Row:53 Col:94 */
    {0x6545,0xB9CA}, /* 故: GB2312 Code: 0xB9CA ==> Row:25 Col:42 */
    {0x6548,0xD0A7}, /* 效: GB2312 Code: 0xD0A7 ==> Row:48 Col:07 */
    {0x6549,0xF4CD}, /* 敉: GB2312 Code: 0xF4CD ==> Row:84 Col:45 */
    {0x654C,0xB5D0}, /* 敌: GB2312 Code: 0xB5D0 ==> Row:21 Col:48 */
    {0x654F,0xC3F4}, /* 敏: GB2312 Code: 0xC3F4 ==> Row:35 Col:84 */
    {0x6551,0xBEC8}, /* 救: GB2312 Code: 0xBEC8 ==> Row:30 Col:40 */
    {0x6555,0xEBB7}, /* 敕: GB2312 Code: 0xEBB7 ==> Row:75 Col:23 */
    {0x6556,0xB0BD}, /* 敖: GB2312 Code: 0xB0BD ==> Row:16 Col:29 */
    {0x6559,0xBDCC}, /* 教: GB2312 Code: 0xBDCC ==> Row:29 Col:44 */
    {0x655B,0xC1B2}, /* 敛: GB2312 Code: 0xC1B2 ==> Row:33 Col:18 */
    {0x655D,0xB1D6}, /* 敝: GB2312 Code: 0xB1D6 ==> Row:17 Col:54 */
    {0x655E,0xB3A8}, /* 敞: GB2312 Code: 0xB3A8 ==> Row:19 Col:08 */
    {0x6562,0xB8D2}, /* 敢: GB2312 Code: 0xB8D2 ==> Row:24 Col:50 */
    {0x6563,0xC9A2}, /* 散: GB2312 Code: 0xC9A2 ==> Row:41 Col:02 */
    {0x6566,0xB6D8}, /* 敦: GB2312 Code: 0xB6D8 ==> Row:22 Col:56 */
    {0x656B,0xEBB8}, /* 敫: GB2312 Code: 0xEBB8 ==> Row:75 Col:24 */
    {0x656C,0xBEB4}, /* 敬: GB2312 Code: 0xBEB4 ==> Row:30 Col:20 */
    {0x6570,0xCAFD}, /* 数: GB2312 Code: 0xCAFD ==> Row:42 Col:93 */
    {0x6572,0xC7C3}, /* 敲: GB2312 Code: 0xC7C3 ==> Row:39 Col:35 */
    {0x6574,0xD5FB}, /* 整: GB2312 Code: 0xD5FB ==> Row:53 Col:91 */
    {0x6577,0xB7F3}, /* 敷: GB2312 Code: 0xB7F3 ==> Row:23 Col:83 */
    {0x6587,0xCEC4}, /* 文: GB2312 Code: 0xCEC4 ==> Row:46 Col:36 */
    {0x658B,0xD5AB}, /* 斋: GB2312 Code: 0xD5AB ==> Row:53 Col:11 */
    {0x658C,0xB1F3}, /* 斌: GB2312 Code: 0xB1F3 ==> Row:17 Col:83 */
    {0x6590,0xECB3}, /* 斐: GB2312 Code: 0xECB3 ==> Row:76 Col:19 */
    {0x6591,0xB0DF}, /* 斑: GB2312 Code: 0xB0DF ==> Row:16 Col:63 */
    {0x6593,0xECB5}, /* 斓: GB2312 Code: 0xECB5 ==> Row:76 Col:21 */
    {0x6597,0xB6B7}, /* 斗: GB2312 Code: 0xB6B7 ==> Row:22 Col:23 */
    {0x6599,0xC1CF}, /* 料: GB2312 Code: 0xC1CF ==> Row:33 Col:47 */
    {0x659B,0xF5FA}, /* 斛: GB2312 Code: 0xF5FA ==> Row:85 Col:90 */
    {0x659C,0xD0B1}, /* 斜: GB2312 Code: 0xD0B1 ==> Row:48 Col:17 */
    {0x659F,0xD5E5}, /* 斟: GB2312 Code: 0xD5E5 ==> Row:53 Col:69 */
    {0x65A1,0xCED3}, /* 斡: GB2312 Code: 0xCED3 ==> Row:46 Col:51 */
    {0x65A4,0xBDEF}, /* 斤: GB2312 Code: 0xBDEF ==> Row:29 Col:79 */
    {0x65A5,0xB3E2}, /* 斥: GB2312 Code: 0xB3E2 ==> Row:19 Col:66 */
    {0x65A7,0xB8AB}, /* 斧: GB2312 Code: 0xB8AB ==> Row:24 Col:11 */
    {0x65A9,0xD5B6}, /* 斩: GB2312 Code: 0xD5B6 ==> Row:53 Col:22 */
    {0x65AB,0xEDBD}, /* 斫: GB2312 Code: 0xEDBD ==> Row:77 Col:29 */
    {0x65AD,0xB6CF}, /* 断: GB2312 Code: 0xB6CF ==> Row:22 Col:47 */
    {0x65AF,0xCBB9}, /* 斯: GB2312 Code: 0xCBB9 ==> Row:43 Col:25 */
    {0x65B0,0xD0C2}, /* 新: GB2312 Code: 0xD0C2 ==> Row:48 Col:34 */
    {0x65B9,0xB7BD}, /* 方: GB2312 Code: 0xB7BD ==> Row:23 Col:29 */
    {0x65BC,0xECB6}, /* 於: GB2312 Code: 0xECB6 ==> Row:76 Col:22 */
    {0x65BD,0xCAA9}, /* 施: GB2312 Code: 0xCAA9 ==> Row:42 Col:09 */
    {0x65C1,0xC5D4}, /* 旁: GB2312 Code: 0xC5D4 ==> Row:37 Col:52 */
    {0x65C3,0xECB9}, /* 旃: GB2312 Code: 0xECB9 ==> Row:76 Col:25 */
    {0x65C4,0xECB8}, /* 旄: GB2312 Code: 0xECB8 ==> Row:76 Col:24 */
    {0x65C5,0xC2C3}, /* 旅: GB2312 Code: 0xC2C3 ==> Row:34 Col:35 */
    {0x65C6,0xECB7}, /* 旆: GB2312 Code: 0xECB7 ==> Row:76 Col:23 */
    {0x65CB,0xD0FD}, /* 旋: GB2312 Code: 0xD0FD ==> Row:48 Col:93 */
    {0x65CC,0xECBA}, /* 旌: GB2312 Code: 0xECBA ==> Row:76 Col:26 */
    {0x65CE,0xECBB}, /* 旎: GB2312 Code: 0xECBB ==> Row:76 Col:27 */
    {0x65CF,0xD7E5}, /* 族: GB2312 Code: 0xD7E5 ==> Row:55 Col:69 */
    {0x65D2,0xECBC}, /* 旒: GB2312 Code: 0xECBC ==> Row:76 Col:28 */
    {0x65D6,0xECBD}, /* 旖: GB2312 Code: 0xECBD ==> Row:76 Col:29 */
    {0x65D7,0xC6EC}, /* 旗: GB2312 Code: 0xC6EC ==> Row:38 Col:76 */
    {0x65E0,0xCEDE}, /* 无: GB2312 Code: 0xCEDE ==> Row:46 Col:62 */
    {0x65E2,0xBCC8}, /* 既: GB2312 Code: 0xBCC8 ==> Row:28 Col:40 */
    {0x65E5,0xC8D5}, /* 日: GB2312 Code: 0xC8D5 ==> Row:40 Col:53 */
    {0x65E6,0xB5A9}, /* 旦: GB2312 Code: 0xB5A9 ==> Row:21 Col:09 */
    {0x65E7,0xBEC9}, /* 旧: GB2312 Code: 0xBEC9 ==> Row:30 Col:41 */
    {0x65E8,0xD6BC}, /* 旨: GB2312 Code: 0xD6BC ==> Row:54 Col:28 */
    {0x65E9,0xD4E7}, /* 早: GB2312 Code: 0xD4E7 ==> Row:52 Col:71 */
    {0x65EC,0xD1AE}, /* 旬: GB2312 Code: 0xD1AE ==> Row:49 Col:14 */
    {0x65ED,0xD0F1}, /* 旭: GB2312 Code: 0xD0F1 ==> Row:48 Col:81 */
    {0x65EE,0xEAB8}, /* 旮: GB2312 Code: 0xEAB8 ==> Row:74 Col:24 */
    {0x65EF,0xEAB9}, /* 旯: GB2312 Code: 0xEAB9 ==> Row:74 Col:25 */
    {0x65F0,0xEABA}, /* 旰: GB2312 Code: 0xEABA ==> Row:74 Col:26 */
    {0x65F1,0xBAB5}, /* 旱: GB2312 Code: 0xBAB5 ==> Row:26 Col:21 */
    {0x65F6,0xCAB1}, /* 时: GB2312 Code: 0xCAB1 ==> Row:42 Col:17 */
    {0x65F7,0xBFF5}, /* 旷: GB2312 Code: 0xBFF5 ==> Row:31 Col:85 */
    {0x65FA,0xCDFA}, /* 旺: GB2312 Code: 0xCDFA ==> Row:45 Col:90 */
    {0x6600,0xEAC0}, /* 昀: GB2312 Code: 0xEAC0 ==> Row:74 Col:32 */
    {0x6602,0xB0BA}, /* 昂: GB2312 Code: 0xB0BA ==> Row:16 Col:26 */
    {0x6603,0xEABE}, /* 昃: GB2312 Code: 0xEABE ==> Row:74 Col:30 */
    {0x6606,0xC0A5}, /* 昆: GB2312 Code: 0xC0A5 ==> Row:32 Col:05 */
    {0x660A,0xEABB}, /* 昊: GB2312 Code: 0xEABB ==> Row:74 Col:27 */
    {0x660C,0xB2FD}, /* 昌: GB2312 Code: 0xB2FD ==> Row:18 Col:93 */
    {0x660E,0xC3F7}, /* 明: GB2312 Code: 0xC3F7 ==> Row:35 Col:87 */
    {0x660F,0xBBE8}, /* 昏: GB2312 Code: 0xBBE8 ==> Row:27 Col:72 */
    {0x6613,0xD2D7}, /* 易: GB2312 Code: 0xD2D7 ==> Row:50 Col:55 */
    {0x6614,0xCEF4}, /* 昔: GB2312 Code: 0xCEF4 ==> Row:46 Col:84 */
    {0x6615,0xEABF}, /* 昕: GB2312 Code: 0xEABF ==> Row:74 Col:31 */
    {0x6619,0xEABC}, /* 昙: GB2312 Code: 0xEABC ==> Row:74 Col:28 */
    {0x661D,0xEAC3}, /* 昝: GB2312 Code: 0xEAC3 ==> Row:74 Col:35 */
    {0x661F,0xD0C7}, /* 星: GB2312 Code: 0xD0C7 ==> Row:48 Col:39 */
    {0x6620,0xD3B3}, /* 映: GB2312 Code: 0xD3B3 ==> Row:51 Col:19 */
    {0x6625,0xB4BA}, /* 春: GB2312 Code: 0xB4BA ==> Row:20 Col:26 */
    {0x6627,0xC3C1}, /* 昧: GB2312 Code: 0xC3C1 ==> Row:35 Col:33 */
    {0x6628,0xD7F2}, /* 昨: GB2312 Code: 0xD7F2 ==> Row:55 Col:82 */
    {0x662D,0xD5D1}, /* 昭: GB2312 Code: 0xD5D1 ==> Row:53 Col:49 */
    {0x662F,0xCAC7}, /* 是: GB2312 Code: 0xCAC7 ==> Row:42 Col:39 */
    {0x6631,0xEAC5}, /* 昱: GB2312 Code: 0xEAC5 ==> Row:74 Col:37 */
    {0x6634,0xEAC4}, /* 昴: GB2312 Code: 0xEAC4 ==> Row:74 Col:36 */
    {0x6635,0xEAC7}, /* 昵: GB2312 Code: 0xEAC7 ==> Row:74 Col:39 */
    {0x6636,0xEAC6}, /* 昶: GB2312 Code: 0xEAC6 ==> Row:74 Col:38 */
    {0x663C,0xD6E7}, /* 昼: GB2312 Code: 0xD6E7 ==> Row:54 Col:71 */
    {0x663E,0xCFD4}, /* 显: GB2312 Code: 0xCFD4 ==> Row:47 Col:52 */
    {0x6641,0xEACB}, /* 晁: GB2312 Code: 0xEACB ==> Row:74 Col:43 */
    {0x6643,0xBBCE}, /* 晃: GB2312 Code: 0xBBCE ==> Row:27 Col:46 */
    {0x664B,0xBDFA}, /* 晋: GB2312 Code: 0xBDFA ==> Row:29 Col:90 */
    {0x664C,0xC9CE}, /* 晌: GB2312 Code: 0xC9CE ==> Row:41 Col:46 */
    {0x664F,0xEACC}, /* 晏: GB2312 Code: 0xEACC ==> Row:74 Col:44 */
    {0x6652,0xC9B9}, /* 晒: GB2312 Code: 0xC9B9 ==> Row:41 Col:25 */
    {0x6653,0xCFFE}, /* 晓: GB2312 Code: 0xCFFE ==> Row:47 Col:94 */
    {0x6654,0xEACA}, /* 晔: GB2312 Code: 0xEACA ==> Row:74 Col:42 */
    {0x6655,0xD4CE}, /* 晕: GB2312 Code: 0xD4CE ==> Row:52 Col:46 */
    {0x6656,0xEACD}, /* 晖: GB2312 Code: 0xEACD ==> Row:74 Col:45 */
    {0x6657,0xEACF}, /* 晗: GB2312 Code: 0xEACF ==> Row:74 Col:47 */
    {0x665A,0xCDED}, /* 晚: GB2312 Code: 0xCDED ==> Row:45 Col:77 */
    {0x665F,0xEAC9}, /* 晟: GB2312 Code: 0xEAC9 ==> Row:74 Col:41 */
    {0x6661,0xEACE}, /* 晡: GB2312 Code: 0xEACE ==> Row:74 Col:46 */
    {0x6664,0xCEEE}, /* 晤: GB2312 Code: 0xCEEE ==> Row:46 Col:78 */
    {0x6666,0xBBDE}, /* 晦: GB2312 Code: 0xBBDE ==> Row:27 Col:62 */
    {0x6668,0xB3BF}, /* 晨: GB2312 Code: 0xB3BF ==> Row:19 Col:31 */
    {0x666E,0xC6D5}, /* 普: GB2312 Code: 0xC6D5 ==> Row:38 Col:53 */
    {0x666F,0xBEB0}, /* 景: GB2312 Code: 0xBEB0 ==> Row:30 Col:16 */
    {0x6670,0xCEFA}, /* 晰: GB2312 Code: 0xCEFA ==> Row:46 Col:90 */
    {0x6674,0xC7E7}, /* 晴: GB2312 Code: 0xC7E7 ==> Row:39 Col:71 */
    {0x6676,0xBEA7}, /* 晶: GB2312 Code: 0xBEA7 ==> Row:30 Col:07 */
    {0x6677,0xEAD0}, /* 晷: GB2312 Code: 0xEAD0 ==> Row:74 Col:48 */
    {0x667A,0xD6C7}, /* 智: GB2312 Code: 0xD6C7 ==> Row:54 Col:39 */
    {0x667E,0xC1C0}, /* 晾: GB2312 Code: 0xC1C0 ==> Row:33 Col:32 */
    {0x6682,0xD4DD}, /* 暂: GB2312 Code: 0xD4DD ==> Row:52 Col:61 */
    {0x6684,0xEAD1}, /* 暄: GB2312 Code: 0xEAD1 ==> Row:74 Col:49 */
    {0x6687,0xCFBE}, /* 暇: GB2312 Code: 0xCFBE ==> Row:47 Col:30 */
    {0x668C,0xEAD2}, /* 暌: GB2312 Code: 0xEAD2 ==> Row:74 Col:50 */
    {0x6691,0xCAEE}, /* 暑: GB2312 Code: 0xCAEE ==> Row:42 Col:78 */
    {0x6696,0xC5AF}, /* 暖: GB2312 Code: 0xC5AF ==> Row:37 Col:15 */
    {0x6697,0xB0B5}, /* 暗: GB2312 Code: 0xB0B5 ==> Row:16 Col:21 */
    {0x669D,0xEAD4}, /* 暝: GB2312 Code: 0xEAD4 ==> Row:74 Col:52 */
    {0x66A7,0xEAD3}, /* 暧: GB2312 Code: 0xEAD3 ==> Row:74 Col:51 */
    {0x66A8,0xF4DF}, /* 暨: GB2312 Code: 0xF4DF ==> Row:84 Col:63 */
    {0x66AE,0xC4BA}, /* 暮: GB2312 Code: 0xC4BA ==> Row:36 Col:26 */
    {0x66B4,0xB1A9}, /* 暴: GB2312 Code: 0xB1A9 ==> Row:17 Col:09 */
    {0x66B9,0xE5DF}, /* 暹: GB2312 Code: 0xE5DF ==> Row:69 Col:63 */
    {0x66BE,0xEAD5}, /* 暾: GB2312 Code: 0xEAD5 ==> Row:74 Col:53 */
    {0x66D9,0xCAEF}, /* 曙: GB2312 Code: 0xCAEF ==> Row:42 Col:79 */
    {0x66DB,0xEAD6}, /* 曛: GB2312 Code: 0xEAD6 ==> Row:74 Col:54 */
    {0x66DC,0xEAD7}, /* 曜: GB2312 Code: 0xEAD7 ==> Row:74 Col:55 */
    {0x66DD,0xC6D8}, /* 曝: GB2312 Code: 0xC6D8 ==> Row:38 Col:56 */
    {0x66E6,0xEAD8}, /* 曦: GB2312 Code: 0xEAD8 ==> Row:74 Col:56 */
    {0x66E9,0xEAD9}, /* 曩: GB2312 Code: 0xEAD9 ==> Row:74 Col:57 */
    {0x66F0,0xD4BB}, /* 曰: GB2312 Code: 0xD4BB ==> Row:52 Col:27 */
    {0x66F2,0xC7FA}, /* 曲: GB2312 Code: 0xC7FA ==> Row:39 Col:90 */
    {0x66F3,0xD2B7}, /* 曳: GB2312 Code: 0xD2B7 ==> Row:50 Col:23 */
    {0x66F4,0xB8FC}, /* 更: GB2312 Code: 0xB8FC ==> Row:24 Col:92 */
    {0x66F7,0xEAC2}, /* 曷: GB2312 Code: 0xEAC2 ==> Row:74 Col:34 */
    {0x66F9,0xB2DC}, /* 曹: GB2312 Code: 0xB2DC ==> Row:18 Col:60 */
    {0x66FC,0xC2FC}, /* 曼: GB2312 Code: 0xC2FC ==> Row:34 Col:92 */
    {0x66FE,0xD4F8}, /* 曾: GB2312 Code: 0xD4F8 ==> Row:52 Col:88 */
    {0x66FF,0xCCE6}, /* 替: GB2312 Code: 0xCCE6 ==> Row:44 Col:70 */
    {0x6700,0xD7EE}, /* 最: GB2312 Code: 0xD7EE ==> Row:55 Col:78 */
    {0x6708,0xD4C2}, /* 月: GB2312 Code: 0xD4C2 ==> Row:52 Col:34 */
    {0x6709,0xD3D0}, /* 有: GB2312 Code: 0xD3D0 ==> Row:51 Col:48 */
    {0x670A,0xEBC3}, /* 朊: GB2312 Code: 0xEBC3 ==> Row:75 Col:35 */
    {0x670B,0xC5F3}, /* 朋: GB2312 Code: 0xC5F3 ==> Row:37 Col:83 */
    {0x670D,0xB7FE}, /* 服: GB2312 Code: 0xB7FE ==> Row:23 Col:94 */
    {0x6710,0xEBD4}, /* 朐: GB2312 Code: 0xEBD4 ==> Row:75 Col:52 */
    {0x6714,0xCBB7}, /* 朔: GB2312 Code: 0xCBB7 ==> Row:43 Col:23 */
    {0x6715,0xEBDE}, /* 朕: GB2312 Code: 0xEBDE ==> Row:75 Col:62 */
    {0x6717,0xC0CA}, /* 朗: GB2312 Code: 0xC0CA ==> Row:32 Col:42 */
    {0x671B,0xCDFB}, /* 望: GB2312 Code: 0xCDFB ==> Row:45 Col:91 */
    {0x671D,0xB3AF}, /* 朝: GB2312 Code: 0xB3AF ==> Row:19 Col:15 */
    {0x671F,0xC6DA}, /* 期: GB2312 Code: 0xC6DA ==> Row:38 Col:58 */
    {0x6726,0xEBFC}, /* 朦: GB2312 Code: 0xEBFC ==> Row:75 Col:92 */
    {0x6728,0xC4BE}, /* 木: GB2312 Code: 0xC4BE ==> Row:36 Col:30 */
    {0x672A,0xCEB4}, /* 未: GB2312 Code: 0xCEB4 ==> Row:46 Col:20 */
    {0x672B,0xC4A9}, /* 末: GB2312 Code: 0xC4A9 ==> Row:36 Col:09 */
    {0x672C,0xB1BE}, /* 本: GB2312 Code: 0xB1BE ==> Row:17 Col:30 */
    {0x672D,0xD4FD}, /* 札: GB2312 Code: 0xD4FD ==> Row:52 Col:93 */
    {0x672F,0xCAF5}, /* 术: GB2312 Code: 0xCAF5 ==> Row:42 Col:85 */
    {0x6731,0xD6EC}, /* 朱: GB2312 Code: 0xD6EC ==> Row:54 Col:76 */
    {0x6734,0xC6D3}, /* 朴: GB2312 Code: 0xC6D3 ==> Row:38 Col:51 */
    {0x6735,0xB6E4}, /* 朵: GB2312 Code: 0xB6E4 ==> Row:22 Col:68 */
    {0x673A,0xBBFA}, /* 机: GB2312 Code: 0xBBFA ==> Row:27 Col:90 */
    {0x673D,0xD0E0}, /* 朽: GB2312 Code: 0xD0E0 ==> Row:48 Col:64 */
    {0x6740,0xC9B1}, /* 杀: GB2312 Code: 0xC9B1 ==> Row:41 Col:17 */
    {0x6742,0xD4D3}, /* 杂: GB2312 Code: 0xD4D3 ==> Row:52 Col:51 */
    {0x6743,0xC8A8}, /* 权: GB2312 Code: 0xC8A8 ==> Row:40 Col:08 */
    {0x6746,0xB8CB}, /* 杆: GB2312 Code: 0xB8CB ==> Row:24 Col:43 */
    {0x6748,0xE8BE}, /* 杈: GB2312 Code: 0xE8BE ==> Row:72 Col:30 */
    {0x6749,0xC9BC}, /* 杉: GB2312 Code: 0xC9BC ==> Row:41 Col:28 */
    {0x674C,0xE8BB}, /* 杌: GB2312 Code: 0xE8BB ==> Row:72 Col:27 */
    {0x674E,0xC0EE}, /* 李: GB2312 Code: 0xC0EE ==> Row:32 Col:78 */
    {0x674F,0xD0D3}, /* 杏: GB2312 Code: 0xD0D3 ==> Row:48 Col:51 */
    {0x6750,0xB2C4}, /* 材: GB2312 Code: 0xB2C4 ==> Row:18 Col:36 */
    {0x6751,0xB4E5}, /* 村: GB2312 Code: 0xB4E5 ==> Row:20 Col:69 */
    {0x6753,0xE8BC}, /* 杓: GB2312 Code: 0xE8BC ==> Row:72 Col:28 */
    {0x6756,0xD5C8}, /* 杖: GB2312 Code: 0xD5C8 ==> Row:53 Col:40 */
    {0x675C,0xB6C5}, /* 杜: GB2312 Code: 0xB6C5 ==> Row:22 Col:37 */
    {0x675E,0xE8BD}, /* 杞: GB2312 Code: 0xE8BD ==> Row:72 Col:29 */
    {0x675F,0xCAF8}, /* 束: GB2312 Code: 0xCAF8 ==> Row:42 Col:88 */
    {0x6760,0xB8DC}, /* 杠: GB2312 Code: 0xB8DC ==> Row:24 Col:60 */
    {0x6761,0xCCF5}, /* 条: GB2312 Code: 0xCCF5 ==> Row:44 Col:85 */
    {0x6765,0xC0B4}, /* 来: GB2312 Code: 0xC0B4 ==> Row:32 Col:20 */
    {0x6768,0xD1EE}, /* 杨: GB2312 Code: 0xD1EE ==> Row:49 Col:78 */
    {0x6769,0xE8BF}, /* 杩: GB2312 Code: 0xE8BF ==> Row:72 Col:31 */
    {0x676A,0xE8C2}, /* 杪: GB2312 Code: 0xE8C2 ==> Row:72 Col:34 */
    {0x676D,0xBABC}, /* 杭: GB2312 Code: 0xBABC ==> Row:26 Col:28 */
    {0x676F,0xB1AD}, /* 杯: GB2312 Code: 0xB1AD ==> Row:17 Col:13 */
    {0x6770,0xBDDC}, /* 杰: GB2312 Code: 0xBDDC ==> Row:29 Col:60 */
    {0x6772,0xEABD}, /* 杲: GB2312 Code: 0xEABD ==> Row:74 Col:29 */
    {0x6773,0xE8C3}, /* 杳: GB2312 Code: 0xE8C3 ==> Row:72 Col:35 */
    {0x6775,0xE8C6}, /* 杵: GB2312 Code: 0xE8C6 ==> Row:72 Col:38 */
    {0x6777,0xE8CB}, /* 杷: GB2312 Code: 0xE8CB ==> Row:72 Col:43 */
    {0x677C,0xE8CC}, /* 杼: GB2312 Code: 0xE8CC ==> Row:72 Col:44 */
    {0x677E,0xCBC9}, /* 松: GB2312 Code: 0xCBC9 ==> Row:43 Col:41 */
    {0x677F,0xB0E5}, /* 板: GB2312 Code: 0xB0E5 ==> Row:16 Col:69 */
    {0x6781,0xBCAB}, /* 极: GB2312 Code: 0xBCAB ==> Row:28 Col:11 */
    {0x6784,0xB9B9}, /* 构: GB2312 Code: 0xB9B9 ==> Row:25 Col:25 */
    {0x6787,0xE8C1}, /* 枇: GB2312 Code: 0xE8C1 ==> Row:72 Col:33 */
    {0x6789,0xCDF7}, /* 枉: GB2312 Code: 0xCDF7 ==> Row:45 Col:87 */
    {0x678B,0xE8CA}, /* 枋: GB2312 Code: 0xE8CA ==> Row:72 Col:42 */
    {0x6790,0xCEF6}, /* 析: GB2312 Code: 0xCEF6 ==> Row:46 Col:86 */
    {0x6795,0xD5ED}, /* 枕: GB2312 Code: 0xD5ED ==> Row:53 Col:77 */
    {0x6797,0xC1D6}, /* 林: GB2312 Code: 0xC1D6 ==> Row:33 Col:54 */
    {0x6798,0xE8C4}, /* 枘: GB2312 Code: 0xE8C4 ==> Row:72 Col:36 */
    {0x679A,0xC3B6}, /* 枚: GB2312 Code: 0xC3B6 ==> Row:35 Col:22 */
    {0x679C,0xB9FB}, /* 果: GB2312 Code: 0xB9FB ==> Row:25 Col:91 */
    {0x679D,0xD6A6}, /* 枝: GB2312 Code: 0xD6A6 ==> Row:54 Col:06 */
    {0x679E,0xE8C8}, /* 枞: GB2312 Code: 0xE8C8 ==> Row:72 Col:40 */
    {0x67A2,0xCAE0}, /* 枢: GB2312 Code: 0xCAE0 ==> Row:42 Col:64 */
    {0x67A3,0xD4E6}, /* 枣: GB2312 Code: 0xD4E6 ==> Row:52 Col:70 */
    {0x67A5,0xE8C0}, /* 枥: GB2312 Code: 0xE8C0 ==> Row:72 Col:32 */
    {0x67A7,0xE8C5}, /* 枧: GB2312 Code: 0xE8C5 ==> Row:72 Col:37 */
    {0x67A8,0xE8C7}, /* 枨: GB2312 Code: 0xE8C7 ==> Row:72 Col:39 */
    {0x67AA,0xC7B9}, /* 枪: GB2312 Code: 0xC7B9 ==> Row:39 Col:25 */
    {0x67AB,0xB7E3}, /* 枫: GB2312 Code: 0xB7E3 ==> Row:23 Col:67 */
    {0x67AD,0xE8C9}, /* 枭: GB2312 Code: 0xE8C9 ==> Row:72 Col:41 */
    {0x67AF,0xBFDD}, /* 枯: GB2312 Code: 0xBFDD ==> Row:31 Col:61 */
    {0x67B0,0xE8D2}, /* 枰: GB2312 Code: 0xE8D2 ==> Row:72 Col:50 */
    {0x67B3,0xE8D7}, /* 枳: GB2312 Code: 0xE8D7 ==> Row:72 Col:55 */
    {0x67B5,0xE8D5}, /* 枵: GB2312 Code: 0xE8D5 ==> Row:72 Col:53 */
    {0x67B6,0xBCDC}, /* 架: GB2312 Code: 0xBCDC ==> Row:28 Col:60 */
    {0x67B7,0xBCCF}, /* 枷: GB2312 Code: 0xBCCF ==> Row:28 Col:47 */
    {0x67B8,0xE8DB}, /* 枸: GB2312 Code: 0xE8DB ==> Row:72 Col:59 */
    {0x67C1,0xE8DE}, /* 柁: GB2312 Code: 0xE8DE ==> Row:72 Col:62 */
    {0x67C3,0xE8DA}, /* 柃: GB2312 Code: 0xE8DA ==> Row:72 Col:58 */
    {0x67C4,0xB1FA}, /* 柄: GB2312 Code: 0xB1FA ==> Row:17 Col:90 */
    {0x67CF,0xB0D8}, /* 柏: GB2312 Code: 0xB0D8 ==> Row:16 Col:56 */
    {0x67D0,0xC4B3}, /* 某: GB2312 Code: 0xC4B3 ==> Row:36 Col:19 */
    {0x67D1,0xB8CC}, /* 柑: GB2312 Code: 0xB8CC ==> Row:24 Col:44 */
    {0x67D2,0xC6E2}, /* 柒: GB2312 Code: 0xC6E2 ==> Row:38 Col:66 */
    {0x67D3,0xC8BE}, /* 染: GB2312 Code: 0xC8BE ==> Row:40 Col:30 */
    {0x67D4,0xC8E1}, /* 柔: GB2312 Code: 0xC8E1 ==> Row:40 Col:65 */
    {0x67D8,0xE8CF}, /* 柘: GB2312 Code: 0xE8CF ==> Row:72 Col:47 */
    {0x67D9,0xE8D4}, /* 柙: GB2312 Code: 0xE8D4 ==> Row:72 Col:52 */
    {0x67DA,0xE8D6}, /* 柚: GB2312 Code: 0xE8D6 ==> Row:72 Col:54 */
    {0x67DC,0xB9F1}, /* 柜: GB2312 Code: 0xB9F1 ==> Row:25 Col:81 */
    {0x67DD,0xE8D8}, /* 柝: GB2312 Code: 0xE8D8 ==> Row:72 Col:56 */
    {0x67DE,0xD7F5}, /* 柞: GB2312 Code: 0xD7F5 ==> Row:55 Col:85 */
    {0x67E0,0xC4FB}, /* 柠: GB2312 Code: 0xC4FB ==> Row:36 Col:91 */
    {0x67E2,0xE8DC}, /* 柢: GB2312 Code: 0xE8DC ==> Row:72 Col:60 */
    {0x67E5,0xB2E9}, /* 查: GB2312 Code: 0xB2E9 ==> Row:18 Col:73 */
    {0x67E9,0xE8D1}, /* 柩: GB2312 Code: 0xE8D1 ==> Row:72 Col:49 */
    {0x67EC,0xBCED}, /* 柬: GB2312 Code: 0xBCED ==> Row:28 Col:77 */
    {0x67EF,0xBFC2}, /* 柯: GB2312 Code: 0xBFC2 ==> Row:31 Col:34 */
    {0x67F0,0xE8CD}, /* 柰: GB2312 Code: 0xE8CD ==> Row:72 Col:45 */
    {0x67F1,0xD6F9}, /* 柱: GB2312 Code: 0xD6F9 ==> Row:54 Col:89 */
    {0x67F3,0xC1F8}, /* 柳: GB2312 Code: 0xC1F8 ==> Row:33 Col:88 */
    {0x67F4,0xB2F1}, /* 柴: GB2312 Code: 0xB2F1 ==> Row:18 Col:81 */
    {0x67FD,0xE8DF}, /* 柽: GB2312 Code: 0xE8DF ==> Row:72 Col:63 */
    {0x67FF,0xCAC1}, /* 柿: GB2312 Code: 0xCAC1 ==> Row:42 Col:33 */
    {0x6800,0xE8D9}, /* 栀: GB2312 Code: 0xE8D9 ==> Row:72 Col:57 */
    {0x6805,0xD5A4}, /* 栅: GB2312 Code: 0xD5A4 ==> Row:53 Col:04 */
    {0x6807,0xB1EA}, /* 标: GB2312 Code: 0xB1EA ==> Row:17 Col:74 */
    {0x6808,0xD5BB}, /* 栈: GB2312 Code: 0xD5BB ==> Row:53 Col:27 */
    {0x6809,0xE8CE}, /* 栉: GB2312 Code: 0xE8CE ==> Row:72 Col:46 */
    {0x680A,0xE8D0}, /* 栊: GB2312 Code: 0xE8D0 ==> Row:72 Col:48 */
    {0x680B,0xB6B0}, /* 栋: GB2312 Code: 0xB6B0 ==> Row:22 Col:16 */
    {0x680C,0xE8D3}, /* 栌: GB2312 Code: 0xE8D3 ==> Row:72 Col:51 */
    {0x680E,0xE8DD}, /* 栎: GB2312 Code: 0xE8DD ==> Row:72 Col:61 */
    {0x680F,0xC0B8}, /* 栏: GB2312 Code: 0xC0B8 ==> Row:32 Col:24 */
    {0x6811,0xCAF7}, /* 树: GB2312 Code: 0xCAF7 ==> Row:42 Col:87 */
    {0x6813,0xCBA8}, /* 栓: GB2312 Code: 0xCBA8 ==> Row:43 Col:08 */
    {0x6816,0xC6DC}, /* 栖: GB2312 Code: 0xC6DC ==> Row:38 Col:60 */
    {0x6817,0xC0F5}, /* 栗: GB2312 Code: 0xC0F5 ==> Row:32 Col:85 */
    {0x681D,0xE8E9}, /* 栝: GB2312 Code: 0xE8E9 ==> Row:72 Col:73 */
    {0x6821,0xD0A3}, /* 校: GB2312 Code: 0xD0A3 ==> Row:48 Col:03 */
    {0x6829,0xE8F2}, /* 栩: GB2312 Code: 0xE8F2 ==> Row:72 Col:82 */
    {0x682A,0xD6EA}, /* 株: GB2312 Code: 0xD6EA ==> Row:54 Col:74 */
    {0x6832,0xE8E0}, /* 栲: GB2312 Code: 0xE8E0 ==> Row:72 Col:64 */
    {0x6833,0xE8E1}, /* 栳: GB2312 Code: 0xE8E1 ==> Row:72 Col:65 */
    {0x6837,0xD1F9}, /* 样: GB2312 Code: 0xD1F9 ==> Row:49 Col:89 */
    {0x6838,0xBACB}, /* 核: GB2312 Code: 0xBACB ==> Row:26 Col:43 */
    {0x6839,0xB8F9}, /* 根: GB2312 Code: 0xB8F9 ==> Row:24 Col:89 */
    {0x683C,0xB8F1}, /* 格: GB2312 Code: 0xB8F1 ==> Row:24 Col:81 */
    {0x683D,0xD4D4}, /* 栽: GB2312 Code: 0xD4D4 ==> Row:52 Col:52 */
    {0x683E,0xE8EF}, /* 栾: GB2312 Code: 0xE8EF ==> Row:72 Col:79 */
    {0x6840,0xE8EE}, /* 桀: GB2312 Code: 0xE8EE ==> Row:72 Col:78 */
    {0x6841,0xE8EC}, /* 桁: GB2312 Code: 0xE8EC ==> Row:72 Col:76 */
    {0x6842,0xB9F0}, /* 桂: GB2312 Code: 0xB9F0 ==> Row:25 Col:80 */
    {0x6843,0xCCD2}, /* 桃: GB2312 Code: 0xCCD2 ==> Row:44 Col:50 */
    {0x6844,0xE8E6}, /* 桄: GB2312 Code: 0xE8E6 ==> Row:72 Col:70 */
    {0x6845,0xCEA6}, /* 桅: GB2312 Code: 0xCEA6 ==> Row:46 Col:06 */
    {0x6846,0xBFF2}, /* 框: GB2312 Code: 0xBFF2 ==> Row:31 Col:82 */
    {0x6848,0xB0B8}, /* 案: GB2312 Code: 0xB0B8 ==> Row:16 Col:24 */
    {0x6849,0xE8F1}, /* 桉: GB2312 Code: 0xE8F1 ==> Row:72 Col:81 */
    {0x684A,0xE8F0}, /* 桊: GB2312 Code: 0xE8F0 ==> Row:72 Col:80 */
    {0x684C,0xD7C0}, /* 桌: GB2312 Code: 0xD7C0 ==> Row:55 Col:32 */
    {0x684E,0xE8E4}, /* 桎: GB2312 Code: 0xE8E4 ==> Row:72 Col:68 */
    {0x6850,0xCDA9}, /* 桐: GB2312 Code: 0xCDA9 ==> Row:45 Col:09 */
    {0x6851,0xC9A3}, /* 桑: GB2312 Code: 0xC9A3 ==> Row:41 Col:03 */
    {0x6853,0xBBB8}, /* 桓: GB2312 Code: 0xBBB8 ==> Row:27 Col:24 */
    {0x6854,0xBDDB}, /* 桔: GB2312 Code: 0xBDDB ==> Row:29 Col:59 */
    {0x6855,0xE8EA}, /* 桕: GB2312 Code: 0xE8EA ==> Row:72 Col:74 */
    {0x6860,0xE8E2}, /* 桠: GB2312 Code: 0xE8E2 ==> Row:72 Col:66 */
    {0x6861,0xE8E3}, /* 桡: GB2312 Code: 0xE8E3 ==> Row:72 Col:67 */
    {0x6862,0xE8E5}, /* 桢: GB2312 Code: 0xE8E5 ==> Row:72 Col:69 */
    {0x6863,0xB5B5}, /* 档: GB2312 Code: 0xB5B5 ==> Row:21 Col:21 */
    {0x6864,0xE8E7}, /* 桤: GB2312 Code: 0xE8E7 ==> Row:72 Col:71 */
    {0x6865,0xC7C5}, /* 桥: GB2312 Code: 0xC7C5 ==> Row:39 Col:37 */
    {0x6866,0xE8EB}, /* 桦: GB2312 Code: 0xE8EB ==> Row:72 Col:75 */
    {0x6867,0xE8ED}, /* 桧: GB2312 Code: 0xE8ED ==> Row:72 Col:77 */
    {0x6868,0xBDB0}, /* 桨: GB2312 Code: 0xBDB0 ==> Row:29 Col:16 */
    {0x6869,0xD7AE}, /* 桩: GB2312 Code: 0xD7AE ==> Row:55 Col:14 */
    {0x686B,0xE8F8}, /* 桫: GB2312 Code: 0xE8F8 ==> Row:72 Col:88 */
    {0x6874,0xE8F5}, /* 桴: GB2312 Code: 0xE8F5 ==> Row:72 Col:85 */
    {0x6876,0xCDB0}, /* 桶: GB2312 Code: 0xCDB0 ==> Row:45 Col:16 */
    {0x6877,0xE8F6}, /* 桷: GB2312 Code: 0xE8F6 ==> Row:72 Col:86 */
    {0x6881,0xC1BA}, /* 梁: GB2312 Code: 0xC1BA ==> Row:33 Col:26 */
    {0x6883,0xE8E8}, /* 梃: GB2312 Code: 0xE8E8 ==> Row:72 Col:72 */
    {0x6885,0xC3B7}, /* 梅: GB2312 Code: 0xC3B7 ==> Row:35 Col:23 */
    {0x6886,0xB0F0}, /* 梆: GB2312 Code: 0xB0F0 ==> Row:16 Col:80 */
    {0x688F,0xE8F4}, /* 梏: GB2312 Code: 0xE8F4 ==> Row:72 Col:84 */
    {0x6893,0xE8F7}, /* 梓: GB2312 Code: 0xE8F7 ==> Row:72 Col:87 */
    {0x6897,0xB9A3}, /* 梗: GB2312 Code: 0xB9A3 ==> Row:25 Col:03 */
    {0x68A2,0xC9D2}, /* 梢: GB2312 Code: 0xC9D2 ==> Row:41 Col:50 */
    {0x68A6,0xC3CE}, /* 梦: GB2312 Code: 0xC3CE ==> Row:35 Col:46 */
    {0x68A7,0xCEE0}, /* 梧: GB2312 Code: 0xCEE0 ==> Row:46 Col:64 */
    {0x68A8,0xC0E6}, /* 梨: GB2312 Code: 0xC0E6 ==> Row:32 Col:70 */
    {0x68AD,0xCBF3}, /* 梭: GB2312 Code: 0xCBF3 ==> Row:43 Col:83 */
    {0x68AF,0xCCDD}, /* 梯: GB2312 Code: 0xCCDD ==> Row:44 Col:61 */
    {0x68B0,0xD0B5}, /* 械: GB2312 Code: 0xD0B5 ==> Row:48 Col:21 */
    {0x68B3,0xCAE1}, /* 梳: GB2312 Code: 0xCAE1 ==> Row:42 Col:65 */
    {0x68B5,0xE8F3}, /* 梵: GB2312 Code: 0xE8F3 ==> Row:72 Col:83 */
    {0x68C0,0xBCEC}, /* 检: GB2312 Code: 0xBCEC ==> Row:28 Col:76 */
    {0x68C2,0xE8F9}, /* 棂: GB2312 Code: 0xE8F9 ==> Row:72 Col:89 */
    {0x68C9,0xC3DE}, /* 棉: GB2312 Code: 0xC3DE ==> Row:35 Col:62 */
    {0x68CB,0xC6E5}, /* 棋: GB2312 Code: 0xC6E5 ==> Row:38 Col:69 */
    {0x68CD,0xB9F7}, /* 棍: GB2312 Code: 0xB9F7 ==> Row:25 Col:87 */
    {0x68D2,0xB0F4}, /* 棒: GB2312 Code: 0xB0F4 ==> Row:16 Col:84 */
    {0x68D5,0xD7D8}, /* 棕: GB2312 Code: 0xD7D8 ==> Row:55 Col:56 */
    {0x68D8,0xBCAC}, /* 棘: GB2312 Code: 0xBCAC ==> Row:28 Col:12 */
    {0x68DA,0xC5EF}, /* 棚: GB2312 Code: 0xC5EF ==> Row:37 Col:79 */
    {0x68E0,0xCCC4}, /* 棠: GB2312 Code: 0xCCC4 ==> Row:44 Col:36 */
    {0x68E3,0xE9A6}, /* 棣: GB2312 Code: 0xE9A6 ==> Row:73 Col:06 */
    {0x68EE,0xC9AD}, /* 森: GB2312 Code: 0xC9AD ==> Row:41 Col:13 */
    {0x68F0,0xE9A2}, /* 棰: GB2312 Code: 0xE9A2 ==> Row:73 Col:02 */
    {0x68F1,0xC0E2}, /* 棱: GB2312 Code: 0xC0E2 ==> Row:32 Col:66 */
    {0x68F5,0xBFC3}, /* 棵: GB2312 Code: 0xBFC3 ==> Row:31 Col:35 */
    {0x68F9,0xE8FE}, /* 棹: GB2312 Code: 0xE8FE ==> Row:72 Col:94 */
    {0x68FA,0xB9D7}, /* 棺: GB2312 Code: 0xB9D7 ==> Row:25 Col:55 */
    {0x68FC,0xE8FB}, /* 棼: GB2312 Code: 0xE8FB ==> Row:72 Col:91 */
    {0x6901,0xE9A4}, /* 椁: GB2312 Code: 0xE9A4 ==> Row:73 Col:04 */
    {0x6905,0xD2CE}, /* 椅: GB2312 Code: 0xD2CE ==> Row:50 Col:46 */
    {0x690B,0xE9A3}, /* 椋: GB2312 Code: 0xE9A3 ==> Row:73 Col:03 */
    {0x690D,0xD6B2}, /* 植: GB2312 Code: 0xD6B2 ==> Row:54 Col:18 */
    {0x690E,0xD7B5}, /* 椎: GB2312 Code: 0xD7B5 ==> Row:55 Col:21 */
    {0x6910,0xE9A7}, /* 椐: GB2312 Code: 0xE9A7 ==> Row:73 Col:07 */
    {0x6912,0xBDB7}, /* 椒: GB2312 Code: 0xBDB7 ==> Row:29 Col:23 */
    {0x691F,0xE8FC}, /* 椟: GB2312 Code: 0xE8FC ==> Row:72 Col:92 */
    {0x6920,0xE8FD}, /* 椠: GB2312 Code: 0xE8FD ==> Row:72 Col:93 */
    {0x6924,0xE9A1}, /* 椤: GB2312 Code: 0xE9A1 ==> Row:73 Col:01 */
    {0x692D,0xCDD6}, /* 椭: GB2312 Code: 0xCDD6 ==> Row:45 Col:54 */
    {0x6930,0xD2AC}, /* 椰: GB2312 Code: 0xD2AC ==> Row:50 Col:12 */
    {0x6934,0xE9B2}, /* 椴: GB2312 Code: 0xE9B2 ==> Row:73 Col:18 */
    {0x6939,0xE9A9}, /* 椹: GB2312 Code: 0xE9A9 ==> Row:73 Col:09 */
    {0x693D,0xB4AA}, /* 椽: GB2312 Code: 0xB4AA ==> Row:20 Col:10 */
    {0x693F,0xB4BB}, /* 椿: GB2312 Code: 0xB4BB ==> Row:20 Col:27 */
    {0x6942,0xE9AB}, /* 楂: GB2312 Code: 0xE9AB ==> Row:73 Col:11 */
    {0x6954,0xD0A8}, /* 楔: GB2312 Code: 0xD0A8 ==> Row:48 Col:08 */
    {0x6957,0xE9A5}, /* 楗: GB2312 Code: 0xE9A5 ==> Row:73 Col:05 */
    {0x695A,0xB3FE}, /* 楚: GB2312 Code: 0xB3FE ==> Row:19 Col:94 */
    {0x695D,0xE9AC}, /* 楝: GB2312 Code: 0xE9AC ==> Row:73 Col:12 */
    {0x695E,0xC0E3}, /* 楞: GB2312 Code: 0xC0E3 ==> Row:32 Col:67 */
    {0x6960,0xE9AA}, /* 楠: GB2312 Code: 0xE9AA ==> Row:73 Col:10 */
    {0x6963,0xE9B9}, /* 楣: GB2312 Code: 0xE9B9 ==> Row:73 Col:25 */
    {0x6966,0xE9B8}, /* 楦: GB2312 Code: 0xE9B8 ==> Row:73 Col:24 */
    {0x696B,0xE9AE}, /* 楫: GB2312 Code: 0xE9AE ==> Row:73 Col:14 */
    {0x696E,0xE8FA}, /* 楮: GB2312 Code: 0xE8FA ==> Row:72 Col:90 */
    {0x6971,0xE9A8}, /* 楱: GB2312 Code: 0xE9A8 ==> Row:73 Col:08 */
    {0x6977,0xBFAC}, /* 楷: GB2312 Code: 0xBFAC ==> Row:31 Col:12 */
    {0x6978,0xE9B1}, /* 楸: GB2312 Code: 0xE9B1 ==> Row:73 Col:17 */
    {0x6979,0xE9BA}, /* 楹: GB2312 Code: 0xE9BA ==> Row:73 Col:26 */
    {0x697C,0xC2A5}, /* 楼: GB2312 Code: 0xC2A5 ==> Row:34 Col:05 */
    {0x6980,0xE9AF}, /* 榀: GB2312 Code: 0xE9AF ==> Row:73 Col:15 */
    {0x6982,0xB8C5}, /* 概: GB2312 Code: 0xB8C5 ==> Row:24 Col:37 */
    {0x6984,0xE9AD}, /* 榄: GB2312 Code: 0xE9AD ==> Row:73 Col:13 */
    {0x6986,0xD3DC}, /* 榆: GB2312 Code: 0xD3DC ==> Row:51 Col:60 */
    {0x6987,0xE9B4}, /* 榇: GB2312 Code: 0xE9B4 ==> Row:73 Col:20 */
    {0x6988,0xE9B5}, /* 榈: GB2312 Code: 0xE9B5 ==> Row:73 Col:21 */
    {0x6989,0xE9B7}, /* 榉: GB2312 Code: 0xE9B7 ==> Row:73 Col:23 */
    {0x698D,0xE9C7}, /* 榍: GB2312 Code: 0xE9C7 ==> Row:73 Col:39 */
    {0x6994,0xC0C6}, /* 榔: GB2312 Code: 0xC0C6 ==> Row:32 Col:38 */
    {0x6995,0xE9C5}, /* 榕: GB2312 Code: 0xE9C5 ==> Row:73 Col:37 */
    {0x6998,0xE9B0}, /* 榘: GB2312 Code: 0xE9B0 ==> Row:73 Col:16 */
    {0x699B,0xE9BB}, /* 榛: GB2312 Code: 0xE9BB ==> Row:73 Col:27 */
    {0x699C,0xB0F1}, /* 榜: GB2312 Code: 0xB0F1 ==> Row:16 Col:81 */
    {0x69A7,0xE9BC}, /* 榧: GB2312 Code: 0xE9BC ==> Row:73 Col:28 */
    {0x69A8,0xD5A5}, /* 榨: GB2312 Code: 0xD5A5 ==> Row:53 Col:05 */
    {0x69AB,0xE9BE}, /* 榫: GB2312 Code: 0xE9BE ==> Row:73 Col:30 */
    {0x69AD,0xE9BF}, /* 榭: GB2312 Code: 0xE9BF ==> Row:73 Col:31 */
    {0x69B1,0xE9C1}, /* 榱: GB2312 Code: 0xE9C1 ==> Row:73 Col:33 */
    {0x69B4,0xC1F1}, /* 榴: GB2312 Code: 0xC1F1 ==> Row:33 Col:81 */
    {0x69B7,0xC8B6}, /* 榷: GB2312 Code: 0xC8B6 ==> Row:40 Col:22 */
    {0x69BB,0xE9BD}, /* 榻: GB2312 Code: 0xE9BD ==> Row:73 Col:29 */
    {0x69C1,0xE9C2}, /* 槁: GB2312 Code: 0xE9C2 ==> Row:73 Col:34 */
    {0x69CA,0xE9C3}, /* 槊: GB2312 Code: 0xE9C3 ==> Row:73 Col:35 */
    {0x69CC,0xE9B3}, /* 槌: GB2312 Code: 0xE9B3 ==> Row:73 Col:19 */
    {0x69CE,0xE9B6}, /* 槎: GB2312 Code: 0xE9B6 ==> Row:73 Col:22 */
    {0x69D0,0xBBB1}, /* 槐: GB2312 Code: 0xBBB1 ==> Row:27 Col:17 */
    {0x69D4,0xE9C0}, /* 槔: GB2312 Code: 0xE9C0 ==> Row:73 Col:32 */
    {0x69DB,0xBCF7}, /* 槛: GB2312 Code: 0xBCF7 ==> Row:28 Col:87 */
    {0x69DF,0xE9C4}, /* 槟: GB2312 Code: 0xE9C4 ==> Row:73 Col:36 */
    {0x69E0,0xE9C6}, /* 槠: GB2312 Code: 0xE9C6 ==> Row:73 Col:38 */
    {0x69ED,0xE9CA}, /* 槭: GB2312 Code: 0xE9CA ==> Row:73 Col:42 */
    {0x69F2,0xE9CE}, /* 槲: GB2312 Code: 0xE9CE ==> Row:73 Col:46 */
    {0x69FD,0xB2DB}, /* 槽: GB2312 Code: 0xB2DB ==> Row:18 Col:59 */
    {0x69FF,0xE9C8}, /* 槿: GB2312 Code: 0xE9C8 ==> Row:73 Col:40 */
    {0x6A0A,0xB7AE}, /* 樊: GB2312 Code: 0xB7AE ==> Row:23 Col:14 */
    {0x6A17,0xE9CB}, /* 樗: GB2312 Code: 0xE9CB ==> Row:73 Col:43 */
    {0x6A18,0xE9CC}, /* 樘: GB2312 Code: 0xE9CC ==> Row:73 Col:44 */
    {0x6A1F,0xD5C1}, /* 樟: GB2312 Code: 0xD5C1 ==> Row:53 Col:33 */
    {0x6A21,0xC4A3}, /* 模: GB2312 Code: 0xC4A3 ==> Row:36 Col:03 */
    {0x6A28,0xE9D8}, /* 樨: GB2312 Code: 0xE9D8 ==> Row:73 Col:56 */
    {0x6A2A,0xBAE1}, /* 横: GB2312 Code: 0xBAE1 ==> Row:26 Col:65 */
    {0x6A2F,0xE9C9}, /* 樯: GB2312 Code: 0xE9C9 ==> Row:73 Col:41 */
    {0x6A31,0xD3A3}, /* 樱: GB2312 Code: 0xD3A3 ==> Row:51 Col:03 */
    {0x6A35,0xE9D4}, /* 樵: GB2312 Code: 0xE9D4 ==> Row:73 Col:52 */
    {0x6A3D,0xE9D7}, /* 樽: GB2312 Code: 0xE9D7 ==> Row:73 Col:55 */
    {0x6A3E,0xE9D0}, /* 樾: GB2312 Code: 0xE9D0 ==> Row:73 Col:48 */
    {0x6A44,0xE9CF}, /* 橄: GB2312 Code: 0xE9CF ==> Row:73 Col:47 */
    {0x6A47,0xC7C1}, /* 橇: GB2312 Code: 0xC7C1 ==> Row:39 Col:33 */
    {0x6A50,0xE9D2}, /* 橐: GB2312 Code: 0xE9D2 ==> Row:73 Col:50 */
    {0x6A58,0xE9D9}, /* 橘: GB2312 Code: 0xE9D9 ==> Row:73 Col:57 */
    {0x6A59,0xB3C8}, /* 橙: GB2312 Code: 0xB3C8 ==> Row:19 Col:40 */
    {0x6A5B,0xE9D3}, /* 橛: GB2312 Code: 0xE9D3 ==> Row:73 Col:51 */
    {0x6A61,0xCFF0}, /* 橡: GB2312 Code: 0xCFF0 ==> Row:47 Col:80 */
    {0x6A65,0xE9CD}, /* 橥: GB2312 Code: 0xE9CD ==> Row:73 Col:45 */
    {0x6A71,0xB3F7}, /* 橱: GB2312 Code: 0xB3F7 ==> Row:19 Col:87 */
    {0x6A79,0xE9D6}, /* 橹: GB2312 Code: 0xE9D6 ==> Row:73 Col:54 */
    {0x6A7C,0xE9DA}, /* 橼: GB2312 Code: 0xE9DA ==> Row:73 Col:58 */
    {0x6A80,0xCCB4}, /* 檀: GB2312 Code: 0xCCB4 ==> Row:44 Col:20 */
    {0x6A84,0xCFAD}, /* 檄: GB2312 Code: 0xCFAD ==> Row:47 Col:13 */
    {0x6A8E,0xE9D5}, /* 檎: GB2312 Code: 0xE9D5 ==> Row:73 Col:53 */
    {0x6A90,0xE9DC}, /* 檐: GB2312 Code: 0xE9DC ==> Row:73 Col:60 */
    {0x6A91,0xE9DB}, /* 檑: GB2312 Code: 0xE9DB ==> Row:73 Col:59 */
    {0x6A97,0xE9DE}, /* 檗: GB2312 Code: 0xE9DE ==> Row:73 Col:62 */
    {0x6AA0,0xE9D1}, /* 檠: GB2312 Code: 0xE9D1 ==> Row:73 Col:49 */
    {0x6AA9,0xE9DD}, /* 檩: GB2312 Code: 0xE9DD ==> Row:73 Col:61 */
    {0x6AAB,0xE9DF}, /* 檫: GB2312 Code: 0xE9DF ==> Row:73 Col:63 */
    {0x6AAC,0xC3CA}, /* 檬: GB2312 Code: 0xC3CA ==> Row:35 Col:42 */
    {0x6B20,0xC7B7}, /* 欠: GB2312 Code: 0xC7B7 ==> Row:39 Col:23 */
    {0x6B21,0xB4CE}, /* 次: GB2312 Code: 0xB4CE ==> Row:20 Col:46 */
    {0x6B22,0xBBB6}, /* 欢: GB2312 Code: 0xBBB6 ==> Row:27 Col:22 */
    {0x6B23,0xD0C0}, /* 欣: GB2312 Code: 0xD0C0 ==> Row:48 Col:32 */
    {0x6B24,0xECA3}, /* 欤: GB2312 Code: 0xECA3 ==> Row:76 Col:03 */
    {0x6B27,0xC5B7}, /* 欧: GB2312 Code: 0xC5B7 ==> Row:37 Col:23 */
    {0x6B32,0xD3FB}, /* 欲: GB2312 Code: 0xD3FB ==> Row:51 Col:91 */
    {0x6B37,0xECA4}, /* 欷: GB2312 Code: 0xECA4 ==> Row:76 Col:04 */
    {0x6B39,0xECA5}, /* 欹: GB2312 Code: 0xECA5 ==> Row:76 Col:05 */
    {0x6B3A,0xC6DB}, /* 欺: GB2312 Code: 0xC6DB ==> Row:38 Col:59 */
    {0x6B3E,0xBFEE}, /* 款: GB2312 Code: 0xBFEE ==> Row:31 Col:78 */
    {0x6B43,0xECA6}, /* 歃: GB2312 Code: 0xECA6 ==> Row:76 Col:06 */
    {0x6B46,0xECA7}, /* 歆: GB2312 Code: 0xECA7 ==> Row:76 Col:07 */
    {0x6B47,0xD0AA}, /* 歇: GB2312 Code: 0xD0AA ==> Row:48 Col:10 */
    {0x6B49,0xC7B8}, /* 歉: GB2312 Code: 0xC7B8 ==> Row:39 Col:24 */
    {0x6B4C,0xB8E8}, /* 歌: GB2312 Code: 0xB8E8 ==> Row:24 Col:72 */
    {0x6B59,0xECA8}, /* 歙: GB2312 Code: 0xECA8 ==> Row:76 Col:08 */
    {0x6B62,0xD6B9}, /* 止: GB2312 Code: 0xD6B9 ==> Row:54 Col:25 */
    {0x6B63,0xD5FD}, /* 正: GB2312 Code: 0xD5FD ==> Row:53 Col:93 */
    {0x6B64,0xB4CB}, /* 此: GB2312 Code: 0xB4CB ==> Row:20 Col:43 */
    {0x6B65,0xB2BD}, /* 步: GB2312 Code: 0xB2BD ==> Row:18 Col:29 */
    {0x6B66,0xCEE4}, /* 武: GB2312 Code: 0xCEE4 ==> Row:46 Col:68 */
    {0x6B67,0xC6E7}, /* 歧: GB2312 Code: 0xC6E7 ==> Row:38 Col:71 */
    {0x6B6A,0xCDE1}, /* 歪: GB2312 Code: 0xCDE1 ==> Row:45 Col:65 */
    {0x6B79,0xB4F5}, /* 歹: GB2312 Code: 0xB4F5 ==> Row:20 Col:85 */
    {0x6B7B,0xCBC0}, /* 死: GB2312 Code: 0xCBC0 ==> Row:43 Col:32 */
    {0x6B7C,0xBCDF}, /* 歼: GB2312 Code: 0xBCDF ==> Row:28 Col:63 */
    {0x6B81,0xE9E2}, /* 殁: GB2312 Code: 0xE9E2 ==> Row:73 Col:66 */
    {0x6B82,0xE9E3}, /* 殂: GB2312 Code: 0xE9E3 ==> Row:73 Col:67 */
    {0x6B83,0xD1EA}, /* 殃: GB2312 Code: 0xD1EA ==> Row:49 Col:74 */
    {0x6B84,0xE9E5}, /* 殄: GB2312 Code: 0xE9E5 ==> Row:73 Col:69 */
    {0x6B86,0xB4F9}, /* 殆: GB2312 Code: 0xB4F9 ==> Row:20 Col:89 */
    {0x6B87,0xE9E4}, /* 殇: GB2312 Code: 0xE9E4 ==> Row:73 Col:68 */
    {0x6B89,0xD1B3}, /* 殉: GB2312 Code: 0xD1B3 ==> Row:49 Col:19 */
    {0x6B8A,0xCAE2}, /* 殊: GB2312 Code: 0xCAE2 ==> Row:42 Col:66 */
    {0x6B8B,0xB2D0}, /* 残: GB2312 Code: 0xB2D0 ==> Row:18 Col:48 */
    {0x6B8D,0xE9E8}, /* 殍: GB2312 Code: 0xE9E8 ==> Row:73 Col:72 */
    {0x6B92,0xE9E6}, /* 殒: GB2312 Code: 0xE9E6 ==> Row:73 Col:70 */
    {0x6B93,0xE9E7}, /* 殓: GB2312 Code: 0xE9E7 ==> Row:73 Col:71 */
    {0x6B96,0xD6B3}, /* 殖: GB2312 Code: 0xD6B3 ==> Row:54 Col:19 */
    {0x6B9A,0xE9E9}, /* 殚: GB2312 Code: 0xE9E9 ==> Row:73 Col:73 */
    {0x6B9B,0xE9EA}, /* 殛: GB2312 Code: 0xE9EA ==> Row:73 Col:74 */
    {0x6BA1,0xE9EB}, /* 殡: GB2312 Code: 0xE9EB ==> Row:73 Col:75 */
    {0x6BAA,0xE9EC}, /* 殪: GB2312 Code: 0xE9EC ==> Row:73 Col:76 */
    {0x6BB3,0xECAF}, /* 殳: GB2312 Code: 0xECAF ==> Row:76 Col:15 */
    {0x6BB4,0xC5B9}, /* 殴: GB2312 Code: 0xC5B9 ==> Row:37 Col:25 */
    {0x6BB5,0xB6CE}, /* 段: GB2312 Code: 0xB6CE ==> Row:22 Col:46 */
    {0x6BB7,0xD2F3}, /* 殷: GB2312 Code: 0xD2F3 ==> Row:50 Col:83 */
    {0x6BBF,0xB5EE}, /* 殿: GB2312 Code: 0xB5EE ==> Row:21 Col:78 */
    {0x6BC1,0xBBD9}, /* 毁: GB2312 Code: 0xBBD9 ==> Row:27 Col:57 */
    {0x6BC2,0xECB1}, /* 毂: GB2312 Code: 0xECB1 ==> Row:76 Col:17 */
    {0x6BC5,0xD2E3}, /* 毅: GB2312 Code: 0xD2E3 ==> Row:50 Col:67 */
    {0x6BCB,0xCEE3}, /* 毋: GB2312 Code: 0xCEE3 ==> Row:46 Col:67 */
    {0x6BCD,0xC4B8}, /* 母: GB2312 Code: 0xC4B8 ==> Row:36 Col:24 */
    {0x6BCF,0xC3BF}, /* 每: GB2312 Code: 0xC3BF ==> Row:35 Col:31 */
    {0x6BD2,0xB6BE}, /* 毒: GB2312 Code: 0xB6BE ==> Row:22 Col:30 */
    {0x6BD3,0xD8B9}, /* 毓: GB2312 Code: 0xD8B9 ==> Row:56 Col:25 */
    {0x6BD4,0xB1C8}, /* 比: GB2312 Code: 0xB1C8 ==> Row:17 Col:40 */
    {0x6BD5,0xB1CF}, /* 毕: GB2312 Code: 0xB1CF ==> Row:17 Col:47 */
    {0x6BD6,0xB1D1}, /* 毖: GB2312 Code: 0xB1D1 ==> Row:17 Col:49 */
    {0x6BD7,0xC5FE}, /* 毗: GB2312 Code: 0xC5FE ==> Row:37 Col:94 */
    {0x6BD9,0xB1D0}, /* 毙: GB2312 Code: 0xB1D0 ==> Row:17 Col:48 */
    {0x6BDB,0xC3AB}, /* 毛: GB2312 Code: 0xC3AB ==> Row:35 Col:11 */
    {0x6BE1,0xD5B1}, /* 毡: GB2312 Code: 0xD5B1 ==> Row:53 Col:17 */
    {0x6BEA,0xEBA4}, /* 毪: GB2312 Code: 0xEBA4 ==> Row:75 Col:04 */
    {0x6BEB,0xBAC1}, /* 毫: GB2312 Code: 0xBAC1 ==> Row:26 Col:33 */
    {0x6BEF,0xCCBA}, /* 毯: GB2312 Code: 0xCCBA ==> Row:44 Col:26 */
    {0x6BF3,0xEBA5}, /* 毳: GB2312 Code: 0xEBA5 ==> Row:75 Col:05 */
    {0x6BF5,0xEBA7}, /* 毵: GB2312 Code: 0xEBA7 ==> Row:75 Col:07 */
    {0x6BF9,0xEBA8}, /* 毹: GB2312 Code: 0xEBA8 ==> Row:75 Col:08 */
    {0x6BFD,0xEBA6}, /* 毽: GB2312 Code: 0xEBA6 ==> Row:75 Col:06 */
    {0x6C05,0xEBA9}, /* 氅: GB2312 Code: 0xEBA9 ==> Row:75 Col:09 */
    {0x6C06,0xEBAB}, /* 氆: GB2312 Code: 0xEBAB ==> Row:75 Col:11 */
    {0x6C07,0xEBAA}, /* 氇: GB2312 Code: 0xEBAA ==> Row:75 Col:10 */
    {0x6C0D,0xEBAC}, /* 氍: GB2312 Code: 0xEBAC ==> Row:75 Col:12 */
    {0x6C0F,0xCACF}, /* 氏: GB2312 Code: 0xCACF ==> Row:42 Col:47 */
    {0x6C10,0xD8B5}, /* 氐: GB2312 Code: 0xD8B5 ==> Row:56 Col:21 */
    {0x6C11,0xC3F1}, /* 民: GB2312 Code: 0xC3F1 ==> Row:35 Col:81 */
    {0x6C13,0xC3A5}, /* 氓: GB2312 Code: 0xC3A5 ==> Row:35 Col:05 */
    {0x6C14,0xC6F8}, /* 气: GB2312 Code: 0xC6F8 ==> Row:38 Col:88 */
    {0x6C15,0xEBAD}, /* 氕: GB2312 Code: 0xEBAD ==> Row:75 Col:13 */
    {0x6C16,0xC4CA}, /* 氖: GB2312 Code: 0xC4CA ==> Row:36 Col:42 */
    {0x6C18,0xEBAE}, /* 氘: GB2312 Code: 0xEBAE ==> Row:75 Col:14 */
    {0x6C19,0xEBAF}, /* 氙: GB2312 Code: 0xEBAF ==> Row:75 Col:15 */
    {0x6C1A,0xEBB0}, /* 氚: GB2312 Code: 0xEBB0 ==> Row:75 Col:16 */
    {0x6C1B,0xB7D5}, /* 氛: GB2312 Code: 0xB7D5 ==> Row:23 Col:53 */
    {0x6C1F,0xB7FA}, /* 氟: GB2312 Code: 0xB7FA ==> Row:23 Col:90 */
    {0x6C21,0xEBB1}, /* 氡: GB2312 Code: 0xEBB1 ==> Row:75 Col:17 */
    {0x6C22,0xC7E2}, /* 氢: GB2312 Code: 0xC7E2 ==> Row:39 Col:66 */
    {0x6C24,0xEBB3}, /* 氤: GB2312 Code: 0xEBB3 ==> Row:75 Col:19 */
    {0x6C26,0xBAA4}, /* 氦: GB2312 Code: 0xBAA4 ==> Row:26 Col:04 */
    {0x6C27,0xD1F5}, /* 氧: GB2312 Code: 0xD1F5 ==> Row:49 Col:85 */
    {0x6C28,0xB0B1}, /* 氨: GB2312 Code: 0xB0B1 ==> Row:16 Col:17 */
    {0x6C29,0xEBB2}, /* 氩: GB2312 Code: 0xEBB2 ==> Row:75 Col:18 */
    {0x6C2A,0xEBB4}, /* 氪: GB2312 Code: 0xEBB4 ==> Row:75 Col:20 */
    {0x6C2E,0xB5AA}, /* 氮: GB2312 Code: 0xB5AA ==> Row:21 Col:10 */
    {0x6C2F,0xC2C8}, /* 氯: GB2312 Code: 0xC2C8 ==> Row:34 Col:40 */
    {0x6C30,0xC7E8}, /* 氰: GB2312 Code: 0xC7E8 ==> Row:39 Col:72 */
    {0x6C32,0xEBB5}, /* 氲: GB2312 Code: 0xEBB5 ==> Row:75 Col:21 */
    {0x6C34,0xCBAE}, /* 水: GB2312 Code: 0xCBAE ==> Row:43 Col:14 */
    {0x6C35,0xE3DF}, /* 氵: GB2312 Code: 0xE3DF ==> Row:67 Col:63 */
    {0x6C38,0xD3C0}, /* 永: GB2312 Code: 0xD3C0 ==> Row:51 Col:32 */
    {0x6C3D,0xD9DB}, /* 氽: GB2312 Code: 0xD9DB ==> Row:57 Col:59 */
    {0x6C40,0xCDA1}, /* 汀: GB2312 Code: 0xCDA1 ==> Row:45 Col:01 */
    {0x6C41,0xD6AD}, /* 汁: GB2312 Code: 0xD6AD ==> Row:54 Col:13 */
    {0x6C42,0xC7F3}, /* 求: GB2312 Code: 0xC7F3 ==> Row:39 Col:83 */
    {0x6C46,0xD9E0}, /* 汆: GB2312 Code: 0xD9E0 ==> Row:57 Col:64 */
    {0x6C47,0xBBE3}, /* 汇: GB2312 Code: 0xBBE3 ==> Row:27 Col:67 */
    {0x6C49,0xBABA}, /* 汉: GB2312 Code: 0xBABA ==> Row:26 Col:26 */
    {0x6C4A,0xE3E2}, /* 汊: GB2312 Code: 0xE3E2 ==> Row:67 Col:66 */
    {0x6C50,0xCFAB}, /* 汐: GB2312 Code: 0xCFAB ==> Row:47 Col:11 */
    {0x6C54,0xE3E0}, /* 汔: GB2312 Code: 0xE3E0 ==> Row:67 Col:64 */
    {0x6C55,0xC9C7}, /* 汕: GB2312 Code: 0xC9C7 ==> Row:41 Col:39 */
    {0x6C57,0xBAB9}, /* 汗: GB2312 Code: 0xBAB9 ==> Row:26 Col:25 */
    {0x6C5B,0xD1B4}, /* 汛: GB2312 Code: 0xD1B4 ==> Row:49 Col:20 */
    {0x6C5C,0xE3E1}, /* 汜: GB2312 Code: 0xE3E1 ==> Row:67 Col:65 */
    {0x6C5D,0xC8EA}, /* 汝: GB2312 Code: 0xC8EA ==> Row:40 Col:74 */
    {0x6C5E,0xB9AF}, /* 汞: GB2312 Code: 0xB9AF ==> Row:25 Col:15 */
    {0x6C5F,0xBDAD}, /* 江: GB2312 Code: 0xBDAD ==> Row:29 Col:13 */
    {0x6C60,0xB3D8}, /* 池: GB2312 Code: 0xB3D8 ==> Row:19 Col:56 */
    {0x6C61,0xCEDB}, /* 污: GB2312 Code: 0xCEDB ==> Row:46 Col:59 */
    {0x6C64,0xCCC0}, /* 汤: GB2312 Code: 0xCCC0 ==> Row:44 Col:32 */
    {0x6C68,0xE3E8}, /* 汨: GB2312 Code: 0xE3E8 ==> Row:67 Col:72 */
    {0x6C69,0xE3E9}, /* 汩: GB2312 Code: 0xE3E9 ==> Row:67 Col:73 */
    {0x6C6A,0xCDF4}, /* 汪: GB2312 Code: 0xCDF4 ==> Row:45 Col:84 */
    {0x6C70,0xCCAD}, /* 汰: GB2312 Code: 0xCCAD ==> Row:44 Col:13 */
    {0x6C72,0xBCB3}, /* 汲: GB2312 Code: 0xBCB3 ==> Row:28 Col:19 */
    {0x6C74,0xE3EA}, /* 汴: GB2312 Code: 0xE3EA ==> Row:67 Col:74 */
    {0x6C76,0xE3EB}, /* 汶: GB2312 Code: 0xE3EB ==> Row:67 Col:75 */
    {0x6C79,0xD0DA}, /* 汹: GB2312 Code: 0xD0DA ==> Row:48 Col:58 */
    {0x6C7D,0xC6FB}, /* 汽: GB2312 Code: 0xC6FB ==> Row:38 Col:91 */
    {0x6C7E,0xB7DA}, /* 汾: GB2312 Code: 0xB7DA ==> Row:23 Col:58 */
    {0x6C81,0xC7DF}, /* 沁: GB2312 Code: 0xC7DF ==> Row:39 Col:63 */
    {0x6C82,0xD2CA}, /* 沂: GB2312 Code: 0xD2CA ==> Row:50 Col:42 */
    {0x6C83,0xCED6}, /* 沃: GB2312 Code: 0xCED6 ==> Row:46 Col:54 */
    {0x6C85,0xE3E4}, /* 沅: GB2312 Code: 0xE3E4 ==> Row:67 Col:68 */
    {0x6C86,0xE3EC}, /* 沆: GB2312 Code: 0xE3EC ==> Row:67 Col:76 */
    {0x6C88,0xC9F2}, /* 沈: GB2312 Code: 0xC9F2 ==> Row:41 Col:82 */
    {0x6C89,0xB3C1}, /* 沉: GB2312 Code: 0xB3C1 ==> Row:19 Col:33 */
    {0x6C8C,0xE3E7}, /* 沌: GB2312 Code: 0xE3E7 ==> Row:67 Col:71 */
    {0x6C8F,0xC6E3}, /* 沏: GB2312 Code: 0xC6E3 ==> Row:38 Col:67 */
    {0x6C90,0xE3E5}, /* 沐: GB2312 Code: 0xE3E5 ==> Row:67 Col:69 */
    {0x6C93,0xEDB3}, /* 沓: GB2312 Code: 0xEDB3 ==> Row:77 Col:19 */
    {0x6C94,0xE3E6}, /* 沔: GB2312 Code: 0xE3E6 ==> Row:67 Col:70 */
    {0x6C99,0xC9B3}, /* 沙: GB2312 Code: 0xC9B3 ==> Row:41 Col:19 */
    {0x6C9B,0xC5E6}, /* 沛: GB2312 Code: 0xC5E6 ==> Row:37 Col:70 */
    {0x6C9F,0xB9B5}, /* 沟: GB2312 Code: 0xB9B5 ==> Row:25 Col:21 */
    {0x6CA1,0xC3BB}, /* 没: GB2312 Code: 0xC3BB ==> Row:35 Col:27 */
    {0x6CA3,0xE3E3}, /* 沣: GB2312 Code: 0xE3E3 ==> Row:67 Col:67 */
    {0x6CA4,0xC5BD}, /* 沤: GB2312 Code: 0xC5BD ==> Row:37 Col:29 */
    {0x6CA5,0xC1A4}, /* 沥: GB2312 Code: 0xC1A4 ==> Row:33 Col:04 */
    {0x6CA6,0xC2D9}, /* 沦: GB2312 Code: 0xC2D9 ==> Row:34 Col:57 */
    {0x6CA7,0xB2D7}, /* 沧: GB2312 Code: 0xB2D7 ==> Row:18 Col:55 */
    {0x6CA9,0xE3ED}, /* 沩: GB2312 Code: 0xE3ED ==> Row:67 Col:77 */
    {0x6CAA,0xBBA6}, /* 沪: GB2312 Code: 0xBBA6 ==> Row:27 Col:06 */
    {0x6CAB,0xC4AD}, /* 沫: GB2312 Code: 0xC4AD ==> Row:36 Col:13 */
    {0x6CAD,0xE3F0}, /* 沭: GB2312 Code: 0xE3F0 ==> Row:67 Col:80 */
    {0x6CAE,0xBEDA}, /* 沮: GB2312 Code: 0xBEDA ==> Row:30 Col:58 */
    {0x6CB1,0xE3FB}, /* 沱: GB2312 Code: 0xE3FB ==> Row:67 Col:91 */
    {0x6CB2,0xE3F5}, /* 沲: GB2312 Code: 0xE3F5 ==> Row:67 Col:85 */
    {0x6CB3,0xBAD3}, /* 河: GB2312 Code: 0xBAD3 ==> Row:26 Col:51 */
    {0x6CB8,0xB7D0}, /* 沸: GB2312 Code: 0xB7D0 ==> Row:23 Col:48 */
    {0x6CB9,0xD3CD}, /* 油: GB2312 Code: 0xD3CD ==> Row:51 Col:45 */
    {0x6CBB,0xD6CE}, /* 治: GB2312 Code: 0xD6CE ==> Row:54 Col:46 */
    {0x6CBC,0xD5D3}, /* 沼: GB2312 Code: 0xD5D3 ==> Row:53 Col:51 */
    {0x6CBD,0xB9C1}, /* 沽: GB2312 Code: 0xB9C1 ==> Row:25 Col:33 */
    {0x6CBE,0xD5B4}, /* 沾: GB2312 Code: 0xD5B4 ==> Row:53 Col:20 */
    {0x6CBF,0xD1D8}, /* 沿: GB2312 Code: 0xD1D8 ==> Row:49 Col:56 */
    {0x6CC4,0xD0B9}, /* 泄: GB2312 Code: 0xD0B9 ==> Row:48 Col:25 */
    {0x6CC5,0xC7F6}, /* 泅: GB2312 Code: 0xC7F6 ==> Row:39 Col:86 */
    {0x6CC9,0xC8AA}, /* 泉: GB2312 Code: 0xC8AA ==> Row:40 Col:10 */
    {0x6CCA,0xB2B4}, /* 泊: GB2312 Code: 0xB2B4 ==> Row:18 Col:20 */
    {0x6CCC,0xC3DA}, /* 泌: GB2312 Code: 0xC3DA ==> Row:35 Col:58 */
    {0x6CD0,0xE3EE}, /* 泐: GB2312 Code: 0xE3EE ==> Row:67 Col:78 */
    {0x6CD3,0xE3FC}, /* 泓: GB2312 Code: 0xE3FC ==> Row:67 Col:92 */
    {0x6CD4,0xE3EF}, /* 泔: GB2312 Code: 0xE3EF ==> Row:67 Col:79 */
    {0x6CD5,0xB7A8}, /* 法: GB2312 Code: 0xB7A8 ==> Row:23 Col:08 */
    {0x6CD6,0xE3F7}, /* 泖: GB2312 Code: 0xE3F7 ==> Row:67 Col:87 */
    {0x6CD7,0xE3F4}, /* 泗: GB2312 Code: 0xE3F4 ==> Row:67 Col:84 */
    {0x6CDB,0xB7BA}, /* 泛: GB2312 Code: 0xB7BA ==> Row:23 Col:26 */
    {0x6CDE,0xC5A2}, /* 泞: GB2312 Code: 0xC5A2 ==> Row:37 Col:02 */
    {0x6CE0,0xE3F6}, /* 泠: GB2312 Code: 0xE3F6 ==> Row:67 Col:86 */
    {0x6CE1,0xC5DD}, /* 泡: GB2312 Code: 0xC5DD ==> Row:37 Col:61 */
    {0x6CE2,0xB2A8}, /* 波: GB2312 Code: 0xB2A8 ==> Row:18 Col:08 */
    {0x6CE3,0xC6FC}, /* 泣: GB2312 Code: 0xC6FC ==> Row:38 Col:92 */
    {0x6CE5,0xC4E0}, /* 泥: GB2312 Code: 0xC4E0 ==> Row:36 Col:64 */
    {0x6CE8,0xD7A2}, /* 注: GB2312 Code: 0xD7A2 ==> Row:55 Col:02 */
    {0x6CEA,0xC0E1}, /* 泪: GB2312 Code: 0xC0E1 ==> Row:32 Col:65 */
    {0x6CEB,0xE3F9}, /* 泫: GB2312 Code: 0xE3F9 ==> Row:67 Col:89 */
    {0x6CEE,0xE3FA}, /* 泮: GB2312 Code: 0xE3FA ==> Row:67 Col:90 */
    {0x6CEF,0xE3FD}, /* 泯: GB2312 Code: 0xE3FD ==> Row:67 Col:93 */
    {0x6CF0,0xCCA9}, /* 泰: GB2312 Code: 0xCCA9 ==> Row:44 Col:09 */
    {0x6CF1,0xE3F3}, /* 泱: GB2312 Code: 0xE3F3 ==> Row:67 Col:83 */
    {0x6CF3,0xD3BE}, /* 泳: GB2312 Code: 0xD3BE ==> Row:51 Col:30 */
    {0x6CF5,0xB1C3}, /* 泵: GB2312 Code: 0xB1C3 ==> Row:17 Col:35 */
    {0x6CF6,0xEDB4}, /* 泶: GB2312 Code: 0xEDB4 ==> Row:77 Col:20 */
    {0x6CF7,0xE3F1}, /* 泷: GB2312 Code: 0xE3F1 ==> Row:67 Col:81 */
    {0x6CF8,0xE3F2}, /* 泸: GB2312 Code: 0xE3F2 ==> Row:67 Col:82 */
    {0x6CFA,0xE3F8}, /* 泺: GB2312 Code: 0xE3F8 ==> Row:67 Col:88 */
    {0x6CFB,0xD0BA}, /* 泻: GB2312 Code: 0xD0BA ==> Row:48 Col:26 */
    {0x6CFC,0xC6C3}, /* 泼: GB2312 Code: 0xC6C3 ==> Row:38 Col:35 */
    {0x6CFD,0xD4F3}, /* 泽: GB2312 Code: 0xD4F3 ==> Row:52 Col:83 */
    {0x6CFE,0xE3FE}, /* 泾: GB2312 Code: 0xE3FE ==> Row:67 Col:94 */
    {0x6D01,0xBDE0}, /* 洁: GB2312 Code: 0xBDE0 ==> Row:29 Col:64 */
    {0x6D04,0xE4A7}, /* 洄: GB2312 Code: 0xE4A7 ==> Row:68 Col:07 */
    {0x6D07,0xE4A6}, /* 洇: GB2312 Code: 0xE4A6 ==> Row:68 Col:06 */
    {0x6D0B,0xD1F3}, /* 洋: GB2312 Code: 0xD1F3 ==> Row:49 Col:83 */
    {0x6D0C,0xE4A3}, /* 洌: GB2312 Code: 0xE4A3 ==> Row:68 Col:03 */
    {0x6D0E,0xE4A9}, /* 洎: GB2312 Code: 0xE4A9 ==> Row:68 Col:09 */
    {0x6D12,0xC8F7}, /* 洒: GB2312 Code: 0xC8F7 ==> Row:40 Col:87 */
    {0x6D17,0xCFB4}, /* 洗: GB2312 Code: 0xCFB4 ==> Row:47 Col:20 */
    {0x6D19,0xE4A8}, /* 洙: GB2312 Code: 0xE4A8 ==> Row:68 Col:08 */
    {0x6D1A,0xE4AE}, /* 洚: GB2312 Code: 0xE4AE ==> Row:68 Col:14 */
    {0x6D1B,0xC2E5}, /* 洛: GB2312 Code: 0xC2E5 ==> Row:34 Col:69 */
    {0x6D1E,0xB6B4}, /* 洞: GB2312 Code: 0xB6B4 ==> Row:22 Col:20 */
    {0x6D25,0xBDF2}, /* 津: GB2312 Code: 0xBDF2 ==> Row:29 Col:82 */
    {0x6D27,0xE4A2}, /* 洧: GB2312 Code: 0xE4A2 ==> Row:68 Col:02 */
    {0x6D2A,0xBAE9}, /* 洪: GB2312 Code: 0xBAE9 ==> Row:26 Col:73 */
    {0x6D2B,0xE4AA}, /* 洫: GB2312 Code: 0xE4AA ==> Row:68 Col:10 */
    {0x6D2E,0xE4AC}, /* 洮: GB2312 Code: 0xE4AC ==> Row:68 Col:12 */
    {0x6D31,0xB6FD}, /* 洱: GB2312 Code: 0xB6FD ==> Row:22 Col:93 */
    {0x6D32,0xD6DE}, /* 洲: GB2312 Code: 0xD6DE ==> Row:54 Col:62 */
    {0x6D33,0xE4B2}, /* 洳: GB2312 Code: 0xE4B2 ==> Row:68 Col:18 */
    {0x6D35,0xE4AD}, /* 洵: GB2312 Code: 0xE4AD ==> Row:68 Col:13 */
    {0x6D39,0xE4A1}, /* 洹: GB2312 Code: 0xE4A1 ==> Row:68 Col:01 */
    {0x6D3B,0xBBEE}, /* 活: GB2312 Code: 0xBBEE ==> Row:27 Col:78 */
    {0x6D3C,0xCDDD}, /* 洼: GB2312 Code: 0xCDDD ==> Row:45 Col:61 */
    {0x6D3D,0xC7A2}, /* 洽: GB2312 Code: 0xC7A2 ==> Row:39 Col:02 */
    {0x6D3E,0xC5C9}, /* 派: GB2312 Code: 0xC5C9 ==> Row:37 Col:41 */
    {0x6D41,0xC1F7}, /* 流: GB2312 Code: 0xC1F7 ==> Row:33 Col:87 */
    {0x6D43,0xE4A4}, /* 浃: GB2312 Code: 0xE4A4 ==> Row:68 Col:04 */
    {0x6D45,0xC7B3}, /* 浅: GB2312 Code: 0xC7B3 ==> Row:39 Col:19 */
    {0x6D46,0xBDAC}, /* 浆: GB2312 Code: 0xBDAC ==> Row:29 Col:12 */
    {0x6D47,0xBDBD}, /* 浇: GB2312 Code: 0xBDBD ==> Row:29 Col:29 */
    {0x6D48,0xE4A5}, /* 浈: GB2312 Code: 0xE4A5 ==> Row:68 Col:05 */
    {0x6D4A,0xD7C7}, /* 浊: GB2312 Code: 0xD7C7 ==> Row:55 Col:39 */
    {0x6D4B,0xB2E2}, /* 测: GB2312 Code: 0xB2E2 ==> Row:18 Col:66 */
    {0x6D4D,0xE4AB}, /* 浍: GB2312 Code: 0xE4AB ==> Row:68 Col:11 */
    {0x6D4E,0xBCC3}, /* 济: GB2312 Code: 0xBCC3 ==> Row:28 Col:35 */
    {0x6D4F,0xE4AF}, /* 浏: GB2312 Code: 0xE4AF ==> Row:68 Col:15 */
    {0x6D51,0xBBEB}, /* 浑: GB2312 Code: 0xBBEB ==> Row:27 Col:75 */
    {0x6D52,0xE4B0}, /* 浒: GB2312 Code: 0xE4B0 ==> Row:68 Col:16 */
    {0x6D53,0xC5A8}, /* 浓: GB2312 Code: 0xC5A8 ==> Row:37 Col:08 */
    {0x6D54,0xE4B1}, /* 浔: GB2312 Code: 0xE4B1 ==> Row:68 Col:17 */
    {0x6D59,0xD5E3}, /* 浙: GB2312 Code: 0xD5E3 ==> Row:53 Col:67 */
    {0x6D5A,0xBFA3}, /* 浚: GB2312 Code: 0xBFA3 ==> Row:31 Col:03 */
    {0x6D5C,0xE4BA}, /* 浜: GB2312 Code: 0xE4BA ==> Row:68 Col:26 */
    {0x6D5E,0xE4B7}, /* 浞: GB2312 Code: 0xE4B7 ==> Row:68 Col:23 */
    {0x6D60,0xE4BB}, /* 浠: GB2312 Code: 0xE4BB ==> Row:68 Col:27 */
    {0x6D63,0xE4BD}, /* 浣: GB2312 Code: 0xE4BD ==> Row:68 Col:29 */
    {0x6D66,0xC6D6}, /* 浦: GB2312 Code: 0xC6D6 ==> Row:38 Col:54 */
    {0x6D69,0xBAC6}, /* 浩: GB2312 Code: 0xBAC6 ==> Row:26 Col:38 */
    {0x6D6A,0xC0CB}, /* 浪: GB2312 Code: 0xC0CB ==> Row:32 Col:43 */
    {0x6D6E,0xB8A1}, /* 浮: GB2312 Code: 0xB8A1 ==> Row:24 Col:01 */
    {0x6D6F,0xE4B4}, /* 浯: GB2312 Code: 0xE4B4 ==> Row:68 Col:20 */
    {0x6D74,0xD4A1}, /* 浴: GB2312 Code: 0xD4A1 ==> Row:52 Col:01 */
    {0x6D77,0xBAA3}, /* 海: GB2312 Code: 0xBAA3 ==> Row:26 Col:03 */
    {0x6D78,0xBDFE}, /* 浸: GB2312 Code: 0xBDFE ==> Row:29 Col:94 */
    {0x6D7C,0xE4BC}, /* 浼: GB2312 Code: 0xE4BC ==> Row:68 Col:28 */
    {0x6D82,0xCDBF}, /* 涂: GB2312 Code: 0xCDBF ==> Row:45 Col:31 */
    {0x6D85,0xC4F9}, /* 涅: GB2312 Code: 0xC4F9 ==> Row:36 Col:89 */
    {0x6D88,0xCFFB}, /* 消: GB2312 Code: 0xCFFB ==> Row:47 Col:91 */
    {0x6D89,0xC9E6}, /* 涉: GB2312 Code: 0xC9E6 ==> Row:41 Col:70 */
    {0x6D8C,0xD3BF}, /* 涌: GB2312 Code: 0xD3BF ==> Row:51 Col:31 */
    {0x6D8E,0xCFD1}, /* 涎: GB2312 Code: 0xCFD1 ==> Row:47 Col:49 */
    {0x6D91,0xE4B3}, /* 涑: GB2312 Code: 0xE4B3 ==> Row:68 Col:19 */
    {0x6D93,0xE4B8}, /* 涓: GB2312 Code: 0xE4B8 ==> Row:68 Col:24 */
    {0x6D94,0xE4B9}, /* 涔: GB2312 Code: 0xE4B9 ==> Row:68 Col:25 */
    {0x6D95,0xCCE9}, /* 涕: GB2312 Code: 0xCCE9 ==> Row:44 Col:73 */
    {0x6D9B,0xCCCE}, /* 涛: GB2312 Code: 0xCCCE ==> Row:44 Col:46 */
    {0x6D9D,0xC0D4}, /* 涝: GB2312 Code: 0xC0D4 ==> Row:32 Col:52 */
    {0x6D9E,0xE4B5}, /* 涞: GB2312 Code: 0xE4B5 ==> Row:68 Col:21 */
    {0x6D9F,0xC1B0}, /* 涟: GB2312 Code: 0xC1B0 ==> Row:33 Col:16 */
    {0x6DA0,0xE4B6}, /* 涠: GB2312 Code: 0xE4B6 ==> Row:68 Col:22 */
    {0x6DA1,0xCED0}, /* 涡: GB2312 Code: 0xCED0 ==> Row:46 Col:48 */
    {0x6DA3,0xBBC1}, /* 涣: GB2312 Code: 0xBBC1 ==> Row:27 Col:33 */
    {0x6DA4,0xB5D3}, /* 涤: GB2312 Code: 0xB5D3 ==> Row:21 Col:51 */
    {0x6DA6,0xC8F3}, /* 润: GB2312 Code: 0xC8F3 ==> Row:40 Col:83 */
    {0x6DA7,0xBDA7}, /* 涧: GB2312 Code: 0xBDA7 ==> Row:29 Col:07 */
    {0x6DA8,0xD5C7}, /* 涨: GB2312 Code: 0xD5C7 ==> Row:53 Col:39 */
    {0x6DA9,0xC9AC}, /* 涩: GB2312 Code: 0xC9AC ==> Row:41 Col:12 */
    {0x6DAA,0xB8A2}, /* 涪: GB2312 Code: 0xB8A2 ==> Row:24 Col:02 */
    {0x6DAB,0xE4CA}, /* 涫: GB2312 Code: 0xE4CA ==> Row:68 Col:42 */
    {0x6DAE,0xE4CC}, /* 涮: GB2312 Code: 0xE4CC ==> Row:68 Col:44 */
    {0x6DAF,0xD1C4}, /* 涯: GB2312 Code: 0xD1C4 ==> Row:49 Col:36 */
    {0x6DB2,0xD2BA}, /* 液: GB2312 Code: 0xD2BA ==> Row:50 Col:26 */
    {0x6DB5,0xBAAD}, /* 涵: GB2312 Code: 0xBAAD ==> Row:26 Col:13 */
    {0x6DB8,0xBAD4}, /* 涸: GB2312 Code: 0xBAD4 ==> Row:26 Col:52 */
    {0x6DBF,0xE4C3}, /* 涿: GB2312 Code: 0xE4C3 ==> Row:68 Col:35 */
    {0x6DC0,0xB5ED}, /* 淀: GB2312 Code: 0xB5ED ==> Row:21 Col:77 */
    {0x6DC4,0xD7CD}, /* 淄: GB2312 Code: 0xD7CD ==> Row:55 Col:45 */
    {0x6DC5,0xE4C0}, /* 淅: GB2312 Code: 0xE4C0 ==> Row:68 Col:32 */
    {0x6DC6,0xCFFD}, /* 淆: GB2312 Code: 0xCFFD ==> Row:47 Col:93 */
    {0x6DC7,0xE4BF}, /* 淇: GB2312 Code: 0xE4BF ==> Row:68 Col:31 */
    {0x6DCB,0xC1DC}, /* 淋: GB2312 Code: 0xC1DC ==> Row:33 Col:60 */
    {0x6DCC,0xCCCA}, /* 淌: GB2312 Code: 0xCCCA ==> Row:44 Col:42 */
    {0x6DD1,0xCAE7}, /* 淑: GB2312 Code: 0xCAE7 ==> Row:42 Col:71 */
    {0x6DD6,0xC4D7}, /* 淖: GB2312 Code: 0xC4D7 ==> Row:36 Col:55 */
    {0x6DD8,0xCCD4}, /* 淘: GB2312 Code: 0xCCD4 ==> Row:44 Col:52 */
    {0x6DD9,0xE4C8}, /* 淙: GB2312 Code: 0xE4C8 ==> Row:68 Col:40 */
    {0x6DDD,0xE4C7}, /* 淝: GB2312 Code: 0xE4C7 ==> Row:68 Col:39 */
    {0x6DDE,0xE4C1}, /* 淞: GB2312 Code: 0xE4C1 ==> Row:68 Col:33 */
    {0x6DE0,0xE4C4}, /* 淠: GB2312 Code: 0xE4C4 ==> Row:68 Col:36 */
    {0x6DE1,0xB5AD}, /* 淡: GB2312 Code: 0xB5AD ==> Row:21 Col:13 */
    {0x6DE4,0xD3D9}, /* 淤: GB2312 Code: 0xD3D9 ==> Row:51 Col:57 */
    {0x6DE6,0xE4C6}, /* 淦: GB2312 Code: 0xE4C6 ==> Row:68 Col:38 */
    {0x6DEB,0xD2F9}, /* 淫: GB2312 Code: 0xD2F9 ==> Row:50 Col:89 */
    {0x6DEC,0xB4E3}, /* 淬: GB2312 Code: 0xB4E3 ==> Row:20 Col:67 */
    {0x6DEE,0xBBB4}, /* 淮: GB2312 Code: 0xBBB4 ==> Row:27 Col:20 */
    {0x6DF1,0xC9EE}, /* 深: GB2312 Code: 0xC9EE ==> Row:41 Col:78 */
    {0x6DF3,0xB4BE}, /* 淳: GB2312 Code: 0xB4BE ==> Row:20 Col:30 */
    {0x6DF7,0xBBEC}, /* 混: GB2312 Code: 0xBBEC ==> Row:27 Col:76 */
    {0x6DF9,0xD1CD}, /* 淹: GB2312 Code: 0xD1CD ==> Row:49 Col:45 */
    {0x6DFB,0xCCED}, /* 添: GB2312 Code: 0xCCED ==> Row:44 Col:77 */
    {0x6DFC,0xEDB5}, /* 淼: GB2312 Code: 0xEDB5 ==> Row:77 Col:21 */
    {0x6E05,0xC7E5}, /* 清: GB2312 Code: 0xC7E5 ==> Row:39 Col:69 */
    {0x6E0A,0xD4A8}, /* 渊: GB2312 Code: 0xD4A8 ==> Row:52 Col:08 */
    {0x6E0C,0xE4CB}, /* 渌: GB2312 Code: 0xE4CB ==> Row:68 Col:43 */
    {0x6E0D,0xD7D5}, /* 渍: GB2312 Code: 0xD7D5 ==> Row:55 Col:53 */
    {0x6E0E,0xE4C2}, /* 渎: GB2312 Code: 0xE4C2 ==> Row:68 Col:34 */
    {0x6E10,0xBDA5}, /* 渐: GB2312 Code: 0xBDA5 ==> Row:29 Col:05 */
    {0x6E11,0xE4C5}, /* 渑: GB2312 Code: 0xE4C5 ==> Row:68 Col:37 */
    {0x6E14,0xD3E6}, /* 渔: GB2312 Code: 0xD3E6 ==> Row:51 Col:70 */
    {0x6E16,0xE4C9}, /* 渖: GB2312 Code: 0xE4C9 ==> Row:68 Col:41 */
    {0x6E17,0xC9F8}, /* 渗: GB2312 Code: 0xC9F8 ==> Row:41 Col:88 */
    {0x6E1A,0xE4BE}, /* 渚: GB2312 Code: 0xE4BE ==> Row:68 Col:30 */
    {0x6E1D,0xD3E5}, /* 渝: GB2312 Code: 0xD3E5 ==> Row:51 Col:69 */
    {0x6E20,0xC7FE}, /* 渠: GB2312 Code: 0xC7FE ==> Row:39 Col:94 */
    {0x6E21,0xB6C9}, /* 渡: GB2312 Code: 0xB6C9 ==> Row:22 Col:41 */
    {0x6E23,0xD4FC}, /* 渣: GB2312 Code: 0xD4FC ==> Row:52 Col:92 */
    {0x6E24,0xB2B3}, /* 渤: GB2312 Code: 0xB2B3 ==> Row:18 Col:19 */
    {0x6E25,0xE4D7}, /* 渥: GB2312 Code: 0xE4D7 ==> Row:68 Col:55 */
    {0x6E29,0xCEC2}, /* 温: GB2312 Code: 0xCEC2 ==> Row:46 Col:34 */
    {0x6E2B,0xE4CD}, /* 渫: GB2312 Code: 0xE4CD ==> Row:68 Col:45 */
    {0x6E2D,0xCEBC}, /* 渭: GB2312 Code: 0xCEBC ==> Row:46 Col:28 */
    {0x6E2F,0xB8DB}, /* 港: GB2312 Code: 0xB8DB ==> Row:24 Col:59 */
    {0x6E32,0xE4D6}, /* 渲: GB2312 Code: 0xE4D6 ==> Row:68 Col:54 */
    {0x6E34,0xBFCA}, /* 渴: GB2312 Code: 0xBFCA ==> Row:31 Col:42 */
    {0x6E38,0xD3CE}, /* 游: GB2312 Code: 0xD3CE ==> Row:51 Col:46 */
    {0x6E3A,0xC3EC}, /* 渺: GB2312 Code: 0xC3EC ==> Row:35 Col:76 */
    {0x6E43,0xC5C8}, /* 湃: GB2312 Code: 0xC5C8 ==> Row:37 Col:40 */
    {0x6E44,0xE4D8}, /* 湄: GB2312 Code: 0xE4D8 ==> Row:68 Col:56 */
    {0x6E4D,0xCDC4}, /* 湍: GB2312 Code: 0xCDC4 ==> Row:45 Col:36 */
    {0x6E4E,0xE4CF}, /* 湎: GB2312 Code: 0xE4CF ==> Row:68 Col:47 */
    {0x6E53,0xE4D4}, /* 湓: GB2312 Code: 0xE4D4 ==> Row:68 Col:52 */
    {0x6E54,0xE4D5}, /* 湔: GB2312 Code: 0xE4D5 ==> Row:68 Col:53 */
    {0x6E56,0xBAFE}, /* 湖: GB2312 Code: 0xBAFE ==> Row:26 Col:94 */
    {0x6E58,0xCFE6}, /* 湘: GB2312 Code: 0xCFE6 ==> Row:47 Col:70 */
    {0x6E5B,0xD5BF}, /* 湛: GB2312 Code: 0xD5BF ==> Row:53 Col:31 */
    {0x6E5F,0xE4D2}, /* 湟: GB2312 Code: 0xE4D2 ==> Row:68 Col:50 */
    {0x6E6B,0xE4D0}, /* 湫: GB2312 Code: 0xE4D0 ==> Row:68 Col:48 */
    {0x6E6E,0xE4CE}, /* 湮: GB2312 Code: 0xE4CE ==> Row:68 Col:46 */
    {0x6E7E,0xCDE5}, /* 湾: GB2312 Code: 0xCDE5 ==> Row:45 Col:69 */
    {0x6E7F,0xCAAA}, /* 湿: GB2312 Code: 0xCAAA ==> Row:42 Col:10 */
    {0x6E83,0xC0A3}, /* 溃: GB2312 Code: 0xC0A3 ==> Row:32 Col:03 */
    {0x6E85,0xBDA6}, /* 溅: GB2312 Code: 0xBDA6 ==> Row:29 Col:06 */
    {0x6E86,0xE4D3}, /* 溆: GB2312 Code: 0xE4D3 ==> Row:68 Col:51 */
    {0x6E89,0xB8C8}, /* 溉: GB2312 Code: 0xB8C8 ==> Row:24 Col:40 */
    {0x6E8F,0xE4E7}, /* 溏: GB2312 Code: 0xE4E7 ==> Row:68 Col:71 */
    {0x6E90,0xD4B4}, /* 源: GB2312 Code: 0xD4B4 ==> Row:52 Col:20 */
    {0x6E98,0xE4DB}, /* 溘: GB2312 Code: 0xE4DB ==> Row:68 Col:59 */
    {0x6E9C,0xC1EF}, /* 溜: GB2312 Code: 0xC1EF ==> Row:33 Col:79 */
    {0x6E9F,0xE4E9}, /* 溟: GB2312 Code: 0xE4E9 ==> Row:68 Col:73 */
    {0x6EA2,0xD2E7}, /* 溢: GB2312 Code: 0xD2E7 ==> Row:50 Col:71 */
    {0x6EA5,0xE4DF}, /* 溥: GB2312 Code: 0xE4DF ==> Row:68 Col:63 */
    {0x6EA7,0xE4E0}, /* 溧: GB2312 Code: 0xE4E0 ==> Row:68 Col:64 */
    {0x6EAA,0xCFAA}, /* 溪: GB2312 Code: 0xCFAA ==> Row:47 Col:10 */
    {0x6EAF,0xCBDD}, /* 溯: GB2312 Code: 0xCBDD ==> Row:43 Col:61 */
    {0x6EB1,0xE4DA}, /* 溱: GB2312 Code: 0xE4DA ==> Row:68 Col:58 */
    {0x6EB2,0xE4D1}, /* 溲: GB2312 Code: 0xE4D1 ==> Row:68 Col:49 */
    {0x6EB4,0xE4E5}, /* 溴: GB2312 Code: 0xE4E5 ==> Row:68 Col:69 */
    {0x6EB6,0xC8DC}, /* 溶: GB2312 Code: 0xC8DC ==> Row:40 Col:60 */
    {0x6EB7,0xE4E3}, /* 溷: GB2312 Code: 0xE4E3 ==> Row:68 Col:67 */
    {0x6EBA,0xC4E7}, /* 溺: GB2312 Code: 0xC4E7 ==> Row:36 Col:71 */
    {0x6EBB,0xE4E2}, /* 溻: GB2312 Code: 0xE4E2 ==> Row:68 Col:66 */
    {0x6EBD,0xE4E1}, /* 溽: GB2312 Code: 0xE4E1 ==> Row:68 Col:65 */
    {0x6EC1,0xB3FC}, /* 滁: GB2312 Code: 0xB3FC ==> Row:19 Col:92 */
    {0x6EC2,0xE4E8}, /* 滂: GB2312 Code: 0xE4E8 ==> Row:68 Col:72 */
    {0x6EC7,0xB5E1}, /* 滇: GB2312 Code: 0xB5E1 ==> Row:21 Col:65 */
    {0x6ECB,0xD7CC}, /* 滋: GB2312 Code: 0xD7CC ==> Row:55 Col:44 */
    {0x6ECF,0xE4E6}, /* 滏: GB2312 Code: 0xE4E6 ==> Row:68 Col:70 */
    {0x6ED1,0xBBAC}, /* 滑: GB2312 Code: 0xBBAC ==> Row:27 Col:12 */
    {0x6ED3,0xD7D2}, /* 滓: GB2312 Code: 0xD7D2 ==> Row:55 Col:50 */
    {0x6ED4,0xCCCF}, /* 滔: GB2312 Code: 0xCCCF ==> Row:44 Col:47 */
    {0x6ED5,0xEBF8}, /* 滕: GB2312 Code: 0xEBF8 ==> Row:75 Col:88 */
    {0x6ED7,0xE4E4}, /* 滗: GB2312 Code: 0xE4E4 ==> Row:68 Col:68 */
    {0x6EDA,0xB9F6}, /* 滚: GB2312 Code: 0xB9F6 ==> Row:25 Col:86 */
    {0x6EDE,0xD6CD}, /* 滞: GB2312 Code: 0xD6CD ==> Row:54 Col:45 */
    {0x6EDF,0xE4D9}, /* 滟: GB2312 Code: 0xE4D9 ==> Row:68 Col:57 */
    {0x6EE0,0xE4DC}, /* 滠: GB2312 Code: 0xE4DC ==> Row:68 Col:60 */
    {0x6EE1,0xC2FA}, /* 满: GB2312 Code: 0xC2FA ==> Row:34 Col:90 */
    {0x6EE2,0xE4DE}, /* 滢: GB2312 Code: 0xE4DE ==> Row:68 Col:62 */
    {0x6EE4,0xC2CB}, /* 滤: GB2312 Code: 0xC2CB ==> Row:34 Col:43 */
    {0x6EE5,0xC0C4}, /* 滥: GB2312 Code: 0xC0C4 ==> Row:32 Col:36 */
    {0x6EE6,0xC2D0}, /* 滦: GB2312 Code: 0xC2D0 ==> Row:34 Col:48 */
    {0x6EE8,0xB1F5}, /* 滨: GB2312 Code: 0xB1F5 ==> Row:17 Col:85 */
    {0x6EE9,0xCCB2}, /* 滩: GB2312 Code: 0xCCB2 ==> Row:44 Col:18 */
    {0x6EF4,0xB5CE}, /* 滴: GB2312 Code: 0xB5CE ==> Row:21 Col:46 */
    {0x6EF9,0xE4EF}, /* 滹: GB2312 Code: 0xE4EF ==> Row:68 Col:79 */
    {0x6F02,0xC6AF}, /* 漂: GB2312 Code: 0xC6AF ==> Row:38 Col:15 */
    {0x6F06,0xC6E1}, /* 漆: GB2312 Code: 0xC6E1 ==> Row:38 Col:65 */
    {0x6F09,0xE4F5}, /* 漉: GB2312 Code: 0xE4F5 ==> Row:68 Col:85 */
    {0x6F0F,0xC2A9}, /* 漏: GB2312 Code: 0xC2A9 ==> Row:34 Col:09 */
    {0x6F13,0xC0EC}, /* 漓: GB2312 Code: 0xC0EC ==> Row:32 Col:76 */
    {0x6F14,0xD1DD}, /* 演: GB2312 Code: 0xD1DD ==> Row:49 Col:61 */
    {0x6F15,0xE4EE}, /* 漕: GB2312 Code: 0xE4EE ==> Row:68 Col:78 */
    {0x6F20,0xC4AE}, /* 漠: GB2312 Code: 0xC4AE ==> Row:36 Col:14 */
    {0x6F24,0xE4ED}, /* 漤: GB2312 Code: 0xE4ED ==> Row:68 Col:77 */
    {0x6F29,0xE4F6}, /* 漩: GB2312 Code: 0xE4F6 ==> Row:68 Col:86 */
    {0x6F2A,0xE4F4}, /* 漪: GB2312 Code: 0xE4F4 ==> Row:68 Col:84 */
    {0x6F2B,0xC2FE}, /* 漫: GB2312 Code: 0xC2FE ==> Row:34 Col:94 */
    {0x6F2D,0xE4DD}, /* 漭: GB2312 Code: 0xE4DD ==> Row:68 Col:61 */
    {0x6F2F,0xE4F0}, /* 漯: GB2312 Code: 0xE4F0 ==> Row:68 Col:80 */
    {0x6F31,0xCAFE}, /* 漱: GB2312 Code: 0xCAFE ==> Row:42 Col:94 */
    {0x6F33,0xD5C4}, /* 漳: GB2312 Code: 0xD5C4 ==> Row:53 Col:36 */
    {0x6F36,0xE4F1}, /* 漶: GB2312 Code: 0xE4F1 ==> Row:68 Col:81 */
    {0x6F3E,0xD1FA}, /* 漾: GB2312 Code: 0xD1FA ==> Row:49 Col:90 */
    {0x6F46,0xE4EB}, /* 潆: GB2312 Code: 0xE4EB ==> Row:68 Col:75 */
    {0x6F47,0xE4EC}, /* 潇: GB2312 Code: 0xE4EC ==> Row:68 Col:76 */
    {0x6F4B,0xE4F2}, /* 潋: GB2312 Code: 0xE4F2 ==> Row:68 Col:82 */
    {0x6F4D,0xCEAB}, /* 潍: GB2312 Code: 0xCEAB ==> Row:46 Col:11 */
    {0x6F58,0xC5CB}, /* 潘: GB2312 Code: 0xC5CB ==> Row:37 Col:43 */
    {0x6F5C,0xC7B1}, /* 潜: GB2312 Code: 0xC7B1 ==> Row:39 Col:17 */
    {0x6F5E,0xC2BA}, /* 潞: GB2312 Code: 0xC2BA ==> Row:34 Col:26 */
    {0x6F62,0xE4EA}, /* 潢: GB2312 Code: 0xE4EA ==> Row:68 Col:74 */
    {0x6F66,0xC1CA}, /* 潦: GB2312 Code: 0xC1CA ==> Row:33 Col:42 */
    {0x6F6D,0xCCB6}, /* 潭: GB2312 Code: 0xCCB6 ==> Row:44 Col:22 */
    {0x6F6E,0xB3B1}, /* 潮: GB2312 Code: 0xB3B1 ==> Row:19 Col:17 */
    {0x6F72,0xE4FB}, /* 潲: GB2312 Code: 0xE4FB ==> Row:68 Col:91 */
    {0x6F74,0xE4F3}, /* 潴: GB2312 Code: 0xE4F3 ==> Row:68 Col:83 */
    {0x6F78,0xE4FA}, /* 潸: GB2312 Code: 0xE4FA ==> Row:68 Col:90 */
    {0x6F7A,0xE4FD}, /* 潺: GB2312 Code: 0xE4FD ==> Row:68 Col:93 */
    {0x6F7C,0xE4FC}, /* 潼: GB2312 Code: 0xE4FC ==> Row:68 Col:92 */
    {0x6F84,0xB3CE}, /* 澄: GB2312 Code: 0xB3CE ==> Row:19 Col:46 */
    {0x6F88,0xB3BA}, /* 澈: GB2312 Code: 0xB3BA ==> Row:19 Col:26 */
    {0x6F89,0xE4F7}, /* 澉: GB2312 Code: 0xE4F7 ==> Row:68 Col:87 */
    {0x6F8C,0xE4F9}, /* 澌: GB2312 Code: 0xE4F9 ==> Row:68 Col:89 */
    {0x6F8D,0xE4F8}, /* 澍: GB2312 Code: 0xE4F8 ==> Row:68 Col:88 */
    {0x6F8E,0xC5EC}, /* 澎: GB2312 Code: 0xC5EC ==> Row:37 Col:76 */
    {0x6F9C,0xC0BD}, /* 澜: GB2312 Code: 0xC0BD ==> Row:32 Col:29 */
    {0x6FA1,0xD4E8}, /* 澡: GB2312 Code: 0xD4E8 ==> Row:52 Col:72 */
    {0x6FA7,0xE5A2}, /* 澧: GB2312 Code: 0xE5A2 ==> Row:69 Col:02 */
    {0x6FB3,0xB0C4}, /* 澳: GB2312 Code: 0xB0C4 ==> Row:16 Col:36 */
    {0x6FB6,0xE5A4}, /* 澶: GB2312 Code: 0xE5A4 ==> Row:69 Col:04 */
    {0x6FB9,0xE5A3}, /* 澹: GB2312 Code: 0xE5A3 ==> Row:69 Col:03 */
    {0x6FC0,0xBCA4}, /* 激: GB2312 Code: 0xBCA4 ==> Row:28 Col:04 */
    {0x6FC2,0xE5A5}, /* 濂: GB2312 Code: 0xE5A5 ==> Row:69 Col:05 */
    {0x6FC9,0xE5A1}, /* 濉: GB2312 Code: 0xE5A1 ==> Row:69 Col:01 */
    {0x6FD1,0xE4FE}, /* 濑: GB2312 Code: 0xE4FE ==> Row:68 Col:94 */
    {0x6FD2,0xB1F4}, /* 濒: GB2312 Code: 0xB1F4 ==> Row:17 Col:84 */
    {0x6FDE,0xE5A8}, /* 濞: GB2312 Code: 0xE5A8 ==> Row:69 Col:08 */
    {0x6FE0,0xE5A9}, /* 濠: GB2312 Code: 0xE5A9 ==> Row:69 Col:09 */
    {0x6FE1,0xE5A6}, /* 濡: GB2312 Code: 0xE5A6 ==> Row:69 Col:06 */
    {0x6FEE,0xE5A7}, /* 濮: GB2312 Code: 0xE5A7 ==> Row:69 Col:07 */
    {0x6FEF,0xE5AA}, /* 濯: GB2312 Code: 0xE5AA ==> Row:69 Col:10 */
    {0x7011,0xC6D9}, /* 瀑: GB2312 Code: 0xC6D9 ==> Row:38 Col:57 */
    {0x701A,0xE5AB}, /* 瀚: GB2312 Code: 0xE5AB ==> Row:69 Col:11 */
    {0x701B,0xE5AD}, /* 瀛: GB2312 Code: 0xE5AD ==> Row:69 Col:13 */
    {0x7023,0xE5AC}, /* 瀣: GB2312 Code: 0xE5AC ==> Row:69 Col:12 */
    {0x7035,0xE5AF}, /* 瀵: GB2312 Code: 0xE5AF ==> Row:69 Col:15 */
    {0x7039,0xE5AE}, /* 瀹: GB2312 Code: 0xE5AE ==> Row:69 Col:14 */
    {0x704C,0xB9E0}, /* 灌: GB2312 Code: 0xB9E0 ==> Row:25 Col:64 */
    {0x704F,0xE5B0}, /* 灏: GB2312 Code: 0xE5B0 ==> Row:69 Col:16 */
    {0x705E,0xE5B1}, /* 灞: GB2312 Code: 0xE5B1 ==> Row:69 Col:17 */
    {0x706B,0xBBF0}, /* 火: GB2312 Code: 0xBBF0 ==> Row:27 Col:80 */
    {0x706C,0xECE1}, /* 灬: GB2312 Code: 0xECE1 ==> Row:76 Col:65 */
    {0x706D,0xC3F0}, /* 灭: GB2312 Code: 0xC3F0 ==> Row:35 Col:80 */
    {0x706F,0xB5C6}, /* 灯: GB2312 Code: 0xB5C6 ==> Row:21 Col:38 */
    {0x7070,0xBBD2}, /* 灰: GB2312 Code: 0xBBD2 ==> Row:27 Col:50 */
    {0x7075,0xC1E9}, /* 灵: GB2312 Code: 0xC1E9 ==> Row:33 Col:73 */
    {0x7076,0xD4EE}, /* 灶: GB2312 Code: 0xD4EE ==> Row:52 Col:78 */
    {0x7078,0xBEC4}, /* 灸: GB2312 Code: 0xBEC4 ==> Row:30 Col:36 */
    {0x707C,0xD7C6}, /* 灼: GB2312 Code: 0xD7C6 ==> Row:55 Col:38 */
    {0x707E,0xD4D6}, /* 灾: GB2312 Code: 0xD4D6 ==> Row:52 Col:54 */
    {0x707F,0xB2D3}, /* 灿: GB2312 Code: 0xB2D3 ==> Row:18 Col:51 */
    {0x7080,0xECBE}, /* 炀: GB2312 Code: 0xECBE ==> Row:76 Col:30 */
    {0x7085,0xEAC1}, /* 炅: GB2312 Code: 0xEAC1 ==> Row:74 Col:33 */
    {0x7089,0xC2AF}, /* 炉: GB2312 Code: 0xC2AF ==> Row:34 Col:15 */
    {0x708A,0xB4B6}, /* 炊: GB2312 Code: 0xB4B6 ==> Row:20 Col:22 */
    {0x708E,0xD1D7}, /* 炎: GB2312 Code: 0xD1D7 ==> Row:49 Col:55 */
    {0x7092,0xB3B4}, /* 炒: GB2312 Code: 0xB3B4 ==> Row:19 Col:20 */
    {0x7094,0xC8B2}, /* 炔: GB2312 Code: 0xC8B2 ==> Row:40 Col:18 */
    {0x7095,0xBFBB}, /* 炕: GB2312 Code: 0xBFBB ==> Row:31 Col:27 */
    {0x7096,0xECC0}, /* 炖: GB2312 Code: 0xECC0 ==> Row:76 Col:32 */
    {0x7099,0xD6CB}, /* 炙: GB2312 Code: 0xD6CB ==> Row:54 Col:43 */
    {0x709C,0xECBF}, /* 炜: GB2312 Code: 0xECBF ==> Row:76 Col:31 */
    {0x709D,0xECC1}, /* 炝: GB2312 Code: 0xECC1 ==> Row:76 Col:33 */
    {0x70AB,0xECC5}, /* 炫: GB2312 Code: 0xECC5 ==> Row:76 Col:37 */
    {0x70AC,0xBEE6}, /* 炬: GB2312 Code: 0xBEE6 ==> Row:30 Col:70 */
    {0x70AD,0xCCBF}, /* 炭: GB2312 Code: 0xCCBF ==> Row:44 Col:31 */
    {0x70AE,0xC5DA}, /* 炮: GB2312 Code: 0xC5DA ==> Row:37 Col:58 */
    {0x70AF,0xBEBC}, /* 炯: GB2312 Code: 0xBEBC ==> Row:30 Col:28 */
    {0x70B1,0xECC6}, /* 炱: GB2312 Code: 0xECC6 ==> Row:76 Col:38 */
    {0x70B3,0xB1FE}, /* 炳: GB2312 Code: 0xB1FE ==> Row:17 Col:94 */
    {0x70B7,0xECC4}, /* 炷: GB2312 Code: 0xECC4 ==> Row:76 Col:36 */
    {0x70B8,0xD5A8}, /* 炸: GB2312 Code: 0xD5A8 ==> Row:53 Col:08 */
    {0x70B9,0xB5E3}, /* 点: GB2312 Code: 0xB5E3 ==> Row:21 Col:67 */
    {0x70BB,0xECC2}, /* 炻: GB2312 Code: 0xECC2 ==> Row:76 Col:34 */
    {0x70BC,0xC1B6}, /* 炼: GB2312 Code: 0xC1B6 ==> Row:33 Col:22 */
    {0x70BD,0xB3E3}, /* 炽: GB2312 Code: 0xB3E3 ==> Row:19 Col:67 */
    {0x70C0,0xECC3}, /* 烀: GB2312 Code: 0xECC3 ==> Row:76 Col:35 */
    {0x70C1,0xCBB8}, /* 烁: GB2312 Code: 0xCBB8 ==> Row:43 Col:24 */
    {0x70C2,0xC0C3}, /* 烂: GB2312 Code: 0xC0C3 ==> Row:32 Col:35 */
    {0x70C3,0xCCFE}, /* 烃: GB2312 Code: 0xCCFE ==> Row:44 Col:94 */
    {0x70C8,0xC1D2}, /* 烈: GB2312 Code: 0xC1D2 ==> Row:33 Col:50 */
    {0x70CA,0xECC8}, /* 烊: GB2312 Code: 0xECC8 ==> Row:76 Col:40 */
    {0x70D8,0xBAE6}, /* 烘: GB2312 Code: 0xBAE6 ==> Row:26 Col:70 */
    {0x70D9,0xC0D3}, /* 烙: GB2312 Code: 0xC0D3 ==> Row:32 Col:51 */
    {0x70DB,0xD6F2}, /* 烛: GB2312 Code: 0xD6F2 ==> Row:54 Col:82 */
    {0x70DF,0xD1CC}, /* 烟: GB2312 Code: 0xD1CC ==> Row:49 Col:44 */
    {0x70E4,0xBFBE}, /* 烤: GB2312 Code: 0xBFBE ==> Row:31 Col:30 */
    {0x70E6,0xB7B3}, /* 烦: GB2312 Code: 0xB7B3 ==> Row:23 Col:19 */
    {0x70E7,0xC9D5}, /* 烧: GB2312 Code: 0xC9D5 ==> Row:41 Col:53 */
    {0x70E8,0xECC7}, /* 烨: GB2312 Code: 0xECC7 ==> Row:76 Col:39 */
    {0x70E9,0xBBE2}, /* 烩: GB2312 Code: 0xBBE2 ==> Row:27 Col:66 */
    {0x70EB,0xCCCC}, /* 烫: GB2312 Code: 0xCCCC ==> Row:44 Col:44 */
    {0x70EC,0xBDFD}, /* 烬: GB2312 Code: 0xBDFD ==> Row:29 Col:93 */
    {0x70ED,0xC8C8}, /* 热: GB2312 Code: 0xC8C8 ==> Row:40 Col:40 */
    {0x70EF,0xCFA9}, /* 烯: GB2312 Code: 0xCFA9 ==> Row:47 Col:09 */
    {0x70F7,0xCDE9}, /* 烷: GB2312 Code: 0xCDE9 ==> Row:45 Col:73 */
    {0x70F9,0xC5EB}, /* 烹: GB2312 Code: 0xC5EB ==> Row:37 Col:75 */
    {0x70FD,0xB7E9}, /* 烽: GB2312 Code: 0xB7E9 ==> Row:23 Col:73 */
    {0x7109,0xD1C9}, /* 焉: GB2312 Code: 0xD1C9 ==> Row:49 Col:41 */
    {0x710A,0xBAB8}, /* 焊: GB2312 Code: 0xBAB8 ==> Row:26 Col:24 */
    {0x7110,0xECC9}, /* 焐: GB2312 Code: 0xECC9 ==> Row:76 Col:41 */
    {0x7113,0xECCA}, /* 焓: GB2312 Code: 0xECCA ==> Row:76 Col:42 */
    {0x7115,0xBBC0}, /* 焕: GB2312 Code: 0xBBC0 ==> Row:27 Col:32 */
    {0x7116,0xECCB}, /* 焖: GB2312 Code: 0xECCB ==> Row:76 Col:43 */
    {0x7118,0xECE2}, /* 焘: GB2312 Code: 0xECE2 ==> Row:76 Col:66 */
    {0x7119,0xB1BA}, /* 焙: GB2312 Code: 0xB1BA ==> Row:17 Col:26 */
    {0x711A,0xB7D9}, /* 焚: GB2312 Code: 0xB7D9 ==> Row:23 Col:57 */
    {0x7126,0xBDB9}, /* 焦: GB2312 Code: 0xBDB9 ==> Row:29 Col:25 */
    {0x712F,0xECCC}, /* 焯: GB2312 Code: 0xECCC ==> Row:76 Col:44 */
    {0x7130,0xD1E6}, /* 焰: GB2312 Code: 0xD1E6 ==> Row:49 Col:70 */
    {0x7131,0xECCD}, /* 焱: GB2312 Code: 0xECCD ==> Row:76 Col:45 */
    {0x7136,0xC8BB}, /* 然: GB2312 Code: 0xC8BB ==> Row:40 Col:27 */
    {0x7145,0xECD1}, /* 煅: GB2312 Code: 0xECD1 ==> Row:76 Col:49 */
    {0x714A,0xECD3}, /* 煊: GB2312 Code: 0xECD3 ==> Row:76 Col:51 */
    {0x714C,0xBBCD}, /* 煌: GB2312 Code: 0xBBCD ==> Row:27 Col:45 */
    {0x714E,0xBCE5}, /* 煎: GB2312 Code: 0xBCE5 ==> Row:28 Col:69 */
    {0x715C,0xECCF}, /* 煜: GB2312 Code: 0xECCF ==> Row:76 Col:47 */
    {0x715E,0xC9B7}, /* 煞: GB2312 Code: 0xC9B7 ==> Row:41 Col:23 */
    {0x7164,0xC3BA}, /* 煤: GB2312 Code: 0xC3BA ==> Row:35 Col:26 */
    {0x7166,0xECE3}, /* 煦: GB2312 Code: 0xECE3 ==> Row:76 Col:67 */
    {0x7167,0xD5D5}, /* 照: GB2312 Code: 0xD5D5 ==> Row:53 Col:53 */
    {0x7168,0xECD0}, /* 煨: GB2312 Code: 0xECD0 ==> Row:76 Col:48 */
    {0x716E,0xD6F3}, /* 煮: GB2312 Code: 0xD6F3 ==> Row:54 Col:83 */
    {0x7172,0xECD2}, /* 煲: GB2312 Code: 0xECD2 ==> Row:76 Col:50 */
    {0x7173,0xECCE}, /* 煳: GB2312 Code: 0xECCE ==> Row:76 Col:46 */
    {0x7178,0xECD4}, /* 煸: GB2312 Code: 0xECD4 ==> Row:76 Col:52 */
    {0x717A,0xECD5}, /* 煺: GB2312 Code: 0xECD5 ==> Row:76 Col:53 */
    {0x717D,0xC9BF}, /* 煽: GB2312 Code: 0xC9BF ==> Row:41 Col:31 */
    {0x7184,0xCFA8}, /* 熄: GB2312 Code: 0xCFA8 ==> Row:47 Col:08 */
    {0x718A,0xD0DC}, /* 熊: GB2312 Code: 0xD0DC ==> Row:48 Col:60 */
    {0x718F,0xD1AC}, /* 熏: GB2312 Code: 0xD1AC ==> Row:49 Col:12 */
    {0x7194,0xC8DB}, /* 熔: GB2312 Code: 0xC8DB ==> Row:40 Col:59 */
    {0x7198,0xECD6}, /* 熘: GB2312 Code: 0xECD6 ==> Row:76 Col:54 */
    {0x7199,0xCEF5}, /* 熙: GB2312 Code: 0xCEF5 ==> Row:46 Col:85 */
    {0x719F,0xCAEC}, /* 熟: GB2312 Code: 0xCAEC ==> Row:42 Col:76 */
    {0x71A0,0xECDA}, /* 熠: GB2312 Code: 0xECDA ==> Row:76 Col:58 */
    {0x71A8,0xECD9}, /* 熨: GB2312 Code: 0xECD9 ==> Row:76 Col:57 */
    {0x71AC,0xB0BE}, /* 熬: GB2312 Code: 0xB0BE ==> Row:16 Col:30 */
    {0x71B3,0xECD7}, /* 熳: GB2312 Code: 0xECD7 ==> Row:76 Col:55 */
    {0x71B5,0xECD8}, /* 熵: GB2312 Code: 0xECD8 ==> Row:76 Col:56 */
    {0x71B9,0xECE4}, /* 熹: GB2312 Code: 0xECE4 ==> Row:76 Col:68 */
    {0x71C3,0xC8BC}, /* 燃: GB2312 Code: 0xC8BC ==> Row:40 Col:28 */
    {0x71CE,0xC1C7}, /* 燎: GB2312 Code: 0xC1C7 ==> Row:33 Col:39 */
    {0x71D4,0xECDC}, /* 燔: GB2312 Code: 0xECDC ==> Row:76 Col:60 */
    {0x71D5,0xD1E0}, /* 燕: GB2312 Code: 0xD1E0 ==> Row:49 Col:64 */
    {0x71E0,0xECDB}, /* 燠: GB2312 Code: 0xECDB ==> Row:76 Col:59 */
    {0x71E5,0xD4EF}, /* 燥: GB2312 Code: 0xD4EF ==> Row:52 Col:79 */
    {0x71E7,0xECDD}, /* 燧: GB2312 Code: 0xECDD ==> Row:76 Col:61 */
    {0x71EE,0xDBC6}, /* 燮: GB2312 Code: 0xDBC6 ==> Row:59 Col:38 */
    {0x71F9,0xECDE}, /* 燹: GB2312 Code: 0xECDE ==> Row:76 Col:62 */
    {0x7206,0xB1AC}, /* 爆: GB2312 Code: 0xB1AC ==> Row:17 Col:12 */
    {0x721D,0xECDF}, /* 爝: GB2312 Code: 0xECDF ==> Row:76 Col:63 */
    {0x7228,0xECE0}, /* 爨: GB2312 Code: 0xECE0 ==> Row:76 Col:64 */
    {0x722A,0xD7A6}, /* 爪: GB2312 Code: 0xD7A6 ==> Row:55 Col:06 */
    {0x722C,0xC5C0}, /* 爬: GB2312 Code: 0xC5C0 ==> Row:37 Col:32 */
    {0x7230,0xEBBC}, /* 爰: GB2312 Code: 0xEBBC ==> Row:75 Col:28 */
    {0x7231,0xB0AE}, /* 爱: GB2312 Code: 0xB0AE ==> Row:16 Col:14 */
    {0x7235,0xBEF4}, /* 爵: GB2312 Code: 0xBEF4 ==> Row:30 Col:84 */
    {0x7236,0xB8B8}, /* 父: GB2312 Code: 0xB8B8 ==> Row:24 Col:24 */
    {0x7237,0xD2AF}, /* 爷: GB2312 Code: 0xD2AF ==> Row:50 Col:15 */
    {0x7238,0xB0D6}, /* 爸: GB2312 Code: 0xB0D6 ==> Row:16 Col:54 */
    {0x7239,0xB5F9}, /* 爹: GB2312 Code: 0xB5F9 ==> Row:21 Col:89 */
    {0x723B,0xD8B3}, /* 爻: GB2312 Code: 0xD8B3 ==> Row:56 Col:19 */
    {0x723D,0xCBAC}, /* 爽: GB2312 Code: 0xCBAC ==> Row:43 Col:12 */
    {0x723F,0xE3DD}, /* 爿: GB2312 Code: 0xE3DD ==> Row:67 Col:61 */
    {0x7247,0xC6AC}, /* 片: GB2312 Code: 0xC6AC ==> Row:38 Col:12 */
    {0x7248,0xB0E6}, /* 版: GB2312 Code: 0xB0E6 ==> Row:16 Col:70 */
    {0x724C,0xC5C6}, /* 牌: GB2312 Code: 0xC5C6 ==> Row:37 Col:38 */
    {0x724D,0xEBB9}, /* 牍: GB2312 Code: 0xEBB9 ==> Row:75 Col:25 */
    {0x7252,0xEBBA}, /* 牒: GB2312 Code: 0xEBBA ==> Row:75 Col:26 */
    {0x7256,0xEBBB}, /* 牖: GB2312 Code: 0xEBBB ==> Row:75 Col:27 */
    {0x7259,0xD1C0}, /* 牙: GB2312 Code: 0xD1C0 ==> Row:49 Col:32 */
    {0x725B,0xC5A3}, /* 牛: GB2312 Code: 0xC5A3 ==> Row:37 Col:03 */
    {0x725D,0xEAF2}, /* 牝: GB2312 Code: 0xEAF2 ==> Row:74 Col:82 */
    {0x725F,0xC4B2}, /* 牟: GB2312 Code: 0xC4B2 ==> Row:36 Col:18 */
    {0x7261,0xC4B5}, /* 牡: GB2312 Code: 0xC4B5 ==> Row:36 Col:21 */
    {0x7262,0xC0CE}, /* 牢: GB2312 Code: 0xC0CE ==> Row:32 Col:46 */
    {0x7266,0xEAF3}, /* 牦: GB2312 Code: 0xEAF3 ==> Row:74 Col:83 */
    {0x7267,0xC4C1}, /* 牧: GB2312 Code: 0xC4C1 ==> Row:36 Col:33 */
    {0x7269,0xCEEF}, /* 物: GB2312 Code: 0xCEEF ==> Row:46 Col:79 */
    {0x726E,0xEAF0}, /* 牮: GB2312 Code: 0xEAF0 ==> Row:74 Col:80 */
    {0x726F,0xEAF4}, /* 牯: GB2312 Code: 0xEAF4 ==> Row:74 Col:84 */
    {0x7272,0xC9FC}, /* 牲: GB2312 Code: 0xC9FC ==> Row:41 Col:92 */
    {0x7275,0xC7A3}, /* 牵: GB2312 Code: 0xC7A3 ==> Row:39 Col:03 */
    {0x7279,0xCCD8}, /* 特: GB2312 Code: 0xCCD8 ==> Row:44 Col:56 */
    {0x727A,0xCEFE}, /* 牺: GB2312 Code: 0xCEFE ==> Row:46 Col:94 */
    {0x727E,0xEAF5}, /* 牾: GB2312 Code: 0xEAF5 ==> Row:74 Col:85 */
    {0x727F,0xEAF6}, /* 牿: GB2312 Code: 0xEAF6 ==> Row:74 Col:86 */
    {0x7280,0xCFAC}, /* 犀: GB2312 Code: 0xCFAC ==> Row:47 Col:12 */
    {0x7281,0xC0E7}, /* 犁: GB2312 Code: 0xC0E7 ==> Row:32 Col:71 */
    {0x7284,0xEAF7}, /* 犄: GB2312 Code: 0xEAF7 ==> Row:74 Col:87 */
    {0x728A,0xB6BF}, /* 犊: GB2312 Code: 0xB6BF ==> Row:22 Col:31 */
    {0x728B,0xEAF8}, /* 犋: GB2312 Code: 0xEAF8 ==> Row:74 Col:88 */
    {0x728D,0xEAF9}, /* 犍: GB2312 Code: 0xEAF9 ==> Row:74 Col:89 */
    {0x728F,0xEAFA}, /* 犏: GB2312 Code: 0xEAFA ==> Row:74 Col:90 */
    {0x7292,0xEAFB}, /* 犒: GB2312 Code: 0xEAFB ==> Row:74 Col:91 */
    {0x729F,0xEAF1}, /* 犟: GB2312 Code: 0xEAF1 ==> Row:74 Col:81 */
    {0x72AC,0xC8AE}, /* 犬: GB2312 Code: 0xC8AE ==> Row:40 Col:14 */
    {0x72AD,0xE1EB}, /* 犭: GB2312 Code: 0xE1EB ==> Row:65 Col:75 */
    {0x72AF,0xB7B8}, /* 犯: GB2312 Code: 0xB7B8 ==> Row:23 Col:24 */
    {0x72B0,0xE1EC}, /* 犰: GB2312 Code: 0xE1EC ==> Row:65 Col:76 */
    {0x72B4,0xE1ED}, /* 犴: GB2312 Code: 0xE1ED ==> Row:65 Col:77 */
    {0x72B6,0xD7B4}, /* 状: GB2312 Code: 0xD7B4 ==> Row:55 Col:20 */
    {0x72B7,0xE1EE}, /* 犷: GB2312 Code: 0xE1EE ==> Row:65 Col:78 */
    {0x72B8,0xE1EF}, /* 犸: GB2312 Code: 0xE1EF ==> Row:65 Col:79 */
    {0x72B9,0xD3CC}, /* 犹: GB2312 Code: 0xD3CC ==> Row:51 Col:44 */
    {0x72C1,0xE1F1}, /* 狁: GB2312 Code: 0xE1F1 ==> Row:65 Col:81 */
    {0x72C2,0xBFF1}, /* 狂: GB2312 Code: 0xBFF1 ==> Row:31 Col:81 */
    {0x72C3,0xE1F0}, /* 狃: GB2312 Code: 0xE1F0 ==> Row:65 Col:80 */
    {0x72C4,0xB5D2}, /* 狄: GB2312 Code: 0xB5D2 ==> Row:21 Col:50 */
    {0x72C8,0xB1B7}, /* 狈: GB2312 Code: 0xB1B7 ==> Row:17 Col:23 */
    {0x72CD,0xE1F3}, /* 狍: GB2312 Code: 0xE1F3 ==> Row:65 Col:83 */
    {0x72CE,0xE1F2}, /* 狎: GB2312 Code: 0xE1F2 ==> Row:65 Col:82 */
    {0x72D0,0xBAFC}, /* 狐: GB2312 Code: 0xBAFC ==> Row:26 Col:92 */
    {0x72D2,0xE1F4}, /* 狒: GB2312 Code: 0xE1F4 ==> Row:65 Col:84 */
    {0x72D7,0xB9B7}, /* 狗: GB2312 Code: 0xB9B7 ==> Row:25 Col:23 */
    {0x72D9,0xBED1}, /* 狙: GB2312 Code: 0xBED1 ==> Row:30 Col:49 */
    {0x72DE,0xC4FC}, /* 狞: GB2312 Code: 0xC4FC ==> Row:36 Col:92 */
    {0x72E0,0xBADD}, /* 狠: GB2312 Code: 0xBADD ==> Row:26 Col:61 */
    {0x72E1,0xBDC6}, /* 狡: GB2312 Code: 0xBDC6 ==> Row:29 Col:38 */
    {0x72E8,0xE1F5}, /* 狨: GB2312 Code: 0xE1F5 ==> Row:65 Col:85 */
    {0x72E9,0xE1F7}, /* 狩: GB2312 Code: 0xE1F7 ==> Row:65 Col:87 */
    {0x72EC,0xB6C0}, /* 独: GB2312 Code: 0xB6C0 ==> Row:22 Col:32 */
    {0x72ED,0xCFC1}, /* 狭: GB2312 Code: 0xCFC1 ==> Row:47 Col:33 */
    {0x72EE,0xCAA8}, /* 狮: GB2312 Code: 0xCAA8 ==> Row:42 Col:08 */
    {0x72EF,0xE1F6}, /* 狯: GB2312 Code: 0xE1F6 ==> Row:65 Col:86 */
    {0x72F0,0xD5F8}, /* 狰: GB2312 Code: 0xD5F8 ==> Row:53 Col:88 */
    {0x72F1,0xD3FC}, /* 狱: GB2312 Code: 0xD3FC ==> Row:51 Col:92 */
    {0x72F2,0xE1F8}, /* 狲: GB2312 Code: 0xE1F8 ==> Row:65 Col:88 */
    {0x72F3,0xE1FC}, /* 狳: GB2312 Code: 0xE1FC ==> Row:65 Col:92 */
    {0x72F4,0xE1F9}, /* 狴: GB2312 Code: 0xE1F9 ==> Row:65 Col:89 */
    {0x72F7,0xE1FA}, /* 狷: GB2312 Code: 0xE1FA ==> Row:65 Col:90 */
    {0x72F8,0xC0EA}, /* 狸: GB2312 Code: 0xC0EA ==> Row:32 Col:74 */
    {0x72FA,0xE1FE}, /* 狺: GB2312 Code: 0xE1FE ==> Row:65 Col:94 */
    {0x72FB,0xE2A1}, /* 狻: GB2312 Code: 0xE2A1 ==> Row:66 Col:01 */
    {0x72FC,0xC0C7}, /* 狼: GB2312 Code: 0xC0C7 ==> Row:32 Col:39 */
    {0x7301,0xE1FB}, /* 猁: GB2312 Code: 0xE1FB ==> Row:65 Col:91 */
    {0x7303,0xE1FD}, /* 猃: GB2312 Code: 0xE1FD ==> Row:65 Col:93 */
    {0x730A,0xE2A5}, /* 猊: GB2312 Code: 0xE2A5 ==> Row:66 Col:05 */
    {0x730E,0xC1D4}, /* 猎: GB2312 Code: 0xC1D4 ==> Row:33 Col:52 */
    {0x7313,0xE2A3}, /* 猓: GB2312 Code: 0xE2A3 ==> Row:66 Col:03 */
    {0x7315,0xE2A8}, /* 猕: GB2312 Code: 0xE2A8 ==> Row:66 Col:08 */
    {0x7316,0xB2FE}, /* 猖: GB2312 Code: 0xB2FE ==> Row:18 Col:94 */
    {0x7317,0xE2A2}, /* 猗: GB2312 Code: 0xE2A2 ==> Row:66 Col:02 */
    {0x731B,0xC3CD}, /* 猛: GB2312 Code: 0xC3CD ==> Row:35 Col:45 */
    {0x731C,0xB2C2}, /* 猜: GB2312 Code: 0xB2C2 ==> Row:18 Col:34 */
    {0x731D,0xE2A7}, /* 猝: GB2312 Code: 0xE2A7 ==> Row:66 Col:07 */
    {0x731E,0xE2A6}, /* 猞: GB2312 Code: 0xE2A6 ==> Row:66 Col:06 */
    {0x7321,0xE2A4}, /* 猡: GB2312 Code: 0xE2A4 ==> Row:66 Col:04 */
    {0x7322,0xE2A9}, /* 猢: GB2312 Code: 0xE2A9 ==> Row:66 Col:09 */
    {0x7325,0xE2AB}, /* 猥: GB2312 Code: 0xE2AB ==> Row:66 Col:11 */
    {0x7329,0xD0C9}, /* 猩: GB2312 Code: 0xD0C9 ==> Row:48 Col:41 */
    {0x732A,0xD6ED}, /* 猪: GB2312 Code: 0xD6ED ==> Row:54 Col:77 */
    {0x732B,0xC3A8}, /* 猫: GB2312 Code: 0xC3A8 ==> Row:35 Col:08 */
    {0x732C,0xE2AC}, /* 猬: GB2312 Code: 0xE2AC ==> Row:66 Col:12 */
    {0x732E,0xCFD7}, /* 献: GB2312 Code: 0xCFD7 ==> Row:47 Col:55 */
    {0x7331,0xE2AE}, /* 猱: GB2312 Code: 0xE2AE ==> Row:66 Col:14 */
    {0x7334,0xBAEF}, /* 猴: GB2312 Code: 0xBAEF ==> Row:26 Col:79 */
    {0x7337,0xE9E0}, /* 猷: GB2312 Code: 0xE9E0 ==> Row:73 Col:64 */
    {0x7338,0xE2AD}, /* 猸: GB2312 Code: 0xE2AD ==> Row:66 Col:13 */
    {0x7339,0xE2AA}, /* 猹: GB2312 Code: 0xE2AA ==> Row:66 Col:10 */
    {0x733E,0xBBAB}, /* 猾: GB2312 Code: 0xBBAB ==> Row:27 Col:11 */
    {0x733F,0xD4B3}, /* 猿: GB2312 Code: 0xD4B3 ==> Row:52 Col:19 */
    {0x734D,0xE2B0}, /* 獍: GB2312 Code: 0xE2B0 ==> Row:66 Col:16 */
    {0x7350,0xE2AF}, /* 獐: GB2312 Code: 0xE2AF ==> Row:66 Col:15 */
    {0x7352,0xE9E1}, /* 獒: GB2312 Code: 0xE9E1 ==> Row:73 Col:65 */
    {0x7357,0xE2B1}, /* 獗: GB2312 Code: 0xE2B1 ==> Row:66 Col:17 */
    {0x7360,0xE2B2}, /* 獠: GB2312 Code: 0xE2B2 ==> Row:66 Col:18 */
    {0x736C,0xE2B3}, /* 獬: GB2312 Code: 0xE2B3 ==> Row:66 Col:19 */
    {0x736D,0xCCA1}, /* 獭: GB2312 Code: 0xCCA1 ==> Row:44 Col:01 */
    {0x736F,0xE2B4}, /* 獯: GB2312 Code: 0xE2B4 ==> Row:66 Col:20 */
    {0x737E,0xE2B5}, /* 獾: GB2312 Code: 0xE2B5 ==> Row:66 Col:21 */
    {0x7384,0xD0FE}, /* 玄: GB2312 Code: 0xD0FE ==> Row:48 Col:94 */
    {0x7387,0xC2CA}, /* 率: GB2312 Code: 0xC2CA ==> Row:34 Col:42 */
    {0x7389,0xD3F1}, /* 玉: GB2312 Code: 0xD3F1 ==> Row:51 Col:81 */
    {0x738B,0xCDF5}, /* 王: GB2312 Code: 0xCDF5 ==> Row:45 Col:85 */
    {0x738E,0xE7E0}, /* 玎: GB2312 Code: 0xE7E0 ==> Row:71 Col:64 */
    {0x7391,0xE7E1}, /* 玑: GB2312 Code: 0xE7E1 ==> Row:71 Col:65 */
    {0x7396,0xBEC1}, /* 玖: GB2312 Code: 0xBEC1 ==> Row:30 Col:33 */
    {0x739B,0xC2EA}, /* 玛: GB2312 Code: 0xC2EA ==> Row:34 Col:74 */
    {0x739F,0xE7E4}, /* 玟: GB2312 Code: 0xE7E4 ==> Row:71 Col:68 */
    {0x73A2,0xE7E3}, /* 玢: GB2312 Code: 0xE7E3 ==> Row:71 Col:67 */
    {0x73A9,0xCDE6}, /* 玩: GB2312 Code: 0xCDE6 ==> Row:45 Col:70 */
    {0x73AB,0xC3B5}, /* 玫: GB2312 Code: 0xC3B5 ==> Row:35 Col:21 */
    {0x73AE,0xE7E2}, /* 玮: GB2312 Code: 0xE7E2 ==> Row:71 Col:66 */
    {0x73AF,0xBBB7}, /* 环: GB2312 Code: 0xBBB7 ==> Row:27 Col:23 */
    {0x73B0,0xCFD6}, /* 现: GB2312 Code: 0xCFD6 ==> Row:47 Col:54 */
    {0x73B2,0xC1E1}, /* 玲: GB2312 Code: 0xC1E1 ==> Row:33 Col:65 */
    {0x73B3,0xE7E9}, /* 玳: GB2312 Code: 0xE7E9 ==> Row:71 Col:73 */
    {0x73B7,0xE7E8}, /* 玷: GB2312 Code: 0xE7E8 ==> Row:71 Col:72 */
    {0x73BA,0xE7F4}, /* 玺: GB2312 Code: 0xE7F4 ==> Row:71 Col:84 */
    {0x73BB,0xB2A3}, /* 玻: GB2312 Code: 0xB2A3 ==> Row:18 Col:03 */
    {0x73C0,0xE7EA}, /* 珀: GB2312 Code: 0xE7EA ==> Row:71 Col:74 */
    {0x73C2,0xE7E6}, /* 珂: GB2312 Code: 0xE7E6 ==> Row:71 Col:70 */
    {0x73C8,0xE7EC}, /* 珈: GB2312 Code: 0xE7EC ==> Row:71 Col:76 */
    {0x73C9,0xE7EB}, /* 珉: GB2312 Code: 0xE7EB ==> Row:71 Col:75 */
    {0x73CA,0xC9BA}, /* 珊: GB2312 Code: 0xC9BA ==> Row:41 Col:26 */
    {0x73CD,0xD5E4}, /* 珍: GB2312 Code: 0xD5E4 ==> Row:53 Col:68 */
    {0x73CF,0xE7E5}, /* 珏: GB2312 Code: 0xE7E5 ==> Row:71 Col:69 */
    {0x73D0,0xB7A9}, /* 珐: GB2312 Code: 0xB7A9 ==> Row:23 Col:09 */
    {0x73D1,0xE7E7}, /* 珑: GB2312 Code: 0xE7E7 ==> Row:71 Col:71 */
    {0x73D9,0xE7EE}, /* 珙: GB2312 Code: 0xE7EE ==> Row:71 Col:78 */
    {0x73DE,0xE7F3}, /* 珞: GB2312 Code: 0xE7F3 ==> Row:71 Col:83 */
    {0x73E0,0xD6E9}, /* 珠: GB2312 Code: 0xD6E9 ==> Row:54 Col:73 */
    {0x73E5,0xE7ED}, /* 珥: GB2312 Code: 0xE7ED ==> Row:71 Col:77 */
    {0x73E7,0xE7F2}, /* 珧: GB2312 Code: 0xE7F2 ==> Row:71 Col:82 */
    {0x73E9,0xE7F1}, /* 珩: GB2312 Code: 0xE7F1 ==> Row:71 Col:81 */
    {0x73ED,0xB0E0}, /* 班: GB2312 Code: 0xB0E0 ==> Row:16 Col:64 */
    {0x73F2,0xE7F5}, /* 珲: GB2312 Code: 0xE7F5 ==> Row:71 Col:85 */
    {0x7403,0xC7F2}, /* 球: GB2312 Code: 0xC7F2 ==> Row:39 Col:82 */
    {0x7405,0xC0C5}, /* 琅: GB2312 Code: 0xC0C5 ==> Row:32 Col:37 */
    {0x7406,0xC0ED}, /* 理: GB2312 Code: 0xC0ED ==> Row:32 Col:77 */
    {0x7409,0xC1F0}, /* 琉: GB2312 Code: 0xC1F0 ==> Row:33 Col:80 */
    {0x740A,0xE7F0}, /* 琊: GB2312 Code: 0xE7F0 ==> Row:71 Col:80 */
    {0x740F,0xE7F6}, /* 琏: GB2312 Code: 0xE7F6 ==> Row:71 Col:86 */
    {0x7410,0xCBF6}, /* 琐: GB2312 Code: 0xCBF6 ==> Row:43 Col:86 */
    {0x741A,0xE8A2}, /* 琚: GB2312 Code: 0xE8A2 ==> Row:72 Col:02 */
    {0x741B,0xE8A1}, /* 琛: GB2312 Code: 0xE8A1 ==> Row:72 Col:01 */
    {0x7422,0xD7C1}, /* 琢: GB2312 Code: 0xD7C1 ==> Row:55 Col:33 */
    {0x7425,0xE7FA}, /* 琥: GB2312 Code: 0xE7FA ==> Row:71 Col:90 */
    {0x7426,0xE7F9}, /* 琦: GB2312 Code: 0xE7F9 ==> Row:71 Col:89 */
    {0x7428,0xE7FB}, /* 琨: GB2312 Code: 0xE7FB ==> Row:71 Col:91 */
    {0x742A,0xE7F7}, /* 琪: GB2312 Code: 0xE7F7 ==> Row:71 Col:87 */
    {0x742C,0xE7FE}, /* 琬: GB2312 Code: 0xE7FE ==> Row:71 Col:94 */
    {0x742E,0xE7FD}, /* 琮: GB2312 Code: 0xE7FD ==> Row:71 Col:93 */
    {0x7430,0xE7FC}, /* 琰: GB2312 Code: 0xE7FC ==> Row:71 Col:92 */
    {0x7433,0xC1D5}, /* 琳: GB2312 Code: 0xC1D5 ==> Row:33 Col:53 */
    {0x7434,0xC7D9}, /* 琴: GB2312 Code: 0xC7D9 ==> Row:39 Col:57 */
    {0x7435,0xC5FD}, /* 琵: GB2312 Code: 0xC5FD ==> Row:37 Col:93 */
    {0x7436,0xC5C3}, /* 琶: GB2312 Code: 0xC5C3 ==> Row:37 Col:35 */
    {0x743C,0xC7ED}, /* 琼: GB2312 Code: 0xC7ED ==> Row:39 Col:77 */
    {0x7441,0xE8A3}, /* 瑁: GB2312 Code: 0xE8A3 ==> Row:72 Col:03 */
    {0x7455,0xE8A6}, /* 瑕: GB2312 Code: 0xE8A6 ==> Row:72 Col:06 */
    {0x7457,0xE8A5}, /* 瑗: GB2312 Code: 0xE8A5 ==> Row:72 Col:05 */
    {0x7459,0xE8A7}, /* 瑙: GB2312 Code: 0xE8A7 ==> Row:72 Col:07 */
    {0x745A,0xBAF7}, /* 瑚: GB2312 Code: 0xBAF7 ==> Row:26 Col:87 */
    {0x745B,0xE7F8}, /* 瑛: GB2312 Code: 0xE7F8 ==> Row:71 Col:88 */
    {0x745C,0xE8A4}, /* 瑜: GB2312 Code: 0xE8A4 ==> Row:72 Col:04 */
    {0x745E,0xC8F0}, /* 瑞: GB2312 Code: 0xC8F0 ==> Row:40 Col:80 */
    {0x745F,0xC9AA}, /* 瑟: GB2312 Code: 0xC9AA ==> Row:41 Col:10 */
    {0x746D,0xE8A9}, /* 瑭: GB2312 Code: 0xE8A9 ==> Row:72 Col:09 */
    {0x7470,0xB9E5}, /* 瑰: GB2312 Code: 0xB9E5 ==> Row:25 Col:69 */
    {0x7476,0xD1FE}, /* 瑶: GB2312 Code: 0xD1FE ==> Row:49 Col:94 */
    {0x7477,0xE8A8}, /* 瑷: GB2312 Code: 0xE8A8 ==> Row:72 Col:08 */
    {0x747E,0xE8AA}, /* 瑾: GB2312 Code: 0xE8AA ==> Row:72 Col:10 */
    {0x7480,0xE8AD}, /* 璀: GB2312 Code: 0xE8AD ==> Row:72 Col:13 */
    {0x7481,0xE8AE}, /* 璁: GB2312 Code: 0xE8AE ==> Row:72 Col:14 */
    {0x7483,0xC1A7}, /* 璃: GB2312 Code: 0xC1A7 ==> Row:33 Col:07 */
    {0x7487,0xE8AF}, /* 璇: GB2312 Code: 0xE8AF ==> Row:72 Col:15 */
    {0x748B,0xE8B0}, /* 璋: GB2312 Code: 0xE8B0 ==> Row:72 Col:16 */
    {0x748E,0xE8AC}, /* 璎: GB2312 Code: 0xE8AC ==> Row:72 Col:12 */
    {0x7490,0xE8B4}, /* 璐: GB2312 Code: 0xE8B4 ==> Row:72 Col:20 */
    {0x749C,0xE8AB}, /* 璜: GB2312 Code: 0xE8AB ==> Row:72 Col:11 */
    {0x749E,0xE8B1}, /* 璞: GB2312 Code: 0xE8B1 ==> Row:72 Col:17 */
    {0x74A7,0xE8B5}, /* 璧: GB2312 Code: 0xE8B5 ==> Row:72 Col:21 */
    {0x74A8,0xE8B2}, /* 璨: GB2312 Code: 0xE8B2 ==> Row:72 Col:18 */
    {0x74A9,0xE8B3}, /* 璩: GB2312 Code: 0xE8B3 ==> Row:72 Col:19 */
    {0x74BA,0xE8B7}, /* 璺: GB2312 Code: 0xE8B7 ==> Row:72 Col:23 */
    {0x74D2,0xE8B6}, /* 瓒: GB2312 Code: 0xE8B6 ==> Row:72 Col:22 */
    {0x74DC,0xB9CF}, /* 瓜: GB2312 Code: 0xB9CF ==> Row:25 Col:47 */
    {0x74DE,0xF0AC}, /* 瓞: GB2312 Code: 0xF0AC ==> Row:80 Col:12 */
    {0x74E0,0xF0AD}, /* 瓠: GB2312 Code: 0xF0AD ==> Row:80 Col:13 */
    {0x74E2,0xC6B0}, /* 瓢: GB2312 Code: 0xC6B0 ==> Row:38 Col:16 */
    {0x74E3,0xB0EA}, /* 瓣: GB2312 Code: 0xB0EA ==> Row:16 Col:74 */
    {0x74E4,0xC8BF}, /* 瓤: GB2312 Code: 0xC8BF ==> Row:40 Col:31 */
    {0x74E6,0xCDDF}, /* 瓦: GB2312 Code: 0xCDDF ==> Row:45 Col:63 */
    {0x74EE,0xCECD}, /* 瓮: GB2312 Code: 0xCECD ==> Row:46 Col:45 */
    {0x74EF,0xEAB1}, /* 瓯: GB2312 Code: 0xEAB1 ==> Row:74 Col:17 */
    {0x74F4,0xEAB2}, /* 瓴: GB2312 Code: 0xEAB2 ==> Row:74 Col:18 */
    {0x74F6,0xC6BF}, /* 瓶: GB2312 Code: 0xC6BF ==> Row:38 Col:31 */
    {0x74F7,0xB4C9}, /* 瓷: GB2312 Code: 0xB4C9 ==> Row:20 Col:41 */
    {0x74FF,0xEAB3}, /* 瓿: GB2312 Code: 0xEAB3 ==> Row:74 Col:19 */
    {0x7504,0xD5E7}, /* 甄: GB2312 Code: 0xD5E7 ==> Row:53 Col:71 */
    {0x750D,0xDDF9}, /* 甍: GB2312 Code: 0xDDF9 ==> Row:61 Col:89 */
    {0x750F,0xEAB4}, /* 甏: GB2312 Code: 0xEAB4 ==> Row:74 Col:20 */
    {0x7511,0xEAB5}, /* 甑: GB2312 Code: 0xEAB5 ==> Row:74 Col:21 */
    {0x7513,0xEAB6}, /* 甓: GB2312 Code: 0xEAB6 ==> Row:74 Col:22 */
    {0x7518,0xB8CA}, /* 甘: GB2312 Code: 0xB8CA ==> Row:24 Col:42 */
    {0x7519,0xDFB0}, /* 甙: GB2312 Code: 0xDFB0 ==> Row:63 Col:16 */
    {0x751A,0xC9F5}, /* 甚: GB2312 Code: 0xC9F5 ==> Row:41 Col:85 */
    {0x751C,0xCCF0}, /* 甜: GB2312 Code: 0xCCF0 ==> Row:44 Col:80 */
    {0x751F,0xC9FA}, /* 生: GB2312 Code: 0xC9FA ==> Row:41 Col:90 */
    {0x7525,0xC9FB}, /* 甥: GB2312 Code: 0xC9FB ==> Row:41 Col:91 */
    {0x7528,0xD3C3}, /* 用: GB2312 Code: 0xD3C3 ==> Row:51 Col:35 */
    {0x7529,0xCBA6}, /* 甩: GB2312 Code: 0xCBA6 ==> Row:43 Col:06 */
    {0x752B,0xB8A6}, /* 甫: GB2312 Code: 0xB8A6 ==> Row:24 Col:06 */
    {0x752C,0xF0AE}, /* 甬: GB2312 Code: 0xF0AE ==> Row:80 Col:14 */
    {0x752D,0xB1C2}, /* 甭: GB2312 Code: 0xB1C2 ==> Row:17 Col:34 */
    {0x752F,0xE5B8}, /* 甯: GB2312 Code: 0xE5B8 ==> Row:69 Col:24 */
    {0x7530,0xCCEF}, /* 田: GB2312 Code: 0xCCEF ==> Row:44 Col:79 */
    {0x7531,0xD3C9}, /* 由: GB2312 Code: 0xD3C9 ==> Row:51 Col:41 */
    {0x7532,0xBCD7}, /* 甲: GB2312 Code: 0xBCD7 ==> Row:28 Col:55 */
    {0x7533,0xC9EA}, /* 申: GB2312 Code: 0xC9EA ==> Row:41 Col:74 */
    {0x7535,0xB5E7}, /* 电: GB2312 Code: 0xB5E7 ==> Row:21 Col:71 */
    {0x7537,0xC4D0}, /* 男: GB2312 Code: 0xC4D0 ==> Row:36 Col:48 */
    {0x7538,0xB5E9}, /* 甸: GB2312 Code: 0xB5E9 ==> Row:21 Col:73 */
    {0x753A,0xEEAE}, /* 町: GB2312 Code: 0xEEAE ==> Row:78 Col:14 */
    {0x753B,0xBBAD}, /* 画: GB2312 Code: 0xBBAD ==> Row:27 Col:13 */
    {0x753E,0xE7DE}, /* 甾: GB2312 Code: 0xE7DE ==> Row:71 Col:62 */
    {0x7540,0xEEAF}, /* 畀: GB2312 Code: 0xEEAF ==> Row:78 Col:15 */
    {0x7545,0xB3A9}, /* 畅: GB2312 Code: 0xB3A9 ==> Row:19 Col:09 */
    {0x7548,0xEEB2}, /* 畈: GB2312 Code: 0xEEB2 ==> Row:78 Col:18 */
    {0x754B,0xEEB1}, /* 畋: GB2312 Code: 0xEEB1 ==> Row:78 Col:17 */
    {0x754C,0xBDE7}, /* 界: GB2312 Code: 0xBDE7 ==> Row:29 Col:71 */
    {0x754E,0xEEB0}, /* 畎: GB2312 Code: 0xEEB0 ==> Row:78 Col:16 */
    {0x754F,0xCEB7}, /* 畏: GB2312 Code: 0xCEB7 ==> Row:46 Col:23 */
    {0x7554,0xC5CF}, /* 畔: GB2312 Code: 0xC5CF ==> Row:37 Col:47 */
    {0x7559,0xC1F4}, /* 留: GB2312 Code: 0xC1F4 ==> Row:33 Col:84 */
    {0x755A,0xDBCE}, /* 畚: GB2312 Code: 0xDBCE ==> Row:59 Col:46 */
    {0x755B,0xEEB3}, /* 畛: GB2312 Code: 0xEEB3 ==> Row:78 Col:19 */
    {0x755C,0xD0F3}, /* 畜: GB2312 Code: 0xD0F3 ==> Row:48 Col:83 */
    {0x7565,0xC2D4}, /* 略: GB2312 Code: 0xC2D4 ==> Row:34 Col:52 */
    {0x7566,0xC6E8}, /* 畦: GB2312 Code: 0xC6E8 ==> Row:38 Col:72 */
    {0x756A,0xB7AC}, /* 番: GB2312 Code: 0xB7AC ==> Row:23 Col:12 */
    {0x7572,0xEEB4}, /* 畲: GB2312 Code: 0xEEB4 ==> Row:78 Col:20 */
    {0x7574,0xB3EB}, /* 畴: GB2312 Code: 0xB3EB ==> Row:19 Col:75 */
    {0x7578,0xBBFB}, /* 畸: GB2312 Code: 0xBBFB ==> Row:27 Col:91 */
    {0x7579,0xEEB5}, /* 畹: GB2312 Code: 0xEEB5 ==> Row:78 Col:21 */
    {0x757F,0xE7DC}, /* 畿: GB2312 Code: 0xE7DC ==> Row:71 Col:60 */
    {0x7583,0xEEB6}, /* 疃: GB2312 Code: 0xEEB6 ==> Row:78 Col:22 */
    {0x7586,0xBDAE}, /* 疆: GB2312 Code: 0xBDAE ==> Row:29 Col:14 */
    {0x758B,0xF1E2}, /* 疋: GB2312 Code: 0xF1E2 ==> Row:81 Col:66 */
    {0x758F,0xCAE8}, /* 疏: GB2312 Code: 0xCAE8 ==> Row:42 Col:72 */
    {0x7591,0xD2C9}, /* 疑: GB2312 Code: 0xD2C9 ==> Row:50 Col:41 */
    {0x7592,0xF0DA}, /* 疒: GB2312 Code: 0xF0DA ==> Row:80 Col:58 */
    {0x7594,0xF0DB}, /* 疔: GB2312 Code: 0xF0DB ==> Row:80 Col:59 */
    {0x7596,0xF0DC}, /* 疖: GB2312 Code: 0xF0DC ==> Row:80 Col:60 */
    {0x7597,0xC1C6}, /* 疗: GB2312 Code: 0xC1C6 ==> Row:33 Col:38 */
    {0x7599,0xB8ED}, /* 疙: GB2312 Code: 0xB8ED ==> Row:24 Col:77 */
    {0x759A,0xBECE}, /* 疚: GB2312 Code: 0xBECE ==> Row:30 Col:46 */
    {0x759D,0xF0DE}, /* 疝: GB2312 Code: 0xF0DE ==> Row:80 Col:62 */
    {0x759F,0xC5B1}, /* 疟: GB2312 Code: 0xC5B1 ==> Row:37 Col:17 */
    {0x75A0,0xF0DD}, /* 疠: GB2312 Code: 0xF0DD ==> Row:80 Col:61 */
    {0x75A1,0xD1F1}, /* 疡: GB2312 Code: 0xD1F1 ==> Row:49 Col:81 */
    {0x75A3,0xF0E0}, /* 疣: GB2312 Code: 0xF0E0 ==> Row:80 Col:64 */
    {0x75A4,0xB0CC}, /* 疤: GB2312 Code: 0xB0CC ==> Row:16 Col:44 */
    {0x75A5,0xBDEA}, /* 疥: GB2312 Code: 0xBDEA ==> Row:29 Col:74 */
    {0x75AB,0xD2DF}, /* 疫: GB2312 Code: 0xD2DF ==> Row:50 Col:63 */
    {0x75AC,0xF0DF}, /* 疬: GB2312 Code: 0xF0DF ==> Row:80 Col:63 */
    {0x75AE,0xB4AF}, /* 疮: GB2312 Code: 0xB4AF ==> Row:20 Col:15 */
    {0x75AF,0xB7E8}, /* 疯: GB2312 Code: 0xB7E8 ==> Row:23 Col:72 */
    {0x75B0,0xF0E6}, /* 疰: GB2312 Code: 0xF0E6 ==> Row:80 Col:70 */
    {0x75B1,0xF0E5}, /* 疱: GB2312 Code: 0xF0E5 ==> Row:80 Col:69 */
    {0x75B2,0xC6A3}, /* 疲: GB2312 Code: 0xC6A3 ==> Row:38 Col:03 */
    {0x75B3,0xF0E1}, /* 疳: GB2312 Code: 0xF0E1 ==> Row:80 Col:65 */
    {0x75B4,0xF0E2}, /* 疴: GB2312 Code: 0xF0E2 ==> Row:80 Col:66 */
    {0x75B5,0xB4C3}, /* 疵: GB2312 Code: 0xB4C3 ==> Row:20 Col:35 */
    {0x75B8,0xF0E3}, /* 疸: GB2312 Code: 0xF0E3 ==> Row:80 Col:67 */
    {0x75B9,0xD5EE}, /* 疹: GB2312 Code: 0xD5EE ==> Row:53 Col:78 */
    {0x75BC,0xCCDB}, /* 疼: GB2312 Code: 0xCCDB ==> Row:44 Col:59 */
    {0x75BD,0xBED2}, /* 疽: GB2312 Code: 0xBED2 ==> Row:30 Col:50 */
    {0x75BE,0xBCB2}, /* 疾: GB2312 Code: 0xBCB2 ==> Row:28 Col:18 */
    {0x75C2,0xF0E8}, /* 痂: GB2312 Code: 0xF0E8 ==> Row:80 Col:72 */
    {0x75C3,0xF0E7}, /* 痃: GB2312 Code: 0xF0E7 ==> Row:80 Col:71 */
    {0x75C4,0xF0E4}, /* 痄: GB2312 Code: 0xF0E4 ==> Row:80 Col:68 */
    {0x75C5,0xB2A1}, /* 病: GB2312 Code: 0xB2A1 ==> Row:18 Col:01 */
    {0x75C7,0xD6A2}, /* 症: GB2312 Code: 0xD6A2 ==> Row:54 Col:02 */
    {0x75C8,0xD3B8}, /* 痈: GB2312 Code: 0xD3B8 ==> Row:51 Col:24 */
    {0x75C9,0xBEB7}, /* 痉: GB2312 Code: 0xBEB7 ==> Row:30 Col:23 */
    {0x75CA,0xC8AC}, /* 痊: GB2312 Code: 0xC8AC ==> Row:40 Col:12 */
    {0x75CD,0xF0EA}, /* 痍: GB2312 Code: 0xF0EA ==> Row:80 Col:74 */
    {0x75D2,0xD1F7}, /* 痒: GB2312 Code: 0xD1F7 ==> Row:49 Col:87 */
    {0x75D4,0xD6CC}, /* 痔: GB2312 Code: 0xD6CC ==> Row:54 Col:44 */
    {0x75D5,0xBADB}, /* 痕: GB2312 Code: 0xBADB ==> Row:26 Col:59 */
    {0x75D6,0xF0E9}, /* 痖: GB2312 Code: 0xF0E9 ==> Row:80 Col:73 */
    {0x75D8,0xB6BB}, /* 痘: GB2312 Code: 0xB6BB ==> Row:22 Col:27 */
    {0x75DB,0xCDB4}, /* 痛: GB2312 Code: 0xCDB4 ==> Row:45 Col:20 */
    {0x75DE,0xC6A6}, /* 痞: GB2312 Code: 0xC6A6 ==> Row:38 Col:06 */
    {0x75E2,0xC1A1}, /* 痢: GB2312 Code: 0xC1A1 ==> Row:33 Col:01 */
    {0x75E3,0xF0EB}, /* 痣: GB2312 Code: 0xF0EB ==> Row:80 Col:75 */
    {0x75E4,0xF0EE}, /* 痤: GB2312 Code: 0xF0EE ==> Row:80 Col:78 */
    {0x75E6,0xF0ED}, /* 痦: GB2312 Code: 0xF0ED ==> Row:80 Col:77 */
    {0x75E7,0xF0F0}, /* 痧: GB2312 Code: 0xF0F0 ==> Row:80 Col:80 */
    {0x75E8,0xF0EC}, /* 痨: GB2312 Code: 0xF0EC ==> Row:80 Col:76 */
    {0x75EA,0xBBBE}, /* 痪: GB2312 Code: 0xBBBE ==> Row:27 Col:30 */
    {0x75EB,0xF0EF}, /* 痫: GB2312 Code: 0xF0EF ==> Row:80 Col:79 */
    {0x75F0,0xCCB5}, /* 痰: GB2312 Code: 0xCCB5 ==> Row:44 Col:21 */
    {0x75F1,0xF0F2}, /* 痱: GB2312 Code: 0xF0F2 ==> Row:80 Col:82 */
    {0x75F4,0xB3D5}, /* 痴: GB2312 Code: 0xB3D5 ==> Row:19 Col:53 */
    {0x75F9,0xB1D4}, /* 痹: GB2312 Code: 0xB1D4 ==> Row:17 Col:52 */
    {0x75FC,0xF0F3}, /* 痼: GB2312 Code: 0xF0F3 ==> Row:80 Col:83 */
    {0x75FF,0xF0F4}, /* 痿: GB2312 Code: 0xF0F4 ==> Row:80 Col:84 */
    {0x7600,0xF0F6}, /* 瘀: GB2312 Code: 0xF0F6 ==> Row:80 Col:86 */
    {0x7601,0xB4E1}, /* 瘁: GB2312 Code: 0xB4E1 ==> Row:20 Col:65 */
    {0x7603,0xF0F1}, /* 瘃: GB2312 Code: 0xF0F1 ==> Row:80 Col:81 */
    {0x7605,0xF0F7}, /* 瘅: GB2312 Code: 0xF0F7 ==> Row:80 Col:87 */
    {0x760A,0xF0FA}, /* 瘊: GB2312 Code: 0xF0FA ==> Row:80 Col:90 */
    {0x760C,0xF0F8}, /* 瘌: GB2312 Code: 0xF0F8 ==> Row:80 Col:88 */
    {0x7610,0xF0F5}, /* 瘐: GB2312 Code: 0xF0F5 ==> Row:80 Col:85 */
    {0x7615,0xF0FD}, /* 瘕: GB2312 Code: 0xF0FD ==> Row:80 Col:93 */
    {0x7617,0xF0F9}, /* 瘗: GB2312 Code: 0xF0F9 ==> Row:80 Col:89 */
    {0x7618,0xF0FC}, /* 瘘: GB2312 Code: 0xF0FC ==> Row:80 Col:92 */
    {0x7619,0xF0FE}, /* 瘙: GB2312 Code: 0xF0FE ==> Row:80 Col:94 */
    {0x761B,0xF1A1}, /* 瘛: GB2312 Code: 0xF1A1 ==> Row:81 Col:01 */
    {0x761F,0xCEC1}, /* 瘟: GB2312 Code: 0xCEC1 ==> Row:46 Col:33 */
    {0x7620,0xF1A4}, /* 瘠: GB2312 Code: 0xF1A4 ==> Row:81 Col:04 */
    {0x7622,0xF1A3}, /* 瘢: GB2312 Code: 0xF1A3 ==> Row:81 Col:03 */
    {0x7624,0xC1F6}, /* 瘤: GB2312 Code: 0xC1F6 ==> Row:33 Col:86 */
    {0x7625,0xF0FB}, /* 瘥: GB2312 Code: 0xF0FB ==> Row:80 Col:91 */
    {0x7626,0xCADD}, /* 瘦: GB2312 Code: 0xCADD ==> Row:42 Col:61 */
    {0x7629,0xB4F1}, /* 瘩: GB2312 Code: 0xB4F1 ==> Row:20 Col:81 */
    {0x762A,0xB1F1}, /* 瘪: GB2312 Code: 0xB1F1 ==> Row:17 Col:81 */
    {0x762B,0xCCB1}, /* 瘫: GB2312 Code: 0xCCB1 ==> Row:44 Col:17 */
    {0x762D,0xF1A6}, /* 瘭: GB2312 Code: 0xF1A6 ==> Row:81 Col:06 */
    {0x7630,0xF1A7}, /* 瘰: GB2312 Code: 0xF1A7 ==> Row:81 Col:07 */
    {0x7633,0xF1AC}, /* 瘳: GB2312 Code: 0xF1AC ==> Row:81 Col:12 */
    {0x7634,0xD5CE}, /* 瘴: GB2312 Code: 0xD5CE ==> Row:53 Col:46 */
    {0x7635,0xF1A9}, /* 瘵: GB2312 Code: 0xF1A9 ==> Row:81 Col:09 */
    {0x7638,0xC8B3}, /* 瘸: GB2312 Code: 0xC8B3 ==> Row:40 Col:19 */
    {0x763C,0xF1A2}, /* 瘼: GB2312 Code: 0xF1A2 ==> Row:81 Col:02 */
    {0x763E,0xF1AB}, /* 瘾: GB2312 Code: 0xF1AB ==> Row:81 Col:11 */
    {0x763F,0xF1A8}, /* 瘿: GB2312 Code: 0xF1A8 ==> Row:81 Col:08 */
    {0x7640,0xF1A5}, /* 癀: GB2312 Code: 0xF1A5 ==> Row:81 Col:05 */
    {0x7643,0xF1AA}, /* 癃: GB2312 Code: 0xF1AA ==> Row:81 Col:10 */
    {0x764C,0xB0A9}, /* 癌: GB2312 Code: 0xB0A9 ==> Row:16 Col:09 */
    {0x764D,0xF1AD}, /* 癍: GB2312 Code: 0xF1AD ==> Row:81 Col:13 */
    {0x7654,0xF1AF}, /* 癔: GB2312 Code: 0xF1AF ==> Row:81 Col:15 */
    {0x7656,0xF1B1}, /* 癖: GB2312 Code: 0xF1B1 ==> Row:81 Col:17 */
    {0x765C,0xF1B0}, /* 癜: GB2312 Code: 0xF1B0 ==> Row:81 Col:16 */
    {0x765E,0xF1AE}, /* 癞: GB2312 Code: 0xF1AE ==> Row:81 Col:14 */
    {0x7663,0xD1A2}, /* 癣: GB2312 Code: 0xD1A2 ==> Row:49 Col:02 */
    {0x766B,0xF1B2}, /* 癫: GB2312 Code: 0xF1B2 ==> Row:81 Col:18 */
    {0x766F,0xF1B3}, /* 癯: GB2312 Code: 0xF1B3 ==> Row:81 Col:19 */
    {0x7678,0xB9EF}, /* 癸: GB2312 Code: 0xB9EF ==> Row:25 Col:79 */
    {0x767B,0xB5C7}, /* 登: GB2312 Code: 0xB5C7 ==> Row:21 Col:39 */
    {0x767D,0xB0D7}, /* 白: GB2312 Code: 0xB0D7 ==> Row:16 Col:55 */
    {0x767E,0xB0D9}, /* 百: GB2312 Code: 0xB0D9 ==> Row:16 Col:57 */
    {0x7682,0xD4ED}, /* 皂: GB2312 Code: 0xD4ED ==> Row:52 Col:77 */
    {0x7684,0xB5C4}, /* 的: GB2312 Code: 0xB5C4 ==> Row:21 Col:36 */
    {0x7686,0xBDD4}, /* 皆: GB2312 Code: 0xBDD4 ==> Row:29 Col:52 */
    {0x7687,0xBBCA}, /* 皇: GB2312 Code: 0xBBCA ==> Row:27 Col:42 */
    {0x7688,0xF0A7}, /* 皈: GB2312 Code: 0xF0A7 ==> Row:80 Col:07 */
    {0x768B,0xB8DE}, /* 皋: GB2312 Code: 0xB8DE ==> Row:24 Col:62 */
    {0x768E,0xF0A8}, /* 皎: GB2312 Code: 0xF0A8 ==> Row:80 Col:08 */
    {0x7691,0xB0A8}, /* 皑: GB2312 Code: 0xB0A8 ==> Row:16 Col:08 */
    {0x7693,0xF0A9}, /* 皓: GB2312 Code: 0xF0A9 ==> Row:80 Col:09 */
    {0x7696,0xCDEE}, /* 皖: GB2312 Code: 0xCDEE ==> Row:45 Col:78 */
    {0x7699,0xF0AA}, /* 皙: GB2312 Code: 0xF0AA ==> Row:80 Col:10 */
    {0x76A4,0xF0AB}, /* 皤: GB2312 Code: 0xF0AB ==> Row:80 Col:11 */
    {0x76AE,0xC6A4}, /* 皮: GB2312 Code: 0xC6A4 ==> Row:38 Col:04 */
    {0x76B1,0xD6E5}, /* 皱: GB2312 Code: 0xD6E5 ==> Row:54 Col:69 */
    {0x76B2,0xF1E4}, /* 皲: GB2312 Code: 0xF1E4 ==> Row:81 Col:68 */
    {0x76B4,0xF1E5}, /* 皴: GB2312 Code: 0xF1E5 ==> Row:81 Col:69 */
    {0x76BF,0xC3F3}, /* 皿: GB2312 Code: 0xC3F3 ==> Row:35 Col:83 */
    {0x76C2,0xD3DB}, /* 盂: GB2312 Code: 0xD3DB ==> Row:51 Col:59 */
    {0x76C5,0xD6D1}, /* 盅: GB2312 Code: 0xD6D1 ==> Row:54 Col:49 */
    {0x76C6,0xC5E8}, /* 盆: GB2312 Code: 0xC5E8 ==> Row:37 Col:72 */
    {0x76C8,0xD3AF}, /* 盈: GB2312 Code: 0xD3AF ==> Row:51 Col:15 */
    {0x76CA,0xD2E6}, /* 益: GB2312 Code: 0xD2E6 ==> Row:50 Col:70 */
    {0x76CD,0xEEC1}, /* 盍: GB2312 Code: 0xEEC1 ==> Row:78 Col:33 */
    {0x76CE,0xB0BB}, /* 盎: GB2312 Code: 0xB0BB ==> Row:16 Col:27 */
    {0x76CF,0xD5B5}, /* 盏: GB2312 Code: 0xD5B5 ==> Row:53 Col:21 */
    {0x76D0,0xD1CE}, /* 盐: GB2312 Code: 0xD1CE ==> Row:49 Col:46 */
    {0x76D1,0xBCE0}, /* 监: GB2312 Code: 0xBCE0 ==> Row:28 Col:64 */
    {0x76D2,0xBAD0}, /* 盒: GB2312 Code: 0xBAD0 ==> Row:26 Col:48 */
    {0x76D4,0xBFF8}, /* 盔: GB2312 Code: 0xBFF8 ==> Row:31 Col:88 */
    {0x76D6,0xB8C7}, /* 盖: GB2312 Code: 0xB8C7 ==> Row:24 Col:39 */
    {0x76D7,0xB5C1}, /* 盗: GB2312 Code: 0xB5C1 ==> Row:21 Col:33 */
    {0x76D8,0xC5CC}, /* 盘: GB2312 Code: 0xC5CC ==> Row:37 Col:44 */
    {0x76DB,0xCAA2}, /* 盛: GB2312 Code: 0xCAA2 ==> Row:42 Col:02 */
    {0x76DF,0xC3CB}, /* 盟: GB2312 Code: 0xC3CB ==> Row:35 Col:43 */
    {0x76E5,0xEEC2}, /* 盥: GB2312 Code: 0xEEC2 ==> Row:78 Col:34 */
    {0x76EE,0xC4BF}, /* 目: GB2312 Code: 0xC4BF ==> Row:36 Col:31 */
    {0x76EF,0xB6A2}, /* 盯: GB2312 Code: 0xB6A2 ==> Row:22 Col:02 */
    {0x76F1,0xEDEC}, /* 盱: GB2312 Code: 0xEDEC ==> Row:77 Col:76 */
    {0x76F2,0xC3A4}, /* 盲: GB2312 Code: 0xC3A4 ==> Row:35 Col:04 */
    {0x76F4,0xD6B1}, /* 直: GB2312 Code: 0xD6B1 ==> Row:54 Col:17 */
    {0x76F8,0xCFE0}, /* 相: GB2312 Code: 0xCFE0 ==> Row:47 Col:64 */
    {0x76F9,0xEDEF}, /* 盹: GB2312 Code: 0xEDEF ==> Row:77 Col:79 */
    {0x76FC,0xC5CE}, /* 盼: GB2312 Code: 0xC5CE ==> Row:37 Col:46 */
    {0x76FE,0xB6DC}, /* 盾: GB2312 Code: 0xB6DC ==> Row:22 Col:60 */
    {0x7701,0xCAA1}, /* 省: GB2312 Code: 0xCAA1 ==> Row:42 Col:01 */
    {0x7704,0xEDED}, /* 眄: GB2312 Code: 0xEDED ==> Row:77 Col:77 */
    {0x7707,0xEDF0}, /* 眇: GB2312 Code: 0xEDF0 ==> Row:77 Col:80 */
    {0x7708,0xEDF1}, /* 眈: GB2312 Code: 0xEDF1 ==> Row:77 Col:81 */
    {0x7709,0xC3BC}, /* 眉: GB2312 Code: 0xC3BC ==> Row:35 Col:28 */
    {0x770B,0xBFB4}, /* 看: GB2312 Code: 0xBFB4 ==> Row:31 Col:20 */
    {0x770D,0xEDEE}, /* 眍: GB2312 Code: 0xEDEE ==> Row:77 Col:78 */
    {0x7719,0xEDF4}, /* 眙: GB2312 Code: 0xEDF4 ==> Row:77 Col:84 */
    {0x771A,0xEDF2}, /* 眚: GB2312 Code: 0xEDF2 ==> Row:77 Col:82 */
    {0x771F,0xD5E6}, /* 真: GB2312 Code: 0xD5E6 ==> Row:53 Col:70 */
    {0x7720,0xC3DF}, /* 眠: GB2312 Code: 0xC3DF ==> Row:35 Col:63 */
    {0x7722,0xEDF3}, /* 眢: GB2312 Code: 0xEDF3 ==> Row:77 Col:83 */
    {0x7726,0xEDF6}, /* 眦: GB2312 Code: 0xEDF6 ==> Row:77 Col:86 */
    {0x7728,0xD5A3}, /* 眨: GB2312 Code: 0xD5A3 ==> Row:53 Col:03 */
    {0x7729,0xD1A3}, /* 眩: GB2312 Code: 0xD1A3 ==> Row:49 Col:03 */
    {0x772D,0xEDF5}, /* 眭: GB2312 Code: 0xEDF5 ==> Row:77 Col:85 */
    {0x772F,0xC3D0}, /* 眯: GB2312 Code: 0xC3D0 ==> Row:35 Col:48 */
    {0x7735,0xEDF7}, /* 眵: GB2312 Code: 0xEDF7 ==> Row:77 Col:87 */
    {0x7736,0xBFF4}, /* 眶: GB2312 Code: 0xBFF4 ==> Row:31 Col:84 */
    {0x7737,0xBEEC}, /* 眷: GB2312 Code: 0xBEEC ==> Row:30 Col:76 */
    {0x7738,0xEDF8}, /* 眸: GB2312 Code: 0xEDF8 ==> Row:77 Col:88 */
    {0x773A,0xCCF7}, /* 眺: GB2312 Code: 0xCCF7 ==> Row:44 Col:87 */
    {0x773C,0xD1DB}, /* 眼: GB2312 Code: 0xD1DB ==> Row:49 Col:59 */
    {0x7740,0xD7C5}, /* 着: GB2312 Code: 0xD7C5 ==> Row:55 Col:37 */
    {0x7741,0xD5F6}, /* 睁: GB2312 Code: 0xD5F6 ==> Row:53 Col:86 */
    {0x7743,0xEDFC}, /* 睃: GB2312 Code: 0xEDFC ==> Row:77 Col:92 */
    {0x7747,0xEDFB}, /* 睇: GB2312 Code: 0xEDFB ==> Row:77 Col:91 */
    {0x7750,0xEDF9}, /* 睐: GB2312 Code: 0xEDF9 ==> Row:77 Col:89 */
    {0x7751,0xEDFA}, /* 睑: GB2312 Code: 0xEDFA ==> Row:77 Col:90 */
    {0x775A,0xEDFD}, /* 睚: GB2312 Code: 0xEDFD ==> Row:77 Col:93 */
    {0x775B,0xBEA6}, /* 睛: GB2312 Code: 0xBEA6 ==> Row:30 Col:06 */
    {0x7761,0xCBAF}, /* 睡: GB2312 Code: 0xCBAF ==> Row:43 Col:15 */
    {0x7762,0xEEA1}, /* 睢: GB2312 Code: 0xEEA1 ==> Row:78 Col:01 */
    {0x7763,0xB6BD}, /* 督: GB2312 Code: 0xB6BD ==> Row:22 Col:29 */
    {0x7765,0xEEA2}, /* 睥: GB2312 Code: 0xEEA2 ==> Row:78 Col:02 */
    {0x7766,0xC4C0}, /* 睦: GB2312 Code: 0xC4C0 ==> Row:36 Col:32 */
    {0x7768,0xEDFE}, /* 睨: GB2312 Code: 0xEDFE ==> Row:77 Col:94 */
    {0x776B,0xBDDE}, /* 睫: GB2312 Code: 0xBDDE ==> Row:29 Col:62 */
    {0x776C,0xB2C7}, /* 睬: GB2312 Code: 0xB2C7 ==> Row:18 Col:39 */
    {0x7779,0xB6C3}, /* 睹: GB2312 Code: 0xB6C3 ==> Row:22 Col:35 */
    {0x777D,0xEEA5}, /* 睽: GB2312 Code: 0xEEA5 ==> Row:78 Col:05 */
    {0x777E,0xD8BA}, /* 睾: GB2312 Code: 0xD8BA ==> Row:56 Col:26 */
    {0x777F,0xEEA3}, /* 睿: GB2312 Code: 0xEEA3 ==> Row:78 Col:03 */
    {0x7780,0xEEA6}, /* 瞀: GB2312 Code: 0xEEA6 ==> Row:78 Col:06 */
    {0x7784,0xC3E9}, /* 瞄: GB2312 Code: 0xC3E9 ==> Row:35 Col:73 */
    {0x7785,0xB3F2}, /* 瞅: GB2312 Code: 0xB3F2 ==> Row:19 Col:82 */
    {0x778C,0xEEA7}, /* 瞌: GB2312 Code: 0xEEA7 ==> Row:78 Col:07 */
    {0x778D,0xEEA4}, /* 瞍: GB2312 Code: 0xEEA4 ==> Row:78 Col:04 */
    {0x778E,0xCFB9}, /* 瞎: GB2312 Code: 0xCFB9 ==> Row:47 Col:25 */
    {0x7791,0xEEA8}, /* 瞑: GB2312 Code: 0xEEA8 ==> Row:78 Col:08 */
    {0x7792,0xC2F7}, /* 瞒: GB2312 Code: 0xC2F7 ==> Row:34 Col:87 */
    {0x779F,0xEEA9}, /* 瞟: GB2312 Code: 0xEEA9 ==> Row:78 Col:09 */
    {0x77A0,0xEEAA}, /* 瞠: GB2312 Code: 0xEEAA ==> Row:78 Col:10 */
    {0x77A2,0xDEAB}, /* 瞢: GB2312 Code: 0xDEAB ==> Row:62 Col:11 */
    {0x77A5,0xC6B3}, /* 瞥: GB2312 Code: 0xC6B3 ==> Row:38 Col:19 */
    {0x77A7,0xC7C6}, /* 瞧: GB2312 Code: 0xC7C6 ==> Row:39 Col:38 */
    {0x77A9,0xD6F5}, /* 瞩: GB2312 Code: 0xD6F5 ==> Row:54 Col:85 */
    {0x77AA,0xB5C9}, /* 瞪: GB2312 Code: 0xB5C9 ==> Row:21 Col:41 */
    {0x77AC,0xCBB2}, /* 瞬: GB2312 Code: 0xCBB2 ==> Row:43 Col:18 */
    {0x77B0,0xEEAB}, /* 瞰: GB2312 Code: 0xEEAB ==> Row:78 Col:11 */
    {0x77B3,0xCDAB}, /* 瞳: GB2312 Code: 0xCDAB ==> Row:45 Col:11 */
    {0x77B5,0xEEAC}, /* 瞵: GB2312 Code: 0xEEAC ==> Row:78 Col:12 */
    {0x77BB,0xD5B0}, /* 瞻: GB2312 Code: 0xD5B0 ==> Row:53 Col:16 */
    {0x77BD,0xEEAD}, /* 瞽: GB2312 Code: 0xEEAD ==> Row:78 Col:13 */
    {0x77BF,0xF6C4}, /* 瞿: GB2312 Code: 0xF6C4 ==> Row:86 Col:36 */
    {0x77CD,0xDBC7}, /* 矍: GB2312 Code: 0xDBC7 ==> Row:59 Col:39 */
    {0x77D7,0xB4A3}, /* 矗: GB2312 Code: 0xB4A3 ==> Row:20 Col:03 */
    {0x77DB,0xC3AC}, /* 矛: GB2312 Code: 0xC3AC ==> Row:35 Col:12 */
    {0x77DC,0xF1E6}, /* 矜: GB2312 Code: 0xF1E6 ==> Row:81 Col:70 */
    {0x77E2,0xCAB8}, /* 矢: GB2312 Code: 0xCAB8 ==> Row:42 Col:24 */
    {0x77E3,0xD2D3}, /* 矣: GB2312 Code: 0xD2D3 ==> Row:50 Col:51 */
    {0x77E5,0xD6AA}, /* 知: GB2312 Code: 0xD6AA ==> Row:54 Col:10 */
    {0x77E7,0xEFF2}, /* 矧: GB2312 Code: 0xEFF2 ==> Row:79 Col:82 */
    {0x77E9,0xBED8}, /* 矩: GB2312 Code: 0xBED8 ==> Row:30 Col:56 */
    {0x77EB,0xBDC3}, /* 矫: GB2312 Code: 0xBDC3 ==> Row:29 Col:35 */
    {0x77EC,0xEFF3}, /* 矬: GB2312 Code: 0xEFF3 ==> Row:79 Col:83 */
    {0x77ED,0xB6CC}, /* 短: GB2312 Code: 0xB6CC ==> Row:22 Col:44 */
    {0x77EE,0xB0AB}, /* 矮: GB2312 Code: 0xB0AB ==> Row:16 Col:11 */
    {0x77F3,0xCAAF}, /* 石: GB2312 Code: 0xCAAF ==> Row:42 Col:15 */
    {0x77F6,0xEDB6}, /* 矶: GB2312 Code: 0xEDB6 ==> Row:77 Col:22 */
    {0x77F8,0xEDB7}, /* 矸: GB2312 Code: 0xEDB7 ==> Row:77 Col:23 */
    {0x77FD,0xCEF9}, /* 矽: GB2312 Code: 0xCEF9 ==> Row:46 Col:89 */
    {0x77FE,0xB7AF}, /* 矾: GB2312 Code: 0xB7AF ==> Row:23 Col:15 */
    {0x77FF,0xBFF3}, /* 矿: GB2312 Code: 0xBFF3 ==> Row:31 Col:83 */
    {0x7800,0xEDB8}, /* 砀: GB2312 Code: 0xEDB8 ==> Row:77 Col:24 */
    {0x7801,0xC2EB}, /* 码: GB2312 Code: 0xC2EB ==> Row:34 Col:75 */
    {0x7802,0xC9B0}, /* 砂: GB2312 Code: 0xC9B0 ==> Row:41 Col:16 */
    {0x7809,0xEDB9}, /* 砉: GB2312 Code: 0xEDB9 ==> Row:77 Col:25 */
    {0x780C,0xC6F6}, /* 砌: GB2312 Code: 0xC6F6 ==> Row:38 Col:86 */
    {0x780D,0xBFB3}, /* 砍: GB2312 Code: 0xBFB3 ==> Row:31 Col:19 */
    {0x7811,0xEDBC}, /* 砑: GB2312 Code: 0xEDBC ==> Row:77 Col:28 */
    {0x7812,0xC5F8}, /* 砒: GB2312 Code: 0xC5F8 ==> Row:37 Col:88 */
    {0x7814,0xD1D0}, /* 研: GB2312 Code: 0xD1D0 ==> Row:49 Col:48 */
    {0x7816,0xD7A9}, /* 砖: GB2312 Code: 0xD7A9 ==> Row:55 Col:09 */
    {0x7817,0xEDBA}, /* 砗: GB2312 Code: 0xEDBA ==> Row:77 Col:26 */
    {0x7818,0xEDBB}, /* 砘: GB2312 Code: 0xEDBB ==> Row:77 Col:27 */
    {0x781A,0xD1E2}, /* 砚: GB2312 Code: 0xD1E2 ==> Row:49 Col:66 */
    {0x781C,0xEDBF}, /* 砜: GB2312 Code: 0xEDBF ==> Row:77 Col:31 */
    {0x781D,0xEDC0}, /* 砝: GB2312 Code: 0xEDC0 ==> Row:77 Col:32 */
    {0x781F,0xEDC4}, /* 砟: GB2312 Code: 0xEDC4 ==> Row:77 Col:36 */
    {0x7823,0xEDC8}, /* 砣: GB2312 Code: 0xEDC8 ==> Row:77 Col:40 */
    {0x7825,0xEDC6}, /* 砥: GB2312 Code: 0xEDC6 ==> Row:77 Col:38 */
    {0x7826,0xEDCE}, /* 砦: GB2312 Code: 0xEDCE ==> Row:77 Col:46 */
    {0x7827,0xD5E8}, /* 砧: GB2312 Code: 0xD5E8 ==> Row:53 Col:72 */
    {0x7829,0xEDC9}, /* 砩: GB2312 Code: 0xEDC9 ==> Row:77 Col:41 */
    {0x782C,0xEDC7}, /* 砬: GB2312 Code: 0xEDC7 ==> Row:77 Col:39 */
    {0x782D,0xEDBE}, /* 砭: GB2312 Code: 0xEDBE ==> Row:77 Col:30 */
    {0x7830,0xC5E9}, /* 砰: GB2312 Code: 0xC5E9 ==> Row:37 Col:73 */
    {0x7834,0xC6C6}, /* 破: GB2312 Code: 0xC6C6 ==> Row:38 Col:38 */
    {0x7837,0xC9E9}, /* 砷: GB2312 Code: 0xC9E9 ==> Row:41 Col:73 */
    {0x7838,0xD4D2}, /* 砸: GB2312 Code: 0xD4D2 ==> Row:52 Col:50 */
    {0x7839,0xEDC1}, /* 砹: GB2312 Code: 0xEDC1 ==> Row:77 Col:33 */
    {0x783A,0xEDC2}, /* 砺: GB2312 Code: 0xEDC2 ==> Row:77 Col:34 */
    {0x783B,0xEDC3}, /* 砻: GB2312 Code: 0xEDC3 ==> Row:77 Col:35 */
    {0x783C,0xEDC5}, /* 砼: GB2312 Code: 0xEDC5 ==> Row:77 Col:37 */
    {0x783E,0xC0F9}, /* 砾: GB2312 Code: 0xC0F9 ==> Row:32 Col:89 */
    {0x7840,0xB4A1}, /* 础: GB2312 Code: 0xB4A1 ==> Row:20 Col:01 */
    {0x7845,0xB9E8}, /* 硅: GB2312 Code: 0xB9E8 ==> Row:25 Col:72 */
    {0x7847,0xEDD0}, /* 硇: GB2312 Code: 0xEDD0 ==> Row:77 Col:48 */
    {0x784C,0xEDD1}, /* 硌: GB2312 Code: 0xEDD1 ==> Row:77 Col:49 */
    {0x784E,0xEDCA}, /* 硎: GB2312 Code: 0xEDCA ==> Row:77 Col:42 */
    {0x7850,0xEDCF}, /* 硐: GB2312 Code: 0xEDCF ==> Row:77 Col:47 */
    {0x7852,0xCEF8}, /* 硒: GB2312 Code: 0xCEF8 ==> Row:46 Col:88 */
    {0x7855,0xCBB6}, /* 硕: GB2312 Code: 0xCBB6 ==> Row:43 Col:22 */
    {0x7856,0xEDCC}, /* 硖: GB2312 Code: 0xEDCC ==> Row:77 Col:44 */
    {0x7857,0xEDCD}, /* 硗: GB2312 Code: 0xEDCD ==> Row:77 Col:45 */
    {0x785D,0xCFF5}, /* 硝: GB2312 Code: 0xCFF5 ==> Row:47 Col:85 */
    {0x786A,0xEDD2}, /* 硪: GB2312 Code: 0xEDD2 ==> Row:77 Col:50 */
    {0x786B,0xC1F2}, /* 硫: GB2312 Code: 0xC1F2 ==> Row:33 Col:82 */
    {0x786C,0xD3B2}, /* 硬: GB2312 Code: 0xD3B2 ==> Row:51 Col:18 */
    {0x786D,0xEDCB}, /* 硭: GB2312 Code: 0xEDCB ==> Row:77 Col:43 */
    {0x786E,0xC8B7}, /* 确: GB2312 Code: 0xC8B7 ==> Row:40 Col:23 */
    {0x7877,0xBCEF}, /* 硷: GB2312 Code: 0xBCEF ==> Row:28 Col:79 */
    {0x787C,0xC5F0}, /* 硼: GB2312 Code: 0xC5F0 ==> Row:37 Col:80 */
    {0x7887,0xEDD6}, /* 碇: GB2312 Code: 0xEDD6 ==> Row:77 Col:54 */
    {0x7889,0xB5EF}, /* 碉: GB2312 Code: 0xB5EF ==> Row:21 Col:79 */
    {0x788C,0xC2B5}, /* 碌: GB2312 Code: 0xC2B5 ==> Row:34 Col:21 */
    {0x788D,0xB0AD}, /* 碍: GB2312 Code: 0xB0AD ==> Row:16 Col:13 */
    {0x788E,0xCBE9}, /* 碎: GB2312 Code: 0xCBE9 ==> Row:43 Col:73 */
    {0x7891,0xB1AE}, /* 碑: GB2312 Code: 0xB1AE ==> Row:17 Col:14 */
    {0x7893,0xEDD4}, /* 碓: GB2312 Code: 0xEDD4 ==> Row:77 Col:52 */
    {0x7897,0xCDEB}, /* 碗: GB2312 Code: 0xCDEB ==> Row:45 Col:75 */
    {0x7898,0xB5E2}, /* 碘: GB2312 Code: 0xB5E2 ==> Row:21 Col:66 */
    {0x789A,0xEDD5}, /* 碚: GB2312 Code: 0xEDD5 ==> Row:77 Col:53 */
    {0x789B,0xEDD3}, /* 碛: GB2312 Code: 0xEDD3 ==> Row:77 Col:51 */
    {0x789C,0xEDD7}, /* 碜: GB2312 Code: 0xEDD7 ==> Row:77 Col:55 */
    {0x789F,0xB5FA}, /* 碟: GB2312 Code: 0xB5FA ==> Row:21 Col:90 */
    {0x78A1,0xEDD8}, /* 碡: GB2312 Code: 0xEDD8 ==> Row:77 Col:56 */
    {0x78A3,0xEDD9}, /* 碣: GB2312 Code: 0xEDD9 ==> Row:77 Col:57 */
    {0x78A5,0xEDDC}, /* 碥: GB2312 Code: 0xEDDC ==> Row:77 Col:60 */
    {0x78A7,0xB1CC}, /* 碧: GB2312 Code: 0xB1CC ==> Row:17 Col:44 */
    {0x78B0,0xC5F6}, /* 碰: GB2312 Code: 0xC5F6 ==> Row:37 Col:86 */
    {0x78B1,0xBCEE}, /* 碱: GB2312 Code: 0xBCEE ==> Row:28 Col:78 */
    {0x78B2,0xEDDA}, /* 碲: GB2312 Code: 0xEDDA ==> Row:77 Col:58 */
    {0x78B3,0xCCBC}, /* 碳: GB2312 Code: 0xCCBC ==> Row:44 Col:28 */
    {0x78B4,0xB2EA}, /* 碴: GB2312 Code: 0xB2EA ==> Row:18 Col:74 */
    {0x78B9,0xEDDB}, /* 碹: GB2312 Code: 0xEDDB ==> Row:77 Col:59 */
    {0x78BE,0xC4EB}, /* 碾: GB2312 Code: 0xC4EB ==> Row:36 Col:75 */
    {0x78C1,0xB4C5}, /* 磁: GB2312 Code: 0xB4C5 ==> Row:20 Col:37 */
    {0x78C5,0xB0F5}, /* 磅: GB2312 Code: 0xB0F5 ==> Row:16 Col:85 */
    {0x78C9,0xEDDF}, /* 磉: GB2312 Code: 0xEDDF ==> Row:77 Col:63 */
    {0x78CA,0xC0DA}, /* 磊: GB2312 Code: 0xC0DA ==> Row:32 Col:58 */
    {0x78CB,0xB4E8}, /* 磋: GB2312 Code: 0xB4E8 ==> Row:20 Col:72 */
    {0x78D0,0xC5CD}, /* 磐: GB2312 Code: 0xC5CD ==> Row:37 Col:45 */
    {0x78D4,0xEDDD}, /* 磔: GB2312 Code: 0xEDDD ==> Row:77 Col:61 */
    {0x78D5,0xBFC4}, /* 磕: GB2312 Code: 0xBFC4 ==> Row:31 Col:36 */
    {0x78D9,0xEDDE}, /* 磙: GB2312 Code: 0xEDDE ==> Row:77 Col:62 */
    {0x78E8,0xC4A5}, /* 磨: GB2312 Code: 0xC4A5 ==> Row:36 Col:05 */
    {0x78EC,0xEDE0}, /* 磬: GB2312 Code: 0xEDE0 ==> Row:77 Col:64 */
    {0x78F2,0xEDE1}, /* 磲: GB2312 Code: 0xEDE1 ==> Row:77 Col:65 */
    {0x78F4,0xEDE3}, /* 磴: GB2312 Code: 0xEDE3 ==> Row:77 Col:67 */
    {0x78F7,0xC1D7}, /* 磷: GB2312 Code: 0xC1D7 ==> Row:33 Col:55 */
    {0x78FA,0xBBC7}, /* 磺: GB2312 Code: 0xBBC7 ==> Row:27 Col:39 */
    {0x7901,0xBDB8}, /* 礁: GB2312 Code: 0xBDB8 ==> Row:29 Col:24 */
    {0x7905,0xEDE2}, /* 礅: GB2312 Code: 0xEDE2 ==> Row:77 Col:66 */
    {0x7913,0xEDE4}, /* 礓: GB2312 Code: 0xEDE4 ==> Row:77 Col:68 */
    {0x791E,0xEDE6}, /* 礞: GB2312 Code: 0xEDE6 ==> Row:77 Col:70 */
    {0x7924,0xEDE5}, /* 礤: GB2312 Code: 0xEDE5 ==> Row:77 Col:69 */
    {0x7934,0xEDE7}, /* 礴: GB2312 Code: 0xEDE7 ==> Row:77 Col:71 */
    {0x793A,0xCABE}, /* 示: GB2312 Code: 0xCABE ==> Row:42 Col:30 */
    {0x793B,0xECEA}, /* 礻: GB2312 Code: 0xECEA ==> Row:76 Col:74 */
    {0x793C,0xC0F1}, /* 礼: GB2312 Code: 0xC0F1 ==> Row:32 Col:81 */
    {0x793E,0xC9E7}, /* 社: GB2312 Code: 0xC9E7 ==> Row:41 Col:71 */
    {0x7940,0xECEB}, /* 祀: GB2312 Code: 0xECEB ==> Row:76 Col:75 */
    {0x7941,0xC6EE}, /* 祁: GB2312 Code: 0xC6EE ==> Row:38 Col:78 */
    {0x7946,0xECEC}, /* 祆: GB2312 Code: 0xECEC ==> Row:76 Col:76 */
    {0x7948,0xC6ED}, /* 祈: GB2312 Code: 0xC6ED ==> Row:38 Col:77 */
    {0x7949,0xECED}, /* 祉: GB2312 Code: 0xECED ==> Row:76 Col:77 */
    {0x7953,0xECF0}, /* 祓: GB2312 Code: 0xECF0 ==> Row:76 Col:80 */
    {0x7956,0xD7E6}, /* 祖: GB2312 Code: 0xD7E6 ==> Row:55 Col:70 */
    {0x7957,0xECF3}, /* 祗: GB2312 Code: 0xECF3 ==> Row:76 Col:83 */
    {0x795A,0xECF1}, /* 祚: GB2312 Code: 0xECF1 ==> Row:76 Col:81 */
    {0x795B,0xECEE}, /* 祛: GB2312 Code: 0xECEE ==> Row:76 Col:78 */
    {0x795C,0xECEF}, /* 祜: GB2312 Code: 0xECEF ==> Row:76 Col:79 */
    {0x795D,0xD7A3}, /* 祝: GB2312 Code: 0xD7A3 ==> Row:55 Col:03 */
    {0x795E,0xC9F1}, /* 神: GB2312 Code: 0xC9F1 ==> Row:41 Col:81 */
    {0x795F,0xCBEE}, /* 祟: GB2312 Code: 0xCBEE ==> Row:43 Col:78 */
    {0x7960,0xECF4}, /* 祠: GB2312 Code: 0xECF4 ==> Row:76 Col:84 */
    {0x7962,0xECF2}, /* 祢: GB2312 Code: 0xECF2 ==> Row:76 Col:82 */
    {0x7965,0xCFE9}, /* 祥: GB2312 Code: 0xCFE9 ==> Row:47 Col:73 */
    {0x7967,0xECF6}, /* 祧: GB2312 Code: 0xECF6 ==> Row:76 Col:86 */
    {0x7968,0xC6B1}, /* 票: GB2312 Code: 0xC6B1 ==> Row:38 Col:17 */
    {0x796D,0xBCC0}, /* 祭: GB2312 Code: 0xBCC0 ==> Row:28 Col:32 */
    {0x796F,0xECF5}, /* 祯: GB2312 Code: 0xECF5 ==> Row:76 Col:85 */
    {0x7977,0xB5BB}, /* 祷: GB2312 Code: 0xB5BB ==> Row:21 Col:27 */
    {0x7978,0xBBF6}, /* 祸: GB2312 Code: 0xBBF6 ==> Row:27 Col:86 */
    {0x797A,0xECF7}, /* 祺: GB2312 Code: 0xECF7 ==> Row:76 Col:87 */
    {0x7980,0xD9F7}, /* 禀: GB2312 Code: 0xD9F7 ==> Row:57 Col:87 */
    {0x7981,0xBDFB}, /* 禁: GB2312 Code: 0xBDFB ==> Row:29 Col:91 */
    {0x7984,0xC2BB}, /* 禄: GB2312 Code: 0xC2BB ==> Row:34 Col:27 */
    {0x7985,0xECF8}, /* 禅: GB2312 Code: 0xECF8 ==> Row:76 Col:88 */
    {0x798A,0xECF9}, /* 禊: GB2312 Code: 0xECF9 ==> Row:76 Col:89 */
    {0x798F,0xB8A3}, /* 福: GB2312 Code: 0xB8A3 ==> Row:24 Col:03 */
    {0x799A,0xECFA}, /* 禚: GB2312 Code: 0xECFA ==> Row:76 Col:90 */
    {0x79A7,0xECFB}, /* 禧: GB2312 Code: 0xECFB ==> Row:76 Col:91 */
    {0x79B3,0xECFC}, /* 禳: GB2312 Code: 0xECFC ==> Row:76 Col:92 */
    {0x79B9,0xD3ED}, /* 禹: GB2312 Code: 0xD3ED ==> Row:51 Col:77 */
    {0x79BA,0xD8AE}, /* 禺: GB2312 Code: 0xD8AE ==> Row:56 Col:14 */
    {0x79BB,0xC0EB}, /* 离: GB2312 Code: 0xC0EB ==> Row:32 Col:75 */
    {0x79BD,0xC7DD}, /* 禽: GB2312 Code: 0xC7DD ==> Row:39 Col:61 */
    {0x79BE,0xBACC}, /* 禾: GB2312 Code: 0xBACC ==> Row:26 Col:44 */
    {0x79C0,0xD0E3}, /* 秀: GB2312 Code: 0xD0E3 ==> Row:48 Col:67 */
    {0x79C1,0xCBBD}, /* 私: GB2312 Code: 0xCBBD ==> Row:43 Col:29 */
    {0x79C3,0xCDBA}, /* 秃: GB2312 Code: 0xCDBA ==> Row:45 Col:26 */
    {0x79C6,0xB8D1}, /* 秆: GB2312 Code: 0xB8D1 ==> Row:24 Col:49 */
    {0x79C9,0xB1FC}, /* 秉: GB2312 Code: 0xB1FC ==> Row:17 Col:92 */
    {0x79CB,0xC7EF}, /* 秋: GB2312 Code: 0xC7EF ==> Row:39 Col:79 */
    {0x79CD,0xD6D6}, /* 种: GB2312 Code: 0xD6D6 ==> Row:54 Col:54 */
    {0x79D1,0xBFC6}, /* 科: GB2312 Code: 0xBFC6 ==> Row:31 Col:38 */
    {0x79D2,0xC3EB}, /* 秒: GB2312 Code: 0xC3EB ==> Row:35 Col:75 */
    {0x79D5,0xEFF5}, /* 秕: GB2312 Code: 0xEFF5 ==> Row:79 Col:85 */
    {0x79D8,0xC3D8}, /* 秘: GB2312 Code: 0xC3D8 ==> Row:35 Col:56 */
    {0x79DF,0xD7E2}, /* 租: GB2312 Code: 0xD7E2 ==> Row:55 Col:66 */
    {0x79E3,0xEFF7}, /* 秣: GB2312 Code: 0xEFF7 ==> Row:79 Col:87 */
    {0x79E4,0xB3D3}, /* 秤: GB2312 Code: 0xB3D3 ==> Row:19 Col:51 */
    {0x79E6,0xC7D8}, /* 秦: GB2312 Code: 0xC7D8 ==> Row:39 Col:56 */
    {0x79E7,0xD1ED}, /* 秧: GB2312 Code: 0xD1ED ==> Row:49 Col:77 */
    {0x79E9,0xD6C8}, /* 秩: GB2312 Code: 0xD6C8 ==> Row:54 Col:40 */
    {0x79EB,0xEFF8}, /* 秫: GB2312 Code: 0xEFF8 ==> Row:79 Col:88 */
    {0x79ED,0xEFF6}, /* 秭: GB2312 Code: 0xEFF6 ==> Row:79 Col:86 */
    {0x79EF,0xBBFD}, /* 积: GB2312 Code: 0xBBFD ==> Row:27 Col:93 */
    {0x79F0,0xB3C6}, /* 称: GB2312 Code: 0xB3C6 ==> Row:19 Col:38 */
    {0x79F8,0xBDD5}, /* 秸: GB2312 Code: 0xBDD5 ==> Row:29 Col:53 */
    {0x79FB,0xD2C6}, /* 移: GB2312 Code: 0xD2C6 ==> Row:50 Col:38 */
    {0x79FD,0xBBE0}, /* 秽: GB2312 Code: 0xBBE0 ==> Row:27 Col:64 */
    {0x7A00,0xCFA1}, /* 稀: GB2312 Code: 0xCFA1 ==> Row:47 Col:01 */
    {0x7A02,0xEFFC}, /* 稂: GB2312 Code: 0xEFFC ==> Row:79 Col:92 */
    {0x7A03,0xEFFB}, /* 稃: GB2312 Code: 0xEFFB ==> Row:79 Col:91 */
    {0x7A06,0xEFF9}, /* 稆: GB2312 Code: 0xEFF9 ==> Row:79 Col:89 */
    {0x7A0B,0xB3CC}, /* 程: GB2312 Code: 0xB3CC ==> Row:19 Col:44 */
    {0x7A0D,0xC9D4}, /* 稍: GB2312 Code: 0xC9D4 ==> Row:41 Col:52 */
    {0x7A0E,0xCBB0}, /* 税: GB2312 Code: 0xCBB0 ==> Row:43 Col:16 */
    {0x7A14,0xEFFE}, /* 稔: GB2312 Code: 0xEFFE ==> Row:79 Col:94 */
    {0x7A17,0xB0DE}, /* 稗: GB2312 Code: 0xB0DE ==> Row:16 Col:62 */
    {0x7A1A,0xD6C9}, /* 稚: GB2312 Code: 0xD6C9 ==> Row:54 Col:41 */
    {0x7A1E,0xEFFD}, /* 稞: GB2312 Code: 0xEFFD ==> Row:79 Col:93 */
    {0x7A20,0xB3ED}, /* 稠: GB2312 Code: 0xB3ED ==> Row:19 Col:77 */
    {0x7A23,0xF6D5}, /* 稣: GB2312 Code: 0xF6D5 ==> Row:86 Col:53 */
    {0x7A33,0xCEC8}, /* 稳: GB2312 Code: 0xCEC8 ==> Row:46 Col:40 */
    {0x7A37,0xF0A2}, /* 稷: GB2312 Code: 0xF0A2 ==> Row:80 Col:02 */
    {0x7A39,0xF0A1}, /* 稹: GB2312 Code: 0xF0A1 ==> Row:80 Col:01 */
    {0x7A3B,0xB5BE}, /* 稻: GB2312 Code: 0xB5BE ==> Row:21 Col:30 */
    {0x7A3C,0xBCDA}, /* 稼: GB2312 Code: 0xBCDA ==> Row:28 Col:58 */
    {0x7A3D,0xBBFC}, /* 稽: GB2312 Code: 0xBBFC ==> Row:27 Col:92 */
    {0x7A3F,0xB8E5}, /* 稿: GB2312 Code: 0xB8E5 ==> Row:24 Col:69 */
    {0x7A46,0xC4C2}, /* 穆: GB2312 Code: 0xC4C2 ==> Row:36 Col:34 */
    {0x7A51,0xF0A3}, /* 穑: GB2312 Code: 0xF0A3 ==> Row:80 Col:03 */
    {0x7A57,0xCBEB}, /* 穗: GB2312 Code: 0xCBEB ==> Row:43 Col:75 */
    {0x7A70,0xF0A6}, /* 穰: GB2312 Code: 0xF0A6 ==> Row:80 Col:06 */
    {0x7A74,0xD1A8}, /* 穴: GB2312 Code: 0xD1A8 ==> Row:49 Col:08 */
    {0x7A76,0xBEBF}, /* 究: GB2312 Code: 0xBEBF ==> Row:30 Col:31 */
    {0x7A77,0xC7EE}, /* 穷: GB2312 Code: 0xC7EE ==> Row:39 Col:78 */
    {0x7A78,0xF1B6}, /* 穸: GB2312 Code: 0xF1B6 ==> Row:81 Col:22 */
    {0x7A79,0xF1B7}, /* 穹: GB2312 Code: 0xF1B7 ==> Row:81 Col:23 */
    {0x7A7A,0xBFD5}, /* 空: GB2312 Code: 0xBFD5 ==> Row:31 Col:53 */
    {0x7A7F,0xB4A9}, /* 穿: GB2312 Code: 0xB4A9 ==> Row:20 Col:09 */
    {0x7A80,0xF1B8}, /* 窀: GB2312 Code: 0xF1B8 ==> Row:81 Col:24 */
    {0x7A81,0xCDBB}, /* 突: GB2312 Code: 0xCDBB ==> Row:45 Col:27 */
    {0x7A83,0xC7D4}, /* 窃: GB2312 Code: 0xC7D4 ==> Row:39 Col:52 */
    {0x7A84,0xD5AD}, /* 窄: GB2312 Code: 0xD5AD ==> Row:53 Col:13 */
    {0x7A86,0xF1B9}, /* 窆: GB2312 Code: 0xF1B9 ==> Row:81 Col:25 */
    {0x7A88,0xF1BA}, /* 窈: GB2312 Code: 0xF1BA ==> Row:81 Col:26 */
    {0x7A8D,0xC7CF}, /* 窍: GB2312 Code: 0xC7CF ==> Row:39 Col:47 */
    {0x7A91,0xD2A4}, /* 窑: GB2312 Code: 0xD2A4 ==> Row:50 Col:04 */
    {0x7A92,0xD6CF}, /* 窒: GB2312 Code: 0xD6CF ==> Row:54 Col:47 */
    {0x7A95,0xF1BB}, /* 窕: GB2312 Code: 0xF1BB ==> Row:81 Col:27 */
    {0x7A96,0xBDD1}, /* 窖: GB2312 Code: 0xBDD1 ==> Row:29 Col:49 */
    {0x7A97,0xB4B0}, /* 窗: GB2312 Code: 0xB4B0 ==> Row:20 Col:16 */
    {0x7A98,0xBEBD}, /* 窘: GB2312 Code: 0xBEBD ==> Row:30 Col:29 */
    {0x7A9C,0xB4DC}, /* 窜: GB2312 Code: 0xB4DC ==> Row:20 Col:60 */
    {0x7A9D,0xCED1}, /* 窝: GB2312 Code: 0xCED1 ==> Row:46 Col:49 */
    {0x7A9F,0xBFDF}, /* 窟: GB2312 Code: 0xBFDF ==> Row:31 Col:63 */
    {0x7AA0,0xF1BD}, /* 窠: GB2312 Code: 0xF1BD ==> Row:81 Col:29 */
    {0x7AA5,0xBFFA}, /* 窥: GB2312 Code: 0xBFFA ==> Row:31 Col:90 */
    {0x7AA6,0xF1BC}, /* 窦: GB2312 Code: 0xF1BC ==> Row:81 Col:28 */
    {0x7AA8,0xF1BF}, /* 窨: GB2312 Code: 0xF1BF ==> Row:81 Col:31 */
    {0x7AAC,0xF1BE}, /* 窬: GB2312 Code: 0xF1BE ==> Row:81 Col:30 */
    {0x7AAD,0xF1C0}, /* 窭: GB2312 Code: 0xF1C0 ==> Row:81 Col:32 */
    {0x7AB3,0xF1C1}, /* 窳: GB2312 Code: 0xF1C1 ==> Row:81 Col:33 */
    {0x7ABF,0xC1FE}, /* 窿: GB2312 Code: 0xC1FE ==> Row:33 Col:94 */
    {0x7ACB,0xC1A2}, /* 立: GB2312 Code: 0xC1A2 ==> Row:33 Col:02 */
    {0x7AD6,0xCAFA}, /* 竖: GB2312 Code: 0xCAFA ==> Row:42 Col:90 */
    {0x7AD9,0xD5BE}, /* 站: GB2312 Code: 0xD5BE ==> Row:53 Col:30 */
    {0x7ADE,0xBEBA}, /* 竞: GB2312 Code: 0xBEBA ==> Row:30 Col:26 */
    {0x7ADF,0xBEB9}, /* 竟: GB2312 Code: 0xBEB9 ==> Row:30 Col:25 */
    {0x7AE0,0xD5C2}, /* 章: GB2312 Code: 0xD5C2 ==> Row:53 Col:34 */
    {0x7AE3,0xBFA2}, /* 竣: GB2312 Code: 0xBFA2 ==> Row:31 Col:02 */
    {0x7AE5,0xCDAF}, /* 童: GB2312 Code: 0xCDAF ==> Row:45 Col:15 */
    {0x7AE6,0xF1B5}, /* 竦: GB2312 Code: 0xF1B5 ==> Row:81 Col:21 */
    {0x7AED,0xBDDF}, /* 竭: GB2312 Code: 0xBDDF ==> Row:29 Col:63 */
    {0x7AEF,0xB6CB}, /* 端: GB2312 Code: 0xB6CB ==> Row:22 Col:43 */
    {0x7AF9,0xD6F1}, /* 竹: GB2312 Code: 0xD6F1 ==> Row:54 Col:81 */
    {0x7AFA,0xF3C3}, /* 竺: GB2312 Code: 0xF3C3 ==> Row:83 Col:35 */
    {0x7AFD,0xF3C4}, /* 竽: GB2312 Code: 0xF3C4 ==> Row:83 Col:36 */
    {0x7AFF,0xB8CD}, /* 竿: GB2312 Code: 0xB8CD ==> Row:24 Col:45 */
    {0x7B03,0xF3C6}, /* 笃: GB2312 Code: 0xF3C6 ==> Row:83 Col:38 */
    {0x7B04,0xF3C7}, /* 笄: GB2312 Code: 0xF3C7 ==> Row:83 Col:39 */
    {0x7B06,0xB0CA}, /* 笆: GB2312 Code: 0xB0CA ==> Row:16 Col:42 */
    {0x7B08,0xF3C5}, /* 笈: GB2312 Code: 0xF3C5 ==> Row:83 Col:37 */
    {0x7B0A,0xF3C9}, /* 笊: GB2312 Code: 0xF3C9 ==> Row:83 Col:41 */
    {0x7B0B,0xCBF1}, /* 笋: GB2312 Code: 0xCBF1 ==> Row:43 Col:81 */
    {0x7B0F,0xF3CB}, /* 笏: GB2312 Code: 0xF3CB ==> Row:83 Col:43 */
    {0x7B11,0xD0A6}, /* 笑: GB2312 Code: 0xD0A6 ==> Row:48 Col:06 */
    {0x7B14,0xB1CA}, /* 笔: GB2312 Code: 0xB1CA ==> Row:17 Col:42 */
    {0x7B15,0xF3C8}, /* 笕: GB2312 Code: 0xF3C8 ==> Row:83 Col:40 */
    {0x7B19,0xF3CF}, /* 笙: GB2312 Code: 0xF3CF ==> Row:83 Col:47 */
    {0x7B1B,0xB5D1}, /* 笛: GB2312 Code: 0xB5D1 ==> Row:21 Col:49 */
    {0x7B1E,0xF3D7}, /* 笞: GB2312 Code: 0xF3D7 ==> Row:83 Col:55 */
    {0x7B20,0xF3D2}, /* 笠: GB2312 Code: 0xF3D2 ==> Row:83 Col:50 */
    {0x7B24,0xF3D4}, /* 笤: GB2312 Code: 0xF3D4 ==> Row:83 Col:52 */
    {0x7B25,0xF3D3}, /* 笥: GB2312 Code: 0xF3D3 ==> Row:83 Col:51 */
    {0x7B26,0xB7FB}, /* 符: GB2312 Code: 0xB7FB ==> Row:23 Col:91 */
    {0x7B28,0xB1BF}, /* 笨: GB2312 Code: 0xB1BF ==> Row:17 Col:31 */
    {0x7B2A,0xF3CE}, /* 笪: GB2312 Code: 0xF3CE ==> Row:83 Col:46 */
    {0x7B2B,0xF3CA}, /* 笫: GB2312 Code: 0xF3CA ==> Row:83 Col:42 */
    {0x7B2C,0xB5DA}, /* 第: GB2312 Code: 0xB5DA ==> Row:21 Col:58 */
    {0x7B2E,0xF3D0}, /* 笮: GB2312 Code: 0xF3D0 ==> Row:83 Col:48 */
    {0x7B31,0xF3D1}, /* 笱: GB2312 Code: 0xF3D1 ==> Row:83 Col:49 */
    {0x7B33,0xF3D5}, /* 笳: GB2312 Code: 0xF3D5 ==> Row:83 Col:53 */
    {0x7B38,0xF3CD}, /* 笸: GB2312 Code: 0xF3CD ==> Row:83 Col:45 */
    {0x7B3A,0xBCE3}, /* 笺: GB2312 Code: 0xBCE3 ==> Row:28 Col:67 */
    {0x7B3C,0xC1FD}, /* 笼: GB2312 Code: 0xC1FD ==> Row:33 Col:93 */
    {0x7B3E,0xF3D6}, /* 笾: GB2312 Code: 0xF3D6 ==> Row:83 Col:54 */
    {0x7B45,0xF3DA}, /* 筅: GB2312 Code: 0xF3DA ==> Row:83 Col:58 */
    {0x7B47,0xF3CC}, /* 筇: GB2312 Code: 0xF3CC ==> Row:83 Col:44 */
    {0x7B49,0xB5C8}, /* 等: GB2312 Code: 0xB5C8 ==> Row:21 Col:40 */
    {0x7B4B,0xBDEE}, /* 筋: GB2312 Code: 0xBDEE ==> Row:29 Col:78 */
    {0x7B4C,0xF3DC}, /* 筌: GB2312 Code: 0xF3DC ==> Row:83 Col:60 */
    {0x7B4F,0xB7A4}, /* 筏: GB2312 Code: 0xB7A4 ==> Row:23 Col:04 */
    {0x7B50,0xBFF0}, /* 筐: GB2312 Code: 0xBFF0 ==> Row:31 Col:80 */
    {0x7B51,0xD6FE}, /* 筑: GB2312 Code: 0xD6FE ==> Row:54 Col:94 */
    {0x7B52,0xCDB2}, /* 筒: GB2312 Code: 0xCDB2 ==> Row:45 Col:18 */
    {0x7B54,0xB4F0}, /* 答: GB2312 Code: 0xB4F0 ==> Row:20 Col:80 */
    {0x7B56,0xB2DF}, /* 策: GB2312 Code: 0xB2DF ==> Row:18 Col:63 */
    {0x7B58,0xF3D8}, /* 筘: GB2312 Code: 0xF3D8 ==> Row:83 Col:56 */
    {0x7B5A,0xF3D9}, /* 筚: GB2312 Code: 0xF3D9 ==> Row:83 Col:57 */
    {0x7B5B,0xC9B8}, /* 筛: GB2312 Code: 0xC9B8 ==> Row:41 Col:24 */
    {0x7B5D,0xF3DD}, /* 筝: GB2312 Code: 0xF3DD ==> Row:83 Col:61 */
    {0x7B60,0xF3DE}, /* 筠: GB2312 Code: 0xF3DE ==> Row:83 Col:62 */
    {0x7B62,0xF3E1}, /* 筢: GB2312 Code: 0xF3E1 ==> Row:83 Col:65 */
    {0x7B6E,0xF3DF}, /* 筮: GB2312 Code: 0xF3DF ==> Row:83 Col:63 */
    {0x7B71,0xF3E3}, /* 筱: GB2312 Code: 0xF3E3 ==> Row:83 Col:67 */
    {0x7B72,0xF3E2}, /* 筲: GB2312 Code: 0xF3E2 ==> Row:83 Col:66 */
    {0x7B75,0xF3DB}, /* 筵: GB2312 Code: 0xF3DB ==> Row:83 Col:59 */
    {0x7B77,0xBFEA}, /* 筷: GB2312 Code: 0xBFEA ==> Row:31 Col:74 */
    {0x7B79,0xB3EF}, /* 筹: GB2312 Code: 0xB3EF ==> Row:19 Col:79 */
    {0x7B7B,0xF3E0}, /* 筻: GB2312 Code: 0xF3E0 ==> Row:83 Col:64 */
    {0x7B7E,0xC7A9}, /* 签: GB2312 Code: 0xC7A9 ==> Row:39 Col:09 */
    {0x7B80,0xBCF2}, /* 简: GB2312 Code: 0xBCF2 ==> Row:28 Col:82 */
    {0x7B85,0xF3EB}, /* 箅: GB2312 Code: 0xF3EB ==> Row:83 Col:75 */
    {0x7B8D,0xB9BF}, /* 箍: GB2312 Code: 0xB9BF ==> Row:25 Col:31 */
    {0x7B90,0xF3E4}, /* 箐: GB2312 Code: 0xF3E4 ==> Row:83 Col:68 */
    {0x7B94,0xB2AD}, /* 箔: GB2312 Code: 0xB2AD ==> Row:18 Col:13 */
    {0x7B95,0xBBFE}, /* 箕: GB2312 Code: 0xBBFE ==> Row:27 Col:94 */
    {0x7B97,0xCBE3}, /* 算: GB2312 Code: 0xCBE3 ==> Row:43 Col:67 */
    {0x7B9C,0xF3ED}, /* 箜: GB2312 Code: 0xF3ED ==> Row:83 Col:77 */
    {0x7B9D,0xF3E9}, /* 箝: GB2312 Code: 0xF3E9 ==> Row:83 Col:73 */
    {0x7BA1,0xB9DC}, /* 管: GB2312 Code: 0xB9DC ==> Row:25 Col:60 */
    {0x7BA2,0xF3EE}, /* 箢: GB2312 Code: 0xF3EE ==> Row:83 Col:78 */
    {0x7BA6,0xF3E5}, /* 箦: GB2312 Code: 0xF3E5 ==> Row:83 Col:69 */
    {0x7BA7,0xF3E6}, /* 箧: GB2312 Code: 0xF3E6 ==> Row:83 Col:70 */
    {0x7BA8,0xF3EA}, /* 箨: GB2312 Code: 0xF3EA ==> Row:83 Col:74 */
    {0x7BA9,0xC2E1}, /* 箩: GB2312 Code: 0xC2E1 ==> Row:34 Col:65 */
    {0x7BAA,0xF3EC}, /* 箪: GB2312 Code: 0xF3EC ==> Row:83 Col:76 */
    {0x7BAB,0xF3EF}, /* 箫: GB2312 Code: 0xF3EF ==> Row:83 Col:79 */
    {0x7BAC,0xF3E8}, /* 箬: GB2312 Code: 0xF3E8 ==> Row:83 Col:72 */
    {0x7BAD,0xBCFD}, /* 箭: GB2312 Code: 0xBCFD ==> Row:28 Col:93 */
    {0x7BB1,0xCFE4}, /* 箱: GB2312 Code: 0xCFE4 ==> Row:47 Col:68 */
    {0x7BB4,0xF3F0}, /* 箴: GB2312 Code: 0xF3F0 ==> Row:83 Col:80 */
    {0x7BB8,0xF3E7}, /* 箸: GB2312 Code: 0xF3E7 ==> Row:83 Col:71 */
    {0x7BC1,0xF3F2}, /* 篁: GB2312 Code: 0xF3F2 ==> Row:83 Col:82 */
    {0x7BC6,0xD7AD}, /* 篆: GB2312 Code: 0xD7AD ==> Row:55 Col:13 */
    {0x7BC7,0xC6AA}, /* 篇: GB2312 Code: 0xC6AA ==> Row:38 Col:10 */
    {0x7BCC,0xF3F3}, /* 篌: GB2312 Code: 0xF3F3 ==> Row:83 Col:83 */
    {0x7BD1,0xF3F1}, /* 篑: GB2312 Code: 0xF3F1 ==> Row:83 Col:81 */
    {0x7BD3,0xC2A8}, /* 篓: GB2312 Code: 0xC2A8 ==> Row:34 Col:08 */
    {0x7BD9,0xB8DD}, /* 篙: GB2312 Code: 0xB8DD ==> Row:24 Col:61 */
    {0x7BDA,0xF3F5}, /* 篚: GB2312 Code: 0xF3F5 ==> Row:83 Col:85 */
    {0x7BDD,0xF3F4}, /* 篝: GB2312 Code: 0xF3F4 ==> Row:83 Col:84 */
    {0x7BE1,0xB4DB}, /* 篡: GB2312 Code: 0xB4DB ==> Row:20 Col:59 */
    {0x7BE5,0xF3F6}, /* 篥: GB2312 Code: 0xF3F6 ==> Row:83 Col:86 */
    {0x7BE6,0xF3F7}, /* 篦: GB2312 Code: 0xF3F7 ==> Row:83 Col:87 */
    {0x7BEA,0xF3F8}, /* 篪: GB2312 Code: 0xF3F8 ==> Row:83 Col:88 */
    {0x7BEE,0xC0BA}, /* 篮: GB2312 Code: 0xC0BA ==> Row:32 Col:26 */
    {0x7BF1,0xC0E9}, /* 篱: GB2312 Code: 0xC0E9 ==> Row:32 Col:73 */
    {0x7BF7,0xC5F1}, /* 篷: GB2312 Code: 0xC5F1 ==> Row:37 Col:81 */
    {0x7BFC,0xF3FB}, /* 篼: GB2312 Code: 0xF3FB ==> Row:83 Col:91 */
    {0x7BFE,0xF3FA}, /* 篾: GB2312 Code: 0xF3FA ==> Row:83 Col:90 */
    {0x7C07,0xB4D8}, /* 簇: GB2312 Code: 0xB4D8 ==> Row:20 Col:56 */
    {0x7C0B,0xF3FE}, /* 簋: GB2312 Code: 0xF3FE ==> Row:83 Col:94 */
    {0x7C0C,0xF3F9}, /* 簌: GB2312 Code: 0xF3F9 ==> Row:83 Col:89 */
    {0x7C0F,0xF3FC}, /* 簏: GB2312 Code: 0xF3FC ==> Row:83 Col:92 */
    {0x7C16,0xF3FD}, /* 簖: GB2312 Code: 0xF3FD ==> Row:83 Col:93 */
    {0x7C1F,0xF4A1}, /* 簟: GB2312 Code: 0xF4A1 ==> Row:84 Col:01 */
    {0x7C26,0xF4A3}, /* 簦: GB2312 Code: 0xF4A3 ==> Row:84 Col:03 */
    {0x7C27,0xBBC9}, /* 簧: GB2312 Code: 0xBBC9 ==> Row:27 Col:41 */
    {0x7C2A,0xF4A2}, /* 簪: GB2312 Code: 0xF4A2 ==> Row:84 Col:02 */
    {0x7C38,0xF4A4}, /* 簸: GB2312 Code: 0xF4A4 ==> Row:84 Col:04 */
    {0x7C3F,0xB2BE}, /* 簿: GB2312 Code: 0xB2BE ==> Row:18 Col:30 */
    {0x7C40,0xF4A6}, /* 籀: GB2312 Code: 0xF4A6 ==> Row:84 Col:06 */
    {0x7C41,0xF4A5}, /* 籁: GB2312 Code: 0xF4A5 ==> Row:84 Col:05 */
    {0x7C4D,0xBCAE}, /* 籍: GB2312 Code: 0xBCAE ==> Row:28 Col:14 */
    {0x7C73,0xC3D7}, /* 米: GB2312 Code: 0xC3D7 ==> Row:35 Col:55 */
    {0x7C74,0xD9E1}, /* 籴: GB2312 Code: 0xD9E1 ==> Row:57 Col:65 */
    {0x7C7B,0xC0E0}, /* 类: GB2312 Code: 0xC0E0 ==> Row:32 Col:64 */
    {0x7C7C,0xF4CC}, /* 籼: GB2312 Code: 0xF4CC ==> Row:84 Col:44 */
    {0x7C7D,0xD7D1}, /* 籽: GB2312 Code: 0xD7D1 ==> Row:55 Col:49 */
    {0x7C89,0xB7DB}, /* 粉: GB2312 Code: 0xB7DB ==> Row:23 Col:59 */
    {0x7C91,0xF4CE}, /* 粑: GB2312 Code: 0xF4CE ==> Row:84 Col:46 */
    {0x7C92,0xC1A3}, /* 粒: GB2312 Code: 0xC1A3 ==> Row:33 Col:03 */
    {0x7C95,0xC6C9}, /* 粕: GB2312 Code: 0xC6C9 ==> Row:38 Col:41 */
    {0x7C97,0xB4D6}, /* 粗: GB2312 Code: 0xB4D6 ==> Row:20 Col:54 */
    {0x7C98,0xD5B3}, /* 粘: GB2312 Code: 0xD5B3 ==> Row:53 Col:19 */
    {0x7C9C,0xF4D0}, /* 粜: GB2312 Code: 0xF4D0 ==> Row:84 Col:48 */
    {0x7C9D,0xF4CF}, /* 粝: GB2312 Code: 0xF4CF ==> Row:84 Col:47 */
    {0x7C9E,0xF4D1}, /* 粞: GB2312 Code: 0xF4D1 ==> Row:84 Col:49 */
    {0x7C9F,0xCBDA}, /* 粟: GB2312 Code: 0xCBDA ==> Row:43 Col:58 */
    {0x7CA2,0xF4D2}, /* 粢: GB2312 Code: 0xF4D2 ==> Row:84 Col:50 */
    {0x7CA4,0xD4C1}, /* 粤: GB2312 Code: 0xD4C1 ==> Row:52 Col:33 */
    {0x7CA5,0xD6E0}, /* 粥: GB2312 Code: 0xD6E0 ==> Row:54 Col:64 */
    {0x7CAA,0xB7E0}, /* 粪: GB2312 Code: 0xB7E0 ==> Row:23 Col:64 */
    {0x7CAE,0xC1B8}, /* 粮: GB2312 Code: 0xC1B8 ==> Row:33 Col:24 */
    {0x7CB1,0xC1BB}, /* 粱: GB2312 Code: 0xC1BB ==> Row:33 Col:27 */
    {0x7CB2,0xF4D3}, /* 粲: GB2312 Code: 0xF4D3 ==> Row:84 Col:51 */
    {0x7CB3,0xBEAC}, /* 粳: GB2312 Code: 0xBEAC ==> Row:30 Col:12 */
    {0x7CB9,0xB4E2}, /* 粹: GB2312 Code: 0xB4E2 ==> Row:20 Col:66 */
    {0x7CBC,0xF4D4}, /* 粼: GB2312 Code: 0xF4D4 ==> Row:84 Col:52 */
    {0x7CBD,0xF4D5}, /* 粽: GB2312 Code: 0xF4D5 ==> Row:84 Col:53 */
    {0x7CBE,0xBEAB}, /* 精: GB2312 Code: 0xBEAB ==> Row:30 Col:11 */
    {0x7CC1,0xF4D6}, /* 糁: GB2312 Code: 0xF4D6 ==> Row:84 Col:54 */
    {0x7CC5,0xF4DB}, /* 糅: GB2312 Code: 0xF4DB ==> Row:84 Col:59 */
    {0x7CC7,0xF4D7}, /* 糇: GB2312 Code: 0xF4D7 ==> Row:84 Col:55 */
    {0x7CC8,0xF4DA}, /* 糈: GB2312 Code: 0xF4DA ==> Row:84 Col:58 */
    {0x7CCA,0xBAFD}, /* 糊: GB2312 Code: 0xBAFD ==> Row:26 Col:93 */
    {0x7CCC,0xF4D8}, /* 糌: GB2312 Code: 0xF4D8 ==> Row:84 Col:56 */
    {0x7CCD,0xF4D9}, /* 糍: GB2312 Code: 0xF4D9 ==> Row:84 Col:57 */
    {0x7CD5,0xB8E2}, /* 糕: GB2312 Code: 0xB8E2 ==> Row:24 Col:66 */
    {0x7CD6,0xCCC7}, /* 糖: GB2312 Code: 0xCCC7 ==> Row:44 Col:39 */
    {0x7CD7,0xF4DC}, /* 糗: GB2312 Code: 0xF4DC ==> Row:84 Col:60 */
    {0x7CD9,0xB2DA}, /* 糙: GB2312 Code: 0xB2DA ==> Row:18 Col:58 */
    {0x7CDC,0xC3D3}, /* 糜: GB2312 Code: 0xC3D3 ==> Row:35 Col:51 */
    {0x7CDF,0xD4E3}, /* 糟: GB2312 Code: 0xD4E3 ==> Row:52 Col:67 */
    {0x7CE0,0xBFB7}, /* 糠: GB2312 Code: 0xBFB7 ==> Row:31 Col:23 */
    {0x7CE8,0xF4DD}, /* 糨: GB2312 Code: 0xF4DD ==> Row:84 Col:61 */
    {0x7CEF,0xC5B4}, /* 糯: GB2312 Code: 0xC5B4 ==> Row:37 Col:20 */
    {0x7CF8,0xF4E9}, /* 糸: GB2312 Code: 0xF4E9 ==> Row:84 Col:73 */
    {0x7CFB,0xCFB5}, /* 系: GB2312 Code: 0xCFB5 ==> Row:47 Col:21 */
    {0x7D0A,0xCEC9}, /* 紊: GB2312 Code: 0xCEC9 ==> Row:46 Col:41 */
    {0x7D20,0xCBD8}, /* 素: GB2312 Code: 0xCBD8 ==> Row:43 Col:56 */
    {0x7D22,0xCBF7}, /* 索: GB2312 Code: 0xCBF7 ==> Row:43 Col:87 */
    {0x7D27,0xBDF4}, /* 紧: GB2312 Code: 0xBDF4 ==> Row:29 Col:84 */
    {0x7D2B,0xD7CF}, /* 紫: GB2312 Code: 0xD7CF ==> Row:55 Col:47 */
    {0x7D2F,0xC0DB}, /* 累: GB2312 Code: 0xC0DB ==> Row:32 Col:59 */
    {0x7D6E,0xD0F5}, /* 絮: GB2312 Code: 0xD0F5 ==> Row:48 Col:85 */
    {0x7D77,0xF4EA}, /* 絷: GB2312 Code: 0xF4EA ==> Row:84 Col:74 */
    {0x7DA6,0xF4EB}, /* 綦: GB2312 Code: 0xF4EB ==> Row:84 Col:75 */
    {0x7DAE,0xF4EC}, /* 綮: GB2312 Code: 0xF4EC ==> Row:84 Col:76 */
    {0x7E3B,0xF7E3}, /* 縻: GB2312 Code: 0xF7E3 ==> Row:87 Col:67 */
    {0x7E41,0xB7B1}, /* 繁: GB2312 Code: 0xB7B1 ==> Row:23 Col:17 */
    {0x7E47,0xF4ED}, /* 繇: GB2312 Code: 0xF4ED ==> Row:84 Col:77 */
    {0x7E82,0xD7EB}, /* 纂: GB2312 Code: 0xD7EB ==> Row:55 Col:75 */
    {0x7E9B,0xF4EE}, /* 纛: GB2312 Code: 0xF4EE ==> Row:84 Col:78 */
    {0x7E9F,0xE6F9}, /* 纟: GB2312 Code: 0xE6F9 ==> Row:70 Col:89 */
    {0x7EA0,0xBEC0}, /* 纠: GB2312 Code: 0xBEC0 ==> Row:30 Col:32 */
    {0x7EA1,0xE6FA}, /* 纡: GB2312 Code: 0xE6FA ==> Row:70 Col:90 */
    {0x7EA2,0xBAEC}, /* 红: GB2312 Code: 0xBAEC ==> Row:26 Col:76 */
    {0x7EA3,0xE6FB}, /* 纣: GB2312 Code: 0xE6FB ==> Row:70 Col:91 */
    {0x7EA4,0xCFCB}, /* 纤: GB2312 Code: 0xCFCB ==> Row:47 Col:43 */
    {0x7EA5,0xE6FC}, /* 纥: GB2312 Code: 0xE6FC ==> Row:70 Col:92 */
    {0x7EA6,0xD4BC}, /* 约: GB2312 Code: 0xD4BC ==> Row:52 Col:28 */
    {0x7EA7,0xBCB6}, /* 级: GB2312 Code: 0xBCB6 ==> Row:28 Col:22 */
    {0x7EA8,0xE6FD}, /* 纨: GB2312 Code: 0xE6FD ==> Row:70 Col:93 */
    {0x7EA9,0xE6FE}, /* 纩: GB2312 Code: 0xE6FE ==> Row:70 Col:94 */
    {0x7EAA,0xBCCD}, /* 纪: GB2312 Code: 0xBCCD ==> Row:28 Col:45 */
    {0x7EAB,0xC8D2}, /* 纫: GB2312 Code: 0xC8D2 ==> Row:40 Col:50 */
    {0x7EAC,0xCEB3}, /* 纬: GB2312 Code: 0xCEB3 ==> Row:46 Col:19 */
    {0x7EAD,0xE7A1}, /* 纭: GB2312 Code: 0xE7A1 ==> Row:71 Col:01 */
    {0x7EAF,0xB4BF}, /* 纯: GB2312 Code: 0xB4BF ==> Row:20 Col:31 */
    {0x7EB0,0xE7A2}, /* 纰: GB2312 Code: 0xE7A2 ==> Row:71 Col:02 */
    {0x7EB1,0xC9B4}, /* 纱: GB2312 Code: 0xC9B4 ==> Row:41 Col:20 */
    {0x7EB2,0xB8D9}, /* 纲: GB2312 Code: 0xB8D9 ==> Row:24 Col:57 */
    {0x7EB3,0xC4C9}, /* 纳: GB2312 Code: 0xC4C9 ==> Row:36 Col:41 */
    {0x7EB5,0xD7DD}, /* 纵: GB2312 Code: 0xD7DD ==> Row:55 Col:61 */
    {0x7EB6,0xC2DA}, /* 纶: GB2312 Code: 0xC2DA ==> Row:34 Col:58 */
    {0x7EB7,0xB7D7}, /* 纷: GB2312 Code: 0xB7D7 ==> Row:23 Col:55 */
    {0x7EB8,0xD6BD}, /* 纸: GB2312 Code: 0xD6BD ==> Row:54 Col:29 */
    {0x7EB9,0xCEC6}, /* 纹: GB2312 Code: 0xCEC6 ==> Row:46 Col:38 */
    {0x7EBA,0xB7C4}, /* 纺: GB2312 Code: 0xB7C4 ==> Row:23 Col:36 */
    {0x7EBD,0xC5A6}, /* 纽: GB2312 Code: 0xC5A6 ==> Row:37 Col:06 */
    {0x7EBE,0xE7A3}, /* 纾: GB2312 Code: 0xE7A3 ==> Row:71 Col:03 */
    {0x7EBF,0xCFDF}, /* 线: GB2312 Code: 0xCFDF ==> Row:47 Col:63 */
    {0x7EC0,0xE7A4}, /* 绀: GB2312 Code: 0xE7A4 ==> Row:71 Col:04 */
    {0x7EC1,0xE7A5}, /* 绁: GB2312 Code: 0xE7A5 ==> Row:71 Col:05 */
    {0x7EC2,0xE7A6}, /* 绂: GB2312 Code: 0xE7A6 ==> Row:71 Col:06 */
    {0x7EC3,0xC1B7}, /* 练: GB2312 Code: 0xC1B7 ==> Row:33 Col:23 */
    {0x7EC4,0xD7E9}, /* 组: GB2312 Code: 0xD7E9 ==> Row:55 Col:73 */
    {0x7EC5,0xC9F0}, /* 绅: GB2312 Code: 0xC9F0 ==> Row:41 Col:80 */
    {0x7EC6,0xCFB8}, /* 细: GB2312 Code: 0xCFB8 ==> Row:47 Col:24 */
    {0x7EC7,0xD6AF}, /* 织: GB2312 Code: 0xD6AF ==> Row:54 Col:15 */
    {0x7EC8,0xD6D5}, /* 终: GB2312 Code: 0xD6D5 ==> Row:54 Col:53 */
    {0x7EC9,0xE7A7}, /* 绉: GB2312 Code: 0xE7A7 ==> Row:71 Col:07 */
    {0x7ECA,0xB0ED}, /* 绊: GB2312 Code: 0xB0ED ==> Row:16 Col:77 */
    {0x7ECB,0xE7A8}, /* 绋: GB2312 Code: 0xE7A8 ==> Row:71 Col:08 */
    {0x7ECC,0xE7A9}, /* 绌: GB2312 Code: 0xE7A9 ==> Row:71 Col:09 */
    {0x7ECD,0xC9DC}, /* 绍: GB2312 Code: 0xC9DC ==> Row:41 Col:60 */
    {0x7ECE,0xD2EF}, /* 绎: GB2312 Code: 0xD2EF ==> Row:50 Col:79 */
    {0x7ECF,0xBEAD}, /* 经: GB2312 Code: 0xBEAD ==> Row:30 Col:13 */
    {0x7ED0,0xE7AA}, /* 绐: GB2312 Code: 0xE7AA ==> Row:71 Col:10 */
    {0x7ED1,0xB0F3}, /* 绑: GB2312 Code: 0xB0F3 ==> Row:16 Col:83 */
    {0x7ED2,0xC8DE}, /* 绒: GB2312 Code: 0xC8DE ==> Row:40 Col:62 */
    {0x7ED3,0xBDE1}, /* 结: GB2312 Code: 0xBDE1 ==> Row:29 Col:65 */
    {0x7ED4,0xE7AB}, /* 绔: GB2312 Code: 0xE7AB ==> Row:71 Col:11 */
    {0x7ED5,0xC8C6}, /* 绕: GB2312 Code: 0xC8C6 ==> Row:40 Col:38 */
    {0x7ED7,0xE7AC}, /* 绗: GB2312 Code: 0xE7AC ==> Row:71 Col:12 */
    {0x7ED8,0xBBE6}, /* 绘: GB2312 Code: 0xBBE6 ==> Row:27 Col:70 */
    {0x7ED9,0xB8F8}, /* 给: GB2312 Code: 0xB8F8 ==> Row:24 Col:88 */
    {0x7EDA,0xD1A4}, /* 绚: GB2312 Code: 0xD1A4 ==> Row:49 Col:04 */
    {0x7EDB,0xE7AD}, /* 绛: GB2312 Code: 0xE7AD ==> Row:71 Col:13 */
    {0x7EDC,0xC2E7}, /* 络: GB2312 Code: 0xC2E7 ==> Row:34 Col:71 */
    {0x7EDD,0xBEF8}, /* 绝: GB2312 Code: 0xBEF8 ==> Row:30 Col:88 */
    {0x7EDE,0xBDCA}, /* 绞: GB2312 Code: 0xBDCA ==> Row:29 Col:42 */
    {0x7EDF,0xCDB3}, /* 统: GB2312 Code: 0xCDB3 ==> Row:45 Col:19 */
    {0x7EE0,0xE7AE}, /* 绠: GB2312 Code: 0xE7AE ==> Row:71 Col:14 */
    {0x7EE1,0xE7AF}, /* 绡: GB2312 Code: 0xE7AF ==> Row:71 Col:15 */
    {0x7EE2,0xBEEE}, /* 绢: GB2312 Code: 0xBEEE ==> Row:30 Col:78 */
    {0x7EE3,0xD0E5}, /* 绣: GB2312 Code: 0xD0E5 ==> Row:48 Col:69 */
    {0x7EE5,0xCBE7}, /* 绥: GB2312 Code: 0xCBE7 ==> Row:43 Col:71 */
    {0x7EE6,0xCCD0}, /* 绦: GB2312 Code: 0xCCD0 ==> Row:44 Col:48 */
    {0x7EE7,0xBCCC}, /* 继: GB2312 Code: 0xBCCC ==> Row:28 Col:44 */
    {0x7EE8,0xE7B0}, /* 绨: GB2312 Code: 0xE7B0 ==> Row:71 Col:16 */
    {0x7EE9,0xBCA8}, /* 绩: GB2312 Code: 0xBCA8 ==> Row:28 Col:08 */
    {0x7EEA,0xD0F7}, /* 绪: GB2312 Code: 0xD0F7 ==> Row:48 Col:87 */
    {0x7EEB,0xE7B1}, /* 绫: GB2312 Code: 0xE7B1 ==> Row:71 Col:17 */
    {0x7EED,0xD0F8}, /* 续: GB2312 Code: 0xD0F8 ==> Row:48 Col:88 */
    {0x7EEE,0xE7B2}, /* 绮: GB2312 Code: 0xE7B2 ==> Row:71 Col:18 */
    {0x7EEF,0xE7B3}, /* 绯: GB2312 Code: 0xE7B3 ==> Row:71 Col:19 */
    {0x7EF0,0xB4C2}, /* 绰: GB2312 Code: 0xB4C2 ==> Row:20 Col:34 */
    {0x7EF1,0xE7B4}, /* 绱: GB2312 Code: 0xE7B4 ==> Row:71 Col:20 */
    {0x7EF2,0xE7B5}, /* 绲: GB2312 Code: 0xE7B5 ==> Row:71 Col:21 */
    {0x7EF3,0xC9FE}, /* 绳: GB2312 Code: 0xC9FE ==> Row:41 Col:94 */
    {0x7EF4,0xCEAC}, /* 维: GB2312 Code: 0xCEAC ==> Row:46 Col:12 */
    {0x7EF5,0xC3E0}, /* 绵: GB2312 Code: 0xC3E0 ==> Row:35 Col:64 */
    {0x7EF6,0xE7B7}, /* 绶: GB2312 Code: 0xE7B7 ==> Row:71 Col:23 */
    {0x7EF7,0xB1C1}, /* 绷: GB2312 Code: 0xB1C1 ==> Row:17 Col:33 */
    {0x7EF8,0xB3F1}, /* 绸: GB2312 Code: 0xB3F1 ==> Row:19 Col:81 */
    {0x7EFA,0xE7B8}, /* 绺: GB2312 Code: 0xE7B8 ==> Row:71 Col:24 */
    {0x7EFB,0xE7B9}, /* 绻: GB2312 Code: 0xE7B9 ==> Row:71 Col:25 */
    {0x7EFC,0xD7DB}, /* 综: GB2312 Code: 0xD7DB ==> Row:55 Col:59 */
    {0x7EFD,0xD5C0}, /* 绽: GB2312 Code: 0xD5C0 ==> Row:53 Col:32 */
    {0x7EFE,0xE7BA}, /* 绾: GB2312 Code: 0xE7BA ==> Row:71 Col:26 */
    {0x7EFF,0xC2CC}, /* 绿: GB2312 Code: 0xC2CC ==> Row:34 Col:44 */
    {0x7F00,0xD7BA}, /* 缀: GB2312 Code: 0xD7BA ==> Row:55 Col:26 */
    {0x7F01,0xE7BB}, /* 缁: GB2312 Code: 0xE7BB ==> Row:71 Col:27 */
    {0x7F02,0xE7BC}, /* 缂: GB2312 Code: 0xE7BC ==> Row:71 Col:28 */
    {0x7F03,0xE7BD}, /* 缃: GB2312 Code: 0xE7BD ==> Row:71 Col:29 */
    {0x7F04,0xBCEA}, /* 缄: GB2312 Code: 0xBCEA ==> Row:28 Col:74 */
    {0x7F05,0xC3E5}, /* 缅: GB2312 Code: 0xC3E5 ==> Row:35 Col:69 */
    {0x7F06,0xC0C2}, /* 缆: GB2312 Code: 0xC0C2 ==> Row:32 Col:34 */
    {0x7F07,0xE7BE}, /* 缇: GB2312 Code: 0xE7BE ==> Row:71 Col:30 */
    {0x7F08,0xE7BF}, /* 缈: GB2312 Code: 0xE7BF ==> Row:71 Col:31 */
    {0x7F09,0xBCA9}, /* 缉: GB2312 Code: 0xBCA9 ==> Row:28 Col:09 */
    {0x7F0B,0xE7C0}, /* 缋: GB2312 Code: 0xE7C0 ==> Row:71 Col:32 */
    {0x7F0C,0xE7C1}, /* 缌: GB2312 Code: 0xE7C1 ==> Row:71 Col:33 */
    {0x7F0D,0xE7B6}, /* 缍: GB2312 Code: 0xE7B6 ==> Row:71 Col:22 */
    {0x7F0E,0xB6D0}, /* 缎: GB2312 Code: 0xB6D0 ==> Row:22 Col:48 */
    {0x7F0F,0xE7C2}, /* 缏: GB2312 Code: 0xE7C2 ==> Row:71 Col:34 */
    {0x7F11,0xE7C3}, /* 缑: GB2312 Code: 0xE7C3 ==> Row:71 Col:35 */
    {0x7F12,0xE7C4}, /* 缒: GB2312 Code: 0xE7C4 ==> Row:71 Col:36 */
    {0x7F13,0xBBBA}, /* 缓: GB2312 Code: 0xBBBA ==> Row:27 Col:26 */
    {0x7F14,0xB5DE}, /* 缔: GB2312 Code: 0xB5DE ==> Row:21 Col:62 */
    {0x7F15,0xC2C6}, /* 缕: GB2312 Code: 0xC2C6 ==> Row:34 Col:38 */
    {0x7F16,0xB1E0}, /* 编: GB2312 Code: 0xB1E0 ==> Row:17 Col:64 */
    {0x7F17,0xE7C5}, /* 缗: GB2312 Code: 0xE7C5 ==> Row:71 Col:37 */
    {0x7F18,0xD4B5}, /* 缘: GB2312 Code: 0xD4B5 ==> Row:52 Col:21 */
    {0x7F19,0xE7C6}, /* 缙: GB2312 Code: 0xE7C6 ==> Row:71 Col:38 */
    {0x7F1A,0xB8BF}, /* 缚: GB2312 Code: 0xB8BF ==> Row:24 Col:31 */
    {0x7F1B,0xE7C8}, /* 缛: GB2312 Code: 0xE7C8 ==> Row:71 Col:40 */
    {0x7F1C,0xE7C7}, /* 缜: GB2312 Code: 0xE7C7 ==> Row:71 Col:39 */
    {0x7F1D,0xB7EC}, /* 缝: GB2312 Code: 0xB7EC ==> Row:23 Col:76 */
    {0x7F1F,0xE7C9}, /* 缟: GB2312 Code: 0xE7C9 ==> Row:71 Col:41 */
    {0x7F20,0xB2F8}, /* 缠: GB2312 Code: 0xB2F8 ==> Row:18 Col:88 */
    {0x7F21,0xE7CA}, /* 缡: GB2312 Code: 0xE7CA ==> Row:71 Col:42 */
    {0x7F22,0xE7CB}, /* 缢: GB2312 Code: 0xE7CB ==> Row:71 Col:43 */
    {0x7F23,0xE7CC}, /* 缣: GB2312 Code: 0xE7CC ==> Row:71 Col:44 */
    {0x7F24,0xE7CD}, /* 缤: GB2312 Code: 0xE7CD ==> Row:71 Col:45 */
    {0x7F25,0xE7CE}, /* 缥: GB2312 Code: 0xE7CE ==> Row:71 Col:46 */
    {0x7F26,0xE7CF}, /* 缦: GB2312 Code: 0xE7CF ==> Row:71 Col:47 */
    {0x7F27,0xE7D0}, /* 缧: GB2312 Code: 0xE7D0 ==> Row:71 Col:48 */
    {0x7F28,0xD3A7}, /* 缨: GB2312 Code: 0xD3A7 ==> Row:51 Col:07 */
    {0x7F29,0xCBF5}, /* 缩: GB2312 Code: 0xCBF5 ==> Row:43 Col:85 */
    {0x7F2A,0xE7D1}, /* 缪: GB2312 Code: 0xE7D1 ==> Row:71 Col:49 */
    {0x7F2B,0xE7D2}, /* 缫: GB2312 Code: 0xE7D2 ==> Row:71 Col:50 */
    {0x7F2C,0xE7D3}, /* 缬: GB2312 Code: 0xE7D3 ==> Row:71 Col:51 */
    {0x7F2D,0xE7D4}, /* 缭: GB2312 Code: 0xE7D4 ==> Row:71 Col:52 */
    {0x7F2E,0xC9C9}, /* 缮: GB2312 Code: 0xC9C9 ==> Row:41 Col:41 */
    {0x7F2F,0xE7D5}, /* 缯: GB2312 Code: 0xE7D5 ==> Row:71 Col:53 */
    {0x7F30,0xE7D6}, /* 缰: GB2312 Code: 0xE7D6 ==> Row:71 Col:54 */
    {0x7F31,0xE7D7}, /* 缱: GB2312 Code: 0xE7D7 ==> Row:71 Col:55 */
    {0x7F32,0xE7D8}, /* 缲: GB2312 Code: 0xE7D8 ==> Row:71 Col:56 */
    {0x7F33,0xE7D9}, /* 缳: GB2312 Code: 0xE7D9 ==> Row:71 Col:57 */
    {0x7F34,0xBDC9}, /* 缴: GB2312 Code: 0xBDC9 ==> Row:29 Col:41 */
    {0x7F35,0xE7DA}, /* 缵: GB2312 Code: 0xE7DA ==> Row:71 Col:58 */
    {0x7F36,0xF3BE}, /* 缶: GB2312 Code: 0xF3BE ==> Row:83 Col:30 */
    {0x7F38,0xB8D7}, /* 缸: GB2312 Code: 0xB8D7 ==> Row:24 Col:55 */
    {0x7F3A,0xC8B1}, /* 缺: GB2312 Code: 0xC8B1 ==> Row:40 Col:17 */
    {0x7F42,0xF3BF}, /* 罂: GB2312 Code: 0xF3BF ==> Row:83 Col:31 */
    {0x7F44,0xF3C0}, /* 罄: GB2312 Code: 0xF3C0 ==> Row:83 Col:32 */
    {0x7F45,0xF3C1}, /* 罅: GB2312 Code: 0xF3C1 ==> Row:83 Col:33 */
    {0x7F50,0xB9DE}, /* 罐: GB2312 Code: 0xB9DE ==> Row:25 Col:62 */
    {0x7F51,0xCDF8}, /* 网: GB2312 Code: 0xCDF8 ==> Row:45 Col:88 */
    {0x7F54,0xD8E8}, /* 罔: GB2312 Code: 0xD8E8 ==> Row:56 Col:72 */
    {0x7F55,0xBAB1}, /* 罕: GB2312 Code: 0xBAB1 ==> Row:26 Col:17 */
    {0x7F57,0xC2DE}, /* 罗: GB2312 Code: 0xC2DE ==> Row:34 Col:62 */
    {0x7F58,0xEEB7}, /* 罘: GB2312 Code: 0xEEB7 ==> Row:78 Col:23 */
    {0x7F5A,0xB7A3}, /* 罚: GB2312 Code: 0xB7A3 ==> Row:23 Col:03 */
    {0x7F5F,0xEEB9}, /* 罟: GB2312 Code: 0xEEB9 ==> Row:78 Col:25 */
    {0x7F61,0xEEB8}, /* 罡: GB2312 Code: 0xEEB8 ==> Row:78 Col:24 */
    {0x7F62,0xB0D5}, /* 罢: GB2312 Code: 0xB0D5 ==> Row:16 Col:53 */
    {0x7F68,0xEEBB}, /* 罨: GB2312 Code: 0xEEBB ==> Row:78 Col:27 */
    {0x7F69,0xD5D6}, /* 罩: GB2312 Code: 0xD5D6 ==> Row:53 Col:54 */
    {0x7F6A,0xD7EF}, /* 罪: GB2312 Code: 0xD7EF ==> Row:55 Col:79 */
    {0x7F6E,0xD6C3}, /* 置: GB2312 Code: 0xD6C3 ==> Row:54 Col:35 */
    {0x7F71,0xEEBD}, /* 罱: GB2312 Code: 0xEEBD ==> Row:78 Col:29 */
    {0x7F72,0xCAF0}, /* 署: GB2312 Code: 0xCAF0 ==> Row:42 Col:80 */
    {0x7F74,0xEEBC}, /* 罴: GB2312 Code: 0xEEBC ==> Row:78 Col:28 */
    {0x7F79,0xEEBE}, /* 罹: GB2312 Code: 0xEEBE ==> Row:78 Col:30 */
    {0x7F7E,0xEEC0}, /* 罾: GB2312 Code: 0xEEC0 ==> Row:78 Col:32 */
    {0x7F81,0xEEBF}, /* 羁: GB2312 Code: 0xEEBF ==> Row:78 Col:31 */
    {0x7F8A,0xD1F2}, /* 羊: GB2312 Code: 0xD1F2 ==> Row:49 Col:82 */
    {0x7F8C,0xC7BC}, /* 羌: GB2312 Code: 0xC7BC ==> Row:39 Col:28 */
    {0x7F8E,0xC3C0}, /* 美: GB2312 Code: 0xC3C0 ==> Row:35 Col:32 */
    {0x7F94,0xB8E1}, /* 羔: GB2312 Code: 0xB8E1 ==> Row:24 Col:65 */
    {0x7F9A,0xC1E7}, /* 羚: GB2312 Code: 0xC1E7 ==> Row:33 Col:71 */
    {0x7F9D,0xF4C6}, /* 羝: GB2312 Code: 0xF4C6 ==> Row:84 Col:38 */
    {0x7F9E,0xD0DF}, /* 羞: GB2312 Code: 0xD0DF ==> Row:48 Col:63 */
    {0x7F9F,0xF4C7}, /* 羟: GB2312 Code: 0xF4C7 ==> Row:84 Col:39 */
    {0x7FA1,0xCFDB}, /* 羡: GB2312 Code: 0xCFDB ==> Row:47 Col:59 */
    {0x7FA4,0xC8BA}, /* 群: GB2312 Code: 0xC8BA ==> Row:40 Col:26 */
    {0x7FA7,0xF4C8}, /* 羧: GB2312 Code: 0xF4C8 ==> Row:84 Col:40 */
    {0x7FAF,0xF4C9}, /* 羯: GB2312 Code: 0xF4C9 ==> Row:84 Col:41 */
    {0x7FB0,0xF4CA}, /* 羰: GB2312 Code: 0xF4CA ==> Row:84 Col:42 */
    {0x7FB2,0xF4CB}, /* 羲: GB2312 Code: 0xF4CB ==> Row:84 Col:43 */
    {0x7FB8,0xD9FA}, /* 羸: GB2312 Code: 0xD9FA ==> Row:57 Col:90 */
    {0x7FB9,0xB8FE}, /* 羹: GB2312 Code: 0xB8FE ==> Row:24 Col:94 */
    {0x7FBC,0xE5F1}, /* 羼: GB2312 Code: 0xE5F1 ==> Row:69 Col:81 */
    {0x7FBD,0xD3F0}, /* 羽: GB2312 Code: 0xD3F0 ==> Row:51 Col:80 */
    {0x7FBF,0xF4E0}, /* 羿: GB2312 Code: 0xF4E0 ==> Row:84 Col:64 */
    {0x7FC1,0xCECC}, /* 翁: GB2312 Code: 0xCECC ==> Row:46 Col:44 */
    {0x7FC5,0xB3E1}, /* 翅: GB2312 Code: 0xB3E1 ==> Row:19 Col:65 */
    {0x7FCA,0xF1B4}, /* 翊: GB2312 Code: 0xF1B4 ==> Row:81 Col:20 */
    {0x7FCC,0xD2EE}, /* 翌: GB2312 Code: 0xD2EE ==> Row:50 Col:78 */
    {0x7FCE,0xF4E1}, /* 翎: GB2312 Code: 0xF4E1 ==> Row:84 Col:65 */
    {0x7FD4,0xCFE8}, /* 翔: GB2312 Code: 0xCFE8 ==> Row:47 Col:72 */
    {0x7FD5,0xF4E2}, /* 翕: GB2312 Code: 0xF4E2 ==> Row:84 Col:66 */
    {0x7FD8,0xC7CC}, /* 翘: GB2312 Code: 0xC7CC ==> Row:39 Col:44 */
    {0x7FDF,0xB5D4}, /* 翟: GB2312 Code: 0xB5D4 ==> Row:21 Col:52 */
    {0x7FE0,0xB4E4}, /* 翠: GB2312 Code: 0xB4E4 ==> Row:20 Col:68 */
    {0x7FE1,0xF4E4}, /* 翡: GB2312 Code: 0xF4E4 ==> Row:84 Col:68 */
    {0x7FE5,0xF4E3}, /* 翥: GB2312 Code: 0xF4E3 ==> Row:84 Col:67 */
    {0x7FE6,0xF4E5}, /* 翦: GB2312 Code: 0xF4E5 ==> Row:84 Col:69 */
    {0x7FE9,0xF4E6}, /* 翩: GB2312 Code: 0xF4E6 ==> Row:84 Col:70 */
    {0x7FEE,0xF4E7}, /* 翮: GB2312 Code: 0xF4E7 ==> Row:84 Col:71 */
    {0x7FF0,0xBAB2}, /* 翰: GB2312 Code: 0xBAB2 ==> Row:26 Col:18 */
    {0x7FF1,0xB0BF}, /* 翱: GB2312 Code: 0xB0BF ==> Row:16 Col:31 */
    {0x7FF3,0xF4E8}, /* 翳: GB2312 Code: 0xF4E8 ==> Row:84 Col:72 */
    {0x7FFB,0xB7AD}, /* 翻: GB2312 Code: 0xB7AD ==> Row:23 Col:13 */
    {0x7FFC,0xD2ED}, /* 翼: GB2312 Code: 0xD2ED ==> Row:50 Col:77 */
    {0x8000,0xD2AB}, /* 耀: GB2312 Code: 0xD2AB ==> Row:50 Col:11 */
    {0x8001,0xC0CF}, /* 老: GB2312 Code: 0xC0CF ==> Row:32 Col:47 */
    {0x8003,0xBFBC}, /* 考: GB2312 Code: 0xBFBC ==> Row:31 Col:28 */
    {0x8004,0xEBA3}, /* 耄: GB2312 Code: 0xEBA3 ==> Row:75 Col:03 */
    {0x8005,0xD5DF}, /* 者: GB2312 Code: 0xD5DF ==> Row:53 Col:63 */
    {0x8006,0xEAC8}, /* 耆: GB2312 Code: 0xEAC8 ==> Row:74 Col:40 */
    {0x800B,0xF1F3}, /* 耋: GB2312 Code: 0xF1F3 ==> Row:81 Col:83 */
    {0x800C,0xB6F8}, /* 而: GB2312 Code: 0xB6F8 ==> Row:22 Col:88 */
    {0x800D,0xCBA3}, /* 耍: GB2312 Code: 0xCBA3 ==> Row:43 Col:03 */
    {0x8010,0xC4CD}, /* 耐: GB2312 Code: 0xC4CD ==> Row:36 Col:45 */
    {0x8012,0xF1E7}, /* 耒: GB2312 Code: 0xF1E7 ==> Row:81 Col:71 */
    {0x8014,0xF1E8}, /* 耔: GB2312 Code: 0xF1E8 ==> Row:81 Col:72 */
    {0x8015,0xB8FB}, /* 耕: GB2312 Code: 0xB8FB ==> Row:24 Col:91 */
    {0x8016,0xF1E9}, /* 耖: GB2312 Code: 0xF1E9 ==> Row:81 Col:73 */
    {0x8017,0xBAC4}, /* 耗: GB2312 Code: 0xBAC4 ==> Row:26 Col:36 */
    {0x8018,0xD4C5}, /* 耘: GB2312 Code: 0xD4C5 ==> Row:52 Col:37 */
    {0x8019,0xB0D2}, /* 耙: GB2312 Code: 0xB0D2 ==> Row:16 Col:50 */
    {0x801C,0xF1EA}, /* 耜: GB2312 Code: 0xF1EA ==> Row:81 Col:74 */
    {0x8020,0xF1EB}, /* 耠: GB2312 Code: 0xF1EB ==> Row:81 Col:75 */
    {0x8022,0xF1EC}, /* 耢: GB2312 Code: 0xF1EC ==> Row:81 Col:76 */
    {0x8025,0xF1ED}, /* 耥: GB2312 Code: 0xF1ED ==> Row:81 Col:77 */
    {0x8026,0xF1EE}, /* 耦: GB2312 Code: 0xF1EE ==> Row:81 Col:78 */
    {0x8027,0xF1EF}, /* 耧: GB2312 Code: 0xF1EF ==> Row:81 Col:79 */
    {0x8028,0xF1F1}, /* 耨: GB2312 Code: 0xF1F1 ==> Row:81 Col:81 */
    {0x8029,0xF1F0}, /* 耩: GB2312 Code: 0xF1F0 ==> Row:81 Col:80 */
    {0x802A,0xC5D5}, /* 耪: GB2312 Code: 0xC5D5 ==> Row:37 Col:53 */
    {0x8031,0xF1F2}, /* 耱: GB2312 Code: 0xF1F2 ==> Row:81 Col:82 */
    {0x8033,0xB6FA}, /* 耳: GB2312 Code: 0xB6FA ==> Row:22 Col:90 */
    {0x8035,0xF1F4}, /* 耵: GB2312 Code: 0xF1F4 ==> Row:81 Col:84 */
    {0x8036,0xD2AE}, /* 耶: GB2312 Code: 0xD2AE ==> Row:50 Col:14 */
    {0x8037,0xDEC7}, /* 耷: GB2312 Code: 0xDEC7 ==> Row:62 Col:39 */
    {0x8038,0xCBCA}, /* 耸: GB2312 Code: 0xCBCA ==> Row:43 Col:42 */
    {0x803B,0xB3DC}, /* 耻: GB2312 Code: 0xB3DC ==> Row:19 Col:60 */
    {0x803D,0xB5A2}, /* 耽: GB2312 Code: 0xB5A2 ==> Row:21 Col:02 */
    {0x803F,0xB9A2}, /* 耿: GB2312 Code: 0xB9A2 ==> Row:25 Col:02 */
    {0x8042,0xC4F4}, /* 聂: GB2312 Code: 0xC4F4 ==> Row:36 Col:84 */
    {0x8043,0xF1F5}, /* 聃: GB2312 Code: 0xF1F5 ==> Row:81 Col:85 */
    {0x8046,0xF1F6}, /* 聆: GB2312 Code: 0xF1F6 ==> Row:81 Col:86 */
    {0x804A,0xC1C4}, /* 聊: GB2312 Code: 0xC1C4 ==> Row:33 Col:36 */
    {0x804B,0xC1FB}, /* 聋: GB2312 Code: 0xC1FB ==> Row:33 Col:91 */
    {0x804C,0xD6B0}, /* 职: GB2312 Code: 0xD6B0 ==> Row:54 Col:16 */
    {0x804D,0xF1F7}, /* 聍: GB2312 Code: 0xF1F7 ==> Row:81 Col:87 */
    {0x8052,0xF1F8}, /* 聒: GB2312 Code: 0xF1F8 ==> Row:81 Col:88 */
    {0x8054,0xC1AA}, /* 联: GB2312 Code: 0xC1AA ==> Row:33 Col:10 */
    {0x8058,0xC6B8}, /* 聘: GB2312 Code: 0xC6B8 ==> Row:38 Col:24 */
    {0x805A,0xBEDB}, /* 聚: GB2312 Code: 0xBEDB ==> Row:30 Col:59 */
    {0x8069,0xF1F9}, /* 聩: GB2312 Code: 0xF1F9 ==> Row:81 Col:89 */
    {0x806A,0xB4CF}, /* 聪: GB2312 Code: 0xB4CF ==> Row:20 Col:47 */
    {0x8071,0xF1FA}, /* 聱: GB2312 Code: 0xF1FA ==> Row:81 Col:90 */
    {0x807F,0xEDB2}, /* 聿: GB2312 Code: 0xEDB2 ==> Row:77 Col:18 */
    {0x8080,0xEDB1}, /* 肀: GB2312 Code: 0xEDB1 ==> Row:77 Col:17 */
    {0x8083,0xCBE0}, /* 肃: GB2312 Code: 0xCBE0 ==> Row:43 Col:64 */
    {0x8084,0xD2DE}, /* 肄: GB2312 Code: 0xD2DE ==> Row:50 Col:62 */
    {0x8086,0xCBC1}, /* 肆: GB2312 Code: 0xCBC1 ==> Row:43 Col:33 */
    {0x8087,0xD5D8}, /* 肇: GB2312 Code: 0xD5D8 ==> Row:53 Col:56 */
    {0x8089,0xC8E2}, /* 肉: GB2312 Code: 0xC8E2 ==> Row:40 Col:66 */
    {0x808B,0xC0DF}, /* 肋: GB2312 Code: 0xC0DF ==> Row:32 Col:63 */
    {0x808C,0xBCA1}, /* 肌: GB2312 Code: 0xBCA1 ==> Row:28 Col:01 */
    {0x8093,0xEBC1}, /* 肓: GB2312 Code: 0xEBC1 ==> Row:75 Col:33 */
    {0x8096,0xD0A4}, /* 肖: GB2312 Code: 0xD0A4 ==> Row:48 Col:04 */
    {0x8098,0xD6E2}, /* 肘: GB2312 Code: 0xD6E2 ==> Row:54 Col:66 */
    {0x809A,0xB6C7}, /* 肚: GB2312 Code: 0xB6C7 ==> Row:22 Col:39 */
    {0x809B,0xB8D8}, /* 肛: GB2312 Code: 0xB8D8 ==> Row:24 Col:56 */
    {0x809C,0xEBC0}, /* 肜: GB2312 Code: 0xEBC0 ==> Row:75 Col:32 */
    {0x809D,0xB8CE}, /* 肝: GB2312 Code: 0xB8CE ==> Row:24 Col:46 */
    {0x809F,0xEBBF}, /* 肟: GB2312 Code: 0xEBBF ==> Row:75 Col:31 */
    {0x80A0,0xB3A6}, /* 肠: GB2312 Code: 0xB3A6 ==> Row:19 Col:06 */
    {0x80A1,0xB9C9}, /* 股: GB2312 Code: 0xB9C9 ==> Row:25 Col:41 */
    {0x80A2,0xD6AB}, /* 肢: GB2312 Code: 0xD6AB ==> Row:54 Col:11 */
    {0x80A4,0xB7F4}, /* 肤: GB2312 Code: 0xB7F4 ==> Row:23 Col:84 */
    {0x80A5,0xB7CA}, /* 肥: GB2312 Code: 0xB7CA ==> Row:23 Col:42 */
    {0x80A9,0xBCE7}, /* 肩: GB2312 Code: 0xBCE7 ==> Row:28 Col:71 */
    {0x80AA,0xB7BE}, /* 肪: GB2312 Code: 0xB7BE ==> Row:23 Col:30 */
    {0x80AB,0xEBC6}, /* 肫: GB2312 Code: 0xEBC6 ==> Row:75 Col:38 */
    {0x80AD,0xEBC7}, /* 肭: GB2312 Code: 0xEBC7 ==> Row:75 Col:39 */
    {0x80AE,0xB0B9}, /* 肮: GB2312 Code: 0xB0B9 ==> Row:16 Col:25 */
    {0x80AF,0xBFCF}, /* 肯: GB2312 Code: 0xBFCF ==> Row:31 Col:47 */
    {0x80B1,0xEBC5}, /* 肱: GB2312 Code: 0xEBC5 ==> Row:75 Col:37 */
    {0x80B2,0xD3FD}, /* 育: GB2312 Code: 0xD3FD ==> Row:51 Col:93 */
    {0x80B4,0xEBC8}, /* 肴: GB2312 Code: 0xEBC8 ==> Row:75 Col:40 */
    {0x80B7,0xEBC9}, /* 肷: GB2312 Code: 0xEBC9 ==> Row:75 Col:41 */
    {0x80BA,0xB7CE}, /* 肺: GB2312 Code: 0xB7CE ==> Row:23 Col:46 */
    {0x80BC,0xEBC2}, /* 肼: GB2312 Code: 0xEBC2 ==> Row:75 Col:34 */
    {0x80BD,0xEBC4}, /* 肽: GB2312 Code: 0xEBC4 ==> Row:75 Col:36 */
    {0x80BE,0xC9F6}, /* 肾: GB2312 Code: 0xC9F6 ==> Row:41 Col:86 */
    {0x80BF,0xD6D7}, /* 肿: GB2312 Code: 0xD6D7 ==> Row:54 Col:55 */
    {0x80C0,0xD5CD}, /* 胀: GB2312 Code: 0xD5CD ==> Row:53 Col:45 */
    {0x80C1,0xD0B2}, /* 胁: GB2312 Code: 0xD0B2 ==> Row:48 Col:18 */
    {0x80C2,0xEBCF}, /* 胂: GB2312 Code: 0xEBCF ==> Row:75 Col:47 */
    {0x80C3,0xCEB8}, /* 胃: GB2312 Code: 0xCEB8 ==> Row:46 Col:24 */
    {0x80C4,0xEBD0}, /* 胄: GB2312 Code: 0xEBD0 ==> Row:75 Col:48 */
    {0x80C6,0xB5A8}, /* 胆: GB2312 Code: 0xB5A8 ==> Row:21 Col:08 */
    {0x80CC,0xB1B3}, /* 背: GB2312 Code: 0xB1B3 ==> Row:17 Col:19 */
    {0x80CD,0xEBD2}, /* 胍: GB2312 Code: 0xEBD2 ==> Row:75 Col:50 */
    {0x80CE,0xCCA5}, /* 胎: GB2312 Code: 0xCCA5 ==> Row:44 Col:05 */
    {0x80D6,0xC5D6}, /* 胖: GB2312 Code: 0xC5D6 ==> Row:37 Col:54 */
    {0x80D7,0xEBD3}, /* 胗: GB2312 Code: 0xEBD3 ==> Row:75 Col:51 */
    {0x80D9,0xEBD1}, /* 胙: GB2312 Code: 0xEBD1 ==> Row:75 Col:49 */
    {0x80DA,0xC5DF}, /* 胚: GB2312 Code: 0xC5DF ==> Row:37 Col:63 */
    {0x80DB,0xEBCE}, /* 胛: GB2312 Code: 0xEBCE ==> Row:75 Col:46 */
    {0x80DC,0xCAA4}, /* 胜: GB2312 Code: 0xCAA4 ==> Row:42 Col:04 */
    {0x80DD,0xEBD5}, /* 胝: GB2312 Code: 0xEBD5 ==> Row:75 Col:53 */
    {0x80DE,0xB0FB}, /* 胞: GB2312 Code: 0xB0FB ==> Row:16 Col:91 */
    {0x80E1,0xBAFA}, /* 胡: GB2312 Code: 0xBAFA ==> Row:26 Col:90 */
    {0x80E4,0xD8B7}, /* 胤: GB2312 Code: 0xD8B7 ==> Row:56 Col:23 */
    {0x80E5,0xF1E3}, /* 胥: GB2312 Code: 0xF1E3 ==> Row:81 Col:67 */
    {0x80E7,0xEBCA}, /* 胧: GB2312 Code: 0xEBCA ==> Row:75 Col:42 */
    {0x80E8,0xEBCB}, /* 胨: GB2312 Code: 0xEBCB ==> Row:75 Col:43 */
    {0x80E9,0xEBCC}, /* 胩: GB2312 Code: 0xEBCC ==> Row:75 Col:44 */
    {0x80EA,0xEBCD}, /* 胪: GB2312 Code: 0xEBCD ==> Row:75 Col:45 */
    {0x80EB,0xEBD6}, /* 胫: GB2312 Code: 0xEBD6 ==> Row:75 Col:54 */
    {0x80EC,0xE6C0}, /* 胬: GB2312 Code: 0xE6C0 ==> Row:70 Col:32 */
    {0x80ED,0xEBD9}, /* 胭: GB2312 Code: 0xEBD9 ==> Row:75 Col:57 */
    {0x80EF,0xBFE8}, /* 胯: GB2312 Code: 0xBFE8 ==> Row:31 Col:72 */
    {0x80F0,0xD2C8}, /* 胰: GB2312 Code: 0xD2C8 ==> Row:50 Col:40 */
    {0x80F1,0xEBD7}, /* 胱: GB2312 Code: 0xEBD7 ==> Row:75 Col:55 */
    {0x80F2,0xEBDC}, /* 胲: GB2312 Code: 0xEBDC ==> Row:75 Col:60 */
    {0x80F3,0xB8EC}, /* 胳: GB2312 Code: 0xB8EC ==> Row:24 Col:76 */
    {0x80F4,0xEBD8}, /* 胴: GB2312 Code: 0xEBD8 ==> Row:75 Col:56 */
    {0x80F6,0xBDBA}, /* 胶: GB2312 Code: 0xBDBA ==> Row:29 Col:26 */
    {0x80F8,0xD0D8}, /* 胸: GB2312 Code: 0xD0D8 ==> Row:48 Col:56 */
    {0x80FA,0xB0B7}, /* 胺: GB2312 Code: 0xB0B7 ==> Row:16 Col:23 */
    {0x80FC,0xEBDD}, /* 胼: GB2312 Code: 0xEBDD ==> Row:75 Col:61 */
    {0x80FD,0xC4DC}, /* 能: GB2312 Code: 0xC4DC ==> Row:36 Col:60 */
    {0x8102,0xD6AC}, /* 脂: GB2312 Code: 0xD6AC ==> Row:54 Col:12 */
    {0x8106,0xB4E0}, /* 脆: GB2312 Code: 0xB4E0 ==> Row:20 Col:64 */
    {0x8109,0xC2F6}, /* 脉: GB2312 Code: 0xC2F6 ==> Row:34 Col:86 */
    {0x810A,0xBCB9}, /* 脊: GB2312 Code: 0xBCB9 ==> Row:28 Col:25 */
    {0x810D,0xEBDA}, /* 脍: GB2312 Code: 0xEBDA ==> Row:75 Col:58 */
    {0x810E,0xEBDB}, /* 脎: GB2312 Code: 0xEBDB ==> Row:75 Col:59 */
    {0x810F,0xD4E0}, /* 脏: GB2312 Code: 0xD4E0 ==> Row:52 Col:64 */
    {0x8110,0xC6EA}, /* 脐: GB2312 Code: 0xC6EA ==> Row:38 Col:74 */
    {0x8111,0xC4D4}, /* 脑: GB2312 Code: 0xC4D4 ==> Row:36 Col:52 */
    {0x8112,0xEBDF}, /* 脒: GB2312 Code: 0xEBDF ==> Row:75 Col:63 */
    {0x8113,0xC5A7}, /* 脓: GB2312 Code: 0xC5A7 ==> Row:37 Col:07 */
    {0x8114,0xD9F5}, /* 脔: GB2312 Code: 0xD9F5 ==> Row:57 Col:85 */
    {0x8116,0xB2B1}, /* 脖: GB2312 Code: 0xB2B1 ==> Row:18 Col:17 */
    {0x8118,0xEBE4}, /* 脘: GB2312 Code: 0xEBE4 ==> Row:75 Col:68 */
    {0x811A,0xBDC5}, /* 脚: GB2312 Code: 0xBDC5 ==> Row:29 Col:37 */
    {0x811E,0xEBE2}, /* 脞: GB2312 Code: 0xEBE2 ==> Row:75 Col:66 */
    {0x812C,0xEBE3}, /* 脬: GB2312 Code: 0xEBE3 ==> Row:75 Col:67 */
    {0x812F,0xB8AC}, /* 脯: GB2312 Code: 0xB8AC ==> Row:24 Col:12 */
    {0x8131,0xCDD1}, /* 脱: GB2312 Code: 0xCDD1 ==> Row:45 Col:49 */
    {0x8132,0xEBE5}, /* 脲: GB2312 Code: 0xEBE5 ==> Row:75 Col:69 */
    {0x8136,0xEBE1}, /* 脶: GB2312 Code: 0xEBE1 ==> Row:75 Col:65 */
    {0x8138,0xC1B3}, /* 脸: GB2312 Code: 0xC1B3 ==> Row:33 Col:19 */
    {0x813E,0xC6A2}, /* 脾: GB2312 Code: 0xC6A2 ==> Row:38 Col:02 */
    {0x8146,0xCCF3}, /* 腆: GB2312 Code: 0xCCF3 ==> Row:44 Col:83 */
    {0x8148,0xEBE6}, /* 腈: GB2312 Code: 0xEBE6 ==> Row:75 Col:70 */
    {0x814A,0xC0B0}, /* 腊: GB2312 Code: 0xC0B0 ==> Row:32 Col:16 */
    {0x814B,0xD2B8}, /* 腋: GB2312 Code: 0xD2B8 ==> Row:50 Col:24 */
    {0x814C,0xEBE7}, /* 腌: GB2312 Code: 0xEBE7 ==> Row:75 Col:71 */
    {0x8150,0xB8AF}, /* 腐: GB2312 Code: 0xB8AF ==> Row:24 Col:15 */
    {0x8151,0xB8AD}, /* 腑: GB2312 Code: 0xB8AD ==> Row:24 Col:13 */
    {0x8153,0xEBE8}, /* 腓: GB2312 Code: 0xEBE8 ==> Row:75 Col:72 */
    {0x8154,0xC7BB}, /* 腔: GB2312 Code: 0xC7BB ==> Row:39 Col:27 */
    {0x8155,0xCDF3}, /* 腕: GB2312 Code: 0xCDF3 ==> Row:45 Col:83 */
    {0x8159,0xEBEA}, /* 腙: GB2312 Code: 0xEBEA ==> Row:75 Col:74 */
    {0x815A,0xEBEB}, /* 腚: GB2312 Code: 0xEBEB ==> Row:75 Col:75 */
    {0x8160,0xEBED}, /* 腠: GB2312 Code: 0xEBED ==> Row:75 Col:77 */
    {0x8165,0xD0C8}, /* 腥: GB2312 Code: 0xD0C8 ==> Row:48 Col:40 */
    {0x8167,0xEBF2}, /* 腧: GB2312 Code: 0xEBF2 ==> Row:75 Col:82 */
    {0x8169,0xEBEE}, /* 腩: GB2312 Code: 0xEBEE ==> Row:75 Col:78 */
    {0x816D,0xEBF1}, /* 腭: GB2312 Code: 0xEBF1 ==> Row:75 Col:81 */
    {0x816E,0xC8F9}, /* 腮: GB2312 Code: 0xC8F9 ==> Row:40 Col:89 */
    {0x8170,0xD1FC}, /* 腰: GB2312 Code: 0xD1FC ==> Row:49 Col:92 */
    {0x8171,0xEBEC}, /* 腱: GB2312 Code: 0xEBEC ==> Row:75 Col:76 */
    {0x8174,0xEBE9}, /* 腴: GB2312 Code: 0xEBE9 ==> Row:75 Col:73 */
    {0x8179,0xB8B9}, /* 腹: GB2312 Code: 0xB8B9 ==> Row:24 Col:25 */
    {0x817A,0xCFD9}, /* 腺: GB2312 Code: 0xCFD9 ==> Row:47 Col:57 */
    {0x817B,0xC4E5}, /* 腻: GB2312 Code: 0xC4E5 ==> Row:36 Col:69 */
    {0x817C,0xEBEF}, /* 腼: GB2312 Code: 0xEBEF ==> Row:75 Col:79 */
    {0x817D,0xEBF0}, /* 腽: GB2312 Code: 0xEBF0 ==> Row:75 Col:80 */
    {0x817E,0xCCDA}, /* 腾: GB2312 Code: 0xCCDA ==> Row:44 Col:58 */
    {0x817F,0xCDC8}, /* 腿: GB2312 Code: 0xCDC8 ==> Row:45 Col:40 */
    {0x8180,0xB0F2}, /* 膀: GB2312 Code: 0xB0F2 ==> Row:16 Col:82 */
    {0x8182,0xEBF6}, /* 膂: GB2312 Code: 0xEBF6 ==> Row:75 Col:86 */
    {0x8188,0xEBF5}, /* 膈: GB2312 Code: 0xEBF5 ==> Row:75 Col:85 */
    {0x818A,0xB2B2}, /* 膊: GB2312 Code: 0xB2B2 ==> Row:18 Col:18 */
    {0x818F,0xB8E0}, /* 膏: GB2312 Code: 0xB8E0 ==> Row:24 Col:64 */
    {0x8191,0xEBF7}, /* 膑: GB2312 Code: 0xEBF7 ==> Row:75 Col:87 */
    {0x8198,0xB1EC}, /* 膘: GB2312 Code: 0xB1EC ==> Row:17 Col:76 */
    {0x819B,0xCCC5}, /* 膛: GB2312 Code: 0xCCC5 ==> Row:44 Col:37 */
    {0x819C,0xC4A4}, /* 膜: GB2312 Code: 0xC4A4 ==> Row:36 Col:04 */
    {0x819D,0xCFA5}, /* 膝: GB2312 Code: 0xCFA5 ==> Row:47 Col:05 */
    {0x81A3,0xEBF9}, /* 膣: GB2312 Code: 0xEBF9 ==> Row:75 Col:89 */
    {0x81A6,0xECA2}, /* 膦: GB2312 Code: 0xECA2 ==> Row:76 Col:02 */
    {0x81A8,0xC5F2}, /* 膨: GB2312 Code: 0xC5F2 ==> Row:37 Col:82 */
    {0x81AA,0xEBFA}, /* 膪: GB2312 Code: 0xEBFA ==> Row:75 Col:90 */
    {0x81B3,0xC9C5}, /* 膳: GB2312 Code: 0xC9C5 ==> Row:41 Col:37 */
    {0x81BA,0xE2DF}, /* 膺: GB2312 Code: 0xE2DF ==> Row:66 Col:63 */
    {0x81BB,0xEBFE}, /* 膻: GB2312 Code: 0xEBFE ==> Row:75 Col:94 */
    {0x81C0,0xCDCE}, /* 臀: GB2312 Code: 0xCDCE ==> Row:45 Col:46 */
    {0x81C1,0xECA1}, /* 臁: GB2312 Code: 0xECA1 ==> Row:76 Col:01 */
    {0x81C2,0xB1DB}, /* 臂: GB2312 Code: 0xB1DB ==> Row:17 Col:59 */
    {0x81C3,0xD3B7}, /* 臃: GB2312 Code: 0xD3B7 ==> Row:51 Col:23 */
    {0x81C6,0xD2DC}, /* 臆: GB2312 Code: 0xD2DC ==> Row:50 Col:60 */
    {0x81CA,0xEBFD}, /* 臊: GB2312 Code: 0xEBFD ==> Row:75 Col:93 */
    {0x81CC,0xEBFB}, /* 臌: GB2312 Code: 0xEBFB ==> Row:75 Col:91 */
    {0x81E3,0xB3BC}, /* 臣: GB2312 Code: 0xB3BC ==> Row:19 Col:28 */
    {0x81E7,0xEAB0}, /* 臧: GB2312 Code: 0xEAB0 ==> Row:74 Col:16 */
    {0x81EA,0xD7D4}, /* 自: GB2312 Code: 0xD7D4 ==> Row:55 Col:52 */
    {0x81EC,0xF4AB}, /* 臬: GB2312 Code: 0xF4AB ==> Row:84 Col:11 */
    {0x81ED,0xB3F4}, /* 臭: GB2312 Code: 0xB3F4 ==> Row:19 Col:84 */
    {0x81F3,0xD6C1}, /* 至: GB2312 Code: 0xD6C1 ==> Row:54 Col:33 */
    {0x81F4,0xD6C2}, /* 致: GB2312 Code: 0xD6C2 ==> Row:54 Col:34 */
    {0x81FB,0xD5E9}, /* 臻: GB2312 Code: 0xD5E9 ==> Row:53 Col:73 */
    {0x81FC,0xBECA}, /* 臼: GB2312 Code: 0xBECA ==> Row:30 Col:42 */
    {0x81FE,0xF4A7}, /* 臾: GB2312 Code: 0xF4A7 ==> Row:84 Col:07 */
    {0x8200,0xD2A8}, /* 舀: GB2312 Code: 0xD2A8 ==> Row:50 Col:08 */
    {0x8201,0xF4A8}, /* 舁: GB2312 Code: 0xF4A8 ==> Row:84 Col:08 */
    {0x8202,0xF4A9}, /* 舂: GB2312 Code: 0xF4A9 ==> Row:84 Col:09 */
    {0x8204,0xF4AA}, /* 舄: GB2312 Code: 0xF4AA ==> Row:84 Col:10 */
    {0x8205,0xBECB}, /* 舅: GB2312 Code: 0xBECB ==> Row:30 Col:43 */
    {0x8206,0xD3DF}, /* 舆: GB2312 Code: 0xD3DF ==> Row:51 Col:63 */
    {0x820C,0xC9E0}, /* 舌: GB2312 Code: 0xC9E0 ==> Row:41 Col:64 */
    {0x820D,0xC9E1}, /* 舍: GB2312 Code: 0xC9E1 ==> Row:41 Col:65 */
    {0x8210,0xF3C2}, /* 舐: GB2312 Code: 0xF3C2 ==> Row:83 Col:34 */
    {0x8212,0xCAE6}, /* 舒: GB2312 Code: 0xCAE6 ==> Row:42 Col:70 */
    {0x8214,0xCCF2}, /* 舔: GB2312 Code: 0xCCF2 ==> Row:44 Col:82 */
    {0x821B,0xE2B6}, /* 舛: GB2312 Code: 0xE2B6 ==> Row:66 Col:22 */
    {0x821C,0xCBB4}, /* 舜: GB2312 Code: 0xCBB4 ==> Row:43 Col:20 */
    {0x821E,0xCEE8}, /* 舞: GB2312 Code: 0xCEE8 ==> Row:46 Col:72 */
    {0x821F,0xD6DB}, /* 舟: GB2312 Code: 0xD6DB ==> Row:54 Col:59 */
    {0x8221,0xF4AD}, /* 舡: GB2312 Code: 0xF4AD ==> Row:84 Col:13 */
    {0x8222,0xF4AE}, /* 舢: GB2312 Code: 0xF4AE ==> Row:84 Col:14 */
    {0x8223,0xF4AF}, /* 舣: GB2312 Code: 0xF4AF ==> Row:84 Col:15 */
    {0x8228,0xF4B2}, /* 舨: GB2312 Code: 0xF4B2 ==> Row:84 Col:18 */
    {0x822A,0xBABD}, /* 航: GB2312 Code: 0xBABD ==> Row:26 Col:29 */
    {0x822B,0xF4B3}, /* 舫: GB2312 Code: 0xF4B3 ==> Row:84 Col:19 */
    {0x822C,0xB0E3}, /* 般: GB2312 Code: 0xB0E3 ==> Row:16 Col:67 */
    {0x822D,0xF4B0}, /* 舭: GB2312 Code: 0xF4B0 ==> Row:84 Col:16 */
    {0x822F,0xF4B1}, /* 舯: GB2312 Code: 0xF4B1 ==> Row:84 Col:17 */
    {0x8230,0xBDA2}, /* 舰: GB2312 Code: 0xBDA2 ==> Row:29 Col:02 */
    {0x8231,0xB2D5}, /* 舱: GB2312 Code: 0xB2D5 ==> Row:18 Col:53 */
    {0x8233,0xF4B6}, /* 舳: GB2312 Code: 0xF4B6 ==> Row:84 Col:22 */
    {0x8234,0xF4B7}, /* 舴: GB2312 Code: 0xF4B7 ==> Row:84 Col:23 */
    {0x8235,0xB6E6}, /* 舵: GB2312 Code: 0xB6E6 ==> Row:22 Col:70 */
    {0x8236,0xB2B0}, /* 舶: GB2312 Code: 0xB2B0 ==> Row:18 Col:16 */
    {0x8237,0xCFCF}, /* 舷: GB2312 Code: 0xCFCF ==> Row:47 Col:47 */
    {0x8238,0xF4B4}, /* 舸: GB2312 Code: 0xF4B4 ==> Row:84 Col:20 */
    {0x8239,0xB4AC}, /* 船: GB2312 Code: 0xB4AC ==> Row:20 Col:12 */
    {0x823B,0xF4B5}, /* 舻: GB2312 Code: 0xF4B5 ==> Row:84 Col:21 */
    {0x823E,0xF4B8}, /* 舾: GB2312 Code: 0xF4B8 ==> Row:84 Col:24 */
    {0x8244,0xF4B9}, /* 艄: GB2312 Code: 0xF4B9 ==> Row:84 Col:25 */
    {0x8247,0xCDA7}, /* 艇: GB2312 Code: 0xCDA7 ==> Row:45 Col:07 */
    {0x8249,0xF4BA}, /* 艉: GB2312 Code: 0xF4BA ==> Row:84 Col:26 */
    {0x824B,0xF4BB}, /* 艋: GB2312 Code: 0xF4BB ==> Row:84 Col:27 */
    {0x824F,0xF4BC}, /* 艏: GB2312 Code: 0xF4BC ==> Row:84 Col:28 */
    {0x8258,0xCBD2}, /* 艘: GB2312 Code: 0xCBD2 ==> Row:43 Col:50 */
    {0x825A,0xF4BD}, /* 艚: GB2312 Code: 0xF4BD ==> Row:84 Col:29 */
    {0x825F,0xF4BE}, /* 艟: GB2312 Code: 0xF4BE ==> Row:84 Col:30 */
    {0x8268,0xF4BF}, /* 艨: GB2312 Code: 0xF4BF ==> Row:84 Col:31 */
    {0x826E,0xF4DE}, /* 艮: GB2312 Code: 0xF4DE ==> Row:84 Col:62 */
    {0x826F,0xC1BC}, /* 良: GB2312 Code: 0xC1BC ==> Row:33 Col:28 */
    {0x8270,0xBCE8}, /* 艰: GB2312 Code: 0xBCE8 ==> Row:28 Col:72 */
    {0x8272,0xC9AB}, /* 色: GB2312 Code: 0xC9AB ==> Row:41 Col:11 */
    {0x8273,0xD1DE}, /* 艳: GB2312 Code: 0xD1DE ==> Row:49 Col:62 */
    {0x8274,0xE5F5}, /* 艴: GB2312 Code: 0xE5F5 ==> Row:69 Col:85 */
    {0x8279,0xDCB3}, /* 艹: GB2312 Code: 0xDCB3 ==> Row:60 Col:19 */
    {0x827A,0xD2D5}, /* 艺: GB2312 Code: 0xD2D5 ==> Row:50 Col:53 */
    {0x827D,0xDCB4}, /* 艽: GB2312 Code: 0xDCB4 ==> Row:60 Col:20 */
    {0x827E,0xB0AC}, /* 艾: GB2312 Code: 0xB0AC ==> Row:16 Col:12 */
    {0x827F,0xDCB5}, /* 艿: GB2312 Code: 0xDCB5 ==> Row:60 Col:21 */
    {0x8282,0xBDDA}, /* 节: GB2312 Code: 0xBDDA ==> Row:29 Col:58 */
    {0x8284,0xDCB9}, /* 芄: GB2312 Code: 0xDCB9 ==> Row:60 Col:25 */
    {0x8288,0xD8C2}, /* 芈: GB2312 Code: 0xD8C2 ==> Row:56 Col:34 */
    {0x828A,0xDCB7}, /* 芊: GB2312 Code: 0xDCB7 ==> Row:60 Col:23 */
    {0x828B,0xD3F3}, /* 芋: GB2312 Code: 0xD3F3 ==> Row:51 Col:83 */
    {0x828D,0xC9D6}, /* 芍: GB2312 Code: 0xC9D6 ==> Row:41 Col:54 */
    {0x828E,0xDCBA}, /* 芎: GB2312 Code: 0xDCBA ==> Row:60 Col:26 */
    {0x828F,0xDCB6}, /* 芏: GB2312 Code: 0xDCB6 ==> Row:60 Col:22 */
    {0x8291,0xDCBB}, /* 芑: GB2312 Code: 0xDCBB ==> Row:60 Col:27 */
    {0x8292,0xC3A2}, /* 芒: GB2312 Code: 0xC3A2 ==> Row:35 Col:02 */
    {0x8297,0xDCBC}, /* 芗: GB2312 Code: 0xDCBC ==> Row:60 Col:28 */
    {0x8298,0xDCC5}, /* 芘: GB2312 Code: 0xDCC5 ==> Row:60 Col:37 */
    {0x8299,0xDCBD}, /* 芙: GB2312 Code: 0xDCBD ==> Row:60 Col:29 */
    {0x829C,0xCEDF}, /* 芜: GB2312 Code: 0xCEDF ==> Row:46 Col:63 */
    {0x829D,0xD6A5}, /* 芝: GB2312 Code: 0xD6A5 ==> Row:54 Col:05 */
    {0x829F,0xDCCF}, /* 芟: GB2312 Code: 0xDCCF ==> Row:60 Col:47 */
    {0x82A1,0xDCCD}, /* 芡: GB2312 Code: 0xDCCD ==> Row:60 Col:45 */
    {0x82A4,0xDCD2}, /* 芤: GB2312 Code: 0xDCD2 ==> Row:60 Col:50 */
    {0x82A5,0xBDE6}, /* 芥: GB2312 Code: 0xBDE6 ==> Row:29 Col:70 */
    {0x82A6,0xC2AB}, /* 芦: GB2312 Code: 0xC2AB ==> Row:34 Col:11 */
    {0x82A8,0xDCB8}, /* 芨: GB2312 Code: 0xDCB8 ==> Row:60 Col:24 */
    {0x82A9,0xDCCB}, /* 芩: GB2312 Code: 0xDCCB ==> Row:60 Col:43 */
    {0x82AA,0xDCCE}, /* 芪: GB2312 Code: 0xDCCE ==> Row:60 Col:46 */
    {0x82AB,0xDCBE}, /* 芫: GB2312 Code: 0xDCBE ==> Row:60 Col:30 */
    {0x82AC,0xB7D2}, /* 芬: GB2312 Code: 0xB7D2 ==> Row:23 Col:50 */
    {0x82AD,0xB0C5}, /* 芭: GB2312 Code: 0xB0C5 ==> Row:16 Col:37 */
    {0x82AE,0xDCC7}, /* 芮: GB2312 Code: 0xDCC7 ==> Row:60 Col:39 */
    {0x82AF,0xD0BE}, /* 芯: GB2312 Code: 0xD0BE ==> Row:48 Col:30 */
    {0x82B0,0xDCC1}, /* 芰: GB2312 Code: 0xDCC1 ==> Row:60 Col:33 */
    {0x82B1,0xBBA8}, /* 花: GB2312 Code: 0xBBA8 ==> Row:27 Col:08 */
    {0x82B3,0xB7BC}, /* 芳: GB2312 Code: 0xB7BC ==> Row:23 Col:28 */
    {0x82B4,0xDCCC}, /* 芴: GB2312 Code: 0xDCCC ==> Row:60 Col:44 */
    {0x82B7,0xDCC6}, /* 芷: GB2312 Code: 0xDCC6 ==> Row:60 Col:38 */
    {0x82B8,0xDCBF}, /* 芸: GB2312 Code: 0xDCBF ==> Row:60 Col:31 */
    {0x82B9,0xC7DB}, /* 芹: GB2312 Code: 0xC7DB ==> Row:39 Col:59 */
    {0x82BD,0xD1BF}, /* 芽: GB2312 Code: 0xD1BF ==> Row:49 Col:31 */
    {0x82BE,0xDCC0}, /* 芾: GB2312 Code: 0xDCC0 ==> Row:60 Col:32 */
    {0x82C1,0xDCCA}, /* 苁: GB2312 Code: 0xDCCA ==> Row:60 Col:42 */
    {0x82C4,0xDCD0}, /* 苄: GB2312 Code: 0xDCD0 ==> Row:60 Col:48 */
    {0x82C7,0xCEAD}, /* 苇: GB2312 Code: 0xCEAD ==> Row:46 Col:13 */
    {0x82C8,0xDCC2}, /* 苈: GB2312 Code: 0xDCC2 ==> Row:60 Col:34 */
    {0x82CA,0xDCC3}, /* 苊: GB2312 Code: 0xDCC3 ==> Row:60 Col:35 */
    {0x82CB,0xDCC8}, /* 苋: GB2312 Code: 0xDCC8 ==> Row:60 Col:40 */
    {0x82CC,0xDCC9}, /* 苌: GB2312 Code: 0xDCC9 ==> Row:60 Col:41 */
    {0x82CD,0xB2D4}, /* 苍: GB2312 Code: 0xB2D4 ==> Row:18 Col:52 */
    {0x82CE,0xDCD1}, /* 苎: GB2312 Code: 0xDCD1 ==> Row:60 Col:49 */
    {0x82CF,0xCBD5}, /* 苏: GB2312 Code: 0xCBD5 ==> Row:43 Col:53 */
    {0x82D1,0xD4B7}, /* 苑: GB2312 Code: 0xD4B7 ==> Row:52 Col:23 */
    {0x82D2,0xDCDB}, /* 苒: GB2312 Code: 0xDCDB ==> Row:60 Col:59 */
    {0x82D3,0xDCDF}, /* 苓: GB2312 Code: 0xDCDF ==> Row:60 Col:63 */
    {0x82D4,0xCCA6}, /* 苔: GB2312 Code: 0xCCA6 ==> Row:44 Col:06 */
    {0x82D5,0xDCE6}, /* 苕: GB2312 Code: 0xDCE6 ==> Row:60 Col:70 */
    {0x82D7,0xC3E7}, /* 苗: GB2312 Code: 0xC3E7 ==> Row:35 Col:71 */
    {0x82D8,0xDCDC}, /* 苘: GB2312 Code: 0xDCDC ==> Row:60 Col:60 */
    {0x82DB,0xBFC1}, /* 苛: GB2312 Code: 0xBFC1 ==> Row:31 Col:33 */
    {0x82DC,0xDCD9}, /* 苜: GB2312 Code: 0xDCD9 ==> Row:60 Col:57 */
    {0x82DE,0xB0FA}, /* 苞: GB2312 Code: 0xB0FA ==> Row:16 Col:90 */
    {0x82DF,0xB9B6}, /* 苟: GB2312 Code: 0xB9B6 ==> Row:25 Col:22 */
    {0x82E0,0xDCE5}, /* 苠: GB2312 Code: 0xDCE5 ==> Row:60 Col:69 */
    {0x82E1,0xDCD3}, /* 苡: GB2312 Code: 0xDCD3 ==> Row:60 Col:51 */
    {0x82E3,0xDCC4}, /* 苣: GB2312 Code: 0xDCC4 ==> Row:60 Col:36 */
    {0x82E4,0xDCD6}, /* 苤: GB2312 Code: 0xDCD6 ==> Row:60 Col:54 */
    {0x82E5,0xC8F4}, /* 若: GB2312 Code: 0xC8F4 ==> Row:40 Col:84 */
    {0x82E6,0xBFE0}, /* 苦: GB2312 Code: 0xBFE0 ==> Row:31 Col:64 */
    {0x82EB,0xC9BB}, /* 苫: GB2312 Code: 0xC9BB ==> Row:41 Col:27 */
    {0x82EF,0xB1BD}, /* 苯: GB2312 Code: 0xB1BD ==> Row:17 Col:29 */
    {0x82F1,0xD3A2}, /* 英: GB2312 Code: 0xD3A2 ==> Row:51 Col:02 */
    {0x82F4,0xDCDA}, /* 苴: GB2312 Code: 0xDCDA ==> Row:60 Col:58 */
    {0x82F7,0xDCD5}, /* 苷: GB2312 Code: 0xDCD5 ==> Row:60 Col:53 */
    {0x82F9,0xC6BB}, /* 苹: GB2312 Code: 0xC6BB ==> Row:38 Col:27 */
    {0x82FB,0xDCDE}, /* 苻: GB2312 Code: 0xDCDE ==> Row:60 Col:62 */
    {0x8301,0xD7C2}, /* 茁: GB2312 Code: 0xD7C2 ==> Row:55 Col:34 */
    {0x8302,0xC3AF}, /* 茂: GB2312 Code: 0xC3AF ==> Row:35 Col:15 */
    {0x8303,0xB7B6}, /* 范: GB2312 Code: 0xB7B6 ==> Row:23 Col:22 */
    {0x8304,0xC7D1}, /* 茄: GB2312 Code: 0xC7D1 ==> Row:39 Col:49 */
    {0x8305,0xC3A9}, /* 茅: GB2312 Code: 0xC3A9 ==> Row:35 Col:09 */
    {0x8306,0xDCE2}, /* 茆: GB2312 Code: 0xDCE2 ==> Row:60 Col:66 */
    {0x8307,0xDCD8}, /* 茇: GB2312 Code: 0xDCD8 ==> Row:60 Col:56 */
    {0x8308,0xDCEB}, /* 茈: GB2312 Code: 0xDCEB ==> Row:60 Col:75 */
    {0x8309,0xDCD4}, /* 茉: GB2312 Code: 0xDCD4 ==> Row:60 Col:52 */
    {0x830C,0xDCDD}, /* 茌: GB2312 Code: 0xDCDD ==> Row:60 Col:61 */
    {0x830E,0xBEA5}, /* 茎: GB2312 Code: 0xBEA5 ==> Row:30 Col:05 */
    {0x830F,0xDCD7}, /* 茏: GB2312 Code: 0xDCD7 ==> Row:60 Col:55 */
    {0x8311,0xDCE0}, /* 茑: GB2312 Code: 0xDCE0 ==> Row:60 Col:64 */
    {0x8314,0xDCE3}, /* 茔: GB2312 Code: 0xDCE3 ==> Row:60 Col:67 */
    {0x8315,0xDCE4}, /* 茕: GB2312 Code: 0xDCE4 ==> Row:60 Col:68 */
    {0x8317,0xDCF8}, /* 茗: GB2312 Code: 0xDCF8 ==> Row:60 Col:88 */
    {0x831A,0xDCE1}, /* 茚: GB2312 Code: 0xDCE1 ==> Row:60 Col:65 */
    {0x831B,0xDDA2}, /* 茛: GB2312 Code: 0xDDA2 ==> Row:61 Col:02 */
    {0x831C,0xDCE7}, /* 茜: GB2312 Code: 0xDCE7 ==> Row:60 Col:71 */
    {0x8327,0xBCEB}, /* 茧: GB2312 Code: 0xBCEB ==> Row:28 Col:75 */
    {0x8328,0xB4C4}, /* 茨: GB2312 Code: 0xB4C4 ==> Row:20 Col:36 */
    {0x832B,0xC3A3}, /* 茫: GB2312 Code: 0xC3A3 ==> Row:35 Col:03 */
    {0x832C,0xB2E7}, /* 茬: GB2312 Code: 0xB2E7 ==> Row:18 Col:71 */
    {0x832D,0xDCFA}, /* 茭: GB2312 Code: 0xDCFA ==> Row:60 Col:90 */
    {0x832F,0xDCF2}, /* 茯: GB2312 Code: 0xDCF2 ==> Row:60 Col:82 */
    {0x8331,0xDCEF}, /* 茱: GB2312 Code: 0xDCEF ==> Row:60 Col:79 */
    {0x8333,0xDCFC}, /* 茳: GB2312 Code: 0xDCFC ==> Row:60 Col:92 */
    {0x8334,0xDCEE}, /* 茴: GB2312 Code: 0xDCEE ==> Row:60 Col:78 */
    {0x8335,0xD2F0}, /* 茵: GB2312 Code: 0xD2F0 ==> Row:50 Col:80 */
    {0x8336,0xB2E8}, /* 茶: GB2312 Code: 0xB2E8 ==> Row:18 Col:72 */
    {0x8338,0xC8D7}, /* 茸: GB2312 Code: 0xC8D7 ==> Row:40 Col:55 */
    {0x8339,0xC8E3}, /* 茹: GB2312 Code: 0xC8E3 ==> Row:40 Col:67 */
    {0x833A,0xDCFB}, /* 茺: GB2312 Code: 0xDCFB ==> Row:60 Col:91 */
    {0x833C,0xDCED}, /* 茼: GB2312 Code: 0xDCED ==> Row:60 Col:77 */
    {0x8340,0xDCF7}, /* 荀: GB2312 Code: 0xDCF7 ==> Row:60 Col:87 */
    {0x8343,0xDCF5}, /* 荃: GB2312 Code: 0xDCF5 ==> Row:60 Col:85 */
    {0x8346,0xBEA3}, /* 荆: GB2312 Code: 0xBEA3 ==> Row:30 Col:03 */
    {0x8347,0xDCF4}, /* 荇: GB2312 Code: 0xDCF4 ==> Row:60 Col:84 */
    {0x8349,0xB2DD}, /* 草: GB2312 Code: 0xB2DD ==> Row:18 Col:61 */
    {0x834F,0xDCF3}, /* 荏: GB2312 Code: 0xDCF3 ==> Row:60 Col:83 */
    {0x8350,0xBCF6}, /* 荐: GB2312 Code: 0xBCF6 ==> Row:28 Col:86 */
    {0x8351,0xDCE8}, /* 荑: GB2312 Code: 0xDCE8 ==> Row:60 Col:72 */
    {0x8352,0xBBC4}, /* 荒: GB2312 Code: 0xBBC4 ==> Row:27 Col:36 */
    {0x8354,0xC0F3}, /* 荔: GB2312 Code: 0xC0F3 ==> Row:32 Col:83 */
    {0x835A,0xBCD4}, /* 荚: GB2312 Code: 0xBCD4 ==> Row:28 Col:52 */
    {0x835B,0xDCE9}, /* 荛: GB2312 Code: 0xDCE9 ==> Row:60 Col:73 */
    {0x835C,0xDCEA}, /* 荜: GB2312 Code: 0xDCEA ==> Row:60 Col:74 */
    {0x835E,0xDCF1}, /* 荞: GB2312 Code: 0xDCF1 ==> Row:60 Col:81 */
    {0x835F,0xDCF6}, /* 荟: GB2312 Code: 0xDCF6 ==> Row:60 Col:86 */
    {0x8360,0xDCF9}, /* 荠: GB2312 Code: 0xDCF9 ==> Row:60 Col:89 */
    {0x8361,0xB5B4}, /* 荡: GB2312 Code: 0xB5B4 ==> Row:21 Col:20 */
    {0x8363,0xC8D9}, /* 荣: GB2312 Code: 0xC8D9 ==> Row:40 Col:57 */
    {0x8364,0xBBE7}, /* 荤: GB2312 Code: 0xBBE7 ==> Row:27 Col:71 */
    {0x8365,0xDCFE}, /* 荥: GB2312 Code: 0xDCFE ==> Row:60 Col:94 */
    {0x8366,0xDCFD}, /* 荦: GB2312 Code: 0xDCFD ==> Row:60 Col:93 */
    {0x8367,0xD3AB}, /* 荧: GB2312 Code: 0xD3AB ==> Row:51 Col:11 */
    {0x8368,0xDDA1}, /* 荨: GB2312 Code: 0xDDA1 ==> Row:61 Col:01 */
    {0x8369,0xDDA3}, /* 荩: GB2312 Code: 0xDDA3 ==> Row:61 Col:03 */
    {0x836A,0xDDA5}, /* 荪: GB2312 Code: 0xDDA5 ==> Row:61 Col:05 */
    {0x836B,0xD2F1}, /* 荫: GB2312 Code: 0xD2F1 ==> Row:50 Col:81 */
    {0x836C,0xDDA4}, /* 荬: GB2312 Code: 0xDDA4 ==> Row:61 Col:04 */
    {0x836D,0xDDA6}, /* 荭: GB2312 Code: 0xDDA6 ==> Row:61 Col:06 */
    {0x836E,0xDDA7}, /* 荮: GB2312 Code: 0xDDA7 ==> Row:61 Col:07 */
    {0x836F,0xD2A9}, /* 药: GB2312 Code: 0xD2A9 ==> Row:50 Col:09 */
    {0x8377,0xBAC9}, /* 荷: GB2312 Code: 0xBAC9 ==> Row:26 Col:41 */
    {0x8378,0xDDA9}, /* 荸: GB2312 Code: 0xDDA9 ==> Row:61 Col:09 */
    {0x837B,0xDDB6}, /* 荻: GB2312 Code: 0xDDB6 ==> Row:61 Col:22 */
    {0x837C,0xDDB1}, /* 荼: GB2312 Code: 0xDDB1 ==> Row:61 Col:17 */
    {0x837D,0xDDB4}, /* 荽: GB2312 Code: 0xDDB4 ==> Row:61 Col:20 */
    {0x8385,0xDDB0}, /* 莅: GB2312 Code: 0xDDB0 ==> Row:61 Col:16 */
    {0x8386,0xC6CE}, /* 莆: GB2312 Code: 0xC6CE ==> Row:38 Col:46 */
    {0x8389,0xC0F2}, /* 莉: GB2312 Code: 0xC0F2 ==> Row:32 Col:82 */
    {0x838E,0xC9AF}, /* 莎: GB2312 Code: 0xC9AF ==> Row:41 Col:15 */
    {0x8392,0xDCEC}, /* 莒: GB2312 Code: 0xDCEC ==> Row:60 Col:76 */
    {0x8393,0xDDAE}, /* 莓: GB2312 Code: 0xDDAE ==> Row:61 Col:14 */
    {0x8398,0xDDB7}, /* 莘: GB2312 Code: 0xDDB7 ==> Row:61 Col:23 */
    {0x839B,0xDCF0}, /* 莛: GB2312 Code: 0xDCF0 ==> Row:60 Col:80 */
    {0x839C,0xDDAF}, /* 莜: GB2312 Code: 0xDDAF ==> Row:61 Col:15 */
    {0x839E,0xDDB8}, /* 莞: GB2312 Code: 0xDDB8 ==> Row:61 Col:24 */
    {0x83A0,0xDDAC}, /* 莠: GB2312 Code: 0xDDAC ==> Row:61 Col:12 */
    {0x83A8,0xDDB9}, /* 莨: GB2312 Code: 0xDDB9 ==> Row:61 Col:25 */
    {0x83A9,0xDDB3}, /* 莩: GB2312 Code: 0xDDB3 ==> Row:61 Col:19 */
    {0x83AA,0xDDAD}, /* 莪: GB2312 Code: 0xDDAD ==> Row:61 Col:13 */
    {0x83AB,0xC4AA}, /* 莫: GB2312 Code: 0xC4AA ==> Row:36 Col:10 */
    {0x83B0,0xDDA8}, /* 莰: GB2312 Code: 0xDDA8 ==> Row:61 Col:08 */
    {0x83B1,0xC0B3}, /* 莱: GB2312 Code: 0xC0B3 ==> Row:32 Col:19 */
    {0x83B2,0xC1AB}, /* 莲: GB2312 Code: 0xC1AB ==> Row:33 Col:11 */
    {0x83B3,0xDDAA}, /* 莳: GB2312 Code: 0xDDAA ==> Row:61 Col:10 */
    {0x83B4,0xDDAB}, /* 莴: GB2312 Code: 0xDDAB ==> Row:61 Col:11 */
    {0x83B6,0xDDB2}, /* 莶: GB2312 Code: 0xDDB2 ==> Row:61 Col:18 */
    {0x83B7,0xBBF1}, /* 获: GB2312 Code: 0xBBF1 ==> Row:27 Col:81 */
    {0x83B8,0xDDB5}, /* 莸: GB2312 Code: 0xDDB5 ==> Row:61 Col:21 */
    {0x83B9,0xD3A8}, /* 莹: GB2312 Code: 0xD3A8 ==> Row:51 Col:08 */
    {0x83BA,0xDDBA}, /* 莺: GB2312 Code: 0xDDBA ==> Row:61 Col:26 */
    {0x83BC,0xDDBB}, /* 莼: GB2312 Code: 0xDDBB ==> Row:61 Col:27 */
    {0x83BD,0xC3A7}, /* 莽: GB2312 Code: 0xC3A7 ==> Row:35 Col:07 */
    {0x83C0,0xDDD2}, /* 菀: GB2312 Code: 0xDDD2 ==> Row:61 Col:50 */
    {0x83C1,0xDDBC}, /* 菁: GB2312 Code: 0xDDBC ==> Row:61 Col:28 */
    {0x83C5,0xDDD1}, /* 菅: GB2312 Code: 0xDDD1 ==> Row:61 Col:49 */
    {0x83C7,0xB9BD}, /* 菇: GB2312 Code: 0xB9BD ==> Row:25 Col:29 */
    {0x83CA,0xBED5}, /* 菊: GB2312 Code: 0xBED5 ==> Row:30 Col:53 */
    {0x83CC,0xBEFA}, /* 菌: GB2312 Code: 0xBEFA ==> Row:30 Col:90 */
    {0x83CF,0xBACA}, /* 菏: GB2312 Code: 0xBACA ==> Row:26 Col:42 */
    {0x83D4,0xDDCA}, /* 菔: GB2312 Code: 0xDDCA ==> Row:61 Col:42 */
    {0x83D6,0xDDC5}, /* 菖: GB2312 Code: 0xDDC5 ==> Row:61 Col:37 */
    {0x83D8,0xDDBF}, /* 菘: GB2312 Code: 0xDDBF ==> Row:61 Col:31 */
    {0x83DC,0xB2CB}, /* 菜: GB2312 Code: 0xB2CB ==> Row:18 Col:43 */
    {0x83DD,0xDDC3}, /* 菝: GB2312 Code: 0xDDC3 ==> Row:61 Col:35 */
    {0x83DF,0xDDCB}, /* 菟: GB2312 Code: 0xDDCB ==> Row:61 Col:43 */
    {0x83E0,0xB2A4}, /* 菠: GB2312 Code: 0xB2A4 ==> Row:18 Col:04 */
    {0x83E1,0xDDD5}, /* 菡: GB2312 Code: 0xDDD5 ==> Row:61 Col:53 */
    {0x83E5,0xDDBE}, /* 菥: GB2312 Code: 0xDDBE ==> Row:61 Col:30 */
    {0x83E9,0xC6D0}, /* 菩: GB2312 Code: 0xC6D0 ==> Row:38 Col:48 */
    {0x83EA,0xDDD0}, /* 菪: GB2312 Code: 0xDDD0 ==> Row:61 Col:48 */
    {0x83F0,0xDDD4}, /* 菰: GB2312 Code: 0xDDD4 ==> Row:61 Col:52 */
    {0x83F1,0xC1E2}, /* 菱: GB2312 Code: 0xC1E2 ==> Row:33 Col:66 */
    {0x83F2,0xB7C6}, /* 菲: GB2312 Code: 0xB7C6 ==> Row:23 Col:38 */
    {0x83F8,0xDDCE}, /* 菸: GB2312 Code: 0xDDCE ==> Row:61 Col:46 */
    {0x83F9,0xDDCF}, /* 菹: GB2312 Code: 0xDDCF ==> Row:61 Col:47 */
    {0x83FD,0xDDC4}, /* 菽: GB2312 Code: 0xDDC4 ==> Row:61 Col:36 */
    {0x8401,0xDDBD}, /* 萁: GB2312 Code: 0xDDBD ==> Row:61 Col:29 */
    {0x8403,0xDDCD}, /* 萃: GB2312 Code: 0xDDCD ==> Row:61 Col:45 */
    {0x8404,0xCCD1}, /* 萄: GB2312 Code: 0xCCD1 ==> Row:44 Col:49 */
    {0x8406,0xDDC9}, /* 萆: GB2312 Code: 0xDDC9 ==> Row:61 Col:41 */
    {0x840B,0xDDC2}, /* 萋: GB2312 Code: 0xDDC2 ==> Row:61 Col:34 */
    {0x840C,0xC3C8}, /* 萌: GB2312 Code: 0xC3C8 ==> Row:35 Col:40 */
    {0x840D,0xC6BC}, /* 萍: GB2312 Code: 0xC6BC ==> Row:38 Col:28 */
    {0x840E,0xCEAE}, /* 萎: GB2312 Code: 0xCEAE ==> Row:46 Col:14 */
    {0x840F,0xDDCC}, /* 萏: GB2312 Code: 0xDDCC ==> Row:61 Col:44 */
    {0x8411,0xDDC8}, /* 萑: GB2312 Code: 0xDDC8 ==> Row:61 Col:40 */
    {0x8418,0xDDC1}, /* 萘: GB2312 Code: 0xDDC1 ==> Row:61 Col:33 */
    {0x841C,0xDDC6}, /* 萜: GB2312 Code: 0xDDC6 ==> Row:61 Col:38 */
    {0x841D,0xC2DC}, /* 萝: GB2312 Code: 0xC2DC ==> Row:34 Col:60 */
    {0x8424,0xD3A9}, /* 萤: GB2312 Code: 0xD3A9 ==> Row:51 Col:09 */
    {0x8425,0xD3AA}, /* 营: GB2312 Code: 0xD3AA ==> Row:51 Col:10 */
    {0x8426,0xDDD3}, /* 萦: GB2312 Code: 0xDDD3 ==> Row:61 Col:51 */
    {0x8427,0xCFF4}, /* 萧: GB2312 Code: 0xCFF4 ==> Row:47 Col:84 */
    {0x8428,0xC8F8}, /* 萨: GB2312 Code: 0xC8F8 ==> Row:40 Col:88 */
    {0x8431,0xDDE6}, /* 萱: GB2312 Code: 0xDDE6 ==> Row:61 Col:70 */
    {0x8438,0xDDC7}, /* 萸: GB2312 Code: 0xDDC7 ==> Row:61 Col:39 */
    {0x843C,0xDDE0}, /* 萼: GB2312 Code: 0xDDE0 ==> Row:61 Col:64 */
    {0x843D,0xC2E4}, /* 落: GB2312 Code: 0xC2E4 ==> Row:34 Col:68 */
    {0x8446,0xDDE1}, /* 葆: GB2312 Code: 0xDDE1 ==> Row:61 Col:65 */
    {0x8451,0xDDD7}, /* 葑: GB2312 Code: 0xDDD7 ==> Row:61 Col:55 */
    {0x8457,0xD6F8}, /* 著: GB2312 Code: 0xD6F8 ==> Row:54 Col:88 */
    {0x8459,0xDDD9}, /* 葙: GB2312 Code: 0xDDD9 ==> Row:61 Col:57 */
    {0x845A,0xDDD8}, /* 葚: GB2312 Code: 0xDDD8 ==> Row:61 Col:56 */
    {0x845B,0xB8F0}, /* 葛: GB2312 Code: 0xB8F0 ==> Row:24 Col:80 */
    {0x845C,0xDDD6}, /* 葜: GB2312 Code: 0xDDD6 ==> Row:61 Col:54 */
    {0x8461,0xC6CF}, /* 葡: GB2312 Code: 0xC6CF ==> Row:38 Col:47 */
    {0x8463,0xB6AD}, /* 董: GB2312 Code: 0xB6AD ==> Row:22 Col:13 */
    {0x8469,0xDDE2}, /* 葩: GB2312 Code: 0xDDE2 ==> Row:61 Col:66 */
    {0x846B,0xBAF9}, /* 葫: GB2312 Code: 0xBAF9 ==> Row:26 Col:89 */
    {0x846C,0xD4E1}, /* 葬: GB2312 Code: 0xD4E1 ==> Row:52 Col:65 */
    {0x846D,0xDDE7}, /* 葭: GB2312 Code: 0xDDE7 ==> Row:61 Col:71 */
    {0x8471,0xB4D0}, /* 葱: GB2312 Code: 0xB4D0 ==> Row:20 Col:48 */
    {0x8473,0xDDDA}, /* 葳: GB2312 Code: 0xDDDA ==> Row:61 Col:58 */
    {0x8475,0xBFFB}, /* 葵: GB2312 Code: 0xBFFB ==> Row:31 Col:91 */
    {0x8476,0xDDE3}, /* 葶: GB2312 Code: 0xDDE3 ==> Row:61 Col:67 */
    {0x8478,0xDDDF}, /* 葸: GB2312 Code: 0xDDDF ==> Row:61 Col:63 */
    {0x847A,0xDDDD}, /* 葺: GB2312 Code: 0xDDDD ==> Row:61 Col:61 */
    {0x8482,0xB5D9}, /* 蒂: GB2312 Code: 0xB5D9 ==> Row:21 Col:57 */
    {0x8487,0xDDDB}, /* 蒇: GB2312 Code: 0xDDDB ==> Row:61 Col:59 */
    {0x8488,0xDDDC}, /* 蒈: GB2312 Code: 0xDDDC ==> Row:61 Col:60 */
    {0x8489,0xDDDE}, /* 蒉: GB2312 Code: 0xDDDE ==> Row:61 Col:62 */
    {0x848B,0xBDAF}, /* 蒋: GB2312 Code: 0xBDAF ==> Row:29 Col:15 */
    {0x848C,0xDDE4}, /* 蒌: GB2312 Code: 0xDDE4 ==> Row:61 Col:68 */
    {0x848E,0xDDE5}, /* 蒎: GB2312 Code: 0xDDE5 ==> Row:61 Col:69 */
    {0x8497,0xDDF5}, /* 蒗: GB2312 Code: 0xDDF5 ==> Row:61 Col:85 */
    {0x8499,0xC3C9}, /* 蒙: GB2312 Code: 0xC3C9 ==> Row:35 Col:41 */
    {0x849C,0xCBE2}, /* 蒜: GB2312 Code: 0xCBE2 ==> Row:43 Col:66 */
    {0x84A1,0xDDF2}, /* 蒡: GB2312 Code: 0xDDF2 ==> Row:61 Col:82 */
    {0x84AF,0xD8E1}, /* 蒯: GB2312 Code: 0xD8E1 ==> Row:56 Col:65 */
    {0x84B2,0xC6D1}, /* 蒲: GB2312 Code: 0xC6D1 ==> Row:38 Col:49 */
    {0x84B4,0xDDF4}, /* 蒴: GB2312 Code: 0xDDF4 ==> Row:61 Col:84 */
    {0x84B8,0xD5F4}, /* 蒸: GB2312 Code: 0xD5F4 ==> Row:53 Col:84 */
    {0x84B9,0xDDF3}, /* 蒹: GB2312 Code: 0xDDF3 ==> Row:61 Col:83 */
    {0x84BA,0xDDF0}, /* 蒺: GB2312 Code: 0xDDF0 ==> Row:61 Col:80 */
    {0x84BD,0xDDEC}, /* 蒽: GB2312 Code: 0xDDEC ==> Row:61 Col:76 */
    {0x84BF,0xDDEF}, /* 蒿: GB2312 Code: 0xDDEF ==> Row:61 Col:79 */
    {0x84C1,0xDDE8}, /* 蓁: GB2312 Code: 0xDDE8 ==> Row:61 Col:72 */
    {0x84C4,0xD0EE}, /* 蓄: GB2312 Code: 0xD0EE ==> Row:48 Col:78 */
    {0x84C9,0xC8D8}, /* 蓉: GB2312 Code: 0xC8D8 ==> Row:40 Col:56 */
    {0x84CA,0xDDEE}, /* 蓊: GB2312 Code: 0xDDEE ==> Row:61 Col:78 */
    {0x84CD,0xDDE9}, /* 蓍: GB2312 Code: 0xDDE9 ==> Row:61 Col:73 */
    {0x84D0,0xDDEA}, /* 蓐: GB2312 Code: 0xDDEA ==> Row:61 Col:74 */
    {0x84D1,0xCBF2}, /* 蓑: GB2312 Code: 0xCBF2 ==> Row:43 Col:82 */
    {0x84D3,0xDDED}, /* 蓓: GB2312 Code: 0xDDED ==> Row:61 Col:77 */
    {0x84D6,0xB1CD}, /* 蓖: GB2312 Code: 0xB1CD ==> Row:17 Col:45 */
    {0x84DD,0xC0B6}, /* 蓝: GB2312 Code: 0xC0B6 ==> Row:32 Col:22 */
    {0x84DF,0xBCBB}, /* 蓟: GB2312 Code: 0xBCBB ==> Row:28 Col:27 */
    {0x84E0,0xDDF1}, /* 蓠: GB2312 Code: 0xDDF1 ==> Row:61 Col:81 */
    {0x84E3,0xDDF7}, /* 蓣: GB2312 Code: 0xDDF7 ==> Row:61 Col:87 */
    {0x84E5,0xDDF6}, /* 蓥: GB2312 Code: 0xDDF6 ==> Row:61 Col:86 */
    {0x84E6,0xDDEB}, /* 蓦: GB2312 Code: 0xDDEB ==> Row:61 Col:75 */
    {0x84EC,0xC5EE}, /* 蓬: GB2312 Code: 0xC5EE ==> Row:37 Col:78 */
    {0x84F0,0xDDFB}, /* 蓰: GB2312 Code: 0xDDFB ==> Row:61 Col:91 */
    {0x84FC,0xDEA4}, /* 蓼: GB2312 Code: 0xDEA4 ==> Row:62 Col:04 */
    {0x84FF,0xDEA3}, /* 蓿: GB2312 Code: 0xDEA3 ==> Row:62 Col:03 */
    {0x850C,0xDDF8}, /* 蔌: GB2312 Code: 0xDDF8 ==> Row:61 Col:88 */
    {0x8511,0xC3EF}, /* 蔑: GB2312 Code: 0xC3EF ==> Row:35 Col:79 */
    {0x8513,0xC2FB}, /* 蔓: GB2312 Code: 0xC2FB ==> Row:34 Col:91 */
    {0x8517,0xD5E1}, /* 蔗: GB2312 Code: 0xD5E1 ==> Row:53 Col:65 */
    {0x851A,0xCEB5}, /* 蔚: GB2312 Code: 0xCEB5 ==> Row:46 Col:21 */
    {0x851F,0xDDFD}, /* 蔟: GB2312 Code: 0xDDFD ==> Row:61 Col:93 */
    {0x8521,0xB2CC}, /* 蔡: GB2312 Code: 0xB2CC ==> Row:18 Col:44 */
    {0x852B,0xC4E8}, /* 蔫: GB2312 Code: 0xC4E8 ==> Row:36 Col:72 */
    {0x852C,0xCADF}, /* 蔬: GB2312 Code: 0xCADF ==> Row:42 Col:63 */
    {0x8537,0xC7BE}, /* 蔷: GB2312 Code: 0xC7BE ==> Row:39 Col:30 */
    {0x8538,0xDDFA}, /* 蔸: GB2312 Code: 0xDDFA ==> Row:61 Col:90 */
    {0x8539,0xDDFC}, /* 蔹: GB2312 Code: 0xDDFC ==> Row:61 Col:92 */
    {0x853A,0xDDFE}, /* 蔺: GB2312 Code: 0xDDFE ==> Row:61 Col:94 */
    {0x853B,0xDEA2}, /* 蔻: GB2312 Code: 0xDEA2 ==> Row:62 Col:02 */
    {0x853C,0xB0AA}, /* 蔼: GB2312 Code: 0xB0AA ==> Row:16 Col:10 */
    {0x853D,0xB1CE}, /* 蔽: GB2312 Code: 0xB1CE ==> Row:17 Col:46 */
    {0x8543,0xDEAC}, /* 蕃: GB2312 Code: 0xDEAC ==> Row:62 Col:12 */
    {0x8548,0xDEA6}, /* 蕈: GB2312 Code: 0xDEA6 ==> Row:62 Col:06 */
    {0x8549,0xBDB6}, /* 蕉: GB2312 Code: 0xBDB6 ==> Row:29 Col:22 */
    {0x854A,0xC8EF}, /* 蕊: GB2312 Code: 0xC8EF ==> Row:40 Col:79 */
    {0x8556,0xDEA1}, /* 蕖: GB2312 Code: 0xDEA1 ==> Row:62 Col:01 */
    {0x8559,0xDEA5}, /* 蕙: GB2312 Code: 0xDEA5 ==> Row:62 Col:05 */
    {0x855E,0xDEA9}, /* 蕞: GB2312 Code: 0xDEA9 ==> Row:62 Col:09 */
    {0x8564,0xDEA8}, /* 蕤: GB2312 Code: 0xDEA8 ==> Row:62 Col:08 */
    {0x8568,0xDEA7}, /* 蕨: GB2312 Code: 0xDEA7 ==> Row:62 Col:07 */
    {0x8572,0xDEAD}, /* 蕲: GB2312 Code: 0xDEAD ==> Row:62 Col:13 */
    {0x8574,0xD4CC}, /* 蕴: GB2312 Code: 0xD4CC ==> Row:52 Col:44 */
    {0x8579,0xDEB3}, /* 蕹: GB2312 Code: 0xDEB3 ==> Row:62 Col:19 */
    {0x857A,0xDEAA}, /* 蕺: GB2312 Code: 0xDEAA ==> Row:62 Col:10 */
    {0x857B,0xDEAE}, /* 蕻: GB2312 Code: 0xDEAE ==> Row:62 Col:14 */
    {0x857E,0xC0D9}, /* 蕾: GB2312 Code: 0xC0D9 ==> Row:32 Col:57 */
    {0x8584,0xB1A1}, /* 薄: GB2312 Code: 0xB1A1 ==> Row:17 Col:01 */
    {0x8585,0xDEB6}, /* 薅: GB2312 Code: 0xDEB6 ==> Row:62 Col:22 */
    {0x8587,0xDEB1}, /* 薇: GB2312 Code: 0xDEB1 ==> Row:62 Col:17 */
    {0x858F,0xDEB2}, /* 薏: GB2312 Code: 0xDEB2 ==> Row:62 Col:18 */
    {0x859B,0xD1A6}, /* 薛: GB2312 Code: 0xD1A6 ==> Row:49 Col:06 */
    {0x859C,0xDEB5}, /* 薜: GB2312 Code: 0xDEB5 ==> Row:62 Col:21 */
    {0x85A4,0xDEAF}, /* 薤: GB2312 Code: 0xDEAF ==> Row:62 Col:15 */
    {0x85A8,0xDEB0}, /* 薨: GB2312 Code: 0xDEB0 ==> Row:62 Col:16 */
    {0x85AA,0xD0BD}, /* 薪: GB2312 Code: 0xD0BD ==> Row:48 Col:29 */
    {0x85AE,0xDEB4}, /* 薮: GB2312 Code: 0xDEB4 ==> Row:62 Col:20 */
    {0x85AF,0xCAED}, /* 薯: GB2312 Code: 0xCAED ==> Row:42 Col:77 */
    {0x85B0,0xDEB9}, /* 薰: GB2312 Code: 0xDEB9 ==> Row:62 Col:25 */
    {0x85B7,0xDEB8}, /* 薷: GB2312 Code: 0xDEB8 ==> Row:62 Col:24 */
    {0x85B9,0xDEB7}, /* 薹: GB2312 Code: 0xDEB7 ==> Row:62 Col:23 */
    {0x85C1,0xDEBB}, /* 藁: GB2312 Code: 0xDEBB ==> Row:62 Col:27 */
    {0x85C9,0xBDE5}, /* 藉: GB2312 Code: 0xBDE5 ==> Row:29 Col:69 */
    {0x85CF,0xB2D8}, /* 藏: GB2312 Code: 0xB2D8 ==> Row:18 Col:56 */
    {0x85D0,0xC3EA}, /* 藐: GB2312 Code: 0xC3EA ==> Row:35 Col:74 */
    {0x85D3,0xDEBA}, /* 藓: GB2312 Code: 0xDEBA ==> Row:62 Col:26 */
    {0x85D5,0xC5BA}, /* 藕: GB2312 Code: 0xC5BA ==> Row:37 Col:26 */
    {0x85DC,0xDEBC}, /* 藜: GB2312 Code: 0xDEBC ==> Row:62 Col:28 */
    {0x85E4,0xCCD9}, /* 藤: GB2312 Code: 0xCCD9 ==> Row:44 Col:57 */
    {0x85E9,0xB7AA}, /* 藩: GB2312 Code: 0xB7AA ==> Row:23 Col:10 */
    {0x85FB,0xD4E5}, /* 藻: GB2312 Code: 0xD4E5 ==> Row:52 Col:69 */
    {0x85FF,0xDEBD}, /* 藿: GB2312 Code: 0xDEBD ==> Row:62 Col:29 */
    {0x8605,0xDEBF}, /* 蘅: GB2312 Code: 0xDEBF ==> Row:62 Col:31 */
    {0x8611,0xC4A2}, /* 蘑: GB2312 Code: 0xC4A2 ==> Row:36 Col:02 */
    {0x8616,0xDEC1}, /* 蘖: GB2312 Code: 0xDEC1 ==> Row:62 Col:33 */
    {0x8627,0xDEBE}, /* 蘧: GB2312 Code: 0xDEBE ==> Row:62 Col:30 */
    {0x8629,0xDEC0}, /* 蘩: GB2312 Code: 0xDEC0 ==> Row:62 Col:32 */
    {0x8638,0xD5BA}, /* 蘸: GB2312 Code: 0xD5BA ==> Row:53 Col:26 */
    {0x863C,0xDEC2}, /* 蘼: GB2312 Code: 0xDEC2 ==> Row:62 Col:34 */
    {0x864D,0xF2AE}, /* 虍: GB2312 Code: 0xF2AE ==> Row:82 Col:14 */
    {0x864E,0xBBA2}, /* 虎: GB2312 Code: 0xBBA2 ==> Row:27 Col:02 */
    {0x864F,0xC2B2}, /* 虏: GB2312 Code: 0xC2B2 ==> Row:34 Col:18 */
    {0x8650,0xC5B0}, /* 虐: GB2312 Code: 0xC5B0 ==> Row:37 Col:16 */
    {0x8651,0xC2C7}, /* 虑: GB2312 Code: 0xC2C7 ==> Row:34 Col:39 */
    {0x8654,0xF2AF}, /* 虔: GB2312 Code: 0xF2AF ==> Row:82 Col:15 */
    {0x865A,0xD0E9}, /* 虚: GB2312 Code: 0xD0E9 ==> Row:48 Col:73 */
    {0x865E,0xD3DD}, /* 虞: GB2312 Code: 0xD3DD ==> Row:51 Col:61 */
    {0x8662,0xEBBD}, /* 虢: GB2312 Code: 0xEBBD ==> Row:75 Col:29 */
    {0x866B,0xB3E6}, /* 虫: GB2312 Code: 0xB3E6 ==> Row:19 Col:70 */
    {0x866C,0xF2B0}, /* 虬: GB2312 Code: 0xF2B0 ==> Row:82 Col:16 */
    {0x866E,0xF2B1}, /* 虮: GB2312 Code: 0xF2B1 ==> Row:82 Col:17 */
    {0x8671,0xCAAD}, /* 虱: GB2312 Code: 0xCAAD ==> Row:42 Col:13 */
    {0x8679,0xBAE7}, /* 虹: GB2312 Code: 0xBAE7 ==> Row:26 Col:71 */
    {0x867A,0xF2B3}, /* 虺: GB2312 Code: 0xF2B3 ==> Row:82 Col:19 */
    {0x867B,0xF2B5}, /* 虻: GB2312 Code: 0xF2B5 ==> Row:82 Col:21 */
    {0x867C,0xF2B4}, /* 虼: GB2312 Code: 0xF2B4 ==> Row:82 Col:20 */
    {0x867D,0xCBE4}, /* 虽: GB2312 Code: 0xCBE4 ==> Row:43 Col:68 */
    {0x867E,0xCFBA}, /* 虾: GB2312 Code: 0xCFBA ==> Row:47 Col:26 */
    {0x867F,0xF2B2}, /* 虿: GB2312 Code: 0xF2B2 ==> Row:82 Col:18 */
    {0x8680,0xCAB4}, /* 蚀: GB2312 Code: 0xCAB4 ==> Row:42 Col:20 */
    {0x8681,0xD2CF}, /* 蚁: GB2312 Code: 0xD2CF ==> Row:50 Col:47 */
    {0x8682,0xC2EC}, /* 蚂: GB2312 Code: 0xC2EC ==> Row:34 Col:76 */
    {0x868A,0xCEC3}, /* 蚊: GB2312 Code: 0xCEC3 ==> Row:46 Col:35 */
    {0x868B,0xF2B8}, /* 蚋: GB2312 Code: 0xF2B8 ==> Row:82 Col:24 */
    {0x868C,0xB0F6}, /* 蚌: GB2312 Code: 0xB0F6 ==> Row:16 Col:86 */
    {0x868D,0xF2B7}, /* 蚍: GB2312 Code: 0xF2B7 ==> Row:82 Col:23 */
    {0x8693,0xF2BE}, /* 蚓: GB2312 Code: 0xF2BE ==> Row:82 Col:30 */
    {0x8695,0xB2CF}, /* 蚕: GB2312 Code: 0xB2CF ==> Row:18 Col:47 */
    {0x869C,0xD1C1}, /* 蚜: GB2312 Code: 0xD1C1 ==> Row:49 Col:33 */
    {0x869D,0xF2BA}, /* 蚝: GB2312 Code: 0xF2BA ==> Row:82 Col:26 */
    {0x86A3,0xF2BC}, /* 蚣: GB2312 Code: 0xF2BC ==> Row:82 Col:28 */
    {0x86A4,0xD4E9}, /* 蚤: GB2312 Code: 0xD4E9 ==> Row:52 Col:73 */
    {0x86A7,0xF2BB}, /* 蚧: GB2312 Code: 0xF2BB ==> Row:82 Col:27 */
    {0x86A8,0xF2B6}, /* 蚨: GB2312 Code: 0xF2B6 ==> Row:82 Col:22 */
    {0x86A9,0xF2BF}, /* 蚩: GB2312 Code: 0xF2BF ==> Row:82 Col:31 */
    {0x86AA,0xF2BD}, /* 蚪: GB2312 Code: 0xF2BD ==> Row:82 Col:29 */
    {0x86AC,0xF2B9}, /* 蚬: GB2312 Code: 0xF2B9 ==> Row:82 Col:25 */
    {0x86AF,0xF2C7}, /* 蚯: GB2312 Code: 0xF2C7 ==> Row:82 Col:39 */
    {0x86B0,0xF2C4}, /* 蚰: GB2312 Code: 0xF2C4 ==> Row:82 Col:36 */
    {0x86B1,0xF2C6}, /* 蚱: GB2312 Code: 0xF2C6 ==> Row:82 Col:38 */
    {0x86B4,0xF2CA}, /* 蚴: GB2312 Code: 0xF2CA ==> Row:82 Col:42 */
    {0x86B5,0xF2C2}, /* 蚵: GB2312 Code: 0xF2C2 ==> Row:82 Col:34 */
    {0x86B6,0xF2C0}, /* 蚶: GB2312 Code: 0xF2C0 ==> Row:82 Col:32 */
    {0x86BA,0xF2C5}, /* 蚺: GB2312 Code: 0xF2C5 ==> Row:82 Col:37 */
    {0x86C0,0xD6FB}, /* 蛀: GB2312 Code: 0xD6FB ==> Row:54 Col:91 */
    {0x86C4,0xF2C1}, /* 蛄: GB2312 Code: 0xF2C1 ==> Row:82 Col:33 */
    {0x86C6,0xC7F9}, /* 蛆: GB2312 Code: 0xC7F9 ==> Row:39 Col:89 */
    {0x86C7,0xC9DF}, /* 蛇: GB2312 Code: 0xC9DF ==> Row:41 Col:63 */
    {0x86C9,0xF2C8}, /* 蛉: GB2312 Code: 0xF2C8 ==> Row:82 Col:40 */
    {0x86CA,0xB9C6}, /* 蛊: GB2312 Code: 0xB9C6 ==> Row:25 Col:38 */
    {0x86CB,0xB5B0}, /* 蛋: GB2312 Code: 0xB5B0 ==> Row:21 Col:16 */
    {0x86CE,0xF2C3}, /* 蛎: GB2312 Code: 0xF2C3 ==> Row:82 Col:35 */
    {0x86CF,0xF2C9}, /* 蛏: GB2312 Code: 0xF2C9 ==> Row:82 Col:41 */
    {0x86D0,0xF2D0}, /* 蛐: GB2312 Code: 0xF2D0 ==> Row:82 Col:48 */
    {0x86D1,0xF2D6}, /* 蛑: GB2312 Code: 0xF2D6 ==> Row:82 Col:54 */
    {0x86D4,0xBBD7}, /* 蛔: GB2312 Code: 0xBBD7 ==> Row:27 Col:55 */
    {0x86D8,0xF2D5}, /* 蛘: GB2312 Code: 0xF2D5 ==> Row:82 Col:53 */
    {0x86D9,0xCDDC}, /* 蛙: GB2312 Code: 0xCDDC ==> Row:45 Col:60 */
    {0x86DB,0xD6EB}, /* 蛛: GB2312 Code: 0xD6EB ==> Row:54 Col:75 */
    {0x86DE,0xF2D2}, /* 蛞: GB2312 Code: 0xF2D2 ==> Row:82 Col:50 */
    {0x86DF,0xF2D4}, /* 蛟: GB2312 Code: 0xF2D4 ==> Row:82 Col:52 */
    {0x86E4,0xB8F2}, /* 蛤: GB2312 Code: 0xB8F2 ==> Row:24 Col:82 */
    {0x86E9,0xF2CB}, /* 蛩: GB2312 Code: 0xF2CB ==> Row:82 Col:43 */
    {0x86ED,0xF2CE}, /* 蛭: GB2312 Code: 0xF2CE ==> Row:82 Col:46 */
    {0x86EE,0xC2F9}, /* 蛮: GB2312 Code: 0xC2F9 ==> Row:34 Col:89 */
    {0x86F0,0xD5DD}, /* 蛰: GB2312 Code: 0xD5DD ==> Row:53 Col:61 */
    {0x86F1,0xF2CC}, /* 蛱: GB2312 Code: 0xF2CC ==> Row:82 Col:44 */
    {0x86F2,0xF2CD}, /* 蛲: GB2312 Code: 0xF2CD ==> Row:82 Col:45 */
    {0x86F3,0xF2CF}, /* 蛳: GB2312 Code: 0xF2CF ==> Row:82 Col:47 */
    {0x86F4,0xF2D3}, /* 蛴: GB2312 Code: 0xF2D3 ==> Row:82 Col:51 */
    {0x86F8,0xF2D9}, /* 蛸: GB2312 Code: 0xF2D9 ==> Row:82 Col:57 */
    {0x86F9,0xD3BC}, /* 蛹: GB2312 Code: 0xD3BC ==> Row:51 Col:28 */
    {0x86FE,0xB6EA}, /* 蛾: GB2312 Code: 0xB6EA ==> Row:22 Col:74 */
    {0x8700,0xCAF1}, /* 蜀: GB2312 Code: 0xCAF1 ==> Row:42 Col:81 */
    {0x8702,0xB7E4}, /* 蜂: GB2312 Code: 0xB7E4 ==> Row:23 Col:68 */
    {0x8703,0xF2D7}, /* 蜃: GB2312 Code: 0xF2D7 ==> Row:82 Col:55 */
    {0x8707,0xF2D8}, /* 蜇: GB2312 Code: 0xF2D8 ==> Row:82 Col:56 */
    {0x8708,0xF2DA}, /* 蜈: GB2312 Code: 0xF2DA ==> Row:82 Col:58 */
    {0x8709,0xF2DD}, /* 蜉: GB2312 Code: 0xF2DD ==> Row:82 Col:61 */
    {0x870A,0xF2DB}, /* 蜊: GB2312 Code: 0xF2DB ==> Row:82 Col:59 */
    {0x870D,0xF2DC}, /* 蜍: GB2312 Code: 0xF2DC ==> Row:82 Col:60 */
    {0x8712,0xD1D1}, /* 蜒: GB2312 Code: 0xD1D1 ==> Row:49 Col:49 */
    {0x8713,0xF2D1}, /* 蜓: GB2312 Code: 0xF2D1 ==> Row:82 Col:49 */
    {0x8715,0xCDC9}, /* 蜕: GB2312 Code: 0xCDC9 ==> Row:45 Col:41 */
    {0x8717,0xCECF}, /* 蜗: GB2312 Code: 0xCECF ==> Row:46 Col:47 */
    {0x8718,0xD6A9}, /* 蜘: GB2312 Code: 0xD6A9 ==> Row:54 Col:09 */
    {0x871A,0xF2E3}, /* 蜚: GB2312 Code: 0xF2E3 ==> Row:82 Col:67 */
    {0x871C,0xC3DB}, /* 蜜: GB2312 Code: 0xC3DB ==> Row:35 Col:59 */
    {0x871E,0xF2E0}, /* 蜞: GB2312 Code: 0xF2E0 ==> Row:82 Col:64 */
    {0x8721,0xC0AF}, /* 蜡: GB2312 Code: 0xC0AF ==> Row:32 Col:15 */
    {0x8722,0xF2EC}, /* 蜢: GB2312 Code: 0xF2EC ==> Row:82 Col:76 */
    {0x8723,0xF2DE}, /* 蜣: GB2312 Code: 0xF2DE ==> Row:82 Col:62 */
    {0x8725,0xF2E1}, /* 蜥: GB2312 Code: 0xF2E1 ==> Row:82 Col:65 */
    {0x8729,0xF2E8}, /* 蜩: GB2312 Code: 0xF2E8 ==> Row:82 Col:72 */
    {0x872E,0xF2E2}, /* 蜮: GB2312 Code: 0xF2E2 ==> Row:82 Col:66 */
    {0x8731,0xF2E7}, /* 蜱: GB2312 Code: 0xF2E7 ==> Row:82 Col:71 */
    {0x8734,0xF2E6}, /* 蜴: GB2312 Code: 0xF2E6 ==> Row:82 Col:70 */
    {0x8737,0xF2E9}, /* 蜷: GB2312 Code: 0xF2E9 ==> Row:82 Col:73 */
    {0x873B,0xF2DF}, /* 蜻: GB2312 Code: 0xF2DF ==> Row:82 Col:63 */
    {0x873E,0xF2E4}, /* 蜾: GB2312 Code: 0xF2E4 ==> Row:82 Col:68 */
    {0x873F,0xF2EA}, /* 蜿: GB2312 Code: 0xF2EA ==> Row:82 Col:74 */
    {0x8747,0xD3AC}, /* 蝇: GB2312 Code: 0xD3AC ==> Row:51 Col:12 */
    {0x8748,0xF2E5}, /* 蝈: GB2312 Code: 0xF2E5 ==> Row:82 Col:69 */
    {0x8749,0xB2F5}, /* 蝉: GB2312 Code: 0xB2F5 ==> Row:18 Col:85 */
    {0x874C,0xF2F2}, /* 蝌: GB2312 Code: 0xF2F2 ==> Row:82 Col:82 */
    {0x874E,0xD0AB}, /* 蝎: GB2312 Code: 0xD0AB ==> Row:48 Col:11 */
    {0x8753,0xF2F5}, /* 蝓: GB2312 Code: 0xF2F5 ==> Row:82 Col:85 */
    {0x8757,0xBBC8}, /* 蝗: GB2312 Code: 0xBBC8 ==> Row:27 Col:40 */
    {0x8759,0xF2F9}, /* 蝙: GB2312 Code: 0xF2F9 ==> Row:82 Col:89 */
    {0x8760,0xF2F0}, /* 蝠: GB2312 Code: 0xF2F0 ==> Row:82 Col:80 */
    {0x8763,0xF2F6}, /* 蝣: GB2312 Code: 0xF2F6 ==> Row:82 Col:86 */
    {0x8764,0xF2F8}, /* 蝤: GB2312 Code: 0xF2F8 ==> Row:82 Col:88 */
    {0x8765,0xF2FA}, /* 蝥: GB2312 Code: 0xF2FA ==> Row:82 Col:90 */
    {0x876E,0xF2F3}, /* 蝮: GB2312 Code: 0xF2F3 ==> Row:82 Col:83 */
    {0x8770,0xF2F1}, /* 蝰: GB2312 Code: 0xF2F1 ==> Row:82 Col:81 */
    {0x8774,0xBAFB}, /* 蝴: GB2312 Code: 0xBAFB ==> Row:26 Col:91 */
    {0x8776,0xB5FB}, /* 蝶: GB2312 Code: 0xB5FB ==> Row:21 Col:91 */
    {0x877B,0xF2EF}, /* 蝻: GB2312 Code: 0xF2EF ==> Row:82 Col:79 */
    {0x877C,0xF2F7}, /* 蝼: GB2312 Code: 0xF2F7 ==> Row:82 Col:87 */
    {0x877D,0xF2ED}, /* 蝽: GB2312 Code: 0xF2ED ==> Row:82 Col:77 */
    {0x877E,0xF2EE}, /* 蝾: GB2312 Code: 0xF2EE ==> Row:82 Col:78 */
    {0x8782,0xF2EB}, /* 螂: GB2312 Code: 0xF2EB ==> Row:82 Col:75 */
    {0x8783,0xF3A6}, /* 螃: GB2312 Code: 0xF3A6 ==> Row:83 Col:06 */
    {0x8785,0xF3A3}, /* 螅: GB2312 Code: 0xF3A3 ==> Row:83 Col:03 */
    {0x8788,0xF3A2}, /* 螈: GB2312 Code: 0xF3A2 ==> Row:83 Col:02 */
    {0x878B,0xF2F4}, /* 螋: GB2312 Code: 0xF2F4 ==> Row:82 Col:84 */
    {0x878D,0xC8DA}, /* 融: GB2312 Code: 0xC8DA ==> Row:40 Col:58 */
    {0x8793,0xF2FB}, /* 螓: GB2312 Code: 0xF2FB ==> Row:82 Col:91 */
    {0x8797,0xF3A5}, /* 螗: GB2312 Code: 0xF3A5 ==> Row:83 Col:05 */
    {0x879F,0xC3F8}, /* 螟: GB2312 Code: 0xC3F8 ==> Row:35 Col:88 */
    {0x87A8,0xF2FD}, /* 螨: GB2312 Code: 0xF2FD ==> Row:82 Col:93 */
    {0x87AB,0xF3A7}, /* 螫: GB2312 Code: 0xF3A7 ==> Row:83 Col:07 */
    {0x87AC,0xF3A9}, /* 螬: GB2312 Code: 0xF3A9 ==> Row:83 Col:09 */
    {0x87AD,0xF3A4}, /* 螭: GB2312 Code: 0xF3A4 ==> Row:83 Col:04 */
    {0x87AF,0xF2FC}, /* 螯: GB2312 Code: 0xF2FC ==> Row:82 Col:92 */
    {0x87B3,0xF3AB}, /* 螳: GB2312 Code: 0xF3AB ==> Row:83 Col:11 */
    {0x87B5,0xF3AA}, /* 螵: GB2312 Code: 0xF3AA ==> Row:83 Col:10 */
    {0x87BA,0xC2DD}, /* 螺: GB2312 Code: 0xC2DD ==> Row:34 Col:61 */
    {0x87BD,0xF3AE}, /* 螽: GB2312 Code: 0xF3AE ==> Row:83 Col:14 */
    {0x87C0,0xF3B0}, /* 蟀: GB2312 Code: 0xF3B0 ==> Row:83 Col:16 */
    {0x87C6,0xF3A1}, /* 蟆: GB2312 Code: 0xF3A1 ==> Row:83 Col:01 */
    {0x87CA,0xF3B1}, /* 蟊: GB2312 Code: 0xF3B1 ==> Row:83 Col:17 */
    {0x87CB,0xF3AC}, /* 蟋: GB2312 Code: 0xF3AC ==> Row:83 Col:12 */
    {0x87D1,0xF3AF}, /* 蟑: GB2312 Code: 0xF3AF ==> Row:83 Col:15 */
    {0x87D2,0xF2FE}, /* 蟒: GB2312 Code: 0xF2FE ==> Row:82 Col:94 */
    {0x87D3,0xF3AD}, /* 蟓: GB2312 Code: 0xF3AD ==> Row:83 Col:13 */
    {0x87DB,0xF3B2}, /* 蟛: GB2312 Code: 0xF3B2 ==> Row:83 Col:18 */
    {0x87E0,0xF3B4}, /* 蟠: GB2312 Code: 0xF3B4 ==> Row:83 Col:20 */
    {0x87E5,0xF3A8}, /* 蟥: GB2312 Code: 0xF3A8 ==> Row:83 Col:08 */
    {0x87EA,0xF3B3}, /* 蟪: GB2312 Code: 0xF3B3 ==> Row:83 Col:19 */
    {0x87EE,0xF3B5}, /* 蟮: GB2312 Code: 0xF3B5 ==> Row:83 Col:21 */
    {0x87F9,0xD0B7}, /* 蟹: GB2312 Code: 0xD0B7 ==> Row:48 Col:23 */
    {0x87FE,0xF3B8}, /* 蟾: GB2312 Code: 0xF3B8 ==> Row:83 Col:24 */
    {0x8803,0xD9F9}, /* 蠃: GB2312 Code: 0xD9F9 ==> Row:57 Col:89 */
    {0x880A,0xF3B9}, /* 蠊: GB2312 Code: 0xF3B9 ==> Row:83 Col:25 */
    {0x8813,0xF3B7}, /* 蠓: GB2312 Code: 0xF3B7 ==> Row:83 Col:23 */
    {0x8815,0xC8E4}, /* 蠕: GB2312 Code: 0xC8E4 ==> Row:40 Col:68 */
    {0x8816,0xF3B6}, /* 蠖: GB2312 Code: 0xF3B6 ==> Row:83 Col:22 */
    {0x881B,0xF3BA}, /* 蠛: GB2312 Code: 0xF3BA ==> Row:83 Col:26 */
    {0x8821,0xF3BB}, /* 蠡: GB2312 Code: 0xF3BB ==> Row:83 Col:27 */
    {0x8822,0xB4C0}, /* 蠢: GB2312 Code: 0xB4C0 ==> Row:20 Col:32 */
    {0x8832,0xEEC3}, /* 蠲: GB2312 Code: 0xEEC3 ==> Row:78 Col:35 */
    {0x8839,0xF3BC}, /* 蠹: GB2312 Code: 0xF3BC ==> Row:83 Col:28 */
    {0x883C,0xF3BD}, /* 蠼: GB2312 Code: 0xF3BD ==> Row:83 Col:29 */
    {0x8840,0xD1AA}, /* 血: GB2312 Code: 0xD1AA ==> Row:49 Col:10 */
    {0x8844,0xF4AC}, /* 衄: GB2312 Code: 0xF4AC ==> Row:84 Col:12 */
    {0x8845,0xD0C6}, /* 衅: GB2312 Code: 0xD0C6 ==> Row:48 Col:38 */
    {0x884C,0xD0D0}, /* 行: GB2312 Code: 0xD0D0 ==> Row:48 Col:48 */
    {0x884D,0xD1DC}, /* 衍: GB2312 Code: 0xD1DC ==> Row:49 Col:60 */
    {0x8854,0xCFCE}, /* 衔: GB2312 Code: 0xCFCE ==> Row:47 Col:46 */
    {0x8857,0xBDD6}, /* 街: GB2312 Code: 0xBDD6 ==> Row:29 Col:54 */
    {0x8859,0xD1C3}, /* 衙: GB2312 Code: 0xD1C3 ==> Row:49 Col:35 */
    {0x8861,0xBAE2}, /* 衡: GB2312 Code: 0xBAE2 ==> Row:26 Col:66 */
    {0x8862,0xE1E9}, /* 衢: GB2312 Code: 0xE1E9 ==> Row:65 Col:73 */
    {0x8863,0xD2C2}, /* 衣: GB2312 Code: 0xD2C2 ==> Row:50 Col:34 */
    {0x8864,0xF1C2}, /* 衤: GB2312 Code: 0xF1C2 ==> Row:81 Col:34 */
    {0x8865,0xB2B9}, /* 补: GB2312 Code: 0xB2B9 ==> Row:18 Col:25 */
    {0x8868,0xB1ED}, /* 表: GB2312 Code: 0xB1ED ==> Row:17 Col:77 */
    {0x8869,0xF1C3}, /* 衩: GB2312 Code: 0xF1C3 ==> Row:81 Col:35 */
    {0x886B,0xC9C0}, /* 衫: GB2312 Code: 0xC9C0 ==> Row:41 Col:32 */
    {0x886C,0xB3C4}, /* 衬: GB2312 Code: 0xB3C4 ==> Row:19 Col:36 */
    {0x886E,0xD9F2}, /* 衮: GB2312 Code: 0xD9F2 ==> Row:57 Col:82 */
    {0x8870,0xCBA5}, /* 衰: GB2312 Code: 0xCBA5 ==> Row:43 Col:05 */
    {0x8872,0xF1C4}, /* 衲: GB2312 Code: 0xF1C4 ==> Row:81 Col:36 */
    {0x8877,0xD6D4}, /* 衷: GB2312 Code: 0xD6D4 ==> Row:54 Col:52 */
    {0x887D,0xF1C5}, /* 衽: GB2312 Code: 0xF1C5 ==> Row:81 Col:37 */
    {0x887E,0xF4C0}, /* 衾: GB2312 Code: 0xF4C0 ==> Row:84 Col:32 */
    {0x887F,0xF1C6}, /* 衿: GB2312 Code: 0xF1C6 ==> Row:81 Col:38 */
    {0x8881,0xD4AC}, /* 袁: GB2312 Code: 0xD4AC ==> Row:52 Col:12 */
    {0x8882,0xF1C7}, /* 袂: GB2312 Code: 0xF1C7 ==> Row:81 Col:39 */
    {0x8884,0xB0C0}, /* 袄: GB2312 Code: 0xB0C0 ==> Row:16 Col:32 */
    {0x8885,0xF4C1}, /* 袅: GB2312 Code: 0xF4C1 ==> Row:84 Col:33 */
    {0x8888,0xF4C2}, /* 袈: GB2312 Code: 0xF4C2 ==> Row:84 Col:34 */
    {0x888B,0xB4FC}, /* 袋: GB2312 Code: 0xB4FC ==> Row:20 Col:92 */
    {0x888D,0xC5DB}, /* 袍: GB2312 Code: 0xC5DB ==> Row:37 Col:59 */
    {0x8892,0xCCBB}, /* 袒: GB2312 Code: 0xCCBB ==> Row:44 Col:27 */
    {0x8896,0xD0E4}, /* 袖: GB2312 Code: 0xD0E4 ==> Row:48 Col:68 */
    {0x889C,0xCDE0}, /* 袜: GB2312 Code: 0xCDE0 ==> Row:45 Col:64 */
    {0x88A2,0xF1C8}, /* 袢: GB2312 Code: 0xF1C8 ==> Row:81 Col:40 */
    {0x88A4,0xD9F3}, /* 袤: GB2312 Code: 0xD9F3 ==> Row:57 Col:83 */
    {0x88AB,0xB1BB}, /* 被: GB2312 Code: 0xB1BB ==> Row:17 Col:27 */
    {0x88AD,0xCFAE}, /* 袭: GB2312 Code: 0xCFAE ==> Row:47 Col:14 */
    {0x88B1,0xB8A4}, /* 袱: GB2312 Code: 0xB8A4 ==> Row:24 Col:04 */
    {0x88B7,0xF1CA}, /* 袷: GB2312 Code: 0xF1CA ==> Row:81 Col:42 */
    {0x88BC,0xF1CB}, /* 袼: GB2312 Code: 0xF1CB ==> Row:81 Col:43 */
    {0x88C1,0xB2C3}, /* 裁: GB2312 Code: 0xB2C3 ==> Row:18 Col:35 */
    {0x88C2,0xC1D1}, /* 裂: GB2312 Code: 0xC1D1 ==> Row:33 Col:49 */
    {0x88C5,0xD7B0}, /* 装: GB2312 Code: 0xD7B0 ==> Row:55 Col:16 */
    {0x88C6,0xF1C9}, /* 裆: GB2312 Code: 0xF1C9 ==> Row:81 Col:41 */
    {0x88C9,0xF1CC}, /* 裉: GB2312 Code: 0xF1CC ==> Row:81 Col:44 */
    {0x88CE,0xF1CE}, /* 裎: GB2312 Code: 0xF1CE ==> Row:81 Col:46 */
    {0x88D2,0xD9F6}, /* 裒: GB2312 Code: 0xD9F6 ==> Row:57 Col:86 */
    {0x88D4,0xD2E1}, /* 裔: GB2312 Code: 0xD2E1 ==> Row:50 Col:65 */
    {0x88D5,0xD4A3}, /* 裕: GB2312 Code: 0xD4A3 ==> Row:52 Col:03 */
    {0x88D8,0xF4C3}, /* 裘: GB2312 Code: 0xF4C3 ==> Row:84 Col:35 */
    {0x88D9,0xC8B9}, /* 裙: GB2312 Code: 0xC8B9 ==> Row:40 Col:25 */
    {0x88DF,0xF4C4}, /* 裟: GB2312 Code: 0xF4C4 ==> Row:84 Col:36 */
    {0x88E2,0xF1CD}, /* 裢: GB2312 Code: 0xF1CD ==> Row:81 Col:45 */
    {0x88E3,0xF1CF}, /* 裣: GB2312 Code: 0xF1CF ==> Row:81 Col:47 */
    {0x88E4,0xBFE3}, /* 裤: GB2312 Code: 0xBFE3 ==> Row:31 Col:67 */
    {0x88E5,0xF1D0}, /* 裥: GB2312 Code: 0xF1D0 ==> Row:81 Col:48 */
    {0x88E8,0xF1D4}, /* 裨: GB2312 Code: 0xF1D4 ==> Row:81 Col:52 */
    {0x88F0,0xF1D6}, /* 裰: GB2312 Code: 0xF1D6 ==> Row:81 Col:54 */
    {0x88F1,0xF1D1}, /* 裱: GB2312 Code: 0xF1D1 ==> Row:81 Col:49 */
    {0x88F3,0xC9D1}, /* 裳: GB2312 Code: 0xC9D1 ==> Row:41 Col:49 */
    {0x88F4,0xC5E1}, /* 裴: GB2312 Code: 0xC5E1 ==> Row:37 Col:65 */
    {0x88F8,0xC2E3}, /* 裸: GB2312 Code: 0xC2E3 ==> Row:34 Col:67 */
    {0x88F9,0xB9FC}, /* 裹: GB2312 Code: 0xB9FC ==> Row:25 Col:92 */
    {0x88FC,0xF1D3}, /* 裼: GB2312 Code: 0xF1D3 ==> Row:81 Col:51 */
    {0x88FE,0xF1D5}, /* 裾: GB2312 Code: 0xF1D5 ==> Row:81 Col:53 */
    {0x8902,0xB9D3}, /* 褂: GB2312 Code: 0xB9D3 ==> Row:25 Col:51 */
    {0x890A,0xF1DB}, /* 褊: GB2312 Code: 0xF1DB ==> Row:81 Col:59 */
    {0x8910,0xBAD6}, /* 褐: GB2312 Code: 0xBAD6 ==> Row:26 Col:54 */
    {0x8912,0xB0FD}, /* 褒: GB2312 Code: 0xB0FD ==> Row:16 Col:93 */
    {0x8913,0xF1D9}, /* 褓: GB2312 Code: 0xF1D9 ==> Row:81 Col:57 */
    {0x8919,0xF1D8}, /* 褙: GB2312 Code: 0xF1D8 ==> Row:81 Col:56 */
    {0x891A,0xF1D2}, /* 褚: GB2312 Code: 0xF1D2 ==> Row:81 Col:50 */
    {0x891B,0xF1DA}, /* 褛: GB2312 Code: 0xF1DA ==> Row:81 Col:58 */
    {0x8921,0xF1D7}, /* 褡: GB2312 Code: 0xF1D7 ==> Row:81 Col:55 */
    {0x8925,0xC8EC}, /* 褥: GB2312 Code: 0xC8EC ==> Row:40 Col:76 */
    {0x892A,0xCDCA}, /* 褪: GB2312 Code: 0xCDCA ==> Row:45 Col:42 */
    {0x892B,0xF1DD}, /* 褫: GB2312 Code: 0xF1DD ==> Row:81 Col:61 */
    {0x8930,0xE5BD}, /* 褰: GB2312 Code: 0xE5BD ==> Row:69 Col:29 */
    {0x8934,0xF1DC}, /* 褴: GB2312 Code: 0xF1DC ==> Row:81 Col:60 */
    {0x8936,0xF1DE}, /* 褶: GB2312 Code: 0xF1DE ==> Row:81 Col:62 */
    {0x8941,0xF1DF}, /* 襁: GB2312 Code: 0xF1DF ==> Row:81 Col:63 */
    {0x8944,0xCFE5}, /* 襄: GB2312 Code: 0xCFE5 ==> Row:47 Col:69 */
    {0x895E,0xF4C5}, /* 襞: GB2312 Code: 0xF4C5 ==> Row:84 Col:37 */
    {0x895F,0xBDF3}, /* 襟: GB2312 Code: 0xBDF3 ==> Row:29 Col:83 */
    {0x8966,0xF1E0}, /* 襦: GB2312 Code: 0xF1E0 ==> Row:81 Col:64 */
    {0x897B,0xF1E1}, /* 襻: GB2312 Code: 0xF1E1 ==> Row:81 Col:65 */
    {0x897F,0xCEF7}, /* 西: GB2312 Code: 0xCEF7 ==> Row:46 Col:87 */
    {0x8981,0xD2AA}, /* 要: GB2312 Code: 0xD2AA ==> Row:50 Col:10 */
    {0x8983,0xF1FB}, /* 覃: GB2312 Code: 0xF1FB ==> Row:81 Col:91 */
    {0x8986,0xB8B2}, /* 覆: GB2312 Code: 0xB8B2 ==> Row:24 Col:18 */
    {0x89C1,0xBCFB}, /* 见: GB2312 Code: 0xBCFB ==> Row:28 Col:91 */
    {0x89C2,0xB9DB}, /* 观: GB2312 Code: 0xB9DB ==> Row:25 Col:59 */
    {0x89C4,0xB9E6}, /* 规: GB2312 Code: 0xB9E6 ==> Row:25 Col:70 */
    {0x89C5,0xC3D9}, /* 觅: GB2312 Code: 0xC3D9 ==> Row:35 Col:57 */
    {0x89C6,0xCAD3}, /* 视: GB2312 Code: 0xCAD3 ==> Row:42 Col:51 */
    {0x89C7,0xEAE8}, /* 觇: GB2312 Code: 0xEAE8 ==> Row:74 Col:72 */
    {0x89C8,0xC0C0}, /* 览: GB2312 Code: 0xC0C0 ==> Row:32 Col:32 */
    {0x89C9,0xBEF5}, /* 觉: GB2312 Code: 0xBEF5 ==> Row:30 Col:85 */
    {0x89CA,0xEAE9}, /* 觊: GB2312 Code: 0xEAE9 ==> Row:74 Col:73 */
    {0x89CB,0xEAEA}, /* 觋: GB2312 Code: 0xEAEA ==> Row:74 Col:74 */
    {0x89CC,0xEAEB}, /* 觌: GB2312 Code: 0xEAEB ==> Row:74 Col:75 */
    {0x89CE,0xEAEC}, /* 觎: GB2312 Code: 0xEAEC ==> Row:74 Col:76 */
    {0x89CF,0xEAED}, /* 觏: GB2312 Code: 0xEAED ==> Row:74 Col:77 */
    {0x89D0,0xEAEE}, /* 觐: GB2312 Code: 0xEAEE ==> Row:74 Col:78 */
    {0x89D1,0xEAEF}, /* 觑: GB2312 Code: 0xEAEF ==> Row:74 Col:79 */
    {0x89D2,0xBDC7}, /* 角: GB2312 Code: 0xBDC7 ==> Row:29 Col:39 */
    {0x89D6,0xF5FB}, /* 觖: GB2312 Code: 0xF5FB ==> Row:85 Col:91 */
    {0x89DA,0xF5FD}, /* 觚: GB2312 Code: 0xF5FD ==> Row:85 Col:93 */
    {0x89DC,0xF5FE}, /* 觜: GB2312 Code: 0xF5FE ==> Row:85 Col:94 */
    {0x89DE,0xF5FC}, /* 觞: GB2312 Code: 0xF5FC ==> Row:85 Col:92 */
    {0x89E3,0xBDE2}, /* 解: GB2312 Code: 0xBDE2 ==> Row:29 Col:66 */
    {0x89E5,0xF6A1}, /* 觥: GB2312 Code: 0xF6A1 ==> Row:86 Col:01 */
    {0x89E6,0xB4A5}, /* 触: GB2312 Code: 0xB4A5 ==> Row:20 Col:05 */
    {0x89EB,0xF6A2}, /* 觫: GB2312 Code: 0xF6A2 ==> Row:86 Col:02 */
    {0x89EF,0xF6A3}, /* 觯: GB2312 Code: 0xF6A3 ==> Row:86 Col:03 */
    {0x89F3,0xECB2}, /* 觳: GB2312 Code: 0xECB2 ==> Row:76 Col:18 */
    {0x8A00,0xD1D4}, /* 言: GB2312 Code: 0xD1D4 ==> Row:49 Col:52 */
    {0x8A07,0xD9EA}, /* 訇: GB2312 Code: 0xD9EA ==> Row:57 Col:74 */
    {0x8A3E,0xF6A4}, /* 訾: GB2312 Code: 0xF6A4 ==> Row:86 Col:04 */
    {0x8A48,0xEEBA}, /* 詈: GB2312 Code: 0xEEBA ==> Row:78 Col:26 */
    {0x8A79,0xD5B2}, /* 詹: GB2312 Code: 0xD5B2 ==> Row:53 Col:18 */
    {0x8A89,0xD3FE}, /* 誉: GB2312 Code: 0xD3FE ==> Row:51 Col:94 */
    {0x8A8A,0xCCDC}, /* 誊: GB2312 Code: 0xCCDC ==> Row:44 Col:60 */
    {0x8A93,0xCAC4}, /* 誓: GB2312 Code: 0xCAC4 ==> Row:42 Col:36 */
    {0x8B07,0xE5C0}, /* 謇: GB2312 Code: 0xE5C0 ==> Row:69 Col:32 */
    {0x8B26,0xF6A5}, /* 謦: GB2312 Code: 0xF6A5 ==> Row:86 Col:05 */
    {0x8B66,0xBEAF}, /* 警: GB2312 Code: 0xBEAF ==> Row:30 Col:15 */
    {0x8B6C,0xC6A9}, /* 譬: GB2312 Code: 0xC6A9 ==> Row:38 Col:09 */
    {0x8BA0,0xDAA5}, /* 讠: GB2312 Code: 0xDAA5 ==> Row:58 Col:05 */
    {0x8BA1,0xBCC6}, /* 计: GB2312 Code: 0xBCC6 ==> Row:28 Col:38 */
    {0x8BA2,0xB6A9}, /* 订: GB2312 Code: 0xB6A9 ==> Row:22 Col:09 */
    {0x8BA3,0xB8BC}, /* 讣: GB2312 Code: 0xB8BC ==> Row:24 Col:28 */
    {0x8BA4,0xC8CF}, /* 认: GB2312 Code: 0xC8CF ==> Row:40 Col:47 */
    {0x8BA5,0xBCA5}, /* 讥: GB2312 Code: 0xBCA5 ==> Row:28 Col:05 */
    {0x8BA6,0xDAA6}, /* 讦: GB2312 Code: 0xDAA6 ==> Row:58 Col:06 */
    {0x8BA7,0xDAA7}, /* 讧: GB2312 Code: 0xDAA7 ==> Row:58 Col:07 */
    {0x8BA8,0xCCD6}, /* 讨: GB2312 Code: 0xCCD6 ==> Row:44 Col:54 */
    {0x8BA9,0xC8C3}, /* 让: GB2312 Code: 0xC8C3 ==> Row:40 Col:35 */
    {0x8BAA,0xDAA8}, /* 讪: GB2312 Code: 0xDAA8 ==> Row:58 Col:08 */
    {0x8BAB,0xC6FD}, /* 讫: GB2312 Code: 0xC6FD ==> Row:38 Col:93 */
    {0x8BAD,0xD1B5}, /* 训: GB2312 Code: 0xD1B5 ==> Row:49 Col:21 */
    {0x8BAE,0xD2E9}, /* 议: GB2312 Code: 0xD2E9 ==> Row:50 Col:73 */
    {0x8BAF,0xD1B6}, /* 讯: GB2312 Code: 0xD1B6 ==> Row:49 Col:22 */
    {0x8BB0,0xBCC7}, /* 记: GB2312 Code: 0xBCC7 ==> Row:28 Col:39 */
    {0x8BB2,0xBDB2}, /* 讲: GB2312 Code: 0xBDB2 ==> Row:29 Col:18 */
    {0x8BB3,0xBBE4}, /* 讳: GB2312 Code: 0xBBE4 ==> Row:27 Col:68 */
    {0x8BB4,0xDAA9}, /* 讴: GB2312 Code: 0xDAA9 ==> Row:58 Col:09 */
    {0x8BB5,0xDAAA}, /* 讵: GB2312 Code: 0xDAAA ==> Row:58 Col:10 */
    {0x8BB6,0xD1C8}, /* 讶: GB2312 Code: 0xD1C8 ==> Row:49 Col:40 */
    {0x8BB7,0xDAAB}, /* 讷: GB2312 Code: 0xDAAB ==> Row:58 Col:11 */
    {0x8BB8,0xD0ED}, /* 许: GB2312 Code: 0xD0ED ==> Row:48 Col:77 */
    {0x8BB9,0xB6EF}, /* 讹: GB2312 Code: 0xB6EF ==> Row:22 Col:79 */
    {0x8BBA,0xC2DB}, /* 论: GB2312 Code: 0xC2DB ==> Row:34 Col:59 */
    {0x8BBC,0xCBCF}, /* 讼: GB2312 Code: 0xCBCF ==> Row:43 Col:47 */
    {0x8BBD,0xB7ED}, /* 讽: GB2312 Code: 0xB7ED ==> Row:23 Col:77 */
    {0x8BBE,0xC9E8}, /* 设: GB2312 Code: 0xC9E8 ==> Row:41 Col:72 */
    {0x8BBF,0xB7C3}, /* 访: GB2312 Code: 0xB7C3 ==> Row:23 Col:35 */
    {0x8BC0,0xBEF7}, /* 诀: GB2312 Code: 0xBEF7 ==> Row:30 Col:87 */
    {0x8BC1,0xD6A4}, /* 证: GB2312 Code: 0xD6A4 ==> Row:54 Col:04 */
    {0x8BC2,0xDAAC}, /* 诂: GB2312 Code: 0xDAAC ==> Row:58 Col:12 */
    {0x8BC3,0xDAAD}, /* 诃: GB2312 Code: 0xDAAD ==> Row:58 Col:13 */
    {0x8BC4,0xC6C0}, /* 评: GB2312 Code: 0xC6C0 ==> Row:38 Col:32 */
    {0x8BC5,0xD7E7}, /* 诅: GB2312 Code: 0xD7E7 ==> Row:55 Col:71 */
    {0x8BC6,0xCAB6}, /* 识: GB2312 Code: 0xCAB6 ==> Row:42 Col:22 */
    {0x8BC8,0xD5A9}, /* 诈: GB2312 Code: 0xD5A9 ==> Row:53 Col:09 */
    {0x8BC9,0xCBDF}, /* 诉: GB2312 Code: 0xCBDF ==> Row:43 Col:63 */
    {0x8BCA,0xD5EF}, /* 诊: GB2312 Code: 0xD5EF ==> Row:53 Col:79 */
    {0x8BCB,0xDAAE}, /* 诋: GB2312 Code: 0xDAAE ==> Row:58 Col:14 */
    {0x8BCC,0xD6DF}, /* 诌: GB2312 Code: 0xD6DF ==> Row:54 Col:63 */
    {0x8BCD,0xB4CA}, /* 词: GB2312 Code: 0xB4CA ==> Row:20 Col:42 */
    {0x8BCE,0xDAB0}, /* 诎: GB2312 Code: 0xDAB0 ==> Row:58 Col:16 */
    {0x8BCF,0xDAAF}, /* 诏: GB2312 Code: 0xDAAF ==> Row:58 Col:15 */
    {0x8BD1,0xD2EB}, /* 译: GB2312 Code: 0xD2EB ==> Row:50 Col:75 */
    {0x8BD2,0xDAB1}, /* 诒: GB2312 Code: 0xDAB1 ==> Row:58 Col:17 */
    {0x8BD3,0xDAB2}, /* 诓: GB2312 Code: 0xDAB2 ==> Row:58 Col:18 */
    {0x8BD4,0xDAB3}, /* 诔: GB2312 Code: 0xDAB3 ==> Row:58 Col:19 */
    {0x8BD5,0xCAD4}, /* 试: GB2312 Code: 0xCAD4 ==> Row:42 Col:52 */
    {0x8BD6,0xDAB4}, /* 诖: GB2312 Code: 0xDAB4 ==> Row:58 Col:20 */
    {0x8BD7,0xCAAB}, /* 诗: GB2312 Code: 0xCAAB ==> Row:42 Col:11 */
    {0x8BD8,0xDAB5}, /* 诘: GB2312 Code: 0xDAB5 ==> Row:58 Col:21 */
    {0x8BD9,0xDAB6}, /* 诙: GB2312 Code: 0xDAB6 ==> Row:58 Col:22 */
    {0x8BDA,0xB3CF}, /* 诚: GB2312 Code: 0xB3CF ==> Row:19 Col:47 */
    {0x8BDB,0xD6EF}, /* 诛: GB2312 Code: 0xD6EF ==> Row:54 Col:79 */
    {0x8BDC,0xDAB7}, /* 诜: GB2312 Code: 0xDAB7 ==> Row:58 Col:23 */
    {0x8BDD,0xBBB0}, /* 话: GB2312 Code: 0xBBB0 ==> Row:27 Col:16 */
    {0x8BDE,0xB5AE}, /* 诞: GB2312 Code: 0xB5AE ==> Row:21 Col:14 */
    {0x8BDF,0xDAB8}, /* 诟: GB2312 Code: 0xDAB8 ==> Row:58 Col:24 */
    {0x8BE0,0xDAB9}, /* 诠: GB2312 Code: 0xDAB9 ==> Row:58 Col:25 */
    {0x8BE1,0xB9EE}, /* 诡: GB2312 Code: 0xB9EE ==> Row:25 Col:78 */
    {0x8BE2,0xD1AF}, /* 询: GB2312 Code: 0xD1AF ==> Row:49 Col:15 */
    {0x8BE3,0xD2E8}, /* 诣: GB2312 Code: 0xD2E8 ==> Row:50 Col:72 */
    {0x8BE4,0xDABA}, /* 诤: GB2312 Code: 0xDABA ==> Row:58 Col:26 */
    {0x8BE5,0xB8C3}, /* 该: GB2312 Code: 0xB8C3 ==> Row:24 Col:35 */
    {0x8BE6,0xCFEA}, /* 详: GB2312 Code: 0xCFEA ==> Row:47 Col:74 */
    {0x8BE7,0xB2EF}, /* 诧: GB2312 Code: 0xB2EF ==> Row:18 Col:79 */
    {0x8BE8,0xDABB}, /* 诨: GB2312 Code: 0xDABB ==> Row:58 Col:27 */
    {0x8BE9,0xDABC}, /* 诩: GB2312 Code: 0xDABC ==> Row:58 Col:28 */
    {0x8BEB,0xBDEB}, /* 诫: GB2312 Code: 0xBDEB ==> Row:29 Col:75 */
    {0x8BEC,0xCEDC}, /* 诬: GB2312 Code: 0xCEDC ==> Row:46 Col:60 */
    {0x8BED,0xD3EF}, /* 语: GB2312 Code: 0xD3EF ==> Row:51 Col:79 */
    {0x8BEE,0xDABD}, /* 诮: GB2312 Code: 0xDABD ==> Row:58 Col:29 */
    {0x8BEF,0xCEF3}, /* 误: GB2312 Code: 0xCEF3 ==> Row:46 Col:83 */
    {0x8BF0,0xDABE}, /* 诰: GB2312 Code: 0xDABE ==> Row:58 Col:30 */
    {0x8BF1,0xD3D5}, /* 诱: GB2312 Code: 0xD3D5 ==> Row:51 Col:53 */
    {0x8BF2,0xBBE5}, /* 诲: GB2312 Code: 0xBBE5 ==> Row:27 Col:69 */
    {0x8BF3,0xDABF}, /* 诳: GB2312 Code: 0xDABF ==> Row:58 Col:31 */
    {0x8BF4,0xCBB5}, /* 说: GB2312 Code: 0xCBB5 ==> Row:43 Col:21 */
    {0x8BF5,0xCBD0}, /* 诵: GB2312 Code: 0xCBD0 ==> Row:43 Col:48 */
    {0x8BF6,0xDAC0}, /* 诶: GB2312 Code: 0xDAC0 ==> Row:58 Col:32 */
    {0x8BF7,0xC7EB}, /* 请: GB2312 Code: 0xC7EB ==> Row:39 Col:75 */
    {0x8BF8,0xD6EE}, /* 诸: GB2312 Code: 0xD6EE ==> Row:54 Col:78 */
    {0x8BF9,0xDAC1}, /* 诹: GB2312 Code: 0xDAC1 ==> Row:58 Col:33 */
    {0x8BFA,0xC5B5}, /* 诺: GB2312 Code: 0xC5B5 ==> Row:37 Col:21 */
    {0x8BFB,0xB6C1}, /* 读: GB2312 Code: 0xB6C1 ==> Row:22 Col:33 */
    {0x8BFC,0xDAC2}, /* 诼: GB2312 Code: 0xDAC2 ==> Row:58 Col:34 */
    {0x8BFD,0xB7CC}, /* 诽: GB2312 Code: 0xB7CC ==> Row:23 Col:44 */
    {0x8BFE,0xBFCE}, /* 课: GB2312 Code: 0xBFCE ==> Row:31 Col:46 */
    {0x8BFF,0xDAC3}, /* 诿: GB2312 Code: 0xDAC3 ==> Row:58 Col:35 */
    {0x8C00,0xDAC4}, /* 谀: GB2312 Code: 0xDAC4 ==> Row:58 Col:36 */
    {0x8C01,0xCBAD}, /* 谁: GB2312 Code: 0xCBAD ==> Row:43 Col:13 */
    {0x8C02,0xDAC5}, /* 谂: GB2312 Code: 0xDAC5 ==> Row:58 Col:37 */
    {0x8C03,0xB5F7}, /* 调: GB2312 Code: 0xB5F7 ==> Row:21 Col:87 */
    {0x8C04,0xDAC6}, /* 谄: GB2312 Code: 0xDAC6 ==> Row:58 Col:38 */
    {0x8C05,0xC1C2}, /* 谅: GB2312 Code: 0xC1C2 ==> Row:33 Col:34 */
    {0x8C06,0xD7BB}, /* 谆: GB2312 Code: 0xD7BB ==> Row:55 Col:27 */
    {0x8C07,0xDAC7}, /* 谇: GB2312 Code: 0xDAC7 ==> Row:58 Col:39 */
    {0x8C08,0xCCB8}, /* 谈: GB2312 Code: 0xCCB8 ==> Row:44 Col:24 */
    {0x8C0A,0xD2EA}, /* 谊: GB2312 Code: 0xD2EA ==> Row:50 Col:74 */
    {0x8C0B,0xC4B1}, /* 谋: GB2312 Code: 0xC4B1 ==> Row:36 Col:17 */
    {0x8C0C,0xDAC8}, /* 谌: GB2312 Code: 0xDAC8 ==> Row:58 Col:40 */
    {0x8C0D,0xB5FD}, /* 谍: GB2312 Code: 0xB5FD ==> Row:21 Col:93 */
    {0x8C0E,0xBBD1}, /* 谎: GB2312 Code: 0xBBD1 ==> Row:27 Col:49 */
    {0x8C0F,0xDAC9}, /* 谏: GB2312 Code: 0xDAC9 ==> Row:58 Col:41 */
    {0x8C10,0xD0B3}, /* 谐: GB2312 Code: 0xD0B3 ==> Row:48 Col:19 */
    {0x8C11,0xDACA}, /* 谑: GB2312 Code: 0xDACA ==> Row:58 Col:42 */
    {0x8C12,0xDACB}, /* 谒: GB2312 Code: 0xDACB ==> Row:58 Col:43 */
    {0x8C13,0xCEBD}, /* 谓: GB2312 Code: 0xCEBD ==> Row:46 Col:29 */
    {0x8C14,0xDACC}, /* 谔: GB2312 Code: 0xDACC ==> Row:58 Col:44 */
    {0x8C15,0xDACD}, /* 谕: GB2312 Code: 0xDACD ==> Row:58 Col:45 */
    {0x8C16,0xDACE}, /* 谖: GB2312 Code: 0xDACE ==> Row:58 Col:46 */
    {0x8C17,0xB2F7}, /* 谗: GB2312 Code: 0xB2F7 ==> Row:18 Col:87 */
    {0x8C18,0xDAD1}, /* 谘: GB2312 Code: 0xDAD1 ==> Row:58 Col:49 */
    {0x8C19,0xDACF}, /* 谙: GB2312 Code: 0xDACF ==> Row:58 Col:47 */
    {0x8C1A,0xD1E8}, /* 谚: GB2312 Code: 0xD1E8 ==> Row:49 Col:72 */
    {0x8C1B,0xDAD0}, /* 谛: GB2312 Code: 0xDAD0 ==> Row:58 Col:48 */
    {0x8C1C,0xC3D5}, /* 谜: GB2312 Code: 0xC3D5 ==> Row:35 Col:53 */
    {0x8C1D,0xDAD2}, /* 谝: GB2312 Code: 0xDAD2 ==> Row:58 Col:50 */
    {0x8C1F,0xDAD3}, /* 谟: GB2312 Code: 0xDAD3 ==> Row:58 Col:51 */
    {0x8C20,0xDAD4}, /* 谠: GB2312 Code: 0xDAD4 ==> Row:58 Col:52 */
    {0x8C21,0xDAD5}, /* 谡: GB2312 Code: 0xDAD5 ==> Row:58 Col:53 */
    {0x8C22,0xD0BB}, /* 谢: GB2312 Code: 0xD0BB ==> Row:48 Col:27 */
    {0x8C23,0xD2A5}, /* 谣: GB2312 Code: 0xD2A5 ==> Row:50 Col:05 */
    {0x8C24,0xB0F9}, /* 谤: GB2312 Code: 0xB0F9 ==> Row:16 Col:89 */
    {0x8C25,0xDAD6}, /* 谥: GB2312 Code: 0xDAD6 ==> Row:58 Col:54 */
    {0x8C26,0xC7AB}, /* 谦: GB2312 Code: 0xC7AB ==> Row:39 Col:11 */
    {0x8C27,0xDAD7}, /* 谧: GB2312 Code: 0xDAD7 ==> Row:58 Col:55 */
    {0x8C28,0xBDF7}, /* 谨: GB2312 Code: 0xBDF7 ==> Row:29 Col:87 */
    {0x8C29,0xC3A1}, /* 谩: GB2312 Code: 0xC3A1 ==> Row:35 Col:01 */
    {0x8C2A,0xDAD8}, /* 谪: GB2312 Code: 0xDAD8 ==> Row:58 Col:56 */
    {0x8C2B,0xDAD9}, /* 谫: GB2312 Code: 0xDAD9 ==> Row:58 Col:57 */
    {0x8C2C,0xC3FD}, /* 谬: GB2312 Code: 0xC3FD ==> Row:35 Col:93 */
    {0x8C2D,0xCCB7}, /* 谭: GB2312 Code: 0xCCB7 ==> Row:44 Col:23 */
    {0x8C2E,0xDADA}, /* 谮: GB2312 Code: 0xDADA ==> Row:58 Col:58 */
    {0x8C2F,0xDADB}, /* 谯: GB2312 Code: 0xDADB ==> Row:58 Col:59 */
    {0x8C30,0xC0BE}, /* 谰: GB2312 Code: 0xC0BE ==> Row:32 Col:30 */
    {0x8C31,0xC6D7}, /* 谱: GB2312 Code: 0xC6D7 ==> Row:38 Col:55 */
    {0x8C32,0xDADC}, /* 谲: GB2312 Code: 0xDADC ==> Row:58 Col:60 */
    {0x8C33,0xDADD}, /* 谳: GB2312 Code: 0xDADD ==> Row:58 Col:61 */
    {0x8C34,0xC7B4}, /* 谴: GB2312 Code: 0xC7B4 ==> Row:39 Col:20 */
    {0x8C35,0xDADE}, /* 谵: GB2312 Code: 0xDADE ==> Row:58 Col:62 */
    {0x8C36,0xDADF}, /* 谶: GB2312 Code: 0xDADF ==> Row:58 Col:63 */
    {0x8C37,0xB9C8}, /* 谷: GB2312 Code: 0xB9C8 ==> Row:25 Col:40 */
    {0x8C41,0xBBED}, /* 豁: GB2312 Code: 0xBBED ==> Row:27 Col:77 */
    {0x8C46,0xB6B9}, /* 豆: GB2312 Code: 0xB6B9 ==> Row:22 Col:25 */
    {0x8C47,0xF4F8}, /* 豇: GB2312 Code: 0xF4F8 ==> Row:84 Col:88 */
    {0x8C49,0xF4F9}, /* 豉: GB2312 Code: 0xF4F9 ==> Row:84 Col:89 */
    {0x8C4C,0xCDE3}, /* 豌: GB2312 Code: 0xCDE3 ==> Row:45 Col:67 */
    {0x8C55,0xF5B9}, /* 豕: GB2312 Code: 0xF5B9 ==> Row:85 Col:25 */
    {0x8C5A,0xEBE0}, /* 豚: GB2312 Code: 0xEBE0 ==> Row:75 Col:64 */
    {0x8C61,0xCFF3}, /* 象: GB2312 Code: 0xCFF3 ==> Row:47 Col:83 */
    {0x8C62,0xBBBF}, /* 豢: GB2312 Code: 0xBBBF ==> Row:27 Col:31 */
    {0x8C6A,0xBAC0}, /* 豪: GB2312 Code: 0xBAC0 ==> Row:26 Col:32 */
    {0x8C6B,0xD4A5}, /* 豫: GB2312 Code: 0xD4A5 ==> Row:52 Col:05 */
    {0x8C73,0xE1D9}, /* 豳: GB2312 Code: 0xE1D9 ==> Row:65 Col:57 */
    {0x8C78,0xF5F4}, /* 豸: GB2312 Code: 0xF5F4 ==> Row:85 Col:84 */
    {0x8C79,0xB1AA}, /* 豹: GB2312 Code: 0xB1AA ==> Row:17 Col:10 */
    {0x8C7A,0xB2F2}, /* 豺: GB2312 Code: 0xB2F2 ==> Row:18 Col:82 */
    {0x8C82,0xF5F5}, /* 貂: GB2312 Code: 0xF5F5 ==> Row:85 Col:85 */
    {0x8C85,0xF5F7}, /* 貅: GB2312 Code: 0xF5F7 ==> Row:85 Col:87 */
    {0x8C89,0xBAD1}, /* 貉: GB2312 Code: 0xBAD1 ==> Row:26 Col:49 */
    {0x8C8A,0xF5F6}, /* 貊: GB2312 Code: 0xF5F6 ==> Row:85 Col:86 */
    {0x8C8C,0xC3B2}, /* 貌: GB2312 Code: 0xC3B2 ==> Row:35 Col:18 */
    {0x8C94,0xF5F9}, /* 貔: GB2312 Code: 0xF5F9 ==> Row:85 Col:89 */
    {0x8C98,0xF5F8}, /* 貘: GB2312 Code: 0xF5F8 ==> Row:85 Col:88 */
    {0x8D1D,0xB1B4}, /* 贝: GB2312 Code: 0xB1B4 ==> Row:17 Col:20 */
    {0x8D1E,0xD5EA}, /* 贞: GB2312 Code: 0xD5EA ==> Row:53 Col:74 */
    {0x8D1F,0xB8BA}, /* 负: GB2312 Code: 0xB8BA ==> Row:24 Col:26 */
    {0x8D21,0xB9B1}, /* 贡: GB2312 Code: 0xB9B1 ==> Row:25 Col:17 */
    {0x8D22,0xB2C6}, /* 财: GB2312 Code: 0xB2C6 ==> Row:18 Col:38 */
    {0x8D23,0xD4F0}, /* 责: GB2312 Code: 0xD4F0 ==> Row:52 Col:80 */
    {0x8D24,0xCFCD}, /* 贤: GB2312 Code: 0xCFCD ==> Row:47 Col:45 */
    {0x8D25,0xB0DC}, /* 败: GB2312 Code: 0xB0DC ==> Row:16 Col:60 */
    {0x8D26,0xD5CB}, /* 账: GB2312 Code: 0xD5CB ==> Row:53 Col:43 */
    {0x8D27,0xBBF5}, /* 货: GB2312 Code: 0xBBF5 ==> Row:27 Col:85 */
    {0x8D28,0xD6CA}, /* 质: GB2312 Code: 0xD6CA ==> Row:54 Col:42 */
    {0x8D29,0xB7B7}, /* 贩: GB2312 Code: 0xB7B7 ==> Row:23 Col:23 */
    {0x8D2A,0xCCB0}, /* 贪: GB2312 Code: 0xCCB0 ==> Row:44 Col:16 */
    {0x8D2B,0xC6B6}, /* 贫: GB2312 Code: 0xC6B6 ==> Row:38 Col:22 */
    {0x8D2C,0xB1E1}, /* 贬: GB2312 Code: 0xB1E1 ==> Row:17 Col:65 */
    {0x8D2D,0xB9BA}, /* 购: GB2312 Code: 0xB9BA ==> Row:25 Col:26 */
    {0x8D2E,0xD6FC}, /* 贮: GB2312 Code: 0xD6FC ==> Row:54 Col:92 */
    {0x8D2F,0xB9E1}, /* 贯: GB2312 Code: 0xB9E1 ==> Row:25 Col:65 */
    {0x8D30,0xB7A1}, /* 贰: GB2312 Code: 0xB7A1 ==> Row:23 Col:01 */
    {0x8D31,0xBCFA}, /* 贱: GB2312 Code: 0xBCFA ==> Row:28 Col:90 */
    {0x8D32,0xEADA}, /* 贲: GB2312 Code: 0xEADA ==> Row:74 Col:58 */
    {0x8D33,0xEADB}, /* 贳: GB2312 Code: 0xEADB ==> Row:74 Col:59 */
    {0x8D34,0xCCF9}, /* 贴: GB2312 Code: 0xCCF9 ==> Row:44 Col:89 */
    {0x8D35,0xB9F3}, /* 贵: GB2312 Code: 0xB9F3 ==> Row:25 Col:83 */
    {0x8D36,0xEADC}, /* 贶: GB2312 Code: 0xEADC ==> Row:74 Col:60 */
    {0x8D37,0xB4FB}, /* 贷: GB2312 Code: 0xB4FB ==> Row:20 Col:91 */
    {0x8D38,0xC3B3}, /* 贸: GB2312 Code: 0xC3B3 ==> Row:35 Col:19 */
    {0x8D39,0xB7D1}, /* 费: GB2312 Code: 0xB7D1 ==> Row:23 Col:49 */
    {0x8D3A,0xBAD8}, /* 贺: GB2312 Code: 0xBAD8 ==> Row:26 Col:56 */
    {0x8D3B,0xEADD}, /* 贻: GB2312 Code: 0xEADD ==> Row:74 Col:61 */
    {0x8D3C,0xD4F4}, /* 贼: GB2312 Code: 0xD4F4 ==> Row:52 Col:84 */
    {0x8D3D,0xEADE}, /* 贽: GB2312 Code: 0xEADE ==> Row:74 Col:62 */
    {0x8D3E,0xBCD6}, /* 贾: GB2312 Code: 0xBCD6 ==> Row:28 Col:54 */
    {0x8D3F,0xBBDF}, /* 贿: GB2312 Code: 0xBBDF ==> Row:27 Col:63 */
    {0x8D40,0xEADF}, /* 赀: GB2312 Code: 0xEADF ==> Row:74 Col:63 */
    {0x8D41,0xC1DE}, /* 赁: GB2312 Code: 0xC1DE ==> Row:33 Col:62 */
    {0x8D42,0xC2B8}, /* 赂: GB2312 Code: 0xC2B8 ==> Row:34 Col:24 */
    {0x8D43,0xD4DF}, /* 赃: GB2312 Code: 0xD4DF ==> Row:52 Col:63 */
    {0x8D44,0xD7CA}, /* 资: GB2312 Code: 0xD7CA ==> Row:55 Col:42 */
    {0x8D45,0xEAE0}, /* 赅: GB2312 Code: 0xEAE0 ==> Row:74 Col:64 */
    {0x8D46,0xEAE1}, /* 赆: GB2312 Code: 0xEAE1 ==> Row:74 Col:65 */
    {0x8D47,0xEAE4}, /* 赇: GB2312 Code: 0xEAE4 ==> Row:74 Col:68 */
    {0x8D48,0xEAE2}, /* 赈: GB2312 Code: 0xEAE2 ==> Row:74 Col:66 */
    {0x8D49,0xEAE3}, /* 赉: GB2312 Code: 0xEAE3 ==> Row:74 Col:67 */
    {0x8D4A,0xC9DE}, /* 赊: GB2312 Code: 0xC9DE ==> Row:41 Col:62 */
    {0x8D4B,0xB8B3}, /* 赋: GB2312 Code: 0xB8B3 ==> Row:24 Col:19 */
    {0x8D4C,0xB6C4}, /* 赌: GB2312 Code: 0xB6C4 ==> Row:22 Col:36 */
    {0x8D4D,0xEAE5}, /* 赍: GB2312 Code: 0xEAE5 ==> Row:74 Col:69 */
    {0x8D4E,0xCAEA}, /* 赎: GB2312 Code: 0xCAEA ==> Row:42 Col:74 */
    {0x8D4F,0xC9CD}, /* 赏: GB2312 Code: 0xC9CD ==> Row:41 Col:45 */
    {0x8D50,0xB4CD}, /* 赐: GB2312 Code: 0xB4CD ==> Row:20 Col:45 */
    {0x8D53,0xE2D9}, /* 赓: GB2312 Code: 0xE2D9 ==> Row:66 Col:57 */
    {0x8D54,0xC5E2}, /* 赔: GB2312 Code: 0xC5E2 ==> Row:37 Col:66 */
    {0x8D55,0xEAE6}, /* 赕: GB2312 Code: 0xEAE6 ==> Row:74 Col:70 */
    {0x8D56,0xC0B5}, /* 赖: GB2312 Code: 0xC0B5 ==> Row:32 Col:21 */
    {0x8D58,0xD7B8}, /* 赘: GB2312 Code: 0xD7B8 ==> Row:55 Col:24 */
    {0x8D59,0xEAE7}, /* 赙: GB2312 Code: 0xEAE7 ==> Row:74 Col:71 */
    {0x8D5A,0xD7AC}, /* 赚: GB2312 Code: 0xD7AC ==> Row:55 Col:12 */
    {0x8D5B,0xC8FC}, /* 赛: GB2312 Code: 0xC8FC ==> Row:40 Col:92 */
    {0x8D5C,0xD8D3}, /* 赜: GB2312 Code: 0xD8D3 ==> Row:56 Col:51 */
    {0x8D5D,0xD8CD}, /* 赝: GB2312 Code: 0xD8CD ==> Row:56 Col:45 */
    {0x8D5E,0xD4DE}, /* 赞: GB2312 Code: 0xD4DE ==> Row:52 Col:62 */
    {0x8D60,0xD4F9}, /* 赠: GB2312 Code: 0xD4F9 ==> Row:52 Col:89 */
    {0x8D61,0xC9C4}, /* 赡: GB2312 Code: 0xC9C4 ==> Row:41 Col:36 */
    {0x8D62,0xD3AE}, /* 赢: GB2312 Code: 0xD3AE ==> Row:51 Col:14 */
    {0x8D63,0xB8D3}, /* 赣: GB2312 Code: 0xB8D3 ==> Row:24 Col:51 */
    {0x8D64,0xB3E0}, /* 赤: GB2312 Code: 0xB3E0 ==> Row:19 Col:64 */
    {0x8D66,0xC9E2}, /* 赦: GB2312 Code: 0xC9E2 ==> Row:41 Col:66 */
    {0x8D67,0xF4F6}, /* 赧: GB2312 Code: 0xF4F6 ==> Row:84 Col:86 */
    {0x8D6B,0xBAD5}, /* 赫: GB2312 Code: 0xBAD5 ==> Row:26 Col:53 */
    {0x8D6D,0xF4F7}, /* 赭: GB2312 Code: 0xF4F7 ==> Row:84 Col:87 */
    {0x8D70,0xD7DF}, /* 走: GB2312 Code: 0xD7DF ==> Row:55 Col:63 */
    {0x8D73,0xF4F1}, /* 赳: GB2312 Code: 0xF4F1 ==> Row:84 Col:81 */
    {0x8D74,0xB8B0}, /* 赴: GB2312 Code: 0xB8B0 ==> Row:24 Col:16 */
    {0x8D75,0xD5D4}, /* 赵: GB2312 Code: 0xD5D4 ==> Row:53 Col:52 */
    {0x8D76,0xB8CF}, /* 赶: GB2312 Code: 0xB8CF ==> Row:24 Col:47 */
    {0x8D77,0xC6F0}, /* 起: GB2312 Code: 0xC6F0 ==> Row:38 Col:80 */
    {0x8D81,0xB3C3}, /* 趁: GB2312 Code: 0xB3C3 ==> Row:19 Col:35 */
    {0x8D84,0xF4F2}, /* 趄: GB2312 Code: 0xF4F2 ==> Row:84 Col:82 */
    {0x8D85,0xB3AC}, /* 超: GB2312 Code: 0xB3AC ==> Row:19 Col:12 */
    {0x8D8A,0xD4BD}, /* 越: GB2312 Code: 0xD4BD ==> Row:52 Col:29 */
    {0x8D8B,0xC7F7}, /* 趋: GB2312 Code: 0xC7F7 ==> Row:39 Col:87 */
    {0x8D91,0xF4F4}, /* 趑: GB2312 Code: 0xF4F4 ==> Row:84 Col:84 */
    {0x8D94,0xF4F3}, /* 趔: GB2312 Code: 0xF4F3 ==> Row:84 Col:83 */
    {0x8D9F,0xCCCB}, /* 趟: GB2312 Code: 0xCCCB ==> Row:44 Col:43 */
    {0x8DA3,0xC8A4}, /* 趣: GB2312 Code: 0xC8A4 ==> Row:40 Col:04 */
    {0x8DB1,0xF4F5}, /* 趱: GB2312 Code: 0xF4F5 ==> Row:84 Col:85 */
    {0x8DB3,0xD7E3}, /* 足: GB2312 Code: 0xD7E3 ==> Row:55 Col:67 */
    {0x8DB4,0xC5BF}, /* 趴: GB2312 Code: 0xC5BF ==> Row:37 Col:31 */
    {0x8DB5,0xF5C0}, /* 趵: GB2312 Code: 0xF5C0 ==> Row:85 Col:32 */
    {0x8DB8,0xF5BB}, /* 趸: GB2312 Code: 0xF5BB ==> Row:85 Col:27 */
    {0x8DBA,0xF5C3}, /* 趺: GB2312 Code: 0xF5C3 ==> Row:85 Col:35 */
    {0x8DBC,0xF5C2}, /* 趼: GB2312 Code: 0xF5C2 ==> Row:85 Col:34 */
    {0x8DBE,0xD6BA}, /* 趾: GB2312 Code: 0xD6BA ==> Row:54 Col:26 */
    {0x8DBF,0xF5C1}, /* 趿: GB2312 Code: 0xF5C1 ==> Row:85 Col:33 */
    {0x8DC3,0xD4BE}, /* 跃: GB2312 Code: 0xD4BE ==> Row:52 Col:30 */
    {0x8DC4,0xF5C4}, /* 跄: GB2312 Code: 0xF5C4 ==> Row:85 Col:36 */
    {0x8DC6,0xF5CC}, /* 跆: GB2312 Code: 0xF5CC ==> Row:85 Col:44 */
    {0x8DCB,0xB0CF}, /* 跋: GB2312 Code: 0xB0CF ==> Row:16 Col:47 */
    {0x8DCC,0xB5F8}, /* 跌: GB2312 Code: 0xB5F8 ==> Row:21 Col:88 */
    {0x8DCE,0xF5C9}, /* 跎: GB2312 Code: 0xF5C9 ==> Row:85 Col:41 */
    {0x8DCF,0xF5CA}, /* 跏: GB2312 Code: 0xF5CA ==> Row:85 Col:42 */
    {0x8DD1,0xC5DC}, /* 跑: GB2312 Code: 0xC5DC ==> Row:37 Col:60 */
    {0x8DD6,0xF5C5}, /* 跖: GB2312 Code: 0xF5C5 ==> Row:85 Col:37 */
    {0x8DD7,0xF5C6}, /* 跗: GB2312 Code: 0xF5C6 ==> Row:85 Col:38 */
    {0x8DDA,0xF5C7}, /* 跚: GB2312 Code: 0xF5C7 ==> Row:85 Col:39 */
    {0x8DDB,0xF5CB}, /* 跛: GB2312 Code: 0xF5CB ==> Row:85 Col:43 */
    {0x8DDD,0xBEE0}, /* 距: GB2312 Code: 0xBEE0 ==> Row:30 Col:64 */
    {0x8DDE,0xF5C8}, /* 跞: GB2312 Code: 0xF5C8 ==> Row:85 Col:40 */
    {0x8DDF,0xB8FA}, /* 跟: GB2312 Code: 0xB8FA ==> Row:24 Col:90 */
    {0x8DE3,0xF5D0}, /* 跣: GB2312 Code: 0xF5D0 ==> Row:85 Col:48 */
    {0x8DE4,0xF5D3}, /* 跤: GB2312 Code: 0xF5D3 ==> Row:85 Col:51 */
    {0x8DE8,0xBFE7}, /* 跨: GB2312 Code: 0xBFE7 ==> Row:31 Col:71 */
    {0x8DEA,0xB9F2}, /* 跪: GB2312 Code: 0xB9F2 ==> Row:25 Col:82 */
    {0x8DEB,0xF5BC}, /* 跫: GB2312 Code: 0xF5BC ==> Row:85 Col:28 */
    {0x8DEC,0xF5CD}, /* 跬: GB2312 Code: 0xF5CD ==> Row:85 Col:45 */
    {0x8DEF,0xC2B7}, /* 路: GB2312 Code: 0xC2B7 ==> Row:34 Col:23 */
    {0x8DF3,0xCCF8}, /* 跳: GB2312 Code: 0xCCF8 ==> Row:44 Col:88 */
    {0x8DF5,0xBCF9}, /* 践: GB2312 Code: 0xBCF9 ==> Row:28 Col:89 */
    {0x8DF7,0xF5CE}, /* 跷: GB2312 Code: 0xF5CE ==> Row:85 Col:46 */
    {0x8DF8,0xF5CF}, /* 跸: GB2312 Code: 0xF5CF ==> Row:85 Col:47 */
    {0x8DF9,0xF5D1}, /* 跹: GB2312 Code: 0xF5D1 ==> Row:85 Col:49 */
    {0x8DFA,0xB6E5}, /* 跺: GB2312 Code: 0xB6E5 ==> Row:22 Col:69 */
    {0x8DFB,0xF5D2}, /* 跻: GB2312 Code: 0xF5D2 ==> Row:85 Col:50 */
    {0x8DFD,0xF5D5}, /* 跽: GB2312 Code: 0xF5D5 ==> Row:85 Col:53 */
    {0x8E05,0xF5BD}, /* 踅: GB2312 Code: 0xF5BD ==> Row:85 Col:29 */
    {0x8E09,0xF5D4}, /* 踉: GB2312 Code: 0xF5D4 ==> Row:85 Col:52 */
    {0x8E0A,0xD3BB}, /* 踊: GB2312 Code: 0xD3BB ==> Row:51 Col:27 */
    {0x8E0C,0xB3EC}, /* 踌: GB2312 Code: 0xB3EC ==> Row:19 Col:76 */
    {0x8E0F,0xCCA4}, /* 踏: GB2312 Code: 0xCCA4 ==> Row:44 Col:04 */
    {0x8E14,0xF5D6}, /* 踔: GB2312 Code: 0xF5D6 ==> Row:85 Col:54 */
    {0x8E1D,0xF5D7}, /* 踝: GB2312 Code: 0xF5D7 ==> Row:85 Col:55 */
    {0x8E1E,0xBEE1}, /* 踞: GB2312 Code: 0xBEE1 ==> Row:30 Col:65 */
    {0x8E1F,0xF5D8}, /* 踟: GB2312 Code: 0xF5D8 ==> Row:85 Col:56 */
    {0x8E22,0xCCDF}, /* 踢: GB2312 Code: 0xCCDF ==> Row:44 Col:63 */
    {0x8E23,0xF5DB}, /* 踣: GB2312 Code: 0xF5DB ==> Row:85 Col:59 */
    {0x8E29,0xB2C8}, /* 踩: GB2312 Code: 0xB2C8 ==> Row:18 Col:40 */
    {0x8E2A,0xD7D9}, /* 踪: GB2312 Code: 0xD7D9 ==> Row:55 Col:57 */
    {0x8E2C,0xF5D9}, /* 踬: GB2312 Code: 0xF5D9 ==> Row:85 Col:57 */
    {0x8E2E,0xF5DA}, /* 踮: GB2312 Code: 0xF5DA ==> Row:85 Col:58 */
    {0x8E2F,0xF5DC}, /* 踯: GB2312 Code: 0xF5DC ==> Row:85 Col:60 */
    {0x8E31,0xF5E2}, /* 踱: GB2312 Code: 0xF5E2 ==> Row:85 Col:66 */
    {0x8E35,0xF5E0}, /* 踵: GB2312 Code: 0xF5E0 ==> Row:85 Col:64 */
    {0x8E39,0xF5DF}, /* 踹: GB2312 Code: 0xF5DF ==> Row:85 Col:63 */
    {0x8E3A,0xF5DD}, /* 踺: GB2312 Code: 0xF5DD ==> Row:85 Col:61 */
    {0x8E3D,0xF5E1}, /* 踽: GB2312 Code: 0xF5E1 ==> Row:85 Col:65 */
    {0x8E40,0xF5DE}, /* 蹀: GB2312 Code: 0xF5DE ==> Row:85 Col:62 */
    {0x8E41,0xF5E4}, /* 蹁: GB2312 Code: 0xF5E4 ==> Row:85 Col:68 */
    {0x8E42,0xF5E5}, /* 蹂: GB2312 Code: 0xF5E5 ==> Row:85 Col:69 */
    {0x8E44,0xCCE3}, /* 蹄: GB2312 Code: 0xCCE3 ==> Row:44 Col:67 */
    {0x8E47,0xE5BF}, /* 蹇: GB2312 Code: 0xE5BF ==> Row:69 Col:31 */
    {0x8E48,0xB5B8}, /* 蹈: GB2312 Code: 0xB5B8 ==> Row:21 Col:24 */
    {0x8E49,0xF5E3}, /* 蹉: GB2312 Code: 0xF5E3 ==> Row:85 Col:67 */
    {0x8E4A,0xF5E8}, /* 蹊: GB2312 Code: 0xF5E8 ==> Row:85 Col:72 */
    {0x8E4B,0xCCA3}, /* 蹋: GB2312 Code: 0xCCA3 ==> Row:44 Col:03 */
    {0x8E51,0xF5E6}, /* 蹑: GB2312 Code: 0xF5E6 ==> Row:85 Col:70 */
    {0x8E52,0xF5E7}, /* 蹒: GB2312 Code: 0xF5E7 ==> Row:85 Col:71 */
    {0x8E59,0xF5BE}, /* 蹙: GB2312 Code: 0xF5BE ==> Row:85 Col:30 */
    {0x8E66,0xB1C4}, /* 蹦: GB2312 Code: 0xB1C4 ==> Row:17 Col:36 */
    {0x8E69,0xF5BF}, /* 蹩: GB2312 Code: 0xF5BF ==> Row:85 Col:31 */
    {0x8E6C,0xB5C5}, /* 蹬: GB2312 Code: 0xB5C5 ==> Row:21 Col:37 */
    {0x8E6D,0xB2E4}, /* 蹭: GB2312 Code: 0xB2E4 ==> Row:18 Col:68 */
    {0x8E6F,0xF5EC}, /* 蹯: GB2312 Code: 0xF5EC ==> Row:85 Col:76 */
    {0x8E70,0xF5E9}, /* 蹰: GB2312 Code: 0xF5E9 ==> Row:85 Col:73 */
    {0x8E72,0xB6D7}, /* 蹲: GB2312 Code: 0xB6D7 ==> Row:22 Col:55 */
    {0x8E74,0xF5ED}, /* 蹴: GB2312 Code: 0xF5ED ==> Row:85 Col:77 */
    {0x8E76,0xF5EA}, /* 蹶: GB2312 Code: 0xF5EA ==> Row:85 Col:74 */
    {0x8E7C,0xF5EB}, /* 蹼: GB2312 Code: 0xF5EB ==> Row:85 Col:75 */
    {0x8E7F,0xB4DA}, /* 蹿: GB2312 Code: 0xB4DA ==> Row:20 Col:58 */
    {0x8E81,0xD4EA}, /* 躁: GB2312 Code: 0xD4EA ==> Row:52 Col:74 */
    {0x8E85,0xF5EE}, /* 躅: GB2312 Code: 0xF5EE ==> Row:85 Col:78 */
    {0x8E87,0xB3F9}, /* 躇: GB2312 Code: 0xB3F9 ==> Row:19 Col:89 */
    {0x8E8F,0xF5EF}, /* 躏: GB2312 Code: 0xF5EF ==> Row:85 Col:79 */
    {0x8E90,0xF5F1}, /* 躐: GB2312 Code: 0xF5F1 ==> Row:85 Col:81 */
    {0x8E94,0xF5F0}, /* 躔: GB2312 Code: 0xF5F0 ==> Row:85 Col:80 */
    {0x8E9C,0xF5F2}, /* 躜: GB2312 Code: 0xF5F2 ==> Row:85 Col:82 */
    {0x8E9E,0xF5F3}, /* 躞: GB2312 Code: 0xF5F3 ==> Row:85 Col:83 */
    {0x8EAB,0xC9ED}, /* 身: GB2312 Code: 0xC9ED ==> Row:41 Col:77 */
    {0x8EAC,0xB9AA}, /* 躬: GB2312 Code: 0xB9AA ==> Row:25 Col:10 */
    {0x8EAF,0xC7FB}, /* 躯: GB2312 Code: 0xC7FB ==> Row:39 Col:91 */
    {0x8EB2,0xB6E3}, /* 躲: GB2312 Code: 0xB6E3 ==> Row:22 Col:67 */
    {0x8EBA,0xCCC9}, /* 躺: GB2312 Code: 0xCCC9 ==> Row:44 Col:41 */
    {0x8ECE,0xEAA6}, /* 軎: GB2312 Code: 0xEAA6 ==> Row:74 Col:06 */
    {0x8F66,0xB3B5}, /* 车: GB2312 Code: 0xB3B5 ==> Row:19 Col:21 */
    {0x8F67,0xD4FE}, /* 轧: GB2312 Code: 0xD4FE ==> Row:52 Col:94 */
    {0x8F68,0xB9EC}, /* 轨: GB2312 Code: 0xB9EC ==> Row:25 Col:76 */
    {0x8F69,0xD0F9}, /* 轩: GB2312 Code: 0xD0F9 ==> Row:48 Col:89 */
    {0x8F6B,0xE9ED}, /* 轫: GB2312 Code: 0xE9ED ==> Row:73 Col:77 */
    {0x8F6C,0xD7AA}, /* 转: GB2312 Code: 0xD7AA ==> Row:55 Col:10 */
    {0x8F6D,0xE9EE}, /* 轭: GB2312 Code: 0xE9EE ==> Row:73 Col:78 */
    {0x8F6E,0xC2D6}, /* 轮: GB2312 Code: 0xC2D6 ==> Row:34 Col:54 */
    {0x8F6F,0xC8ED}, /* 软: GB2312 Code: 0xC8ED ==> Row:40 Col:77 */
    {0x8F70,0xBAE4}, /* 轰: GB2312 Code: 0xBAE4 ==> Row:26 Col:68 */
    {0x8F71,0xE9EF}, /* 轱: GB2312 Code: 0xE9EF ==> Row:73 Col:79 */
    {0x8F72,0xE9F0}, /* 轲: GB2312 Code: 0xE9F0 ==> Row:73 Col:80 */
    {0x8F73,0xE9F1}, /* 轳: GB2312 Code: 0xE9F1 ==> Row:73 Col:81 */
    {0x8F74,0xD6E1}, /* 轴: GB2312 Code: 0xD6E1 ==> Row:54 Col:65 */
    {0x8F75,0xE9F2}, /* 轵: GB2312 Code: 0xE9F2 ==> Row:73 Col:82 */
    {0x8F76,0xE9F3}, /* 轶: GB2312 Code: 0xE9F3 ==> Row:73 Col:83 */
    {0x8F77,0xE9F5}, /* 轷: GB2312 Code: 0xE9F5 ==> Row:73 Col:85 */
    {0x8F78,0xE9F4}, /* 轸: GB2312 Code: 0xE9F4 ==> Row:73 Col:84 */
    {0x8F79,0xE9F6}, /* 轹: GB2312 Code: 0xE9F6 ==> Row:73 Col:86 */
    {0x8F7A,0xE9F7}, /* 轺: GB2312 Code: 0xE9F7 ==> Row:73 Col:87 */
    {0x8F7B,0xC7E1}, /* 轻: GB2312 Code: 0xC7E1 ==> Row:39 Col:65 */
    {0x8F7C,0xE9F8}, /* 轼: GB2312 Code: 0xE9F8 ==> Row:73 Col:88 */
    {0x8F7D,0xD4D8}, /* 载: GB2312 Code: 0xD4D8 ==> Row:52 Col:56 */
    {0x8F7E,0xE9F9}, /* 轾: GB2312 Code: 0xE9F9 ==> Row:73 Col:89 */
    {0x8F7F,0xBDCE}, /* 轿: GB2312 Code: 0xBDCE ==> Row:29 Col:46 */
    {0x8F81,0xE9FA}, /* 辁: GB2312 Code: 0xE9FA ==> Row:73 Col:90 */
    {0x8F82,0xE9FB}, /* 辂: GB2312 Code: 0xE9FB ==> Row:73 Col:91 */
    {0x8F83,0xBDCF}, /* 较: GB2312 Code: 0xBDCF ==> Row:29 Col:47 */
    {0x8F84,0xE9FC}, /* 辄: GB2312 Code: 0xE9FC ==> Row:73 Col:92 */
    {0x8F85,0xB8A8}, /* 辅: GB2312 Code: 0xB8A8 ==> Row:24 Col:08 */
    {0x8F86,0xC1BE}, /* 辆: GB2312 Code: 0xC1BE ==> Row:33 Col:30 */
    {0x8F87,0xE9FD}, /* 辇: GB2312 Code: 0xE9FD ==> Row:73 Col:93 */
    {0x8F88,0xB1B2}, /* 辈: GB2312 Code: 0xB1B2 ==> Row:17 Col:18 */
    {0x8F89,0xBBD4}, /* 辉: GB2312 Code: 0xBBD4 ==> Row:27 Col:52 */
    {0x8F8A,0xB9F5}, /* 辊: GB2312 Code: 0xB9F5 ==> Row:25 Col:85 */
    {0x8F8B,0xE9FE}, /* 辋: GB2312 Code: 0xE9FE ==> Row:73 Col:94 */
    {0x8F8D,0xEAA1}, /* 辍: GB2312 Code: 0xEAA1 ==> Row:74 Col:01 */
    {0x8F8E,0xEAA2}, /* 辎: GB2312 Code: 0xEAA2 ==> Row:74 Col:02 */
    {0x8F8F,0xEAA3}, /* 辏: GB2312 Code: 0xEAA3 ==> Row:74 Col:03 */
    {0x8F90,0xB7F8}, /* 辐: GB2312 Code: 0xB7F8 ==> Row:23 Col:88 */
    {0x8F91,0xBCAD}, /* 辑: GB2312 Code: 0xBCAD ==> Row:28 Col:13 */
    {0x8F93,0xCAE4}, /* 输: GB2312 Code: 0xCAE4 ==> Row:42 Col:68 */
    {0x8F94,0xE0CE}, /* 辔: GB2312 Code: 0xE0CE ==> Row:64 Col:46 */
    {0x8F95,0xD4AF}, /* 辕: GB2312 Code: 0xD4AF ==> Row:52 Col:15 */
    {0x8F96,0xCFBD}, /* 辖: GB2312 Code: 0xCFBD ==> Row:47 Col:29 */
    {0x8F97,0xD5B7}, /* 辗: GB2312 Code: 0xD5B7 ==> Row:53 Col:23 */
    {0x8F98,0xEAA4}, /* 辘: GB2312 Code: 0xEAA4 ==> Row:74 Col:04 */
    {0x8F99,0xD5DE}, /* 辙: GB2312 Code: 0xD5DE ==> Row:53 Col:62 */
    {0x8F9A,0xEAA5}, /* 辚: GB2312 Code: 0xEAA5 ==> Row:74 Col:05 */
    {0x8F9B,0xD0C1}, /* 辛: GB2312 Code: 0xD0C1 ==> Row:48 Col:33 */
    {0x8F9C,0xB9BC}, /* 辜: GB2312 Code: 0xB9BC ==> Row:25 Col:28 */
    {0x8F9E,0xB4C7}, /* 辞: GB2312 Code: 0xB4C7 ==> Row:20 Col:39 */
    {0x8F9F,0xB1D9}, /* 辟: GB2312 Code: 0xB1D9 ==> Row:17 Col:57 */
    {0x8FA3,0xC0B1}, /* 辣: GB2312 Code: 0xC0B1 ==> Row:32 Col:17 */
    {0x8FA8,0xB1E6}, /* 辨: GB2312 Code: 0xB1E6 ==> Row:17 Col:70 */
    {0x8FA9,0xB1E7}, /* 辩: GB2312 Code: 0xB1E7 ==> Row:17 Col:71 */
    {0x8FAB,0xB1E8}, /* 辫: GB2312 Code: 0xB1E8 ==> Row:17 Col:72 */
    {0x8FB0,0xB3BD}, /* 辰: GB2312 Code: 0xB3BD ==> Row:19 Col:29 */
    {0x8FB1,0xC8E8}, /* 辱: GB2312 Code: 0xC8E8 ==> Row:40 Col:72 */
    {0x8FB6,0xE5C1}, /* 辶: GB2312 Code: 0xE5C1 ==> Row:69 Col:33 */
    {0x8FB9,0xB1DF}, /* 边: GB2312 Code: 0xB1DF ==> Row:17 Col:63 */
    {0x8FBD,0xC1C9}, /* 辽: GB2312 Code: 0xC1C9 ==> Row:33 Col:41 */
    {0x8FBE,0xB4EF}, /* 达: GB2312 Code: 0xB4EF ==> Row:20 Col:79 */
    {0x8FC1,0xC7A8}, /* 迁: GB2312 Code: 0xC7A8 ==> Row:39 Col:08 */
    {0x8FC2,0xD3D8}, /* 迂: GB2312 Code: 0xD3D8 ==> Row:51 Col:56 */
    {0x8FC4,0xC6F9}, /* 迄: GB2312 Code: 0xC6F9 ==> Row:38 Col:89 */
    {0x8FC5,0xD1B8}, /* 迅: GB2312 Code: 0xD1B8 ==> Row:49 Col:24 */
    {0x8FC7,0xB9FD}, /* 过: GB2312 Code: 0xB9FD ==> Row:25 Col:93 */
    {0x8FC8,0xC2F5}, /* 迈: GB2312 Code: 0xC2F5 ==> Row:34 Col:85 */
    {0x8FCE,0xD3AD}, /* 迎: GB2312 Code: 0xD3AD ==> Row:51 Col:13 */
    {0x8FD0,0xD4CB}, /* 运: GB2312 Code: 0xD4CB ==> Row:52 Col:43 */
    {0x8FD1,0xBDFC}, /* 近: GB2312 Code: 0xBDFC ==> Row:29 Col:92 */
    {0x8FD3,0xE5C2}, /* 迓: GB2312 Code: 0xE5C2 ==> Row:69 Col:34 */
    {0x8FD4,0xB7B5}, /* 返: GB2312 Code: 0xB7B5 ==> Row:23 Col:21 */
    {0x8FD5,0xE5C3}, /* 迕: GB2312 Code: 0xE5C3 ==> Row:69 Col:35 */
    {0x8FD8,0xBBB9}, /* 还: GB2312 Code: 0xBBB9 ==> Row:27 Col:25 */
    {0x8FD9,0xD5E2}, /* 这: GB2312 Code: 0xD5E2 ==> Row:53 Col:66 */
    {0x8FDB,0xBDF8}, /* 进: GB2312 Code: 0xBDF8 ==> Row:29 Col:88 */
    {0x8FDC,0xD4B6}, /* 远: GB2312 Code: 0xD4B6 ==> Row:52 Col:22 */
    {0x8FDD,0xCEA5}, /* 违: GB2312 Code: 0xCEA5 ==> Row:46 Col:05 */
    {0x8FDE,0xC1AC}, /* 连: GB2312 Code: 0xC1AC ==> Row:33 Col:12 */
    {0x8FDF,0xB3D9}, /* 迟: GB2312 Code: 0xB3D9 ==> Row:19 Col:57 */
    {0x8FE2,0xCCF6}, /* 迢: GB2312 Code: 0xCCF6 ==> Row:44 Col:86 */
    {0x8FE4,0xE5C6}, /* 迤: GB2312 Code: 0xE5C6 ==> Row:69 Col:38 */
    {0x8FE5,0xE5C4}, /* 迥: GB2312 Code: 0xE5C4 ==> Row:69 Col:36 */
    {0x8FE6,0xE5C8}, /* 迦: GB2312 Code: 0xE5C8 ==> Row:69 Col:40 */
    {0x8FE8,0xE5CA}, /* 迨: GB2312 Code: 0xE5CA ==> Row:69 Col:42 */
    {0x8FE9,0xE5C7}, /* 迩: GB2312 Code: 0xE5C7 ==> Row:69 Col:39 */
    {0x8FEA,0xB5CF}, /* 迪: GB2312 Code: 0xB5CF ==> Row:21 Col:47 */
    {0x8FEB,0xC6C8}, /* 迫: GB2312 Code: 0xC6C8 ==> Row:38 Col:40 */
    {0x8FED,0xB5FC}, /* 迭: GB2312 Code: 0xB5FC ==> Row:21 Col:92 */
    {0x8FEE,0xE5C5}, /* 迮: GB2312 Code: 0xE5C5 ==> Row:69 Col:37 */
    {0x8FF0,0xCAF6}, /* 述: GB2312 Code: 0xCAF6 ==> Row:42 Col:86 */
    {0x8FF3,0xE5C9}, /* 迳: GB2312 Code: 0xE5C9 ==> Row:69 Col:41 */
    {0x8FF7,0xC3D4}, /* 迷: GB2312 Code: 0xC3D4 ==> Row:35 Col:52 */
    {0x8FF8,0xB1C5}, /* 迸: GB2312 Code: 0xB1C5 ==> Row:17 Col:37 */
    {0x8FF9,0xBCA3}, /* 迹: GB2312 Code: 0xBCA3 ==> Row:28 Col:03 */
    {0x8FFD,0xD7B7}, /* 追: GB2312 Code: 0xD7B7 ==> Row:55 Col:23 */
    {0x9000,0xCDCB}, /* 退: GB2312 Code: 0xCDCB ==> Row:45 Col:43 */
    {0x9001,0xCBCD}, /* 送: GB2312 Code: 0xCBCD ==> Row:43 Col:45 */
    {0x9002,0xCACA}, /* 适: GB2312 Code: 0xCACA ==> Row:42 Col:42 */
    {0x9003,0xCCD3}, /* 逃: GB2312 Code: 0xCCD3 ==> Row:44 Col:51 */
    {0x9004,0xE5CC}, /* 逄: GB2312 Code: 0xE5CC ==> Row:69 Col:44 */
    {0x9005,0xE5CB}, /* 逅: GB2312 Code: 0xE5CB ==> Row:69 Col:43 */
    {0x9006,0xC4E6}, /* 逆: GB2312 Code: 0xC4E6 ==> Row:36 Col:70 */
    {0x9009,0xD1A1}, /* 选: GB2312 Code: 0xD1A1 ==> Row:49 Col:01 */
    {0x900A,0xD1B7}, /* 逊: GB2312 Code: 0xD1B7 ==> Row:49 Col:23 */
    {0x900B,0xE5CD}, /* 逋: GB2312 Code: 0xE5CD ==> Row:69 Col:45 */
    {0x900D,0xE5D0}, /* 逍: GB2312 Code: 0xE5D0 ==> Row:69 Col:48 */
    {0x900F,0xCDB8}, /* 透: GB2312 Code: 0xCDB8 ==> Row:45 Col:24 */
    {0x9010,0xD6F0}, /* 逐: GB2312 Code: 0xD6F0 ==> Row:54 Col:80 */
    {0x9011,0xE5CF}, /* 逑: GB2312 Code: 0xE5CF ==> Row:69 Col:47 */
    {0x9012,0xB5DD}, /* 递: GB2312 Code: 0xB5DD ==> Row:21 Col:61 */
    {0x9014,0xCDBE}, /* 途: GB2312 Code: 0xCDBE ==> Row:45 Col:30 */
    {0x9016,0xE5D1}, /* 逖: GB2312 Code: 0xE5D1 ==> Row:69 Col:49 */
    {0x9017,0xB6BA}, /* 逗: GB2312 Code: 0xB6BA ==> Row:22 Col:26 */
    {0x901A,0xCDA8}, /* 通: GB2312 Code: 0xCDA8 ==> Row:45 Col:08 */
    {0x901B,0xB9E4}, /* 逛: GB2312 Code: 0xB9E4 ==> Row:25 Col:68 */
    {0x901D,0xCAC5}, /* 逝: GB2312 Code: 0xCAC5 ==> Row:42 Col:37 */
    {0x901E,0xB3D1}, /* 逞: GB2312 Code: 0xB3D1 ==> Row:19 Col:49 */
    {0x901F,0xCBD9}, /* 速: GB2312 Code: 0xCBD9 ==> Row:43 Col:57 */
    {0x9020,0xD4EC}, /* 造: GB2312 Code: 0xD4EC ==> Row:52 Col:76 */
    {0x9021,0xE5D2}, /* 逡: GB2312 Code: 0xE5D2 ==> Row:69 Col:50 */
    {0x9022,0xB7EA}, /* 逢: GB2312 Code: 0xB7EA ==> Row:23 Col:74 */
    {0x9026,0xE5CE}, /* 逦: GB2312 Code: 0xE5CE ==> Row:69 Col:46 */
    {0x902D,0xE5D5}, /* 逭: GB2312 Code: 0xE5D5 ==> Row:69 Col:53 */
    {0x902E,0xB4FE}, /* 逮: GB2312 Code: 0xB4FE ==> Row:20 Col:94 */
    {0x902F,0xE5D6}, /* 逯: GB2312 Code: 0xE5D6 ==> Row:69 Col:54 */
    {0x9035,0xE5D3}, /* 逵: GB2312 Code: 0xE5D3 ==> Row:69 Col:51 */
    {0x9036,0xE5D4}, /* 逶: GB2312 Code: 0xE5D4 ==> Row:69 Col:52 */
    {0x9038,0xD2DD}, /* 逸: GB2312 Code: 0xD2DD ==> Row:50 Col:61 */
    {0x903B,0xC2DF}, /* 逻: GB2312 Code: 0xC2DF ==> Row:34 Col:63 */
    {0x903C,0xB1C6}, /* 逼: GB2312 Code: 0xB1C6 ==> Row:17 Col:38 */
    {0x903E,0xD3E2}, /* 逾: GB2312 Code: 0xD3E2 ==> Row:51 Col:66 */
    {0x9041,0xB6DD}, /* 遁: GB2312 Code: 0xB6DD ==> Row:22 Col:61 */
    {0x9042,0xCBEC}, /* 遂: GB2312 Code: 0xCBEC ==> Row:43 Col:76 */
    {0x9044,0xE5D7}, /* 遄: GB2312 Code: 0xE5D7 ==> Row:69 Col:55 */
    {0x9047,0xD3F6}, /* 遇: GB2312 Code: 0xD3F6 ==> Row:51 Col:86 */
    {0x904D,0xB1E9}, /* 遍: GB2312 Code: 0xB1E9 ==> Row:17 Col:73 */
    {0x904F,0xB6F4}, /* 遏: GB2312 Code: 0xB6F4 ==> Row:22 Col:84 */
    {0x9050,0xE5DA}, /* 遐: GB2312 Code: 0xE5DA ==> Row:69 Col:58 */
    {0x9051,0xE5D8}, /* 遑: GB2312 Code: 0xE5D8 ==> Row:69 Col:56 */
    {0x9052,0xE5D9}, /* 遒: GB2312 Code: 0xE5D9 ==> Row:69 Col:57 */
    {0x9053,0xB5C0}, /* 道: GB2312 Code: 0xB5C0 ==> Row:21 Col:32 */
    {0x9057,0xD2C5}, /* 遗: GB2312 Code: 0xD2C5 ==> Row:50 Col:37 */
    {0x9058,0xE5DC}, /* 遘: GB2312 Code: 0xE5DC ==> Row:69 Col:60 */
    {0x905B,0xE5DE}, /* 遛: GB2312 Code: 0xE5DE ==> Row:69 Col:62 */
    {0x9062,0xE5DD}, /* 遢: GB2312 Code: 0xE5DD ==> Row:69 Col:61 */
    {0x9063,0xC7B2}, /* 遣: GB2312 Code: 0xC7B2 ==> Row:39 Col:18 */
    {0x9065,0xD2A3}, /* 遥: GB2312 Code: 0xD2A3 ==> Row:50 Col:03 */
    {0x9068,0xE5DB}, /* 遨: GB2312 Code: 0xE5DB ==> Row:69 Col:59 */
    {0x906D,0xD4E2}, /* 遭: GB2312 Code: 0xD4E2 ==> Row:52 Col:66 */
    {0x906E,0xD5DA}, /* 遮: GB2312 Code: 0xD5DA ==> Row:53 Col:58 */
    {0x9074,0xE5E0}, /* 遴: GB2312 Code: 0xE5E0 ==> Row:69 Col:64 */
    {0x9075,0xD7F1}, /* 遵: GB2312 Code: 0xD7F1 ==> Row:55 Col:81 */
    {0x907D,0xE5E1}, /* 遽: GB2312 Code: 0xE5E1 ==> Row:69 Col:65 */
    {0x907F,0xB1DC}, /* 避: GB2312 Code: 0xB1DC ==> Row:17 Col:60 */
    {0x9080,0xD1FB}, /* 邀: GB2312 Code: 0xD1FB ==> Row:49 Col:91 */
    {0x9082,0xE5E2}, /* 邂: GB2312 Code: 0xE5E2 ==> Row:69 Col:66 */
    {0x9083,0xE5E4}, /* 邃: GB2312 Code: 0xE5E4 ==> Row:69 Col:68 */
    {0x9088,0xE5E3}, /* 邈: GB2312 Code: 0xE5E3 ==> Row:69 Col:67 */
    {0x908B,0xE5E5}, /* 邋: GB2312 Code: 0xE5E5 ==> Row:69 Col:69 */
    {0x9091,0xD2D8}, /* 邑: GB2312 Code: 0xD2D8 ==> Row:50 Col:56 */
    {0x9093,0xB5CB}, /* 邓: GB2312 Code: 0xB5CB ==> Row:21 Col:43 */
    {0x9095,0xE7DF}, /* 邕: GB2312 Code: 0xE7DF ==> Row:71 Col:63 */
    {0x9097,0xDAF5}, /* 邗: GB2312 Code: 0xDAF5 ==> Row:58 Col:85 */
    {0x9099,0xDAF8}, /* 邙: GB2312 Code: 0xDAF8 ==> Row:58 Col:88 */
    {0x909B,0xDAF6}, /* 邛: GB2312 Code: 0xDAF6 ==> Row:58 Col:86 */
    {0x909D,0xDAF7}, /* 邝: GB2312 Code: 0xDAF7 ==> Row:58 Col:87 */
    {0x90A1,0xDAFA}, /* 邡: GB2312 Code: 0xDAFA ==> Row:58 Col:90 */
    {0x90A2,0xD0CF}, /* 邢: GB2312 Code: 0xD0CF ==> Row:48 Col:47 */
    {0x90A3,0xC4C7}, /* 那: GB2312 Code: 0xC4C7 ==> Row:36 Col:39 */
    {0x90A6,0xB0EE}, /* 邦: GB2312 Code: 0xB0EE ==> Row:16 Col:78 */
    {0x90AA,0xD0B0}, /* 邪: GB2312 Code: 0xD0B0 ==> Row:48 Col:16 */
    {0x90AC,0xDAF9}, /* 邬: GB2312 Code: 0xDAF9 ==> Row:58 Col:89 */
    {0x90AE,0xD3CA}, /* 邮: GB2312 Code: 0xD3CA ==> Row:51 Col:42 */
    {0x90AF,0xBAAA}, /* 邯: GB2312 Code: 0xBAAA ==> Row:26 Col:10 */
    {0x90B0,0xDBA2}, /* 邰: GB2312 Code: 0xDBA2 ==> Row:59 Col:02 */
    {0x90B1,0xC7F1}, /* 邱: GB2312 Code: 0xC7F1 ==> Row:39 Col:81 */
    {0x90B3,0xDAFC}, /* 邳: GB2312 Code: 0xDAFC ==> Row:58 Col:92 */
    {0x90B4,0xDAFB}, /* 邴: GB2312 Code: 0xDAFB ==> Row:58 Col:91 */
    {0x90B5,0xC9DB}, /* 邵: GB2312 Code: 0xC9DB ==> Row:41 Col:59 */
    {0x90B6,0xDAFD}, /* 邶: GB2312 Code: 0xDAFD ==> Row:58 Col:93 */
    {0x90B8,0xDBA1}, /* 邸: GB2312 Code: 0xDBA1 ==> Row:59 Col:01 */
    {0x90B9,0xD7DE}, /* 邹: GB2312 Code: 0xD7DE ==> Row:55 Col:62 */
    {0x90BA,0xDAFE}, /* 邺: GB2312 Code: 0xDAFE ==> Row:58 Col:94 */
    {0x90BB,0xC1DA}, /* 邻: GB2312 Code: 0xC1DA ==> Row:33 Col:58 */
    {0x90BE,0xDBA5}, /* 邾: GB2312 Code: 0xDBA5 ==> Row:59 Col:05 */
    {0x90C1,0xD3F4}, /* 郁: GB2312 Code: 0xD3F4 ==> Row:51 Col:84 */
    {0x90C4,0xDBA7}, /* 郄: GB2312 Code: 0xDBA7 ==> Row:59 Col:07 */
    {0x90C5,0xDBA4}, /* 郅: GB2312 Code: 0xDBA4 ==> Row:59 Col:04 */
    {0x90C7,0xDBA8}, /* 郇: GB2312 Code: 0xDBA8 ==> Row:59 Col:08 */
    {0x90CA,0xBDBC}, /* 郊: GB2312 Code: 0xBDBC ==> Row:29 Col:28 */
    {0x90CE,0xC0C9}, /* 郎: GB2312 Code: 0xC0C9 ==> Row:32 Col:41 */
    {0x90CF,0xDBA3}, /* 郏: GB2312 Code: 0xDBA3 ==> Row:59 Col:03 */
    {0x90D0,0xDBA6}, /* 郐: GB2312 Code: 0xDBA6 ==> Row:59 Col:06 */
    {0x90D1,0xD6A3}, /* 郑: GB2312 Code: 0xD6A3 ==> Row:54 Col:03 */
    {0x90D3,0xDBA9}, /* 郓: GB2312 Code: 0xDBA9 ==> Row:59 Col:09 */
    {0x90D7,0xDBAD}, /* 郗: GB2312 Code: 0xDBAD ==> Row:59 Col:13 */
    {0x90DB,0xDBAE}, /* 郛: GB2312 Code: 0xDBAE ==> Row:59 Col:14 */
    {0x90DC,0xDBAC}, /* 郜: GB2312 Code: 0xDBAC ==> Row:59 Col:12 */
    {0x90DD,0xBAC2}, /* 郝: GB2312 Code: 0xBAC2 ==> Row:26 Col:34 */
    {0x90E1,0xBFA4}, /* 郡: GB2312 Code: 0xBFA4 ==> Row:31 Col:04 */
    {0x90E2,0xDBAB}, /* 郢: GB2312 Code: 0xDBAB ==> Row:59 Col:11 */
    {0x90E6,0xDBAA}, /* 郦: GB2312 Code: 0xDBAA ==> Row:59 Col:10 */
    {0x90E7,0xD4C7}, /* 郧: GB2312 Code: 0xD4C7 ==> Row:52 Col:39 */
    {0x90E8,0xB2BF}, /* 部: GB2312 Code: 0xB2BF ==> Row:18 Col:31 */
    {0x90EB,0xDBAF}, /* 郫: GB2312 Code: 0xDBAF ==> Row:59 Col:15 */
    {0x90ED,0xB9F9}, /* 郭: GB2312 Code: 0xB9F9 ==> Row:25 Col:89 */
    {0x90EF,0xDBB0}, /* 郯: GB2312 Code: 0xDBB0 ==> Row:59 Col:16 */
    {0x90F4,0xB3BB}, /* 郴: GB2312 Code: 0xB3BB ==> Row:19 Col:27 */
    {0x90F8,0xB5A6}, /* 郸: GB2312 Code: 0xB5A6 ==> Row:21 Col:06 */
    {0x90FD,0xB6BC}, /* 都: GB2312 Code: 0xB6BC ==> Row:22 Col:28 */
    {0x90FE,0xDBB1}, /* 郾: GB2312 Code: 0xDBB1 ==> Row:59 Col:17 */
    {0x9102,0xB6F5}, /* 鄂: GB2312 Code: 0xB6F5 ==> Row:22 Col:85 */
    {0x9104,0xDBB2}, /* 鄄: GB2312 Code: 0xDBB2 ==> Row:59 Col:18 */
    {0x9119,0xB1C9}, /* 鄙: GB2312 Code: 0xB1C9 ==> Row:17 Col:41 */
    {0x911E,0xDBB4}, /* 鄞: GB2312 Code: 0xDBB4 ==> Row:59 Col:20 */
    {0x9122,0xDBB3}, /* 鄢: GB2312 Code: 0xDBB3 ==> Row:59 Col:19 */
    {0x9123,0xDBB5}, /* 鄣: GB2312 Code: 0xDBB5 ==> Row:59 Col:21 */
    {0x912F,0xDBB7}, /* 鄯: GB2312 Code: 0xDBB7 ==> Row:59 Col:23 */
    {0x9131,0xDBB6}, /* 鄱: GB2312 Code: 0xDBB6 ==> Row:59 Col:22 */
    {0x9139,0xDBB8}, /* 鄹: GB2312 Code: 0xDBB8 ==> Row:59 Col:24 */
    {0x9143,0xDBB9}, /* 酃: GB2312 Code: 0xDBB9 ==> Row:59 Col:25 */
    {0x9146,0xDBBA}, /* 酆: GB2312 Code: 0xDBBA ==> Row:59 Col:26 */
    {0x9149,0xD3CF}, /* 酉: GB2312 Code: 0xD3CF ==> Row:51 Col:47 */
    {0x914A,0xF4FA}, /* 酊: GB2312 Code: 0xF4FA ==> Row:84 Col:90 */
    {0x914B,0xC7F5}, /* 酋: GB2312 Code: 0xC7F5 ==> Row:39 Col:85 */
    {0x914C,0xD7C3}, /* 酌: GB2312 Code: 0xD7C3 ==> Row:55 Col:35 */
    {0x914D,0xC5E4}, /* 配: GB2312 Code: 0xC5E4 ==> Row:37 Col:68 */
    {0x914E,0xF4FC}, /* 酎: GB2312 Code: 0xF4FC ==> Row:84 Col:92 */
    {0x914F,0xF4FD}, /* 酏: GB2312 Code: 0xF4FD ==> Row:84 Col:93 */
    {0x9150,0xF4FB}, /* 酐: GB2312 Code: 0xF4FB ==> Row:84 Col:91 */
    {0x9152,0xBEC6}, /* 酒: GB2312 Code: 0xBEC6 ==> Row:30 Col:38 */
    {0x9157,0xD0EF}, /* 酗: GB2312 Code: 0xD0EF ==> Row:48 Col:79 */
    {0x915A,0xB7D3}, /* 酚: GB2312 Code: 0xB7D3 ==> Row:23 Col:51 */
    {0x915D,0xD4CD}, /* 酝: GB2312 Code: 0xD4CD ==> Row:52 Col:45 */
    {0x915E,0xCCAA}, /* 酞: GB2312 Code: 0xCCAA ==> Row:44 Col:10 */
    {0x9161,0xF5A2}, /* 酡: GB2312 Code: 0xF5A2 ==> Row:85 Col:02 */
    {0x9162,0xF5A1}, /* 酢: GB2312 Code: 0xF5A1 ==> Row:85 Col:01 */
    {0x9163,0xBAA8}, /* 酣: GB2312 Code: 0xBAA8 ==> Row:26 Col:08 */
    {0x9164,0xF4FE}, /* 酤: GB2312 Code: 0xF4FE ==> Row:84 Col:94 */
    {0x9165,0xCBD6}, /* 酥: GB2312 Code: 0xCBD6 ==> Row:43 Col:54 */
    {0x9169,0xF5A4}, /* 酩: GB2312 Code: 0xF5A4 ==> Row:85 Col:04 */
    {0x916A,0xC0D2}, /* 酪: GB2312 Code: 0xC0D2 ==> Row:32 Col:50 */
    {0x916C,0xB3EA}, /* 酬: GB2312 Code: 0xB3EA ==> Row:19 Col:74 */
    {0x916E,0xCDAA}, /* 酮: GB2312 Code: 0xCDAA ==> Row:45 Col:10 */
    {0x916F,0xF5A5}, /* 酯: GB2312 Code: 0xF5A5 ==> Row:85 Col:05 */
    {0x9170,0xF5A3}, /* 酰: GB2312 Code: 0xF5A3 ==> Row:85 Col:03 */
    {0x9171,0xBDB4}, /* 酱: GB2312 Code: 0xBDB4 ==> Row:29 Col:20 */
    {0x9172,0xF5A8}, /* 酲: GB2312 Code: 0xF5A8 ==> Row:85 Col:08 */
    {0x9174,0xF5A9}, /* 酴: GB2312 Code: 0xF5A9 ==> Row:85 Col:09 */
    {0x9175,0xBDCD}, /* 酵: GB2312 Code: 0xBDCD ==> Row:29 Col:45 */
    {0x9176,0xC3B8}, /* 酶: GB2312 Code: 0xC3B8 ==> Row:35 Col:24 */
    {0x9177,0xBFE1}, /* 酷: GB2312 Code: 0xBFE1 ==> Row:31 Col:65 */
    {0x9178,0xCBE1}, /* 酸: GB2312 Code: 0xCBE1 ==> Row:43 Col:65 */
    {0x9179,0xF5AA}, /* 酹: GB2312 Code: 0xF5AA ==> Row:85 Col:10 */
    {0x917D,0xF5A6}, /* 酽: GB2312 Code: 0xF5A6 ==> Row:85 Col:06 */
    {0x917E,0xF5A7}, /* 酾: GB2312 Code: 0xF5A7 ==> Row:85 Col:07 */
    {0x917F,0xC4F0}, /* 酿: GB2312 Code: 0xC4F0 ==> Row:36 Col:80 */
    {0x9185,0xF5AC}, /* 醅: GB2312 Code: 0xF5AC ==> Row:85 Col:12 */
    {0x9187,0xB4BC}, /* 醇: GB2312 Code: 0xB4BC ==> Row:20 Col:28 */
    {0x9189,0xD7ED}, /* 醉: GB2312 Code: 0xD7ED ==> Row:55 Col:77 */
    {0x918B,0xB4D7}, /* 醋: GB2312 Code: 0xB4D7 ==> Row:20 Col:55 */
    {0x918C,0xF5AB}, /* 醌: GB2312 Code: 0xF5AB ==> Row:85 Col:11 */
    {0x918D,0xF5AE}, /* 醍: GB2312 Code: 0xF5AE ==> Row:85 Col:14 */
    {0x9190,0xF5AD}, /* 醐: GB2312 Code: 0xF5AD ==> Row:85 Col:13 */
    {0x9191,0xF5AF}, /* 醑: GB2312 Code: 0xF5AF ==> Row:85 Col:15 */
    {0x9192,0xD0D1}, /* 醒: GB2312 Code: 0xD0D1 ==> Row:48 Col:49 */
    {0x919A,0xC3D1}, /* 醚: GB2312 Code: 0xC3D1 ==> Row:35 Col:49 */
    {0x919B,0xC8A9}, /* 醛: GB2312 Code: 0xC8A9 ==> Row:40 Col:09 */
    {0x91A2,0xF5B0}, /* 醢: GB2312 Code: 0xF5B0 ==> Row:85 Col:16 */
    {0x91A3,0xF5B1}, /* 醣: GB2312 Code: 0xF5B1 ==> Row:85 Col:17 */
    {0x91AA,0xF5B2}, /* 醪: GB2312 Code: 0xF5B2 ==> Row:85 Col:18 */
    {0x91AD,0xF5B3}, /* 醭: GB2312 Code: 0xF5B3 ==> Row:85 Col:19 */
    {0x91AE,0xF5B4}, /* 醮: GB2312 Code: 0xF5B4 ==> Row:85 Col:20 */
    {0x91AF,0xF5B5}, /* 醯: GB2312 Code: 0xF5B5 ==> Row:85 Col:21 */
    {0x91B4,0xF5B7}, /* 醴: GB2312 Code: 0xF5B7 ==> Row:85 Col:23 */
    {0x91B5,0xF5B6}, /* 醵: GB2312 Code: 0xF5B6 ==> Row:85 Col:22 */
    {0x91BA,0xF5B8}, /* 醺: GB2312 Code: 0xF5B8 ==> Row:85 Col:24 */
    {0x91C7,0xB2C9}, /* 采: GB2312 Code: 0xB2C9 ==> Row:18 Col:41 */
    {0x91C9,0xD3D4}, /* 釉: GB2312 Code: 0xD3D4 ==> Row:51 Col:52 */
    {0x91CA,0xCACD}, /* 释: GB2312 Code: 0xCACD ==> Row:42 Col:45 */
    {0x91CC,0xC0EF}, /* 里: GB2312 Code: 0xC0EF ==> Row:32 Col:79 */
    {0x91CD,0xD6D8}, /* 重: GB2312 Code: 0xD6D8 ==> Row:54 Col:56 */
    {0x91CE,0xD2B0}, /* 野: GB2312 Code: 0xD2B0 ==> Row:50 Col:16 */
    {0x91CF,0xC1BF}, /* 量: GB2312 Code: 0xC1BF ==> Row:33 Col:31 */
    {0x91D1,0xBDF0}, /* 金: GB2312 Code: 0xBDF0 ==> Row:29 Col:80 */
    {0x91DC,0xB8AA}, /* 釜: GB2312 Code: 0xB8AA ==> Row:24 Col:10 */
    {0x9274,0xBCF8}, /* 鉴: GB2312 Code: 0xBCF8 ==> Row:28 Col:88 */
    {0x928E,0xF6C6}, /* 銎: GB2312 Code: 0xF6C6 ==> Row:86 Col:38 */
    {0x92AE,0xF6C7}, /* 銮: GB2312 Code: 0xF6C7 ==> Row:86 Col:39 */
    {0x92C8,0xF6C8}, /* 鋈: GB2312 Code: 0xF6C8 ==> Row:86 Col:40 */
    {0x933E,0xF6C9}, /* 錾: GB2312 Code: 0xF6C9 ==> Row:86 Col:41 */
    {0x936A,0xF6CA}, /* 鍪: GB2312 Code: 0xF6CA ==> Row:86 Col:42 */
    {0x938F,0xF6CC}, /* 鎏: GB2312 Code: 0xF6CC ==> Row:86 Col:44 */
    {0x93CA,0xF6CB}, /* 鏊: GB2312 Code: 0xF6CB ==> Row:86 Col:43 */
    {0x93D6,0xF7E9}, /* 鏖: GB2312 Code: 0xF7E9 ==> Row:87 Col:73 */
    {0x943E,0xF6CD}, /* 鐾: GB2312 Code: 0xF6CD ==> Row:86 Col:45 */
    {0x946B,0xF6CE}, /* 鑫: GB2312 Code: 0xF6CE ==> Row:86 Col:46 */
    {0x9485,0xEEC4}, /* 钅: GB2312 Code: 0xEEC4 ==> Row:78 Col:36 */
    {0x9486,0xEEC5}, /* 钆: GB2312 Code: 0xEEC5 ==> Row:78 Col:37 */
    {0x9487,0xEEC6}, /* 钇: GB2312 Code: 0xEEC6 ==> Row:78 Col:38 */
    {0x9488,0xD5EB}, /* 针: GB2312 Code: 0xD5EB ==> Row:53 Col:75 */
    {0x9489,0xB6A4}, /* 钉: GB2312 Code: 0xB6A4 ==> Row:22 Col:04 */
    {0x948A,0xEEC8}, /* 钊: GB2312 Code: 0xEEC8 ==> Row:78 Col:40 */
    {0x948B,0xEEC7}, /* 钋: GB2312 Code: 0xEEC7 ==> Row:78 Col:39 */
    {0x948C,0xEEC9}, /* 钌: GB2312 Code: 0xEEC9 ==> Row:78 Col:41 */
    {0x948D,0xEECA}, /* 钍: GB2312 Code: 0xEECA ==> Row:78 Col:42 */
    {0x948E,0xC7A5}, /* 钎: GB2312 Code: 0xC7A5 ==> Row:39 Col:05 */
    {0x948F,0xEECB}, /* 钏: GB2312 Code: 0xEECB ==> Row:78 Col:43 */
    {0x9490,0xEECC}, /* 钐: GB2312 Code: 0xEECC ==> Row:78 Col:44 */
    {0x9492,0xB7B0}, /* 钒: GB2312 Code: 0xB7B0 ==> Row:23 Col:16 */
    {0x9493,0xB5F6}, /* 钓: GB2312 Code: 0xB5F6 ==> Row:21 Col:86 */
    {0x9494,0xEECD}, /* 钔: GB2312 Code: 0xEECD ==> Row:78 Col:45 */
    {0x9495,0xEECF}, /* 钕: GB2312 Code: 0xEECF ==> Row:78 Col:47 */
    {0x9497,0xEECE}, /* 钗: GB2312 Code: 0xEECE ==> Row:78 Col:46 */
    {0x9499,0xB8C6}, /* 钙: GB2312 Code: 0xB8C6 ==> Row:24 Col:38 */
    {0x949A,0xEED0}, /* 钚: GB2312 Code: 0xEED0 ==> Row:78 Col:48 */
    {0x949B,0xEED1}, /* 钛: GB2312 Code: 0xEED1 ==> Row:78 Col:49 */
    {0x949C,0xEED2}, /* 钜: GB2312 Code: 0xEED2 ==> Row:78 Col:50 */
    {0x949D,0xB6DB}, /* 钝: GB2312 Code: 0xB6DB ==> Row:22 Col:59 */
    {0x949E,0xB3AE}, /* 钞: GB2312 Code: 0xB3AE ==> Row:19 Col:14 */
    {0x949F,0xD6D3}, /* 钟: GB2312 Code: 0xD6D3 ==> Row:54 Col:51 */
    {0x94A0,0xC4C6}, /* 钠: GB2312 Code: 0xC4C6 ==> Row:36 Col:38 */
    {0x94A1,0xB1B5}, /* 钡: GB2312 Code: 0xB1B5 ==> Row:17 Col:21 */
    {0x94A2,0xB8D6}, /* 钢: GB2312 Code: 0xB8D6 ==> Row:24 Col:54 */
    {0x94A3,0xEED3}, /* 钣: GB2312 Code: 0xEED3 ==> Row:78 Col:51 */
    {0x94A4,0xEED4}, /* 钤: GB2312 Code: 0xEED4 ==> Row:78 Col:52 */
    {0x94A5,0xD4BF}, /* 钥: GB2312 Code: 0xD4BF ==> Row:52 Col:31 */
    {0x94A6,0xC7D5}, /* 钦: GB2312 Code: 0xC7D5 ==> Row:39 Col:53 */
    {0x94A7,0xBEFB}, /* 钧: GB2312 Code: 0xBEFB ==> Row:30 Col:91 */
    {0x94A8,0xCED9}, /* 钨: GB2312 Code: 0xCED9 ==> Row:46 Col:57 */
    {0x94A9,0xB9B3}, /* 钩: GB2312 Code: 0xB9B3 ==> Row:25 Col:19 */
    {0x94AA,0xEED6}, /* 钪: GB2312 Code: 0xEED6 ==> Row:78 Col:54 */
    {0x94AB,0xEED5}, /* 钫: GB2312 Code: 0xEED5 ==> Row:78 Col:53 */
    {0x94AC,0xEED8}, /* 钬: GB2312 Code: 0xEED8 ==> Row:78 Col:56 */
    {0x94AD,0xEED7}, /* 钭: GB2312 Code: 0xEED7 ==> Row:78 Col:55 */
    {0x94AE,0xC5A5}, /* 钮: GB2312 Code: 0xC5A5 ==> Row:37 Col:05 */
    {0x94AF,0xEED9}, /* 钯: GB2312 Code: 0xEED9 ==> Row:78 Col:57 */
    {0x94B0,0xEEDA}, /* 钰: GB2312 Code: 0xEEDA ==> Row:78 Col:58 */
    {0x94B1,0xC7AE}, /* 钱: GB2312 Code: 0xC7AE ==> Row:39 Col:14 */
    {0x94B2,0xEEDB}, /* 钲: GB2312 Code: 0xEEDB ==> Row:78 Col:59 */
    {0x94B3,0xC7AF}, /* 钳: GB2312 Code: 0xC7AF ==> Row:39 Col:15 */
    {0x94B4,0xEEDC}, /* 钴: GB2312 Code: 0xEEDC ==> Row:78 Col:60 */
    {0x94B5,0xB2A7}, /* 钵: GB2312 Code: 0xB2A7 ==> Row:18 Col:07 */
    {0x94B6,0xEEDD}, /* 钶: GB2312 Code: 0xEEDD ==> Row:78 Col:61 */
    {0x94B7,0xEEDE}, /* 钷: GB2312 Code: 0xEEDE ==> Row:78 Col:62 */
    {0x94B8,0xEEDF}, /* 钸: GB2312 Code: 0xEEDF ==> Row:78 Col:63 */
    {0x94B9,0xEEE0}, /* 钹: GB2312 Code: 0xEEE0 ==> Row:78 Col:64 */
    {0x94BA,0xEEE1}, /* 钺: GB2312 Code: 0xEEE1 ==> Row:78 Col:65 */
    {0x94BB,0xD7EA}, /* 钻: GB2312 Code: 0xD7EA ==> Row:55 Col:74 */
    {0x94BC,0xEEE2}, /* 钼: GB2312 Code: 0xEEE2 ==> Row:78 Col:66 */
    {0x94BD,0xEEE3}, /* 钽: GB2312 Code: 0xEEE3 ==> Row:78 Col:67 */
    {0x94BE,0xBCD8}, /* 钾: GB2312 Code: 0xBCD8 ==> Row:28 Col:56 */
    {0x94BF,0xEEE4}, /* 钿: GB2312 Code: 0xEEE4 ==> Row:78 Col:68 */
    {0x94C0,0xD3CB}, /* 铀: GB2312 Code: 0xD3CB ==> Row:51 Col:43 */
    {0x94C1,0xCCFA}, /* 铁: GB2312 Code: 0xCCFA ==> Row:44 Col:90 */
    {0x94C2,0xB2AC}, /* 铂: GB2312 Code: 0xB2AC ==> Row:18 Col:12 */
    {0x94C3,0xC1E5}, /* 铃: GB2312 Code: 0xC1E5 ==> Row:33 Col:69 */
    {0x94C4,0xEEE5}, /* 铄: GB2312 Code: 0xEEE5 ==> Row:78 Col:69 */
    {0x94C5,0xC7A6}, /* 铅: GB2312 Code: 0xC7A6 ==> Row:39 Col:06 */
    {0x94C6,0xC3AD}, /* 铆: GB2312 Code: 0xC3AD ==> Row:35 Col:13 */
    {0x94C8,0xEEE6}, /* 铈: GB2312 Code: 0xEEE6 ==> Row:78 Col:70 */
    {0x94C9,0xEEE7}, /* 铉: GB2312 Code: 0xEEE7 ==> Row:78 Col:71 */
    {0x94CA,0xEEE8}, /* 铊: GB2312 Code: 0xEEE8 ==> Row:78 Col:72 */
    {0x94CB,0xEEE9}, /* 铋: GB2312 Code: 0xEEE9 ==> Row:78 Col:73 */
    {0x94CC,0xEEEA}, /* 铌: GB2312 Code: 0xEEEA ==> Row:78 Col:74 */
    {0x94CD,0xEEEB}, /* 铍: GB2312 Code: 0xEEEB ==> Row:78 Col:75 */
    {0x94CE,0xEEEC}, /* 铎: GB2312 Code: 0xEEEC ==> Row:78 Col:76 */
    {0x94D0,0xEEED}, /* 铐: GB2312 Code: 0xEEED ==> Row:78 Col:77 */
    {0x94D1,0xEEEE}, /* 铑: GB2312 Code: 0xEEEE ==> Row:78 Col:78 */
    {0x94D2,0xEEEF}, /* 铒: GB2312 Code: 0xEEEF ==> Row:78 Col:79 */
    {0x94D5,0xEEF0}, /* 铕: GB2312 Code: 0xEEF0 ==> Row:78 Col:80 */
    {0x94D6,0xEEF1}, /* 铖: GB2312 Code: 0xEEF1 ==> Row:78 Col:81 */
    {0x94D7,0xEEF2}, /* 铗: GB2312 Code: 0xEEF2 ==> Row:78 Col:82 */
    {0x94D8,0xEEF4}, /* 铘: GB2312 Code: 0xEEF4 ==> Row:78 Col:84 */
    {0x94D9,0xEEF3}, /* 铙: GB2312 Code: 0xEEF3 ==> Row:78 Col:83 */
    {0x94DB,0xEEF5}, /* 铛: GB2312 Code: 0xEEF5 ==> Row:78 Col:85 */
    {0x94DC,0xCDAD}, /* 铜: GB2312 Code: 0xCDAD ==> Row:45 Col:13 */
    {0x94DD,0xC2C1}, /* 铝: GB2312 Code: 0xC2C1 ==> Row:34 Col:33 */
    {0x94DE,0xEEF6}, /* 铞: GB2312 Code: 0xEEF6 ==> Row:78 Col:86 */
    {0x94DF,0xEEF7}, /* 铟: GB2312 Code: 0xEEF7 ==> Row:78 Col:87 */
    {0x94E0,0xEEF8}, /* 铠: GB2312 Code: 0xEEF8 ==> Row:78 Col:88 */
    {0x94E1,0xD5A1}, /* 铡: GB2312 Code: 0xD5A1 ==> Row:53 Col:01 */
    {0x94E2,0xEEF9}, /* 铢: GB2312 Code: 0xEEF9 ==> Row:78 Col:89 */
    {0x94E3,0xCFB3}, /* 铣: GB2312 Code: 0xCFB3 ==> Row:47 Col:19 */
    {0x94E4,0xEEFA}, /* 铤: GB2312 Code: 0xEEFA ==> Row:78 Col:90 */
    {0x94E5,0xEEFB}, /* 铥: GB2312 Code: 0xEEFB ==> Row:78 Col:91 */
    {0x94E7,0xEEFC}, /* 铧: GB2312 Code: 0xEEFC ==> Row:78 Col:92 */
    {0x94E8,0xEEFD}, /* 铨: GB2312 Code: 0xEEFD ==> Row:78 Col:93 */
    {0x94E9,0xEFA1}, /* 铩: GB2312 Code: 0xEFA1 ==> Row:79 Col:01 */
    {0x94EA,0xEEFE}, /* 铪: GB2312 Code: 0xEEFE ==> Row:78 Col:94 */
    {0x94EB,0xEFA2}, /* 铫: GB2312 Code: 0xEFA2 ==> Row:79 Col:02 */
    {0x94EC,0xB8F5}, /* 铬: GB2312 Code: 0xB8F5 ==> Row:24 Col:85 */
    {0x94ED,0xC3FA}, /* 铭: GB2312 Code: 0xC3FA ==> Row:35 Col:90 */
    {0x94EE,0xEFA3}, /* 铮: GB2312 Code: 0xEFA3 ==> Row:79 Col:03 */
    {0x94EF,0xEFA4}, /* 铯: GB2312 Code: 0xEFA4 ==> Row:79 Col:04 */
    {0x94F0,0xBDC2}, /* 铰: GB2312 Code: 0xBDC2 ==> Row:29 Col:34 */
    {0x94F1,0xD2BF}, /* 铱: GB2312 Code: 0xD2BF ==> Row:50 Col:31 */
    {0x94F2,0xB2F9}, /* 铲: GB2312 Code: 0xB2F9 ==> Row:18 Col:89 */
    {0x94F3,0xEFA5}, /* 铳: GB2312 Code: 0xEFA5 ==> Row:79 Col:05 */
    {0x94F4,0xEFA6}, /* 铴: GB2312 Code: 0xEFA6 ==> Row:79 Col:06 */
    {0x94F5,0xEFA7}, /* 铵: GB2312 Code: 0xEFA7 ==> Row:79 Col:07 */
    {0x94F6,0xD2F8}, /* 银: GB2312 Code: 0xD2F8 ==> Row:50 Col:88 */
    {0x94F7,0xEFA8}, /* 铷: GB2312 Code: 0xEFA8 ==> Row:79 Col:08 */
    {0x94F8,0xD6FD}, /* 铸: GB2312 Code: 0xD6FD ==> Row:54 Col:93 */
    {0x94F9,0xEFA9}, /* 铹: GB2312 Code: 0xEFA9 ==> Row:79 Col:09 */
    {0x94FA,0xC6CC}, /* 铺: GB2312 Code: 0xC6CC ==> Row:38 Col:44 */
    {0x94FC,0xEFAA}, /* 铼: GB2312 Code: 0xEFAA ==> Row:79 Col:10 */
    {0x94FD,0xEFAB}, /* 铽: GB2312 Code: 0xEFAB ==> Row:79 Col:11 */
    {0x94FE,0xC1B4}, /* 链: GB2312 Code: 0xC1B4 ==> Row:33 Col:20 */
    {0x94FF,0xEFAC}, /* 铿: GB2312 Code: 0xEFAC ==> Row:79 Col:12 */
    {0x9500,0xCFFA}, /* 销: GB2312 Code: 0xCFFA ==> Row:47 Col:90 */
    {0x9501,0xCBF8}, /* 锁: GB2312 Code: 0xCBF8 ==> Row:43 Col:88 */
    {0x9502,0xEFAE}, /* 锂: GB2312 Code: 0xEFAE ==> Row:79 Col:14 */
    {0x9503,0xEFAD}, /* 锃: GB2312 Code: 0xEFAD ==> Row:79 Col:13 */
    {0x9504,0xB3FA}, /* 锄: GB2312 Code: 0xB3FA ==> Row:19 Col:90 */
    {0x9505,0xB9F8}, /* 锅: GB2312 Code: 0xB9F8 ==> Row:25 Col:88 */
    {0x9506,0xEFAF}, /* 锆: GB2312 Code: 0xEFAF ==> Row:79 Col:15 */
    {0x9507,0xEFB0}, /* 锇: GB2312 Code: 0xEFB0 ==> Row:79 Col:16 */
    {0x9508,0xD0E2}, /* 锈: GB2312 Code: 0xD0E2 ==> Row:48 Col:66 */
    {0x9509,0xEFB1}, /* 锉: GB2312 Code: 0xEFB1 ==> Row:79 Col:17 */
    {0x950A,0xEFB2}, /* 锊: GB2312 Code: 0xEFB2 ==> Row:79 Col:18 */
    {0x950B,0xB7E6}, /* 锋: GB2312 Code: 0xB7E6 ==> Row:23 Col:70 */
    {0x950C,0xD0BF}, /* 锌: GB2312 Code: 0xD0BF ==> Row:48 Col:31 */
    {0x950D,0xEFB3}, /* 锍: GB2312 Code: 0xEFB3 ==> Row:79 Col:19 */
    {0x950E,0xEFB4}, /* 锎: GB2312 Code: 0xEFB4 ==> Row:79 Col:20 */
    {0x950F,0xEFB5}, /* 锏: GB2312 Code: 0xEFB5 ==> Row:79 Col:21 */
    {0x9510,0xC8F1}, /* 锐: GB2312 Code: 0xC8F1 ==> Row:40 Col:81 */
    {0x9511,0xCCE0}, /* 锑: GB2312 Code: 0xCCE0 ==> Row:44 Col:64 */
    {0x9512,0xEFB6}, /* 锒: GB2312 Code: 0xEFB6 ==> Row:79 Col:22 */
    {0x9513,0xEFB7}, /* 锓: GB2312 Code: 0xEFB7 ==> Row:79 Col:23 */
    {0x9514,0xEFB8}, /* 锔: GB2312 Code: 0xEFB8 ==> Row:79 Col:24 */
    {0x9515,0xEFB9}, /* 锕: GB2312 Code: 0xEFB9 ==> Row:79 Col:25 */
    {0x9516,0xEFBA}, /* 锖: GB2312 Code: 0xEFBA ==> Row:79 Col:26 */
    {0x9517,0xD5E0}, /* 锗: GB2312 Code: 0xD5E0 ==> Row:53 Col:64 */
    {0x9518,0xEFBB}, /* 锘: GB2312 Code: 0xEFBB ==> Row:79 Col:27 */
    {0x9519,0xB4ED}, /* 错: GB2312 Code: 0xB4ED ==> Row:20 Col:77 */
    {0x951A,0xC3AA}, /* 锚: GB2312 Code: 0xC3AA ==> Row:35 Col:10 */
    {0x951B,0xEFBC}, /* 锛: GB2312 Code: 0xEFBC ==> Row:79 Col:28 */
    {0x951D,0xEFBD}, /* 锝: GB2312 Code: 0xEFBD ==> Row:79 Col:29 */
    {0x951E,0xEFBE}, /* 锞: GB2312 Code: 0xEFBE ==> Row:79 Col:30 */
    {0x951F,0xEFBF}, /* 锟: GB2312 Code: 0xEFBF ==> Row:79 Col:31 */
    {0x9521,0xCEFD}, /* 锡: GB2312 Code: 0xCEFD ==> Row:46 Col:93 */
    {0x9522,0xEFC0}, /* 锢: GB2312 Code: 0xEFC0 ==> Row:79 Col:32 */
    {0x9523,0xC2E0}, /* 锣: GB2312 Code: 0xC2E0 ==> Row:34 Col:64 */
    {0x9524,0xB4B8}, /* 锤: GB2312 Code: 0xB4B8 ==> Row:20 Col:24 */
    {0x9525,0xD7B6}, /* 锥: GB2312 Code: 0xD7B6 ==> Row:55 Col:22 */
    {0x9526,0xBDF5}, /* 锦: GB2312 Code: 0xBDF5 ==> Row:29 Col:85 */
    {0x9528,0xCFC7}, /* 锨: GB2312 Code: 0xCFC7 ==> Row:47 Col:39 */
    {0x9529,0xEFC3}, /* 锩: GB2312 Code: 0xEFC3 ==> Row:79 Col:35 */
    {0x952A,0xEFC1}, /* 锪: GB2312 Code: 0xEFC1 ==> Row:79 Col:33 */
    {0x952B,0xEFC2}, /* 锫: GB2312 Code: 0xEFC2 ==> Row:79 Col:34 */
    {0x952C,0xEFC4}, /* 锬: GB2312 Code: 0xEFC4 ==> Row:79 Col:36 */
    {0x952D,0xB6A7}, /* 锭: GB2312 Code: 0xB6A7 ==> Row:22 Col:07 */
    {0x952E,0xBCFC}, /* 键: GB2312 Code: 0xBCFC ==> Row:28 Col:92 */
    {0x952F,0xBEE2}, /* 锯: GB2312 Code: 0xBEE2 ==> Row:30 Col:66 */
    {0x9530,0xC3CC}, /* 锰: GB2312 Code: 0xC3CC ==> Row:35 Col:44 */
    {0x9531,0xEFC5}, /* 锱: GB2312 Code: 0xEFC5 ==> Row:79 Col:37 */
    {0x9532,0xEFC6}, /* 锲: GB2312 Code: 0xEFC6 ==> Row:79 Col:38 */
    {0x9534,0xEFC7}, /* 锴: GB2312 Code: 0xEFC7 ==> Row:79 Col:39 */
    {0x9535,0xEFCF}, /* 锵: GB2312 Code: 0xEFCF ==> Row:79 Col:47 */
    {0x9536,0xEFC8}, /* 锶: GB2312 Code: 0xEFC8 ==> Row:79 Col:40 */
    {0x9537,0xEFC9}, /* 锷: GB2312 Code: 0xEFC9 ==> Row:79 Col:41 */
    {0x9538,0xEFCA}, /* 锸: GB2312 Code: 0xEFCA ==> Row:79 Col:42 */
    {0x9539,0xC7C2}, /* 锹: GB2312 Code: 0xC7C2 ==> Row:39 Col:34 */
    {0x953A,0xEFF1}, /* 锺: GB2312 Code: 0xEFF1 ==> Row:79 Col:81 */
    {0x953B,0xB6CD}, /* 锻: GB2312 Code: 0xB6CD ==> Row:22 Col:45 */
    {0x953C,0xEFCB}, /* 锼: GB2312 Code: 0xEFCB ==> Row:79 Col:43 */
    {0x953E,0xEFCC}, /* 锾: GB2312 Code: 0xEFCC ==> Row:79 Col:44 */
    {0x953F,0xEFCD}, /* 锿: GB2312 Code: 0xEFCD ==> Row:79 Col:45 */
    {0x9540,0xB6C6}, /* 镀: GB2312 Code: 0xB6C6 ==> Row:22 Col:38 */
    {0x9541,0xC3BE}, /* 镁: GB2312 Code: 0xC3BE ==> Row:35 Col:30 */
    {0x9542,0xEFCE}, /* 镂: GB2312 Code: 0xEFCE ==> Row:79 Col:46 */
    {0x9544,0xEFD0}, /* 镄: GB2312 Code: 0xEFD0 ==> Row:79 Col:48 */
    {0x9545,0xEFD1}, /* 镅: GB2312 Code: 0xEFD1 ==> Row:79 Col:49 */
    {0x9546,0xEFD2}, /* 镆: GB2312 Code: 0xEFD2 ==> Row:79 Col:50 */
    {0x9547,0xD5F2}, /* 镇: GB2312 Code: 0xD5F2 ==> Row:53 Col:82 */
    {0x9549,0xEFD3}, /* 镉: GB2312 Code: 0xEFD3 ==> Row:79 Col:51 */
    {0x954A,0xC4F7}, /* 镊: GB2312 Code: 0xC4F7 ==> Row:36 Col:87 */
    {0x954C,0xEFD4}, /* 镌: GB2312 Code: 0xEFD4 ==> Row:79 Col:52 */
    {0x954D,0xC4F8}, /* 镍: GB2312 Code: 0xC4F8 ==> Row:36 Col:88 */
    {0x954E,0xEFD5}, /* 镎: GB2312 Code: 0xEFD5 ==> Row:79 Col:53 */
    {0x954F,0xEFD6}, /* 镏: GB2312 Code: 0xEFD6 ==> Row:79 Col:54 */
    {0x9550,0xB8E4}, /* 镐: GB2312 Code: 0xB8E4 ==> Row:24 Col:68 */
    {0x9551,0xB0F7}, /* 镑: GB2312 Code: 0xB0F7 ==> Row:16 Col:87 */
    {0x9552,0xEFD7}, /* 镒: GB2312 Code: 0xEFD7 ==> Row:79 Col:55 */
    {0x9553,0xEFD8}, /* 镓: GB2312 Code: 0xEFD8 ==> Row:79 Col:56 */
    {0x9554,0xEFD9}, /* 镔: GB2312 Code: 0xEFD9 ==> Row:79 Col:57 */
    {0x9556,0xEFDA}, /* 镖: GB2312 Code: 0xEFDA ==> Row:79 Col:58 */
    {0x9557,0xEFDB}, /* 镗: GB2312 Code: 0xEFDB ==> Row:79 Col:59 */
    {0x9558,0xEFDC}, /* 镘: GB2312 Code: 0xEFDC ==> Row:79 Col:60 */
    {0x9559,0xEFDD}, /* 镙: GB2312 Code: 0xEFDD ==> Row:79 Col:61 */
    {0x955B,0xEFDE}, /* 镛: GB2312 Code: 0xEFDE ==> Row:79 Col:62 */
    {0x955C,0xBEB5}, /* ??: GB2312 Code: 0xBEB5 ==> Row:30 Col:21 */
    {0x955D,0xEFE1}, /* 镝: GB2312 Code: 0xEFE1 ==> Row:79 Col:65 */
    {0x955E,0xEFDF}, /* 镞: GB2312 Code: 0xEFDF ==> Row:79 Col:63 */
    {0x955F,0xEFE0}, /* 镟: GB2312 Code: 0xEFE0 ==> Row:79 Col:64 */
    {0x9561,0xEFE2}, /* 镡: GB2312 Code: 0xEFE2 ==> Row:79 Col:66 */
    {0x9562,0xEFE3}, /* 镢: GB2312 Code: 0xEFE3 ==> Row:79 Col:67 */
    {0x9563,0xC1CD}, /* 镣: GB2312 Code: 0xC1CD ==> Row:33 Col:45 */
    {0x9564,0xEFE4}, /* 镤: GB2312 Code: 0xEFE4 ==> Row:79 Col:68 */
    {0x9565,0xEFE5}, /* 镥: GB2312 Code: 0xEFE5 ==> Row:79 Col:69 */
    {0x9566,0xEFE6}, /* 镦: GB2312 Code: 0xEFE6 ==> Row:79 Col:70 */
    {0x9567,0xEFE7}, /* 镧: GB2312 Code: 0xEFE7 ==> Row:79 Col:71 */
    {0x9568,0xEFE8}, /* 镨: GB2312 Code: 0xEFE8 ==> Row:79 Col:72 */
    {0x9569,0xEFE9}, /* 镩: GB2312 Code: 0xEFE9 ==> Row:79 Col:73 */
    {0x956A,0xEFEA}, /* 镪: GB2312 Code: 0xEFEA ==> Row:79 Col:74 */
    {0x956B,0xEFEB}, /* 镫: GB2312 Code: 0xEFEB ==> Row:79 Col:75 */
    {0x956C,0xEFEC}, /* 镬: GB2312 Code: 0xEFEC ==> Row:79 Col:76 */
    {0x956D,0xC0D8}, /* 镭: GB2312 Code: 0xC0D8 ==> Row:32 Col:56 */
    {0x956F,0xEFED}, /* 镯: GB2312 Code: 0xEFED ==> Row:79 Col:77 */
    {0x9570,0xC1AD}, /* 镰: GB2312 Code: 0xC1AD ==> Row:33 Col:13 */
    {0x9571,0xEFEE}, /* 镱: GB2312 Code: 0xEFEE ==> Row:79 Col:78 */
    {0x9572,0xEFEF}, /* 镲: GB2312 Code: 0xEFEF ==> Row:79 Col:79 */
    {0x9573,0xEFF0}, /* 镳: GB2312 Code: 0xEFF0 ==> Row:79 Col:80 */
    {0x9576,0xCFE2}, /* 镶: GB2312 Code: 0xCFE2 ==> Row:47 Col:66 */
    {0x957F,0xB3A4}, /* 长: GB2312 Code: 0xB3A4 ==> Row:19 Col:04 */
    {0x95E8,0xC3C5}, /* 门: GB2312 Code: 0xC3C5 ==> Row:35 Col:37 */
    {0x95E9,0xE3C5}, /* 闩: GB2312 Code: 0xE3C5 ==> Row:67 Col:37 */
    {0x95EA,0xC9C1}, /* 闪: GB2312 Code: 0xC9C1 ==> Row:41 Col:33 */
    {0x95EB,0xE3C6}, /* 闫: GB2312 Code: 0xE3C6 ==> Row:67 Col:38 */
    {0x95ED,0xB1D5}, /* 闭: GB2312 Code: 0xB1D5 ==> Row:17 Col:53 */
    {0x95EE,0xCECA}, /* 问: GB2312 Code: 0xCECA ==> Row:46 Col:42 */
    {0x95EF,0xB4B3}, /* 闯: GB2312 Code: 0xB4B3 ==> Row:20 Col:19 */
    {0x95F0,0xC8F2}, /* 闰: GB2312 Code: 0xC8F2 ==> Row:40 Col:82 */
    {0x95F1,0xE3C7}, /* 闱: GB2312 Code: 0xE3C7 ==> Row:67 Col:39 */
    {0x95F2,0xCFD0}, /* 闲: GB2312 Code: 0xCFD0 ==> Row:47 Col:48 */
    {0x95F3,0xE3C8}, /* 闳: GB2312 Code: 0xE3C8 ==> Row:67 Col:40 */
    {0x95F4,0xBCE4}, /* 间: GB2312 Code: 0xBCE4 ==> Row:28 Col:68 */
    {0x95F5,0xE3C9}, /* 闵: GB2312 Code: 0xE3C9 ==> Row:67 Col:41 */
    {0x95F6,0xE3CA}, /* 闶: GB2312 Code: 0xE3CA ==> Row:67 Col:42 */
    {0x95F7,0xC3C6}, /* 闷: GB2312 Code: 0xC3C6 ==> Row:35 Col:38 */
    {0x95F8,0xD5A2}, /* 闸: GB2312 Code: 0xD5A2 ==> Row:53 Col:02 */
    {0x95F9,0xC4D6}, /* 闹: GB2312 Code: 0xC4D6 ==> Row:36 Col:54 */
    {0x95FA,0xB9EB}, /* 闺: GB2312 Code: 0xB9EB ==> Row:25 Col:75 */
    {0x95FB,0xCEC5}, /* 闻: GB2312 Code: 0xCEC5 ==> Row:46 Col:37 */
    {0x95FC,0xE3CB}, /* 闼: GB2312 Code: 0xE3CB ==> Row:67 Col:43 */
    {0x95FD,0xC3F6}, /* 闽: GB2312 Code: 0xC3F6 ==> Row:35 Col:86 */
    {0x95FE,0xE3CC}, /* 闾: GB2312 Code: 0xE3CC ==> Row:67 Col:44 */
    {0x9600,0xB7A7}, /* 阀: GB2312 Code: 0xB7A7 ==> Row:23 Col:07 */
    {0x9601,0xB8F3}, /* 阁: GB2312 Code: 0xB8F3 ==> Row:24 Col:83 */
    {0x9602,0xBAD2}, /* 阂: GB2312 Code: 0xBAD2 ==> Row:26 Col:50 */
    {0x9603,0xE3CD}, /* 阃: GB2312 Code: 0xE3CD ==> Row:67 Col:45 */
    {0x9604,0xE3CE}, /* 阄: GB2312 Code: 0xE3CE ==> Row:67 Col:46 */
    {0x9605,0xD4C4}, /* 阅: GB2312 Code: 0xD4C4 ==> Row:52 Col:36 */
    {0x9606,0xE3CF}, /* 阆: GB2312 Code: 0xE3CF ==> Row:67 Col:47 */
    {0x9608,0xE3D0}, /* 阈: GB2312 Code: 0xE3D0 ==> Row:67 Col:48 */
    {0x9609,0xD1CB}, /* 阉: GB2312 Code: 0xD1CB ==> Row:49 Col:43 */
    {0x960A,0xE3D1}, /* 阊: GB2312 Code: 0xE3D1 ==> Row:67 Col:49 */
    {0x960B,0xE3D2}, /* 阋: GB2312 Code: 0xE3D2 ==> Row:67 Col:50 */
    {0x960C,0xE3D3}, /* 阌: GB2312 Code: 0xE3D3 ==> Row:67 Col:51 */
    {0x960D,0xE3D4}, /* 阍: GB2312 Code: 0xE3D4 ==> Row:67 Col:52 */
    {0x960E,0xD1D6}, /* 阎: GB2312 Code: 0xD1D6 ==> Row:49 Col:54 */
    {0x960F,0xE3D5}, /* 阏: GB2312 Code: 0xE3D5 ==> Row:67 Col:53 */
    {0x9610,0xB2FB}, /* 阐: GB2312 Code: 0xB2FB ==> Row:18 Col:91 */
    {0x9611,0xC0BB}, /* 阑: GB2312 Code: 0xC0BB ==> Row:32 Col:27 */
    {0x9612,0xE3D6}, /* 阒: GB2312 Code: 0xE3D6 ==> Row:67 Col:54 */
    {0x9614,0xC0AB}, /* 阔: GB2312 Code: 0xC0AB ==> Row:32 Col:11 */
    {0x9615,0xE3D7}, /* 阕: GB2312 Code: 0xE3D7 ==> Row:67 Col:55 */
    {0x9616,0xE3D8}, /* 阖: GB2312 Code: 0xE3D8 ==> Row:67 Col:56 */
    {0x9617,0xE3D9}, /* 阗: GB2312 Code: 0xE3D9 ==> Row:67 Col:57 */
    {0x9619,0xE3DA}, /* 阙: GB2312 Code: 0xE3DA ==> Row:67 Col:58 */
    {0x961A,0xE3DB}, /* 阚: GB2312 Code: 0xE3DB ==> Row:67 Col:59 */
    {0x961C,0xB8B7}, /* 阜: GB2312 Code: 0xB8B7 ==> Row:24 Col:23 */
    {0x961D,0xDAE2}, /* 阝: GB2312 Code: 0xDAE2 ==> Row:58 Col:66 */
    {0x961F,0xB6D3}, /* 队: GB2312 Code: 0xB6D3 ==> Row:22 Col:51 */
    {0x9621,0xDAE4}, /* 阡: GB2312 Code: 0xDAE4 ==> Row:58 Col:68 */
    {0x9622,0xDAE3}, /* 阢: GB2312 Code: 0xDAE3 ==> Row:58 Col:67 */
    {0x962A,0xDAE6}, /* 阪: GB2312 Code: 0xDAE6 ==> Row:58 Col:70 */
    {0x962E,0xC8EE}, /* 阮: GB2312 Code: 0xC8EE ==> Row:40 Col:78 */
    {0x9631,0xDAE5}, /* 阱: GB2312 Code: 0xDAE5 ==> Row:58 Col:69 */
    {0x9632,0xB7C0}, /* 防: GB2312 Code: 0xB7C0 ==> Row:23 Col:32 */
    {0x9633,0xD1F4}, /* 阳: GB2312 Code: 0xD1F4 ==> Row:49 Col:84 */
    {0x9634,0xD2F5}, /* 阴: GB2312 Code: 0xD2F5 ==> Row:50 Col:85 */
    {0x9635,0xD5F3}, /* 阵: GB2312 Code: 0xD5F3 ==> Row:53 Col:83 */
    {0x9636,0xBDD7}, /* 阶: GB2312 Code: 0xBDD7 ==> Row:29 Col:55 */
    {0x963B,0xD7E8}, /* 阻: GB2312 Code: 0xD7E8 ==> Row:55 Col:72 */
    {0x963C,0xDAE8}, /* 阼: GB2312 Code: 0xDAE8 ==> Row:58 Col:72 */
    {0x963D,0xDAE7}, /* 阽: GB2312 Code: 0xDAE7 ==> Row:58 Col:71 */
    {0x963F,0xB0A2}, /* 阿: GB2312 Code: 0xB0A2 ==> Row:16 Col:02 */
    {0x9640,0xCDD3}, /* 陀: GB2312 Code: 0xCDD3 ==> Row:45 Col:51 */
    {0x9642,0xDAE9}, /* 陂: GB2312 Code: 0xDAE9 ==> Row:58 Col:73 */
    {0x9644,0xB8BD}, /* 附: GB2312 Code: 0xB8BD ==> Row:24 Col:29 */
    {0x9645,0xBCCA}, /* 际: GB2312 Code: 0xBCCA ==> Row:28 Col:42 */
    {0x9646,0xC2BD}, /* 陆: GB2312 Code: 0xC2BD ==> Row:34 Col:29 */
    {0x9647,0xC2A4}, /* 陇: GB2312 Code: 0xC2A4 ==> Row:34 Col:04 */
    {0x9648,0xB3C2}, /* 陈: GB2312 Code: 0xB3C2 ==> Row:19 Col:34 */
    {0x9649,0xDAEA}, /* 陉: GB2312 Code: 0xDAEA ==> Row:58 Col:74 */
    {0x964B,0xC2AA}, /* 陋: GB2312 Code: 0xC2AA ==> Row:34 Col:10 */
    {0x964C,0xC4B0}, /* 陌: GB2312 Code: 0xC4B0 ==> Row:36 Col:16 */
    {0x964D,0xBDB5}, /* 降: GB2312 Code: 0xBDB5 ==> Row:29 Col:21 */
    {0x9650,0xCFDE}, /* 限: GB2312 Code: 0xCFDE ==> Row:47 Col:62 */
    {0x9654,0xDAEB}, /* 陔: GB2312 Code: 0xDAEB ==> Row:58 Col:75 */
    {0x9655,0xC9C2}, /* 陕: GB2312 Code: 0xC9C2 ==> Row:41 Col:34 */
    {0x965B,0xB1DD}, /* 陛: GB2312 Code: 0xB1DD ==> Row:17 Col:61 */
    {0x965F,0xDAEC}, /* 陟: GB2312 Code: 0xDAEC ==> Row:58 Col:76 */
    {0x9661,0xB6B8}, /* 陡: GB2312 Code: 0xB6B8 ==> Row:22 Col:24 */
    {0x9662,0xD4BA}, /* 院: GB2312 Code: 0xD4BA ==> Row:52 Col:26 */
    {0x9664,0xB3FD}, /* 除: GB2312 Code: 0xB3FD ==> Row:19 Col:93 */
    {0x9667,0xDAED}, /* 陧: GB2312 Code: 0xDAED ==> Row:58 Col:77 */
    {0x9668,0xD4C9}, /* 陨: GB2312 Code: 0xD4C9 ==> Row:52 Col:41 */
    {0x9669,0xCFD5}, /* 险: GB2312 Code: 0xCFD5 ==> Row:47 Col:53 */
    {0x966A,0xC5E3}, /* 陪: GB2312 Code: 0xC5E3 ==> Row:37 Col:67 */
    {0x966C,0xDAEE}, /* 陬: GB2312 Code: 0xDAEE ==> Row:58 Col:78 */
    {0x9672,0xDAEF}, /* 陲: GB2312 Code: 0xDAEF ==> Row:58 Col:79 */
    {0x9674,0xDAF0}, /* 陴: GB2312 Code: 0xDAF0 ==> Row:58 Col:80 */
    {0x9675,0xC1EA}, /* 陵: GB2312 Code: 0xC1EA ==> Row:33 Col:74 */
    {0x9676,0xCCD5}, /* 陶: GB2312 Code: 0xCCD5 ==> Row:44 Col:53 */
    {0x9677,0xCFDD}, /* 陷: GB2312 Code: 0xCFDD ==> Row:47 Col:61 */
    {0x9685,0xD3E7}, /* 隅: GB2312 Code: 0xD3E7 ==> Row:51 Col:71 */
    {0x9686,0xC2A1}, /* 隆: GB2312 Code: 0xC2A1 ==> Row:34 Col:01 */
    {0x9688,0xDAF1}, /* 隈: GB2312 Code: 0xDAF1 ==> Row:58 Col:81 */
    {0x968B,0xCBE5}, /* 隋: GB2312 Code: 0xCBE5 ==> Row:43 Col:69 */
    {0x968D,0xDAF2}, /* 隍: GB2312 Code: 0xDAF2 ==> Row:58 Col:82 */
    {0x968F,0xCBE6}, /* 随: GB2312 Code: 0xCBE6 ==> Row:43 Col:70 */
    {0x9690,0xD2FE}, /* 隐: GB2312 Code: 0xD2FE ==> Row:50 Col:94 */
    {0x9694,0xB8F4}, /* 隔: GB2312 Code: 0xB8F4 ==> Row:24 Col:84 */
    {0x9697,0xDAF3}, /* 隗: GB2312 Code: 0xDAF3 ==> Row:58 Col:83 */
    {0x9698,0xB0AF}, /* 隘: GB2312 Code: 0xB0AF ==> Row:16 Col:15 */
    {0x9699,0xCFB6}, /* 隙: GB2312 Code: 0xCFB6 ==> Row:47 Col:22 */
    {0x969C,0xD5CF}, /* 障: GB2312 Code: 0xD5CF ==> Row:53 Col:47 */
    {0x96A7,0xCBED}, /* 隧: GB2312 Code: 0xCBED ==> Row:43 Col:77 */
    {0x96B0,0xDAF4}, /* 隰: GB2312 Code: 0xDAF4 ==> Row:58 Col:84 */
    {0x96B3,0xE3C4}, /* 隳: GB2312 Code: 0xE3C4 ==> Row:67 Col:36 */
    {0x96B6,0xC1A5}, /* 隶: GB2312 Code: 0xC1A5 ==> Row:33 Col:05 */
    {0x96B9,0xF6BF}, /* 隹: GB2312 Code: 0xF6BF ==> Row:86 Col:31 */
    {0x96BC,0xF6C0}, /* 隼: GB2312 Code: 0xF6C0 ==> Row:86 Col:32 */
    {0x96BD,0xF6C1}, /* 隽: GB2312 Code: 0xF6C1 ==> Row:86 Col:33 */
    {0x96BE,0xC4D1}, /* 难: GB2312 Code: 0xC4D1 ==> Row:36 Col:49 */
    {0x96C0,0xC8B8}, /* 雀: GB2312 Code: 0xC8B8 ==> Row:40 Col:24 */
    {0x96C1,0xD1E3}, /* 雁: GB2312 Code: 0xD1E3 ==> Row:49 Col:67 */
    {0x96C4,0xD0DB}, /* 雄: GB2312 Code: 0xD0DB ==> Row:48 Col:59 */
    {0x96C5,0xD1C5}, /* 雅: GB2312 Code: 0xD1C5 ==> Row:49 Col:37 */
    {0x96C6,0xBCAF}, /* 集: GB2312 Code: 0xBCAF ==> Row:28 Col:15 */
    {0x96C7,0xB9CD}, /* 雇: GB2312 Code: 0xB9CD ==> Row:25 Col:45 */
    {0x96C9,0xEFF4}, /* 雉: GB2312 Code: 0xEFF4 ==> Row:79 Col:84 */
    {0x96CC,0xB4C6}, /* 雌: GB2312 Code: 0xB4C6 ==> Row:20 Col:38 */
    {0x96CD,0xD3BA}, /* 雍: GB2312 Code: 0xD3BA ==> Row:51 Col:26 */
    {0x96CE,0xF6C2}, /* 雎: GB2312 Code: 0xF6C2 ==> Row:86 Col:34 */
    {0x96CF,0xB3FB}, /* 雏: GB2312 Code: 0xB3FB ==> Row:19 Col:91 */
    {0x96D2,0xF6C3}, /* 雒: GB2312 Code: 0xF6C3 ==> Row:86 Col:35 */
    {0x96D5,0xB5F1}, /* 雕: GB2312 Code: 0xB5F1 ==> Row:21 Col:81 */
    {0x96E0,0xF6C5}, /* 雠: GB2312 Code: 0xF6C5 ==> Row:86 Col:37 */
    {0x96E8,0xD3EA}, /* 雨: GB2312 Code: 0xD3EA ==> Row:51 Col:74 */
    {0x96E9,0xF6A7}, /* 雩: GB2312 Code: 0xF6A7 ==> Row:86 Col:07 */
    {0x96EA,0xD1A9}, /* 雪: GB2312 Code: 0xD1A9 ==> Row:49 Col:09 */
    {0x96EF,0xF6A9}, /* 雯: GB2312 Code: 0xF6A9 ==> Row:86 Col:09 */
    {0x96F3,0xF6A8}, /* 雳: GB2312 Code: 0xF6A8 ==> Row:86 Col:08 */
    {0x96F6,0xC1E3}, /* 零: GB2312 Code: 0xC1E3 ==> Row:33 Col:67 */
    {0x96F7,0xC0D7}, /* 雷: GB2312 Code: 0xC0D7 ==> Row:32 Col:55 */
    {0x96F9,0xB1A2}, /* 雹: GB2312 Code: 0xB1A2 ==> Row:17 Col:02 */
    {0x96FE,0xCEED}, /* 雾: GB2312 Code: 0xCEED ==> Row:46 Col:77 */
    {0x9700,0xD0E8}, /* 需: GB2312 Code: 0xD0E8 ==> Row:48 Col:72 */
    {0x9701,0xF6AB}, /* 霁: GB2312 Code: 0xF6AB ==> Row:86 Col:11 */
    {0x9704,0xCFF6}, /* 霄: GB2312 Code: 0xCFF6 ==> Row:47 Col:86 */
    {0x9706,0xF6AA}, /* 霆: GB2312 Code: 0xF6AA ==> Row:86 Col:10 */
    {0x9707,0xD5F0}, /* 震: GB2312 Code: 0xD5F0 ==> Row:53 Col:80 */
    {0x9708,0xF6AC}, /* 霈: GB2312 Code: 0xF6AC ==> Row:86 Col:12 */
    {0x9709,0xC3B9}, /* 霉: GB2312 Code: 0xC3B9 ==> Row:35 Col:25 */
    {0x970D,0xBBF4}, /* 霍: GB2312 Code: 0xBBF4 ==> Row:27 Col:84 */
    {0x970E,0xF6AE}, /* 霎: GB2312 Code: 0xF6AE ==> Row:86 Col:14 */
    {0x970F,0xF6AD}, /* 霏: GB2312 Code: 0xF6AD ==> Row:86 Col:13 */
    {0x9713,0xC4DE}, /* 霓: GB2312 Code: 0xC4DE ==> Row:36 Col:62 */
    {0x9716,0xC1D8}, /* 霖: GB2312 Code: 0xC1D8 ==> Row:33 Col:56 */
    {0x971C,0xCBAA}, /* 霜: GB2312 Code: 0xCBAA ==> Row:43 Col:10 */
    {0x971E,0xCFBC}, /* 霞: GB2312 Code: 0xCFBC ==> Row:47 Col:28 */
    {0x972A,0xF6AF}, /* 霪: GB2312 Code: 0xF6AF ==> Row:86 Col:15 */
    {0x972D,0xF6B0}, /* 霭: GB2312 Code: 0xF6B0 ==> Row:86 Col:16 */
    {0x9730,0xF6B1}, /* 霰: GB2312 Code: 0xF6B1 ==> Row:86 Col:17 */
    {0x9732,0xC2B6}, /* 露: GB2312 Code: 0xC2B6 ==> Row:34 Col:22 */
    {0x9738,0xB0D4}, /* 霸: GB2312 Code: 0xB0D4 ==> Row:16 Col:52 */
    {0x9739,0xC5F9}, /* 霹: GB2312 Code: 0xC5F9 ==> Row:37 Col:89 */
    {0x973E,0xF6B2}, /* 霾: GB2312 Code: 0xF6B2 ==> Row:86 Col:18 */
    {0x9752,0xC7E0}, /* 青: GB2312 Code: 0xC7E0 ==> Row:39 Col:64 */
    {0x9753,0xF6A6}, /* 靓: GB2312 Code: 0xF6A6 ==> Row:86 Col:06 */
    {0x9756,0xBEB8}, /* 靖: GB2312 Code: 0xBEB8 ==> Row:30 Col:24 */
    {0x9759,0xBEB2}, /* 静: GB2312 Code: 0xBEB2 ==> Row:30 Col:18 */
    {0x975B,0xB5E5}, /* 靛: GB2312 Code: 0xB5E5 ==> Row:21 Col:69 */
    {0x975E,0xB7C7}, /* 非: GB2312 Code: 0xB7C7 ==> Row:23 Col:39 */
    {0x9760,0xBFBF}, /* 靠: GB2312 Code: 0xBFBF ==> Row:31 Col:31 */
    {0x9761,0xC3D2}, /* 靡: GB2312 Code: 0xC3D2 ==> Row:35 Col:50 */
    {0x9762,0xC3E6}, /* 面: GB2312 Code: 0xC3E6 ==> Row:35 Col:70 */
    {0x9765,0xD8CC}, /* 靥: GB2312 Code: 0xD8CC ==> Row:56 Col:44 */
    {0x9769,0xB8EF}, /* 革: GB2312 Code: 0xB8EF ==> Row:24 Col:79 */
    {0x9773,0xBDF9}, /* 靳: GB2312 Code: 0xBDF9 ==> Row:29 Col:89 */
    {0x9774,0xD1A5}, /* 靴: GB2312 Code: 0xD1A5 ==> Row:49 Col:05 */
    {0x9776,0xB0D0}, /* 靶: GB2312 Code: 0xB0D0 ==> Row:16 Col:48 */
    {0x977C,0xF7B0}, /* 靼: GB2312 Code: 0xF7B0 ==> Row:87 Col:16 */
    {0x9785,0xF7B1}, /* 鞅: GB2312 Code: 0xF7B1 ==> Row:87 Col:17 */
    {0x978B,0xD0AC}, /* 鞋: GB2312 Code: 0xD0AC ==> Row:48 Col:12 */
    {0x978D,0xB0B0}, /* 鞍: GB2312 Code: 0xB0B0 ==> Row:16 Col:16 */
    {0x9791,0xF7B2}, /* 鞑: GB2312 Code: 0xF7B2 ==> Row:87 Col:18 */
    {0x9792,0xF7B3}, /* 鞒: GB2312 Code: 0xF7B3 ==> Row:87 Col:19 */
    {0x9794,0xF7B4}, /* 鞔: GB2312 Code: 0xF7B4 ==> Row:87 Col:20 */
    {0x9798,0xC7CA}, /* 鞘: GB2312 Code: 0xC7CA ==> Row:39 Col:42 */
    {0x97A0,0xBECF}, /* 鞠: GB2312 Code: 0xBECF ==> Row:30 Col:47 */
    {0x97A3,0xF7B7}, /* 鞣: GB2312 Code: 0xF7B7 ==> Row:87 Col:23 */
    {0x97AB,0xF7B6}, /* 鞫: GB2312 Code: 0xF7B6 ==> Row:87 Col:22 */
    {0x97AD,0xB1DE}, /* 鞭: GB2312 Code: 0xB1DE ==> Row:17 Col:62 */
    {0x97AF,0xF7B5}, /* 鞯: GB2312 Code: 0xF7B5 ==> Row:87 Col:21 */
    {0x97B2,0xF7B8}, /* 鞲: GB2312 Code: 0xF7B8 ==> Row:87 Col:24 */
    {0x97B4,0xF7B9}, /* 鞴: GB2312 Code: 0xF7B9 ==> Row:87 Col:25 */
    {0x97E6,0xCEA4}, /* 韦: GB2312 Code: 0xCEA4 ==> Row:46 Col:04 */
    {0x97E7,0xC8CD}, /* 韧: GB2312 Code: 0xC8CD ==> Row:40 Col:45 */
    {0x97E9,0xBAAB}, /* 韩: GB2312 Code: 0xBAAB ==> Row:26 Col:11 */
    {0x97EA,0xE8B8}, /* 韪: GB2312 Code: 0xE8B8 ==> Row:72 Col:24 */
    {0x97EB,0xE8B9}, /* 韫: GB2312 Code: 0xE8B9 ==> Row:72 Col:25 */
    {0x97EC,0xE8BA}, /* 韬: GB2312 Code: 0xE8BA ==> Row:72 Col:26 */
    {0x97ED,0xBEC2}, /* 韭: GB2312 Code: 0xBEC2 ==> Row:30 Col:34 */
    {0x97F3,0xD2F4}, /* 音: GB2312 Code: 0xD2F4 ==> Row:50 Col:84 */
    {0x97F5,0xD4CF}, /* 韵: GB2312 Code: 0xD4CF ==> Row:52 Col:47 */
    {0x97F6,0xC9D8}, /* 韶: GB2312 Code: 0xC9D8 ==> Row:41 Col:56 */
    {0x9875,0xD2B3}, /* 页: GB2312 Code: 0xD2B3 ==> Row:50 Col:19 */
    {0x9876,0xB6A5}, /* 顶: GB2312 Code: 0xB6A5 ==> Row:22 Col:05 */
    {0x9877,0xC7EA}, /* 顷: GB2312 Code: 0xC7EA ==> Row:39 Col:74 */
    {0x9878,0xF1FC}, /* 顸: GB2312 Code: 0xF1FC ==> Row:81 Col:92 */
    {0x9879,0xCFEE}, /* 项: GB2312 Code: 0xCFEE ==> Row:47 Col:78 */
    {0x987A,0xCBB3}, /* 顺: GB2312 Code: 0xCBB3 ==> Row:43 Col:19 */
    {0x987B,0xD0EB}, /* 须: GB2312 Code: 0xD0EB ==> Row:48 Col:75 */
    {0x987C,0xE7EF}, /* 顼: GB2312 Code: 0xE7EF ==> Row:71 Col:79 */
    {0x987D,0xCDE7}, /* 顽: GB2312 Code: 0xCDE7 ==> Row:45 Col:71 */
    {0x987E,0xB9CB}, /* 顾: GB2312 Code: 0xB9CB ==> Row:25 Col:43 */
    {0x987F,0xB6D9}, /* 顿: GB2312 Code: 0xB6D9 ==> Row:22 Col:57 */
    {0x9880,0xF1FD}, /* 颀: GB2312 Code: 0xF1FD ==> Row:81 Col:93 */
    {0x9881,0xB0E4}, /* 颁: GB2312 Code: 0xB0E4 ==> Row:16 Col:68 */
    {0x9882,0xCBCC}, /* 颂: GB2312 Code: 0xCBCC ==> Row:43 Col:44 */
    {0x9883,0xF1FE}, /* 颃: GB2312 Code: 0xF1FE ==> Row:81 Col:94 */
    {0x9884,0xD4A4}, /* 预: GB2312 Code: 0xD4A4 ==> Row:52 Col:04 */
    {0x9885,0xC2AD}, /* 颅: GB2312 Code: 0xC2AD ==> Row:34 Col:13 */
    {0x9886,0xC1EC}, /* 领: GB2312 Code: 0xC1EC ==> Row:33 Col:76 */
    {0x9887,0xC6C4}, /* 颇: GB2312 Code: 0xC6C4 ==> Row:38 Col:36 */
    {0x9888,0xBEB1}, /* 颈: GB2312 Code: 0xBEB1 ==> Row:30 Col:17 */
    {0x9889,0xF2A1}, /* 颉: GB2312 Code: 0xF2A1 ==> Row:82 Col:01 */
    {0x988A,0xBCD5}, /* 颊: GB2312 Code: 0xBCD5 ==> Row:28 Col:53 */
    {0x988C,0xF2A2}, /* 颌: GB2312 Code: 0xF2A2 ==> Row:82 Col:02 */
    {0x988D,0xF2A3}, /* 颍: GB2312 Code: 0xF2A3 ==> Row:82 Col:03 */
    {0x988F,0xF2A4}, /* 颏: GB2312 Code: 0xF2A4 ==> Row:82 Col:04 */
    {0x9890,0xD2C3}, /* 颐: GB2312 Code: 0xD2C3 ==> Row:50 Col:35 */
    {0x9891,0xC6B5}, /* 频: GB2312 Code: 0xC6B5 ==> Row:38 Col:21 */
    {0x9893,0xCDC7}, /* 颓: GB2312 Code: 0xCDC7 ==> Row:45 Col:39 */
    {0x9894,0xF2A5}, /* 颔: GB2312 Code: 0xF2A5 ==> Row:82 Col:05 */
    {0x9896,0xD3B1}, /* 颖: GB2312 Code: 0xD3B1 ==> Row:51 Col:17 */
    {0x9897,0xBFC5}, /* 颗: GB2312 Code: 0xBFC5 ==> Row:31 Col:37 */
    {0x9898,0xCCE2}, /* 题: GB2312 Code: 0xCCE2 ==> Row:44 Col:66 */
    {0x989A,0xF2A6}, /* 颚: GB2312 Code: 0xF2A6 ==> Row:82 Col:06 */
    {0x989B,0xF2A7}, /* 颛: GB2312 Code: 0xF2A7 ==> Row:82 Col:07 */
    {0x989C,0xD1D5}, /* 颜: GB2312 Code: 0xD1D5 ==> Row:49 Col:53 */
    {0x989D,0xB6EE}, /* 额: GB2312 Code: 0xB6EE ==> Row:22 Col:78 */
    {0x989E,0xF2A8}, /* 颞: GB2312 Code: 0xF2A8 ==> Row:82 Col:08 */
    {0x989F,0xF2A9}, /* 颟: GB2312 Code: 0xF2A9 ==> Row:82 Col:09 */
    {0x98A0,0xB5DF}, /* 颠: GB2312 Code: 0xB5DF ==> Row:21 Col:63 */
    {0x98A1,0xF2AA}, /* 颡: GB2312 Code: 0xF2AA ==> Row:82 Col:10 */
    {0x98A2,0xF2AB}, /* 颢: GB2312 Code: 0xF2AB ==> Row:82 Col:11 */
    {0x98A4,0xB2FC}, /* 颤: GB2312 Code: 0xB2FC ==> Row:18 Col:92 */
    {0x98A5,0xF2AC}, /* 颥: GB2312 Code: 0xF2AC ==> Row:82 Col:12 */
    {0x98A6,0xF2AD}, /* 颦: GB2312 Code: 0xF2AD ==> Row:82 Col:13 */
    {0x98A7,0xC8A7}, /* 颧: GB2312 Code: 0xC8A7 ==> Row:40 Col:07 */
    {0x98CE,0xB7E7}, /* 风: GB2312 Code: 0xB7E7 ==> Row:23 Col:71 */
    {0x98D1,0xECA9}, /* 飑: GB2312 Code: 0xECA9 ==> Row:76 Col:09 */
    {0x98D2,0xECAA}, /* 飒: GB2312 Code: 0xECAA ==> Row:76 Col:10 */
    {0x98D3,0xECAB}, /* 飓: GB2312 Code: 0xECAB ==> Row:76 Col:11 */
    {0x98D5,0xECAC}, /* 飕: GB2312 Code: 0xECAC ==> Row:76 Col:12 */
    {0x98D8,0xC6AE}, /* 飘: GB2312 Code: 0xC6AE ==> Row:38 Col:14 */
    {0x98D9,0xECAD}, /* 飙: GB2312 Code: 0xECAD ==> Row:76 Col:13 */
    {0x98DA,0xECAE}, /* 飚: GB2312 Code: 0xECAE ==> Row:76 Col:14 */
    {0x98DE,0xB7C9}, /* 飞: GB2312 Code: 0xB7C9 ==> Row:23 Col:41 */
    {0x98DF,0xCAB3}, /* 食: GB2312 Code: 0xCAB3 ==> Row:42 Col:19 */
    {0x98E7,0xE2B8}, /* 飧: GB2312 Code: 0xE2B8 ==> Row:66 Col:24 */
    {0x98E8,0xF7CF}, /* 飨: GB2312 Code: 0xF7CF ==> Row:87 Col:47 */
    {0x990D,0xF7D0}, /* 餍: GB2312 Code: 0xF7D0 ==> Row:87 Col:48 */
    {0x9910,0xB2CD}, /* 餐: GB2312 Code: 0xB2CD ==> Row:18 Col:45 */
    {0x992E,0xF7D1}, /* 餮: GB2312 Code: 0xF7D1 ==> Row:87 Col:49 */
    {0x9954,0xF7D3}, /* 饔: GB2312 Code: 0xF7D3 ==> Row:87 Col:51 */
    {0x9955,0xF7D2}, /* 饕: GB2312 Code: 0xF7D2 ==> Row:87 Col:50 */
    {0x9963,0xE2BB}, /* 饣: GB2312 Code: 0xE2BB ==> Row:66 Col:27 */
    {0x9965,0xBCA2}, /* 饥: GB2312 Code: 0xBCA2 ==> Row:28 Col:02 */
    {0x9967,0xE2BC}, /* 饧: GB2312 Code: 0xE2BC ==> Row:66 Col:28 */
    {0x9968,0xE2BD}, /* 饨: GB2312 Code: 0xE2BD ==> Row:66 Col:29 */
    {0x9969,0xE2BE}, /* 饩: GB2312 Code: 0xE2BE ==> Row:66 Col:30 */
    {0x996A,0xE2BF}, /* 饪: GB2312 Code: 0xE2BF ==> Row:66 Col:31 */
    {0x996B,0xE2C0}, /* 饫: GB2312 Code: 0xE2C0 ==> Row:66 Col:32 */
    {0x996C,0xE2C1}, /* 饬: GB2312 Code: 0xE2C1 ==> Row:66 Col:33 */
    {0x996D,0xB7B9}, /* 饭: GB2312 Code: 0xB7B9 ==> Row:23 Col:25 */
    {0x996E,0xD2FB}, /* 饮: GB2312 Code: 0xD2FB ==> Row:50 Col:91 */
    {0x996F,0xBDA4}, /* 饯: GB2312 Code: 0xBDA4 ==> Row:29 Col:04 */
    {0x9970,0xCACE}, /* 饰: GB2312 Code: 0xCACE ==> Row:42 Col:46 */
    {0x9971,0xB1A5}, /* 饱: GB2312 Code: 0xB1A5 ==> Row:17 Col:05 */
    {0x9972,0xCBC7}, /* 饲: GB2312 Code: 0xCBC7 ==> Row:43 Col:39 */
    {0x9974,0xE2C2}, /* 饴: GB2312 Code: 0xE2C2 ==> Row:66 Col:34 */
    {0x9975,0xB6FC}, /* 饵: GB2312 Code: 0xB6FC ==> Row:22 Col:92 */
    {0x9976,0xC8C4}, /* 饶: GB2312 Code: 0xC8C4 ==> Row:40 Col:36 */
    {0x9977,0xE2C3}, /* 饷: GB2312 Code: 0xE2C3 ==> Row:66 Col:35 */
    {0x997A,0xBDC8}, /* 饺: GB2312 Code: 0xBDC8 ==> Row:29 Col:40 */
    {0x997C,0xB1FD}, /* 饼: GB2312 Code: 0xB1FD ==> Row:17 Col:93 */
    {0x997D,0xE2C4}, /* 饽: GB2312 Code: 0xE2C4 ==> Row:66 Col:36 */
    {0x997F,0xB6F6}, /* 饿: GB2312 Code: 0xB6F6 ==> Row:22 Col:86 */
    {0x9980,0xE2C5}, /* 馀: GB2312 Code: 0xE2C5 ==> Row:66 Col:37 */
    {0x9981,0xC4D9}, /* 馁: GB2312 Code: 0xC4D9 ==> Row:36 Col:57 */
    {0x9984,0xE2C6}, /* 馄: GB2312 Code: 0xE2C6 ==> Row:66 Col:38 */
    {0x9985,0xCFDA}, /* 馅: GB2312 Code: 0xCFDA ==> Row:47 Col:58 */
    {0x9986,0xB9DD}, /* 馆: GB2312 Code: 0xB9DD ==> Row:25 Col:61 */
    {0x9987,0xE2C7}, /* 馇: GB2312 Code: 0xE2C7 ==> Row:66 Col:39 */
    {0x9988,0xC0A1}, /* 馈: GB2312 Code: 0xC0A1 ==> Row:32 Col:01 */
    {0x998A,0xE2C8}, /* 馊: GB2312 Code: 0xE2C8 ==> Row:66 Col:40 */
    {0x998B,0xB2F6}, /* 馋: GB2312 Code: 0xB2F6 ==> Row:18 Col:86 */
    {0x998D,0xE2C9}, /* 馍: GB2312 Code: 0xE2C9 ==> Row:66 Col:41 */
    {0x998F,0xC1F3}, /* 馏: GB2312 Code: 0xC1F3 ==> Row:33 Col:83 */
    {0x9990,0xE2CA}, /* 馐: GB2312 Code: 0xE2CA ==> Row:66 Col:42 */
    {0x9991,0xE2CB}, /* 馑: GB2312 Code: 0xE2CB ==> Row:66 Col:43 */
    {0x9992,0xC2F8}, /* 馒: GB2312 Code: 0xC2F8 ==> Row:34 Col:88 */
    {0x9993,0xE2CC}, /* 馓: GB2312 Code: 0xE2CC ==> Row:66 Col:44 */
    {0x9994,0xE2CD}, /* 馔: GB2312 Code: 0xE2CD ==> Row:66 Col:45 */
    {0x9995,0xE2CE}, /* 馕: GB2312 Code: 0xE2CE ==> Row:66 Col:46 */
    {0x9996,0xCAD7}, /* 首: GB2312 Code: 0xCAD7 ==> Row:42 Col:55 */
    {0x9997,0xD8B8}, /* 馗: GB2312 Code: 0xD8B8 ==> Row:56 Col:24 */
    {0x9998,0xD9E5}, /* 馘: GB2312 Code: 0xD9E5 ==> Row:57 Col:69 */
    {0x9999,0xCFE3}, /* 香: GB2312 Code: 0xCFE3 ==> Row:47 Col:67 */
    {0x99A5,0xF0A5}, /* 馥: GB2312 Code: 0xF0A5 ==> Row:80 Col:05 */
    {0x99A8,0xDCB0}, /* 馨: GB2312 Code: 0xDCB0 ==> Row:60 Col:16 */
    {0x9A6C,0xC2ED}, /* 马: GB2312 Code: 0xC2ED ==> Row:34 Col:77 */
    {0x9A6D,0xD4A6}, /* 驭: GB2312 Code: 0xD4A6 ==> Row:52 Col:06 */
    {0x9A6E,0xCDD4}, /* 驮: GB2312 Code: 0xCDD4 ==> Row:45 Col:52 */
    {0x9A6F,0xD1B1}, /* 驯: GB2312 Code: 0xD1B1 ==> Row:49 Col:17 */
    {0x9A70,0xB3DB}, /* 驰: GB2312 Code: 0xB3DB ==> Row:19 Col:59 */
    {0x9A71,0xC7FD}, /* 驱: GB2312 Code: 0xC7FD ==> Row:39 Col:93 */
    {0x9A73,0xB2B5}, /* 驳: GB2312 Code: 0xB2B5 ==> Row:18 Col:21 */
    {0x9A74,0xC2BF}, /* 驴: GB2312 Code: 0xC2BF ==> Row:34 Col:31 */
    {0x9A75,0xE6E0}, /* 驵: GB2312 Code: 0xE6E0 ==> Row:70 Col:64 */
    {0x9A76,0xCABB}, /* 驶: GB2312 Code: 0xCABB ==> Row:42 Col:27 */
    {0x9A77,0xE6E1}, /* 驷: GB2312 Code: 0xE6E1 ==> Row:70 Col:65 */
    {0x9A78,0xE6E2}, /* 驸: GB2312 Code: 0xE6E2 ==> Row:70 Col:66 */
    {0x9A79,0xBED4}, /* 驹: GB2312 Code: 0xBED4 ==> Row:30 Col:52 */
    {0x9A7A,0xE6E3}, /* 驺: GB2312 Code: 0xE6E3 ==> Row:70 Col:67 */
    {0x9A7B,0xD7A4}, /* 驻: GB2312 Code: 0xD7A4 ==> Row:55 Col:04 */
    {0x9A7C,0xCDD5}, /* 驼: GB2312 Code: 0xCDD5 ==> Row:45 Col:53 */
    {0x9A7D,0xE6E5}, /* 驽: GB2312 Code: 0xE6E5 ==> Row:70 Col:69 */
    {0x9A7E,0xBCDD}, /* 驾: GB2312 Code: 0xBCDD ==> Row:28 Col:61 */
    {0x9A7F,0xE6E4}, /* 驿: GB2312 Code: 0xE6E4 ==> Row:70 Col:68 */
    {0x9A80,0xE6E6}, /* 骀: GB2312 Code: 0xE6E6 ==> Row:70 Col:70 */
    {0x9A81,0xE6E7}, /* 骁: GB2312 Code: 0xE6E7 ==> Row:70 Col:71 */
    {0x9A82,0xC2EE}, /* 骂: GB2312 Code: 0xC2EE ==> Row:34 Col:78 */
    {0x9A84,0xBDBE}, /* 骄: GB2312 Code: 0xBDBE ==> Row:29 Col:30 */
    {0x9A85,0xE6E8}, /* 骅: GB2312 Code: 0xE6E8 ==> Row:70 Col:72 */
    {0x9A86,0xC2E6}, /* 骆: GB2312 Code: 0xC2E6 ==> Row:34 Col:70 */
    {0x9A87,0xBAA7}, /* 骇: GB2312 Code: 0xBAA7 ==> Row:26 Col:07 */
    {0x9A88,0xE6E9}, /* 骈: GB2312 Code: 0xE6E9 ==> Row:70 Col:73 */
    {0x9A8A,0xE6EA}, /* 骊: GB2312 Code: 0xE6EA ==> Row:70 Col:74 */
    {0x9A8B,0xB3D2}, /* 骋: GB2312 Code: 0xB3D2 ==> Row:19 Col:50 */
    {0x9A8C,0xD1E9}, /* 验: GB2312 Code: 0xD1E9 ==> Row:49 Col:73 */
    {0x9A8F,0xBFA5}, /* 骏: GB2312 Code: 0xBFA5 ==> Row:31 Col:05 */
    {0x9A90,0xE6EB}, /* 骐: GB2312 Code: 0xE6EB ==> Row:70 Col:75 */
    {0x9A91,0xC6EF}, /* 骑: GB2312 Code: 0xC6EF ==> Row:38 Col:79 */
    {0x9A92,0xE6EC}, /* 骒: GB2312 Code: 0xE6EC ==> Row:70 Col:76 */
    {0x9A93,0xE6ED}, /* 骓: GB2312 Code: 0xE6ED ==> Row:70 Col:77 */
    {0x9A96,0xE6EE}, /* 骖: GB2312 Code: 0xE6EE ==> Row:70 Col:78 */
    {0x9A97,0xC6AD}, /* 骗: GB2312 Code: 0xC6AD ==> Row:38 Col:13 */
    {0x9A98,0xE6EF}, /* 骘: GB2312 Code: 0xE6EF ==> Row:70 Col:79 */
    {0x9A9A,0xC9A7}, /* 骚: GB2312 Code: 0xC9A7 ==> Row:41 Col:07 */
    {0x9A9B,0xE6F0}, /* 骛: GB2312 Code: 0xE6F0 ==> Row:70 Col:80 */
    {0x9A9C,0xE6F1}, /* 骜: GB2312 Code: 0xE6F1 ==> Row:70 Col:81 */
    {0x9A9D,0xE6F2}, /* 骝: GB2312 Code: 0xE6F2 ==> Row:70 Col:82 */
    {0x9A9E,0xE5B9}, /* 骞: GB2312 Code: 0xE5B9 ==> Row:69 Col:25 */
    {0x9A9F,0xE6F3}, /* 骟: GB2312 Code: 0xE6F3 ==> Row:70 Col:83 */
    {0x9AA0,0xE6F4}, /* 骠: GB2312 Code: 0xE6F4 ==> Row:70 Col:84 */
    {0x9AA1,0xC2E2}, /* 骡: GB2312 Code: 0xC2E2 ==> Row:34 Col:66 */
    {0x9AA2,0xE6F5}, /* 骢: GB2312 Code: 0xE6F5 ==> Row:70 Col:85 */
    {0x9AA3,0xE6F6}, /* 骣: GB2312 Code: 0xE6F6 ==> Row:70 Col:86 */
    {0x9AA4,0xD6E8}, /* 骤: GB2312 Code: 0xD6E8 ==> Row:54 Col:72 */
    {0x9AA5,0xE6F7}, /* 骥: GB2312 Code: 0xE6F7 ==> Row:70 Col:87 */
    {0x9AA7,0xE6F8}, /* 骧: GB2312 Code: 0xE6F8 ==> Row:70 Col:88 */
    {0x9AA8,0xB9C7}, /* 骨: GB2312 Code: 0xB9C7 ==> Row:25 Col:39 */
    {0x9AB0,0xF7BB}, /* 骰: GB2312 Code: 0xF7BB ==> Row:87 Col:27 */
    {0x9AB1,0xF7BA}, /* 骱: GB2312 Code: 0xF7BA ==> Row:87 Col:26 */
    {0x9AB6,0xF7BE}, /* 骶: GB2312 Code: 0xF7BE ==> Row:87 Col:30 */
    {0x9AB7,0xF7BC}, /* 骷: GB2312 Code: 0xF7BC ==> Row:87 Col:28 */
    {0x9AB8,0xBAA1}, /* 骸: GB2312 Code: 0xBAA1 ==> Row:26 Col:01 */
    {0x9ABA,0xF7BF}, /* 骺: GB2312 Code: 0xF7BF ==> Row:87 Col:31 */
    {0x9ABC,0xF7C0}, /* 骼: GB2312 Code: 0xF7C0 ==> Row:87 Col:32 */
    {0x9AC0,0xF7C2}, /* 髀: GB2312 Code: 0xF7C2 ==> Row:87 Col:34 */
    {0x9AC1,0xF7C1}, /* 髁: GB2312 Code: 0xF7C1 ==> Row:87 Col:33 */
    {0x9AC2,0xF7C4}, /* 髂: GB2312 Code: 0xF7C4 ==> Row:87 Col:36 */
    {0x9AC5,0xF7C3}, /* 髅: GB2312 Code: 0xF7C3 ==> Row:87 Col:35 */
    {0x9ACB,0xF7C5}, /* 髋: GB2312 Code: 0xF7C5 ==> Row:87 Col:37 */
    {0x9ACC,0xF7C6}, /* 髌: GB2312 Code: 0xF7C6 ==> Row:87 Col:38 */
    {0x9AD1,0xF7C7}, /* 髑: GB2312 Code: 0xF7C7 ==> Row:87 Col:39 */
    {0x9AD3,0xCBE8}, /* 髓: GB2312 Code: 0xCBE8 ==> Row:43 Col:72 */
    {0x9AD8,0xB8DF}, /* 高: GB2312 Code: 0xB8DF ==> Row:24 Col:63 */
    {0x9ADF,0xF7D4}, /* 髟: GB2312 Code: 0xF7D4 ==> Row:87 Col:52 */
    {0x9AE1,0xF7D5}, /* 髡: GB2312 Code: 0xF7D5 ==> Row:87 Col:53 */
    {0x9AE6,0xF7D6}, /* 髦: GB2312 Code: 0xF7D6 ==> Row:87 Col:54 */
    {0x9AEB,0xF7D8}, /* 髫: GB2312 Code: 0xF7D8 ==> Row:87 Col:56 */
    {0x9AED,0xF7DA}, /* 髭: GB2312 Code: 0xF7DA ==> Row:87 Col:58 */
    {0x9AEF,0xF7D7}, /* 髯: GB2312 Code: 0xF7D7 ==> Row:87 Col:55 */
    {0x9AF9,0xF7DB}, /* 髹: GB2312 Code: 0xF7DB ==> Row:87 Col:59 */
    {0x9AFB,0xF7D9}, /* 髻: GB2312 Code: 0xF7D9 ==> Row:87 Col:57 */
    {0x9B03,0xD7D7}, /* 鬃: GB2312 Code: 0xD7D7 ==> Row:55 Col:55 */
    {0x9B08,0xF7DC}, /* 鬈: GB2312 Code: 0xF7DC ==> Row:87 Col:60 */
    {0x9B0F,0xF7DD}, /* 鬏: GB2312 Code: 0xF7DD ==> Row:87 Col:61 */
    {0x9B13,0xF7DE}, /* 鬓: GB2312 Code: 0xF7DE ==> Row:87 Col:62 */
    {0x9B1F,0xF7DF}, /* 鬟: GB2312 Code: 0xF7DF ==> Row:87 Col:63 */
    {0x9B23,0xF7E0}, /* 鬣: GB2312 Code: 0xF7E0 ==> Row:87 Col:64 */
    {0x9B2F,0xDBCB}, /* 鬯: GB2312 Code: 0xDBCB ==> Row:59 Col:43 */
    {0x9B32,0xD8AA}, /* 鬲: GB2312 Code: 0xD8AA ==> Row:56 Col:10 */
    {0x9B3B,0xE5F7}, /* 鬻: GB2312 Code: 0xE5F7 ==> Row:69 Col:87 */
    {0x9B3C,0xB9ED}, /* 鬼: GB2312 Code: 0xB9ED ==> Row:25 Col:77 */
    {0x9B41,0xBFFD}, /* 魁: GB2312 Code: 0xBFFD ==> Row:31 Col:93 */
    {0x9B42,0xBBEA}, /* 魂: GB2312 Code: 0xBBEA ==> Row:27 Col:74 */
    {0x9B43,0xF7C9}, /* 魃: GB2312 Code: 0xF7C9 ==> Row:87 Col:41 */
    {0x9B44,0xC6C7}, /* 魄: GB2312 Code: 0xC6C7 ==> Row:38 Col:39 */
    {0x9B45,0xF7C8}, /* 魅: GB2312 Code: 0xF7C8 ==> Row:87 Col:40 */
    {0x9B47,0xF7CA}, /* 魇: GB2312 Code: 0xF7CA ==> Row:87 Col:42 */
    {0x9B48,0xF7CC}, /* 魈: GB2312 Code: 0xF7CC ==> Row:87 Col:44 */
    {0x9B49,0xF7CB}, /* 魉: GB2312 Code: 0xF7CB ==> Row:87 Col:43 */
    {0x9B4D,0xF7CD}, /* 魍: GB2312 Code: 0xF7CD ==> Row:87 Col:45 */
    {0x9B4F,0xCEBA}, /* 魏: GB2312 Code: 0xCEBA ==> Row:46 Col:26 */
    {0x9B51,0xF7CE}, /* 魑: GB2312 Code: 0xF7CE ==> Row:87 Col:46 */
    {0x9B54,0xC4A7}, /* 魔: GB2312 Code: 0xC4A7 ==> Row:36 Col:07 */
    {0x9C7C,0xD3E3}, /* 鱼: GB2312 Code: 0xD3E3 ==> Row:51 Col:67 */
    {0x9C7F,0xF6CF}, /* 鱿: GB2312 Code: 0xF6CF ==> Row:86 Col:47 */
    {0x9C81,0xC2B3}, /* 鲁: GB2312 Code: 0xC2B3 ==> Row:34 Col:19 */
    {0x9C82,0xF6D0}, /* 鲂: GB2312 Code: 0xF6D0 ==> Row:86 Col:48 */
    {0x9C85,0xF6D1}, /* 鲅: GB2312 Code: 0xF6D1 ==> Row:86 Col:49 */
    {0x9C86,0xF6D2}, /* 鲆: GB2312 Code: 0xF6D2 ==> Row:86 Col:50 */
    {0x9C87,0xF6D3}, /* 鲇: GB2312 Code: 0xF6D3 ==> Row:86 Col:51 */
    {0x9C88,0xF6D4}, /* 鲈: GB2312 Code: 0xF6D4 ==> Row:86 Col:52 */
    {0x9C8B,0xF6D6}, /* 鲋: GB2312 Code: 0xF6D6 ==> Row:86 Col:54 */
    {0x9C8D,0xB1AB}, /* 鲍: GB2312 Code: 0xB1AB ==> Row:17 Col:11 */
    {0x9C8E,0xF6D7}, /* 鲎: GB2312 Code: 0xF6D7 ==> Row:86 Col:55 */
    {0x9C90,0xF6D8}, /* 鲐: GB2312 Code: 0xF6D8 ==> Row:86 Col:56 */
    {0x9C91,0xF6D9}, /* 鲑: GB2312 Code: 0xF6D9 ==> Row:86 Col:57 */
    {0x9C92,0xF6DA}, /* 鲒: GB2312 Code: 0xF6DA ==> Row:86 Col:58 */
    {0x9C94,0xF6DB}, /* 鲔: GB2312 Code: 0xF6DB ==> Row:86 Col:59 */
    {0x9C95,0xF6DC}, /* 鲕: GB2312 Code: 0xF6DC ==> Row:86 Col:60 */
    {0x9C9A,0xF6DD}, /* 鲚: GB2312 Code: 0xF6DD ==> Row:86 Col:61 */
    {0x9C9B,0xF6DE}, /* 鲛: GB2312 Code: 0xF6DE ==> Row:86 Col:62 */
    {0x9C9C,0xCFCA}, /* 鲜: GB2312 Code: 0xCFCA ==> Row:47 Col:42 */
    {0x9C9E,0xF6DF}, /* 鲞: GB2312 Code: 0xF6DF ==> Row:86 Col:63 */
    {0x9C9F,0xF6E0}, /* 鲟: GB2312 Code: 0xF6E0 ==> Row:86 Col:64 */
    {0x9CA0,0xF6E1}, /* 鲠: GB2312 Code: 0xF6E1 ==> Row:86 Col:65 */
    {0x9CA1,0xF6E2}, /* 鲡: GB2312 Code: 0xF6E2 ==> Row:86 Col:66 */
    {0x9CA2,0xF6E3}, /* 鲢: GB2312 Code: 0xF6E3 ==> Row:86 Col:67 */
    {0x9CA3,0xF6E4}, /* 鲣: GB2312 Code: 0xF6E4 ==> Row:86 Col:68 */
    {0x9CA4,0xC0F0}, /* 鲤: GB2312 Code: 0xC0F0 ==> Row:32 Col:80 */
    {0x9CA5,0xF6E5}, /* 鲥: GB2312 Code: 0xF6E5 ==> Row:86 Col:69 */
    {0x9CA6,0xF6E6}, /* 鲦: GB2312 Code: 0xF6E6 ==> Row:86 Col:70 */
    {0x9CA7,0xF6E7}, /* 鲧: GB2312 Code: 0xF6E7 ==> Row:86 Col:71 */
    {0x9CA8,0xF6E8}, /* 鲨: GB2312 Code: 0xF6E8 ==> Row:86 Col:72 */
    {0x9CA9,0xF6E9}, /* 鲩: GB2312 Code: 0xF6E9 ==> Row:86 Col:73 */
    {0x9CAB,0xF6EA}, /* 鲫: GB2312 Code: 0xF6EA ==> Row:86 Col:74 */
    {0x9CAD,0xF6EB}, /* 鲭: GB2312 Code: 0xF6EB ==> Row:86 Col:75 */
    {0x9CAE,0xF6EC}, /* 鲮: GB2312 Code: 0xF6EC ==> Row:86 Col:76 */
    {0x9CB0,0xF6ED}, /* 鲰: GB2312 Code: 0xF6ED ==> Row:86 Col:77 */
    {0x9CB1,0xF6EE}, /* 鲱: GB2312 Code: 0xF6EE ==> Row:86 Col:78 */
    {0x9CB2,0xF6EF}, /* 鲲: GB2312 Code: 0xF6EF ==> Row:86 Col:79 */
    {0x9CB3,0xF6F0}, /* 鲳: GB2312 Code: 0xF6F0 ==> Row:86 Col:80 */
    {0x9CB4,0xF6F1}, /* 鲴: GB2312 Code: 0xF6F1 ==> Row:86 Col:81 */
    {0x9CB5,0xF6F2}, /* 鲵: GB2312 Code: 0xF6F2 ==> Row:86 Col:82 */
    {0x9CB6,0xF6F3}, /* 鲶: GB2312 Code: 0xF6F3 ==> Row:86 Col:83 */
    {0x9CB7,0xF6F4}, /* 鲷: GB2312 Code: 0xF6F4 ==> Row:86 Col:84 */
    {0x9CB8,0xBEA8}, /* 鲸: GB2312 Code: 0xBEA8 ==> Row:30 Col:08 */
    {0x9CBA,0xF6F5}, /* 鲺: GB2312 Code: 0xF6F5 ==> Row:86 Col:85 */
    {0x9CBB,0xF6F6}, /* 鲻: GB2312 Code: 0xF6F6 ==> Row:86 Col:86 */
    {0x9CBC,0xF6F7}, /* 鲼: GB2312 Code: 0xF6F7 ==> Row:86 Col:87 */
    {0x9CBD,0xF6F8}, /* 鲽: GB2312 Code: 0xF6F8 ==> Row:86 Col:88 */
    {0x9CC3,0xC8FA}, /* 鳃: GB2312 Code: 0xC8FA ==> Row:40 Col:90 */
    {0x9CC4,0xF6F9}, /* 鳄: GB2312 Code: 0xF6F9 ==> Row:86 Col:89 */
    {0x9CC5,0xF6FA}, /* 鳅: GB2312 Code: 0xF6FA ==> Row:86 Col:90 */
    {0x9CC6,0xF6FB}, /* 鳆: GB2312 Code: 0xF6FB ==> Row:86 Col:91 */
    {0x9CC7,0xF6FC}, /* 鳇: GB2312 Code: 0xF6FC ==> Row:86 Col:92 */
    {0x9CCA,0xF6FD}, /* 鳊: GB2312 Code: 0xF6FD ==> Row:86 Col:93 */
    {0x9CCB,0xF6FE}, /* 鳋: GB2312 Code: 0xF6FE ==> Row:86 Col:94 */
    {0x9CCC,0xF7A1}, /* 鳌: GB2312 Code: 0xF7A1 ==> Row:87 Col:01 */
    {0x9CCD,0xF7A2}, /* 鳍: GB2312 Code: 0xF7A2 ==> Row:87 Col:02 */
    {0x9CCE,0xF7A3}, /* 鳎: GB2312 Code: 0xF7A3 ==> Row:87 Col:03 */
    {0x9CCF,0xF7A4}, /* 鳏: GB2312 Code: 0xF7A4 ==> Row:87 Col:04 */
    {0x9CD0,0xF7A5}, /* 鳐: GB2312 Code: 0xF7A5 ==> Row:87 Col:05 */
    {0x9CD3,0xF7A6}, /* 鳓: GB2312 Code: 0xF7A6 ==> Row:87 Col:06 */
    {0x9CD4,0xF7A7}, /* 鳔: GB2312 Code: 0xF7A7 ==> Row:87 Col:07 */
    {0x9CD5,0xF7A8}, /* 鳕: GB2312 Code: 0xF7A8 ==> Row:87 Col:08 */
    {0x9CD6,0xB1EE}, /* 鳖: GB2312 Code: 0xB1EE ==> Row:17 Col:78 */
    {0x9CD7,0xF7A9}, /* 鳗: GB2312 Code: 0xF7A9 ==> Row:87 Col:09 */
    {0x9CD8,0xF7AA}, /* 鳘: GB2312 Code: 0xF7AA ==> Row:87 Col:10 */
    {0x9CD9,0xF7AB}, /* 鳙: GB2312 Code: 0xF7AB ==> Row:87 Col:11 */
    {0x9CDC,0xF7AC}, /* 鳜: GB2312 Code: 0xF7AC ==> Row:87 Col:12 */
    {0x9CDD,0xF7AD}, /* 鳝: GB2312 Code: 0xF7AD ==> Row:87 Col:13 */
    {0x9CDE,0xC1DB}, /* 鳞: GB2312 Code: 0xC1DB ==> Row:33 Col:59 */
    {0x9CDF,0xF7AE}, /* 鳟: GB2312 Code: 0xF7AE ==> Row:87 Col:14 */
    {0x9CE2,0xF7AF}, /* 鳢: GB2312 Code: 0xF7AF ==> Row:87 Col:15 */
    {0x9E1F,0xC4F1}, /* 鸟: GB2312 Code: 0xC4F1 ==> Row:36 Col:81 */
    {0x9E20,0xF0AF}, /* 鸠: GB2312 Code: 0xF0AF ==> Row:80 Col:15 */
    {0x9E21,0xBCA6}, /* 鸡: GB2312 Code: 0xBCA6 ==> Row:28 Col:06 */
    {0x9E22,0xF0B0}, /* 鸢: GB2312 Code: 0xF0B0 ==> Row:80 Col:16 */
    {0x9E23,0xC3F9}, /* 鸣: GB2312 Code: 0xC3F9 ==> Row:35 Col:89 */
    {0x9E25,0xC5B8}, /* 鸥: GB2312 Code: 0xC5B8 ==> Row:37 Col:24 */
    {0x9E26,0xD1BB}, /* 鸦: GB2312 Code: 0xD1BB ==> Row:49 Col:27 */
    {0x9E28,0xF0B1}, /* 鸨: GB2312 Code: 0xF0B1 ==> Row:80 Col:17 */
    {0x9E29,0xF0B2}, /* 鸩: GB2312 Code: 0xF0B2 ==> Row:80 Col:18 */
    {0x9E2A,0xF0B3}, /* 鸪: GB2312 Code: 0xF0B3 ==> Row:80 Col:19 */
    {0x9E2B,0xF0B4}, /* 鸫: GB2312 Code: 0xF0B4 ==> Row:80 Col:20 */
    {0x9E2C,0xF0B5}, /* 鸬: GB2312 Code: 0xF0B5 ==> Row:80 Col:21 */
    {0x9E2D,0xD1BC}, /* 鸭: GB2312 Code: 0xD1BC ==> Row:49 Col:28 */
    {0x9E2F,0xD1EC}, /* 鸯: GB2312 Code: 0xD1EC ==> Row:49 Col:76 */
    {0x9E31,0xF0B7}, /* 鸱: GB2312 Code: 0xF0B7 ==> Row:80 Col:23 */
    {0x9E32,0xF0B6}, /* 鸲: GB2312 Code: 0xF0B6 ==> Row:80 Col:22 */
    {0x9E33,0xD4A7}, /* 鸳: GB2312 Code: 0xD4A7 ==> Row:52 Col:07 */
    {0x9E35,0xCDD2}, /* 鸵: GB2312 Code: 0xCDD2 ==> Row:45 Col:50 */
    {0x9E36,0xF0B8}, /* 鸶: GB2312 Code: 0xF0B8 ==> Row:80 Col:24 */
    {0x9E37,0xF0BA}, /* 鸷: GB2312 Code: 0xF0BA ==> Row:80 Col:26 */
    {0x9E38,0xF0B9}, /* 鸸: GB2312 Code: 0xF0B9 ==> Row:80 Col:25 */
    {0x9E39,0xF0BB}, /* 鸹: GB2312 Code: 0xF0BB ==> Row:80 Col:27 */
    {0x9E3A,0xF0BC}, /* 鸺: GB2312 Code: 0xF0BC ==> Row:80 Col:28 */
    {0x9E3D,0xB8EB}, /* 鸽: GB2312 Code: 0xB8EB ==> Row:24 Col:75 */
    {0x9E3E,0xF0BD}, /* 鸾: GB2312 Code: 0xF0BD ==> Row:80 Col:29 */
    {0x9E3F,0xBAE8}, /* 鸿: GB2312 Code: 0xBAE8 ==> Row:26 Col:72 */
    {0x9E41,0xF0BE}, /* 鹁: GB2312 Code: 0xF0BE ==> Row:80 Col:30 */
    {0x9E42,0xF0BF}, /* 鹂: GB2312 Code: 0xF0BF ==> Row:80 Col:31 */
    {0x9E43,0xBEE9}, /* 鹃: GB2312 Code: 0xBEE9 ==> Row:30 Col:73 */
    {0x9E44,0xF0C0}, /* 鹄: GB2312 Code: 0xF0C0 ==> Row:80 Col:32 */
    {0x9E45,0xB6EC}, /* 鹅: GB2312 Code: 0xB6EC ==> Row:22 Col:76 */
    {0x9E46,0xF0C1}, /* 鹆: GB2312 Code: 0xF0C1 ==> Row:80 Col:33 */
    {0x9E47,0xF0C2}, /* 鹇: GB2312 Code: 0xF0C2 ==> Row:80 Col:34 */
    {0x9E48,0xF0C3}, /* 鹈: GB2312 Code: 0xF0C3 ==> Row:80 Col:35 */
    {0x9E49,0xF0C4}, /* 鹉: GB2312 Code: 0xF0C4 ==> Row:80 Col:36 */
    {0x9E4A,0xC8B5}, /* 鹊: GB2312 Code: 0xC8B5 ==> Row:40 Col:21 */
    {0x9E4B,0xF0C5}, /* 鹋: GB2312 Code: 0xF0C5 ==> Row:80 Col:37 */
    {0x9E4C,0xF0C6}, /* 鹌: GB2312 Code: 0xF0C6 ==> Row:80 Col:38 */
    {0x9E4E,0xF0C7}, /* 鹎: GB2312 Code: 0xF0C7 ==> Row:80 Col:39 */
    {0x9E4F,0xC5F4}, /* 鹏: GB2312 Code: 0xC5F4 ==> Row:37 Col:84 */
    {0x9E51,0xF0C8}, /* 鹑: GB2312 Code: 0xF0C8 ==> Row:80 Col:40 */
    {0x9E55,0xF0C9}, /* 鹕: GB2312 Code: 0xF0C9 ==> Row:80 Col:41 */
    {0x9E57,0xF0CA}, /* 鹗: GB2312 Code: 0xF0CA ==> Row:80 Col:42 */
    {0x9E58,0xF7BD}, /* 鹘: GB2312 Code: 0xF7BD ==> Row:87 Col:29 */
    {0x9E5A,0xF0CB}, /* 鹚: GB2312 Code: 0xF0CB ==> Row:80 Col:43 */
    {0x9E5B,0xF0CC}, /* 鹛: GB2312 Code: 0xF0CC ==> Row:80 Col:44 */
    {0x9E5C,0xF0CD}, /* 鹜: GB2312 Code: 0xF0CD ==> Row:80 Col:45 */
    {0x9E5E,0xF0CE}, /* 鹞: GB2312 Code: 0xF0CE ==> Row:80 Col:46 */
    {0x9E63,0xF0CF}, /* 鹣: GB2312 Code: 0xF0CF ==> Row:80 Col:47 */
    {0x9E64,0xBAD7}, /* 鹤: GB2312 Code: 0xBAD7 ==> Row:26 Col:55 */
    {0x9E66,0xF0D0}, /* 鹦: GB2312 Code: 0xF0D0 ==> Row:80 Col:48 */
    {0x9E67,0xF0D1}, /* 鹧: GB2312 Code: 0xF0D1 ==> Row:80 Col:49 */
    {0x9E68,0xF0D2}, /* 鹨: GB2312 Code: 0xF0D2 ==> Row:80 Col:50 */
    {0x9E69,0xF0D3}, /* 鹩: GB2312 Code: 0xF0D3 ==> Row:80 Col:51 */
    {0x9E6A,0xF0D4}, /* 鹪: GB2312 Code: 0xF0D4 ==> Row:80 Col:52 */
    {0x9E6B,0xF0D5}, /* 鹫: GB2312 Code: 0xF0D5 ==> Row:80 Col:53 */
    {0x9E6C,0xF0D6}, /* 鹬: GB2312 Code: 0xF0D6 ==> Row:80 Col:54 */
    {0x9E6D,0xF0D8}, /* 鹭: GB2312 Code: 0xF0D8 ==> Row:80 Col:56 */
    {0x9E70,0xD3A5}, /* 鹰: GB2312 Code: 0xD3A5 ==> Row:51 Col:05 */
    {0x9E71,0xF0D7}, /* 鹱: GB2312 Code: 0xF0D7 ==> Row:80 Col:55 */
    {0x9E73,0xF0D9}, /* 鹳: GB2312 Code: 0xF0D9 ==> Row:80 Col:57 */
    {0x9E7E,0xF5BA}, /* 鹾: GB2312 Code: 0xF5BA ==> Row:85 Col:26 */
    {0x9E7F,0xC2B9}, /* 鹿: GB2312 Code: 0xC2B9 ==> Row:34 Col:25 */
    {0x9E82,0xF7E4}, /* 麂: GB2312 Code: 0xF7E4 ==> Row:87 Col:68 */
    {0x9E87,0xF7E5}, /* 麇: GB2312 Code: 0xF7E5 ==> Row:87 Col:69 */
    {0x9E88,0xF7E6}, /* 麈: GB2312 Code: 0xF7E6 ==> Row:87 Col:70 */
    {0x9E8B,0xF7E7}, /* 麋: GB2312 Code: 0xF7E7 ==> Row:87 Col:71 */
    {0x9E92,0xF7E8}, /* 麒: GB2312 Code: 0xF7E8 ==> Row:87 Col:72 */
    {0x9E93,0xC2B4}, /* 麓: GB2312 Code: 0xC2B4 ==> Row:34 Col:20 */
    {0x9E9D,0xF7EA}, /* 麝: GB2312 Code: 0xF7EA ==> Row:87 Col:74 */
    {0x9E9F,0xF7EB}, /* 麟: GB2312 Code: 0xF7EB ==> Row:87 Col:75 */
    {0x9EA6,0xC2F3}, /* 麦: GB2312 Code: 0xC2F3 ==> Row:34 Col:83 */
    {0x9EB4,0xF4F0}, /* 麴: GB2312 Code: 0xF4F0 ==> Row:84 Col:80 */
    {0x9EB8,0xF4EF}, /* 麸: GB2312 Code: 0xF4EF ==> Row:84 Col:79 */
    {0x9EBB,0xC2E9}, /* 麻: GB2312 Code: 0xC2E9 ==> Row:34 Col:73 */
    {0x9EBD,0xF7E1}, /* 麽: GB2312 Code: 0xF7E1 ==> Row:87 Col:65 */
    {0x9EBE,0xF7E2}, /* 麾: GB2312 Code: 0xF7E2 ==> Row:87 Col:66 */
    {0x9EC4,0xBBC6}, /* 黄: GB2312 Code: 0xBBC6 ==> Row:27 Col:38 */
    {0x9EC9,0xD9E4}, /* 黉: GB2312 Code: 0xD9E4 ==> Row:57 Col:68 */
    {0x9ECD,0xCAF2}, /* 黍: GB2312 Code: 0xCAF2 ==> Row:42 Col:82 */
    {0x9ECE,0xC0E8}, /* 黎: GB2312 Code: 0xC0E8 ==> Row:32 Col:72 */
    {0x9ECF,0xF0A4}, /* 黏: GB2312 Code: 0xF0A4 ==> Row:80 Col:04 */
    {0x9ED1,0xBADA}, /* 黑: GB2312 Code: 0xBADA ==> Row:26 Col:58 */
    {0x9ED4,0xC7AD}, /* 黔: GB2312 Code: 0xC7AD ==> Row:39 Col:13 */
    {0x9ED8,0xC4AC}, /* 默: GB2312 Code: 0xC4AC ==> Row:36 Col:12 */
    {0x9EDB,0xF7EC}, /* 黛: GB2312 Code: 0xF7EC ==> Row:87 Col:76 */
    {0x9EDC,0xF7ED}, /* 黜: GB2312 Code: 0xF7ED ==> Row:87 Col:77 */
    {0x9EDD,0xF7EE}, /* 黝: GB2312 Code: 0xF7EE ==> Row:87 Col:78 */
    {0x9EDF,0xF7F0}, /* 黟: GB2312 Code: 0xF7F0 ==> Row:87 Col:80 */
    {0x9EE0,0xF7EF}, /* 黠: GB2312 Code: 0xF7EF ==> Row:87 Col:79 */
    {0x9EE2,0xF7F1}, /* 黢: GB2312 Code: 0xF7F1 ==> Row:87 Col:81 */
    {0x9EE5,0xF7F4}, /* 黥: GB2312 Code: 0xF7F4 ==> Row:87 Col:84 */
    {0x9EE7,0xF7F3}, /* 黧: GB2312 Code: 0xF7F3 ==> Row:87 Col:83 */
    {0x9EE9,0xF7F2}, /* 黩: GB2312 Code: 0xF7F2 ==> Row:87 Col:82 */
    {0x9EEA,0xF7F5}, /* 黪: GB2312 Code: 0xF7F5 ==> Row:87 Col:85 */
    {0x9EEF,0xF7F6}, /* 黯: GB2312 Code: 0xF7F6 ==> Row:87 Col:86 */
    {0x9EF9,0xEDE9}, /* 黹: GB2312 Code: 0xEDE9 ==> Row:77 Col:73 */
    {0x9EFB,0xEDEA}, /* 黻: GB2312 Code: 0xEDEA ==> Row:77 Col:74 */
    {0x9EFC,0xEDEB}, /* 黼: GB2312 Code: 0xEDEB ==> Row:77 Col:75 */
    {0x9EFE,0xF6BC}, /* 黾: GB2312 Code: 0xF6BC ==> Row:86 Col:28 */
    {0x9F0B,0xF6BD}, /* 鼋: GB2312 Code: 0xF6BD ==> Row:86 Col:29 */
    {0x9F0D,0xF6BE}, /* 鼍: GB2312 Code: 0xF6BE ==> Row:86 Col:30 */
    {0x9F0E,0xB6A6}, /* 鼎: GB2312 Code: 0xB6A6 ==> Row:22 Col:06 */
    {0x9F10,0xD8BE}, /* 鼐: GB2312 Code: 0xD8BE ==> Row:56 Col:30 */
    {0x9F13,0xB9C4}, /* 鼓: GB2312 Code: 0xB9C4 ==> Row:25 Col:36 */
    {0x9F17,0xD8BB}, /* 鼗: GB2312 Code: 0xD8BB ==> Row:56 Col:27 */
    {0x9F19,0xDCB1}, /* 鼙: GB2312 Code: 0xDCB1 ==> Row:60 Col:17 */
    {0x9F20,0xCAF3}, /* 鼠: GB2312 Code: 0xCAF3 ==> Row:42 Col:83 */
    {0x9F22,0xF7F7}, /* 鼢: GB2312 Code: 0xF7F7 ==> Row:87 Col:87 */
    {0x9F2C,0xF7F8}, /* 鼬: GB2312 Code: 0xF7F8 ==> Row:87 Col:88 */
    {0x9F2F,0xF7F9}, /* 鼯: GB2312 Code: 0xF7F9 ==> Row:87 Col:89 */
    {0x9F37,0xF7FB}, /* 鼷: GB2312 Code: 0xF7FB ==> Row:87 Col:91 */
    {0x9F39,0xF7FA}, /* 鼹: GB2312 Code: 0xF7FA ==> Row:87 Col:90 */
    {0x9F3B,0xB1C7}, /* 鼻: GB2312 Code: 0xB1C7 ==> Row:17 Col:39 */
    {0x9F3D,0xF7FC}, /* 鼽: GB2312 Code: 0xF7FC ==> Row:87 Col:92 */
    {0x9F3E,0xF7FD}, /* 鼾: GB2312 Code: 0xF7FD ==> Row:87 Col:93 */
    {0x9F44,0xF7FE}, /* 齄: GB2312 Code: 0xF7FE ==> Row:87 Col:94 */
    {0x9F50,0xC6EB}, /* 齐: GB2312 Code: 0xC6EB ==> Row:38 Col:75 */
    {0x9F51,0xECB4}, /* 齑: GB2312 Code: 0xECB4 ==> Row:76 Col:20 */
    {0x9F7F,0xB3DD}, /* 齿: GB2312 Code: 0xB3DD ==> Row:19 Col:61 */
    {0x9F80,0xF6B3}, /* 龀: GB2312 Code: 0xF6B3 ==> Row:86 Col:19 */
    {0x9F83,0xF6B4}, /* 龃: GB2312 Code: 0xF6B4 ==> Row:86 Col:20 */
    {0x9F84,0xC1E4}, /* 龄: GB2312 Code: 0xC1E4 ==> Row:33 Col:68 */
    {0x9F85,0xF6B5}, /* 龅: GB2312 Code: 0xF6B5 ==> Row:86 Col:21 */
    {0x9F86,0xF6B6}, /* 龆: GB2312 Code: 0xF6B6 ==> Row:86 Col:22 */
    {0x9F87,0xF6B7}, /* 龇: GB2312 Code: 0xF6B7 ==> Row:86 Col:23 */
    {0x9F88,0xF6B8}, /* 龈: GB2312 Code: 0xF6B8 ==> Row:86 Col:24 */
    {0x9F89,0xF6B9}, /* 龉: GB2312 Code: 0xF6B9 ==> Row:86 Col:25 */
    {0x9F8A,0xF6BA}, /* 龊: GB2312 Code: 0xF6BA ==> Row:86 Col:26 */
    {0x9F8B,0xC8A3}, /* 龋: GB2312 Code: 0xC8A3 ==> Row:40 Col:03 */
    {0x9F8C,0xF6BB}, /* 龌: GB2312 Code: 0xF6BB ==> Row:86 Col:27 */
    {0x9F99,0xC1FA}, /* 龙: GB2312 Code: 0xC1FA ==> Row:33 Col:90 */
    {0x9F9A,0xB9A8}, /* 龚: GB2312 Code: 0xB9A8 ==> Row:25 Col:08 */
    {0x9F9B,0xEDE8}, /* 龛: GB2312 Code: 0xEDE8 ==> Row:77 Col:72 */
    {0x9F9F,0xB9EA}, /* 龟: GB2312 Code: 0xB9EA ==> Row:25 Col:74 */
    {0x9FA0,0xD9DF}, /* 龠: GB2312 Code: 0xD9DF ==> Row:57 Col:63 */
    {0xE810,0xD7FA}, /* ??: GB2312 Code: 0xD7FA ==> Row:55 Col:90 */
    {0xE811,0xD7FB}, /* ?? : GB2312 Code: 0xD7FB ==> Row:55 Col:91 */
    {0xE812,0xD7FC}, /* ??: GB2312 Code: 0xD7FC ==> Row:55 Col:92 */
    {0xE813,0xD7FD}, /* ?? : GB2312 Code: 0xD7FD ==> Row:55 Col:93 */
    {0xE814,0xD7FE}, /* ??: GB2312 Code: 0xD7FE ==> Row:55 Col:94 */
	{0xFF01,0xA3A1}, // ！
	{0xFF02,0xA3A2}, // ＂
	{0xFF03,0xA3A3}, // ＃
	{0xFF04,0xA1E7}, // ＄
	{0xFF05,0xA3A5}, // ％
	{0xFF06,0xA3A6}, // ＆
	{0xFF07,0xA3A7}, // ＇
	{0xFF08,0xA3A8}, // （
	{0xFF09,0xA3A9}, // ）
	{0xFF0A,0xA3AA}, // ＊
	{0xFF0B,0xA3AB}, // ＋
	{0xFF0C,0xA3AC}, // ，
	{0xFF0D,0xA3AD}, // －
	{0xFF0E,0xA3AE}, // ．
	{0xFF0F,0xA3AF}, // ／
	{0xFF10,0xA3B0}, // ０
	{0xFF11,0xA3B1}, // １
	{0xFF12,0xA3B2}, // ２
	{0xFF13,0xA3B3}, // ３
	{0xFF14,0xA3B4}, // ４
	{0xFF15,0xA3B5}, // ５
	{0xFF16,0xA3B6}, // ６
	{0xFF17,0xA3B7}, // ７
	{0xFF18,0xA3B8}, // ８
	{0xFF19,0xA3B9}, // ９
	{0xFF1A,0xA3BA}, // ：
	{0xFF1B,0xA3BB}, // ；
	{0xFF1C,0xA3BC}, // ＜
	{0xFF1D,0xA3BD}, // ＝
	{0xFF1E,0xA3BE}, // ＞
	{0xFF1F,0xA3BF}, // ？
	{0xFF20,0xA3C0}, // ＠
	{0xFF21,0xA3C1}, // Ａ
	{0xFF22,0xA3C2}, // Ｂ
	{0xFF23,0xA3C3}, // Ｃ
	{0xFF24,0xA3C4}, // Ｄ
	{0xFF25,0xA3C5}, // Ｅ
	{0xFF26,0xA3C6}, // Ｆ
	{0xFF27,0xA3C7}, // Ｇ
	{0xFF28,0xA3C8}, // Ｈ
	{0xFF29,0xA3C9}, // Ｉ
	{0xFF2A,0xA3CA}, // Ｊ
	{0xFF2B,0xA3CB}, // Ｋ
	{0xFF2C,0xA3CC}, // Ｌ
	{0xFF2D,0xA3CD}, // Ｍ
	{0xFF2E,0xA3CE}, // Ｎ
	{0xFF2F,0xA3CF}, // Ｏ
	{0xFF30,0xA3D0}, // Ｐ
	{0xFF31,0xA3D1}, // Ｑ
	{0xFF32,0xA3D2}, // Ｒ
	{0xFF33,0xA3D3}, // Ｓ
	{0xFF34,0xA3D4}, // Ｔ
	{0xFF35,0xA3D5}, // Ｕ
	{0xFF36,0xA3D6}, // Ｖ
	{0xFF37,0xA3D7}, // Ｗ
	{0xFF38,0xA3D8}, // Ｘ
	{0xFF39,0xA3D9}, // Ｙ
	{0xFF3A,0xA3DA}, // Ｚ
	{0xFF3B,0xA3DB}, // ［
	{0xFF3C,0xA3DC}, // ＼
	{0xFF3D,0xA3DD}, // ］
	{0xFF3E,0xA3DE}, // ＾
	{0xFF3F,0xA3DF}, // ＿
	{0xFF40,0xA3E0}, // ｀
	{0xFF41,0xA3E1}, // ａ
	{0xFF42,0xA3E2}, // ｂ
	{0xFF43,0xA3E3}, // ｃ
	{0xFF44,0xA3E4}, // ｄ
	{0xFF45,0xA3E5}, // ｅ
	{0xFF46,0xA3E6}, // ｆ
	{0xFF47,0xA3E7}, // ｇ
	{0xFF48,0xA3E8}, // ｈ
	{0xFF49,0xA3E9}, // ｉ
	{0xFF4A,0xA3EA}, // ｊ
	{0xFF4B,0xA3EB}, // ｋ
	{0xFF4C,0xA3EC}, // ｌ
	{0xFF4D,0xA3ED}, // ｍ
	{0xFF4E,0xA3EE}, // ｎ
	{0xFF4F,0xA3EF}, // ｏ
	{0xFF50,0xA3F0}, // ｐ
	{0xFF51,0xA3F1}, // ｑ
	{0xFF52,0xA3F2}, // ｒ
	{0xFF53,0xA3F3}, // ｓ
	{0xFF54,0xA3F4}, // ｔ
	{0xFF55,0xA3F5}, // ｕ
	{0xFF56,0xA3F6}, // ｖ
	{0xFF57,0xA3F7}, // ｗ
	{0xFF58,0xA3F8}, // ｘ
	{0xFF59,0xA3F9}, // ｙ
	{0xFF5A,0xA3FA}, // ｚ
	{0xFF5B,0xA3FB}, // ｛
	{0xFF5C,0xA3FC}, // ｜
	{0xFF5D,0xA3FD}, // ｝
	{0xFF5E,0xA1AB}, // ～
	{0xFFE0,0xA1E9}, // ￠
	{0xFFE1,0xA1EA}, // ￡
	{0xFFE3,0xA3FE}, // ￣
	{0xFFE5,0xA3A4}, // ￥
};

#define MAX_UNI_INDEX	(sizeof(uni_table)/sizeof(uni_table[0]))

static OSDV2TextKeyword_t OSDV2KeywordList[] = {
	{OSDV2_KW_RES, 9*8, 0, 5, {'[', 'R', 'E', 'S', ']'}},
	{OSDV2_KW_BPS, 7*8, 0, 5, {'[', 'B', 'P', 'S', ']'}},
	{OSDV2_KW_LGT, 30*8, 0, 5, {'[', 'L', 'G', 'T', ']'}},
    {OSDV2_KW_TMP,  6*8,    0, 5, {'[', 'T', 'M', 'P', ']'}},
};

static OSDV2_TEXT_OVERLAY_T OSDV2OverlayList[] = {
	{5, {'[', 'R', 'E', 'S', ']'}},
	{5, {'[', 'B', 'P', 'S', ']'}},
    {11, {'[', 'R', 'E', 'S', ']', ' ', '[', 'R', 'E', 'S', ']'}}
};

#define OSDV2_TEXT_KEYWORD_NUM		(sizeof(OSDV2KeywordList)/sizeof(OSDV2KeywordList[0]))

#ifdef OSD_DEBUG
/*!
\\! 功能 将data 高低字节交换

*/
static unsigned char shift_fun2(unsigned char data) 
{ 
    data=(data<<4)|(data>>4); 
    data=((data<<2)&0xcc)|((data>>2)&0x33); 
    data=((data<<1)&0xaa)|((data>>1)&0x55); 
 //   TRACE_AVL("  after shift fun2 data=%x \n",data); 
 
    return data; 
}

/*!
/\! 功能 将data 按字节正序打印位

*/
static int print_char(int len,int iendl,unsigned char *data)
{
int m=0,n=0,tmp=0,flag=0;
unsigned char tmp_ch;
OSD_DBG("\n==********************************===");
cout<<endl<<"======================len="<<len<<"=============================="<<endl;
	for (m=0;m<len;m++)
	{
	OSD_DBG("0x%02x,",data[m]);

	if (m%10 ==0)
		cout<<endl;
	/*/tmp_ch=shift_fun2(data[m]);
	
		for (n=0;n<8;n++)
		{
		
			tmp=(tmp_ch>>n)&1;
			cout<<tmp;
			flag++;
		}
		if (flag%iendl == 0)
		{
			cout<<endl;
			flag=0;
		}
		*/
	}
cout<<endl<<"================================================================="<<endl;
	return 0;
}
#endif


#define SWAP16(x) (test_lite_big()?(x):(((((x)&0xff)<<8)|(((x)>>8)&0xff))))
#define SWAP32(x)  (test_lite_big()?(x):(((SWAP16((x)&0xffff)<<16)|(SWAP16((x)>>16)))))

//点阵文件路径
//static const char *FileNameOsdPixel = "/tmp/app/res/";

//配置区名
static const char *OsdCfgSection = "OsdCfg";

extern "C" int mysystem(const char *cmdstring);

//二分查找
uint16 BinarySearch(const uint16 x)
{ 
	int left = 0;
	
	int right = MAX_UNI_INDEX - 1;
	
	while(left <= right)
	{
		int middle = (left+right)/2;
		
		if (x == uni_table[middle][0]) 
			return uni_table[middle][1];
		if (x > uni_table[middle][0]) 
			left = middle + 1;
		else 
			right = middle - 1;
	}
	return 0;
}

//将unicode转换为国标编码
void UniToStr(const uint16 *uni,uchar *str)
{
	uint16 gb;
	while( *uni )
	{
		if(0 == (*uni >> 8)) //ASCII
		{
			#ifdef SUPPORT_TEXT_OSD_TWO_LINE
			if(NEW_LINE_CHAR != *uni)//跳过换行符
			#endif
			{
				*str++ = (*uni & 0xff);
				*str++ = 0;
			}
		}
		else	//汉字GB
		{
			gb = BinarySearch(*uni);
			*str ++ = (gb >> 8) & 0xff;
			*str ++ = gb & 0xff;
		}
		uni ++;
	}
	*str ++ = '\0';
}

//for HK
void HKUniToStr(const uint16 *uni,uchar *str,int len)
{
    unsigned short gb;
    int cur_len = 0;
    while( *uni && cur_len<len)
    {
        if( (*uni >> 8) == 0) //ASCII
        {
            *str++ = (*uni & 0xff);
            //*str++ = 0;
        }
        else    //汉字GB
        {
            gb = BinarySearch(*uni);
            *str ++ = (gb >> 8) & 0xff;
            *str ++ = gb & 0xff;
        }
        uni ++;
    
        cur_len++;
    }
    *str ++ = '\0';

}

unsigned short getun(unsigned short int gb) 
{ 
    unsigned int i; 
    for(i=0;i<MAX_UNI_INDEX;i++) 
    { 
        if (uni_table[i][1]==gb) 
        { 
            return uni_table[i][0]; 
        } 
    } 
    return 0xFFFF; 
} 

int gb2unicode(unsigned short int*unicode,char*gb,int len) 
{ 
    int i,j; 
    i=0;     // i是gb的长度,j是unicode的长度
    for(j=0;i<len;j++) 
    { 
        fprintf(stderr, "gb[%d] = %02x\n", i, (unsigned)gb[i]);
        if ((unsigned char)gb[i]<=0x80) 
        { 
            // ascii
            unicode[j] = gb[i];
            unicode[j]= unicode[j]; 
            fprintf(stderr, "directly unicode = %02x\n", unicode[j]);
            i++; 
        }else 
        { 
            unsigned short gb_short = 0;
            unsigned short gb_short_hi  = *(gb+i);
            unsigned short gb_short_low = *(gb+i+1);
            gb_short = (gb_short_hi << 8) | gb_short_low;
            unicode[j]=getun(gb_short); 
            fprintf(stderr, "return unicode = 0x%x    gb_short = 0x%x\n", unicode[j], gb_short);
            i+=2; 
        } 
    } 
    return j*2; 
} 

int findStrInHZK16(unsigned short *value, char *buf)
{
	FILE *fp = 0;
	FILE *fpASC = 0;
	unsigned long offset=0;
	int i=0,j=0,m=0,n=0,count=0,h=0,l=0;
	int hzCount=0,ascCount=0,osdW=0,osdStride=0;
	char flag[OSD_TEXT_EX_MAX_EXTENT_OSD_TEXT_LENGTH] = {0};
	char *pSeg[OSD_TEXT_EX_MAX_EXTENT_OSD_TEXT_LENGTH] = {0};
	
	if ((fp=fopen(OSD_HZK_16,"r"))==NULL)
		return 1;
		
	if ((fpASC=fopen(OSD_ASC_16,"r"))==NULL)
	{
		fclose(fp);
		return 1;
	}

	while(value[i])
	{
		count++;
		if(value[i]&0xff00)
		{
			flag[i] = 1;
			hzCount++;
			osdW += 16;
			pSeg[i] = new char[32];
			l = value[i] >> 8;
			h = value[i] & 0x00ff;
			offset=((h-0xa1)*94+(l-0xa1))*32;
			fseek(fp,offset,SEEK_SET);
			fread(pSeg[i],32,1,fp);
		}
		else
		{
		#ifdef ASC_WIDTH_16
			flag[i] = 1;
			hzCount++;
			osdW += 16;
			pSeg[i] = new char[32];
			h = value[i];
			offset=h*16;
			fseek(fpASC,offset,SEEK_SET);
			fread(temp,16,1,fpASC);
			OSD_DBG("0x%x: ", value[i]);
			for(j=0;j<16;j++)
				OSD_DBG("0x%x, ", temp[j]);
			OSD_DBG("\n");
			for(j=0;j<16;j++)
			{
				 *(pSeg[i]+2*j) = temp[j] >> 4;
				 *(pSeg[i]+2*j+1) = temp[j] << 4;
			}
		#else
			flag[i] = 2;
			ascCount++;
			osdW += 8;
			pSeg[i] = new char[16];
			h = value[i];
			offset=h*16;
			fseek(fpASC,offset,SEEK_SET);
			fread(pSeg[i],16,1,fpASC);
		#endif
		}
		i++;
	}
	osdStride = (osdW >> 3);
	OSD_DBG("count=%d, hzCount=%d, ascCount=%d, osdStride=%d\n", count, hzCount, ascCount, osdStride);
	for(i=0;i<16;i++)
	{
		for(j=0;j<count;j++)
		{
			if(1 == flag[j])
			{
				for(m=0,n=0;m<j;m++)
				{
					if(1 == flag[m])
						n += 2;
					else
						n += 1;
				}
				buf[i*osdStride+n] = *(pSeg[j]+2*i);
				buf[i*osdStride+n+1] = *(pSeg[j]+2*i+1);
			}
			else
				{
					for(m=0,n=0;m<j;m++)
					{
						if(1 == flag[m])
							n += 2;
						else
							n += 1;
					}
					buf[i*osdStride+n] = *(pSeg[j]+i);
				}

		}
	}


	for(i=0;i<count;i++)
	{
		if (pSeg[i])
		{
			delete []pSeg[i];
			pSeg[i] = 0;
		}
	}

	fclose(fp);
	fclose(fpASC);

	return 0;
}

//由于sdk支持多行OSD的接口，在这里将TextOSDInfo_t数据转成TextOSDV2Info_t
//index:多行文本OSD的下标
int COsd::TextOSDToTextOSDV2(int index)
{
	//当多行osdV2 叠加信息后，用老接口设置时没处理好(老接口不支持叠加信息)，现设置时直接清空osdv2的第一块osd内容
	memset(&m_tTextOsdV2Info[index], 0, sizeof(m_tTextOsdV2Info));
	
    memcpy(m_tTextOsdV2Info[index].lineInfo, m_tTextOsd.lineInfo, sizeof(OSDV2TextLineInfo_t)*OSDV2_TEXT_MAX_LINE);
    m_tTextOsdV2Info[index].lineNum = m_tTextOsd.lineNum;
    m_tTextOsdV2Info[index].pPixinfo = m_tTextOsd.pPixinfo;
    m_tTextOsdV2Info[index].Start_X = m_tTextOsd.Start_X;
    m_tTextOsdV2Info[index].Start_Y = m_tTextOsd.Start_Y;
    memcpy(m_tTextOsdV2Info[index].text, m_tTextOsd.strGB2312, sizeof(unsigned short)*20);

    return 0;
}

//!\brief   设置扩展OSD，根据Token来定义OSD的功能
//!\param   OSDToken                OSD的功能，如osd_token_1表示对Text的扩展
//!\param   strText           		OSD内容，一般是unicode编码，有CharSet决定编码格式
//!\param   CharSet                 strText的编码格式
//!\param   pStrName                 解析后的纯Text内容
//!\param   pOffsetX                 解析后的x坐标
//!\param   pOffsetY                 解析后的y坐标
//!\return  成功为0，失败为-1
static int OsdParsing(char *OSDToken, unsigned short *strText, int CharSet, unsigned short *pStrName, unsigned int *pOffsetX, unsigned int *pOffsetY)
{
	int i = 0;
	int j = 0;
	int textCount = 0;
	int xCount = 0;
	int yCount = 0;
	int which = 0;
	int whichLine = 0;
	int line0Len = 0;
	int line1Len = 0;
	unsigned short temp[256] = {0};
	unsigned short offx[3] = {0};
	unsigned short offy[3] = {0};
	char			cOffx[4] = {0};
	char			cOffy[4] = {0};

	*pOffsetX = 0;
	*pOffsetY = 0;

    int nBoardID = 0;
    int OsdLen = 0;//一行Osd的长度
    ZWSys_GetBoardInfo(&nBoardID);  
    //由于内存原因:对3516EV200的F23/307/2232只允许设置每行 18 字符(9中文 或者 18字符)
    if((0x206E == nBoardID) || (0x206F == nBoardID) || (0x2073 == nBoardID))
    {
        OsdLen = 18;
    }
    else if(0x2076 == nBoardID)
    {
		OsdLen = 30;
    }
    else
    {
        OsdLen = 40;
    }

	if(0 == strcmp(OSDToken, OSD_TEXT_EX_TOKEN))
	{
		while(i<OSD_TEXT_EX_MAX_EXTENT_OSD_TEXT_LENGTH)
		{
			if(0 == strText[i])
				break;
			j=0;
			switch(which)
			{
				case 0:
					if(strText[i] == 't')
					{
						j++;
						if(strText[i+j] == '=')
						{
							i+=j;
							which = 1;
							break;
						}
					}
					break;

				case 1:
					if(textCount == OSD_TEXT_EX_MAX_CHAR)
					{
						if(strText[i] == '$')
						{
							j++;
							if(strText[i+j] == 'x')
							{
								j++;
								if(strText[i+j] == '=')
								{
									i+=j;
									which = 2;
									break;
								}
							}
						}
					}
					else//未满
					{
						if(strText[i] == '$')
						{
							j++;
							if(strText[i+j] == 'x')
							{
								j++;
								if(strText[i+j] == '=')
								{
									which = 2;
									i+=j;
									break;
								}
							}
						}
						temp[textCount++] = strText[i];
					}
					
					break;
				case 2://X坐标
					if(xCount == OSD_TEXT_EX_MAX_COORD_LEN)
					{
						if(strText[i] == '$')
						{
							j++;
							if(strText[i+j] == 'y')
							{
								j++;
								if(strText[i+j] == '=')
								{
									i+=j;
									which = 3;
									break;
								}
							}
						}
					}
					else//未满
					{
						if(strText[i] == '$')
						{
							j++;
							if(strText[i+j] == 'y')
							{
								j++;
								if(strText[i+j] == '=')
								{
									which = 3;
									i+=j;
									break;
								}
							}
						}
						offx[xCount++] = strText[i];
					}

					break;

				case 3://Y坐标
					if(yCount == OSD_TEXT_EX_MAX_COORD_LEN)
					{
						break;
					}
					else//未满
					{
						offy[yCount++] = strText[i];
					}
					break;
			}
			i++;
		}	

		//对每行的长度进行限制
		for(i=0,j=0; i<textCount; i++)
		{
			if(0 == whichLine)
			{
				if(temp[i] == '^')
				{
					if(!line0Len)
						continue;
					pStrName[j++] = temp[i];
					whichLine = 1;
				}
				else
					{
						if(line0Len >= OsdLen)
						{
							continue;
						}
						else
						{
							pStrName[j++] = temp[i];
							if(temp[i]&0xff00)
								line0Len+=2;
							else
								line0Len++;
						}
					}
			}
			else
			{
				if(temp[i] == '^')
				{
					break;
				}
				else
					{
						if(line1Len == OsdLen)  
						{
							break;
						}
						else
						{
							pStrName[j++] = temp[i];
							if(temp[i]&0xff00)
								line1Len+=2;
							else
								line1Len++;
						}
					}
			}
			
		}


		//转换为asc
		for(i=0; i<3; i++)
		{
			if(offx[i])
			{
				cOffx[i] = offx[i] & 0xff;
			}
			else
				{
					break;
				}
		}
		for(i=0; i<3; i++)
		{
			if(offy[i])
			{
				cOffy[i] = offy[i] & 0xff;
			}
			else
				{
					break;
				}
		}

		//对坐标进行限制
		*pOffsetX = atoi(cOffx);
		*pOffsetY = atoi(cOffy);
		
		*pOffsetX = (*pOffsetX > 703) ? 703 : (*pOffsetX < 0 ? 0 : *pOffsetX);
		*pOffsetY = (*pOffsetY > 575) ? 575 : (*pOffsetY < 0 ? 0 : *pOffsetY);

	}
	else
		{
			return -1;
		}
	
	
	return 0;
}

//!\brief   OSD字符串解析，最多支持两行，多于两行的被剪掉
//!\param[in|out]   strText                待解析字符串
//!\param[in]   len                 	OSD待解析OSD数据的长度
//!\return  成功为0，失败为-1
int COsd::OSDV1TextParsing(unsigned short *strText, int *maxLineWidth)
{
	int i = 0, j = 0;
	unsigned short strTmp[DVR_MAX_CHANNELNAME_LEN] = {0};
    int curLineWidth = 0;

    //清空一些数据
    memset(m_tTextOsd.lineInfo, 0, sizeof(OSDV2TextLineInfo_t)*OSDV2_TEXT_MAX_LINE);
    m_tTextOsd.lineNum = 0;
    *maxLineWidth = 0;

	while((0 != strText[i]) && (i < DVR_MAX_CHANNELNAME_LEN-1))
	{
		if ('^' == strText[i])
		{
			if (curLineWidth > 0) // 非空行，空行跳过
			{
				m_tTextOsd.lineInfo[m_tTextOsd.lineNum].pixWidth = curLineWidth;
                m_tTextOsd.lineNum++;
                *maxLineWidth = (*maxLineWidth > curLineWidth) ? (*maxLineWidth) : curLineWidth;
                curLineWidth = 0;

                // 检查行数是不是超限
                if(m_tTextOsd.lineNum >= OSDV1_TEXT_MAX_LINE)
                {
                    break;
                }

                strTmp[j++] = strText[i];
			}

            i++;
			continue;
		}

		// 普通文字
        if(strText[i] & 0xff00)
        {
            if((curLineWidth + 16) <= OSDV2_TEXT_MAX_LINE_WIDTH*8)
			{
                curLineWidth += 16;
                strTmp[j++] = strText[i];
            }
        }
        else
        {
            if((curLineWidth + 8) <= OSDV2_TEXT_MAX_LINE_WIDTH*8)
            {
                curLineWidth += 8;
                strTmp[j++] = strText[i];
            }
        }

        i++;
	}

	// 去除最后的换行符
	if ('^' == strTmp[j-1])
	{
		strTmp[j-1] = 0;
	}

    if((m_tTextOsd.lineNum < OSDV1_TEXT_MAX_LINE) && (curLineWidth > 0))
	{
		m_tTextOsd.lineInfo[m_tTextOsd.lineNum].pixWidth = curLineWidth;
        *maxLineWidth = (*maxLineWidth>curLineWidth)?(*maxLineWidth):curLineWidth;
		m_tTextOsd.lineNum++;
	}

	// 将处理后的内容，拷贝回去
	for(int m = 0; m < DVR_MAX_CHANNELNAME_LEN; m++)
	{
        strText[m] = strTmp[m];
    }

	return 0;
}

//!\brief   从unicode数据中提取关键字信息
//!\param[in]   start                	OSD待解析数据，第一个字符为'['
//!\param[in]   len                 	OSD待解析OSD数据的剩余长度
//!\param[out]  index                 	返回KeyWord的在表中的索引
//!\return  成功为0，失败为-1

int OSDV2TextGetKeyWord(unsigned short *start, int len, int *index)
{
	unsigned int i = 0;

	if((NULL == start) || (NULL == index))
	{
		return -1;
	}
	
	*index = -1;
	
	if((unsigned int)len < OSDV2_TEXT_KEYWORD_MAX_LENGTH)
	{
		*index = -1;
		return 0;
	}

	for(i = 0; i < OSDV2_TEXT_KEYWORD_NUM; i++)
	{
		if(len >= OSDV2KeywordList[i].wordLen)
		{
			if(!memcmp(OSDV2KeywordList[i].word, start, OSDV2KeywordList[i].wordLen * sizeof(unsigned short)))
			{
				*index = i;
				return 0;
			}
		}
	}

	if(i >= OSDV2_TEXT_KEYWORD_NUM)
	{
		*index = -1;
	}
	
	return 0;
}

/**************************************************************************/
COsd::COsd(int chhandle, void *hHandle)
{
	int nBoardID;
    int i;
	
	ZWSys_GetBoardInfo(&nBoardID);
	if(nBoardID == 0xc006 || nBoardID == 0xc007)
	{
		FileNameOsdPixel = OSD_PIXEL_S;
	}
	else
	{
		FileNameOsdPixel = OSD_PIXEL;
	}
	m_iChannel = chhandle;
	m_hEnc = hHandle;
	
	m_pixelBuf = NULL;
	m_iPixelBufLen = 0;
	m_iTextW = 550;
	m_iTextH = 48;
	m_iTimeW = 550;
	m_iTimeH = 48;
	m_iOldShowType = 0;
	m_iOsdFlowEn = 0;
	
#ifndef DO_SATA

	memset(m_strName, 0, sizeof(m_strName));

	m_tTimeOsd.Start_X= 0;
	m_tTimeOsd.Start_Y= 0;
	m_tTimeOsd.bHour24 = 0;
	m_tTimeOsd.bTranslant = 0;
	m_tTimeOsd.DisMod = Mode_YYYY_MM_DD;
	m_tTimeOsd.FontWidth = DEFAULT_TIME_PIXEL.w;
	m_tTimeOsd.FontHeight = DEFAULT_TIME_PIXEL.h;
	m_tTimeOsd.pFontPixinfo = (const char *) DEFAULT_TIME_PIXEL.pixel_buf;
//	OSD_Time_swap(pixel_time_buf,DEFAULT_TIME_PIXEL.pixel_buf);
//  m_tTimeOsd.pFontPixinfo =(char *)pixel_time_buf;
	
	m_iTimeW = DEFAULT_TIME_PIXEL.w * 22;
	m_iTimeH = DEFAULT_TIME_PIXEL.h;
#endif

	memset(m_strNameRaw, 0, sizeof(m_strNameRaw));
	memset(m_strNameEx, 0, sizeof(m_strNameEx));
	m_tTextOsdEx.Flag = 0;
	m_tTextOsdEx.pPixinfo = NULL;
	m_tTextOsdEx.Start_X = 0;
	m_tTextOsdEx.Start_Y = 0;
	m_tTextOsdEx.TextNum = 0;
	memset(m_tTextOsdEx.strOSD, 0, sizeof(m_tTextOsdEx.strOSD));
	m_iWhichValid = 0;
	// OSDV2
    for(i = 0; i < OSDV2_TEXT_CNT; i++)
    {
        memset(&m_tTextOsdV2Info[i], 0, sizeof(m_tTextOsdV2Info));
        m_tTextOsdV2Info[i].keyWordFontWidth = DEFAULT_OSDV2_PIXEL.w;
        m_tTextOsdV2Info[i].keyWordFontHeight = DEFAULT_OSDV2_PIXEL.h;
        m_tTextOsdV2Info[i].pKeywordPixInfo = (const char *)DEFAULT_OSDV2_PIXEL.pixel_buf;
    }
	
	memset(&m_tTextOsdV2, 0, sizeof(m_tTextOsdV2));

	memset(&m_tTextOsd, 0, sizeof(TextOSDInfo_t));
}

COsd::~COsd()
{
	UnInit();
}

//!\brief 初始化
int COsd::Init()
{
	OSD_DBG("\n......COsd::Init.....begin.........\n");
	pCConfig = CConfig::GetInstance();
	if (pCConfig == NULL)
	{
		OSD_ERR("[COsd:%s]pCConfig==NULL\n", __FUNCTION__);
		return -1;
	}

	LoadParam();

	return 0;
}

//!\brief 资源释放
int COsd::UnInit()
{
	OSD_DBG("\n......COsd::UnInit.....begin.........\n");
	if (m_pixelBuf != NULL)
	{
		delete m_pixelBuf;

		m_pixelBuf = NULL;
	}
	m_iPixelBufLen = 0;
	
	return 0;
}

//##ModelId=49A263BC029A
int COsd::GetType(int *nShowType)
{
	OSD_DBG("\n......COsd::GetType.....begin.........\n");

	if (NULL == nShowType)
	{
		OSD_ERR("[COsd:%s]nShowType==NULL\n", __FUNCTION__);
		return -1;
	}

	*nShowType = m_iShowType;

	return (int) 0;
}

int COsd::GetTimeWH(int *pWidth, int *pHeight)
{
	int cnt = 19;
	OSD_DBG("\n......COsd::GetTimeWH.....begin.........\n");
	
	if (NULL == pWidth || NULL == pHeight)
	{
		OSD_ERR("[COsd:%s]NULL == pWidth || NULL == pHeight\n", __FUNCTION__);
		return -1;
	}
	//设置时间OSD
	switch (m_iTimeType)
	{
		case 0:				//YYYYMMDD12
			{
				cnt = 22;
				break;
			}
		case 1:				//YYYYMMDD24
			{
				cnt = 19;
				break;
			}
		case 2:				//MMDDYYYY12
			{
				cnt = 22;
				break;
			}
		case 3:				//MMDDYYYY24
			{
				cnt = 19;
				break;
			}
	}
	*pWidth = m_tTimeOsd.FontWidth * cnt;
	*pHeight = m_tTimeOsd.FontHeight;

	return 0;
}

//##ModelId=49A263BF0226
int COsd::SetType(int nShowType)
{
	int result;
    int i;
	
	char key_str[30];
	OSD_DBG("\n......COsd::SetType.....begin.....nShowType=%d....\n",nShowType);
	if(1 == SetOsdDirect)
    {
        ;
    }   
    else
    {
    	snprintf(key_str, sizeof(key_str), "OsdShowType%02d", m_iChannel);
    	pCConfig->SetValue(OsdCfgSection, key_str, nShowType);

    }
	m_iOldShowType = m_iShowType;
	m_iShowType = nShowType;
	
	// 同步数据到OSDV2，防止新旧接口不一致
	m_tTextOsdV2[0].bEnable = (nShowType & 1);
    m_tTextOsdV2[1].bEnable = ((nShowType >> 2)& 1);
    if(1 == SetOsdDirect)
    {
        ;
    }   
    else
    {
        for(i = 0; i < OSDV2_TEXT_CNT; i++)
        {
            snprintf(key_str, sizeof(key_str), "OsdV2Text%02dEn%02d", i, m_iChannel);
	        pCConfig->SetValue(OsdCfgSection, key_str, m_tTextOsdV2[i].bEnable);
        }
    }

//防止重复调用
	if (m_iShowType != m_iOldShowType)
		{
			result = ZWSys_EncSetOsd(m_hEnc, (nShowType >> 2) & 1, (nShowType >> 1) & 1, nShowType & 1);
			OSD_DBG("\n..int COsd::SetType(int nShowType)...ZWSys_EncSetOsd..(nShowType >> 1) & 1=%d...nShowType & 1=%d...\n", (nShowType >> 1) & 1, nShowType & 1);
			if (result)
			{
				OSD_ERR("[COsd:%s]ZWSys_EncSetOsd failed (%d)\n", __FUNCTION__, result);
				return result;
			}

			m_iOldShowType = m_iShowType;
		}

	return (int) 0;
}

//##ModelId=49B7675A0290
int COsd::GetPosition(int OSD_TYPE, int *x, int *y)
{
	OSD_DBG("\n......COsd::GetPosition.....begin.........\n");

	if (NULL == x || NULL == y)
	{
		OSD_ERR("[COsd:%s]x==NULL||y==NULL\n", __FUNCTION__);
		return -1;
	}

	if (OSD_TYPE == 0)
	{
		*x = m_iTimeNetCfgX;
		*y = m_iTimeNetCfgY;
	}
	else
	{
		*x = m_iTextNetCfgX;
		*y = m_iTextNetCfgY;
	}

	return (int) 0;
}

//##ModelId=49B7686E0188
int COsd::SetPosition(int OSD_TYPE, int x, int y)
{
	char key_str[100] = {0};
	char value[100] = {0};
	int tmp_x = 0, tmp_y = 0;
	int result = 0;
    int x_bak = 0, y_bak=0;

	OSD_DBG("\n......COsd::SetPosition.....begin.........\n");

    x_bak = x;
    y_bak = y;
	x &= ~(7);					//8对齐
	y &= ~(3);					//4对齐

	if (x < 0 || y < 0 || x >= 704 || y >= 576)
	{
		OSD_ERR("[%s] range invalid < x = %d ,y = %d>\n", __FUNCTION__, x, y);
		return -1;
	}

	if (OSD_TYPE == 0)
	{
        m_iTimeNetCfgX = x_bak;
        m_iTimeNetCfgY = y_bak;
        
		tmp_x = m_iTimeX;
		tmp_y = m_iTimeY;

		m_iTimeX = x >> 1;
		m_iTimeY = y >> 1;

		if (CheckRect())
		{
			m_iTimeX = tmp_x;
			m_iTimeY = tmp_y;
			OSD_ERR("[COsd:%s] time/text Rect overlap!\n", __FUNCTION__);
			return -1;
		}

		m_tTimeOsd.Start_X = m_iTimeX;
		m_tTimeOsd.Start_Y = m_iTimeY;


		result = SetOsd(NULL, &m_tTimeOsd,NULL,NULL,NULL,0);
		if (result)
		{
			OSD_ERR("[COsd:%s]ZWSys_EncSetOsdTime failed (%d)\n", __FUNCTION__, result);
			return result;
		}
        if(1 == SetOsdDirect)
        {
            ;
        }
        else
        {
    		snprintf(key_str, sizeof(key_str), "OsdTimePos%02d", m_iChannel);
    		snprintf(value, sizeof(value), "%d,%d", x, y);
    		pCConfig->SetValue(OsdCfgSection, key_str, value);            
        }
	}
	else
	{
		tmp_x = m_iTextX;
		tmp_y = m_iTextY;

		m_iTextX = x >> 1;
		m_iTextY = y >> 1;

		if (CheckRect())
		{
			m_iTextX = tmp_x;
			m_iTextY = tmp_y;
			OSD_ERR("[COsd:%s] time/text Rect overlap!\n", __FUNCTION__);

			return -1;
		}

		SetCharPixel(NULL);

        if(1 == SetOsdDirect)
        {
            ;
        }
        else
        {
    		snprintf(key_str, sizeof(key_str), "OsdTextPos%02d", m_iChannel);
    		snprintf(value, sizeof(value), "%d,%d", x, y);
    		pCConfig->SetValue(OsdCfgSection, key_str, value);
        }

		//同步数据到OSDV2
		m_tTextOsdV2[0].nX = x;
		m_tTextOsdV2[0].nY = y;
        if(1 == SetOsdDirect)
        {
            ;
        }
        else
        {
    		snprintf(key_str, sizeof(key_str), "OsdV2Text%02dPos%02d", 0, m_iChannel);
    		snprintf(value, sizeof(value), "%d,%d", m_tTextOsdV2[0].nX, m_tTextOsdV2[0].nY);
    		pCConfig->SetValue(OsdCfgSection, key_str, value);
        }
	}

	return (int) 0;
}

//!\brief 设置时间或者字符的位置
//!\param int OSD_TYPE OSD类型 1-文本,2-时间,3-时间+文本
//!\param int Time_x, int Time_y, int Text_x, int Text_y时间与文件的位置
//!\return  成功为0，失败为-1

int COsd::SetPositionAll(int Time_x, int Time_y, int Text_x, int Text_y, int OSD_TYPE)
{
	char key_str[100];
	char value[100];
	
	OSD_DBG("\n.......COsd::SetPositionAll.........begin..........\n");
	if (OSD_TYPE < 1 || OSD_TYPE > 3)
	{
		OSD_ERR("[%s] OSD_TYPE = %d\n", __FUNCTION__, OSD_TYPE);
		return -1;
	}

	if ((OSD_TYPE & 2) && (Time_x < 0 || Time_y < 0 || Time_x >= 704 || Time_y >= 576))
	{
		OSD_ERR("[%s] range invalid time< x = %d ,y = %d>\n", __FUNCTION__, Time_x, Time_y);
		return -1;
	}
	if ((OSD_TYPE & 1) && (Text_x < 0 || Text_y < 0 || Text_x >= 704 || Text_y >= 576))
	{
		OSD_ERR("[%s] range invalid text< x = %d ,y = %d>\n", __FUNCTION__, Text_x, Text_y);
		return -1;
	}

	int tmp_x, tmp_y, tmp_x1, tmp_y1;
	tmp_x = m_iTimeX;
	tmp_y = m_iTimeY;

	tmp_x1 = m_iTextX;
	tmp_y1 = m_iTextY;

	if (OSD_TYPE & 2)
	{							//时间
		m_iTimeX = Time_x >> 1;
		m_iTimeY = Time_y >> 1;
        m_iTimeNetCfgX = Time_x;
        m_iTimeNetCfgY = Time_y;
	}
	if (OSD_TYPE & 1)
	{							//文本
		m_iTextX = Text_x >> 1;
		m_iTextY = Text_y >> 1;
        m_iTextNetCfgX = Text_x;
        m_iTextNetCfgY = Text_y;
	}

	if (CheckRect())
	{
		m_iTimeX = tmp_x;
		m_iTimeY = tmp_y;

		m_iTextX = tmp_x1;
		m_iTextY = tmp_y1;
		OSD_ERR("[COsd:%s] time/text Rect overlap!\n", __FUNCTION__);
		return -1;
	}

	m_tTimeOsd.Start_X = m_iTimeX;
	m_tTimeOsd.Start_Y = m_iTimeY;

	SetCharPixel(NULL);

    if(1 == SetOsdDirect)
    {
        ;
    }
    else
    {
    	snprintf(key_str, sizeof(key_str), "OsdTimePos%02d", m_iChannel);
    	snprintf(value, sizeof(value), "%d,%d", Time_x, Time_y);
    	pCConfig->SetValue(OsdCfgSection, key_str, value);

    	snprintf(key_str, sizeof(key_str), "OsdTextPos%02d", m_iChannel);
    	snprintf(value, sizeof(value), "%d,%d", m_iTextX, m_iTextY);
    	pCConfig->SetValue(OsdCfgSection, key_str, value);
    }

	//同步数据到OSDV2
	m_tTextOsdV2[0].nX = Text_x;
	m_tTextOsdV2[0].nY = Text_y;
    if(1 == SetOsdDirect)
    {
        ;
    }
    else
    {    
    	snprintf(key_str, sizeof(key_str), "OsdV2Text%02dPos%02d", 0, m_iChannel);
    	snprintf(value, sizeof(value), "%d,%d", m_tTextOsdV2[0].nX, m_tTextOsdV2[0].nY);
    	pCConfig->SetValue(OsdCfgSection, key_str, value);
    }

	return 0;
}

//##ModelId=49B767A50356
int COsd::GetTimeFormat(int *nShowFortMat)
{
	if (NULL == nShowFortMat)
	{
		OSD_ERR("[COsd:%s]nShowFortMat==NULL\n", __FUNCTION__);
		return -1;
	}

	*nShowFortMat = m_iTimeType;
	return (int) 0;
}

//##ModelId=49B768920389
int COsd::SetTimeFormat(int nShowFortMat)
{
	char key_str[100];

	int result;
	OSD_DBG("\n............int COsd::SetTimeFormat(int nShowFortMat)......................\n");
	if (nShowFortMat < 0 || nShowFortMat > 5)
	{
		OSD_ERR("[COsd:%s]nShowFortMat=%d[0-3]\n", __FUNCTION__, nShowFortMat);
		return -1;
	}
    
    if(1 == SetOsdDirect)
    {
        ;
    }
    else
    {
    	snprintf(key_str, sizeof(key_str), "OsdTimeFormat%02d", m_iChannel);
    	pCConfig->SetValue(OsdCfgSection, key_str, nShowFortMat);
    }
	m_iTimeType = nShowFortMat;
	
	OSD_DBG("\n.....COsd::SetTimeFormat()........//设置时间..............nShowFortMat=%d.......\n",nShowFortMat);
	switch (nShowFortMat)
	{
		case 0:				//YYYYMMDD12
			{
				m_tTimeOsd.bHour24 = 0;
				m_tTimeOsd.DisMod = Mode_YYYY_MM_DD;
				break;
			}
		case 1:				//YYYYMMDD24
			{
				m_tTimeOsd.bHour24 = 1;
				m_tTimeOsd.DisMod = Mode_YYYY_MM_DD;
				break;
			}
		case 2:				//MMDDYYYY12
			{
				m_tTimeOsd.bHour24 = 0;
				m_tTimeOsd.DisMod = Mode_MM_DD_YYYY;
				break;
			}
		case 3:				//MMDDYYYY24
			{
				m_tTimeOsd.bHour24 = 1;
				m_tTimeOsd.DisMod = Mode_MM_DD_YYYY;
				break;
			}
		case 4:				//MMDDYYYY24
			{
				m_tTimeOsd.bHour24 = 0;
				m_tTimeOsd.DisMod = Mode_DD_MM_YYYY;
				
				break;
			}
		case 5:				//MMDDYYYY24
			{
				m_tTimeOsd.bHour24 = 1;
				m_tTimeOsd.DisMod = Mode_DD_MM_YYYY;
				
				break;
			}
	}
	OSD_DBG("\n..COsd::SetTimeFormat...m_tTimeOsd.bHour24=%d............m_tTimeOsd.DisMod=%d...........\n",m_tTimeOsd.bHour24,m_tTimeOsd.DisMod);
	m_tTimeOsd.bTranslant = 0;
	m_tTimeOsd.FontWidth = DEFAULT_TIME_PIXEL.w;
	m_tTimeOsd.FontHeight = DEFAULT_TIME_PIXEL.h;
	m_tTimeOsd.pFontPixinfo = (const char *) DEFAULT_TIME_PIXEL.pixel_buf;
	//OSD_Time_swap(pixel_time_buf,DEFAULT_TIME_PIXEL.pixel_buf);
	//m_tTimeOsd.pFontPixinfo = (char *) pixel_time_buf;
	m_tTimeOsd.Start_X = m_iTimeX;
	m_tTimeOsd.Start_Y = m_iTimeY;

	result = SetOsd(NULL, &m_tTimeOsd,NULL,NULL,NULL,0);

	if (result)
	{
		OSD_ERR("[COsd:%s]ZWSys_EncSetOsdTime failed (%d)\n", __FUNCTION__, result);
		return result;
	}

	return (int) 0;
}

//##ModelId=49B767F9028E
int COsd::GetCharBitMap(char *pCharBitMap, int buffSize)
{
	FILE *fp;
	struct stat statbuf;
	int result;
	char fileName[255];
	BITMAPFILEHEADER hdr;
	BITMAPINFOHEADER hbi;

	snprintf(fileName, sizeof(fileName), "%sosd_CH%02d.bmp", FileNameOsdPixel, m_iChannel + 1);
	OSD_DBG("\n......COsd::GetCharBitMap(.......fileName=%s......\n",fileName);
	if (pCharBitMap == NULL)
	{
		OSD_ERR("[COsd:%s]pCharBitMap == NULL\n", __FUNCTION__);
		return -1;
	}
	if ((int) (strlen(fileName) + 1) > buffSize)
	{
		OSD_ERR("[COsd:%s]buffer size too small\n", __FUNCTION__);
		return -1;
	}
	fp = fopen(fileName, "rb");
	if (fp == NULL)
	{
		OSD_ERR("[COsd:%s]open file (%s)failed\n", __FUNCTION__, fileName);
		return -1;
	}

	result = fread(&hdr, 1, sizeof (BITMAPFILEHEADER), fp);
	if (result != sizeof (BITMAPFILEHEADER))
	{
		fclose(fp);
		OSD_ERR("[COsd:%s]read bmp header failed->ch=%d\n", __FUNCTION__, m_iChannel);
		return -1;
	}

	if (SWAP16(hdr.bfType) != 0x4d42)
	{
		fclose(fp);
		OSD_ERR("[COsd:%s]bmp invalid (0x%x)->ch=%d\n", __FUNCTION__, SWAP16(hdr.bfType), m_iChannel);
		return -1;
	}
#if 0
	if (pCharBitMap == NULL)
	{
		fclose(fp);
		return SWAP32(hdr.bfSize);
	}

	if (SWAP32(hdr.bfSize) > (unsigned int) buffSize)
	{
		fclose(fp);
		OSD_ERR("[COsd:%s]buff size too small->ch=%d\n", __FUNCTION__, m_iChannel);
		return -1;
	}
#else
	lstat(fileName, &statbuf);
	if (SWAP32(hdr.bfSize) > (unsigned int) statbuf.st_size)
	//if (SWAP16(hdr.bfSize) > (unsigned int) statbuf.st_size)
	{
		fclose(fp);
		OSD_ERR("[COsd:%s]file size too short->ch=%d\n", __FUNCTION__, m_iChannel);
		return -1;
	}
#endif
	result = fread(&hbi, 1, sizeof (BITMAPINFOHEADER), fp);
	if (result != sizeof (BITMAPINFOHEADER))
	{
		fclose(fp);
		OSD_ERR("[COsd:%s]read bmp info failed->ch=%d\n", __FUNCTION__, m_iChannel);
		return -1;
	}

	if (24 != SWAP16(hbi.biBitCount))
	{
		fclose(fp);
		OSD_ERR("[COsd:%s]not unsupported bmp (bits = %d)type->ch=%d\n", __FUNCTION__, SWAP16(hbi.biBitCount), m_iChannel);
		return -1;
	}
#if 0
	//开始填缓冲
	fseek(fp, 0, SEEK_SET);
	if (SWAP32(hdr.bfSize) != fread(pCharBitMap, 1, SWAP32(hdr.bfSize), fp))
	{
		fclose(fp);
		OSD_ERR("[COsd:%s]read bmp data failed ->ch=%d\n", __FUNCTION__, m_iChannel);
		return -1;

	}
#else
	strncpy(pCharBitMap, fileName,  buffSize - 1);
#endif
	m_iTextW = SWAP32(hbi.biWidth);
	m_iTextH = SWAP32(hbi.biHeight);
	fclose(fp);
	return 0;
}

int COsd::SetCharPixel(char *pCharBitMap)
{
	int charnum;
	int i;
	int result = 0; 
	int charW=0, charH=0;
	char *pnewbuf = 0;

	char szKey[30] = {0};
	char szSection[30] = {0};
	char szDefaultName[30] = {0};
	unsigned int strName[DVR_MAX_CHANNELNAME_LEN] = {0};
	unsigned short strName2[DVR_MAX_CHANNELNAME_LEN] = {0};
	snprintf(szSection, sizeof(szSection), SECTION_OSD_ALIAS, m_iChannel);
	snprintf(szDefaultName, sizeof(szDefaultName), DEFNAME_OSD_ALIAS, m_iChannel+ 1);
	unsigned short buf[256] = {0};
    int maxLineWidth = 0;//最大宽度
    int textOsd1Length = 0; //OSD1纯文本内容长度

	OSD_DBG("m_iTextX:%d, m_iTextY:%d\n", m_iTextY, m_iTextY);

	memset(&strName, 0, sizeof(strName));
	char value[256] = {0};

    if(1 == SetOsdDirect)
    {
    	sscanf(strvalue, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
    		&strName[0],&strName[1],&strName[2],&strName[3],
    		&strName[4],&strName[5],&strName[6],&strName[7],
    		&strName[8],&strName[9],&strName[10],&strName[11],
    		&strName[12],&strName[13],&strName[14],&strName[15],
    		&strName[16],&strName[17],&strName[18],&strName[19]);
    }
    else
    {
    	pCConfig->GetValue(szSection, "Unicode", "73:80:67:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0", value, 256);

    	sscanf(value, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
    		&strName[0],&strName[1],&strName[2],&strName[3],
    		&strName[4],&strName[5],&strName[6],&strName[7],
    		&strName[8],&strName[9],&strName[10],&strName[11],
    		&strName[12],&strName[13],&strName[14],&strName[15],
    		&strName[16],&strName[17],&strName[18],&strName[19]);        
    }

	for(i=0;i<(DVR_MAX_CHANNELNAME_LEN-1);i++)
    {
		strName2[i] = strName[i];
    }

	// 处理从配置读出的OSD内容，只保留两行
	// fix bug(13355):
	// 1、(NVR)使用老接口设置OSD，在里面插入多个换行符，使用新接口的网页配置界面显示跟OSD不一致；
	// 2、原逻辑只过滤了超过两行的换行符，未过滤超过两行的内容；
	OSDV1TextParsing(strName2, &maxLineWidth);

    for(i = 0; i < (DVR_MAX_CHANNELNAME_LEN)-1; i++)
    {
        if(!strName2[i])
        {
            break;
        }
        textOsd1Length++;
    }
	//printf("old:%d-%d, new:%d-%d notCheckParam:%d\n", m_tTextOsd.Start_X, m_tTextOsd.Start_Y, m_iTextX, m_iTextY,notCheckParam);
	//是否改变,初始化时也是调的该函数，所以m_strName不能先从配置获取，否则始终是相等的
	if((1 == notCheckParam) || (0 != memcmp(strName2, m_strName, (DVR_MAX_CHANNELNAME_LEN-1)*sizeof(unsigned short))) || (m_tTextOsd.Start_X != m_iTextX) || (m_tTextOsd.Start_Y != m_iTextY))
	{
		m_tTextOsd.Start_X = m_iTextX;
		m_tTextOsd.Start_Y = m_iTextY;
		m_tTextOsdEx.Start_X = m_iTextX;
		m_tTextOsdEx.Start_Y = m_iTextY;

		m_iWhichValid = 0;
        if(1 == SetOsdDirect)
        {
            ;
        }
        else
        {
		    pCConfig->SetValue(szSection, "WhichValid", m_iWhichValid);
        }

		for(i=0;i<20;i++)
		{
			m_strName[i] =strName2[i];
			m_strNameEx[i] =strName2[i];
		}
		for(i=20;i<OSD_TEXT_EX_MAX_EXTENT_OSD_TEXT_LENGTH;i++)
		{
			m_strNameEx[i] = 0;
		}
		
		//更新OSDV2的配置文件
		//将老接口数据同步到新接口 OSDV2，防止NVR使用老接口导致显示与网页配置不一致
		m_tTextOsdV2[0].nX = (m_tTextOsdEx.Start_X<<1);
		m_tTextOsdV2[0].nY = (m_tTextOsdEx.Start_Y<<1);
        if(1 == SetOsdDirect)
        {
            ;
        }
        else
        {        
            //时间OSD位置
            snprintf(szKey, sizeof(szKey), "OsdTimePos%02d", m_iChannel);
    		snprintf(value, sizeof(value), "%d,%d", m_iTimeNetCfgX, m_iTimeNetCfgY);
    		pCConfig->SetValue(OsdCfgSection, szKey, value);

            //textOSD1位置        
    		snprintf(szKey, sizeof(szKey), "OsdV2Text%02dPos%02d", 0, m_iChannel);
    		snprintf(value, sizeof(value), "%d,%d", m_tTextOsdV2[0].nX, m_tTextOsdV2[0].nY);
    		pCConfig->SetValue(OsdCfgSection, szKey, value);

            //textOSD2位置     
            snprintf(szKey, sizeof(szKey), "OsdV2Text%02dPos%02d", 1, m_iChannel);
    		snprintf(value, sizeof(value), "%d,%d", m_tTextOsdV2[1].nX, m_tTextOsdV2[1].nY);
    		pCConfig->SetValue(OsdCfgSection, szKey, value);
        }
		//保存OSD内容
		memset(m_tTextOsdV2[0].strText, 0, OSDV2_TEXT_LENGTH*sizeof(unsigned short));
		memcpy(m_tTextOsdV2[0].strText, m_strNameEx, sizeof(m_strNameEx));
		int lCnt = OSDV2_TEXT_CNT_SAVE_ONE_LINE;
		unsigned short *OSDV2Text1 = m_tTextOsdV2[0].strText;
        unsigned short *OSDV2Text2 = m_tTextOsdV2[1].strText;

        if(1 == SetOsdDirect)
        {
            ;
        }
        else
        {         
    		for (i = 0; i < OSDV2_TEXT_LENGTH/OSDV2_TEXT_CNT_SAVE_ONE_LINE; i++)
    		{
    			snprintf(szKey, sizeof(szKey), "OsdV2Text%02dUnicode%02d_%02d", 0, i, m_iChannel);
    			snprintf(value, sizeof(value), "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
    				OSDV2Text1[i*lCnt+0],OSDV2Text1[i*lCnt+1],OSDV2Text1[i*lCnt+2],OSDV2Text1[i*lCnt+3],OSDV2Text1[i*lCnt+4],OSDV2Text1[i*lCnt+5],OSDV2Text1[i*lCnt+6],OSDV2Text1[i*lCnt+7],
    				OSDV2Text1[i*lCnt+8],OSDV2Text1[i*lCnt+9],OSDV2Text1[i*lCnt+10],OSDV2Text1[i*lCnt+11],OSDV2Text1[i*lCnt+12],OSDV2Text1[i*lCnt+13],OSDV2Text1[i*lCnt+14],OSDV2Text1[i*lCnt+15],
    				OSDV2Text1[i*lCnt+16],OSDV2Text1[i*lCnt+17],OSDV2Text1[i*lCnt+18],OSDV2Text1[i*lCnt+19],OSDV2Text1[i*lCnt+20],OSDV2Text1[i*lCnt+21],OSDV2Text1[i*lCnt+22],OSDV2Text1[i*lCnt+23],
    				OSDV2Text1[i*lCnt+24],OSDV2Text1[i*lCnt+25],OSDV2Text1[i*lCnt+26],OSDV2Text1[i*lCnt+27],OSDV2Text1[i*lCnt+28],OSDV2Text1[i*lCnt+29],OSDV2Text1[i*lCnt+30],OSDV2Text1[i*lCnt+31]
    			);
    			pCConfig->SetValue(OsdCfgSection, szKey, value);

                /**
                 * 使用IE设置OSD和海康协议设置OSD后简单恢复的差异：
                 * 使用IE设置完OSD后再简单恢复：
                 *     直接从.OSD中读取文本内容和坐标调用sdk的接口，不将数据保存到配置文件中（在手动设置之前不会再修改.OSD文件）
                 * 使用海康协议设置OSD后简单恢复：
                 *      先从.OSD中读取文本内容和坐标，将读取的数据保存到配置文件中，接着更新.OSD，因此要求每个OSD的内容都不能遗漏，有遗漏下次重启或者简单恢复就会出现问题
                 * 因此为了避免简单恢复后，textOSD2的内容因没有备份导致textOSD2被清空，在这里将坐标和文本内容设置到配置文件中
                **/
                snprintf(szKey, sizeof(szKey), "OsdV2Text%02dUnicode%02d_%02d", 1, i, m_iChannel);
    			snprintf(value, sizeof(value), "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
    				OSDV2Text2[i*lCnt+0],OSDV2Text2[i*lCnt+1],OSDV2Text2[i*lCnt+2],OSDV2Text2[i*lCnt+3],OSDV2Text2[i*lCnt+4],OSDV2Text2[i*lCnt+5],OSDV2Text2[i*lCnt+6],OSDV2Text2[i*lCnt+7],
    				OSDV2Text2[i*lCnt+8],OSDV2Text2[i*lCnt+9],OSDV2Text2[i*lCnt+10],OSDV2Text2[i*lCnt+11],OSDV2Text2[i*lCnt+12],OSDV2Text2[i*lCnt+13],OSDV2Text2[i*lCnt+14],OSDV2Text2[i*lCnt+15],
    				OSDV2Text2[i*lCnt+16],OSDV2Text2[i*lCnt+17],OSDV2Text2[i*lCnt+18],OSDV2Text2[i*lCnt+19],OSDV2Text2[i*lCnt+20],OSDV2Text2[i*lCnt+21],OSDV2Text2[i*lCnt+22],OSDV2Text2[i*lCnt+23],
    				OSDV2Text2[i*lCnt+24],OSDV2Text2[i*lCnt+25],OSDV2Text2[i*lCnt+26],OSDV2Text2[i*lCnt+27],OSDV2Text2[i*lCnt+28],OSDV2Text2[i*lCnt+29],OSDV2Text2[i*lCnt+30],OSDV2Text2[i*lCnt+31]
    			);
    			pCConfig->SetValue(OsdCfgSection, szKey, value);
    		}
        }

		for(i=0;i<DVR_MAX_CHANNELNAME_LEN;i++)
		{
			if(0 == m_strName[i])
				break;
			else
				if(0 != (m_strName[i] & 0xff00))
					charW += 16;
				else
		#ifdef ASC_WIDTH_16
					charW += 16;
		#else
				#ifdef SUPPORT_TEXT_OSD_TWO_LINE
					if(NEW_LINE_CHAR != m_strName[i])
				#endif
					{
						charW += 8;
					}

		#endif
		}
		charH = 16;
		charnum = (charW * charH) >> 3;
		OSD_DBG("SetCharPixel charW=%d charH=%d charnum=%d\n", charW, charH, charnum);

		pnewbuf = new char[charnum + 8];
		if (NULL == pnewbuf)
		{
			OSD_ERR("[COsd:%s]'new'failed\n", __FUNCTION__);
			return -1;
		}
		((short *) pnewbuf)[0] = charW;
		((short *) pnewbuf)[1] = charH;
        ((short *) pnewbuf)[2] = maxLineWidth;
        ((short *) pnewbuf)[3] = charH * m_tTextOsd.lineNum;        

		memset(buf, 0, sizeof(buf));
		UniToStr(m_strName, (uchar *)buf);

		if(findStrInHZK16(buf, pnewbuf+8))
		{
			OSD_ERR("[COsd:%s] failed(%d)\n", __FUNCTION__, result);
			return result;
		}
		m_tTextOsd.pPixinfo = pnewbuf;
		memcpy(m_tTextOsd.strOSD, m_strName, sizeof(m_strName));
		memset(m_tTextOsd.strGB2312, 0, sizeof(unsigned short)*21);
		memcpy(m_tTextOsd.strGB2312, buf, sizeof(unsigned short)*20);

        //由于sdk支持多行OSD的接口，在这里将TextOSDInfo_t数据转成TextOSDV2Info_t
        TextOSDToTextOSDV2(0);

		result = SetOsd(NULL, &m_tTimeOsd, NULL, NULL, &m_tTextOsdV2Info[0], 0);
		if (result)
        {
            OSD_ERR("[COsd:%s]SetOsd failed(%d)\n", __FUNCTION__, result);
            delete []pnewbuf;

            return result;
        }

        //只有当OSD的配置发生改变时才去保存一次配置
//        SaveOSDBak();
	}
	else
	{
		result = SetOsd(NULL, &m_tTimeOsd, NULL, NULL, NULL, 0);
        if (result)
        {
            OSD_ERR("[COsd:%s]SetOsd failed(%d)\n", __FUNCTION__, result);
            delete []pnewbuf;

            return result;
        }
	}

	notCheckParam = 0;

	delete []pnewbuf;

	return (int) 0;
}

//##ModelId=49B768A80178
int COsd::SetCharBitMap(char *pCharBitMap, unsigned char *byLattice ,int FontSize, int Width, int Height)
{
	int result = 0;
	struct stat statbuf;
	char *pdata = 0;
	FILE *fp = 0;

	OSD_DBG("\n............int COsd::SetCharBitMap(char *pCharBitMap)..........begin............\n");
	if (NULL == pCharBitMap)
	{
		OSD_ERR("[COsd:%s]pCharBitMap==NULL\n", __FUNCTION__);
		return -1;
	}
	if (strlen(pCharBitMap) > 255)
	{
		OSD_ERR("[COsd:%s]invalid file path\n", __FUNCTION__);
		return -1;
	}
	result = lstat(pCharBitMap, &statbuf);
	if (result)
	{
		OSD_ERR("[COsd:%s]lstat<%s> failed(%d)\n", __FUNCTION__, pCharBitMap, result);
		return -1;
	}
	if (statbuf.st_size <= 0 || statbuf.st_size > MAX_OSD_FILE_SIZE)
	{
		OSD_ERR("[COsd:%s]size = %lld\n", __FUNCTION__, statbuf.st_size);
		return -1;
	}
	pdata = new char[statbuf.st_size];

	if (NULL == pdata)
	{
		OSD_ERR("[COsd:%s]new[%lld]failed\n", __FUNCTION__, statbuf.st_size);
		return -1;
	}
	fp = fopen(pCharBitMap, "r");
	if (NULL == fp)
	{
		OSD_ERR("[COsd:%s] fopen<%s>failed\n", __FUNCTION__, pCharBitMap);
		delete []pdata;

		return -1;
	}

	result = fread(pdata, 1, statbuf.st_size, fp);
	if (result != statbuf.st_size)
	{
		OSD_ERR("[COsd:%s]fread failed(%d)\n", __FUNCTION__, result);
		fclose(fp);
		if (pdata)
			delete []pdata;
		pdata = 0;

		return -1;
	}
	fclose(fp);

	if (WriteTextPixeltoFile(pdata) < 0)
	{
		OSD_ERR("[COsd:%s] write bmp file failed\n", __FUNCTION__);
		delete []pdata;
		return -1;
	}

	OSD_INFO("......SetCharBitMap.......pdata=%s.......\n",pdata);
	result = SetCharPixel(pdata);
	if (pdata)
		delete []pdata;
	pdata = 0;

	return result;
}

int COsd::setTextOsd()
{
    int charnum;
	int i;
	int result = 0; 
	int charW=0, charH=0;
	char *pnewbuf = 0;
	char szSection[30] = {0};
	unsigned short buf[OSD_TEXT_EX_MAX_EXTENT_OSD_TEXT_LENGTH] = {0};
    
	snprintf(szSection, sizeof(szSection), SECTION_OSD_ALIAS, m_iChannel);

	OSD_DBG("m_iTextX:%d, m_iTextY:%d\n", m_iTextY, m_iTextY);

	//统计字符的总宽度
	for(i=0; i<OSD_TEXT_EX_MAX_EXTENT_OSD_TEXT_LENGTH; i++)
	{
		if(0 == m_strNameEx[i])
		{
			break;
		}
		else
			if(0 != (m_strNameEx[i] & 0xff00))
			{
				charW += 16;
			}
			else
#ifdef ASC_WIDTH_16
				{
					charW += 16;
				}
#else
	#ifdef SUPPORT_TEXT_OSD_TWO_LINE
				if(NEW_LINE_CHAR != m_strNameEx[i])
	#endif
				{
					charW += 8;
				}
  #if 0    
    #ifdef SUPPORT_TEXT_OSD_TWO_LINE
                else
                {   //由于内存原因:对3516EV200的F23/307/2232只允许设置一行Osd
                    if((0x206E == nBoardID) || (0x206F == nBoardID) || (0x2073 == nBoardID))
                    {
                        m_strNameEx[i] = '\0';
                        break;
                    }
                }
    #endif
  #endif
#endif
	}
	charH = 16;
	charnum = (charW * charH) >> 3;
	//TRACE_AVL("\n[==COsd::SetCharPixel==] charW =%d charH =%d charnum=%d\n",charW,charH,charnum);
  
	pnewbuf = new char[charnum + 4];
	if (NULL == pnewbuf)
	{
		OSD_ERR("[COsd:%s]'new'failed\n", __FUNCTION__);
		return -1;
	}
	
	//填写宽高
	((short *) pnewbuf)[0] = charW;
	((short *) pnewbuf)[1] = charH;

	//转换为国标编码
	UniToStr(m_strNameEx, (uchar *)buf);

	//获取点阵
	if(findStrInHZK16(buf, pnewbuf+4))
	{
		OSD_ERR("[COsd:%s] failed(%d)\n", __FUNCTION__, result);
		return result;
	}
	
	m_tTextOsdEx.pPixinfo = pnewbuf;
	memset(m_tTextOsdEx.strOSD, 0, sizeof(unsigned short)*256);
	memcpy(m_tTextOsdEx.strOSD, m_strNameEx, sizeof(m_strNameEx));
	memset(m_tTextOsdEx.strGB2312, 0, sizeof(unsigned short)*256);
	memcpy(m_tTextOsdEx.strGB2312, buf, sizeof(unsigned short)*256);

	m_iWhichValid = 1;
    if(1 == SetOsdDirect)
    {
    	;
    }
    else
    {
	    pCConfig->SetValue(szSection, "WhichValid", m_iWhichValid);
    }
	
	result = SetOsd(NULL, &m_tTimeOsd, NULL, &m_tTextOsdEx, NULL,0);
	OSD_DBG("SetOsd return %d\n", result);

	if (result)
	{
		OSD_ERR("[COsd:%s]SetOsd failed(%d)\n", __FUNCTION__, result);
		delete []pnewbuf;

		return result;
	}
	delete []pnewbuf;

	return 0;
}

//!\brief   将Edvr.cfg配置文件里面的osd内容及哪套配置标记进行备份（未备份坐标）
//!\param   
//!\return  成功为0，失败为-1
int COsd::SaveOSDBak()
{
    if(1 == SetOsdDirect)
    {
        return 0;
    }

	FILE *fp = NULL;
	char value[256] = {0};
	char szSection[30] = {0};
	char szKey[30] = {0};
	int  i = 0;

	fp = fopen(OSD_BAK_SAVE, "wb+");

	snprintf(szSection, sizeof(szSection), SECTION_OSD_ALIAS, m_iChannel);

	if(1 == m_iWhichValid)
	{
		if (fp == NULL)
		{
			OSD_ERR("[COsd:%s]open file (%s)failed\n", __FUNCTION__, OSD_BAK_SAVE);
			return -1;
		}
		else
		{
			for(i=0; i<(OSD_TEXT_EX_MAX_EXTENT_OSD_TEXT_LENGTH / OSD_SAVE_COUNT_ONE_LINE); i++)//保存里面的240个
			{
				if(0 == i)
				{
					snprintf(szKey, sizeof(szKey), "Unicode");
				}
				else
				{
					snprintf(szKey, sizeof(szKey), "Unicode%d", i);
				}
				
				memset(value, 0, sizeof(value));
				pCConfig->GetValue(szSection, szKey, "73:80:67:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0", value, sizeof (value));
				fwrite(value, 1, strlen(value), fp);
				fprintf(fp, "\n");
			}
		}
	}
	else if (0 ==  m_iWhichValid || 2 == m_iWhichValid)
	{
		if (fp == NULL)
		{
			OSD_ERR("[COsd:%s]open file (%s)failed\n", __FUNCTION__, OSD_BAK_SAVE);
			return -1;
		}
		else
		{
			for(i=0; i<(OSDV2_TEXT_LENGTH/OSDV2_TEXT_CNT_SAVE_ONE_LINE); i++)
			{
				snprintf(szKey, sizeof(szKey), "OsdV2Text%02dUnicode%02d_%02d", 0, i, 0);

				memset(value, 0, sizeof(value));
				pCConfig->GetValue(OsdCfgSection, szKey, "73:80:67:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0", value, sizeof (value));
				fwrite(value, 1, strlen(value), fp);
				fprintf(fp, "\n");
			}

			for(i=0; i<(OSDV2_TEXT_LENGTH/OSDV2_TEXT_CNT_SAVE_ONE_LINE); i++)
			{
				//通道名OSD2
				snprintf(szKey, sizeof(szKey), "OsdV2Text%02dUnicode%02d_%02d", 1, i, 0);
				
				memset(value, 0, sizeof(value));
				pCConfig->GetValue(OsdCfgSection, szKey, "0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0", value, sizeof(value));
				fwrite(value, 1, strlen(value), fp);
				fprintf(fp, "\n");
			}

			//默认不支持名称OSD,简单恢复后保留名称OSD使能及内容设置
			//所以使能也需要单独保存
			{
				char OsdCfgEx[512] = {0};
			
				//名称OSD使能
				snprintf(szKey, sizeof(szKey), "OsdV2Text%02dEn%02d", 1, 0);
				memset(value, 0, sizeof(value));
				pCConfig->GetValue(OsdCfgSection, szKey, "0", value, sizeof(value));
				snprintf(OsdCfgEx, sizeof(OsdCfgEx), "%s:", value);

				//时间OSD位置
				snprintf(szKey, sizeof(szKey), "OsdTimePos%02d", 0);
				memset(value, 0, sizeof(value));
				pCConfig->GetValue(OsdCfgSection, szKey, "100,0", value, 100);
				strncat(OsdCfgEx, value, sizeof(OsdCfgEx) - strlen(OsdCfgEx) - 1);
				strncat(OsdCfgEx, ":", sizeof(OsdCfgEx) - strlen(OsdCfgEx) - 1);
				
				//通道名OSD位置
				snprintf(szKey, sizeof(szKey), "OsdV2Text%02dPos%02d", 0, 0);
				memset(value, 0, sizeof(value));
				pCConfig->GetValue(OsdCfgSection, szKey, "0,0", value, 100);
				strncat(OsdCfgEx, value, sizeof(OsdCfgEx) - strlen(OsdCfgEx) - 1);
				strncat(OsdCfgEx, ":", sizeof(OsdCfgEx) - strlen(OsdCfgEx) - 1);

				//名称OSD位置
				snprintf(szKey, sizeof(szKey), "OsdV2Text%02dPos%02d", 1, 0);
				memset(value, 0, sizeof(value));
				pCConfig->GetValue(OsdCfgSection, szKey, "0,0", value, 100);
				strncat(OsdCfgEx, value, sizeof(OsdCfgEx) - strlen(OsdCfgEx) - 1);

				fwrite(OsdCfgEx, 1, strlen(OsdCfgEx), fp);
				fprintf(fp, "\n");
			}
		}
	}
	else
	{
		snprintf(szKey, sizeof(szKey), "Unicode");
		pCConfig->GetValue(szSection, szKey, "73:80:67:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0", value, sizeof (value));
		fwrite(value, 1, strlen(value), fp);
	}

	if(fp)
		fclose(fp);
	fp = 0;
	
	memset(value, 0, sizeof(value));
	strncpy(value, "rm /mnt/mtd/config/.OSDCfg:* ", sizeof(value) - 1);
	mysystem(value);
	
	memset(value, 0, sizeof(value));
	snprintf(value, sizeof(value), "touch /mnt/mtd/config/.OSDCfg:%d", m_iWhichValid);
	mysystem(value);	

	return 0;
}

//!\brief   检查备份的跟变量里的是否一致，
//!\param   
//!\return  成功为0，失败为-1
int COsd::RevertOSDBak()
{
	FILE *fp = NULL;
	char value[256] = {0};
	char szSection[30] = {0};
	char szKey[30] = {0};
	char buf[8] = {0};
	int i,j;
	int temp[20] = {0};

	snprintf(szSection, sizeof(szSection), SECTION_OSD_ALIAS, m_iChannel);

	if ((access(OSD_BAK_REVERT0, F_OK)==0) 
		|| (access(OSD_BAK_REVERT1, F_OK)==0) 
		|| (access(OSD_BAK_REVERT2, F_OK)==0))
	{
		strncpy(value, "ls /mnt/mtd/config/.OSDCfg* | awk -F \":\" '{printf $2}'", sizeof(value) - 1);
		ShellCmdOut(value, buf, sizeof(buf) - 1);
		unsigned int bakValid = 0;
		sscanf(buf, "%d", &bakValid);
		if((0 == bakValid) || (1 == bakValid) || (2 == bakValid))
		{
			if(m_iWhichValid != bakValid)
			{
				OSD_ERR("Need reset m_iWhichValid: from %d to %d\n", m_iWhichValid, bakValid);
				m_iWhichValid = bakValid;
				pCConfig->SetValue(szSection, "WhichValid", m_iWhichValid);
			}
		}
		else
		{
			memset(value, 0, sizeof(value));
			strncpy(value, "rm /mnt/mtd/config/.OSDCfg:* ", sizeof(value) - 1);
			mysystem(value);
		}
	}
	else
	{
		memset(value, 0, sizeof(value));
		strncpy(value, "rm /mnt/mtd/config/.OSDCfg:* ", sizeof(value) - 1);
		mysystem(value);
		
		memset(value, 0, sizeof(value));
		snprintf(value, sizeof(value), "touch /mnt/mtd/config/.OSDCfg:%d", m_iWhichValid);
		mysystem(value);	
	}

	//OSD校验
	if(0 == access(OSD_BAK_SAVE, F_OK))
	{
		fp = fopen(OSD_BAK_SAVE, "r");
		if (fp == NULL)
		{
			OSD_ERR("[%s]open file (%s)failed\n", __FUNCTION__, OSD_BAK_SAVE);
			goto GetOSDFromCFG;
		}
		else
		{
			i=0;
			if(1 == m_iWhichValid)
			{
				memset(value, 0, sizeof(value));
				while(fgets(value, sizeof(value), fp))
				{
					if(i >= (OSD_TEXT_EX_MAX_EXTENT_OSD_TEXT_LENGTH / OSD_SAVE_COUNT_ONE_LINE))
						break;
					sscanf(value, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
						&temp[0],&temp[1], &temp[2], &temp[3], &temp[4],
						&temp[5],&temp[6], &temp[7], &temp[8], &temp[9],
						&temp[10],&temp[11],&temp[12],&temp[13],&temp[14],
						&temp[15],&temp[16],&temp[17],&temp[18],&temp[19]);

					if(0 == i)
					{
						snprintf(szKey, sizeof(szKey), "Unicode");
					}
					else
					{
						snprintf(szKey, sizeof(szKey), "Unicode%d", i);
					}
					value[strlen(value) - 1] = '\0';//去掉最后的换行符
					pCConfig->SetValue(szSection, szKey, value);

					for(j=0;j<OSD_SAVE_COUNT_ONE_LINE;j++)
					{
						m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+j] = temp[j];
					}
					if((0 == i) && (1 == m_iWhichValid))
					{
						for(j=0;j<OSD_SAVE_COUNT_ONE_LINE;j++)
						{
							m_strName[j] = temp[j];
						}
					}
					i++;
					memset(value, 0, sizeof(value));
					
				}
			}
			else if (0 == m_iWhichValid || 2 == m_iWhichValid)
			{
				unsigned short *strText = m_tTextOsdV2[0].strText;
				unsigned short *strName = m_tTextOsdV2[1].strText;
				OSDV2Text_t *pNameOsd = &m_tTextOsdV2[1];
				OSDV2Text_t *pTextOsd = &m_tTextOsdV2[0];
				int *pShowType = &m_iShowType;
				j = 0;
				memset(value, 0, sizeof(value));
				while(fgets(value, sizeof(value), fp))
				{
					if(i < (OSDV2_TEXT_LENGTH/OSDV2_TEXT_CNT_SAVE_ONE_LINE))
					{
					sscanf(value, "%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu",
						&strText[i*32+0],&strText[i*32+1],&strText[i*32+2],&strText[i*32+3],&strText[i*32+4],&strText[i*32+5],&strText[i*32+6],&strText[i*32+7],
						&strText[i*32+8],&strText[i*32+9],&strText[i*32+10],&strText[i*32+11],&strText[i*32+12],&strText[i*32+13],&strText[i*32+14],&strText[i*32+15],
						&strText[i*32+16],&strText[i*32+17],&strText[i*32+18],&strText[i*32+19],&strText[i*32+20],&strText[i*32+21],&strText[i*32+22],&strText[i*32+23],
						&strText[i*32+24],&strText[i*32+25],&strText[i*32+26],&strText[i*32+27],&strText[i*32+28],&strText[i*32+29],&strText[i*32+30],&strText[i*32+31]);
					// 接口数据同步在解析OSD时进行，防止将关键字同步到其他接口
					i++;
					memset(value, 0, sizeof(value));
					}
					else if(i < (2 * OSDV2_TEXT_LENGTH/OSDV2_TEXT_CNT_SAVE_ONE_LINE))
					{
						sscanf(value, "%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu",
						     &strName[j*32+0],&strName[j*32+1],&strName[j*32+2],&strName[j*32+3],&strName[j*32+4],&strName[j*32+5],&strName[j*32+6],&strName[j*32+7],
						     &strName[j*32+8],&strName[j*32+9],&strName[j*32+10],&strName[j*32+11],&strName[j*32+12],&strName[j*32+13],&strName[j*32+14],&strName[j*32+15],
						     &strName[j*32+16],&strName[j*32+17],&strName[j*32+18],&strName[j*32+19],&strName[j*32+20],&strName[j*32+21],&strName[j*32+22],&strName[j*32+23],
						     &strName[j*32+24],&strName[j*32+25],&strName[j*32+26],&strName[j*32+27],&strName[j*32+28],&strName[j*32+29],&strName[j*32+30],&strName[j*32+31]);
						i++;
						j++;
						memset(value, 0, sizeof(value));
					}
					else if(i < (2 * OSDV2_TEXT_LENGTH/OSDV2_TEXT_CNT_SAVE_ONE_LINE) + 1)
					{
						sscanf(value, "%d:%d,%d:%d,%d:%d,%d", &pNameOsd->bEnable, &m_iTimeX, &m_iTimeY, 
							&pTextOsd->nX, &pTextOsd->nY, &pNameOsd->nX, &pNameOsd->nY);
						m_iTimeNetCfgX = m_iTimeX;
						m_iTimeNetCfgY = m_iTimeY;							
						m_iTimeX >>= 1;
						m_iTimeY >>= 1;
						m_iTimeX &= ~(3); //CIF，4对齐
						m_iTimeY &= ~(1);
						m_tTimeOsd.Start_X = m_iTimeX;
						m_tTimeOsd.Start_Y = m_iTimeY;

                        m_iTextX = pTextOsd->nX;
                        m_iTextY = pTextOsd->nY;
                        m_iTextX >>= 1;
                        m_iTextY >>= 1;
                        m_iTextX &= ~(3); //CIF，4对齐
                        m_iTextY &= ~(1);
                        m_tTextOsd.Start_X = m_iTextX;
                        m_tTextOsd.Start_Y = m_iTextY;                        
						i++;
						memset(value, 0, sizeof(value));
						if(pNameOsd->bEnable)
						{
							*pShowType |= (1 << 2);
						}
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				if((fgets(value, sizeof(value), fp)))
				{
					sscanf(value, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
						&temp[0],&temp[1], &temp[2], &temp[3], &temp[4],
						&temp[5],&temp[6], &temp[7], &temp[8], &temp[9],
						&temp[10],&temp[11],&temp[12],&temp[13],&temp[14],
						&temp[15],&temp[16],&temp[17],&temp[18],&temp[19]);
					
					pCConfig->SetValue(szSection, "Unicode", value);
					for(j=0;j<OSD_SAVE_COUNT_ONE_LINE;j++)
					{
						m_strNameEx[j] = temp[j];
						m_strName[j] = temp[j];
					}

				}
			}
			fclose(fp);
		}
	}
	else
	{
		goto GetOSDFromCFG;
	}

	return 0;
	
	GetOSDFromCFG:
	if (2 == m_iWhichValid)
	{
		// 导入OSDV2的配置
		for(i = 0; i < OSDV2_TEXT_CNT; i++)
		{
			// 使能
			snprintf(szKey, sizeof(szKey), "OsdV2Text%02dEn%02d", i, m_iChannel);
			pCConfig->GetValue(OsdCfgSection, szKey, "0", value, 100);
			sscanf(value, "%d", &m_tTextOsdV2[i].bEnable);
			
			// 坐标
			snprintf(szKey, sizeof(szKey), "OsdV2Text%02dPos%02d", i, m_iChannel);
			pCConfig->GetValue(OsdCfgSection, szKey, "0,0", value, 100);
			sscanf(value, "%d,%d", &m_tTextOsdV2[i].nX, &m_tTextOsdV2[i].nY);
			
			// 读取TOKEN
			char *szToken = m_tTextOsdV2[i].szToken;
			snprintf(szKey, sizeof(szKey), "OsdV2Text%02dToken%02d", i, m_iChannel);
			pCConfig->GetValue(OsdCfgSection, szKey, "", value, sizeof(value));
			sscanf(value, "%s", szToken);
			
			// 读取OSD的内容
			for(j = 0; j < OSDV2_TEXT_LENGTH/OSDV2_TEXT_CNT_SAVE_ONE_LINE; j++)
			{
				unsigned short *strText = m_tTextOsdV2[i].strText;
				snprintf(szKey, sizeof(szKey), "OsdV2Text%02dUnicode%02d_%02d", i, j, m_iChannel);
				pCConfig->GetValue(OsdCfgSection, szKey, "0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0", value, sizeof(value));
				sscanf(value, "%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu",
					&strText[j*32+0],&strText[j*32+1],&strText[j*32+2],&strText[j*32+3],&strText[j*32+4],&strText[j*32+5],&strText[j*32+6],&strText[j*32+7],
					&strText[j*32+8],&strText[j*32+9],&strText[j*32+10],&strText[j*32+11],&strText[j*32+12],&strText[j*32+13],&strText[j*32+14],&strText[j*32+15],
					&strText[j*32+16],&strText[j*32+17],&strText[j*32+18],&strText[j*32+19],&strText[j*32+20],&strText[j*32+21],&strText[j*32+22],&strText[j*32+23],
					&strText[j*32+24],&strText[j*32+25],&strText[j*32+26],&strText[j*32+27],&strText[j*32+28],&strText[j*32+29],&strText[j*32+30],&strText[j*32+31]);
			}
		}
	}
	else
	{
		for(i=0; i<(OSD_TEXT_EX_MAX_EXTENT_OSD_TEXT_LENGTH / OSD_SAVE_COUNT_ONE_LINE); i++)
		{
			memset(value, 0, sizeof(value));
			if(0 == i)
			{
				snprintf(szKey, sizeof(szKey), "Unicode");
				pCConfig->GetValue(szSection, szKey, "73:80:67:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0", value, sizeof (value));
			}
			else
			{
				snprintf(szKey, sizeof(szKey), "Unicode%d", i);
				pCConfig->GetValue(szSection, szKey, "0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0", value, sizeof (value));
			}

			sscanf(value, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
				&temp[0],&temp[1], &temp[2], &temp[3], &temp[4],
				&temp[5],&temp[6], &temp[7], &temp[8], &temp[9],
				&temp[10],&temp[11],&temp[12],&temp[13],&temp[14],
				&temp[15],&temp[16],&temp[17],&temp[18],&temp[19]);

			for(j=0;j<OSD_SAVE_COUNT_ONE_LINE;j++)
			{
				m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+j] = temp[j];
			}
			
			if((0 == i) && (1 == m_iWhichValid))
			{
				for(j=0;j<OSD_SAVE_COUNT_ONE_LINE;j++)
				{
					m_strName[j] = temp[j];
				}
			}
			//保存参数
		}
	}

	return 0;
}

//!\brief   设置扩展OSD，根据Token来定义OSD的功能
//!\param   OSDToken                OSD的功能，如osd_token_1表示对Text的扩展
//!\param   strText           		OSD内容，一般是unicode编码，有CharSet决定编码格式
//!\param   CharSet                 strText的编码格式
//!\return  成功为0，失败为-1
int COsd::OSD_SetTextEx(char *OSDToken, unsigned short *strText, int CharSet)
{
	unsigned short strName[OSD_TEXT_EX_MAX_EXTENT_OSD_TEXT_LENGTH] = {0};
	char szSection[30] = {0};
	char szKey[30] = {0};
	char value[256] = {0};
	unsigned int offsetX = 0;
	unsigned int offsetY = 0;
	int ret = 0;
	int i;
	
	//该token是对text的扩展
	if(0 == strncmp(OSDToken, OSD_TEXT_EX_TOKEN, OSD_TEXT_EX_MAX_EXTENT_OSD_TOKEN_LENGTH))
	{
		snprintf(szSection, sizeof(szSection), SECTION_OSD_ALIAS, m_iChannel);

		//对内容进行解析
		ret = OsdParsing(OSDToken, strText, CharSet, strName, &offsetX, &offsetY);
		if(ret)
		{
			OSD_ERR("[COsd:%s] Data format error\n", __FUNCTION__);
			return -1;
		}
		
		if(((unsigned int)m_tTextOsdEx.Start_X == (offsetX >> 1)) && ((unsigned int)m_tTextOsdEx.Start_Y == (offsetY >> 1)) &&
			(0 == memcmp(m_strNameEx, strName, sizeof(m_strNameEx))))
		{
			OSD_INFO("[%s:%d]Param same!\n", __FUNCTION__, __LINE__);
			return 0;
		}

        if(1 == SetOsdDirect)
        {
            ;
        }
        else
        {
    		//保存坐标配置
    		snprintf(szKey, sizeof(szKey), "OsdTextPos%02d", m_iChannel);
    		snprintf(value, sizeof(value), "%d,%d", offsetX, offsetY);
    		pCConfig->SetValue(OsdCfgSection, szKey, value);
        }

		m_iTextX = offsetX;
		m_iTextY = offsetY;

		offsetX >>= 1;//除2后是给SDK用的，以前就是这样做的
		offsetY >>= 1;
		memcpy(m_strNameRaw, strText, sizeof(m_strNameRaw));
		memcpy(m_strNameEx, strName, sizeof(m_strNameEx));
		m_tTextOsdEx.Start_X = offsetX;
		m_tTextOsdEx.Start_Y = offsetY;
		m_tTextOsd.Start_X = offsetX;
		m_tTextOsd.Start_Y = offsetY;
		
		OSD_INFO("######################### New TEXT OSD #########################\n");

		ret = setTextOsd();
		if(ret)
		{
			OSD_ERR("[COsd:%s] Set error\n", __FUNCTION__);
			return -1;
		}
        
		for(i=0; i<(OSD_TEXT_EX_MAX_EXTENT_OSD_TEXT_LENGTH / OSD_SAVE_COUNT_ONE_LINE); i++)//保存里面的240个
		{
			if(0 == i)
			{
				snprintf(szKey, sizeof(szKey), "Unicode");
                if(1 == SetOsdDirect)
                {
    			    snprintf(strvalue, sizeof(strvalue), "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
    				    m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+1],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+2],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+3],
    				    m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+4],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+5],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+6],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+7],
    				    m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+8],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+9],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+10],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+11],
    				    m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+12],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+13],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+14],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+15],
    				    m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+16],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+17],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+18],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+19]);                    
                }
            }
			else
			{
				snprintf(szKey, sizeof(szKey), "Unicode%d", i);
			}
			memset(value, 0, sizeof(value));
			snprintf(value, sizeof(value), "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
				m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+1],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+2],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+3],
				m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+4],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+5],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+6],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+7],
				m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+8],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+9],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+10],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+11],
				m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+12],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+13],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+14],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+15],
				m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+16],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+17],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+18],m_strNameEx[i*OSD_SAVE_COUNT_ONE_LINE+19]);
			OSD_INFO("value=%s\n", value);
            
            if(1 == SetOsdDirect)
            { 
                ;
            }
            else
            {
    			//保存参数
    			pCConfig->SetValue(szSection, szKey, value);
            }
		}
        
		#if 1
		//将内容更新到老Text osd的变量里
		for(i=0; i<(DVR_MAX_CHANNELNAME_LEN - 1); i++)
			m_strName[i] = m_strNameEx[i];
		#endif
		//将老接口数据同步到新接口 OSDV2，防止NVR使用老接口导致显示与网页配置不一致
		m_tTextOsdV2[0].nX = (m_tTextOsdEx.Start_X<<1);
		m_tTextOsdV2[0].nY = (m_tTextOsdEx.Start_Y<<1);

        if(1 == SetOsdDirect)
        {
        	;
        }
        else
        {        
    		snprintf(szKey, sizeof(szKey), "OsdV2Text%02dPos%02d", 0, m_iChannel);
    		snprintf(value, sizeof(value), "%d,%d", m_tTextOsdV2[0].nX, m_tTextOsdV2[0].nY);
    		pCConfig->SetValue(OsdCfgSection, szKey, value);
        }
		//保存OSD内容
		memset(m_tTextOsdV2[0].strText, 0, OSDV2_TEXT_LENGTH*sizeof(unsigned short));
		memcpy(m_tTextOsdV2[0].strText, m_strNameEx, sizeof(m_strNameEx));
		int lCnt = OSDV2_TEXT_CNT_SAVE_ONE_LINE;
		unsigned short *OSDV2Text = m_tTextOsdV2[0].strText;

        if(1 == SetOsdDirect)
        {
        	;
        }
        else
        {
    		for (i = 0; i < OSDV2_TEXT_LENGTH/OSDV2_TEXT_CNT_SAVE_ONE_LINE; i++)
    		{
    			
    			snprintf(szKey, sizeof(szKey), "OsdV2Text%02dUnicode%02d_%02d", 0, i, m_iChannel);
    			snprintf(value, sizeof(value), "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
    				OSDV2Text[i*lCnt+0],OSDV2Text[i*lCnt+1],OSDV2Text[i*lCnt+2],OSDV2Text[i*lCnt+3],OSDV2Text[i*lCnt+4],OSDV2Text[i*lCnt+5],OSDV2Text[i*lCnt+6],OSDV2Text[i*lCnt+7],
    				OSDV2Text[i*lCnt+8],OSDV2Text[i*lCnt+9],OSDV2Text[i*lCnt+10],OSDV2Text[i*lCnt+11],OSDV2Text[i*lCnt+12],OSDV2Text[i*lCnt+13],OSDV2Text[i*lCnt+14],OSDV2Text[i*lCnt+15],
    				OSDV2Text[i*lCnt+16],OSDV2Text[i*lCnt+17],OSDV2Text[i*lCnt+18],OSDV2Text[i*lCnt+19],OSDV2Text[i*lCnt+20],OSDV2Text[i*lCnt+21],OSDV2Text[i*lCnt+22],OSDV2Text[i*lCnt+23],
    				OSDV2Text[i*lCnt+24],OSDV2Text[i*lCnt+25],OSDV2Text[i*lCnt+26],OSDV2Text[i*lCnt+27],OSDV2Text[i*lCnt+28],OSDV2Text[i*lCnt+29],OSDV2Text[i*lCnt+30],OSDV2Text[i*lCnt+31]
    			);
    			pCConfig->SetValue(OsdCfgSection, szKey, value);
    		}
        }

//		SaveOSDBak();
	}
	else
		{
			return -1;
		}
	
	return 0;
}

int COsd::OSD_GetOSDV2TextByNum(int i, int *bEnable, char *OSDToken, unsigned short *strText, int *nX, int *nY)
{
    if ((NULL == bEnable) || (NULL == OSDToken) || (NULL == strText) || (NULL == nX) || (NULL == nY))
	{
		OSD_ERR("[COsd:%s] Invalid param\n", __FUNCTION__);
		return -1;
	}

	if (i < 0 || i >= OSDV2_TEXT_CNT)
	{
		OSD_ERR("[COsd:%s] Invalid index\n", __FUNCTION__);
		return -1;
	}
	
	OSDV2Text_t *OSDV2 = &m_tTextOsdV2[i];
	*bEnable = OSDV2->bEnable;
	*nX = OSDV2->nX;
	*nY = OSDV2->nY;
	memcpy(OSDToken, OSDV2->szToken, OSDV2_TEXT_TOKEN_LEN);
	memcpy(strText, OSDV2->strText, OSDV2_TEXT_LENGTH);

	return 0;
}


//!\brief   解析OSDV2的OSD文本内容
//!\param   strText[in]		OSD内容，unicode编码
//!\param   textBuf[out]	去除换行符及附加信息关键字的OSD内容
//!\param   textCnt[out]	OSD纯文本字符的个数
//!\param   totalWidth[out]	OSD纯文本总宽度
//!\param   totalWidth[out]	最大行宽，SDK需要用到
//!\return  成功为0，失败为-1

int COsd::OSDV2TextParsing(unsigned short *strText, unsigned short *textBuf, int *textCnt, int *totalWidth, int *maxLineWidth, int index)
{
	int i = 0;
	int curLineWidth = 0;
	int keywordIdx = 0;
	int aliasIdx = 0;
	unsigned short tmpText[OSDV2_TEXT_LENGTH] = {0};
	int tmpIdx = 0;
    int nameOSDLineNum = 0;

	if((NULL == strText) || (NULL == textBuf) || (NULL == textCnt) || (NULL == totalWidth) || (NULL == maxLineWidth))
	{
		OSD_ERR("[COsd:%s] Invalid param\n", __FUNCTION__);
		return -1;
	}

	*textCnt = 0;
	*totalWidth = 0;
	*maxLineWidth = 0;
	// 清空一些数据
    if(0 == index)
    {
	    memset(m_strName, 0, DVR_MAX_CHANNELNAME_LEN*sizeof(unsigned short));
    }
	memset(m_strNameEx, 0, OSD_TEXT_EX_MAX_EXTENT_OSD_TEXT_LENGTH*sizeof(unsigned short));
	memset(m_tTextOsdV2Info[0].lineInfo, 0, sizeof(OSDV2TextLineInfo_t)*OSDV2_TEXT_MAX_LINE);
	memset(m_tTextOsdV2Info[1].lineInfo, 0, sizeof(OSDV2TextLineInfo_t)*OSDV2_TEXT_MAX_LINE);
	m_tTextOsdV2Info[0].lineNum = 0;
    m_tTextOsdV2Info[1].lineNum = 0;
	/*添加一个过滤 ; 将传过来的OSD参数进行过滤下,不能丢弃后面的东西 */
    {
        int nLineNum = 0;   // 换行符个数   ^
        int nstrLocal = 0;  // 字符总长度
        int nStrCount = 0;   // 用于赛换后，重新填入的记录下标
        int nArrayTab = 0;   // 每一段的大小
        unsigned short nTmp[8][40] = {0};
        while(0 != strText[i] && i < OSDV2_TEXT_MAX_LENGTH)
        {
            if(OSDV2_TEXT_NEW_LINE_CHAR == strText[i]/* && index == 0*/)
            {
                if(nLineNum >= 7)  // 超过8行，就丢弃掉，避免数组越界，同时OSD也不允许这么多行
                {
                    break;
                }
                nLineNum++;
                nArrayTab = 0;
            }
            nTmp[nLineNum][nArrayTab++] = strText[i];
            if(nArrayTab > 39)
            {
                nArrayTab = 0;   // 避免越界 
            }
            nstrLocal++;
            i++;
        }
        /* 合并 --  对 OSD1 和 OSD2 进行过滤 */
        if(nLineNum > 2  && index == 0)  // 当大于2个换行符，说明存在四行
        {
            /* 前三条直接装，再判断最后一条是否装进来 */
            memset(strText, 0, OSDV2_TEXT_LENGTH * sizeof(unsigned short) );
            for(int ii = 0; ii < 3; ii++)
            {
                for(int jj = 0; jj < 40; jj++)
                {
                    if(nTmp[ii][jj] != 0)
                    {
                        strText[nStrCount++] = nTmp[ii][jj];
                    }
                }
            }
            if(nTmp[nLineNum][0] == OSDV2_TEXT_NEW_LINE_CHAR && nTmp[nLineNum][1] == OSDV2_TEXT_KEYWORD_START_CHAR)
            {
                for(int jj = 0; jj < 40; jj++)
                {
                    if(nTmp[nLineNum][jj] != 0)
                    {
                        strText[nStrCount++] = nTmp[nLineNum][jj];   // 叠加信息给到他
                    }
                }
            }
        }
        else if(nLineNum > 1  && index == 1)    // OSD2
        {
            /* 前两条直接装，再判断最后一条是否装进来 */
            memset(strText, 0, OSDV2_TEXT_LENGTH * sizeof(unsigned short) );
            for(int ii = 0; ii < 2; ii++)
            {
                for(int jj = 0; jj < 40; jj++)
                {
                    if(nTmp[ii][jj] != 0)
                    {
                        strText[nStrCount++] = nTmp[ii][jj];
                    }
                }
            }
            /* 两行OSD2装完后，后面的丢弃掉 */
        }
        /* OSD1小于四行的不用管它;OSD2小于3行也不用管他*/
        i = 0;
    }

	while(0 != strText[i] && i < OSDV2_TEXT_MAX_LENGTH)
	{
		if(OSDV2_TEXT_NEW_LINE_CHAR == strText[i] && ((0 == index) || (1 == index && 0 == nameOSDLineNum)))
		{
			// 是空行则跳过，否则记录
			if(curLineWidth > 0)
			{
				if(0 == index)
				{
					m_tTextOsdV2Info[index].lineInfo[m_tTextOsdV2Info[index].lineNum].pixWidth = curLineWidth;
					m_tTextOsdV2Info[index].lineNum++;
				    *maxLineWidth = (*maxLineWidth>curLineWidth)?(*maxLineWidth):curLineWidth;
				    curLineWidth = 0;
				    // 检查行数是不是超限----这里是丢弃尾部的东西，这样会将后面的叠加信息丢掉
					if(m_tTextOsdV2Info[index].lineNum >= OSDV2_TEXT_MAX_LINE)
				    {
					    break;
					}
				}
				else if(1 == index && 0 == nameOSDLineNum)
				{
					m_tTextOsdV2Info[index].lineInfo[m_tTextOsdV2Info[index].lineNum].pixWidth = curLineWidth;
					m_tTextOsdV2Info[index].lineNum++;
					*maxLineWidth = (*maxLineWidth>curLineWidth)?(*maxLineWidth):curLineWidth;
					curLineWidth = 0;
					nameOSDLineNum = 1;
				}
				tmpText[tmpIdx++] = strText[i];
			}
			i++;
			continue;
		}
		if(0 == index && OSDV2_TEXT_KEYWORD_START_CHAR == strText[i])
		{
			// 客流、分辨率&码率叠加互斥，同时开启客流优先。（为了省内存）
			//三种形式：[RES] [BPS]；[BPS]；[RES]；[LGT]
			if (m_iOsdFlowEn)
			{
				if (0 == memcmp(&strText[i], OSDV2OverlayList[2].word, OSDV2OverlayList[2].wordLen))
				{
					i += OSDV2OverlayList[2].wordLen;
					continue;
				}
				else if ((0 == memcmp(&strText[i], OSDV2OverlayList[0].word, OSDV2OverlayList[0].wordLen))
					|| (0 == memcmp(&strText[i], OSDV2OverlayList[1].word, OSDV2OverlayList[1].wordLen)))
				{
					i += OSDV2OverlayList[0].wordLen;
					continue;
				}
			}

			OSDV2TextGetKeyWord(&strText[i], OSDV2_TEXT_MAX_LENGTH-i, &keywordIdx);
			if(keywordIdx >= 0 && (unsigned int)keywordIdx < OSDV2_TEXT_KEYWORD_NUM)// 找到Keyword，没找到则当普通文本处理
			{
				//检查行宽度超过限制或者附加信息个数超限则丢弃keyword信息
				if((((curLineWidth + OSDV2KeywordList[keywordIdx].pixWidth)>>3) <= OSDV2_TEXT_MAX_LENGTH)
					&& (m_tTextOsdV2Info[index].lineInfo[m_tTextOsdV2Info[index].lineNum].keywordNum < OSDV2_TEXT_KEYWORD_MAX_ONE_LINE))
				{
					// 记录附加信息
					OSDV2TextLineInfo_t *lineInfo= &m_tTextOsdV2Info[index].lineInfo[m_tTextOsdV2Info[index].lineNum];
					int lineKeywordIdx = lineInfo->keywordNum;
					OSDV2TextKeyword_t *keyword = &lineInfo->keyWords[lineKeywordIdx];
					memcpy(keyword, &OSDV2KeywordList[keywordIdx], sizeof(OSDV2TextKeyword_t));
					keyword->pos = curLineWidth; // 记录在该行中的位置
					curLineWidth += OSDV2KeywordList[keywordIdx].pixWidth;
					lineInfo->keywordNum++;
					// 记录有效的值
					memcpy(&tmpText[tmpIdx], &strText[i], OSDV2KeywordList[keywordIdx].wordLen * sizeof(unsigned short));
					tmpIdx += OSDV2KeywordList[keywordIdx].wordLen;
				}
				i += OSDV2KeywordList[keywordIdx].wordLen;
				continue;
			}
		}

		// 普通文字
		if(strText[i] & 0xff00)
		{
			if((curLineWidth + 16) <= OSDV2_TEXT_MAX_LINE_WIDTH*8)
			{
				textBuf[(*textCnt)] = strText[i];
				*textCnt += 1;
				curLineWidth += 16;
				*totalWidth += 16;
				tmpText[tmpIdx++] = strText[i];
			}
		}
		else
		{
			if((curLineWidth + 8) <= OSDV2_TEXT_MAX_LINE_WIDTH*8)
			{
				textBuf[(*textCnt)] = strText[i];
				*textCnt += 1;
				curLineWidth += 8;
				*totalWidth += 8;
				tmpText[tmpIdx++] = strText[i];
			}
		}
		
		// 将第一行的纯文本保存到别名中，如果为空则默认为IPC
		if((0 == m_tTextOsdV2Info[index].lineNum) && (aliasIdx < (DVR_MAX_CHANNELNAME_LEN-1)))
		{
			m_strNameEx[aliasIdx] = strText[i];
			aliasIdx++;
		}
		i++;
	}
	textBuf[(*textCnt)] = 0;
    if (OSDV2_TEXT_NEW_LINE_CHAR == textBuf[*textCnt-1])
	{
		textBuf[*textCnt-1] = 0;
	}

	m_strNameEx[aliasIdx] = 0;
	tmpText[tmpIdx] = 0;
	if (OSDV2_TEXT_NEW_LINE_CHAR == tmpText[tmpIdx-1])
	{
		tmpText[tmpIdx-1] = 0;
	}
	
	if(0 == index && (m_tTextOsdV2Info[index].lineNum < OSDV2_TEXT_MAX_LINE) && (curLineWidth > 0))
	{
		m_tTextOsdV2Info[index].lineInfo[m_tTextOsdV2Info[index].lineNum].pixWidth = curLineWidth;
		m_tTextOsdV2Info[index].lineNum++;
		*maxLineWidth = (*maxLineWidth>curLineWidth)?(*maxLineWidth):curLineWidth;
	}
    else if(1 == index && (m_tTextOsdV2Info[index].lineNum < 2) && (curLineWidth > 0))
    {
        m_tTextOsdV2Info[index].lineInfo[m_tTextOsdV2Info[index].lineNum].pixWidth = curLineWidth;
        m_tTextOsdV2Info[index].lineNum++;
        *maxLineWidth = (*maxLineWidth>curLineWidth)?(*maxLineWidth):curLineWidth;
    }
	// 更新本地存储
	memcpy(strText, tmpText, OSDV2_TEXT_LENGTH);
    if(0 == index)
    {
        //目前海康NVR文本OSD最大只支持20个字符
        memcpy(m_strName, tmpText, (DVR_MAX_CHANNELNAME_LEN-1) * sizeof(unsigned short));
    }

	return 0;
}

int COsd::SetTextOSDV2(int index)
{
	int ret = 0;
	unsigned short unicodeBuf[OSDV2_TEXT_MAX_LENGTH] = {0};
	unsigned short gbBuf[OSDV2_TEXT_MAX_LENGTH] = {0};
	// 该版本中只设置一个OSD，默认使用索引为0的OSD
	OSDV2Text_t *OSDV2 = &m_tTextOsdV2[index];

	// 更新传输给SDK的数据
	m_tTextOsdV2Info[index].Start_X = (OSDV2->nX)>>1;
	m_tTextOsdV2Info[index].Start_Y = (OSDV2->nY)>>1;
	//兼容老接口
    if(0 == index)
    {
		m_iTextX = (OSDV2->nX)>>1;
		m_iTextY = (OSDV2->nY)>>1;
		// 规避保存马赛克是调用SetPicCfg函数，导致调用了老的OSD接口setCharPixel,使m_whichValid变为0
		m_tTextOsd.Start_X = m_iTextX;
		m_tTextOsd.Start_Y = m_iTextY;
		m_tTextOsdEx.Start_X = m_iTextX;
		m_tTextOsdEx.Start_Y = m_iTextY;
    }
	// 解析，设置OSD
	int textCnt = 0, charWidth = 0, charHeight = 16, maxLineWidth = 0;
	char *pixBuf = NULL;
	OSDV2TextParsing(OSDV2->strText, unicodeBuf, &textCnt, &charWidth, &maxLineWidth, index);

	UniToStr(unicodeBuf, (uchar *)gbBuf);

	pixBuf = new char[8+(charWidth*charHeight>>3)];
	if(NULL == pixBuf)
	{
		OSD_ERR("[COsd:%s] malloc pixBuf failed(%d)\n", __FUNCTION__, ret);
		return ret;
	}
	memset(pixBuf, 0, 8+(charWidth*charHeight>>3));
	((short *) pixBuf)[0] = charWidth;
	((short *) pixBuf)[1] = charHeight;
	((short *) pixBuf)[2] = maxLineWidth; // 点阵宽度
	((short *) pixBuf)[3] = charHeight * m_tTextOsdV2Info[index].lineNum; // 点阵高度

	if(charWidth > 0)
	{
		//获取点阵
		if((ret = findStrInHZK16(gbBuf, pixBuf+8)) != 0)
		{
			OSD_ERR("[COsd:%s] findStrInHZK16 failed(%d)\n", __FUNCTION__, ret);
			delete []pixBuf;
			return ret;
		}
	}
	m_tTextOsdV2Info[index].pPixinfo = pixBuf;

	m_tTextOsdV2Info[index].pKeywordPixInfo = (const char *)DEFAULT_OSDV2_PIXEL.pixel_buf;
	m_tTextOsdV2Info[index].keyWordFontHeight = DEFAULT_OSDV2_PIXEL.h;
	m_tTextOsdV2Info[index].keyWordFontWidth = DEFAULT_OSDV2_PIXEL.w;
	memcpy(m_tTextOsdV2Info[index].text, gbBuf, sizeof(gbBuf));
	
	ret = SetOsd(NULL, &m_tTimeOsd, NULL, NULL, &m_tTextOsdV2Info[index], index);

	OSD_DBG("SetOsd return %d\n", ret);
	if(ret)
	{
		OSD_ERR("[COsd:%s]SetOsd failed(%d)\n", __FUNCTION__, ret);
	}

	if(pixBuf)
	{
		delete []pixBuf;
	}
	
	return ret;
}

int COsd::SetTextOSD_Cprocustom(char *unicde)
{
	unsigned short m_strText[32] = {0};
	unsigned short *strText =m_strText;
	sscanf(unicde, "%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu", 
						&strText[0], &strText[1], &strText[2], &strText[3], &strText[4], &strText[5],
						&strText[6], &strText[7], &strText[8], &strText[9], &strText[10], &strText[11],
						&strText[12], &strText[13], &strText[14], &strText[15], &strText[16], &strText[17],
						&strText[18], &strText[19],&strText[20],&strText[21],&strText[22],&strText[23],&strText[24],
						&strText[25],&strText[26],&strText[27],&strText[28],&strText[29],&strText[30],&strText[31]);

	char szSection[30] = {0};
	char szKey[30] = {0};
	char value[256] = {0};
	int i = 0, ret = 0;
	int nY,nX;
	int index = 0;
	int bEnable = 1;
	if (index < 0 || index > OSDV2_TEXT_CNT-1)
	{
		OSD_ERR("[COsd:%s] Invalid index\n", __FUNCTION__);
		return -1;
	}
	
	if(NULL == strText)
	{
		OSD_ERR("[COsd:%s] Invalid param\n", __FUNCTION__);
		return -1;
	}
	
    //坐标处理
    nX = 703;
    nY = 572;

	OSDV2Text_t *OSDV2 = &m_tTextOsdV2[index];
	
	if (OSDV2->bEnable == bEnable && OSDV2->nX == nX && OSDV2->nY == nY
		&& !memcmp(OSDV2->strText, strText, sizeof(m_strText))
		&& (2 == m_iWhichValid))
	{
		OSD_INFO("[%s:%d]Param same!\n", __FUNCTION__, __LINE__);
		if(1 < index)
		{
			return 0;
		}
	}
    
    if (0 == index)
    {
	    m_iTextNetCfgX = nX;
	    m_iTextNetCfgY = nY;
    }
	// 使能
	OSDV2->bEnable = bEnable;
    if(1 == SetOsdDirect)
    {
    	;
    }
    else 
    {
    	snprintf(szKey, sizeof(szKey), "OsdV2Text%02dEn%02d", index, m_iChannel);
    	snprintf(value, sizeof(value), "%d", OSDV2->bEnable);
    	pCConfig->SetValue(OsdCfgSection, szKey, value);
    }
	// 坐标
	OSDV2->nX = nX;
	OSDV2->nY = nY;
    if(1 == SetOsdDirect)
    {
    	;
    }
    else
    {
    	snprintf(szKey, sizeof(szKey), "OsdV2Text%02dPos%02d", index, m_iChannel);
    	snprintf(value, sizeof(value), "%d,%d", OSDV2->nX, OSDV2->nY);
    	pCConfig->SetValue(OsdCfgSection, szKey, value);
    }
	// OSD
	memcpy(OSDV2->strText, strText, sizeof(m_strText));
	
	int lCnt = OSDV2_TEXT_CNT_SAVE_ONE_LINE;
    if(1 == SetOsdDirect)
    {
    	;
    }
    else  
    {    
    	for (i = 0; i < OSDV2_TEXT_LENGTH/OSDV2_TEXT_CNT_SAVE_ONE_LINE; i++)
    	{
    		
    		snprintf(szKey, sizeof(szKey), "OsdV2Text%02dUnicode%02d_%02d", index, i, m_iChannel);
    		snprintf(value, sizeof(value), "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
    			strText[i*lCnt+0],strText[i*lCnt+1],strText[i*lCnt+2],strText[i*lCnt+3],strText[i*lCnt+4],strText[i*lCnt+5],strText[i*lCnt+6],strText[i*lCnt+7],
    			strText[i*lCnt+8],strText[i*lCnt+9],strText[i*lCnt+10],strText[i*lCnt+11],strText[i*lCnt+12],strText[i*lCnt+13],strText[i*lCnt+14],strText[i*lCnt+15],
    			strText[i*lCnt+16],strText[i*lCnt+17],strText[i*lCnt+18],strText[i*lCnt+19],strText[i*lCnt+20],strText[i*lCnt+21],strText[i*lCnt+22],strText[i*lCnt+23],
    			strText[i*lCnt+24],strText[i*lCnt+25],strText[i*lCnt+26],strText[i*lCnt+27],strText[i*lCnt+28],strText[i*lCnt+29],strText[i*lCnt+30],strText[i*lCnt+31]
    		);
    		pCConfig->SetValue(OsdCfgSection, szKey, value);
    	}
    }
	
	m_iWhichValid = 2;
	
	// 暂时只支持一个OSD设置到SDK
	if (0 == index || 1 == index)
	{
		ret = SetTextOSDV2(index);
        if(0 == index)
        {
	        if(1 == SetOsdDirect)
	        {
	    		snprintf(strvalue, sizeof(strvalue), "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
	    			strText[0],strText[1],strText[2],strText[3],strText[4],strText[5],strText[6],strText[7],strText[8],strText[9],
	    			strText[10],strText[11],strText[12],strText[13],strText[14],strText[15],strText[16],strText[17],strText[18],strText[19]);
	        }
	        else 
	        {
	    		// 保存配置
	    		snprintf(szSection, sizeof(szSection), SECTION_OSD_ALIAS, m_iChannel);
	    		pCConfig->SetValue(szSection, "WhichValid", m_iWhichValid);
	    		//保存坐标配置
	    		snprintf(szKey, sizeof(szKey), "OsdTextPos%02d", m_iChannel);
	    		snprintf(value, sizeof(value), "%d,%d", m_iTextX, m_iTextY);
	    		pCConfig->SetValue(OsdCfgSection, szKey, value);
	    		// 保存通道别名
	    		snprintf(szKey, sizeof(szKey), "Unicode");
	    		memset(value, 0, sizeof(value));
	    		snprintf(value, sizeof(value), "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
	    			strText[0],strText[1],strText[2],strText[3],strText[4],strText[5],strText[6],strText[7],strText[8],strText[9],
	    			strText[10],strText[11],strText[12],strText[13],strText[14],strText[15],strText[16],strText[17],strText[18],strText[19]);
	    		pCConfig->SetValue(szSection, szKey, value);
			}
        }
		SaveOSDBak();
	}

	return ret;	
}


int COsd::OSD_SetOSDV2(int index, int bEnable, char *OSDToken, unsigned short *strText, int nX, int nY, int PassFlowEna)
{
	char szSection[30] = {0};
	char szKey[30] = {0};
	char value[256] = {0};
	int i = 0, ret = 0;

	if (index < 0 || index > OSDV2_TEXT_CNT-1)
	{
		OSD_ERR("[COsd:%s] Invalid index\n", __FUNCTION__);
		return -1;
	}
	
	if((NULL == OSDToken) || (NULL == strText))
	{
		OSD_ERR("[COsd:%s] Invalid param\n", __FUNCTION__);
		return -1;
	}
	
    //坐标处理
    nX = nX > 703 ? 703 : nX;
    nX = nX < 0 ? 0 : nX;
    nY = nY > 572 ? 572 : nY;
    nY = nY < 0 ? 0 : nY;

	OSDV2Text_t *OSDV2 = &m_tTextOsdV2[index];
	
	if (OSDV2->bEnable == bEnable && OSDV2->nX == nX && OSDV2->nY == nY 
		&& !memcmp(OSDV2->szToken, OSDToken, OSDV2_TEXT_TOKEN_LEN)
		&& !memcmp(OSDV2->strText, strText, OSDV2_TEXT_LENGTH)
		&& (2 == m_iWhichValid) && (m_iOsdFlowEn == PassFlowEna))
	{
		OSD_INFO("[%s:%d]Param same!\n", __FUNCTION__, __LINE__);
		if(1 < index)
		{
			return 0;
		}
	}
    
    if(0 == index)
    {
    m_iTextNetCfgX = nX;
    m_iTextNetCfgY = nY;
    }
	// 客流及叠加使能
	m_iOsdFlowEn = PassFlowEna;

	// 使能
	OSDV2->bEnable = bEnable;
    if(1 == SetOsdDirect)
    {
    	;
    }
    else 
    {
    	snprintf(szKey, sizeof(szKey), "OsdV2Text%02dEn%02d", index, m_iChannel);
    	snprintf(value, sizeof(value), "%d", OSDV2->bEnable);
    	pCConfig->SetValue(OsdCfgSection, szKey, value);
    }
	// 坐标
	OSDV2->nX = nX;
	OSDV2->nY = nY;
    if(1 == SetOsdDirect)
    {
    	;
    }
    else
    {
    	snprintf(szKey, sizeof(szKey), "OsdV2Text%02dPos%02d", index, m_iChannel);
    	snprintf(value, sizeof(value), "%d,%d", OSDV2->nX, OSDV2->nY);
    	pCConfig->SetValue(OsdCfgSection, szKey, value);
    }
	// TOKEN
	memcpy(OSDV2->szToken, OSDToken, OSDV2_TEXT_TOKEN_LEN);
    if(1 == SetOsdDirect)
    {
    	;
    }
    else  
    {
    	snprintf(szKey, sizeof(szKey), "OsdV2Text%02dToken%02d", index, m_iChannel);
    	snprintf(value, sizeof(value), "%s", OSDV2->szToken);
    	pCConfig->SetValue(OsdCfgSection, szKey, value);
    }
	// OSD
	memcpy(OSDV2->strText, strText, sizeof(short) * OSDV2_TEXT_LENGTH);
	
	int lCnt = OSDV2_TEXT_CNT_SAVE_ONE_LINE;
    if(1 == SetOsdDirect)
    {
    	;
    }
    else  
    {    
    	for (i = 0; i < OSDV2_TEXT_LENGTH/OSDV2_TEXT_CNT_SAVE_ONE_LINE; i++)
    	{
    		
    		snprintf(szKey, sizeof(szKey), "OsdV2Text%02dUnicode%02d_%02d", index, i, m_iChannel);
    		snprintf(value, sizeof(value), "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
    			strText[i*lCnt+0],strText[i*lCnt+1],strText[i*lCnt+2],strText[i*lCnt+3],strText[i*lCnt+4],strText[i*lCnt+5],strText[i*lCnt+6],strText[i*lCnt+7],
    			strText[i*lCnt+8],strText[i*lCnt+9],strText[i*lCnt+10],strText[i*lCnt+11],strText[i*lCnt+12],strText[i*lCnt+13],strText[i*lCnt+14],strText[i*lCnt+15],
    			strText[i*lCnt+16],strText[i*lCnt+17],strText[i*lCnt+18],strText[i*lCnt+19],strText[i*lCnt+20],strText[i*lCnt+21],strText[i*lCnt+22],strText[i*lCnt+23],
    			strText[i*lCnt+24],strText[i*lCnt+25],strText[i*lCnt+26],strText[i*lCnt+27],strText[i*lCnt+28],strText[i*lCnt+29],strText[i*lCnt+30],strText[i*lCnt+31]
    		);
    		pCConfig->SetValue(OsdCfgSection, szKey, value);
    	}
    }
	
	m_iWhichValid = 2;
	
	// 暂时只支持一个OSD设置到SDK
	if (0 == index || 1 == index)
	{
		ret = SetTextOSDV2(index);
        if(0 == index)
        {
	        if(1 == SetOsdDirect)
	        {
	    		snprintf(strvalue, sizeof(strvalue), "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
	    			m_strName[0],m_strName[1],m_strName[2],m_strName[3],m_strName[4],m_strName[5],m_strName[6],m_strName[7],m_strName[8],m_strName[9],
	    			m_strName[10],m_strName[11],m_strName[12],m_strName[13],m_strName[14],m_strName[15],m_strName[16],m_strName[17],m_strName[18],m_strName[19]);
	        }
	        else 
	        {
	    		// 保存配置
	    		snprintf(szSection, sizeof(szSection), SECTION_OSD_ALIAS, m_iChannel);
	    		pCConfig->SetValue(szSection, "WhichValid", m_iWhichValid);
	    		//保存坐标配置
	    		snprintf(szKey, sizeof(szKey), "OsdTextPos%02d", m_iChannel);
	    		snprintf(value, sizeof(value), "%d,%d", m_iTextX, m_iTextY);
	    		pCConfig->SetValue(OsdCfgSection, szKey, value);
	    		// 保存通道别名
	    		snprintf(szKey, sizeof(szKey), "Unicode");
	    		memset(value, 0, sizeof(value));
	    		snprintf(value, sizeof(value), "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
	    			m_strName[0],m_strName[1],m_strName[2],m_strName[3],m_strName[4],m_strName[5],m_strName[6],m_strName[7],m_strName[8],m_strName[9],
	    			m_strName[10],m_strName[11],m_strName[12],m_strName[13],m_strName[14],m_strName[15],m_strName[16],m_strName[17],m_strName[18],m_strName[19]);
	    		pCConfig->SetValue(szSection, szKey, value);
			}
        }
//		SaveOSDBak();
	}
	return ret;
}


//!\brief   获取扩展OSD，根据Token来获取相应功能的OSD
//!\param   OSDToken                OSD的功能，如osd_token_1表示对Text的扩展
//!\param   strText           		OSD内容，一般是unicode编码，有CharSet决定编码格式
//!\param   CharSet                 strText的编码格式
//!\return  成功为0，失败为-1
int COsd::OSD_GetTextExByToken(char *OSDToken, unsigned short *strText, int *CharSet)
{
	int i = 0;
	int point = 0;
	char offset[8] = {0};

	*CharSet = 0;

	//该token是对text的扩展
	if(0 == strncmp(OSDToken, OSD_TEXT_EX_TOKEN, OSD_TEXT_EX_MAX_EXTENT_OSD_TOKEN_LENGTH))
	{
		if(m_strNameEx[0])
		{
			strText[point++] = 't';
			strText[point++] = '=';
		}
		else
			return 0;

		while((m_strNameEx[i]) && (i < OSD_TEXT_EX_MAX_CHAR))
		{
			strText[point++] = m_strNameEx[i++];
		}
		strText[point++] = '$';
		strText[point++] = 'x';
		strText[point++] = '=';
		snprintf(offset, sizeof(offset), "%d", (m_tTextOsdEx.Start_X << 1));
		for(i=0;i<OSD_TEXT_EX_MAX_COORD_LEN;i++)
		{
			if(offset[i])
			{
				strText[point++] = offset[i];
			}
		}
		strText[point++] = '$';
		strText[point++] = 'y';
		strText[point++] = '=';
		snprintf(offset, sizeof(offset), "%d", (m_tTextOsdEx.Start_Y << 1));
		for(i=0;i<OSD_TEXT_EX_MAX_COORD_LEN;i++)
		{
			if(offset[i])
			{
				strText[point++] = offset[i];
			}
		}
	}
	else
		{
			return -1;
		}
	
	return 0;
}


//!\brief   获取扩展OSD，根据编号来获取（0~7），已知Token=osd_token_1跟编号0绑定，编号0即获取的是osd_token_1该Token
//!\param   OSDToken                OSD的功能，如osd_token_1表示对Text的扩展
//!\param   strText           		OSD内容，一般是unicode编码，有CharSet决定编码格式
//!\param   CharSet                 strText的编码格式
//!\return  成功为0，失败为-1
int COsd::OSD_GetTextExByNum(int num, char *OSDToken, int tokenSize, unsigned short *strText, int *CharSet)
{
	int i = 0;
	int point = 0;
	char offset[8] = {0};

	*CharSet = 0;

	if(0 == num)
	{
		strncpy(OSDToken, OSD_TEXT_EX_TOKEN, tokenSize - 1);

		if(m_strNameEx[0])
		{
			strText[point++] = 't';
			strText[point++] = '=';
		}
		else
			return 0;

		while((m_strNameEx[i]) && (i < OSD_TEXT_EX_MAX_CHAR))
		{
			strText[point++] = m_strNameEx[i++];
		}
		strText[point++] = '$';
		strText[point++] = 'x';
		strText[point++] = '=';
		snprintf(offset, sizeof(offset), "%d", (m_tTextOsdEx.Start_X << 1));
		for(i=0;i<OSD_TEXT_EX_MAX_COORD_LEN;i++)
		{
			if(offset[i])
			{
				strText[point++] = offset[i];
			}
		}
		strText[point++] = '$';
		strText[point++] = 'y';
		strText[point++] = '=';
		snprintf(offset, sizeof(offset), "%d", (m_tTextOsdEx.Start_Y << 1));
		for(i=0;i<OSD_TEXT_EX_MAX_COORD_LEN;i++)
		{
			if(offset[i])
			{
				strText[point++] = offset[i];
			}
		}
	}
	else
		{
			return -1;
		}
	
	return 0;
}

int COsd::WriteTextPixeltoFile(char *pbuf)
{
	FILE *fp = 0;
	char fileName[255];
	char cmdbuf[512];
	BITMAPFILEHEADER *hdr;
	BITMAPINFOHEADER *hbi;
	int tmp_w, tmp_h;
	
	snprintf(fileName, sizeof(fileName), "%sosd_CH_bak%02d.bmp", FileNameOsdPixel, m_iChannel + 1);
	OSD_DBG("\n........COsd::WriteTextPixeltoFile(char *pbuf).......begin..........\n"); 
	OSD_DBG("\n........COsd::WriteTextPixeltoFile...fileName=%s......\n",fileName);
	fp = fopen(fileName, "wb+");
	if (fp == NULL)
	{
		OSD_ERR("[COsd:%s]open file (%s)failed\n", __FUNCTION__, fileName);
		return -1;
	}
	hdr = (BITMAPFILEHEADER *) pbuf;
	hbi = (BITMAPINFOHEADER *) (pbuf + sizeof (BITMAPFILEHEADER));

	if (24 != SWAP16(hbi->biBitCount) || SWAP16(hdr->bfType) != 0x4d42)
	{
		if (fp)
			fclose(fp);
		OSD_ERR("[COsd:%s]not unsupported bmp type(bitcount =%d,bftype=%x)->ch=%d\n", __FUNCTION__, SWAP16(hbi->biBitCount), SWAP16(hdr->bfType), m_iChannel);
		return -1;
	}
	tmp_w = m_iTextW;
	tmp_h = m_iTextH;
	m_iTextW = SWAP32(hbi->biWidth);
	m_iTextH = SWAP32(hbi->biHeight);

	if (CheckRect())
	{
		m_iTextW = tmp_w;
		m_iTextH = tmp_h;
		OSD_ERR("[COsd:%s] time/text Rect overlap!\n", __FUNCTION__);

		if (fp)
			fclose(fp);

		return -1;
	}

	//开始写文件
	if (SWAP32(hdr->bfSize) != fwrite(pbuf, 1, SWAP32(hdr->bfSize), fp))
	{
		if (fp)
			fclose(fp);

		OSD_ERR("[COsd:%s]write bmp data failed ->ch=%d\n", __FUNCTION__, m_iChannel);
		return -1;
	}
	if (fp)
		fclose(fp);

	snprintf(cmdbuf, sizeof(cmdbuf), "cp %s %sosd_CH%02d.bmp", fileName, FileNameOsdPixel, m_iChannel + 1);
	mysystem(cmdbuf);

	return 0;
}



int COsd::SetOsd(TextOSDInfo_t *textoi, TimeOSDInfo_t * timeoi,PointOSDInfo_t *point_info, TextOSDInfoEx_t *textEx, TextOSDV2Info_t *OSDV2Info, int index)
{
	int result = 0;
	int i = 0;

	//设点阵及位置
	//文本
	//if (textoi != NULL)
	{
		if (textoi != NULL)
		{
			if(0 == m_iWhichValid)
			{
				result = ZWSys_EncSetOsdText(m_hEnc, textoi);
			}
		
			if (result)
			{
				OSD_ERR("[COsd:%s]MP4Sys_EncSetOsdText failed(%d)\n", __FUNCTION__, result);
				return result;
			}
		}
		
		if (textEx != NULL)
		{
			if(1 == m_iWhichValid)
			{
				result = ZWSys_EncSetOsdTextEx(m_hEnc, textEx);
			}
		
			if (result)
			{
				OSD_ERR("[COsd:%s]MP4Sys_EncSetOsdText failed(%d)\n", __FUNCTION__, result);
				return result;
			}
		}
        
		if (OSDV2Info != NULL)
		{
            // m_iWhichValid:多行OSD1的控制逻辑，多行OSD2(名称Osd)不受影响
			//if(2 == m_iWhichValid || 1 == index)
			{
                result = ZWSys_EncSetOsdV2Text(m_hEnc, OSDV2Info, index);
				if (result)
				{
					OSD_ERR("[COsd:%s]ZWSys_EncSetOsdV2Text failed(%d)\n", __FUNCTION__, result);
					return result;
				}
			}
		}
	}
	
	//时间
	if (timeoi != NULL)
	{
		for (i = 0; i < 5; i++)
		{
			result = ZWSys_EncSetOsdTime(m_hEnc, timeoi);
			if (((unsigned int) result) != 0xc0000009)
			{
				break;
			}
		}
		if (result)
		{
			OSD_ERR("[COsd:%s]MP4Sys_EncSetOsdTime failed (%d)\n", __FUNCTION__, result);
			return result;
		}
	}

	//打开
	//是否显示
	if (m_iShowType & 7)
	{
		result = ZWSys_EncSetOsd(m_hEnc, (m_iShowType >> 2) & 1, (m_iShowType >> 1) & 1, m_iShowType & 1);
		if (result)
		{
			OSD_ERR("[COsd:%s]ZWSys_EncSetOsd failed (%d)\n", __FUNCTION__, result);
			return result;
		}
	}

	return 0;
}

int COsd::CheckRect()
{
	return 0;
}

int COsd::LoadParam()
{
	char key_str[100] = {0};
	char value[256] = {0};
	char *pdata = NULL;
	int i=0,j=0;

    int nSDKRet = 0;
    CConfig *pCfg = NULL;
    pCfg = CConfig::NewInstance();
    if(pCfg)
    {
        nSDKRet = pCfg->LoadFileRO(CUSTOM_INFO_FILE);
        if (0 == nSDKRet)
        {
            SetOsdDirect = pCfg->GetValue("MultOsd", "bSetOsdDirect", 0);
        }
        CConfig::ReleaseInstance(pCfg);
    }

#if defined  (__GOKE__)
	//检查文件路径是否存在
	pdir = opendir(FileNameOsdPixel);
	if (NULL == pdir)
	{
		mkdir(FileNameOsdPixel, 777);

	}
	else
	{
		closedir(pdir);
	}
#endif

	//导入时间类型
	snprintf(key_str, sizeof(key_str), "OsdTimeFormat%02d", m_iChannel);
	m_iTimeType = pCConfig->GetValue(OsdCfgSection, key_str, 0);
	
	//导入显示类型
	snprintf(key_str, sizeof(key_str), "OsdShowType%02d", m_iChannel);
	m_iShowType = pCConfig->GetValue(OsdCfgSection, key_str, 3);

	//导入时间位置
	snprintf(key_str, sizeof(key_str), "OsdTimePos%02d", m_iChannel);
	pCConfig->GetValue(OsdCfgSection, key_str, "100,0", value, 100);
	sscanf(value, "%d,%d", &m_iTimeX, &m_iTimeY);

    m_iTimeNetCfgX = m_iTimeX;
    m_iTimeNetCfgY = m_iTimeY;

	// 导入OSDV2的配置, 置于RevertOSDBak前，以备份文件为准，防止简单恢复后OSD内容被重置
	for(i = 0; i < OSDV2_TEXT_CNT; i++)
	{
		// 使能
		snprintf(key_str, sizeof(key_str), "OsdV2Text%02dEn%02d", i, m_iChannel);
		pCConfig->GetValue(OsdCfgSection, key_str, "0", value, 100);
		sscanf(value, "%d", &m_tTextOsdV2[i].bEnable);
		
		// 坐标
		snprintf(key_str, sizeof(key_str), "OsdV2Text%02dPos%02d", i, m_iChannel);
		pCConfig->GetValue(OsdCfgSection, key_str, "0,0", value, 100);
		sscanf(value, "%d,%d", &m_tTextOsdV2[i].nX, &m_tTextOsdV2[i].nY);
		
		// 读取TOKEN
		char *szToken = m_tTextOsdV2[i].szToken;
		snprintf(key_str, sizeof(key_str), "OsdV2Text%02dToken%02d", i, m_iChannel);
		pCConfig->GetValue(OsdCfgSection, key_str, "", value, sizeof(value));
		sscanf(value, "%s", szToken);
		
		// 读取OSD的内容
		for(j = 0; j < OSDV2_TEXT_LENGTH/OSDV2_TEXT_CNT_SAVE_ONE_LINE; j++)
		{
			unsigned short *strText = m_tTextOsdV2[i].strText;
			snprintf(key_str, sizeof(key_str), "OsdV2Text%02dUnicode%02d_%02d", i, j, m_iChannel);
			pCConfig->GetValue(OsdCfgSection, key_str, "0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0", value, sizeof(value));
			sscanf(value, "%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu",
				&strText[j*32+0],&strText[j*32+1],&strText[j*32+2],&strText[j*32+3],&strText[j*32+4],&strText[j*32+5],&strText[j*32+6],&strText[j*32+7],
				&strText[j*32+8],&strText[j*32+9],&strText[j*32+10],&strText[j*32+11],&strText[j*32+12],&strText[j*32+13],&strText[j*32+14],&strText[j*32+15],
				&strText[j*32+16],&strText[j*32+17],&strText[j*32+18],&strText[j*32+19],&strText[j*32+20],&strText[j*32+21],&strText[j*32+22],&strText[j*32+23],
				&strText[j*32+24],&strText[j*32+25],&strText[j*32+26],&strText[j*32+27],&strText[j*32+28],&strText[j*32+29],&strText[j*32+30],&strText[j*32+31]);
		}
        // 填充m_tTextOsdV2Info
        m_tTextOsdV2Info[i].keyWordFontWidth = DEFAULT_OSDV2_PIXEL.w;
        m_tTextOsdV2Info[i].keyWordFontHeight = DEFAULT_OSDV2_PIXEL.h;
        m_tTextOsdV2Info[i].pKeywordPixInfo = (const char *)DEFAULT_OSDV2_PIXEL.pixel_buf;
	}

    m_iTextX = m_tTextOsdV2[0].nX;
    m_iTextY = m_tTextOsdV2[0].nY;

    m_iTextNetCfgX = m_iTextX;
    m_iTextNetCfgY = m_iTextY;
	
	m_iTimeX >>= 1;
	m_iTimeY >>= 1;
	m_iTextX >>= 1;
	m_iTextY >>= 1;

	if (m_iTextX >= 352)
	{
		m_iTextX = 352 / 2;

	}
	if (m_iTextY >= 288)
	{
		m_iTextY = 288 / 2;

	}
	m_iTimeX &= ~(3);
	m_iTextX &= ~(3);			//CIF，4对齐

	m_iTimeY &= ~(1);
	m_iTextY &= ~(1);

	//设置时间OSD
	OSD_DBG("\n.....COsd::LoadParam()........设置时间..............OSDm_iTimeTypen=%d.......\n",m_iTimeType);
	switch (m_iTimeType)
	{
		case 0:				//YYYYMMDD12
		{
			m_tTimeOsd.bHour24 = 0;
			m_tTimeOsd.DisMod = Mode_YYYY_MM_DD;

			break;
		}
		case 1:				//YYYYMMDD24
		{
			m_tTimeOsd.bHour24 = 1;
			m_tTimeOsd.DisMod = Mode_YYYY_MM_DD;
			
			break;
		}
		case 2:				//MMDDYYYY12
		{
			m_tTimeOsd.bHour24 = 0;
			m_tTimeOsd.DisMod = Mode_MM_DD_YYYY;
		
			break;
		}
		case 3:				//MMDDYYYY24
		{
			m_tTimeOsd.bHour24 = 1;
			m_tTimeOsd.DisMod = Mode_MM_DD_YYYY;
			
			break;
		}
		case 4:				//MMDDYYYY24
		{
			m_tTimeOsd.bHour24 = 0;
			m_tTimeOsd.DisMod = Mode_DD_MM_YYYY;
			
			break;
		}
		case 5:				//MMDDYYYY24
		{
			m_tTimeOsd.bHour24 = 1;
			m_tTimeOsd.DisMod = Mode_DD_MM_YYYY;
			
			break;
		}
	}
	OSD_DBG("\n..COsd::LoadParam()...m_tTimeOsd.bHour24=%d............m_tTimeOsd.DisMod=%d...........\n",m_tTimeOsd.bHour24,m_tTimeOsd.DisMod);
	m_tTimeOsd.bTranslant = 0;
	m_tTimeOsd.FontWidth = DEFAULT_TIME_PIXEL.w;
	m_tTimeOsd.FontHeight = DEFAULT_TIME_PIXEL.h;
	m_tTimeOsd.pFontPixinfo = (const char *) DEFAULT_TIME_PIXEL.pixel_buf;
	m_tTimeOsd.Start_X = m_iTimeX;
	m_tTimeOsd.Start_Y = m_iTimeY;       

	snprintf(key_str, sizeof(key_str), SECTION_OSD_ALIAS, m_iChannel);
	m_iWhichValid = pCConfig->GetValue(key_str, "WhichValid", 0);

	m_tTextOsdEx.Start_X = m_iTextX;
	m_tTextOsdEx.Start_Y = m_iTextY;

	m_iOsdFlowEn |= pCConfig->GetValue("PassengerFlowCfg", "Enable", 0);
	m_iOsdFlowEn &= pCConfig->GetValue("PassengerFlowCfg", "StatisticalResults", 0); 

	//RevertOSDBak();
    
//    if (0 == access(CUSTOM_MODE_FLAG, F_OK))
//    {
//        SaveOSDBak();
//    }
//    else
//    {
//	    RevertOSDBak();
//    }

	if(0 == m_iWhichValid)
	{
		notCheckParam = 1;
		SetCharPixel(pdata);
	}
	else if(1 == m_iWhichValid)
	{
		for(j=0;j<(DVR_MAX_CHANNELNAME_LEN - 1);j++)
		{
			m_strName[i] = m_strNameEx[j];
		}

		setTextOsd();
	}
	else
	{
        SetTextOSDV2(0);

        pCConfig->GetValue(key_str, "Unicode", "73:80:67:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0", value, 256);
        sscanf(value, "%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu:%hu",
    		&m_strName[0],&m_strName[1],&m_strName[2],&m_strName[3],
    		&m_strName[4],&m_strName[5],&m_strName[6],&m_strName[7],
    		&m_strName[8],&m_strName[9],&m_strName[10],&m_strName[11],
    		&m_strName[12],&m_strName[13],&m_strName[14],&m_strName[15],
    		&m_strName[16],&m_strName[17],&m_strName[18],&m_strName[19]); 
	}

	
    SetTextOSDV2(1);
	SetType(m_iShowType);

	return 0;
}
