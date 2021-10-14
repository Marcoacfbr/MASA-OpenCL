/*
 * SocketCellsReader.hpp
 *
 *  Created on: Apr 23, 2014
 *      Author: edans
 */

#ifndef SOCKETCELLSREADER_HPP_
#define SOCKETCELLSREADER_HPP_

#include "CellsReader.hpp"
#include <string>
using namespace std;

class SocketCellsReader : public CellsReader {
public:
	SocketCellsReader(string hostname, int port);
	virtual ~SocketCellsReader();
	virtual void close();

	virtual int getType();
	virtual int read(cell_t* buf, int len);

private:
    string hostname;
    int port;
    int socketfd;

    void init();
    int resolveDNS(const char* hostname, char* ip);
};

#endif /* SOCKETCELLSREADER_HPP_ */
