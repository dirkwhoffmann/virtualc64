// -----------------------------------------------------------------------------
// This file is part of Peddle - A MOS 65xx CPU emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Published under the terms of the MIT License
// -----------------------------------------------------------------------------

// Loads a register and sets the Z and V flag
#define loadA(v) { u8 u = (v); reg.a = u; reg.sr.n = u & 0x80; reg.sr.z = u == 0; }
#define loadX(v) { u8 u = (v); reg.x = u; reg.sr.n = u & 0x80; reg.sr.z = u == 0; }
#define loadY(v) { u8 u = (v); reg.y = u; reg.sr.n = u & 0x80; reg.sr.z = u == 0; }

// Atomic CPU tasks
#define FETCH_OPCODE \
if (likely(!rdyLine)) instr = read<C>(reg.pc++); else return;
#define FETCH_ADDR_LO \
if (likely(!rdyLine)) reg.adl = read<C>(reg.pc++); else return;
#define FETCH_ADDR_HI \
if (likely(!rdyLine)) reg.adh = read<C>(reg.pc++); else return;
#define FETCH_POINTER_ADDR \
if (likely(!rdyLine)) reg.idl = read<C>(reg.pc++); else return;
#define FETCH_ADDR_LO_INDIRECT \
if (likely(!rdyLine)) reg.adl = read<C>((u16)reg.idl++); else return;
#define FETCH_ADDR_HI_INDIRECT \
if (likely(!rdyLine)) reg.adh = read<C>((u16)reg.idl++); else return;
#define IDLE_FETCH \
if (likely(!rdyLine)) readIdle<C>(reg.pc); else return;

#define READ_RELATIVE \
if (likely(!rdyLine)) reg.d = read<C>(reg.pc); else return;
#define READ_IMMEDIATE \
if (likely(!rdyLine)) reg.d = read<C>(reg.pc++); else return;
#define READ_FROM(x) \
if (likely(!rdyLine)) reg.d = read<C>(x); else return;
#define READ_FROM_ADDRESS \
if (likely(!rdyLine)) reg.d = read<C>(HI_LO(reg.adh, reg.adl)); else return;
#define READ_FROM_ZERO_PAGE \
if (likely(!rdyLine)) reg.d = readZeroPage<C>(reg.adl); else return;
#define READ_FROM_ADDRESS_INDIRECT \
if (likely(!rdyLine)) reg.d = readZeroPage<C>(reg.dl); else return;

#define IDLE_READ_IMPLIED \
if (likely(!rdyLine)) readIdle<C>(reg.pc); else return;
#define IDLE_READ_IMMEDIATE \
if (likely(!rdyLine)) readIdle<C>(reg.pc++); else return;
#define IDLE_READ_FROM(x) \
if (likely(!rdyLine)) readIdle<C>(x); else return;
#define IDLE_READ_FROM_ADDRESS \
if (likely(!rdyLine)) readIdle<C>(HI_LO(reg.adh, reg.adl)); else return;
#define IDLE_READ_FROM_ZERO_PAGE \
if (likely(!rdyLine)) readZeroPageIdle<C>(reg.adl); else return;
#define IDLE_READ_FROM_ADDRESS_INDIRECT \
if (likely(!rdyLine)) readZeroPageIdle<C>(reg.idl); else return;

#define WRITE_TO_ADDRESS \
write<C>(HI_LO(reg.adh, reg.adl), reg.d);
#define WRITE_TO_ADDRESS_AND_SET_FLAGS \
write<C>(HI_LO(reg.adh, reg.adl), reg.d); setN(reg.d & 0x80); setZ(reg.d == 0);
#define WRITE_TO_ZERO_PAGE \
writeZeroPage<C>(reg.adl, reg.d);
#define WRITE_TO_ZERO_PAGE_AND_SET_FLAGS \
writeZeroPage<C>(reg.adl, reg.d); setN(reg.d & 0x80); setZ(reg.d == 0);

#define ADD_INDEX_X reg.ovl = ((int)reg.adl + (int)reg.x > 0xFF); reg.adl += reg.x;
#define ADD_INDEX_Y reg.ovl = ((int)reg.adl + (int)reg.y > 0xFF); reg.adl += reg.y;
#define ADD_INDEX_X_INDIRECT reg.idl += reg.x;
#define ADD_INDEX_Y_INDIRECT reg.idl += reg.y;

#define SET_PCL(lo) reg.pc = (u16)((reg.pc & 0xff00) | (lo));
#define SET_PCH(hi) reg.pc = (u16)((reg.pc & 0x00ff) | (hi) << 8);
#define PUSH_PCL writeStack<C>(reg.sp--, LO_BYTE(reg.pc));
#define PUSH_PCH writeStack<C>(reg.sp--, HI_BYTE(reg.pc));
#define PUSH_P writeStack<C>(reg.sp--, getP());
#define PUSH_P_WITH_B_SET writeStack<C>(reg.sp--, getP() | B_FLAG);
#define PUSH_A writeStack<C>(reg.sp--, reg.a);
#define PULL_PCL if (likely(!rdyLine)) { SET_PCL(readStack<C>(reg.sp)); } else return;
#define PULL_PCH if (likely(!rdyLine)) { SET_PCH(readStack<C>(reg.sp)); } else return;
#define PULL_P if (likely(!rdyLine)) { setPWithoutB(readStack<C>(reg.sp)); } else return;
#define PULL_A if (likely(!rdyLine)) { loadA(readStack<C>(reg.sp)); } else return;
#define IDLE_PULL if (likely(!rdyLine)) { readStackIdle<C>(reg.sp); } else return;

#define PAGE_BOUNDARY_CROSSED reg.ovl
#define FIX_ADDR_HI reg.adh++;

#define POLL_IRQ doIrq = (levelDetector.delayed() && !getI());
#define POLL_NMI doNmi = edgeDetector.delayed();
#define POLL_INT POLL_IRQ POLL_NMI
#define POLL_INT_AGAIN doIrq |= (levelDetector.delayed() != 0 && !getI()); \
doNmi |= (edgeDetector.delayed() != 0);
#define CONTINUE next = (MicroInstruction)((int)next+1); return;
#define DONE     done<C>(); return;

void
Peddle::adc(u8 op)
{
    if (getD())
        adcBcd(op);
    else
        adcBinary(op);
}

void
Peddle::adcBinary(u8 op)
{
    u16 sum = reg.a + op + (getC() ? 1 : 0);
    
    setC(sum > 255);
    setV(!((reg.a ^ op) & 0x80) && ((reg.a ^ sum) & 0x80));
    loadA((u8)sum);
}

void
Peddle::adcBcd(u8 op)
{
    u16 sum       = reg.a + op + (getC() ? 1 : 0);
    u8  highDigit = (reg.a >> 4) + (op >> 4);
    u8  lowDigit  = (reg.a & 0x0F) + (op & 0x0F) + (getC() ? 1 : 0);
    
    /* Check for overflow conditions. If an overflow occurs on a BCD digit, it
     * needs to be fixed by adding the pseudo-tetrade 0110 (= 6)
     */
    if (lowDigit > 9) {
        lowDigit = lowDigit + 6;
    }
    if (lowDigit > 0x0F) {
        highDigit++;
    }
    
    setZ((sum & 0xFF) == 0);
    setN(highDigit & 0x08);
    setV((((highDigit << 4) ^ reg.a) & 0x80) && !((reg.a ^ op) & 0x80));
    
    if (highDigit > 9) {
        highDigit = (highDigit + 6);
    }
    if (highDigit > 0x0F) {
        setC(1);
    } else {
        setC(0);
    }
    
    lowDigit &= 0x0F;
    reg.a = (u8)((highDigit << 4) | lowDigit);
}

void
Peddle::cmp(u8 op1, u8 op2)
{
    u8 tmp = op1 - op2;
    
    setC(op1 >= op2);
    setN(tmp & 128);
    setZ(tmp == 0);
}

void
Peddle::sbc(u8 op)
{
    if (getD())
        sbcBcd(op);
    else
        sbcBinary(op);
}

void
Peddle::sbcBinary(u8 op)
{
    u16 sum = reg.a - op - (getC() ? 0 : 1);
    
    setC(sum <= 255);
    setV(((reg.a ^ sum) & 0x80) && ((reg.a ^ op) & 0x80));
    loadA((u8)sum);
}

void
Peddle::sbcBcd(u8 op)
{
    u16 sum       = reg.a - op - (getC() ? 0 : 1);
    u8  highDigit = (reg.a >> 4) - (op >> 4);
    u8  lowDigit  = (reg.a & 0x0F) - (op & 0x0F) - (getC() ? 0 : 1);
    
    /* Check for underflow conditions. If an overflow occurs on a BCD digit,
     * it needs to be fixed by subtracting the pseudo-tetrade 0110 (=6)
     */
    if (lowDigit & 0x10) {
        lowDigit = lowDigit - 6;
        highDigit--;
    }
    if (highDigit & 0x10) {
        highDigit = highDigit - 6;
    }
    
    setC(sum < 0x100);
    setV(((reg.a ^ sum) & 0x80) && ((reg.a ^ op) & 0x80));
    setZ((sum & 0xFF) == 0);
    setN(sum & 0x80);
    
    reg.a = (u8)((highDigit << 4) | (lowDigit & 0x0f));
}

isize
Peddle::getLengthOfInstruction(u8 opcode) const
{
    switch(addressingMode[opcode]) {

        case ADDR_IMPLIED:
        case ADDR_ACCUMULATOR:  return 1;
        case ADDR_IMMEDIATE:
        case ADDR_ZERO_PAGE:
        case ADDR_ZERO_PAGE_X:
        case ADDR_ZERO_PAGE_Y:
        case ADDR_INDIRECT_X:
        case ADDR_INDIRECT_Y:
        case ADDR_RELATIVE:     return 2;
        case ADDR_ABSOLUTE:
        case ADDR_ABSOLUTE_X:
        case ADDR_ABSOLUTE_Y:
        case ADDR_DIRECT:
        case ADDR_INDIRECT:     return 3;

        default:
            fatalError;
    }
}

