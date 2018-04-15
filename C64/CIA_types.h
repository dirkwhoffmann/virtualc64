//
//  CIA_types.h
//  V64
//
//  Created by Dirk Hoffmann on 15.04.18.
//

#ifndef CIA_TYPES_H
#define CIA_TYPES_H

/*! @brief    CIA chip model
 *  @details  Currently, only the old model is supported.
 */
typedef enum {
    MOS_6526_OLD,
    MOS_6526_NEW
} CIAChipModel;

/*! @brief    TOD info
 *  @details  Used by CIA::getInfo() to collect debug information
 */
typedef struct {
    struct {
        uint8_t reg;
        uint8_t dir;
    } portA;
    struct {
        uint8_t reg;
        uint8_t dir;
    } portB;
    struct {
        uint32_t count;
        uint32_t latch;
        bool running;
        bool oneShot;
        bool interruptMask;
        bool interruptData;
    } timerA;
    struct {
        uint32_t count;
        uint32_t latch;
        bool running;
        bool oneShot;
        bool interruptMask;
        bool interruptData;
    } timerB;
    TODInfo tod;
    bool todInterruptMask;
} CIAInfo;


#endif
