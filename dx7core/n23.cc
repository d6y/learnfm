#include <iostream>
#include <cstdlib>
#include <math.h>
#include "synth.h"
#include "module.h"
#include "aligned_buf.h"
#include "freqlut.h"
#include "wavout.h"
#include "sawtooth.h"
#include "sin.h"
#include "exp2.h"
#include "log2.h"
#include "resofilter.h"
#include "fm_core.h"
#include "fm_op_kernel.h"
#include "env.h"
#include "patch.h"
#include "controllers.h"
#include "dx7note.h"

using namespace std;

void set_osc(char patch[156], int osc_num_from_one, int coarce, int fine, int output_level)
{
    assert(osc_num_from_one >= 1 && osc_num_from_one <= 6);

    // Where in patch[] does the osc start?
    int osc_offset;
    switch (osc_num_from_one)
    {
    case 6:
        osc_offset = 0;
        break;
    case 5:
        osc_offset = 21;
        break;
    case 4:
        osc_offset = 42;
        break;
    case 3:
        osc_offset = 63;
        break;
    case 2:
        osc_offset = 84;
        break;
    case 1:
        osc_offset = 105;
        break;

    default:
        assert(false && "Unreachable");
    };

    patch[osc_offset + 0] = 99;            // R1
    patch[osc_offset + 1] = 95;            // R2
    patch[osc_offset + 2] = 99;            // R3
    patch[osc_offset + 3] = 99;            // R4
    patch[osc_offset + 4] = 99;            // L1
    patch[osc_offset + 5] = 93;            // L2
    patch[osc_offset + 6] = 99;            // L3
    patch[osc_offset + 7] = 0;             // L4
    patch[osc_offset + 8] = 0;             // BP # break point
    patch[osc_offset + 9] = 0;             // LD # l scale depth
    patch[osc_offset + 10] = 0;            // RD # r scale depth
    patch[osc_offset + 11] = 0;            // LC # l key scale ORDER_CHANGED
    patch[osc_offset + 12] = 0;            // RC # r key scale ORDER_CHANGED
    patch[osc_offset + 13] = 0;            // RS # rate scaling ORDER_CHANGED
    patch[osc_offset + 14] = 0;            // AMS # a mod sense ORDER_CHANGED
    patch[osc_offset + 15] = 0;            // KVS # key velocity ORDER_CHANGED
    patch[osc_offset + 16] = output_level; // OL # output level *N23* ORDER_CHANGED
    patch[osc_offset + 17] = 0;            // M # mode ORDER_CHANGED
    patch[osc_offset + 18] = coarce;       // FC # f course *N23* ORDER_CHANGED
    patch[osc_offset + 19] = fine;         // FF # f fine *N23* ORDER_CHANGED
    patch[osc_offset + 20] = 0;            // DET # detune ORDER_CHANGED
}

void set_others(char patch[156], char name[10])
{
    patch[126] = 99;      // PR1 # PITCH EG RATE 1
    patch[127] = 99;      // PR2 # PITCH EG RATE 2
    patch[128] = 99;      // PR3 # PITCH EG RATE 3
    patch[129] = 99;      // PR4 # PITCH EG RATE 4
    patch[130] = 50;      // PL1 # PITCH EG LEVEL 1
    patch[131] = 50;      // PL2 # PITCH EG LEVEL 2
    patch[132] = 50;      // PL3 # PITCH EG LEVEL 3
    patch[133] = 50;      // PL4 # PITCH EG LEVEL 4
    patch[134] = 1;       // ALG # ALGORITHM (from 0)
    patch[135] = 4;       // FB # FEEDBACK - order ? - N23 ORDER_CHANGED
    patch[136] = 1;       // OKS # OSC KEY SYNC - order ? ORDER_CHANGED
    patch[137] = 35;      // LFS # LFO SPEED
    patch[138] = 0;       // LFD # LFO DELAY
    patch[139] = 0;       // LPMD # LFO PM DEPTH
    patch[140] = 0;       // LAMD # LFO AM DEPTH
    patch[141] = 1;       // LKS # LFO KEY SYNC ? ORDER_CHANGEFD
    patch[142] = 0;       // LFW # LFO WAVE ORDER_CHANGED
    patch[143] = 3;       // LPMS # P MODE SENS ?
    patch[144] = 24;      // TRNSP # Transpose
    patch[145] = name[0]; // NAME CHAR 1
    patch[146] = name[1]; // NAME CHAR 2
    patch[147] = name[2]; // NAME CHAR 3
    patch[148] = name[3]; // NAME CHAR 4
    patch[149] = name[4]; // NAME CHAR 5
    patch[150] = name[5]; // NAME CHAR 6
    patch[151] = name[6]; // NAME CHAR 7
    patch[152] = name[7]; // NAME CHAR 8
    patch[153] = name[8]; // NAME CHAR 9
    patch[154] = name[9]; // NAME CHAR 10

    // SYSEX documentatin:
    // 155        OPERATOR ON/OFF
    //             bit6 = 0 / bit 5: OP1 / ... / bit 0: OP6

    // Turn them all on = 63
    patch[155] = 63;
}

void generate_wav(char patch[156], const char *filename)
{
    double sample_rate = 44100.0;
    const int n_samples = 10 * 1024;

    WavOut w(filename, sample_rate, n_samples);

    Dx7Note note;
    note.init(patch, 50, 100);
    Controllers controllers;
    controllers.values_[kControllerPitch] = 0x2000;
    int32_t buf[N];

    for (int i = 0; i < n_samples; i += N)
    {
        for (int j = 0; j < N; j++)
        {
            buf[j] = 0;
        }
        if (i >= n_samples * (7. / 8.))
        {
            note.keyup();
        }
        note.compute(buf, 0, 0, &controllers);
        for (int j = 0; j < N; j++)
        {
            buf[j] >>= 2;
        }
        w.write_data(buf, N);
    }
    w.close();
}

void write_patch()
{
    double sample_rate = 44100.0;
    Freqlut::init(sample_rate);
    Sawtooth::init(sample_rate);
    Sin::init();
    Exp2::init();
    Log2::init();

    char *unpacked_patch = (char *)malloc(156);

    set_osc(unpacked_patch, 1, 1, 0, 99);
    set_osc(unpacked_patch, 2, 2, 0, 90);
    set_osc(unpacked_patch, 3, 0, 0, 0);
    set_osc(unpacked_patch, 4, 0, 0, 0);
    set_osc(unpacked_patch, 5, 0, 0, 0);
    set_osc(unpacked_patch, 6, 0, 0, 0);

    char name[10] = "TEST     ";
    set_others(unpacked_patch, name);

    CheckPatch(unpacked_patch);

    generate_wav(unpacked_patch, "/tmp/one.wav");

    free(unpacked_patch);
}

int main(int argc, char **argv)
{
    printf("Running n23.cc:main \n");
    write_patch();
    return 0;
}