isize
Peddle::getLengthOfInstructionAt(u16 addr) const
{
    return getLengthOfInstruction(readDasm(addr));
}

isize
Peddle::getLengthOfCurrentInstruction() const
{
    return getLengthOfInstructionAt(getPC0());
}

u16
Peddle::getAddressOfNextInstruction() const
{
    return (u16)(getPC0() + getLengthOfCurrentInstruction());
}

void
Peddle::reset()
{
    switch (cpuModel) {

        case MOS_6502: reset<MOS_6502>(); break;
        case MOS_6507: reset<MOS_6507>(); break;
        case MOS_6510: reset<MOS_6510>(); break;
        case MOS_8502: reset<MOS_8502>(); break;

        default:
            fatalError;
    }
}

template <CPURevision C> void
Peddle::reset()
{
    clock = 0;
    flags = 0;
    next = fetch;
    rdyLine = 0;
    rdyLineUp = 0;
    rdyLineDown = 0;
    nmiLine = 0;
    irqLine = 0;
    edgeDetector.reset(0);
    levelDetector.reset(0);
    doNmi = false;
    doIrq = false;

    reg = { };
    reg.pport.data = 0xFF;
    reg.pc = reg.pc0 = readResetVector();
    setB(1);
    setI(1);

    debugger.reset();
}

void
Peddle::execute()
{
    switch (cpuModel) {

        case MOS_6502: execute<MOS_6502>(); break;
        case MOS_6507: execute<MOS_6507>(); break;
        case MOS_6510: execute<MOS_6510>(); break;
        case MOS_8502: execute<MOS_8502>(); break;

        default:
            fatalError;
    }
}

