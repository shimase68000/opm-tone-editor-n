//
// file_mml.inc
//    MML format I/O. Included from file.c.
//
//    Tone definitions in MDX MML source. Both directions are
//    supported; the editor writes the readable form of a tone bank.
//

//
// mml to tone data
//   form (FORM_MML / FORM_MUS) is unused: both are parsed identically.
//
static int mml2tone(int tan, char *filename, int form)
{
	int	i, n, p;
	int	j;
	int	st;
	int len;
	int	d[47], dp;
	int selectflag;
	int c;

	char  buf[READBUF];
	char *ad;
	char *sfilename;

	(void)form;

	sfilename = (char*)MakeStrFilename(filename);

    // init oeddata
	memset(&oeddata[tan], 0, (int)sizeof(oeddata[tan]));

	// get date status then make file memo
	{
		int fn;
		int date;

		if(0 > (fn = OPEN(filename, 0x000))) return FERRCODE_CANNOT_OPEN_FILE;
		date = FILEDATE(fn, 0);
		CLOSE(fn);

		// make file memo
		memset(&oeddata[tan].filememo, 0, (int)sizeof(oeddata[tan].filememo));
		sprintf((char*)oeddata[tan].filememo, "%s %s",
	                                          sfilename,
								              (char*)MakeStrDateTime(date));
	}

	// open file (text mode)
	if(ropen(filename)) return FERRCODE_CANNOT_OPEN_FILE;

	// init tone data
	InitToneData(tan);

	// select flag
	if(config.auto_select.mml) selectflag = MAINFLAG_SELECT;
	else                       selectflag = 0;

	//
	// main data
	//
	i  = 0; // OED tone number
	n  = 0; // MML @ tone number
	st = 0; // state
	while(i<TONENUM && (ad = my_fgets(buf, READBUF)) != NULL) {
		len = strlen(ad);
		if(!len) continue;

		p = 0; // index of buf

		switch(st) {
			case 0:
				// state 0: find a tone definition "@n = {"
				//   - Stop at a comment. tone2mml() writes the tone name as
				//     "/* name */", and a name such as "@12" must not be taken
				//     for a definition.
				//   - "@" followed by a non-digit is a command (@v, @q, ...).
				//   - "@n" not followed by "=" is a tone change in track data.
				for(;;) {
					while(buf[p] && buf[p] != '@' && !is_mml_comment(buf[p])) p++;
					if(buf[p] != '@') break;         // end of line or comment
					p++;
					// "@@n" (tone macro), "@wn" (waveform), "@kn" (tone map)
					// are other definitions, not tone definitions.
					if(buf[p] == '@') { p++; continue; }
					c = my_tolower(buf[p]);
					if(c == 'w' || c == 'k') continue;
					while(buf[p] == ' ' || buf[p] == '\t') p++;   // "@ 16={"
					if(!isdigit(buf[p])) continue;                // command such as @v / @q
					n = getn(buf, &p, len);
					while(buf[p] == ' ' || buf[p] == '\t') p++;
					if(buf[p] == '=') break;                      // found "@n ="
				}
				if(buf[p] != '=') break;

				st = 1;
				dp = 0; // index of MML tone param

				/* fallthrough - tone data may begin on this same line */

			case 1:
				// state 1: get tone data
				while(0 <= (d[dp] = getn(buf, &p, len))) {
					dp++;
					if(dp < 47) continue;

					// set tone name
                    memset(Tone[tan][i].name, 0, (int)sizeof(Tone[tan][i].name));
					sprintf(Tone[tan][i].name, "@%d", n);

					// set tone memo
					memset(Tone[tan][i].memo, 0, (int)sizeof(Tone[tan][i].memo));
					sprintf(Tone[tan][i].memo, "%s %s", sfilename, Tone[tan][i].name);

					dp = 0;
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
					Tone[tan][i].con      = d[dp++];
					Tone[tan][i].feedback = d[dp++];
					Tone[tan][i].slotmask = d[dp++];
					Tone[tan][i].pan      = DEFAULT_OPM_PAN;

					Tone[tan][i].chmode = DEFAULT_OPM_CHMODE;
					Tone[tan][i].chflag = config.opm_channel.mml;
					Tone[tan][i].volume = DEFAULT_OPM_VOLUME;

					// init. tone select flag
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
// tone data to MML
//
static int tone2mml(int tan, char *filename)
{
	char buf[256];
	int	 i, j;

	if(wopen(filename))
		return FERRCODE_CANNOT_CREATE_FILE;

	sprintf(buf, MML_FILECOMMENT, OE_MYNAME, OE_VERSION, OE_SUBVERSION);
	strcat(buf, "\n");
	wputs(buf);

	// output tone data
	for(i=0; i<TONENUM; i++) {
		// check select flag
		if(!stat_selectflag(STAT_TONE, tan, i)) continue;

		sprintf(buf, "\n/* %s */\n", Tone[tan][i].name);
		wputs(buf);

		sprintf(buf, "@%d = {\n", i+config.offset_export_tone_number+1);
		wputs(buf);

		wputs("/*  AR D1R D2R  RR D1L  TL  KS MUL DT1 DT2 AME  */\n");

		for(j=0; j<4; j++) {
			sprintf(buf, "   %3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,\n",
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

		wputs("/* CON  FL  SM */\n");

		sprintf(buf, "   %3d,%3d,%3d\n",
					Tone[tan][i].con,
					Tone[tan][i].feedback,
					Tone[tan][i].slotmask  );
		wputs(buf);
		if(wputs("}\n")) {
			wclose();
			return FERRCODE_DISK_FULL;
		}
	}

	// output template tone data
	for(i=0; i<TONENUM2; i++) {
		// check select flag
		if(!stat_selectflag(STAT_TONE, 2, i)) continue;

		sprintf(buf, "\n/* %s */\n", Tone[2][i].name);
		wputs(buf);

		sprintf(buf, "@%d = {\n", i+config.offset_export_template_number+1);
		wputs(buf);

		wputs("/*  AR D1R D2R  RR D1L  TL  KS MUL DT1 DT2 AME  */\n");

		for(j=0; j<4; j++) {
			sprintf(buf, "   %3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,\n",
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
					Tone[2][i].reg[j].amse );
			wputs(buf);
		}

		wputs("/* CON  FL  SM */\n");

		sprintf(buf, "   %3d,%3d,%3d\n",
					Tone[2][i].con,
					Tone[2][i].feedback,
					Tone[2][i].slotmask  );
		wputs(buf);
		if(wputs("}\n")) {
			wclose();
			return FERRCODE_DISK_FULL;
		}
	}
	wclose();
	return 0;
}

