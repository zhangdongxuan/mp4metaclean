//
//  MP4ParseContext.cpp
//  metacleaner
//
//  Created by disenzhang on 2023/9/22.
//

#include "MP4ParseContext.h"
#include <string>
extern "C" {
    #include "MP4ParserUtils.h"
}

MP4ParseContext::MP4ParseContext(const char filePath[]) {
    this->fileHandle = fopen(filePath, "rb+");
    if (this->fileHandle == NULL) {
        printf("can't open mp4 file:%s\n", filePath);
        return;
    }
    
    fseek(this->fileHandle, 0, SEEK_END);
    this->fileSize = ftell(this->fileHandle);
    fseek(this->fileHandle, 0, SEEK_SET);
}

void MP4ParseContext::readKeys() {
    MP4Box box = find_box("moov");
    if (box.offset == -1) {
        printf("can't find moov box \n");
        return;
    }
    
    box = find_box("meta");
    if (box.offset == -1) {
        printf("can't find meta box \n");
        return;
    }
    
    box = find_box("keys");
    if (box.offset == -1) {
        printf("can't find keys box \n");
        return;
    }
    
    /** | size(4) | name("keys")(4) | unknow(4) | keys_count(4) | ... */
    skip(12);
    uint64_t offset = box.offset + 12;
    
    this->keysCount = read32() + 1;
    this->metaKeys = (char **)calloc(this->keysCount, sizeof(char *));
    
    /** | item_key_size(4) | item_key_type(4) | item_value(item_key_size - 8) | */
    for (int i = 1; i < this->keysCount; i++) {
        uint32_t item_key_size = read32();
        if (offset + item_key_size > this->fileSize) {
            printf("index:%d current:%llu item_key_size:%u filesize:%llu \n", i, offset, item_key_size, this->fileSize);
            return;
        }
        
        uint32_t item_key_type = read32();
        if (item_key_size < 8) {
            printf("The index:%d key:%s in meta has invalid size:%d \n", i, mp4_ut2ch(item_key_type), item_key_size);
            return;
        }
        
        uint32_t key_size = item_key_size - 8;
        offset += item_key_size;
        
        if (item_key_type != mp4_ch2ut("mdta")) {
            printf("The index:%d is not mdta key:%s\n", i, mp4_ut2ch(item_key_type));
            skip(key_size);
            continue;
        }
        
        this->metaKeys[i] = readbuff(key_size);
    }
}

void MP4ParseContext::clearMetaValues() {
//    https://developer.apple.com/documentation/quicktime-file-format/metadata_item_list_atom
    readKeys();
    MP4Box box = find_box("ilst");
    if (box.offset == -1) {
        printf("can't find keys box \n");
        return;
    }
    
    this->metaValues = (char **)calloc(this->keysCount, sizeof(char *));
    
    uint64_t cursor = ftell(this->fileHandle);
    while (cursor < box.offset + box.size) {
        //   https://developer.apple.com/documentation/quicktime-file-format/metadata_item_list_atom
        /**  | item_size(4) | item_index(4) | kv_size(4) | data_name(4) | data_type(4) | unknow(4) | value(left) | */
        uint32_t item_size = read32();
        uint32_t item_index = read32();
        uint32_t kv_size = read32();
        uint32_t data_name = read32();
        if (data_name != mp4_ch2ut("data")) {
            cursor += item_size;
            seek(cursor);
            continue;
        }
        
        uint32_t data_type = read32();
        skip(4); // unknow
        uint32_t value_size = kv_size - 16;
        this->metaValues[item_index] = readbuff(value_size);
        printf("key:%s value:%s\n", this->metaKeys[item_index], this->metaValues[item_index]);
        seek(cursor + 24);
        memset(this->metaValues[item_index], 0, value_size);
        fwrite(this->metaValues[item_index], value_size, 1, this->fileHandle);
        
        cursor += item_size;
        seek(cursor);
    }
}

MP4Box MP4ParseContext::find_box(const char target_name[4]) {
    MP4Box box;
    box.offset = -1;
    if (this->fileHandle == NULL) {
        printf("file handle is null\n");
        return box;
    }
    
    uint64_t cursor = ftell(this->fileHandle);
    while (cursor < this->fileSize) {
        uint64_t box_size = read32();
        uint32_t box_type = read32();
        
        if (box_size == 1) {
            box_size = read64();
        }
        
        char *box_name = mp4_ut2ch(box_type);
        printf("box name:%s offset:%llu size:%llu\n", box_name, cursor, box_size);
        free(box_name);
        
        if (box_type == mp4_ch2ut(target_name)) {
            box.offset = cursor;
            box.size = box_size;
            box.type = box_type;
            
            return box;
        }
        
        cursor += box_size;
        seek(cursor);
    }
    
    return box;
}

uint32_t MP4ParseContext::read32() {
    uint8_t bytes[4];
    fread(&bytes, 4, 1, this->fileHandle);
    
    uint32_t v = bytes[0];
    v = (v << 8) | bytes[1];
    v = (v << 8) | bytes[2];
    v = (v << 8) | bytes[3];
    
    return v;
}

void MP4ParseContext::skip(int len) {
    fseek(this->fileHandle, 4, SEEK_CUR);
}

void MP4ParseContext::seek(uint64_t pos) {
    if (pos >= this->fileSize) {
        printf("error seek at:%llu filelen:%llu", pos, this->fileSize);
        return;
    }
    fseek(this->fileHandle, pos, SEEK_SET);
}

uint64_t MP4ParseContext::read64() {
    uint64_t v1 = read32();
    uint64_t v2 = read32();
    v1 = (v1 << 32) | v2;
    return v1;
}

char* MP4ParseContext::readbuff(int length) {
    char *buff = (char *)malloc(length);
    fread(buff, length, 1, this->fileHandle);
    return buff;
}