template <CPURevision C> void
Peddle::execute()
{
    u8 instr;
    
    switch (next) {
            
        case fetch:

            // Check interrupt lines
            if (unlikely(doNmi)) {

                nmiWillTrigger();
                IDLE_FETCH
                edgeDetector.clear();
                next = nmi_2;
                doNmi = false;
                doIrq = false; // NMI wins
                return;
                
            } else if (unlikely(doIrq)) {

                irqWillTrigger();
                IDLE_FETCH
                next = irq_2;
                doIrq = false;
                return;
            }
            
            // Execute the Fetch phase
            FETCH_OPCODE
            next = actionFunc[instr];
            return;
            
            //
            // Illegal instructions
            //
            
        case JAM:

            cpuDidJam();
            CONTINUE

        case JAM_2:
            
            POLL_INT
            DONE

            //
            // IRQ handling
            //
            
        case irq_2:
            
            IDLE_READ_IMPLIED
            CONTINUE
            
        case irq_3:
            
            PUSH_PCH
            CONTINUE
            
        case irq_4:
            
            PUSH_PCL
            // Check for interrupt hijacking
            // If there is a positive edge on the NMI line ...
            if (edgeDetector.current()) {
                
                // ... jump to the NMI vector instead of the IRQ vector.
                edgeDetector.clear();
                next = nmi_5;
                return;
            }
            CONTINUE
            
        case irq_5:
            
            write<C>(0x100+(reg.sp--), getPWithClearedB());
            CONTINUE
            
        case irq_6:
            
            READ_FROM(0xFFFE)
            SET_PCL(reg.d);
            setI(1);
            CONTINUE
            
        case irq_7:
            
            READ_FROM(0xFFFF)
            SET_PCH(reg.d);
            irqDidTrigger();
            DONE
            
            //
            // NMI handling
            //

        case nmi_2:

            IDLE_READ_IMPLIED
            CONTINUE
            
        case nmi_3:
            
            PUSH_PCH
            CONTINUE
            
        case nmi_4:
            
            PUSH_PCL
            CONTINUE
            
        case nmi_5:
            
            write<C>(0x100+(reg.sp--), getPWithClearedB());
            CONTINUE
            
        case nmi_6:
            
            READ_FROM(0xFFFA)
            SET_PCL(reg.d);
            setI(1);
            CONTINUE
            
        case nmi_7:

            READ_FROM(0xFFFB)
            SET_PCH(reg.d);
            nmiDidTrigger();
            DONE

            //
            // Adressing mode: Immediate (shared behavior)
            //

        case BRK: case RTI: case RTS:
            
            IDLE_READ_IMMEDIATE
            CONTINUE
            
            //
            // Adressing mode: Implied (shared behavior)
            //

        case PHA: case PHP: case PLA: case PLP:
            
            IDLE_READ_IMPLIED
            CONTINUE
            
            //
            // Adressing mode: Zero-Page  (shared behavior)
            //

        case ADC_zpg: case AND_zpg: case ASL_zpg: case BIT_zpg:
        case CMP_zpg: case CPX_zpg: case CPY_zpg: case DEC_zpg:
        case EOR_zpg: case INC_zpg: case LDA_zpg: case LDX_zpg:
        case LDY_zpg: case LSR_zpg: case NOP_zpg: case ORA_zpg:
        case ROL_zpg: case ROR_zpg: case SBC_zpg: case STA_zpg:
        case STX_zpg: case STY_zpg: case DCP_zpg: case ISC_zpg:
        case LAX_zpg: case RLA_zpg: case RRA_zpg: case SAX_zpg:
        case SLO_zpg: case SRE_zpg:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ASL_zpg_2: case DEC_zpg_2: case INC_zpg_2: case LSR_zpg_2:
        case ROL_zpg_2: case ROR_zpg_2: case DCP_zpg_2: case ISC_zpg_2:
        case RLA_zpg_2: case RRA_zpg_2: case SLO_zpg_2: case SRE_zpg_2:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
            //
            // Adressing mode: Zero-Page Indexed (shared behavior)
            //
            
        case ADC_zpg_x: case AND_zpg_x: case ASL_zpg_x: case CMP_zpg_x:
        case DEC_zpg_x: case EOR_zpg_x: case INC_zpg_x: case LDA_zpg_x:
        case LDY_zpg_x: case LSR_zpg_x: case NOP_zpg_x: case ORA_zpg_x:
        case ROL_zpg_x: case ROR_zpg_x: case SBC_zpg_x: case STA_zpg_x:
        case STY_zpg_x: case DCP_zpg_x: case ISC_zpg_x: case RLA_zpg_x:
        case RRA_zpg_x: case SLO_zpg_x: case SRE_zpg_x:

        case LDX_zpg_y: case STX_zpg_y: case LAX_zpg_y: case SAX_zpg_y:
            
            FETCH_ADDR_LO
            CONTINUE

        case ADC_zpg_x_2: case AND_zpg_x_2: case ASL_zpg_x_2: case CMP_zpg_x_2:
        case DEC_zpg_x_2: case EOR_zpg_x_2: case INC_zpg_x_2: case LDA_zpg_x_2:
        case LDY_zpg_x_2: case LSR_zpg_x_2: case NOP_zpg_x_2: case ORA_zpg_x_2:
        case ROL_zpg_x_2: case ROR_zpg_x_2: case SBC_zpg_x_2: case DCP_zpg_x_2:
        case ISC_zpg_x_2: case RLA_zpg_x_2: case RRA_zpg_x_2: case SLO_zpg_x_2:
        case SRE_zpg_x_2: case STA_zpg_x_2: case STY_zpg_x_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_X
            CONTINUE

        case LDX_zpg_y_2: case LAX_zpg_y_2: case STX_zpg_y_2: case SAX_zpg_y_2:
            
            READ_FROM_ZERO_PAGE
            ADD_INDEX_Y
            CONTINUE

        case ASL_zpg_x_3: case DEC_zpg_x_3: case INC_zpg_x_3: case LSR_zpg_x_3:
        case ROL_zpg_x_3: case ROR_zpg_x_3: case DCP_zpg_x_3: case ISC_zpg_x_3:
        case RLA_zpg_x_3: case RRA_zpg_x_3: case SLO_zpg_x_3: case SRE_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            CONTINUE
            
            
            //
            // Adressing mode: Absolute (shared behavior)
            //
            
        case ADC_abs: case AND_abs: case ASL_abs: case BIT_abs:
        case CMP_abs: case CPX_abs: case CPY_abs: case DEC_abs:
        case EOR_abs: case INC_abs: case LDA_abs: case LDX_abs:
        case LDY_abs: case LSR_abs: case NOP_abs: case ORA_abs:
        case ROL_abs: case ROR_abs: case SBC_abs: case STA_abs:
        case STX_abs: case STY_abs: case DCP_abs: case ISC_abs:
        case LAX_abs: case RLA_abs: case RRA_abs: case SAX_abs:
        case SLO_abs: case SRE_abs:
            
            FETCH_ADDR_LO
            CONTINUE

        case ADC_abs_2: case AND_abs_2: case ASL_abs_2: case BIT_abs_2:
        case CMP_abs_2: case CPX_abs_2: case CPY_abs_2: case DEC_abs_2:
        case EOR_abs_2: case INC_abs_2: case LDA_abs_2: case LDX_abs_2:
        case LDY_abs_2: case LSR_abs_2: case NOP_abs_2: case ORA_abs_2:
        case ROL_abs_2: case ROR_abs_2: case SBC_abs_2: case STA_abs_2:
        case STX_abs_2: case STY_abs_2: case DCP_abs_2: case ISC_abs_2:
        case LAX_abs_2: case RLA_abs_2: case RRA_abs_2: case SAX_abs_2:
        case SLO_abs_2: case SRE_abs_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case ASL_abs_3: case DEC_abs_3: case INC_abs_3: case LSR_abs_3:
        case ROL_abs_3: case ROR_abs_3: case DCP_abs_3: case ISC_abs_3:
        case RLA_abs_3: case RRA_abs_3: case SLO_abs_3: case SRE_abs_3:
            
            READ_FROM_ADDRESS
            CONTINUE
            
            //
            // Adressing mode: Absolute Indexed (shared behavior)
            //
            
        case ADC_abs_x: case AND_abs_x: case ASL_abs_x: case CMP_abs_x:
        case DEC_abs_x: case EOR_abs_x: case INC_abs_x: case LDA_abs_x:
        case LDY_abs_x: case LSR_abs_x: case NOP_abs_x: case ORA_abs_x:
        case ROL_abs_x: case ROR_abs_x: case SBC_abs_x: case STA_abs_x:
        case DCP_abs_x: case ISC_abs_x: case RLA_abs_x: case RRA_abs_x:
        case SHY_abs_x: case SLO_abs_x: case SRE_abs_x:
            
        case ADC_abs_y: case AND_abs_y: case CMP_abs_y: case EOR_abs_y:
        case LDA_abs_y: case LDX_abs_y: case LSR_abs_y: case ORA_abs_y:
        case SBC_abs_y: case STA_abs_y: case DCP_abs_y: case ISC_abs_y:
        case LAS_abs_y: case LAX_abs_y: case RLA_abs_y: case RRA_abs_y:
        case SHA_abs_y: case SHX_abs_y: case SLO_abs_y: case SRE_abs_y:
        case TAS_abs_y:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case ADC_abs_x_2: case AND_abs_x_2: case ASL_abs_x_2: case CMP_abs_x_2:
        case DEC_abs_x_2: case EOR_abs_x_2: case INC_abs_x_2: case LDA_abs_x_2:
        case LDY_abs_x_2: case LSR_abs_x_2: case NOP_abs_x_2: case ORA_abs_x_2:
        case ROL_abs_x_2: case ROR_abs_x_2: case SBC_abs_x_2: case STA_abs_x_2:
        case DCP_abs_x_2: case ISC_abs_x_2: case RLA_abs_x_2: case RRA_abs_x_2:
        case SHY_abs_x_2: case SLO_abs_x_2: case SRE_abs_x_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_X
            CONTINUE
            
        case ADC_abs_y_2: case AND_abs_y_2: case CMP_abs_y_2: case EOR_abs_y_2:
        case LDA_abs_y_2: case LDX_abs_y_2: case LSR_abs_y_2: case ORA_abs_y_2:
        case SBC_abs_y_2: case STA_abs_y_2: case DCP_abs_y_2: case ISC_abs_y_2:
        case LAS_abs_y_2: case LAX_abs_y_2: case RLA_abs_y_2: case RRA_abs_y_2:
        case SHA_abs_y_2: case SHX_abs_y_2: case SLO_abs_y_2: case SRE_abs_y_2:
        case TAS_abs_y_2:
            
            FETCH_ADDR_HI
            ADD_INDEX_Y
            CONTINUE
            
        case ASL_abs_x_3: case DEC_abs_x_3: case INC_abs_x_3: case LSR_abs_x_3:
        case ROL_abs_x_3: case ROR_abs_x_3: case DCP_abs_x_3: case ISC_abs_x_3:
        case RLA_abs_x_3: case RRA_abs_x_3: case STA_abs_x_3: case SLO_abs_x_3:
        case SRE_abs_x_3:

        case LSR_abs_y_3: case STA_abs_y_3: case DCP_abs_y_3: case ISC_abs_y_3:
        case RLA_abs_y_3: case RRA_abs_y_3: case SLO_abs_y_3: case SRE_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case ASL_abs_x_4: case DEC_abs_x_4: case INC_abs_x_4: case LSR_abs_x_4:
        case ROL_abs_x_4: case ROR_abs_x_4: case DCP_abs_x_4: case ISC_abs_x_4:
        case RLA_abs_x_4: case RRA_abs_x_4: case SLO_abs_x_4: case SRE_abs_x_4:
            
        case DCP_abs_y_4: case LSR_abs_y_4: case ISC_abs_y_4: case RLA_abs_y_4:
        case RRA_abs_y_4: case SLO_abs_y_4: case SRE_abs_y_4:
            
            READ_FROM_ADDRESS
            CONTINUE
            
            //
            // Adressing mode: Indexed Indirect (shared behavior)
            //

        case ADC_ind_x: case AND_ind_x: case ASL_ind_x: case CMP_ind_x:
        case DEC_ind_x: case EOR_ind_x: case INC_ind_x: case LDA_ind_x:
        case LDX_ind_x: case LDY_ind_x: case LSR_ind_x: case ORA_ind_x:
        case ROL_ind_x: case ROR_ind_x: case SBC_ind_x: case STA_ind_x:
        case DCP_ind_x: case ISC_ind_x: case LAX_ind_x: case RLA_ind_x:
        case RRA_ind_x: case SAX_ind_x: case SLO_ind_x: case SRE_ind_x:
            
            FETCH_POINTER_ADDR
            CONTINUE
            
        case ADC_ind_x_2: case AND_ind_x_2: case ASL_ind_x_2: case CMP_ind_x_2:
        case DEC_ind_x_2: case EOR_ind_x_2: case INC_ind_x_2: case LDA_ind_x_2:
        case LDX_ind_x_2: case LDY_ind_x_2: case LSR_ind_x_2: case ORA_ind_x_2:
        case ROL_ind_x_2: case ROR_ind_x_2: case SBC_ind_x_2: case STA_ind_x_2:
        case DCP_ind_x_2: case ISC_ind_x_2: case LAX_ind_x_2: case RLA_ind_x_2:
        case RRA_ind_x_2: case SAX_ind_x_2: case SLO_ind_x_2: case SRE_ind_x_2:
            
            IDLE_READ_FROM_ADDRESS_INDIRECT
            ADD_INDEX_X_INDIRECT
            CONTINUE
            
        case ADC_ind_x_3: case AND_ind_x_3: case ASL_ind_x_3: case CMP_ind_x_3:
        case DEC_ind_x_3: case EOR_ind_x_3: case INC_ind_x_3: case LDA_ind_x_3:
        case LDX_ind_x_3: case LDY_ind_x_3: case LSR_ind_x_3: case ORA_ind_x_3:
        case ROL_ind_x_3: case ROR_ind_x_3: case SBC_ind_x_3: case STA_ind_x_3:
        case DCP_ind_x_3: case ISC_ind_x_3: case LAX_ind_x_3: case RLA_ind_x_3:
        case RRA_ind_x_3: case SAX_ind_x_3: case SLO_ind_x_3: case SRE_ind_x_3:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case ADC_ind_x_4: case AND_ind_x_4: case ASL_ind_x_4: case CMP_ind_x_4:
        case DEC_ind_x_4: case EOR_ind_x_4: case INC_ind_x_4: case LDA_ind_x_4:
        case LDX_ind_x_4: case LDY_ind_x_4: case LSR_ind_x_4: case ORA_ind_x_4:
        case ROL_ind_x_4: case ROR_ind_x_4: case SBC_ind_x_4: case STA_ind_x_4:
        case DCP_ind_x_4: case ISC_ind_x_4: case LAX_ind_x_4: case RLA_ind_x_4:
        case RRA_ind_x_4: case SAX_ind_x_4: case SLO_ind_x_4: case SRE_ind_x_4:
            
            FETCH_ADDR_HI_INDIRECT
            CONTINUE
            
        case ASL_ind_x_5: case DEC_ind_x_5: case INC_ind_x_5: case LSR_ind_x_5:
        case ROL_ind_x_5: case ROR_ind_x_5: case DCP_ind_x_5: case ISC_ind_x_5:
        case RLA_ind_x_5: case RRA_ind_x_5: case SLO_ind_x_5: case SRE_ind_x_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
            //
            // Adressing mode: Indirect Indexed (shared behavior)
            //
            
        case ADC_ind_y: case AND_ind_y: case CMP_ind_y: case EOR_ind_y:
        case LDA_ind_y: case LDX_ind_y: case LDY_ind_y: case LSR_ind_y:
        case ORA_ind_y: case SBC_ind_y: case STA_ind_y: case DCP_ind_y:
        case ISC_ind_y: case LAX_ind_y: case RLA_ind_y: case RRA_ind_y:
        case SHA_ind_y: case SLO_ind_y: case SRE_ind_y:
            
            FETCH_POINTER_ADDR
            CONTINUE

        case ADC_ind_y_2: case AND_ind_y_2: case CMP_ind_y_2: case EOR_ind_y_2:
        case LDA_ind_y_2: case LDX_ind_y_2: case LDY_ind_y_2: case LSR_ind_y_2:
        case ORA_ind_y_2: case SBC_ind_y_2: case STA_ind_y_2: case DCP_ind_y_2:
        case ISC_ind_y_2: case LAX_ind_y_2: case RLA_ind_y_2: case RRA_ind_y_2:
        case SHA_ind_y_2: case SLO_ind_y_2: case SRE_ind_y_2:
            
            FETCH_ADDR_LO_INDIRECT
            CONTINUE
            
        case ADC_ind_y_3: case AND_ind_y_3: case CMP_ind_y_3: case EOR_ind_y_3:
        case LDA_ind_y_3: case LDX_ind_y_3: case LDY_ind_y_3: case LSR_ind_y_3:
        case ORA_ind_y_3: case SBC_ind_y_3: case STA_ind_y_3: case DCP_ind_y_3:
        case ISC_ind_y_3: case LAX_ind_y_3: case RLA_ind_y_3: case RRA_ind_y_3:
        case SHA_ind_y_3: case SLO_ind_y_3: case SRE_ind_y_3:
            
            FETCH_ADDR_HI_INDIRECT
            ADD_INDEX_Y
            CONTINUE

        case LSR_ind_y_4: case STA_ind_y_4: case DCP_ind_y_4: case ISC_ind_y_4:
        case RLA_ind_y_4: case RRA_ind_y_4: case SLO_ind_y_4: case SRE_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) { FIX_ADDR_HI }
            CONTINUE
            
        case LSR_ind_y_5: case DCP_ind_y_5: case ISC_ind_y_5: case RLA_ind_y_5:
        case RRA_ind_y_5: case SLO_ind_y_5: case SRE_ind_y_5:
            
            READ_FROM_ADDRESS
            CONTINUE
            
            //
            // Adressing mode: Relative (shared behavior)
            //
            
        case BCC_rel_2: case BCS_rel_2: case BEQ_rel_2: case BMI_rel_2:
        case BNE_rel_2: case BPL_rel_2: case BVC_rel_2: case BVS_rel_2:
        {
            IDLE_READ_IMPLIED
            u8 pc_hi = HI_BYTE(reg.pc);
            reg.pc += (i8)reg.d;
            
            if (unlikely(pc_hi != HI_BYTE(reg.pc))) {
                next = (reg.d & 0x80) ? branch_3_underflow : branch_3_overflow;
                return;
            }
            DONE
        }
            
        case branch_3_underflow:
            
            IDLE_READ_FROM(reg.pc + 0x100)
            POLL_INT_AGAIN
            DONE
            
        case branch_3_overflow:
            
            IDLE_READ_FROM(reg.pc - 0x100)
            POLL_INT_AGAIN
            DONE
            
            
            // Instruction: ADC
            //
            // Operation:   A,C := A+M+C
            //
            // Flags:       N Z C I D V
            //              / / / - - /

        case ADC_imm:

            READ_IMMEDIATE
            adc(reg.d);
            POLL_INT
            DONE

        case ADC_zpg_2:
        case ADC_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            adc(reg.d);
            POLL_INT
            DONE

        case ADC_abs_x_3:
        case ADC_abs_y_3:
        case ADC_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                adc(reg.d);
                POLL_INT
                DONE
            }
            
        case ADC_abs_3:
        case ADC_abs_x_4:
        case ADC_abs_y_4:
        case ADC_ind_x_5:
        case ADC_ind_y_5:
            
            READ_FROM_ADDRESS
            adc(reg.d);
            POLL_INT
            DONE
            
            
            // Instruction: AND
            //
            // Operation:   A := A AND M
            //
            // Flags:       N Z C I D V
            //              / / - - - -

        case AND_imm:
            
            READ_IMMEDIATE
            loadA(reg.a & reg.d);
            POLL_INT
            DONE

        case AND_zpg_2:
        case AND_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            loadA(reg.a & reg.d);
            POLL_INT
            DONE
            
        case AND_abs_x_3:
        case AND_abs_y_3:
        case AND_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadA(reg.a & reg.d);
                POLL_INT
                DONE
            }
            
        case AND_abs_3:
        case AND_abs_x_4:
        case AND_abs_y_4:
        case AND_ind_x_5:
        case AND_ind_y_5:
            
            READ_FROM_ADDRESS
            loadA(reg.a & reg.d);
            POLL_INT
            DONE
            
            
            // Instruction: ASL
            //
            // Operation:   C <- (A|M << 1) <- 0
            //
            // Flags:       N Z C I D V
            //              / / / - - -

