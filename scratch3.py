import sys
import struct

with open("extracted_abl.bin", "rb") as f:
    data = f.read()

target = 0x58bd5

for i in range(0, len(data) - 4, 4):
    inst = struct.unpack("<I", data[i:i+4])[0]
    
    # ADRP
    if (inst & 0x9F000000) == 0x90000000:
        rd = inst & 0x1F
        immlo = (inst >> 29) & 3
        immhi = (inst >> 5) & 0x7FFFF
        imm = (immhi << 2) | immlo
        if imm & 0x100000: imm -= 0x200000
        imm <<= 12
        page = (i & ~0xFFF) + imm
        
        # Check next instruction for ADD (Xn, #imm)
        inst2 = struct.unpack("<I", data[i+4:i+8])[0]
        if (inst2 & 0xFF000000) == 0x91000000:
            rd2 = inst2 & 0x1F
            rn2 = (inst2 >> 5) & 0x1F
            imm2 = (inst2 >> 10) & 0xFFF
            shift2 = (inst2 >> 22) & 3
            if shift2 == 1: imm2 <<= 12
            
            if rd2 == rd and rn2 == rd:
                addr = page + imm2
                if addr == target:
                    print(f"Found EXACT ADRP+ADD at 0x{i:x}")
