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

#ifndef FILEBUFFER_H
#define FILEBUFFER_H

#include "Buffer.hpp"
#include <stdio.h>
#include <string>
using namespace std;

class FileBuffer : public Buffer
{

    public:
        FileBuffer(string filename);
        virtual ~FileBuffer();
        
        virtual void initAutoFlush();
        virtual void initAutoLoad();

        virtual void autoFlushThread();
        virtual void autoLoadThread();

        virtual void destroyAutoFlush();
        virtual void destroyAutoLoad();

    private:
        string filename;
        FILE* file;

};

#endif // FILEBUFFER_H