#define DO_ASL_ACC setC(reg.a & 0x80); loadA((u8)(reg.a << 1));
#define DO_ASL setC(reg.d & 0x80); reg.d = (u8)(reg.d << 1);

        case ASL_acc:
            
            IDLE_READ_IMPLIED
            DO_ASL_ACC
            POLL_INT
            DONE
            
        case ASL_zpg_3:
        case ASL_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_ASL
            CONTINUE

        case ASL_abs_4:
        case ASL_abs_x_5:
        case ASL_ind_x_6:
            
            WRITE_TO_ADDRESS
            DO_ASL
            CONTINUE
            
        case ASL_zpg_4:
        case ASL_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            POLL_INT
            DONE
            
        case ASL_abs_5:
        case ASL_abs_x_6:
        case ASL_ind_x_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE
            
            
            // Instruction: BCC
            //
            // Operation:   Branch on C = 0
            //
            // Flags:       N Z C I D V
            //              - - - - - -

        case BCC_rel:
            
            READ_IMMEDIATE
            POLL_INT
            
            if (!getC()) {
                CONTINUE
            } else {
                DONE
            }
            
            
            // Instruction: BCS
            //
            // Operation:   Branch on C = 1
            //
            // Flags:       N Z C I D V
            //              - - - - - -

        case BCS_rel:
            
            READ_IMMEDIATE
            POLL_INT
            
            if (getC()) {
                CONTINUE
            } else {
                DONE
            }
            

            // Instruction: BEQ
            //
            // Operation:   Branch on Z = 1
            //
            // Flags:       N Z C I D V
            //              - - - - - -
            
        case BEQ_rel:
            
            READ_IMMEDIATE
            POLL_INT
            
            if (getZ()) {
                CONTINUE
            } else {
                DONE
            }
            
            
            // Instruction: BIT
            //
            // Operation:   A AND M, N := M7, V := M6
            //
            // Flags:       N Z C I D V
            //              / / - - - /
            
        case BIT_zpg_2:
            
            READ_FROM_ZERO_PAGE
            setN(reg.d & 128);
            setV(reg.d & 64);
            setZ((reg.d & reg.a) == 0);
            POLL_INT
            DONE

        case BIT_abs_3:
            
            READ_FROM_ADDRESS
            setN(reg.d & 128);
            setV(reg.d & 64);
            setZ((reg.d & reg.a) == 0);
            POLL_INT
            DONE

            
            // Instruction: BMI
            //
            // Operation:   Branch on N = 1
            //
            // Flags:       N Z C I D V
            //              - - - - - -

        case BMI_rel:
            
            READ_IMMEDIATE
            POLL_INT
            
            if (getN()) {
                CONTINUE
            } else {
                DONE
            }

            
            // Instruction: BNE
            //
            // Operation:   Branch on Z = 0
            //
            // Flags:       N Z C I D V
            //              - - - - - -
            
        case BNE_rel:
            
            READ_IMMEDIATE
            POLL_INT
            
            if (!getZ()) {
                CONTINUE
            } else {
                DONE
            }


            // Instruction: BPL
            //
            // Operation:   Branch on N = 0
            //
            // Flags:       N Z C I D V
            //              - - - - - -

        case BPL_rel:
            
            READ_IMMEDIATE
            POLL_INT
            
            if (!getN()) {
                CONTINUE
            } else {
                DONE
            }


            // Instruction: BRK
            //
            // Operation:   Forced Interrupt (Break)
            //
            // Flags:       N Z C I D V    B
            //              - - - 1 - -    1
            
        case BRK_2:
            
            setB(1);
            PUSH_PCH
            CONTINUE
            
        case BRK_3:

            PUSH_PCL
            
            // Check for interrupt hijacking
            // If there is a positive edge on the NMI line, ...
            if (edgeDetector.current()) {

                // ... jump to the NMI vector instead of the IRQ vector.
                edgeDetector.clear();
                next = BRK_nmi_4;
                return;
                
            } else {
                CONTINUE
            }
            
        case BRK_4:
            
            PUSH_P
            CONTINUE
            
        case BRK_5:
            
            READ_FROM(0xFFFE);
            SET_PCL(reg.d);
            setI(1);
            CONTINUE
            
        case BRK_6:
            
            READ_FROM(0xFFFF);
            SET_PCH(reg.d);
            POLL_INT
            doNmi = false; // Only the level detector is polled here. This is
            // the reason why only IRQs can be triggered right
            // after a BRK command, but not NMIs.
            DONE
            
        case BRK_nmi_4:
            
            PUSH_P
            CONTINUE
            
        case BRK_nmi_5:
            
            READ_FROM(0xFFFA);
            SET_PCL(reg.d);
            setI(1);
            CONTINUE
            
        case BRK_nmi_6:
            
            READ_FROM(0xFFFB);
            SET_PCH(reg.d);
            POLL_INT
            DONE

            
            // Instruction: BVC
            //
            // Operation:   Branch on V = 0
            //
            // Flags:       N Z C I D V
            //              - - - - - -

        case BVC_rel:
            
            READ_IMMEDIATE
            POLL_INT

            if (!getV()) {
                CONTINUE
            } else {
                DONE
            }


            // Instruction: BVS
            //
            // Operation:   Branch on V = 1
            //
            // Flags:       N Z C I D V
            //              - - - - - -

        case BVS_rel:
            
            READ_IMMEDIATE
            POLL_INT
            
            if (getV()) {
                CONTINUE
            } else {
                DONE
            }


            // Instruction: CLC
            //
            // Operation:   C := 0
            //
            // Flags:       N Z C I D V
            //              - - 0 - - -

        case CLC:
            
            IDLE_READ_IMPLIED
            setC(0);
            POLL_INT
            DONE


            // Instruction: CLD
            //
            // Operation:   D := 0
            //
            // Flags:       N Z C I D V
            //              - - - - 0 -

        case CLD:
            
            IDLE_READ_IMPLIED
            setD(0);
            POLL_INT
            DONE


            // Instruction: CLI
            //
            // Operation:   I := 0
            //
            // Flags:       N Z C I D V
            //              - - - 0 - -

        case CLI:
            
            POLL_INT
            setI(0);
            IDLE_READ_IMPLIED
            DONE
            
            
            // Instruction: CLV
            //
            // Operation:   V := 0
            //
            // Flags:       N Z C I D V
            //              - - - - - 0

        case CLV:
            
            IDLE_READ_IMPLIED
            setV(0);
            POLL_INT
            DONE


            // Instruction: CMP
            //
            // Operation:   A-M
            //
            // Flags:       N Z C I D V
            //              / / / - - -

        case CMP_imm:
            
            READ_IMMEDIATE
            cmp(reg.a, reg.d);
            POLL_INT
            DONE

        case CMP_zpg_2:
        case CMP_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            cmp(reg.a, reg.d);
            POLL_INT
            DONE

        case CMP_abs_x_3:
        case CMP_abs_y_3:
        case CMP_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                cmp(reg.a, reg.d);
                POLL_INT
                DONE
            }
            
        case CMP_abs_3:
        case CMP_abs_x_4:
        case CMP_abs_y_4:
        case CMP_ind_x_5:
        case CMP_ind_y_5:
            
            READ_FROM_ADDRESS
            cmp(reg.a, reg.d);
            POLL_INT
            DONE

            
            // Instruction: CPX
            //
            // Operation:   X-M
            //
            // Flags:       N Z C I D V
            //              / / / - - -

        case CPX_imm:
            
            READ_IMMEDIATE
            cmp(reg.x, reg.d);
            POLL_INT
            DONE
            
        case CPX_zpg_2:
            
            READ_FROM_ZERO_PAGE
            cmp(reg.x, reg.d);
            POLL_INT
            DONE
            
        case CPX_abs_3:
            
            READ_FROM_ADDRESS
            cmp(reg.x, reg.d);
            POLL_INT
            DONE


            // Instruction: CPY
            //
            // Operation:   Y-M
            //
            // Flags:       N Z C I D V
            //              / / / - - -

        case CPY_imm:
            
            READ_IMMEDIATE
            cmp(reg.y, reg.d);
            POLL_INT
            DONE

        case CPY_zpg_2:
            
            READ_FROM_ZERO_PAGE
            cmp(reg.y, reg.d);
            POLL_INT
            DONE

        case CPY_abs_3:
            
            READ_FROM_ADDRESS
            cmp(reg.y, reg.d);
            POLL_INT
            DONE


            // Instruction: DEC
            //
            // Operation:   M := : M - 1
            //
            // Flags:       N Z C I D V
            //              / / - - - -
            
