//
//	mouse map ms_map.c
//

#include	"oe.h"

//
// Mouse hit map: one map value per text cell (96 columns x 32 rows).
//   Zero is MAP_NONE. The map values are written by ms_map_init() in oe.c,
//   derived from the display positions and sizes in disp.h.
//
uchar ms_map[96*32];
