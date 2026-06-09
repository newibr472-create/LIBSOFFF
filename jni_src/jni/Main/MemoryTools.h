#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
#include <dirent.h>
#include <cstring>
#include <cstdlib>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/uio.h>
#include <dirent.h>
#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <sys/mman.h>
#include <ctime>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <linux/memfd.h>
#include <sys/prctl.h>
#include <linux/filter.h>
#include <linux/seccomp.h>
#include <linux/ptrace.h>
#include <sys/prctl.h>
#include <linux/filter.h>
#include <linux/seccomp.h>
#include <linux/ptrace.h>
#define norvalue MADV_SEQUENTIAL|MADV_WILLNEED|MADV_DONTFORK|MADV_DODUMP


using namespace std;
#include <string>
extern "C" long raw_syscall(long __number, ...);

int getMemPermission(uintptr_t address) {
    char line[PATH_MAX] = {0};
    char perms[5];
    int bol = 1;

    FILE *fp = fopen("/proc/self/maps", "r");
    if (fp == nullptr) {
        return 0;
    }

    while (fgets(line, PATH_MAX, fp) != nullptr) {
        unsigned long start, end;
        int eol = (strchr(line, '\n') != nullptr);
        if (bol) {
            if (!eol) {
                bol = 0;
            }
        } else {
            if (eol) {
                bol = 1;
            }
            continue;
        }
        if (sscanf(line, "%lx-%lx %4s", &start, &end, perms) != 3) {
            continue;
        }
        if (start <= address && address < end) {
            int prot = 0;
            if (perms[0] == 'r') {
                prot |= PROT_READ;
            } else if (perms[0] != '-') {
                goto unknown_perms;
            }
            if (perms[1] == 'w') {
                prot |= PROT_WRITE;
            } else if (perms[1] != '-') {
                goto unknown_perms;
            }
            if (perms[2] == 'x') {
                prot |= PROT_EXEC;
            } else if (perms[2] != '-') {
                goto unknown_perms;
            }
            if (perms[3] != 'p') {
                goto unknown_perms;
            }
            if (perms[4] != '\0') {
                perms[4] = '\0';
                goto unknown_perms;
            }
            fclose(fp);
            return prot;
        }
    }
    unknown_perms:
    fclose(fp);
    return 0;
}


void *my_memmove(void *dst, const void *src, size_t count) {
    if (src == nullptr || dst == nullptr) {
        return nullptr;
    }
    char *tmp_dst = (char *) dst;
    char *tmp_src = (char *) src;
    if (tmp_src == nullptr) {
        return nullptr;
    }
    if (tmp_dst + count < src || tmp_src + count < dst) {
        while (count--)
            *tmp_dst++ = *tmp_src++;
    } else {
        tmp_dst += count - 1;
        tmp_src += count - 1;
        while (count--)
            *tmp_dst-- = *tmp_src--;
    }

    return dst;
}

void *my_memcpy(void *dst, const void *src, size_t count) {
    if (src == nullptr || dst == nullptr) {
        return nullptr;
    }
    char *tmp_dst = (char *) dst;
    char *tmp_src = (char *) src;
    if (tmp_src == nullptr) {
        return nullptr;
    }
    while (count--)
        *tmp_dst++ = *tmp_src++;

    return dst;
}

char *my_strcpy(char *dest, const char *src) {
    //assert((dest != NULL) && (src != NULL));
    if (src == nullptr || dest == nullptr) {
        return nullptr;
    }
    const char *end = src;
    while (*end)
        end++;
    my_memcpy(dest, src, end - src + 1);
    return dest;
}

static int random_fd = -1;


bool IsPtrValid(void *addr) {
    if (!addr)
        return false;
    if (random_fd <= 0)
        random_fd = raw_syscall(__NR_memfd_create, "jit-cache",MFD_CLOEXEC | MFD_ALLOW_SEALING);
    return raw_syscall(__NR_write, random_fd, addr, 4) >= 0;
}
bool isSafeAddress(uintptr_t _addr, size_t _size) {
    if (_addr <= 0x10000000 || _addr >= 0x10000000000)
        return false;
    uintptr_t page_start = (_addr) & (~(PAGE_SIZE - 1));
    unsigned char vec[1] = {0};
    int ret = raw_syscall(__NR_mincore, reinterpret_cast<void *>(page_start), PAGE_SIZE, vec);
    if (ret != -22 && vec[0] != 0)
        return false;
    return IsPtrValid(reinterpret_cast<void *>(_addr));
}
 
