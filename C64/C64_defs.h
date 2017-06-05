//
//  C64_defs.h
//  V64
//
//  Created by Dirk Hoffmann on 05.06.17.
//
//

#ifndef C64_DEFS_H
#define C64_DEFS_H

// Message types
enum {
    MSG_ROM_LOADED = 1,
    MSG_ROM_MISSING,
    MSG_ROM_COMPLETE,
    MSG_RUN,
    MSG_HALT,
    MSG_CPU,
    MSG_WARP,
    MSG_ALWAYS_WARP,
    MSG_LOG,
    MSG_VC1541_ATTACHED,
    MSG_VC1541_ATTACHED_SOUND,
    MSG_VC1541_DISK,
    MSG_VC1541_DISK_SOUND,
    MSG_VC1541_LED,
    MSG_VC1541_DATA,
    MSG_VC1541_MOTOR,
    MSG_VC1541_HEAD,
    MSG_VC1541_HEAD_SOUND,
    MSG_CARTRIDGE,
    MSG_VC1530_TAPE,
    MSG_VC1530_PLAY,
    MSG_VC1530_PROGRESS,
    MSG_JOYSTICK_ATTACHED,
    MSG_JOYSTICK_REMOVED,
    MSG_PAL,
    MSG_NTSC
};

typedef struct {
    int id;			// Message ID
    int i;			// Integer value
} Message;


#endif
