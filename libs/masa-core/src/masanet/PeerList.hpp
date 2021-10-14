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
 * PeerList.hpp
 *
 *  Created on: Oct 18, 2013
 *      Author: edans
 */

class PeerList;

#ifndef PEERLIST_HPP_
#define PEERLIST_HPP_

#include <map>
#include <vector>
using namespace std;

#include "Peer.hpp"

class PeerList {
public:
	PeerList();
	virtual ~PeerList();

	void add(Peer* peer);
	void copy(vector<Peer*> peers);
	void erase(const string& id);
	Peer* get(const string& id) const;
	Peer* getPrev(const string& id) const;
	Peer* getNext(const string& id) const;
	vector<Peer*> getAllPeers() const;
	vector<Peer*> getProcessingPeers() const;

private:
	map<string, Peer*> peerMap;
};

#endif /* PEERLIST_HPP_ */
