//
// Created by seventh on 2020/3/5.
//

#ifndef GSEVENTH_COMMON_SCANNER_H_
#define GSEVENTH_COMMON_SCANNER_H_
struct Scanner {
    static const int BUF_SIZE = 65536;
    char _buf[BUF_SIZE];
    char *curPos;
    FILE *file;
    Scanner(FILE *_file) : curPos(_buf), file(_file) {
        fread(_buf, 1, sizeof(_buf), file);
    }
    inline void ensureCapacity() {
        int size = _buf + BUF_SIZE - curPos;
        if (size < 100) {
            memcpy(_buf, curPos, size);
            fread(_buf + size, 1, BUF_SIZE - size, file);
            curPos = _buf;
        }
    }
    inline int nextInt() {
        ensureCapacity();
        while (*curPos <= ' ')
            ++curPos;
        register bool sign = false;
        if (*curPos == '-') {
            sign = true;
            ++curPos;
        }
        register int res = 0;
        while (*curPos > ' ')
            res = res * 10 + (*(curPos++) & 15);
        return sign ? -res : res;
    }
    inline char nextChar() {
        ensureCapacity();
        while (*curPos <= ' ')
            ++curPos;
        return *(curPos++);
    }
};
#endif //GSEVENTH_COMMON_SCANNER_H_
