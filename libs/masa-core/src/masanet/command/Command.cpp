/*******************************************************************************
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
 ******************************************************************************/

/*
 * Command.cpp
 *
 *  Created on: Sep 22, 2013
 *      Author: edans
 */

#include "Command.hpp"

Command::Command()
{
	// TODO Auto-generated constructor stub

}

Command::~Command()
{
	// TODO Auto-generated destructor stub
}

int Command::getSerial() const {
	return serial;
}

void Command::setSerial(int serial) {
	this->serial = serial;
}