#define DO_DEC reg.d--;
            
        case DEC_zpg_3:
        case DEC_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_DEC
            CONTINUE
            
        case DEC_zpg_4:
        case DEC_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            POLL_INT
            DONE
            
        case DEC_abs_4:
        case DEC_abs_x_5:
        case DEC_ind_x_6:
            
            WRITE_TO_ADDRESS
            DO_DEC
            CONTINUE
            
        case DEC_abs_5:
        case DEC_abs_x_6:
        case DEC_ind_x_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE


            // Instruction: DEX
            //
            // Operation:   X := X - 1
            //
            // Flags:       N Z C I D V
            //              / / - - - -

        case DEX:
            
            IDLE_READ_IMPLIED
            loadX(reg.x - 1);
            POLL_INT
            DONE
            
            
            // Instruction: DEY
            //
            // Operation:   Y := Y - 1
            //
            // Flags:       N Z C I D V
            //              / / - - - -

        case DEY:
            
            IDLE_READ_IMPLIED
            loadY(reg.y - 1);
            POLL_INT
            DONE


            // Instruction: EOR
            //
            // Operation:   A := A XOR M
            //
            // Flags:       N Z C I D V
            //              / / - - - -

#define DO_EOR loadA(reg.a ^ reg.d);
            
        case EOR_imm:
            
            READ_IMMEDIATE
            DO_EOR
            POLL_INT
            DONE
            
        case EOR_zpg_2:
        case EOR_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            DO_EOR
            POLL_INT
            DONE
            
        case EOR_abs_x_3:
        case EOR_abs_y_3:
        case EOR_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                DO_EOR
                POLL_INT
                DONE
            }

        case EOR_abs_3:
        case EOR_abs_x_4:
        case EOR_abs_y_4:
        case EOR_ind_x_5:
        case EOR_ind_y_5:
            
            READ_FROM_ADDRESS
            DO_EOR
            POLL_INT
            DONE


            // Instruction: INC
            //
            // Operation:   M := M + 1
            //
            // Flags:       N Z C I D V
            //              / / - - - -
            
#define DO_INC reg.d++;
            
        case INC_zpg_3:
        case INC_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_INC
            CONTINUE
            
        case INC_zpg_4:
        case INC_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            POLL_INT
            DONE

        case INC_abs_4:
        case INC_abs_x_5:
        case INC_ind_x_6:
            
            WRITE_TO_ADDRESS
            DO_INC
            CONTINUE
            
        case INC_abs_5:
        case INC_abs_x_6:
        case INC_ind_x_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE
            

            // Instruction: INX
            //
            // Operation:   X := X + 1
            //
            // Flags:       N Z C I D V
            //              / / - - - -

        case INX:
            
            IDLE_READ_IMPLIED
            loadX(reg.x + 1);
            POLL_INT
            DONE


            // Instruction: INY
            //
            // Operation:   Y := Y + 1
            //
            // Flags:       N Z C I D V
            //              / / - - - -

        case INY:
            
            IDLE_READ_IMPLIED
            loadY(reg.y + 1);
            POLL_INT
            DONE


            // Instruction: JMP
            //
            // Operation:   PC := Operand
            //
            // Flags:       N Z C I D V
            //              - - - - - -

        case JMP_abs:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case JMP_abs_2:
            
            FETCH_ADDR_HI
            reg.pc = LO_HI(reg.adl, reg.adh);
            POLL_INT
            DONE

        case JMP_abs_ind:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case JMP_abs_ind_2:
            
            FETCH_ADDR_HI
            CONTINUE
            
        case JMP_abs_ind_3:
            
            READ_FROM_ADDRESS
            SET_PCL(reg.d);
            reg.adl++;
            CONTINUE
            
        case JMP_abs_ind_4:
            
            READ_FROM_ADDRESS
            SET_PCH(reg.d);
            POLL_INT
            DONE

            
            // Instruction: JSR
            //
            // Operation:   PC to stack, PC := Operand
            //
            // Flags:       N Z C I D V
            //              - - - - - -
            
        case JSR:
            
            FETCH_ADDR_LO
            CONTINUE
            
        case JSR_2:
            
            IDLE_PULL
            CONTINUE
            
        case JSR_3:
            
            PUSH_PCH
            CONTINUE
            
        case JSR_4:
            
            PUSH_PCL
            CONTINUE
            
        case JSR_5:
            
            FETCH_ADDR_HI
            reg.pc = LO_HI(reg.adl, reg.adh);
            POLL_INT
            DONE

            
            // Instruction: LDA
            //
            // Operation:   A := M
            //
            // Flags:       N Z C I D V
            //              / / - - - -

        case LDA_imm:
            
            READ_IMMEDIATE
            loadA(reg.d);
            POLL_INT
            DONE

        case LDA_zpg_2:
        case LDA_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            loadA(reg.d);
            POLL_INT
            DONE

        case LDA_abs_x_3:
        case LDA_abs_y_3:
        case LDA_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadA(reg.d);
                POLL_INT
                DONE
            }
            
        case LDA_abs_3:
        case LDA_abs_x_4:
        case LDA_abs_y_4:
        case LDA_ind_x_5:
        case LDA_ind_y_5:
            
            READ_FROM_ADDRESS
            loadA(reg.d);
            POLL_INT
            DONE

            
            // Instruction: LDX
            //
            // Operation:   X := M
            //
            // Flags:       N Z C I D V
            //              / / - - - -

        case LDX_imm:
            
            READ_IMMEDIATE
            loadX(reg.d);
            POLL_INT
            DONE

        case LDX_zpg_2:
        case LDX_zpg_y_3:
            
            READ_FROM_ZERO_PAGE
            loadX(reg.d);
            POLL_INT
            DONE

        case LDX_abs_y_3:
        case LDX_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadX(reg.d);
                POLL_INT
                DONE
            }
            
        case LDX_abs_3:
        case LDX_abs_y_4:
        case LDX_ind_x_5:
        case LDX_ind_y_5:
            
            READ_FROM_ADDRESS
            loadX(reg.d);
            POLL_INT
            DONE
            
            
            // Instruction: LDY
            //
            // Operation:   Y := M
            //
            // Flags:       N Z C I D V
            //              / / - - - -

        case LDY_imm:
            
            READ_IMMEDIATE
            loadY(reg.d);
            POLL_INT
            DONE
            
        case LDY_zpg_2:
        case LDY_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            loadY(reg.d);
            POLL_INT
            DONE

        case LDY_abs_x_3:
        case LDY_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadY(reg.d);
                POLL_INT
                DONE
            }

        case LDY_abs_3:
        case LDY_abs_x_4:
        case LDY_ind_x_5:
        case LDY_ind_y_5:
            
            READ_FROM_ADDRESS
            loadY(reg.d);
            POLL_INT
            DONE
            

            // Instruction: LSR
            //
            // Operation:   0 -> (A|M >> 1) -> C
            //
            // Flags:       N Z C I D V
            //              0 / / - - -

        case LSR_acc:
            
            IDLE_READ_IMPLIED
            setC(reg.a & 1); loadA(reg.a >> 1);
            POLL_INT
            DONE

        case LSR_zpg_3:
        case LSR_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            setC(reg.d & 1); reg.d = reg.d >> 1;
            CONTINUE
            
        case LSR_zpg_4:
        case LSR_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            POLL_INT
            DONE
            
        case LSR_abs_4:
        case LSR_abs_x_5:
        case LSR_abs_y_5:
        case LSR_ind_x_6:
        case LSR_ind_y_6:
            
            WRITE_TO_ADDRESS
            setC(reg.d & 1); reg.d = reg.d >> 1;
            CONTINUE
            
        case LSR_abs_5:
        case LSR_abs_x_6:
        case LSR_abs_y_6:
        case LSR_ind_x_7:
        case LSR_ind_y_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE
            

            // Instruction: NOP
            //
            // Operation:   No operation
            //
            // Flags:       N Z C I D V
            //              - - - - - -

        case NOP:
            
            IDLE_READ_IMPLIED
            POLL_INT
            DONE

        case NOP_imm:
            
            IDLE_READ_IMMEDIATE
            POLL_INT
            DONE

        case NOP_zpg_2:
        case NOP_zpg_x_3:
            
            IDLE_READ_FROM_ZERO_PAGE
            POLL_INT
            DONE
            
        case NOP_abs_x_3:
            
            IDLE_READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                POLL_INT
                DONE
            }
            
        case NOP_abs_3:
        case NOP_abs_x_4:
            
            IDLE_READ_FROM_ADDRESS
            POLL_INT
            DONE
            

            // Instruction: ORA
            //
            // Operation:   A := A v M
            //
            // Flags:       N Z C I D V
            //              / / - - - -

        case ORA_imm:
            
            READ_IMMEDIATE
            loadA(reg.a | reg.d);
            POLL_INT
            DONE
            
        case ORA_zpg_2:
        case ORA_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            loadA(reg.a | reg.d);
            POLL_INT
            DONE

        case ORA_abs_x_3:
        case ORA_abs_y_3:
        case ORA_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadA(reg.a | reg.d);
                POLL_INT
                DONE
            }
            
        case ORA_abs_3:
        case ORA_abs_x_4:
        case ORA_abs_y_4:
        case ORA_ind_x_5:
        case ORA_ind_y_5:
            
            READ_FROM_ADDRESS
            loadA(reg.a | reg.d);
            POLL_INT
            DONE
            
            
            // Instruction: PHA
            //
            // Operation:   A to stack
            //
            // Flags:       N Z C I D V
            //              - - - - - -
            
        case PHA_2:
            
            PUSH_A
            POLL_INT
            DONE

            
            // Instruction: PHA
            //
            // Operation:   P to stack
            //
            // Flags:       N Z C I D V
            //              - - - - - -
            
        case PHP_2:
            
            PUSH_P
            POLL_INT
            DONE

            
            // Instruction: PLA
            //
            // Operation:   Stack to A
            //
            // Flags:       N Z C I D V
            //              - - - - - -
            
        case PLA_2:
            
            reg.sp++;
            CONTINUE
            
        case PLA_3:
            
            PULL_A
            POLL_INT
            DONE

            
            // Instruction: PLP
            //
            // Operation:   Stack to p
            //
            // Flags:       N Z C I D V
            //              / / / / / /
            
        case PLP_2:

            IDLE_PULL
            reg.sp++;
            CONTINUE
            
        case PLP_3:

            POLL_INT // Interrupts are polled before P is pulled
            PULL_P
            DONE

            
            // Instruction: ROL
            //
            //              -----------------------
            //              |                     |
            // Operation:   ---(A|M << 1) <- C <---
            //
            // Flags:       N Z C I D V
            //              / / / - - -

