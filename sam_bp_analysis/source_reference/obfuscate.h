#pragma once
// Compile-time string obfuscation shim.
// Real obfuscation (andrivet/ADVobfuscator or similar) is not needed for a local
// development build - we just pass strings through unchanged so the code compiles.
// If you want real obfuscation later, drop in andrivet's header and it will work
// as a drop-in replacement with this same macro name.
#define OBFUSCATE(str) (str)
#define OBF(str)       (str)
