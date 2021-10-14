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

#include "AbstractAlignerParameters.hpp"

#include <stdio.h>
#include <string.h>

/**
 * Initializes the inner structures of the object.
 */
AbstractAlignerParameters::AbstractAlignerParameters() {
	forkId = NOT_FORKED_INSTANCE;
}

/**
 * Destruct the allocated structures.
 */
AbstractAlignerParameters::~AbstractAlignerParameters() {
}

/*
 * @see definition on header file
 */
const char* AbstractAlignerParameters::getLastError() const {
	return lastError;
}

/*
 * @see definition on header file
 */
const char* AbstractAlignerParameters::getUsageHeader() const {
	return usageHeader;
}

/*
 * @see definition on header file
 */
const char* AbstractAlignerParameters::getUsage() const {
	return usage;
}

/*
 * @see definition on header file
 */
const option* AbstractAlignerParameters::getArguments() const {
	return arguments;
}

/*
 * @see definition on header file
 */
void AbstractAlignerParameters::setArguments(const option* arguments,
		const char* usage, const char* usageHeader) {
	this->arguments = arguments;
	this->usage = usage;
	this->usageHeader = usageHeader;
}

/*
 * @see definition on header file
 */
int AbstractAlignerParameters::getForkId() const {
	return forkId;
}

/*
 * @see definition on header file
 */
void AbstractAlignerParameters::setForkId(int forkId) {
	this->forkId = forkId;
}

/*
 * @see definition on header file
 */
void AbstractAlignerParameters::setLastError(const char* error) {
	strncpy(lastError, error, sizeof(lastError)-1);
	lastError[sizeof(lastError)-1] = '\0';
}