#define DO_ROL_ACC { u8 c = !!getC(); setC(reg.a & 0x80); loadA((u8)(reg.a << 1 | c)); }
#define DO_ROL { u8 c = !!getC(); setC(reg.d & 0x80); reg.d = (u8)(reg.d << 1 | c); }

        case ROL_acc:
            
            IDLE_READ_IMPLIED
            DO_ROL_ACC
            POLL_INT
            DONE
            
        case ROL_zpg_3:
        case ROL_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_ROL
            CONTINUE
            
        case ROL_zpg_4:
        case ROL_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            POLL_INT
            DONE
            
        case ROL_abs_4:
        case ROL_abs_x_5:
        case ROL_ind_x_6:
            
            WRITE_TO_ADDRESS
            DO_ROL
            CONTINUE
            
        case ROL_abs_5:
        case ROL_abs_x_6:
        case ROL_ind_x_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE


            // Instruction: ROR
            //
            //              -----------------------
            //              |                     |
            // Operation:   --->(A|M >> 1) -> C ---
            //
            // Flags:       N Z C I D V
            //              / / / - - -

#define DO_ROR_ACC { u8 c = !!getC(); setC(reg.a & 0x1); loadA((u8)(reg.a >> 1 | c << 7)); }
#define DO_ROR { u8 c = !!getC(); setC(reg.d & 0x1); reg.d = (u8)(reg.d >> 1 | c << 7); }
            
        case ROR_acc:
            
            IDLE_READ_IMPLIED
            DO_ROR_ACC
            POLL_INT
            DONE
            
        case ROR_zpg_3:
        case ROR_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_ROR
            CONTINUE
            
        case ROR_zpg_4:
        case ROR_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            POLL_INT
            DONE
            
        case ROR_abs_4:
        case ROR_abs_x_5:
        case ROR_ind_x_6:
            
            WRITE_TO_ADDRESS
            DO_ROR
            CONTINUE
            
        case ROR_abs_5:
        case ROR_abs_x_6:
        case ROR_ind_x_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            POLL_INT
            DONE

            
            // Instruction: RTI
            //
            // Operation:   P from Stack, PC from Stack
            //
            // Flags:       N Z C I D V
            //              / / / / / /
            
        case RTI_2:
            
            IDLE_PULL
            reg.sp++;
            CONTINUE
            
        case RTI_3:
            
            PULL_P
            reg.sp++;
            CONTINUE
            
        case RTI_4:
            
            PULL_PCL
            reg.sp++;
            CONTINUE
            
        case RTI_5:
            
            PULL_PCH
            POLL_INT
            DONE


            // Instruction: RTS
            //
            // Operation:   PC from Stack
            //
            // Flags:       N Z C I D V
            //              - - - - - -
            
        case RTS_2:
            
            IDLE_PULL
            reg.sp++;
            CONTINUE
            
        case RTS_3:
            
            PULL_PCL
            reg.sp++;
            CONTINUE
            
        case RTS_4:
            
            PULL_PCH
            CONTINUE
            
        case RTS_5:
            
            IDLE_READ_IMMEDIATE
            POLL_INT
            DONE

            
            // Instruction: SBC
            //
            // Operation:   A := A - M - (~C)
            //
            // Flags:       N Z C I D V
            //              / / / - - /

        case SBC_imm:
            
            READ_IMMEDIATE
            sbc(reg.d);
            POLL_INT
            DONE
            
        case SBC_zpg_2:
        case SBC_zpg_x_3:
            
            READ_FROM_ZERO_PAGE
            sbc(reg.d);
            POLL_INT
            DONE
            
        case SBC_abs_x_3:
        case SBC_abs_y_3:
        case SBC_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                sbc(reg.d);
                POLL_INT
                DONE
            }
            
        case SBC_abs_3:
        case SBC_abs_x_4:
        case SBC_abs_y_4:
        case SBC_ind_x_5:
        case SBC_ind_y_5:
            
            READ_FROM_ADDRESS
            sbc(reg.d);
            POLL_INT
            DONE


            // Instruction: SEC
            //
            // Operation:   C := 1
            //
            // Flags:       N Z C I D V
            //              - - 1 - - -

        case SEC:
            
            IDLE_READ_IMPLIED
            setC(1);
            POLL_INT
            DONE

            
            // Instruction: SED
            //
            // Operation:   D := 1
            //
            // Flags:       N Z C I D V
            //              - - - - 1 -

        case SED:
            
            IDLE_READ_IMPLIED
            setD(1);
            POLL_INT
            DONE

            
            // Instruction: SEI
            //
            // Operation:   I := 1
            //
            // Flags:       N Z C I D V
            //              - - - 1 - -

        case SEI:
            
            POLL_IRQ
            setI(1);
            [[fallthrough]];
            
        case SEI_cont:
            
            next = SEI_cont;
            IDLE_READ_IMPLIED
            POLL_NMI
            DONE
            

            // Instruction: STA
            //
            // Operation:   M := A
            //
            // Flags:       N Z C I D V
            //              - - - - - -
            
        case STA_zpg_2:
        case STA_zpg_x_3:
            
            reg.d = reg.a;
            WRITE_TO_ZERO_PAGE
            POLL_INT
            DONE
            
        case STA_abs_3:
        case STA_abs_x_4:
            
            reg.d = reg.a;
            WRITE_TO_ADDRESS
            POLL_INT
            DONE
            
        case STA_abs_y_4:
        case STA_ind_x_5:
        case STA_ind_y_5:
            
            reg.d = reg.a;
            WRITE_TO_ADDRESS
            POLL_INT
            DONE


            // Instruction: STX
            //
            // Operation:   M := X
            //
            // Flags:       N Z C I D V
            //              - - - - - -
            
        case STX_zpg_2:
        case STX_zpg_y_3:
            
            reg.d = reg.x;
            WRITE_TO_ZERO_PAGE
            POLL_INT
            DONE
            
        case STX_abs_3:
            
            reg.d = reg.x;
            WRITE_TO_ADDRESS
            POLL_INT
            DONE

            
            // Instruction: STY
            //
            // Operation:   M := Y
            //
            // Flags:       N Z C I D V
            //              - - - - - -
            
        case STY_zpg_2:
        case STY_zpg_x_3:
            
            reg.d = reg.y;
            WRITE_TO_ZERO_PAGE
            POLL_INT
            DONE
            
        case STY_abs_3:
            
            reg.d = reg.y;
            WRITE_TO_ADDRESS
            POLL_INT
            DONE


            // Instruction: TAX
            //
            // Operation:   X := A
            //
            // Flags:       N Z C I D V
            //              / / - - - -

        case TAX:
            
            IDLE_READ_IMPLIED
            loadX(reg.a);
            POLL_INT
            DONE

            
            // Instruction: TAY
            //
            // Operation:   Y := A
            //
            // Flags:       N Z C I D V
            //              / / - - - -

        case TAY:
            
            IDLE_READ_IMPLIED
            loadY(reg.a);
            POLL_INT
            DONE


            // Instruction: TSX
            //
            // Operation:   X := Stack pointer
            //
            // Flags:       N Z C I D V
            //              / / - - - -

        case TSX:
            
            IDLE_READ_IMPLIED
            loadX(reg.sp);
            POLL_INT
            DONE


            // Instruction: TXA
            //
            // Operation:   A := X
            //
            // Flags:       N Z C I D V
            //              / / - - - -

        case TXA:
            
            IDLE_READ_IMPLIED
            loadA(reg.x);
            POLL_INT
            DONE


            // Instruction: TXS
            //
            // Operation:   Stack pointer := X
            //
            // Flags:       N Z C I D V
            //              - - - - - -

        case TXS:
            
            IDLE_READ_IMPLIED
            reg.sp = reg.x;
            POLL_INT
            DONE


            // Instruction: TYA
            //
            // Operation:   A := Y
            //
            // Flags:       N Z C I D V
            //              / / - - - -

        case TYA:
            
            IDLE_READ_IMPLIED
            loadA(reg.y);
            POLL_INT
            DONE

            
            //
            // Illegal instructions
            //

            
            // Instruction: ALR
            //
            // Operation:   AND, followed by LSR
            //
            // Flags:       N Z C I D V
            //              / / / - - -

        case ALR_imm:
            
            READ_IMMEDIATE
            reg.a = reg.a & reg.d;
            setC(reg.a & 1);
            loadA(reg.a >> 1);
            POLL_INT
            DONE


            // Instruction: ANC
            //
            // Operation:   A := A & op,   N flag is copied to C
            //
            // Flags:       N Z C I D V
            //              / / / - - -

        case ANC_imm:
            
            READ_IMMEDIATE
            loadA(reg.a & reg.d);
            setC(getN());
            POLL_INT
            DONE


            // Instruction: ARR
            //
            // Operation:   AND, followed by ROR
            //
            // Flags:       N Z C I D V
            //              / / / - - /

        case ARR_imm:
        {
            READ_IMMEDIATE
            
            u8 tmp2 = reg.a & reg.d;
            
            // Taken from Frodo...
            reg.a = (getC() ? (tmp2 >> 1) | 0x80 : tmp2 >> 1);
            if (!getD()) {
                setN(reg.a & 0x80);
                setZ(reg.a == 0);
                setC(reg.a & 0x40);
                setV((reg.a & 0x40) ^ ((reg.a & 0x20) << 1));
            } else {
                int c_flag;
                
                setN(getC());
                setZ(reg.a == 0);
                setV((tmp2 ^ reg.a) & 0x40);
                if ((tmp2 & 0x0f) + (tmp2 & 0x01) > 5)
                    reg.a = (reg.a & 0xf0) | ((reg.a + 6) & 0x0f);
                c_flag = (tmp2 + (tmp2 & 0x10)) & 0x1f0;
                if (c_flag > 0x50) {
                    setC(1);
                    reg.a += 0x60;
                } else {
                    setC(0);
                }
            }
            POLL_INT
            DONE
        }


            // Instruction: AXS
            //
            // Operation:   X = (A & X) - op
            //
            // Flags:       N Z C I D V
            //              / / / - - -

        case AXS_imm:
        {
            READ_IMMEDIATE
            
            u8 op2  = reg.a & reg.x;
            u8 tmp = op2 - reg.d;
            
            setC(op2 >= reg.d);
            loadX(tmp);
            POLL_INT
            DONE
        }


            // Instruction: DCP
            //
            // Operation:   DEC followed by CMP
            //
            // Flags:       N Z C I D V
            //              / / / - - -
            
        case DCP_zpg_3:
        case DCP_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            reg.d--;
            CONTINUE
            
        case DCP_zpg_4:
        case DCP_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            cmp(reg.a, reg.d);
            POLL_INT
            DONE
            
        case DCP_abs_4:
        case DCP_abs_x_5:
        case DCP_abs_y_5:
        case DCP_ind_x_6:
        case DCP_ind_y_6:
            
            WRITE_TO_ADDRESS
            reg.d--;
            CONTINUE
            
        case DCP_abs_5:
        case DCP_abs_x_6:
        case DCP_abs_y_6:
        case DCP_ind_x_7:
        case DCP_ind_y_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            cmp(reg.a, reg.d);
            POLL_INT
            DONE


            // Instruction: ISC
            //
            // Operation:   INC followed by SBC
            //
            // Flags:       N Z C I D V
            //              / / / - - /
            
        case ISC_zpg_3:
        case ISC_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            reg.d++;
            CONTINUE
            
        case ISC_zpg_4:
        case ISC_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE_AND_SET_FLAGS
            sbc(reg.d);
            POLL_INT
            DONE

        case ISC_abs_4:
        case ISC_abs_x_5:
        case ISC_abs_y_5:
        case ISC_ind_x_6:
        case ISC_ind_y_6:
            
            WRITE_TO_ADDRESS
            reg.d++;
            CONTINUE
            
        case ISC_abs_5:
        case ISC_abs_x_6:
        case ISC_abs_y_6:
        case ISC_ind_x_7:
        case ISC_ind_y_7:
            
            WRITE_TO_ADDRESS_AND_SET_FLAGS
            sbc(reg.d);
            POLL_INT
            DONE


            // Instruction: LAS
            //
            // Operation:   SP,X,A = op & SP
            //
            // Flags:       N Z C I D V
            //              / / - - - -
            
        case LAS_abs_y_3:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                reg.d &= reg.sp;
                reg.sp = reg.d;
                reg.x = reg.d;
                loadA(reg.d);
                POLL_INT
                DONE
            }
            
        case LAS_abs_y_4:
            
            READ_FROM_ADDRESS
            reg.d &= reg.sp;
            reg.sp = reg.d;
            reg.x = reg.d;
            loadA(reg.d);
            POLL_INT
            DONE

            
            // Instruction: LAX
            //
            // Operation:   LDA, followed by LDX
            //
            // Flags:       N Z C I D V
            //              / / - - - -
            
        case LAX_zpg_2:
        case LAX_zpg_y_3:
            
            READ_FROM_ZERO_PAGE
            loadA(reg.d);
            loadX(reg.d);
            POLL_INT
            DONE
            
        case LAX_abs_y_3:
        case LAX_ind_y_4:
            
            READ_FROM_ADDRESS
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI
                CONTINUE
            } else {
                loadA(reg.d);
                loadX(reg.d);
                POLL_INT
                DONE
            }
            
        case LAX_abs_3:
        case LAX_abs_y_4:
        case LAX_ind_x_5:
        case LAX_ind_y_5:
            
            READ_FROM_ADDRESS;
            loadA(reg.d);
            loadX(reg.d);
            POLL_INT
            DONE

            
            // Instruction: RLA
            //
            // Operation:   ROL, followed by AND
            //
            // Flags:       N Z C I D V
            //              / / / - - -
            
        case RLA_zpg_3:
        case RLA_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_ROL
            CONTINUE
            
        case RLA_zpg_4:
        case RLA_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE
            loadA(reg.a & reg.d);
            POLL_INT
            DONE
            
        case RLA_abs_4:
        case RLA_abs_x_5:
        case RLA_abs_y_5:
        case RLA_ind_x_6:
        case RLA_ind_y_6:
            
            WRITE_TO_ADDRESS
            DO_ROL
            CONTINUE
            
        case RLA_abs_5:
        case RLA_abs_x_6:
        case RLA_abs_y_6:
        case RLA_ind_x_7:
        case RLA_ind_y_7:
            
            WRITE_TO_ADDRESS
            loadA(reg.a & reg.d);
            POLL_INT
            DONE

            // Instruction: RRA
            //
            // Operation:   ROR, followed by ADC
            //
            // Flags:       N Z C I D V
            //              / / / - - /
            
        case RRA_zpg_3:
        case RRA_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_ROR
            CONTINUE
            
        case RRA_zpg_4:
        case RRA_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE
            adc(reg.d);
            POLL_INT
            DONE

        case RRA_abs_4:
        case RRA_abs_x_5:
        case RRA_abs_y_5:
        case RRA_ind_x_6:
        case RRA_ind_y_6:
            
            WRITE_TO_ADDRESS
            DO_ROR
            CONTINUE
            
        case RRA_abs_5:
        case RRA_abs_x_6:
        case RRA_abs_y_6:
        case RRA_ind_x_7:
        case RRA_ind_y_7:
            
            WRITE_TO_ADDRESS
            adc(reg.d);
            POLL_INT
            DONE

            
            // Instruction: SAX
            //
            // Operation:   Mem := A & X
            //
            // Flags:       N Z C I D V
            //              - - - - - -
            
        case SAX_zpg_2:
        case SAX_zpg_y_3:
            
            reg.d = reg.a & reg.x;
            WRITE_TO_ZERO_PAGE
            POLL_INT
            DONE

        case SAX_abs_3:
        case SAX_ind_x_5:
            
            reg.d = reg.a & reg.x;
            WRITE_TO_ADDRESS
            POLL_INT
            DONE


            // Instruction: SHA
            //
            // Operation:   Mem := A & X & (M + 1)
            //
            // Flags:       N Z C I D V
            //              - - - - - -
            
        case SHA_abs_y_3:
            
            IDLE_READ_FROM_ADDRESS
            
            /* "There are two unstable conditions, the first is when a DMA is
             *  going on while the instruction executes (the CPU is halted by
             *  the VIC-II) then the & M+1 part drops off."
             */
            
            reg.d = reg.a & reg.x & (rdyLineUp == clock ? 0xFF : reg.adh + 1);
            
            /* "The other unstable condition is when the addressing/indexing
             *  causes a page boundary crossing, in that case the highbyte of
             *  the target address may become equal to the value stored."
             */
            
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI;
                reg.adh = reg.a & reg.x & reg.adh;
            }
            
            CONTINUE
            
        case SHA_abs_y_4:
            
            WRITE_TO_ADDRESS
            POLL_INT
            DONE
            
        case SHA_ind_y_4:
            
            IDLE_READ_FROM_ADDRESS
            
            /* "There are two unstable conditions, the first is when a DMA is
             *  going on while the instruction executes (the CPU is halted by
             *  the VIC-II) then the & M+1 part drops off."
             */
            
            reg.d = reg.a & reg.x & (rdyLineUp == clock ? 0xFF : reg.adh + 1);
            
            /* "The other unstable condition is when the addressing/indexing
             *  causes a page boundary crossing, in that case the highbyte of
             *  the target address may become equal to the value stored."
             */
            
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI;
                reg.adh = reg.a & reg.x & reg.adh;
            }

            CONTINUE
            
        case SHA_ind_y_5:
            
            WRITE_TO_ADDRESS
            POLL_INT
            DONE


            // Instruction: SHX
            //
            // Operation:   Mem := X & (HI_BYTE(op) + 1)
            //
            // Flags:       N Z C I D V
            //              - - - - - -

        case SHX_abs_y_3:
            
            IDLE_READ_FROM_ADDRESS
            
            /* "There are two unstable conditions, the first is when a DMA is
             *  going on while the instruction executes (the CPU is halted by
             *  the VIC-II) then the & M+1 part drops off."
             */
            
            reg.d = reg.x & (rdyLineUp == clock ? 0xFF : reg.adh + 1);
            
            /* "The other unstable condition is when the addressing/indexing
             *  causes a page boundary crossing, in that case the highbyte of
             *  the target address may become equal to the value stored."
             */
            
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI;
                reg.adh = reg.x & reg.adh;
            }
            
            CONTINUE

        case SHX_abs_y_4:
            
            WRITE_TO_ADDRESS
            POLL_INT
            DONE


            // Instruction: SHY
            //
            // Operation:   Mem := Y & (HI_BYTE(op) + 1)
            //
            // Flags:       N Z C I D V
            //              - - - - - -
            
        case SHY_abs_x_3:
            
            IDLE_READ_FROM_ADDRESS
            
            /* "There are two unstable conditions, the first is when a DMA is
             *  going on while the instruction executes (the CPU is halted by
             *  the VIC-II) then the & M+1 part drops off."
             */
            
            reg.d = reg.y & (rdyLineUp == clock ? 0xFF : reg.adh + 1);
            
            /* "The other unstable condition is when the addressing/indexing
             *  causes a page boundary crossing, in that case the highbyte of
             *  the target address may become equal to the value stored."
             */
            
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI;
                reg.adh = reg.y & reg.adh;
            }

            CONTINUE
            
        case SHY_abs_x_4:
            
            WRITE_TO_ADDRESS
            POLL_INT
            DONE


            // Instruction: SLO (ASO)
            //
            // Operation:   ASL memory location, followed by OR on accumulator
            //
            // Flags:       N Z C I D V
            //              / / / - - -

