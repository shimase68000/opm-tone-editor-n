//
// file_opm.inc
//    OPM format I/O. Included from file.c.
//
//    Text format used by OPMDRV*. Both directions are
//    supported.
//

//
// OPM to tone data
//
static int opm2tone(int tan, char *filename, int form)
{
	int	 i, p;
	int	 j;
	int	 d[11*5+2], dp;
	int	 st;
	int  len;
	int  c;
	char buf[READBUF];
	char *ad;
	int  fn, date;
	int  chflag;
	int  selectflag;
	char *sfilename;

	sfilename = (char*)MakeStrFilename(filename);

    // init oeddata
	memset(&oeddata[tan], 0, sizeof(oeddata[tan]));

	// get date status then make file memo
	{
		// get date status
		if(0 > (fn = OPEN(filename, 0x000))) return FERRCODE_CANNOT_OPEN_FILE;
		date = FILEDATE(fn, 0);
		CLOSE(fn);

		// make file memo
		memset(&oeddata[tan].filememo, 0, sizeof(oeddata[tan].filememo));
		sprintf((char*)oeddata[tan].filememo, "%s %s",
	                                          sfilename,
								              (char*)MakeStrDateTime(date));
	}

	// open file (text mode)
	if(ropen(filename)) return FERRCODE_CANNOT_OPEN_FILE;

	// init tone data
	InitToneData(tan);

	// chflag & select flag (OPM/ZMS)
	{
		if(form == FORM_OPM) {
			chflag = config.opm_channel.opm;
			if(config.auto_select.opm) selectflag = MAINFLAG_SELECT;
			else                       selectflag = 0;
		}
		else {
			chflag = config.opm_channel.zms;
			if(config.auto_select.zms) selectflag = MAINFLAG_SELECT;
			else                       selectflag = 0;
		}
	}

	//
	// main data
	//
	i  = 0;	// OED tone number
	st = 0; // state

	while(i<TONENUM && (ad = my_fgets(buf, READBUF)) != NULL) {
		len = strlen(ad);
		if(!len) continue;

		p = 0; // index of buf

		switch(st) {
			case 0:
				// state 0: get tone number
				if(0 > (c = getfc(buf, &p, len))) break;
				if(c != '(') break;

				if(0 > (c = getfc(buf, &p, len))) break;
				if(my_tolower(c) != 'v') break;

				st = 1;
				dp = 0; // index of MML tone param

				/* fallthrough - tone data may begin on this same line */

			case 1:
				// state 1: get tone data
				while(0 <= (d[dp] = getn(buf, &p, len))) {
					dp++;
					if(dp < 57) continue;

					// set tone name
                    memset(Tone[tan][i].name, 0, (int)sizeof(Tone[tan][i].name));
					sprintf(Tone[tan][i].name, "v%d", d[0]);

					// set tone memo
					memset(Tone[tan][i].memo, 0, (int)sizeof(Tone[tan][i].memo));
					sprintf(Tone[tan][i].memo, "%s %s", sfilename, Tone[tan][i].name);

					dp = 2;
					Tone[tan][i].con      = d[dp] & 0x7;
					Tone[tan][i].feedback = d[dp++] >> 3;
					Tone[tan][i].slotmask = d[dp++];
					Tone[tan][i].waveform = d[dp++];
					Tone[tan][i].sync     = d[dp++];
					Tone[tan][i].lfreq    = d[dp++];
					Tone[tan][i].pmd      = d[dp++];
					Tone[tan][i].amd      = d[dp++];
					Tone[tan][i].pms      = d[dp++];
					Tone[tan][i].ams      = d[dp++];
					Tone[tan][i].pan      = d[dp++];
					dp++;
					for(j=0; j<4; j++) {
						Tone[tan][i].reg[j].ar   = d[dp++];
						Tone[tan][i].reg[j].d1r  = d[dp++];
						Tone[tan][i].reg[j].d2r  = d[dp++];
						Tone[tan][i].reg[j].rr   = d[dp++];
						Tone[tan][i].reg[j].d1l  = d[dp++];
						Tone[tan][i].reg[j].tl   = d[dp++];
						Tone[tan][i].reg[j].ks   = d[dp++];
						Tone[tan][i].reg[j].mul  = d[dp++];
						Tone[tan][i].reg[j].dt1  = d[dp++];
						Tone[tan][i].reg[j].dt2  = d[dp++];
						Tone[tan][i].reg[j].amse = d[dp++];
					}
					Tone[tan][i].chmode = DEFAULT_OPM_CHMODE;
					Tone[tan][i].chflag = chflag;
					Tone[tan][i].volume = DEFAULT_OPM_VOLUME;

					// init. selct flag
					flag_select_init_main(tan, i, selectflag);

					st = 0;
					i++;
					break;
				}
		}
	}

    oeddata[tan].tonecount = i;
	oeddata[tan].tonesize = sizeof(OPMDATA);

	rclose();
	return 0;
}

