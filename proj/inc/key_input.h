//
// key_input.h
//

//
// BITSNS_MAP
//
//   [31:24] : bitsns group
//   [23:16] : bitsns value
//   [15]    : mouse left button
//   [14]    : mouse right button
//   [13:8]  : group 0xE check value
//   [7:0]   : ASCII code
//

#define BITSNS_MAP_ESC	  (0x00020000 | 0x1B)   // ESC (cancel)
#define BITSNS_MAP_S      (0x03800000 | 'S')    // Save
#define BITSNS_MAP_L      (0x04400000 | 'L')    // Load
#define BITSNS_MAP_R      (0x02100000 | 'R')    // Rename
#define BITSNS_MAP_M      (0x06010000 | 'M')    // File Memo
#define BITSNS_MAP_LOCK   (0x00040100 | '!')    // Lock (SHIFT+1)

#define BITSNS_MAP_SPACE  (0x06200000 | ' ')    // SPACE key
#define BITSNS_MAP_CR     (0x03200000 | 0x0D)   // ENTER key (full key)
#define BITSNS_MAP_ENTER  (0x09400000 | 0x0D)   // ENTER key (ten key)
#define BITSNS_MAP_MLEFT  (0x00008000)        // left click (mouse)
#define BITSNS_MAP_MRIGHT (0x00004000)        // right click (mouse)

extern int keyin_wait(void);

enum BITSNS_KEYMAP {
	KEYMAP_FILEOP = 0,
	KEYMAP_EXDISP
};