#define DO_SLO setC(reg.d & 128); reg.d <<= 1;

        case SLO_zpg_3:
        case SLO_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_SLO
            CONTINUE
            
        case SLO_zpg_4:
        case SLO_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE
            loadA(reg.a | reg.d);
            POLL_INT
            DONE
            
        case SLO_abs_4:
        case SLO_abs_x_5:
        case SLO_abs_y_5:
        case SLO_ind_x_6:
        case SLO_ind_y_6:
            
            WRITE_TO_ADDRESS
            DO_SLO
            CONTINUE
            
        case SLO_abs_5:
        case SLO_abs_x_6:
        case SLO_abs_y_6:
        case SLO_ind_x_7:
        case SLO_ind_y_7:
            
            WRITE_TO_ADDRESS
            loadA(reg.a | reg.d);
            POLL_INT
            DONE
            

            // Instruction: SRE (LSE)
            //
            // Operation:   LSR, followed by EOR
            //
            // Flags:       N Z C I D V
            //              / / / - - -

#define DO_SRE setC(reg.d & 1); reg.d >>= 1;

        case SRE_zpg_3:
        case SRE_zpg_x_4:
            
            WRITE_TO_ZERO_PAGE
            DO_SRE
            CONTINUE
            
        case SRE_zpg_4:
        case SRE_zpg_x_5:
            
            WRITE_TO_ZERO_PAGE
            loadA(reg.a ^ reg.d);
            POLL_INT
            DONE
            
        case SRE_abs_4:
        case SRE_abs_x_5:
        case SRE_abs_y_5:
        case SRE_ind_x_6:
        case SRE_ind_y_6:
            
            WRITE_TO_ADDRESS
            DO_SRE
            CONTINUE
            
        case SRE_abs_5:
        case SRE_abs_x_6:
        case SRE_abs_y_6:
        case SRE_ind_x_7:
        case SRE_ind_y_7:
            
            WRITE_TO_ADDRESS
            loadA(reg.a ^ reg.d);
            POLL_INT
            DONE


            // Instruction: TAS (SHS)
            //
            // Operation:   SP := A & X,  Mem := SP & (HI_BYTE(op) + 1)
            //
            // Flags:       N Z C I D V
            //              - - - - - -
            
        case TAS_abs_y_3:
            
            IDLE_READ_FROM_ADDRESS
            
            reg.sp = reg.a & reg.x;
            
            /* "There are two unstable conditions, the first is when a DMA is
             *  going on while the instruction executes (the CPU is halted by
             *  the VIC-II) then the & M+1 part drops off."
             */
            
            reg.d = reg.a & reg.x & (rdyLineUp == clock ? 0xFF : reg.adh + 1);
            
            /* "The other unstable condition is when the addressing/indexing
             *  causes a page boundary crossing, in that case the highbyte of
             *  the target address may become equal to the value stored."
             */
            
            if (PAGE_BOUNDARY_CROSSED) {
                FIX_ADDR_HI;
                reg.adh = reg.a & reg.x & reg.adh;
            }

            CONTINUE
            
        case TAS_abs_y_4:
            
            WRITE_TO_ADDRESS
            POLL_INT
            DONE

            // Instruction: ANE
            //
            // Operation:   A = X & op & (A | 0xEE) (taken from Frodo)
            //
            // Flags:       N Z C I D V
            //              / / - - - -

        case ANE_imm:
            
            READ_IMMEDIATE
            loadA(reg.x & reg.d & (reg.a | 0xEE));
            POLL_INT
            DONE


            // Instruction: LXA
            //
            // Operation:   A = X = op & (A | 0xEE) (taken from Frodo)
            //
            // Flags:       N Z C I D V
            //              / / - - - -

        case LXA_imm:
            
            READ_IMMEDIATE
            reg.x = reg.d & (reg.a | 0xEE);
            loadA(reg.x);
            POLL_INT
            DONE
            
        default:
            
            fatalError;
    }
}