//
// tone data to OPM
//
static int tone2opm(int tan, char *filename)
{
	int	 i, j;
	char buf[256];

	if(wopen(filename))
		return FERRCODE_CANNOT_CREATE_FILE;

	sprintf(buf, MML_FILECOMMENT, OE_MYNAME, OE_VERSION, OE_SUBVERSION);
	strcat (buf, "\n");
	wputs(buf);

	// output tone data
	for(i=0; i<TONENUM; i++) {
		// check select flag
		if(!stat_selectflag(STAT_TONE, tan, i)) continue;

		sprintf(buf, "\n/* %s\n", Tone[tan][i].name);
		wputs(buf);

		sprintf(buf, "(v%d,0\n", i+config.offset_export_tone_number+1);
		wputs(buf);

		wputs("/*    F/A  SM  WF  SY  SP PMD AMD PMS AMS  LR\n");
		sprintf(buf, "      %3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,  0\n",
				(Tone[tan][i].feedback << 3) + Tone[tan][i].con,
				Tone[tan][i].slotmask,
				Tone[tan][i].waveform,
				Tone[tan][i].sync,
				Tone[tan][i].lfreq,
				Tone[tan][i].pmd,
				Tone[tan][i].amd,
				Tone[tan][i].pms,
				Tone[tan][i].ams,
				Tone[tan][i].pan);
		wputs(buf);

		wputs("/*     AR D1R D2R  RR D1L  TL  KS MUL DT1 DT2 AME\n");
		for(j=0; j<4; j++) {
			sprintf(buf, "      %3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d\n",
				Tone[tan][i].reg[j].ar,
				Tone[tan][i].reg[j].d1r,
				Tone[tan][i].reg[j].d2r,
				Tone[tan][i].reg[j].rr,
				Tone[tan][i].reg[j].d1l,
				Tone[tan][i].reg[j].tl,
				Tone[tan][i].reg[j].ks,
				Tone[tan][i].reg[j].mul,
				Tone[tan][i].reg[j].dt1,
				Tone[tan][i].reg[j].dt2,
				Tone[tan][i].reg[j].amse);
			wputs(buf);
		}
		if(wputs(")\n")) {
			wclose();
			return FERRCODE_DISK_FULL;
		}
	}

	// output local tone data
	for(i=0; i<TONENUM2; i++) {
		// check select flag
		if(!stat_selectflag(STAT_TONE, 2, i)) continue;

		sprintf(buf, "\n/* %s\n", Tone[2][i].name);
		wputs(buf);

		sprintf(buf, "(v%d,0\n", i+config.offset_export_template_number+1);
		wputs(buf);

		wputs("/*    F/A  SM  WF  SY  SP PMD AMD PMS AMS  LR\n");
		sprintf(buf, "      %3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,  0\n",
				(Tone[2][i].feedback << 3) + Tone[2][i].con,
				Tone[2][i].slotmask,
				Tone[2][i].waveform,
				Tone[2][i].sync,
				Tone[2][i].lfreq,
				Tone[2][i].pmd,
				Tone[2][i].amd,
				Tone[2][i].pms,
				Tone[2][i].ams,
				Tone[2][i].pan);
		wputs(buf);

		wputs("/*     AR D1R D2R  RR D1L  TL  KS MUL DT1 DT2 AME\n");
		for(j=0; j<4; j++) {
			sprintf(buf, "      %3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d\n",
				Tone[2][i].reg[j].ar,
				Tone[2][i].reg[j].d1r,
				Tone[2][i].reg[j].d2r,
				Tone[2][i].reg[j].rr,
				Tone[2][i].reg[j].d1l,
				Tone[2][i].reg[j].tl,
				Tone[2][i].reg[j].ks,
				Tone[2][i].reg[j].mul,
				Tone[2][i].reg[j].dt1,
				Tone[2][i].reg[j].dt2,
				Tone[2][i].reg[j].amse);
			wputs(buf);
		}
		if(wputs(")\n")) {
			wclose();
			return FERRCODE_DISK_FULL;
		}
	}
	wclose();
	return 0;
}

