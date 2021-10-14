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
 * CmdNotifyScore.hpp
 *
 *  Created on: Sep 24, 2013
 *      Author: edans
 */

#ifndef CMDNOTIFYSCORE_HPP_
#define CMDNOTIFYSCORE_HPP_

#include "Command.hpp"

class CmdNotifyScore : public Command {
public:
	CmdNotifyScore();
	virtual ~CmdNotifyScore();

	static Command* creator();

	virtual int getId();

	virtual void send(Peer* socket);
	virtual void receive(Peer* socket);

private:
	int i;
	int j;
	int score;

};

#endif /* CMDNOTIFYSCORE_HPP_ */