void
Peddle::execute(int count)
{
    switch (cpuModel) {

        case MOS_6502: execute<MOS_6502>(count); break;
        case MOS_6507: execute<MOS_6507>(count); break;
        case MOS_6510: execute<MOS_6510>(count); break;
        case MOS_8502: execute<MOS_8502>(count); break;

        default:
            fatalError;
    }
}

template <CPURevision C> void
Peddle::execute(int count)
{
    for (int j = 0; j < count; j++) { execute<C>(); }
}

void
Peddle::executeInstruction()
{
    switch (cpuModel) {

        case MOS_6502: executeInstruction<MOS_6502>(); break;
        case MOS_6507: executeInstruction<MOS_6507>(); break;
        case MOS_6510: executeInstruction<MOS_6510>(); break;
        case MOS_8502: executeInstruction<MOS_8502>(); break;

        default:
            fatalError;
    }
}

template <CPURevision C> void
Peddle::executeInstruction()
{
    // Execute a singe cycle
    execute<C>();

    // Execute more cycles until we reach the fetch phase
    finishInstruction<C>();
}

void
Peddle::executeInstruction(int count)
{
    switch (cpuModel) {

        case MOS_6502: executeInstruction<MOS_6502>(count); break;
        case MOS_6507: executeInstruction<MOS_6507>(count); break;
        case MOS_6510: executeInstruction<MOS_6510>(count); break;
        case MOS_8502: executeInstruction<MOS_8502>(count); break;

        default:
            fatalError;
    }
}

template <CPURevision C> void
Peddle::executeInstruction(int count)
{
    for (int j = 0; j < count; j++) { executeInstruction<C>(); }
}

void
Peddle::finishInstruction()
{
    switch (cpuModel) {

        case MOS_6502: finishInstruction<MOS_6502>(); break;
        case MOS_6507: finishInstruction<MOS_6507>(); break;
        case MOS_6510: finishInstruction<MOS_6510>(); break;
        case MOS_8502: finishInstruction<MOS_8502>(); break;

        default:
            fatalError;
    }
}

template <CPURevision C> void
Peddle::finishInstruction()
{
    while (!inFetchPhase()) execute<C>();
}

template <CPURevision C> void
Peddle::done() {

    if (flags) {

        if (flags & CPU_LOG_INSTRUCTION) {

            debugger.logInstruction();
            instructionLogged();
        }

        if ((flags & CPU_CHECK_BP) && debugger.breakpointMatches(reg.pc)) {

            breakpointReached(reg.pc);
        }
    }

    reg.pc0 = reg.pc;
    next = fetch;
}
