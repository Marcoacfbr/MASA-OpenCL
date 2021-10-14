/*
 * NullBuffer.hpp
 *
 *  Created on: Apr 17, 2013
 *      Author: edans
 */

#ifndef NULLBUFFER_HPP_
#define NULLBUFFER_HPP_

#include "Buffer.hpp"

class NullBuffer : public Buffer {
public:
	NullBuffer(int size = 0);
	virtual ~NullBuffer();

    virtual void initAutoFlush();
    virtual void initAutoLoad();

    virtual void autoFlushThread();
    virtual void autoLoadThread();

    virtual void destroyAutoFlush();
    virtual void destroyAutoLoad();

private:
    int size;
};

#endif /* NULLBUFFER_HPP_ */
