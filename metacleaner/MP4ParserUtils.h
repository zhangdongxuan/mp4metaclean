//
//  MP4ParserUtils.h
//  metacleaner
//
//  Created by disenzhang on 2023/9/22.
//

#ifndef MP4ParserUtils_h
#define MP4ParserUtils_h

#include <stdio.h>
#include <stdlib.h>

extern uint32_t mp4_ch2ut(const char bytes[4]);

extern char *mp4_ut2ch(uint32_t boxType);

extern void mp4_b2l(char *buffer, int length);

#endif /* MP4ParserUtils_h */
