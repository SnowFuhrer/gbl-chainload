import sys
import struct

with open("extracted_abl.bin", "rb") as f:
    data = f.read()

target = 0x58bd5

for i in range(0, len(data) - 4, 4):
    inst = struct.unpack("<I", data[i:i+4])[0]
    
    # ADRP
    if (inst & 0x9F000000) == 0x90000000:
        immlo = (inst >> 29) & 3
        immhi = (inst >> 5) & 0x7FFFF
        imm = (immhi << 2) | immlo
        if imm & 0x100000: imm -= 0x200000
        imm <<= 12
        page = (i & ~0xFFF) + imm
        if page == (target & ~0xFFF):
            print(f"Found ADRP targeting page at 0x{i:x}")
