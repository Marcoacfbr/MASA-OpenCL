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
 * CmdStatusRequest.cpp
 *
 *  Created on: Oct 16, 2013
 *      Author: edans
 */

#include "CmdPeerRequest.hpp"

CmdPeerRequest::CmdPeerRequest() {

}

CmdPeerRequest::~CmdPeerRequest() {
}

Command* CmdPeerRequest::creator() {
	return new CmdPeerRequest();
}

int CmdPeerRequest::getId() {
	return COMMAND_PEER_REQUEST;
}

void CmdPeerRequest::send(Peer* socket) {
	socket->send_int8(type);
}

void CmdPeerRequest::receive(Peer* socket) {
	type = socket->recv_int8();
}

int CmdPeerRequest::getType() const {
	return type;
}

void CmdPeerRequest::setType(int type) {
	this->type = type;
}
