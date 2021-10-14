/*
 * SocketCellsWriter.hpp
 *
 *  Created on: Apr 23, 2014
 *      Author: edans
 */

#ifndef SOCKETCELLSWRITER_HPP_
#define SOCKETCELLSWRITER_HPP_

#include "CellsWriter.hpp"
#include <string>
using namespace std;

class SocketCellsWriter: public CellsWriter {
public:
	SocketCellsWriter(string hostname, int port);
	virtual ~SocketCellsWriter();
	virtual void close();

	virtual int write(const cell_t* buf, int len);
private:
    string hostname;
    int port;
    int socketfd;

    void init();
};

#endif /* SOCKETCELLSWRITER_HPP_ */
