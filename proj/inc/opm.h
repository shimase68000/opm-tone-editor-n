//
// opm.h
////   Requires escseq.h: DISP_ALG* embed attribute sequences.
//


#include    "escseq.h"

#define _DISP_NO_ESCSEQ__
#undef  _DISP_NO_ESCSEQ__

// OPMREG
#define OPMREG_TEST       0x01
#define OPMREG_KON        0x08
#define OPMREG_NE_NFRQ    0x0F
#define OPMREG_CLKA1      0x10
#define OPMREG_ALKA2      0x11
#define OPMREG_CLKB       0x12
#define OPMREG_IRQ_LOAD   0x14
#define OPMREG_LFRQ       0x18
#define OPMREG_PMD_AMD    0x19
#define OPMREG_CT_W       0x1B
#define OPMREG_RL_FB_CON  0x20
#define OPMREG_KC         0x28
#define OPMREG_KF         0x30
#define OPMREG_PMS_AMS    0x38
#define OPMREG_DT1_MUL    0x40
#define OPMREG_TL         0x60
#define OPMREG_KS_AR      0x80
#define OPMREG_AMS_EN_D1R 0xA0
#define OPMREG_DT2_D2R    0xC0
#define OPMREG_D1L_RR     0xE0

// for PMD_AMD reg
#define SEL_PMD_REG       0x80
#define SEL_AMD_REG       0x00

// for disp
#define DISP_ALG_XPOS     (TX0+1)            // disp. alg. pos.
#define DISP_ALG_YPOS     (TY0-2)
#define DISP_OPMTIP_XPOS  (DISP_ALG_XPOS+14) // disp opm tip. pos.
#define DISP_OPMTIP_YPOS  (DISP_ALG_YPOS)

#define DISP_ALG_BASE "CON "
//                     123456789
#ifdef _DISP_NO_ESCSEQ_
  #define DISP_ALG0   "1234=    "   // disp. alg.0
  #define DISP_ALG1   "1-2-34=  "   // disp. alg.1
  #define DISP_ALG2   "1-23-4=  "   // disp. alg.2
  #define DISP_ALG3   "12-3-4=  "   // disp. alg.3
  #define DISP_ALG4   "12=34=   "   // disp. alg.4
  #define DISP_ALG5   "12=13=14="   // disp. alg.5
  #define DISP_ALG6   "12=3=4=  "   // disp. alg.6
  #define DISP_ALG7   "1=2=3=4= "   // disp. alg.7
#else
  #define DISP_ALG0   C1 "1" C1 "234=    "   // disp. alg.0
  #define DISP_ALG1   C1 "1" C1 "-2-34=  "   // disp. alg.1
  #define DISP_ALG2   C1 "1" C1 "-23-4=  "   // disp. alg.2
  #define DISP_ALG3   C1 "1" C1 "2-3-4=  "   // disp. alg.3
  #define DISP_ALG4   C1 "1" C1 "2=34=   "   // disp. alg.4
  #define DISP_ALG5   C1 "1" C1 "2=" C1 "1" C1 "3=" C1 "1" C1 "4="   // disp. alg.5
  #define DISP_ALG6   C1 "1" C1 "2=3=4=  "   // disp. alg.6
  #define DISP_ALG7   C1 "1" C1 "=2=3=4= "   // disp. alg.7
#endif

//                  01234567890
#define DISP_SLOT0  ""            // slot0
#define DISP_SLOT1  "Slot1(M1) "  // slot1
#define DISP_SLOT2  "Slot2(C1) "  // slot2
#define DISP_SLOT3  "Slot3(M2) "  // slot3
#define DISP_SLOT4  "Slot4(C2) "  // slot4

//                  0         1         2         3
//                  0123456789012345678901234567890123456
#define DISP_OPM00  "Connection 0-7                      "  // con
#define DISP_OPM01  "Self feedback level 0-7             "  // feedback
#define DISP_OPM02  "Slotmask 0-15 (bit3-0:C2|M2|C1|M1)  "  // slotmask
#define DISP_OPM03  "L/R Channel enable 0-3 (bit1-0:R|L) "  // pan
#define DISP_OPM04  "Phase modulation sensitivity 0-7    "  // pms
#define DISP_OPM05  "Amplitude modulation sensitivity 0-3"  // ams
#define DISP_OPM06  "Sync enable 0-1 (not supported)     "  // sync
#define DISP_OPM07  "Waveform for LFO 0-3                "  // waveform
#define DISP_OPM08  "Low frequency 0-255                 "  // lfreq
#define DISP_OPM09  "Phase modulation depth 0-127        "  // pmd
#define DISP_OPM10  "Amplitude modulation depth 0-127    "  // amd
#define DISP_OPM11  "Noise enable 0-1 (Ch.H-C2)          "  // noise
#define DISP_OPM12  "Noise frequency 0-31 (Ch.H-C2)      "  // nfreq
//                  0         1         2         3
//                  0123456789012345678901234567890123456
#define DISP_OPM20  "Attack rate 0-31          "  // ar
#define DISP_OPM21  "First decay rate 0-31     "  // d1r
#define DISP_OPM22  "Second decay rate 0-31    "  // d2r
#define DISP_OPM23  "Release rate 0-15         "  // rr
#define DISP_OPM24  "First decay level 0-15    "  // d1l
#define DISP_OPM25  "Total level 0-127         "  // tl
#define DISP_OPM26  "Key scaling 0-3           "  // ks
#define DISP_OPM27  "Phase multiply 0-15       "  // mul
#define DISP_OPM28  "Detune(1) 0-7             "  // dt1
#define DISP_OPM29  "Detune(2) 0-3             "  // dt2
#define DISP_OPM30  "AMS enable 0-1            "  // amse
