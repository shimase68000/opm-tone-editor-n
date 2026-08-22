//
// _debug_.h
//

//#define _debug_
//#define _debug_test_version_
//#define _debug_json_
//#define _debug_undo_
//#define _debug_lock_
//#define _debug_scalekey_

// macro
#define KEYIN_WAIT()        key_inputYN(0)
#define DEBUG_KEYIN_WAIT()  KEYIN_WAIT()

extern int key_inputYN(int);
