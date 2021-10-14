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

#ifndef SOCKETBUFFER_H
#define SOCKETBUFFER_H

#include "Buffer.hpp"
#include <string>
using namespace std;

class SocketBuffer : public Buffer
{
    public:
        SocketBuffer(string hostname, int port);
        virtual ~SocketBuffer();
        
        virtual void initAutoFlush();
        virtual void initAutoLoad();
        
        virtual void autoFlushThread();
        virtual void autoLoadThread();
        
        virtual void destroyAutoFlush();
        virtual void destroyAutoLoad();

    private:
        string hostname;
        int port;
        int socketfd;
};

#endif // SOCKETBUFFER_H
