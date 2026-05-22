import sys
import struct

def decode_adrp(inst):
    if (inst & 0x9F000000) != 0x90000000: return None
    rd = inst & 0x1F
    immlo = (inst >> 29) & 3
    immhi = (inst >> 5) & 0x7FFFF
    imm = (immhi << 2) | immlo
    if imm & 0x100000: imm -= 0x200000
    return rd, imm << 12

def decode_add_imm(inst):
    if (inst & 0xFF000000) != 0x91000000: return None
    rd = inst & 0x1F
    rn = (inst >> 5) & 0x1F
    imm = (inst >> 10) & 0xFFF
    shift = (inst >> 22) & 3
    if shift == 1: imm <<= 12
    return rd, rn, imm

with open("extracted_abl.bin", "rb") as f:
    data = f.read()

target = b"Wait for 5 seconds before proceeding"
idx = data.find(target)
if idx == -1:
    print("String not found")
    sys.exit(1)

print(f"String found at offset 0x{idx:x}")

for i in range(0, len(data) - 4, 4):
    inst1 = struct.unpack("<I", data[i:i+4])[0]
    adrp = decode_adrp(inst1)
    if adrp:
        rd, imm = adrp
        # Calculate target page
        page = (i & ~0xFFF) + imm
        
        inst2 = struct.unpack("<I", data[i+4:i+8])[0]
        add = decode_add_imm(inst2)
        if add and add[0] == rd and add[1] == rd:
            target_addr = page + add[2]
            if target_addr == idx:
                print(f"Found ADRP+ADD at 0x{i:x}")
