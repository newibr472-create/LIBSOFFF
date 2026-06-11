/* Minimal JNI stub for cross-compilation without Android NDK */
#ifndef _JNI_H_
#define _JNI_H_

#include <stdint.h>

#define JNIEXPORT __attribute__((visibility("default")))
#define JNICALL
#define JNI_VERSION_1_6 0x00010006

typedef unsigned char  jboolean;
typedef int            jint;
typedef long long      jlong;
typedef float          jfloat;
typedef double         jdouble;
typedef int            jsize;
typedef void*          jobject;
typedef jobject        jclass;
typedef jobject        jstring;
typedef jobject        jfloatArray;
typedef jobject        jintArray;

class _JNIEnv {
public:
    jfloatArray NewFloatArray(jsize len) { (void)len; return nullptr; }
    void SetFloatArrayRegion(jfloatArray arr, jsize start, jsize len, const jfloat* buf) {
        (void)arr; (void)start; (void)len; (void)buf;
    }
};

typedef _JNIEnv JNIEnv;

struct JavaVM {
    void* reserved;
};

#endif
