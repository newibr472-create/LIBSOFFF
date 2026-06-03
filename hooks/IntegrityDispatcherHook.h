Xynnacore(int64, IntegrityDispatcher, (int64 a1))
{
    if (a1) *(_DWORD *)(a1 + 0xC) = 0x0000FFFF;
    return oIntegrityDispatcher(a1);
}
HOOK_LIB("libanogs.so", "0x378FF4", hIntegrityDispatcher, oIntegrityDispatcher);
