// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#pragma once

#include "RS232Types.h"
#include "SubComponent.h"

namespace vc64 {

class RS232 final : public SubComponent {

    Descriptions descriptions = {{

        .name           = "RS232",
        .description    = "RS232 User-Port Adapter"
    }};

    ConfigOptions options = {

        OPT_RS232_DEVICE,
        OPT_RS232_BAUD
    };

    // Current configuration
    RS232Config config = { };

    // The current values of the external port pins (client side)
    u32 port = 0xFFFF;

    // Bit counters
    isize txdCnt = 0;
    isize rxdCnt = 0;

    // Shift registers
    u16 rxdShr = 0;
    u16 txdShr = 0;

    //
    // u8 pb = 0xFF;

    // Input registers
    u16 receiveBuffer = 0;
    u16 receiveShiftReg = 0;

    // Output registers
    u16 transmitBuffer = 0;
    u16 transmitShiftReg = 0;

    // Bit reception counter
    u8 recCnt = 0;

    // External input
    string input;

    // Log buffers for incoming and outgoing bytes
    std::u16string incoming;
    std::u16string outgoing;


    //
    // Methods
    //

public:

    using SubComponent::SubComponent;

    RS232& operator= (const RS232& other) {

        CLONE(config)

        return *this;
    }


    //
    // Methods from Serializable
    //

public:

    template <class T> void serialize(T& worker) {

        if (isResetter(worker)) return;

        worker

        << config.baud;

    } SERIALIZERS(serialize);


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream& os) const override;


    //
    // Methods from Inspectable
    //

private:

    // void cacheInfo(RS232Info &result) const override;


    //
    // Methods from Configurable
    //

public:

    const RS232Config &getConfig() const { return config; }
    const ConfigOptions &getOptions() const override { return options; }
    i64 getOption(Option opt) const override;
    void checkOption(Option opt, i64 value) override;
    void setOption(Option opt, i64 value) override;


    //
    // Querying properties
    //

public:

    isize packetLength() const { return 9; } // TODO: Make configurable

    
    //
    // Accessing port pins
    //

public:

    // Reads the current value of a certain port pin
    bool getPin(isize nr) const;

    // Modifies the value of a certain port pin
    void setPin(isize nr, bool value);

    // Convenience wrappers
    bool getTXD() const { return getPin(2); }
    bool getRXD() const { return getPin(3); }
    bool getRTS() const { return getPin(4); }
    bool getCTS() const { return getPin(5); }
    bool getDSR() const { return getPin(6); }
    bool getCD () const { return getPin(8); }
    bool getDTR() const { return getPin(20); }
    bool getRI () const { return getPin(22); }

    void setTXD(bool value) { setPin(2, value); }
    void setRXD(bool value) { setPin(3, value); }
    void setRTS(bool value) { setPin(4, value); }
    void setCTS(bool value) { setPin(5, value); }
    void setDSR(bool value) { setPin(6, value); }
    void setCD (bool value) { setPin(8, value); }
    void setDTR(bool value) { setPin(20, value); }
    void setRI (bool value) { setPin(22, value); }

private:

    // Modifies multiple port pins
    void setPort(u32 mask, bool value);

    // Updates the value on the UART's TXD line
    void updateTXD();

    // Called when the RXD port pin changes it's value
    void rxdHasChanged(bool oldValue, bool newValue);



    //
    // Controlling pins
    //

public:

    u8 getPB() const;
    void setPA2(bool value);


    //
    // Receiving and sending data

public:

    // Sends a character or a string to the RS232 interface
    void operator<<(char c);
    void operator<<(const string &s);

private:

    // Returns the number of clock cycles btween two bits
    Cycle pulseWidth() const;


    //
    // Accessing the record buffers
    //

public:

    // Reads and removes the contents of one of the record buffers
    std::u16string readIncoming();
    std::u16string readOutgoing();

    // Reads and removes a single byte from one of the record buffers
    int readIncomingByte();
    int readOutgoingByte();
    int readIncomingPrintableByte();
    int readOutgoingPrintableByte();




private:

    void sendBit(bool value);
    // void sendPacket(u16 value);


    // Called when a packet has been received or sent
    void recordIncomingPacket(u16 byte);
    void recordOutgoingPacket(u16 byte);

    // Dumps a byte to RetroShell
    void dumpPacket(u16 byte);


    //
    // Processing events
    //

public:

    void processTxdEvent();
    void processRxdEvent();

};

}
