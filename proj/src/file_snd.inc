//
// file_snd.inc
//    SND format I/O. Included from file.c.
//
//    SND is the voice format of SOUND PRO-68K, the X68000 tool
//    shipped by SHARP. Both directions are supported, because
//    interoperating with that tool requires writing the same
//    format back.
//

//
// SND to tone data
//
static int snd2tone(int tan, char *filename)
{
	SNDFORM	*pt;
	int  selectflag;
	int	 i, j;
	char *sfilename;

	sfilename = (char*)MakeStrFilename(filename);

	// get data then make file memo
	{
		int fn;
		int date;

		// get date status
		if(0 > (fn = OPEN(filename, 0x000))) return FERRCODE_CANNOT_OPEN_FILE;
		date = FILEDATE(fn, 0);
		CLOSE(fn);

		// make file memo
		memset(oeddata[tan].filememo, 0, sizeof(oeddata[tan].filememo));
		sprintf((char*)oeddata[tan].filememo, "%s %s",
	                                          sfilename,
											  (char*)MakeStrDateTime(date));
	}

	// read file
	{
		int st;

		st = read_binary_file(filename, FORM_SND);
		if(st) return st;
	}

	// init tone data
	InitToneData(tan);

	// set select flag
	if(config.auto_select.snd) selectflag = MAINFLAG_SELECT;
	else                       selectflag = 0;

	pt = (SNDFORM *)filebuf;


	{
		int tonecount;
		int namesize;

		// set tonecount
		tonecount = SND_TONENUM;
		if(tonecount > TONENUM) tonecount = TONENUM;

		// clear tone name
		namesize = (int)sizeof(Tone[tan][0].name);
		if(namesize > SND_TONENAME_SIZE) {
			for(i=0; i<tonecount; i++) {
				memset(Tone[tan][i].name, 0, namesize);
			}
		}

		//
		// main data
		//
		for(i=0; i < tonecount; i++) {
			// set tone name
			memset(Tone[tan][i].name, 0, (int)sizeof(Tone[tan][i].name));
			memcpy(Tone[tan][i].name, pt->name, SND_TONENAME_SIZE);
			// set tone memo
			memset(Tone[tan][i].memo, 0, (int)sizeof(Tone[tan][i].memo));
			sprintf(Tone[tan][i].memo, "%s @%d %s", sfilename, i+1, Tone[tan][i].name);

			Tone[tan][i].con      = pt->reg0[45];
			Tone[tan][i].feedback = pt->reg0[44];
			Tone[tan][i].slotmask = pt->reg1[1];
			Tone[tan][i].pan      = pt->reg1[2];
			Tone[tan][i].pms      = pt->reg0[48];
			Tone[tan][i].ams      = pt->reg0[49];
			Tone[tan][i].sync     = pt->reg1[0];
			Tone[tan][i].waveform = pt->reg0[46];
			Tone[tan][i].lfreq    = pt->reg0[47];
			Tone[tan][i].pmd      = pt->reg0[50];
			Tone[tan][i].amd      = pt->reg0[51];
			Tone[tan][i].noise    = DEFAULT_OPM_NOISE;
			Tone[tan][i].nfreq    = DEFAULT_OPM_NFREQ;
			Tone[tan][i].dummy    = DEFAULT_OPM_DUMMY;

			for( j = 0; j < 4; j++ ) {
				Tone[tan][i].reg[j].ar   = pt->reg0[   j];
				Tone[tan][i].reg[j].d1r  = pt->reg0[ 4+j];
				Tone[tan][i].reg[j].d2r  = pt->reg0[ 8+j];
				Tone[tan][i].reg[j].rr   = pt->reg0[12+j];
				Tone[tan][i].reg[j].d1l  = pt->reg0[16+j];
				Tone[tan][i].reg[j].tl   = pt->reg0[20+j];
				Tone[tan][i].reg[j].ks   = pt->reg0[24+j];
				Tone[tan][i].reg[j].mul  = pt->reg0[28+j];
				Tone[tan][i].reg[j].dt1  = pt->reg0[32+j];
				Tone[tan][i].reg[j].dt2  = pt->reg0[36+j];
				Tone[tan][i].reg[j].amse = pt->reg0[40+j];
				Tone[tan][i].reg[j].dummy = DEFAULT_OPM_DUMMY;
			}
			Tone[tan][i].chmode = DEFAULT_OPM_CHMODE;
			Tone[tan][i].chflag = config.opm_channel.snd;
			Tone[tan][i].volume = DEFAULT_OPM_VOLUME;

			// init. select flag
			flag_select_init_main(tan, i, selectflag);

			pt++;
		}
	}

	MFREE(filebuf);
	return 0;
}

//
// tone data to SND
//
static int tone2snd(int tan, char *filename)
{
	SNDFORM p;
	int tonecount;
	int wcount;
	int i, j;

	if(wopen(filename))
		return FERRCODE_CANNOT_CREATE_FILE;

    // upper limit
    tonecount = SND_TONENUM;
	if(tonecount > TONENUM) tonecount = TONENUM;

	wcount = (int)sizeof(SNDFORM);
	for(i=0; i < tonecount; i++) {
        // clear SNDFORM p
        memset(&p, 0, sizeof(SNDFORM));

		// set tone name
		strncpy(p.name, Tone[tan][i].name, SND_TONENAME_SIZE);

		// set OPM param.
		p.reg0[45] = Tone[tan][i].con;
		p.reg0[44] = Tone[tan][i].feedback;
		p.reg1[1]  = Tone[tan][i].slotmask;
		p.reg1[2]  = Tone[tan][i].pan;
		p.reg0[48] = Tone[tan][i].pms;
		p.reg0[49] = Tone[tan][i].ams;
		p.reg1[0]  = Tone[tan][i].sync;
		p.reg0[46] = Tone[tan][i].waveform;
		p.reg0[47] = Tone[tan][i].lfreq;
		p.reg0[50] = Tone[tan][i].pmd;
		p.reg0[51] = Tone[tan][i].amd;

		for(j=0; j<4; j++) {
			p.reg0[   j] = Tone[tan][i].reg[j].ar;
			p.reg0[ 4+j] = Tone[tan][i].reg[j].d1r;
			p.reg0[ 8+j] = Tone[tan][i].reg[j].d2r;
			p.reg0[12+j] = Tone[tan][i].reg[j].rr;
			p.reg0[16+j] = Tone[tan][i].reg[j].d1l;
			p.reg0[20+j] = Tone[tan][i].reg[j].tl;
			p.reg0[24+j] = Tone[tan][i].reg[j].ks;
			p.reg0[28+j] = Tone[tan][i].reg[j].mul;
			p.reg0[32+j] = Tone[tan][i].reg[j].dt1;
			p.reg0[36+j] = Tone[tan][i].reg[j].dt2;
			p.reg0[40+j] = Tone[tan][i].reg[j].amse;
		}

		if(wbytes(&p, wcount)) {
			wclose();
			return FERRCODE_DISK_FULL;
		}
	}

	wclose();
	return 0;
}

