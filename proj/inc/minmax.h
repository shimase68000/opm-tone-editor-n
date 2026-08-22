//
// minmax.h
//

#define MIN_AUTO_READONLY            0     // 0:disable, 1:enable
#define MAX_AUTO_READONLY            1

#define MIN_MOUSEOVER_HELP           0     // 0:disable, 1:enable
#define MAX_MOUSEOVER_HELP           1

#define MIN_MOUSE_REPEAT_SPEED       0     // wait count
#define MAX_MOUSE_REPEAT_SPEED       30

#define MIN_SCALEKEY_OFFSET          (0-CORRECT_OPM_NOTE_VALUE)
#define MAX_SCALEKEY_OFFSET          (95-CORRECT_OPM_NOTE_VALUE)

#define MIN_SCALEKEY_DELAY_COUNT     0     // 0:Delay OFF, 1-500:Delay count
#define MAX_SCALEKEY_DELAY_COUNT     500

#define MIN_SCALEKEY_START_CHANNEL   0
#define MAX_SCALEKEY_START_CHANNEL   7    // 0-7:normal scan, 8-15:reverse scan (for v1.20)
//#define MAX_SCALEKEY_START_CHANNEL   15   // 0-7:normal scan, 8-15:reverse scan (for future version)

#define MIN_SCALEKEY_OCTKEY          0
#define MAX_SCALEKEY_OCTKEY          1

#define MIN_SCALEKEY_DETUNE          -6912
#define MAX_SCALEKEY_DETUNE          6911

#define MIN_CHANNEL_VOLUME           0
#define MAX_CHANNEL_VOLUME           127

#define MIN_MASTER_TONE_VOLUME       0
#define MAX_MASTER_TONE_VOLUME       127

#define MIN_SCALEKEY_LOAD            0     // true/false
#define MAX_SCALEKEY_LOAD            1

#define MIN_SCALEKEY_UNLOAD          0     // true/false
#define MAX_SCALEKEY_UNLOAD          1

#define MIN_SCALEKEY_OPM_CHANNEL     (0b00000000)
#define MAX_SCALEKEY_OPM_CHANNEL     (0b11111111)

#define MIN_SCALEKEY_POLY_COUNT      0     // 0:Mute, 1-7:poly count
#define MAX_SCALEKEY_POLY_COUNT      8

#define MIN_SCALEKEY_UNISON_COUNT    0     // 0:Mute, 1-7:unison count
#define MAX_SCALEKEY_UNISON_COUNT    8

#define MIN_CHANNEL_ASSIGN_POLICY    0     // SEQ/RDR
#define MAX_CHANNEL_ASSIGN_POLICY    1

#define MIN_CHANNEL_ASSIGN_PRIORITY  0     // HOLD/OVER
#define MAX_CHANNEL_ASSIGN_PRIORITY  1

#define MIN_MIDI_CHANNEL_FILTER      -1    // -1:OFF, 0:Any, 1-16:MIDI channel
#define MAX_MIDI_CHANNEL_FILTER      16

#define MIN_OPM_CHANNEL_DEFAULT      (0b00000000)
#define MAX_OPM_CHANNEL_DEFAULT      (0b11111111)

#define MIN_OPM_CHANNEL_MML          (0b00000000)
#define MAX_OPM_CHANNEL_MML          (0b11111111)

#define MIN_OPM_CHANNEL_SND          (0b00000000)
#define MAX_OPM_CHANNEL_SND          (0b11111111)

#define MIN_OPM_CHANNEL_MDX          (0b00000000)
#define MAX_OPM_CHANNEL_MDX          (0b11111111)

#define MIN_OPM_CHANNEL_OPM          (0b00000000)
#define MAX_OPM_CHANNEL_OPM          (0b11111111)

#define MIN_OPM_CHANNEL_ZMS          (0b00000000)
#define MAX_OPM_CHANNEL_ZMS          (0b11111111)

#define MIN_AUTO_SELECT_OED          0
#define MAX_AUTO_SELECT_OED          1

#define MIN_AUTO_SELECT_MML          0
#define MAX_AUTO_SELECT_MML          1

#define MIN_AUTO_SELECT_SND          0
#define MAX_AUTO_SELECT_SND          1

#define MIN_AUTO_SELECT_MDX          0
#define MAX_AUTO_SELECT_MDX          1

#define MIN_AUTO_SELECT_OPM          0
#define MAX_AUTO_SELECT_OPM          1

#define MIN_AUTO_SELECT_ZMS          0
#define MAX_AUTO_SELECT_ZMS          1

#define MIN_EXPORT_TONE_NUMBER       (-99999)
#define MAX_EXPORT_TONE_NUMBER       (99999)
#define MIN_EXPORT_TEMPLATE_NUMBER   (-99999)
#define MAX_EXPORT_TEMPLATE_NUMBER   (99999)
