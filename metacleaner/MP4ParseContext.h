//
//  MP4ParseContext.hpp
//  metacleaner
//
//  Created by disenzhang on 2023/9/22.
//

#ifndef MP4ParseContext_hpp
#define MP4ParseContext_hpp

#include <stdio.h>
#include <stdlib.h>

typedef struct MP4Box {
    uint64_t offset;
    uint64_t size;
    uint32_t type;
} MP4Box;

typedef struct MoovBox {
    uint64_t offset;
    uint64_t size;
    uint32_t type;
} MoovBox;

class MP4ParseContext {
public:
    MP4ParseContext(const char filePath[]);
    void clearMetaValues();
    
private:
    FILE *fileHandle;
    uint64_t fileSize;
    uint32_t keysCount;
    char **metaKeys;
    char **metaValues;
    uint32_t read32();
    void skip(int len);
    void seek(uint64_t pos);
    uint64_t read64();
    char* readbuff(int length);
    void readKeys();
    MP4Box find_box(const char box_name[4]);
};


#endif /* MP4ParseContext_hpp */
