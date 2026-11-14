//
// file_mdx.inc
//    MDX format input. Included from file.c.
//
//    Tones are read from the 27-byte records held in an MDX file.
//    Import only: a binary format is unreadable without its
//    documentation, so the editor reads it and converts it to a
//    readable format (MML or OPM) instead of producing more of it.
//

//
// mdx to tone data
//
static int mdx2tone(int tan, char *filename)
{
	char nbuf[1024];
	char *p, *s, *r;
	int  tonecount;
	int  selectflag;
	int  i, j;
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

		st = read_binary_file(filename, FORM_MDX);
		if(st) return st;

		// make file memo to nbuf
		memset(nbuf, 0, (int)sizeof(nbuf));
		sprintf(nbuf, "%s %s ", sfilename,
                                (char*)MakeStrDateTime(date));
	}

	// set select flag
	if(config.auto_select.mdx) selectflag = MAINFLAG_SELECT;
	else                       selectflag = 0;

	p = filebuf;
	r = filebuf + filesize;  // r: bottom of mdx data

    // add mdx title to filememo
	{
		int bufsize;
		int len;

		bufsize = (int)sizeof(nbuf);
		len = strlen((char*)nbuf);

		// get mdx title to nbuf
		while(len<bufsize && p<r) {
			if(0x0D == (nbuf[len++] = *p++)) {
				len--;
				break;
			}
		}
		if(len<bufsize && nbuf[len]==0x0D) nbuf[len] = '\0';
	}

	// skip PCM file name
	while(p<r && *p++);

	// p -> BASE POINT (offset table top)
	// table: tone(2)+MML.A-H(2*8)+PCM(2) = 10 offsets = 20 bytes
	if((r-p) < 2*10) {
		MFREE(filebuf);
		return FERRCODE_ILLEGAL_MDXFILE;
	}
	{
		char *base = p;
		int   tone_offset = (U8(base)<<8) + U8(base+1);
		char *tone_end;
		int   k;

		// range check all 10 offsets
		for(k=0; k<10; k++) {
			int offset = (U8(base+k*2)<<8) + U8(base+k*2+1);
			if(!(2*10 <= offset && (base+offset) < (r-2))) {
				MFREE(filebuf);
				return FERRCODE_ILLEGAL_MDXFILE;
			}
		}
		// tone data end = nearest offset after tone_offset (else r)
		tone_end = r;
		for(k=1; k<10; k++) {
			int offset = (U8(base+k*2)<<8) + U8(base+k*2+1);
			if(tone_offset < offset && (base+offset) < tone_end)
				tone_end = base + offset;
		}
		s = base + tone_offset;  // top of tone data
		r = tone_end;            // narrow to tone data area
	}

	// no tone data?
	if(s==r) {
		MFREE(filebuf);
		C_MES(C33, MES_NO_TONE_DATA_MDXFILE, MESTIME_NORMAL);
		return 0;
	}

	// check tone data area
	if(s>r) {
		MFREE(filebuf);
		return FERRCODE_ILLEGAL_MDXFILE;
	}

	// calc tone count
	tonecount = (r-s)/27;
	if((r-s)%27) {
		MFREE(filebuf);
		return FERRCODE_ILLEGAL_MDXFILE;
	}

	// clip tonecount
	if(tonecount > TONENUM) tonecount = TONENUM;

	// init tone data
	InitToneData(tan);

    // init oeddata
	memset(&oeddata[tan], 0, sizeof(oeddata[tan]));

	// copy nbuf to file memo
	memcpy(oeddata[tan].filememo, nbuf, sizeof(oeddata[tan].filememo));

	for(i=0; i<tonecount; i++) {
		// set tone name
		memset(Tone[tan][i].name, 0, (int)sizeof(Tone[tan][i].name));
		sprintf(Tone[tan][i].name, "@%d", U8(s));
		// set tone memo
		memset(Tone[tan][i].memo, 0, (int)sizeof(Tone[tan][i].memo));
		sprintf(Tone[tan][i].memo, "%s %s", sfilename, Tone[tan][i].name);

		unpack_mdx_tone(&Tone[tan][i], s);
		s += 27;

		Tone[tan][i].chmode = DEFAULT_OPM_CHMODE;
		Tone[tan][i].chflag = config.opm_channel.mdx;
		Tone[tan][i].volume = DEFAULT_OPM_VOLUME;

		// init. select flag
		flag_select_init_main(tan, i, selectflag);
	}

    oeddata[tan].tonecount = i;
	oeddata[tan].tonesize = sizeof(OPMDATA);

	MFREE(filebuf);
	return 0;
}

