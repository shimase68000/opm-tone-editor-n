//
//	mouse routine
//

#include	<stdio.h>
#include	<iocslib.h>
#include    "_debug_.h"
#include	"oe.h"
#include	"ms_map.h"
#include	"disp.h"

#define PALETTENO_MOUSE  (8)

extern uchar ms_map[96*32];
extern int   scalekey_available;

extern void	mev_ToneSelect(int,int,int,int,int *,int *,int);
extern void	mev_ToneMark(int,int,int,int,int *,int *,int);
extern void	mev_OPMChSelect(int,int,int,int,int *,int *,int);
extern void	mev_ChVolume(int,int,int,int,int *,int *,int);
extern void	mev_MasterVolume(int,int,int,int,int *,int *,int);
extern void	mev_ExecMenu(int,int,int,int,int *,int *);
extern void	mev_OPMCopy(int,int,int,int,int *,int *,int);
extern void	mev_OPMCopyUNDO(int,int,int,int,int *,int *,int);
extern void mev_FileOperation(int,int,int *);
extern void mev_ScalekeyOffset(int,int,int *,int *);
extern void mev_ScalekeyPlaymode(int,int,int *,int *,int);
extern void mev_ScalekeyMIDIChannelFilter(int,int,int *,int *);
extern void mev_ScalekeySelectPolicy(int,int,int *,int *);
extern void	mev_ScalekeyOPMChSelect(int,int,int,int,int *,int *);
extern void mev_ScalekeyDelayCount(int,int,int *,int *);
extern void mev_ScalekeyStartChannel(int,int,int *,int *);
extern void	mev_ScalekeyDetune(int,int,int,int,int *,int *,int);
extern void mev_ToneCopy(int,int,int,int,int,int *,int *,int);
extern void tonecopy_disarm(void);
extern int  tonecopy_is_armed(void);
extern void	C_FILENAME(int,int);

void ms_init(void);
void ms_exit(void);
int	 ms_routine(void);
void ms_color(int);

int  mes_count = -1;
int  map_value = 0;

//
// init mouse
//
void ms_init(void)
{
	// init mouse
	MS_INIT();
	// not displayed software keyboard
	SKEY_MOD(0,0,0);
	// disp mouse cursor
	MS_CURON();
}

//
// exit mouse
//
void ms_exit(void)
{
	// init mouse
	MS_INIT();
	// not displayed mouse cursor
	MS_CUROF();
}

