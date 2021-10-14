/*******************************************************************************
 *
 * Copyright (c) 2010, 2013   Edans Sandes
 *
 * This file is part of CUDAlign.
 * 
 * CUDAlign is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * CUDAlign is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with CUDAlign.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include "FileBuffer.hpp"

#include <stdlib.h>


FileBuffer::FileBuffer(string filename) {
    this->filename = filename;
}

FileBuffer::~FileBuffer() {
    if (file != NULL) {
        Buffer::destroy();
        //fclose(file); // TODO verificar a ordem do fclose no socket e no file.
        //Buffer::joinThreads();
        //fclose(file); // TODO verificar a ordem do fclose no socket e no file.
        //file == NULL;
    }
}

void FileBuffer::autoFlushThread() {
    char data[16*1024];
    
    while (!isDestroyed()) {
        int len = readBuffer(data, 1, sizeof(data));
        if (len <= 0) break;
        fwrite(data, 1, len, file);
    }
    fclose(file);
    file = NULL;
}

void FileBuffer::autoLoadThread() {
    char data[512];
    
    while (!isDestroyed()) {
        int len = fread(data, 1, sizeof(data), file);
        if (len <= 0) break;
        writeBuffer(data, 1, len);
    }
    fclose(file);
    file = NULL;

}

void FileBuffer::initAutoLoad() {
    file = fopen(filename.c_str(), "r");
    if (file == NULL) {
        fprintf(stderr, "File not found (%s).", filename.c_str());
        exit(1);
    }
}

void FileBuffer::initAutoFlush() {
    file = fopen(filename.c_str(), "w");
    if (file == NULL) {
        fprintf(stderr, "File not found (%s).", filename.c_str());
        exit(1);
    }
}

void FileBuffer::destroyAutoFlush() {
}

void FileBuffer::destroyAutoLoad() {
}