bool isMemoryTrap(uintptr_t addr)
{
    if (addr < 0x10000000 || addr > 0xFFFFFFFFFF) {
        return false;
    }
    return IsPtrValid(reinterpret_cast<void *>(addr));
}


bool memoryRead(uintptr_t address, void *buffer, int size) {
    memset(buffer, 0, size);
    if (isMemoryTrap(address)) {
    	return my_memmove(buffer, reinterpret_cast<void *>(address), size) != nullptr;
    } else {
    	return false;
    }
}
bool editMemProt(uintptr_t address, int prot) {
    uintptr_t page_start = (address) & (~(PAGE_SIZE - 1));
    return mprotect((void *) page_start, PAGE_SIZE, prot) != -1;
}
void writeBuffer(uintptr_t address, void *buffer, int size) {
    if (address <= 0x10000000 || address >= 0x10000000000)
        return ;
    //修改内存属性
    int prot = getMemPermission(address);
    if (prot == 0)
        return;
    if (isSafeAddress(address, size))
        return;
    if (editMemProt(address, PROT_READ | PROT_WRITE | PROT_EXEC)) // 修改内存段的权限属性 -- 可读|可写|可执行
        return;
    my_memmove((void *) address, buffer, size);
    editMemProt(address, prot);//还原内存权限
}

//[bss]类型模块专用
uintptr_t ReadModule(const char *name, int index)
{
    char fname[128];
    char dname[128];
    char line[1024] = {0};
    long start = 0, end = 0;
    int i = 0, ii = 0, iii = 0;
    if (strstr(name, "bss") != NULL) {
        sscanf(name, "%[^:]", dname);
        i++;
    } else {
        sprintf(dname, "%s", name);
    }
    sprintf(fname, "/proc/self/maps");
    FILE *fp = fopen(fname, "r");
    if (fp) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, dname) != NULL) {
                iii++;
                if (i == 0) {
                    if (iii == index) {
                        sscanf(line, "%lx-%lx", &start, &end);
                        break;
                    }
                } else {
                    ii++;
                }
            }
            if (ii > 0) {
                if (strstr(line, "[anon:.bss]") != NULL) {
                    sscanf(line, "%lx-%lx", &start, &end);
                    break;
                }
            }
        }
        fclose(fp);
    }
    return start;
}
bool ReadAddr(void *addr, void *buffer, size_t length) {
    unsigned long page_size = sysconf(_SC_PAGESIZE);
    unsigned long size = page_size * sizeof(uintptr_t);
    return mprotect((void *) ((uintptr_t) addr - ((uintptr_t) addr % page_size) - page_size), (size_t) size, PROT_EXEC | PROT_READ | PROT_WRITE) == 0 && memcpy(buffer, addr, length) != 0;
}

bool WriteAddr(void *addr, void *buffer, size_t length) {
    unsigned long page_size = sysconf(_SC_PAGESIZE);
    unsigned long size = page_size * sizeof(uintptr_t);
    return mprotect((void *) ((uintptr_t) addr - ((uintptr_t) addr % page_size) - page_size), (size_t) size, PROT_EXEC | PROT_READ | PROT_WRITE) == 0 && memcpy(addr, buffer, length) != 0;
}


bool Write(uintptr_t address, void *buffer, int size) {
    if (isMemoryTrap(address)) {
        return my_memmove(reinterpret_cast<void *>(address), buffer, size) != nullptr;
    }
    return false;
}

template <typename T> T Read(uintptr_t address)
{
  	T temp;
    memoryRead(address, &temp, sizeof(temp));
    return temp;
}

void writefloat(uintptr_t addr,float var){
WriteAddr(reinterpret_cast<void*>(addr),reinterpret_cast<void*>(&var),8);
}

uintptr_t getA(uintptr_t address)
{
	return Read <uintptr_t>(address);
}

float getF(uintptr_t address)
{
	return Read <float>(address);
}

int getI(uintptr_t address)
{
	return Read <int>(address);
}



#pragma clang diagnostic pop
