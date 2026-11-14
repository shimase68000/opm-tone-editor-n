//
// file_oed.inc
//    OED format I/O. Included from file.c.
//
//    OED is this editor's own format. It holds both tone blocks
//    (main and template) and the editor state, so it is the only
//    format that reads and writes everything.
//

//
// tone data to OED
// 	  (OED file format v000)
//
static int tone2oed(int tan, char *filename)
{
	char fbuf[1024];
	int  tonecount;
	int  tonesize;
	int  wcount;
	int  i, j;

	// open file (doslib CREATE)
	if(wopen(filename))
		return FERRCODE_CANNOT_CREATE_FILE;

    // set OED_EX oedexbuf
	set_oedexbuf();

	// write OED file header
	wbytes(OED_FILEHEADER_SJIS, OED_FILEHEADER_SIZE);

	// write OED file memo
	memcpy(fbuf, oeddata[tan].filememo, (int)sizeof(oeddata[tan].filememo));
	wbytes(fbuf, (int)sizeof(oeddata[tan].filememo));

    // write OED_EX data
    wbytes(&oedexbuf, (int)sizeof(OED_EX));

	// write channel volume (8ch x 2)
	{
		char vbuf[16];
		int  k = 0;
		for(i=0; i<2; i++)
			for(j=0; j<8; j++) vbuf[k++] = (char)Vol[i][j];
		wbytes(vbuf, 16);
	}

    // write reserve (32byte)
	memset(fbuf, 0, OED_RESERVE0_SIZE);
	wbytes(fbuf, OED_RESERVE0_SIZE);

	// write tone count (TONENUM)
	tonecount = TONENUM;
	wbytes(&tonecount, (int)sizeof(tonecount));

	// wite tone data size (= OPMDATA)
	tonesize = (int)sizeof(OPMDATA);
	wbytes(&tonesize, (int)sizeof(tonesize));

    // write reserve (8byte)
	memset(fbuf, 0, OED_RESERVE1_SIZE);
	wbytes(fbuf, OED_RESERVE1_SIZE);

	// write tone data
	wcount = (int)sizeof(OPMDATA);
	for(i=0; i<tonecount; i++) {
		if(wbytes(&Tone[tan][i], wcount)) {
			wclose();
			return FERRCODE_DISK_FULL;
		}
	}

	// write tone count for template (= TONENUM2)
	tonecount = TONENUM2;
	wbytes(&tonecount, (int)sizeof(tonecount));

	// write tone data size for template (= sizeof(OPMDATA))
	tonesize = (int)sizeof(OPMDATA);
	wbytes(&tonesize, (int)sizeof(tonesize));

    // write reserve1 (8byte)
	memset(fbuf, 0, OED_RESERVE1_SIZE);
	wbytes(fbuf, OED_RESERVE1_SIZE);

	// write template date
	wcount = (int)sizeof(OPMDATA);
	for(i=0; i<tonecount; i++) {
		if(wbytes(&Tone[2][i], wcount)) {
			wclose();
			return FERRCODE_DISK_FULL;
		}
	}

	wclose();
	return 0;
}

//
// OED to tone data
//
static int oed2tone(int tan, char *filename)
{
	int	 i, j;
	char *pt_s;

	// file read
	{
		int st = read_binary_file(filename, FORM_OED);
		if(st) return st;
	}

	// validate structure before touching tone data (fail fast)
	{
		int vstat = oed_validate(filebuf, filesize);
		if(vstat) {MFREE(filebuf); return vstat;}
	}

	// initialize
	{
		// init tone data
		InitToneData(tan);

		// set pt_s
		pt_s = filebuf;

		// init oeddata
		memset(&oeddata[tan], 0, sizeof(oeddata[tan]));
	}

	// read header
	{
		// read file memo
		memcpy(oeddata[tan].filememo, pt_s, OED_FILEMEMO_SIZE);
		pt_s += OED_FILEMEMO_SIZE;

		// read OED_EX data
		memcpy(&oeddata[tan].oedex, pt_s, sizeof(OED_EX));
		pt_s += sizeof(OED_EX);

		// read channel volume ( 8ch x 2 )
		for(j=0; j<2; j++) {
			for(i=0; i<8; i++) {
				oeddata[tan].channelvol[j][i] = *pt_s++;
				Vol[j][i] = (int)oeddata[tan].channelvol[j][i];
			}
		}

		// skip reserve0 area (32byte)
		pt_s += OED_RESERVE0_SIZE;
	}

	//
	// for main data
	//
	{
		int tonecount;
		int tonesize;

		// read tonecount/tonesize
		memcpy(&tonecount, pt_s, sizeof(tonecount));
		oeddata[tan].tonecount = tonecount;
		pt_s += sizeof(tonecount);

		memcpy(&tonesize, pt_s, sizeof(tonesize));
		oeddata[tan].tonesize  = tonesize;
		pt_s += sizeof(tonesize);

		// skip reserve1 area (8byte)
		pt_s += OED_RESERVE1_SIZE;

		// read tone data
		for(i=0; i<tonecount; i++) {
			if(i >= TONENUM) {
				pt_s += sizeof(OPMDATA);
				continue;
			}
			memcpy(&Tone[tan][i], pt_s, sizeof(OPMDATA));
			pt_s += sizeof(OPMDATA);
		}

		// (main slot edit flag is cleared by FileLoad(), for all formats)
	}

	//
	// for template data
	//
	{
		int tonecount;
		int tonesize;

		// read tonecount/tonesize
		memcpy(&tonecount, pt_s, sizeof(tonecount));
		oeddata[2].tonecount = tonecount;
		pt_s += sizeof(tonecount);

		memcpy(&tonesize, pt_s, sizeof(tonesize));
		oeddata[2].tonesize  = tonesize;
		pt_s += sizeof(tonesize);

		// skip reserve area (8byte)
		pt_s += OED_RESERVE1_SIZE;

		// confirm reading template data
		if(conf_read_tempdata(filename)) {
			// read template tone data
			for(i=0; i<tonecount; i++) {
				if(i >= TONENUM2) break;
				memcpy(&Tone[2][i], pt_s, sizeof(OPMDATA));
				pt_s += sizeof(OPMDATA);
			}
			// clear edit flag (template slot)
			//   Cleared here, not in FileLoad(), because it depends on the
			//   answer above.
			proc_editflag(CLEAR_EDITFLAG|EDIT_FILE_LOAD2, 2, 0);
		 }
	}

	MFREE(filebuf);
	return 0;
}
