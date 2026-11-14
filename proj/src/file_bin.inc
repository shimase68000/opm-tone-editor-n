//
// file_bin.inc
//    BIN format input. Included from file.c.
//
//    Voice bank written by NOTE.X with -t switch (save tone data). Fixed
//    7168 bytes: 256 flag bytes, then 27 bytes x 256 records whose
//    layout is the same as MDX. Import only, for the same reason
//    as MDX.
//

//
// NOTE tone bank (tone.bin) to tone data
//
//   NOTE.X writes its FM voice bank with -t switch (save tone data).
//   Layout - 7168 bytes, fixed:
//     +0x0000  256 bytes   valid flag per voice (non-zero = defined)
//     +0x0100  27 x 256    voice records, same layout as MDX tone data
//
//   Record:
//     +0  voice number (must equal its own index)
//     +1  (FL<<3)|CON   (<= 63)
//     +2  slot mask     (<= 15)
//     +3  24 bytes      6 groups x 4 operators (MDX order: M1,M2,C1,C2)
//
//   There is no magic number. Validity rests on the fixed size plus the
//   self-consistency of every flagged record.
//
//   Up to 256 voices may be defined, but the editor holds TONENUM.
//   Defined voices are packed from the top, gaps closed; anything past
//   TONENUM is discarded. The original voice number is kept in the tone
//   name ("@n"), so it survives the renumbering.
//
static int bin2tone(int tan, char *filename)
{
	char *flag;
	char *rec;
	int   selectflag;
	int   i, n;
	char *sfilename;

	sfilename = (char*)MakeStrFilename(filename);

	// get file date then make file memo
	{
		int fn;
		int date;
		int st;

		if(0 > (fn = OPEN(filename, 0x000))) return FERRCODE_CANNOT_OPEN_FILE;
		date = FILEDATE(fn, 0);
		CLOSE(fn);

		st = read_binary_file(filename, FORM_BIN);
		if(st) return st;

		// init oeddata then make file memo
		memset(&oeddata[tan], 0, (int)sizeof(oeddata[tan]));
		sprintf((char*)oeddata[tan].filememo, "%s %s",
		                                      sfilename,
		                                      (char*)MakeStrDateTime(date));
	}

	flag = filebuf;
	rec  = filebuf + BIN_FLAG_SIZE;

	// validate every flagged record before touching tone data (fail fast)
	for(n=0; n<BIN_TONENUM; n++) {
		char *p;

		if(!U8(flag+n)) continue;

		p = rec + n*BIN_TONESIZE;
		if(U8(p)   != n)  {MFREE(filebuf); return FERRCODE_ILLEGAL_BINFILE;}
		if(U8(p+1) > 63)  {MFREE(filebuf); return FERRCODE_ILLEGAL_BINFILE;}
		if(U8(p+2) > 15)  {MFREE(filebuf); return FERRCODE_ILLEGAL_BINFILE;}
	}

	// init tone data
	InitToneData(tan);

	// set select flag
	if(config.auto_select.mdx) selectflag = MAINFLAG_SELECT;
	else                       selectflag = 0;

	// read defined voices, packed from the top
	i = 0;
	for(n=0; n<BIN_TONENUM && i<TONENUM; n++) {
		char *p;

		if(!U8(flag+n)) continue;

		p = rec + n*BIN_TONESIZE;

		// set tone name
		memset(Tone[tan][i].name, 0, (int)sizeof(Tone[tan][i].name));
		sprintf(Tone[tan][i].name, "@%d", U8(p));

		// set tone memo
		memset(Tone[tan][i].memo, 0, (int)sizeof(Tone[tan][i].memo));
		sprintf(Tone[tan][i].memo, "%s %s", sfilename, Tone[tan][i].name);

		unpack_mdx_tone(&Tone[tan][i], p);

		Tone[tan][i].chmode = DEFAULT_OPM_CHMODE;
		Tone[tan][i].chflag = config.opm_channel.mdx;
		Tone[tan][i].volume = DEFAULT_OPM_VOLUME;

		// init. select flag
		flag_select_init_main(tan, i, selectflag);

		i++;
	}

	oeddata[tan].tonecount = i;
	oeddata[tan].tonesize = sizeof(OPMDATA);

	MFREE(filebuf);
	return 0;
}

