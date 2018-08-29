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

inline bool isCIAChipModel(CIAChipModel model) {
    return (model == MOS_6526_OLD) || (model == MOS_6526_NEW);
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
        uint16_t count;
        uint16_t latch;
        bool running;
        bool toggle;
        bool pbout;
        bool oneShot;
    } timerA;
    struct {
        uint16_t count;
        uint16_t latch;
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
