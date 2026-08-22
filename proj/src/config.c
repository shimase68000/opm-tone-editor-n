//
// config.c
//

#include   <stdio.h>
#include   <stdlib.h>
#include   <string.h>
#include   <iocslib.h>
#include   <doslib.h>
#include   "_debug_.h"
#include   "oe.h"
#include   "json.h"
#include   "mylib.h"

extern void mev_exec_command_pre(char *);
extern int  recalc_effective_count(int,int);

extern int  auto_readonly;
extern int  scalekey_channel_assign_policy;
extern int  scalekey_opm_channel;
extern int  user_max_poly_count;
extern int  user_max_unison_count;
extern int  scalekey_poly_count;
extern int  scalekey_unison_count;

//
// set config value
//
void set_config_value(CONFIG *cfg)
{
	// init. exec_command name
	mev_exec_command_pre(cfg->exec_command);

	// set auto_readonly
	auto_readonly = cfg->auto_readonly ? 1 : 0;

	// init scalekey channel_assign_policy
	scalekey_channel_assign_policy = cfg->scalekey.channel_assign_policy;

	// init scalekey opm_channel
	scalekey_opm_channel = cfg->scalekey.opm_channel;

	// calc. poly count
	recalc_effective_count(scalekey_opm_channel, RECALC_UNISON_PRIORITY);

	// init scalekey poly_count
	scalekey_poly_count = cfg->scalekey.polyphony;
	user_max_poly_count = cfg->scalekey.polyphony;

	// init scalekey unison_count
	scalekey_unison_count = cfg->scalekey.unison;
	user_max_unison_count = cfg->scalekey.unison;

	return;
}
