/** @file redmagic_nx809j.c — RedMagic family OEM patches.

  Empty stub array for RedMagic patches.
  Mode 1 fakelocks the bootloader via QCOM_VERIFIEDBOOT_PROTOCOL,
  bypassing the orange state screen natively, so an explicit ABL instruction
  patch is not currently needed.
**/

#include "../../../Include/Library/PatchDesc.h"
#include "../Internal/ScanLib.h"
#include "../Internal/Encode.h"
#include "../Internal/Arm64Decode.h"
#include "Signatures.h"

STATIC CONST CHAR8 kOrangeWarnStr[] = "Device is unlocked, Skipping boot verification";
STATIC CONST CHAR8 kYellowWarnStr[] = "Your device has loaded a different operating system";

PATCH_OUTCOME
ApplyRedmagicWarningScreens (
  IN OUT UINT8  *Buf,
  IN     UINT32  Size
  )
{
  UINT32          StrOff, AdrpOff, BlockStart, BranchOff;
  SCAN_RESULT     R;
  BOOLEAN         Patched = FALSE;

  /* 1. Find and patch ORANGE warning branch */
  R = ScanFor (Buf, Size, (CONST UINT8 *)kOrangeWarnStr, NULL,
               sizeof (kOrangeWarnStr) - 1, &StrOff);
  if (R == SCAN_FOUND) {
    R = Arm64FindAdrpAddTargeting (Buf, Size, StrOff, /*RestrictToExec=*/TRUE,
                                   &AdrpOff);
    if (R == SCAN_FOUND && AdrpOff >= 12) {
      BlockStart = AdrpOff - 12;
      R = Arm64FindCondBranchTargeting (Buf, Size, BlockStart, /*RestrictToExec=*/TRUE,
                                        &BranchOff);
      if (R == SCAN_FOUND) {
        /* Rewrite conditional branch to NOP (0xD503201F) */
        WriteInstrU32 (Buf, BranchOff, 0xD503201FU);
        Patched = TRUE;
      }
    }
  }

  /* 2. Find and patch YELLOW warning branch */
  R = ScanFor (Buf, Size, (CONST UINT8 *)kYellowWarnStr, NULL,
               sizeof (kYellowWarnStr) - 1, &StrOff);
  if (R == SCAN_FOUND) {
    R = Arm64FindAdrpAddTargeting (Buf, Size, StrOff, /*RestrictToExec=*/TRUE,
                                   &AdrpOff);
    if (R == SCAN_FOUND && AdrpOff >= 12) {
      BlockStart = AdrpOff - 12;
      R = Arm64FindCondBranchTargeting (Buf, Size, BlockStart, /*RestrictToExec=*/TRUE,
                                        &BranchOff);
      if (R == SCAN_FOUND) {
        /* Rewrite conditional branch to NOP (0xD503201F) */
        WriteInstrU32 (Buf, BranchOff, 0xD503201FU);
        Patched = TRUE;
      }
    }
  }

  return Patched ? PATCH_OK : PATCH_MISS;
}

CONST PATCH_DESC kOemRedmagicPatches[] = {
  {
    .Name      = "patch-redmagic-warning-screens",
    .Scope     = SCOPE_OEM_REDMAGIC,
    .Mandatory = FALSE,
    .Apply     = ApplyRedmagicWarningScreens,
  },
};

CONST UINTN kOemRedmagicPatchesCount =
  sizeof (kOemRedmagicPatches) / sizeof (kOemRedmagicPatches[0]);
