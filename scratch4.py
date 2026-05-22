import sys
import struct

with open("extracted_abl.bin", "rb") as f:
    data = f.read()

target = 0x58bd5

for i in range(0, len(data) - 4, 4):
    inst = struct.unpack("<I", data[i:i+4])[0]
    
    if (inst & 0xFF000000) == 0x58000000:
        imm19 = (inst >> 5) & 0x7FFFF
        if imm19 & 0x40000: imm19 -= 0x80000
        addr = i + (imm19 << 2)
        if addr == target:
            print(f"Found LDR PC-rel at 0x{i:x}")
            
