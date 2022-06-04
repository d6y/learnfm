/*
 * Copyright 2012 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
extern int file_clamped;

/*
void mkdx7notes(double sample_rate)
{
  const int n_samples = 10 * 1024;

  Dx7Note note;
  FILE *f = fopen("../compact.bin", "rb");
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  char *all_patches = (char *)malloc(fsize);
  fseek(f, 0, SEEK_SET);
  fread(all_patches, 1, fsize, f);
  fclose(f);
  int patches = fsize / 128;
  printf("%d patches\n", patches);
  char *unpacked_patches = (char *)malloc(patches * 156);
  for (int i = 0; i < patches; i++)
  {
    UnpackPatch(all_patches + (i * 128), unpacked_patches + (i * 156));
  }
  printf("%d had to be clamped\n", file_clamped);
  WavOut w("/tmp/foo.wav", sample_rate, n_samples * patches);

  for (int patchy = 0; patchy < patches; patchy++)
  {
    note.init(unpacked_patches + (patchy * 156), 50 + (patchy % 12), 100);
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
  }
  w.close();
  free(all_patches);
  free(unpacked_patches);
}
*/

void record_patch(double sample_rate)
{
  const int n_samples = 10 * 1024;

  Dx7Note note;
  int patches = 1;
  char *unpacked_patches = (char *)malloc(patches * 156);

  unpacked_patches[0] = 99;  // 5_R1
  unpacked_patches[1] = 95;  // 5_R2
  unpacked_patches[2] = 99;  // 5_R3
  unpacked_patches[3] = 99;  // 5_R4
  unpacked_patches[4] = 99;  // 5_L1
  unpacked_patches[5] = 93;  // 5_L2
  unpacked_patches[6] = 99;  // 5_L3
  unpacked_patches[7] = 0;   // 5_L4
  unpacked_patches[8] = 0;   // 5_BP # break point
  unpacked_patches[9] = 0;   // 5_LD # l scale depth
  unpacked_patches[10] = 0;  // 5_RD # r scale depth
  unpacked_patches[11] = 0;  // 5_RC # r key scale
  unpacked_patches[12] = 0;  // 5_LC # l key scale
  unpacked_patches[13] = 0;  // 5_DET # detune
  unpacked_patches[14] = 0;  // 5_RS # rate scaling
  unpacked_patches[15] = 0;  // 5_KVS # key velocity
  unpacked_patches[16] = 0;  // 5_AMS # a mod sense
  unpacked_patches[17] = 0;  // 5_OL # output level
  unpacked_patches[18] = 1;  // 5_FC # f course
  unpacked_patches[19] = 0;  // 5_M # mode
  unpacked_patches[20] = 0;  // 5_FF # f fine
  unpacked_patches[21] = 99; // 4_R1
  unpacked_patches[22] = 95; // 4_R2
  unpacked_patches[23] = 99; // 4_R3
  unpacked_patches[24] = 99; // 4_R4
  unpacked_patches[25] = 99; // 4_L1
  unpacked_patches[26] = 93; // 4_L2
  unpacked_patches[27] = 99; // 4_L3
  unpacked_patches[28] = 0;  // 4_L4
  unpacked_patches[29] = 0;  // 4_BP # break point
  unpacked_patches[30] = 0;  // 4_LD # l scale depth
  unpacked_patches[31] = 0;  // 4_RD # r scale depth
  unpacked_patches[32] = 0;  // 4_RC # r key scale
  unpacked_patches[33] = 0;  // 4_LC # l key scale
  unpacked_patches[34] = 0;  // 4_DET # detune
  unpacked_patches[35] = 0;  // 4_RS # rate scaling
  unpacked_patches[36] = 0;  // 4_KVS # key velocity
  unpacked_patches[37] = 0;  // 4_AMS # a mod sense
  unpacked_patches[38] = 0;  // 4_OL # output level
  unpacked_patches[39] = 1;  // 4_FC # f course
  unpacked_patches[40] = 0;  // 4_M # mode
  unpacked_patches[41] = 0;  // 4_FF # f fine
  unpacked_patches[42] = 99; // 3_R1
  unpacked_patches[43] = 95; // 3_R2
  unpacked_patches[44] = 99; // 3_R3
  unpacked_patches[45] = 99; // 3_R4
  unpacked_patches[46] = 99; // 3_L1
  unpacked_patches[47] = 93; // 3_L2
  unpacked_patches[48] = 99; // 3_L3
  unpacked_patches[49] = 0;  // 3_L4
  unpacked_patches[50] = 0;  // 3_BP # break point
  unpacked_patches[51] = 0;  // 3_LD # l scale depth
  unpacked_patches[52] = 0;  // 3_RD # r scale depth
  unpacked_patches[53] = 0;  // 3_RC # r key scale
  unpacked_patches[54] = 0;  // 3_LC # l key scale
  unpacked_patches[55] = 0;  // 3_DET # detune
  unpacked_patches[56] = 0;  // 3_RS # rate scaling
  unpacked_patches[57] = 0;  // 3_KVS # key velocity
  unpacked_patches[58] = 0;  // 3_AMS # a mod sense
  unpacked_patches[59] = 0;  // 3_OL # output level
  unpacked_patches[60] = 1;  // 3_FC # f course
  unpacked_patches[61] = 0;  // 3_M # mode
  unpacked_patches[62] = 0;  // 3_FF # f fine
  unpacked_patches[63] = 99; // 2_R1
  unpacked_patches[64] = 95; // 2_R2
  unpacked_patches[65] = 99; // 2_R3
  unpacked_patches[66] = 99; // 2_R4
  unpacked_patches[67] = 99; // 2_L1
  unpacked_patches[68] = 93; // 2_L2
  unpacked_patches[69] = 99; // 2_L3
  unpacked_patches[70] = 0;  // 2_L4
  unpacked_patches[71] = 0;  // 2_BP # break point
  unpacked_patches[72] = 0;  // 2_LD # l scale depth
  unpacked_patches[73] = 0;  // 2_RD # r scale depth
  unpacked_patches[74] = 0;  // 2_RC # r key scale
  unpacked_patches[75] = 0;  // 2_LC # l key scale
  unpacked_patches[76] = 0;  // 2_DET # detune
  unpacked_patches[77] = 0;  // 2_RS # rate scaling
  unpacked_patches[78] = 0;  // 2_KVS # key velocity
  unpacked_patches[79] = 0;  // 2_AMS # a mod sense
  unpacked_patches[80] = 0;  // 2_OL # output level
  unpacked_patches[81] = 1;  // 2_FC # f course
  unpacked_patches[82] = 0;  // 2_M # mode
  unpacked_patches[83] = 0;  // 2_FF # f fine

  // VALUE BELOW HERE HAVE BEEN RE-ORDERED TO MATCH
  // ORDER in dx7 sysex format definition.
  // Noted with comment ORDER_CHANGED
  // Would also apply to values above, but we are not using them
  // at the moment

  unpacked_patches[84] = 99;  // 1_R1
  unpacked_patches[85] = 95;  // 1_R2
  unpacked_patches[86] = 99;  // 1_R3
  unpacked_patches[87] = 99;  // 1_R4
  unpacked_patches[88] = 99;  // 1_L1
  unpacked_patches[89] = 93;  // 1_L2
  unpacked_patches[90] = 99;  // 1_L3
  unpacked_patches[91] = 0;   // 1_L4
  unpacked_patches[92] = 0;   // 1_BP # break point
  unpacked_patches[93] = 0;   // 1_LD # l scale depth
  unpacked_patches[94] = 0;   // 1_RD # r scale depth
  unpacked_patches[95] = 0;   // 1_LC # l key scale ORDER_CHANGED
  unpacked_patches[96] = 0;   // 1_RC # r key scale ORDER_CHANGED
  unpacked_patches[97] = 0;   // 1_RS # rate scaling ORDER_CHANGED
  unpacked_patches[98] = 0;   // 1_AMS # a mod sense ORDER_CHANGED
  unpacked_patches[99] = 0;   // 1_KVS # key velocity ORDER_CHANGED
  unpacked_patches[100] = 90; // 1_OL # output level *N23* ORDER_CHANGED
  unpacked_patches[101] = 0;  // 1_M # mode ORDER_CHANGED
  unpacked_patches[102] = 2;  // 1_FC # f course *N23* ORDER_CHANGED
  unpacked_patches[103] = 0;  // 1_FF # f fine *N23* ORDER_CHANGED
  unpacked_patches[104] = 0;  // 1_DET # detune ORDER_CHANGED

  unpacked_patches[105] = 99; // 0_R1
  unpacked_patches[106] = 95; // 0_R2
  unpacked_patches[107] = 99; // 0_R3
  unpacked_patches[108] = 99; // 0_R4
  unpacked_patches[109] = 99; // 0_L1
  unpacked_patches[110] = 93; // 0_L2
  unpacked_patches[111] = 99; // 0_L3
  unpacked_patches[112] = 0;  // 0_L4
  unpacked_patches[113] = 0;  // 0_BP # break point
  unpacked_patches[114] = 0;  // 0_LD # l scale depth
  unpacked_patches[115] = 0;  // 0_RD # r scale depth
  unpacked_patches[116] = 0;  // 0_LC # l key scale ORDER_CHANGED
  unpacked_patches[117] = 0;  // 0_RC # r key scale ORDER_CHANGED
  unpacked_patches[118] = 0;  // 0_RS # rate scaling ORDER_CHANGED
  unpacked_patches[119] = 0;  // 0_AMS # a mod sense ORDER_CHANGED
  unpacked_patches[120] = 0;  // 0_KVS # key velocity ORDER_CHANGED
  unpacked_patches[121] = 99; // 0_OL # output level *N23* ORDER_CHANGED
  unpacked_patches[122] = 0;  // 0_M # mode ORDER_CHANGED
  unpacked_patches[123] = 1;  // 0_FC # f course *N23* ORDER_CHANGED
  unpacked_patches[124] = 0;  // 0_FF # f fine *N23* ORDER_CHANGED
  unpacked_patches[125] = 0;  // 0_DET # detune ORDER_CHANGED

  unpacked_patches[126] = 99;  // PR1 # PITCH EG RATE 1
  unpacked_patches[127] = 99;  // PR2 # PITCH EG RATE 2
  unpacked_patches[128] = 99;  // PR3 # PITCH EG RATE 3
  unpacked_patches[129] = 99;  // PR4 # PITCH EG RATE 4
  unpacked_patches[130] = 50;  // PL1 # PITCH EG LEVEL 1
  unpacked_patches[131] = 50;  // PL2 # PITCH EG LEVEL 2
  unpacked_patches[132] = 50;  // PL3 # PITCH EG LEVEL 3
  unpacked_patches[133] = 50;  // PL4 # PITCH EG LEVEL 4
  unpacked_patches[134] = 1;   // ALG # ALGORITHM (from 0)
  unpacked_patches[135] = 4;   // FB # FEEDBACK - order ? - N23 ORDER_CHANGED
  unpacked_patches[136] = 1;   // OKS # OSC KEY SYNC - order ? ORDER_CHANGED
  unpacked_patches[137] = 35;  // LFS # LFO SPEED
  unpacked_patches[138] = 0;   // LFD # LFO DELAY
  unpacked_patches[139] = 0;   // LPMD # LFO PM DEPTH
  unpacked_patches[140] = 0;   // LAMD # LFO AM DEPTH
  unpacked_patches[141] = 1;   // LKS # LFO KEY SYNC ? ORDER_CHANGEFD
  unpacked_patches[142] = 0;   // LFW # LFO WAVE ORDER_CHANGED
  unpacked_patches[143] = 3;   // LPMS # P MODE SENS ?
  unpacked_patches[144] = 24;  // TRNSP # Transpose
  unpacked_patches[145] = 84;  // NAME CHAR 1
  unpacked_patches[146] = 101; // NAME CHAR 2
  unpacked_patches[147] = 115; // NAME CHAR 3
  unpacked_patches[148] = 116; // NAME CHAR 4
  unpacked_patches[149] = 50;  // NAME CHAR 5
  unpacked_patches[150] = 32;  // NAME CHAR 6
  unpacked_patches[151] = 32;  // NAME CHAR 7
  unpacked_patches[152] = 32;  // NAME CHAR 8
  unpacked_patches[153] = 32;  // NAME CHAR 9
  unpacked_patches[154] = 32;  // NAME CHAR 10

  // SYSEX documentatin:
  // 155        OPERATOR ON/OFF
  //             bit6 = 0 / bit 5: OP1 / ... / bit 0: OP6

  // Turn them all on = 63
  unpacked_patches[155] = 63;

  CheckPatch(unpacked_patches);
  printf("%d had to be clamped\n", file_clamped);

  WavOut w("/tmp/bar.wav", sample_rate, n_samples * patches);

  for (int patchy = 0; patchy < patches; patchy++)
  {
    note.init(unpacked_patches + (patchy * 156), 50 + (patchy % 12), 100);
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
  }
  w.close();
  free(unpacked_patches);
}

int main(int argc, char **argv)
{
  double sample_rate = 44100.0;
  Freqlut::init(sample_rate);
  Sawtooth::init(sample_rate);
  Sin::init();
  Exp2::init();
  Log2::init();
  // mkdx7notes(sample_rate);
  record_patch(sample_rate);
  return 0;
}
