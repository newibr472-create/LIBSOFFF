/*
 * ReportBlocker.h - Block Violation Reports to Server
 * Library: libanogs.so | Created by: Agent 2
 * NOP the BL calls that build and submit reports
 */
PATCH_LIB("libanogs.so", "0x1D0F68", "1F 20 03 D5"); // NOP report builder
PATCH_LIB("libanogs.so", "0x1D0FA8", "1F 20 03 D5"); // NOP report builder2
PATCH_LIB("libanogs.so", "0x1D0FEC", "1F 20 03 D5"); // NOP report submit
