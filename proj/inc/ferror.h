//
// ferror.h (file error)
//

// error code
enum {
	FERRCODE_FILE_NOT_EXIST = 1,
	FERRCODE_CANNOT_OPEN_FILE,
	FERRCODE_CANNOT_CREATE_FILE,
	FERRCODE_DISK_FULL,
	FERRCODE_UNABLE_MALLOC,
	FERRCODE_UNABLE_READ_FILE,
	FERRCODE_NOT_OED_FORMAT,
	FERRCODE_ILLEGAL_SNDFILE,
	FERRCODE_ILLEGAL_MMLFILE,
	FERRCODE_ILLEGAL_MDXFILE,
	FERRCODE_ILLEGAL_OEDFILE
};

// error message
#define MES_FERR_FILE_NOT_EXIST     "ファイルが見つかりません。"
#define MES_FERR_CANNOT_OPEN_FILE   "ファイルが開けません。"
#define MES_FERR_CANNOT_CREATE_FILE "ファイルを作成できません。"
#define MES_FERR_DISK_FULL          "ディスクの空き容量が足りません。"
#define MES_FERR_UNABLE_MALLOC      "メモリが確保できません。"
#define MES_FERR_UNABLE_READ_FILE   "ファイルを読むことができません。"
#define MES_FERR_NOT_OED_FORMAT     "OED形式のファイルではありません。"
#define MES_FERR_ILLEGAL_SNDFILE    "異常なSNDファイルです。"
#define MES_FERR_ILLEGAL_MMLFILE    "異常なMMLファイルです。"
#define MES_FERR_ILLEGAL_MDXFILE    "異常なMDXファイルです。"
#define MES_FERR_ILLEGAL_OEDFILE    "異常なOEDファイルです。"
