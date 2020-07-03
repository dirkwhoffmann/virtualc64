//
//  CIA_types.h
//  V64
//
//  Created by Dirk Hoffmann on 15.04.18.
//

#ifndef CIA_TYPES_H
#define CIA_TYPES_H

//! @brief    CIA model
typedef enum {
    MOS_6526,
    MOS_8521
} CIAModel;

inline bool isCIAModel(CIAModel model) {
    return (model == MOS_6526) || (model == MOS_8521);
}

/*! @brief    TOD info
 *  @details  Used by CIA::getInfo() to collect debug information
 */
typedef struct {
    struct {
        u8 port;
        u8 reg;
        u8 dir;
    } portA;
    struct {
        u8 port;
        u8 reg;
        u8 dir;
    } portB;
    struct {
        u16 count;
        u16 latch;
        bool running;
        bool toggle;
        bool pbout;
        bool oneShot;
    } timerA;
    struct {
        u16 count;
        u16 latch;
        bool running;
        bool toggle;
        bool pbout;
        bool oneShot;
    } timerB;
    u8 icr;
    u8 imr;
    bool intLine;
    TODInfo tod;
    bool todIntEnable;
} CIAInfo;


#endif