//
// mouse routine
//
int ms_routine(void)
{
	static int sbl = 0, sbr = 0;

	int bsCTRL;
	int mx, my, bl, br;
	int msdt, mscur;

	msdt  = MS_GETDT();   // bit15-8: left button, bit7-0: right button
	mscur = MS_CURGT();   // bit31-16: x, bit15-0: y

	mx = mscur >> 16;
	my = mscur & 0xffff;
	bl = (msdt & 0xff00) >> 8;   // on:0xff  off:0x00
	br =  msdt & 0x00ff;         // on:0xff  off:0x00

    bsCTRL = BITSNS(0x0E) & 2;
	map_value = ms_map[mx/8+(my/16)*96];

	if(bl || br) {
		// Every case except MAP_TONETABLE0/1/2 calls tonecopy_disarm():
		// clicking anywhere other than a tone list cancels a pending copy.
		// Do not forget it when adding a case.
		switch(map_value) {
			case MAP_NONE:
				tonecopy_disarm();
				break;

			case MAP_TONETABLE0:
			case MAP_TONETABLE1:
			case MAP_TONETABLE2:
					if(bsCTRL) {
						mev_ToneCopy(TONECOPY_SET, mx, my, bl, br, &sbl, &sbr, map_value);
					} else if(tonecopy_is_armed()) {
						if(bl)      mev_ToneCopy(TONECOPY_COPY,  mx, my, bl, br, &sbl, &sbr, map_value);
						else if(br) mev_ToneCopy(TONECOPY_CLEAR, mx, my, bl, br, &sbl, &sbr, map_value);
					} else {
						mev_ToneSelect(mx, my, bl, br, &sbl, &sbr, map_value);
					}
					break;

			case MAP_TONEMARK0:
			case MAP_TONEMARK1:
			case MAP_TONEMARK2:
				mev_ToneMark(mx, my, bl, br, &sbl, &sbr, map_value);
			    tonecopy_disarm();
				break;

			case MAP_CHANNEL0:
			case MAP_CHANNEL1:
				mev_OPMChSelect(mx, my, bl, br, &sbl, &sbr, map_value);
			    tonecopy_disarm();
				break;

			case MAP_CHANNELVOL0:
			case MAP_CHANNELVOL1:
				mev_ChVolume(mx, my, bl, br, &sbl, &sbr, map_value);
			    tonecopy_disarm();
				break;

			case MAP_MASTERVOL0:
			case MAP_MASTERVOL1:
				mev_MasterVolume(mx, my, bl, br, &sbl, &sbr, map_value);
			    tonecopy_disarm();
				break;

			case MAP_EXECMENU:
				mev_ExecMenu(mx, my, bl, br, &sbl, &sbr);
			    tonecopy_disarm();
				break;

			case MAP_OPMCOPY0:
			case MAP_OPMCOPY1:
				if(bsCTRL && !tonecopy_is_armed())
					mev_OPMCopy(mx, my, bl, br, &sbl, &sbr, map_value);
				tonecopy_disarm();
				break;

			case MAP_OPMCOPY_UNDO0:
			case MAP_OPMCOPY_UNDO1:
				mev_OPMCopyUNDO(mx, my, bl, br, &sbl, &sbr, map_value);
			    tonecopy_disarm();
				break;

		    case MAP_FILENAME0:
				mev_FileOperation(0, bl, &sbl);
			    tonecopy_disarm();
				C_FILENAME(0,0);
				break;

			case MAP_FILENAME1:
				mev_FileOperation(1, bl, &sbl);
			    tonecopy_disarm();
				C_FILENAME(1,0);
				break;

            case MAP_SCALEKEY_KEYOFFSET:
			    mev_ScalekeyOffset(bl, br, &sbl, &sbr);
			    tonecopy_disarm();
				break;

            case MAP_SCALEKEY_DELAY_COUNT:
			    mev_ScalekeyDelayCount(bl, br, &sbl, &sbr);
			    tonecopy_disarm();
				break;

            case MAP_SCALEKEY_START_CHANNEL:
			    mev_ScalekeyStartChannel(bl, br, &sbl, &sbr);
			    tonecopy_disarm();
				break;

			case MAP_SCALEKEY_UNISON:
			    mev_ScalekeyPlaymode(bl, br, &sbl, &sbr, MAP_SCALEKEY_UNISON);
			    tonecopy_disarm();
				break;

			case MAP_SCALEKEY_POLYPHONIC:
			    mev_ScalekeyPlaymode(bl, br, &sbl, &sbr, MAP_SCALEKEY_POLYPHONIC);
			    tonecopy_disarm();
				break;

			case MAP_SCALEKEY_ASSIGN_POLICY:
			    mev_ScalekeySelectPolicy(bl, br, &sbl, &sbr);
			    tonecopy_disarm();
				break;

			case MAP_SCALEKEY_MIDI_CHANNEL:
			    mev_ScalekeyMIDIChannelFilter(bl, br, &sbl, &sbr);
			    tonecopy_disarm();
				break;

			// Only these two are gated on scalekey_available.
			//   They drive the OPM directly through scalekey, whereas the
			//   other scalekey settings are held by the tone editor and only
			//   reach the OPM after further processing.
			//   They also sit in the "playing state" block on screen; the rest
			//   form one settings group, which would become unreadable if it
			//   disappeared whenever scalekey is absent.
			case MAP_SCALEKEY_OPMCHANNEL:
				if(!scalekey_available) break;
				mev_ScalekeyOPMChSelect(mx, my, bl, br, &sbl, &sbr);
			    tonecopy_disarm();
				break;

			case MAP_SCALEKEY_DETUNE:
				if(!scalekey_available) break;
				mev_ScalekeyDetune(mx, my, bl, br, &sbl, &sbr, map_value);
			    tonecopy_disarm();
				break;
		}
	}

	if(!bl) sbl = 0;
	if(!br) sbr = 0;

	return 1;
}

//
// mouse color (for tone copy)
//
void ms_color(int c)
{
	static int	pre_c = 0;

	if((pre_c>0) == (c>0)) return;
	pre_c = c;

	if(c) TPALET(PALETTENO_MOUSE, ~TPALET(PALETTENO_MOUSE, -1));
	else  TPALET(PALETTENO_MOUSE, -2);
}
