//
// file.h
//

#include    "escseq.h"

#define MML_FILECOMMENT      "/* %s version %s%s */"
#define OED_FILECOMMENT      "%s version %s%s"          // default file comment
//
//                            1234567890123456
#ifndef _debug_
#define	OED_FILEHEADER_SJIS  "OED000          "   // file header for SJIS
#define	OED_FILEHEADER_UTF8  "OED000U8        "   // file header for UTF-8
#else
#define	OED_FILEHEADER_SJIS  "OED061          "   // file header for SJIS
#define	OED_FILEHEADER_UTF8  "OED061U8        "   // file header for UTF-8
#endif

#define OED_FILEHEADER_SIZE  (16)
#define OED_TONESIZE         (sizeof(OPMDATA))
//
//                            1234567890123456
#ifndef _debug_
#define	TED_FILEHEADER_SJIS  "TED000          "   // template file header for SJIS
#define	TED_FILEHEADER_UTF8  "TED000U8        "   // template file header for UTF-8
#else
#define	TED_FILEHEADER_SJIS  "TED060          "   // template file header for SJIS
#define	TED_FILEHEADER_UTF8  "TED060U8        "   // template file header for UTF-8
#endif

#define TED_FILEHEADER_SIZE  (16)
#define TED_TONESIZE         (sizeof(OPMDATA))

// SEEK() origin (doslib). Guarded because <stdio.h> defines the same
// names with the same values.
#ifndef SEEK_SET
#define SEEK_SET  0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR  1
#endif
#ifndef SEEK_END
#define SEEK_END  2
#endif

#define	READBUF		        2048
#define FILESIZE_SND        16000

enum FILE_FORM {
	FORM_OED = 1,  // .oed
	FORM_SND,      // .snd
	FORM_MDX,      // .mdx
	FORM_MML,      // .mml
	FORM_MUS,      // .mus
	FORM_OPM,      // .opm
	FORM_ZMS,      // .zms
};

#define STR_FILEEXT_OED     ".OED"
#define STR_FILEEXT_SND     ".SND"
#define STR_FILEEXT_MDX     ".MDX"
#define STR_FILEEXT_MML     ".MML"
#define STR_FILEEXT_MUS     ".MUS"  // *dummy*
#define STR_FILEEXT_OPM     ".OPM"
#define STR_FILEEXT_ZMS     ".ZMS"

// File status strings shown by check_file_status().
//   RO and DIR carry two colour sequences, so they are longer than they
//   appear on screen. check_file_status() subtracts that amount when it
//   trims the line, so keep STR_FILESTATUS_NONPRINT in step.
#define STR_FILESTATUS_RW   "Read/Write"
#define STR_FILESTATUS_RO   C32 "ReadOnly"  C33
#define STR_FILESTATUS_DIR  C31 "Directory" C33
#define STR_FILESTATUS_NONPRINT (10)   // non-printing bytes in RO / DIR

//
// size of reserve area
//
#define OED_RESERVE0_SIZE   32
#define OED_RESERVE1_SIZE   8
#define TED_RESERVE0_SIZE   (OED_RESERVE0_SIZE)
#define TED_RESERVE1_SIZE   (OED_RESERVE1_SIZE)
#define TED_RESERVE2_SIZE   16

//
// OED_EX default
//
#define OED_EX_AUTHOR       ""
#define OED_EX_TOOLNAME     "OPM Tone Editor 'N' v%s%s"
#define OED_EX_ENVIRONMENT  "Human68k"
#define OED_EX_DATETIME     ""
#define OED_EX_TARGETDEVICE "YM2151"

//
// OED file format v000
//
typedef struct {
	char author[32];         // sjis, not include termination code
	char toolname[32];       // sjis
	char environment[32];    // ascii
	char datetime[32];       // ascii
	char targetdevice[32];   // ascii
} OED_EX;

typedef struct {
	char   header[OED_FILEHEADER_SIZE];  // ascii
	char   filememo[OED_FILEMEMO_SIZE];  // sjis, not include termination code
	OED_EX oedex;
	uchar  channelvol[2][8];    // 8ch x 2set
	uchar  reserve0[OED_RESERVE0_SIZE];
	int    tonecount;
	int    tonesize;
	uchar  reserve1[OED_RESERVE1_SIZE];
} OED_FORMAT;

//
// TED file format v000
//
//   Reserved for standalone template (.TED) support, which is not
//   implemented - ted2tone()/tone2ted() are stubs. Every TED_* name
//   below, and OED_FILEHEADER_UTF8, is unused on purpose.
//
typedef struct {
	char   header[TED_FILEHEADER_SIZE];  // ascii
	char   filememo[TED_FILEMEMO_SIZE];  // sjis
	OED_EX oedex;
	uchar  reserve2[TED_RESERVE2_SIZE];
	uchar  reserve0[TED_RESERVE0_SIZE];
	int    tonecount;
	int    tonesize;
	uchar  reserve1[TED_RESERVE1_SIZE];
} TED_FORMAT;

