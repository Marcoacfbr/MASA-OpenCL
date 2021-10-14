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

#include "BufferFactory.hpp"

#include <stdlib.h>

#include "SocketBuffer.hpp"
#include "FileBuffer.hpp"
#include "NullBuffer.hpp"


Buffer* BufferFactory::createBufferFromURL(string url) {
	int pos1 = url.find_first_of("://");
	if (pos1 == -1) return NULL;
	string type = url.substr(0, pos1);
	string param = url.substr(pos1+3);


	fprintf(stderr, "%s:   %s - %s\n", url.c_str(), type.c_str(), param.c_str());
	if (type == "socket") {
		int port;
		string hostname;
		int pos2 = param.find_first_of(":");
		if (pos2 > 0) {
			port = atoi(param.substr(pos2+1).c_str());
			hostname = param.substr(0, pos2);
		} else {
			hostname = param;
		}
		return new SocketBuffer(hostname, port);
	} else if (type == "file") {
		return new FileBuffer(param);
	} else if (type == "null") {
		int size = atoi(param.c_str());
		return new NullBuffer(size);
	} else {
		return NULL;
	}
}
