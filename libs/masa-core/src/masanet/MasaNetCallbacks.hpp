/*
 * MasaNetCallbacks.hpp
 *
 *  Created on: Oct 29, 2013
 *      Author: edans
 */

#ifndef MASANETCALLBACKS_HPP_
#define MASANETCALLBACKS_HPP_

class MasaNetCallbacks {
public:
	virtual bool onConnect(Peer* peer) = 0;
	virtual bool onConnectData(Peer* peer) = 0;
	virtual void onDisconnect(Peer* peer) = 0;

};


#endif /* MASANETCALLBACKS_HPP_ */
