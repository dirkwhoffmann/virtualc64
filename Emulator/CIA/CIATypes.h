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
        uint8_t port;
        uint8_t reg;
        uint8_t dir;
    } portA;
    struct {
        uint8_t port;
        uint8_t reg;
        uint8_t dir;
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
    uint8_t icr;
    uint8_t imr;
    bool intLine;
    TODInfo tod;
    bool todIntEnable;
} CIAInfo;


#endif
