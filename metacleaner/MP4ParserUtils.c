//
//  MP4ParserUtils.c
//  metacleaner
//
//  Created by disenzhang on 2023/9/22.
//

#include "MP4ParserUtils.h"

uint32_t mp4_ch2ut(const char bytes[4]) {
    uint32_t v = bytes[0];
    v = (v << 8) | bytes[1];
    v = (v << 8) | bytes[2];
    v = (v << 8) | bytes[3];
    return v;
}

char *mp4_ut2ch(uint32_t boxType) {
    char *box_name = malloc(4);
    char *tmp = (char *)&boxType;
    box_name[0] = tmp[3];
    box_name[1] = tmp[2];
    box_name[2] = tmp[1];
    box_name[3] = tmp[0];
    
    return box_name;
}

// 从mp4中读取的大端序 转换成 计算机使用的小端序
void mp4_b2l(char *buffer, int length) {
    for (int i = 0; i < length / 2; i ++) {
        char tmp = buffer[i];
        buffer[i] = buffer[length - 1 - i];
        buffer[length - 1 - i] = tmp;
    }
}
