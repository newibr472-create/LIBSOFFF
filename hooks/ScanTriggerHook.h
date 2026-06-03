Xynnacore(int64, ScanTrigger, (int64 a1))
{
    if (a1) *(_BYTE *)(a1 + 0x38) = 0;
    return oScanTrigger(a1);
}
HOOK_LIB("libanogs.so", "0x369658", hScanTrigger, oScanTrigger);
