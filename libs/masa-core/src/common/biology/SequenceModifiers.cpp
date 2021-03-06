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

#include "SequenceModifiers.hpp"

int SequenceModifiers::getTrimEnd() const {
	return trimEnd;
}

void SequenceModifiers::setTrimEnd(int trimEnd) {
	this->trimEnd = trimEnd;
}

int SequenceModifiers::getTrimStart() const {
	return trimStart;
}

void SequenceModifiers::setTrimStart(int trimStart) {
	this->trimStart = trimStart;
}

bool SequenceModifiers::isClearN() const {
	return clear_n;
}

void SequenceModifiers::setClearN(bool clearN) {
	clear_n = clearN;
}

bool SequenceModifiers::isComplement() const {
	return complement;
}

void SequenceModifiers::setComplement(bool complement) {
	this->complement = complement;
}

bool SequenceModifiers::isReverse() const {
	return reverse;
}

bool SequenceModifiers::isCompatible(const SequenceModifiers* other) const {
	bool clearEqual = (this->clear_n == other->clear_n);
	bool complementEqual = (this->complement == other->complement);

	return clearEqual && complementEqual;
}

void SequenceModifiers::setReverse(bool reverse) {
	this->reverse = reverse;
}

