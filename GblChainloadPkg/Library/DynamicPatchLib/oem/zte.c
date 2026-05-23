/** @file zte.c — ZTE/Nubia/Redmagic family OEM patches.

  Bypasses warning screens and delays for ZTE/Nubia/Redmagic devices.
**/

#include "../../../Include/Library/PatchDesc.h"
#include "../Internal/ScanLib.h"
#include "../Internal/Encode.h"
#include "../Internal/Arm64Decode.h"
#include "Signatures.h"

STATIC CONST CHAR8 kOrangeWarnStr[] = "Device is unlocked, Skipping boot verification";
STATIC CONST CHAR8 kYellowWarnStr[] = "Your device has loaded a different operating system";

PATCH_OUTCOME
ApplyZteWarningScreens (
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

CONST PATCH_DESC kOemZtePatches[] = {
  {
    .Name      = "patch-zte-warning-screens",
    .Scope     = SCOPE_OEM_ZTE,
    .Mandatory = FALSE,
    .Apply     = ApplyZteWarningScreens,
  },
};

CONST UINTN kOemZtePatchesCount =
  sizeof (kOemZtePatches) / sizeof (kOemZtePatches[0]);
