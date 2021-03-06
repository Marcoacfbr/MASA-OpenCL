/*
 * Partition.cpp
 *
 *  Created on: Mar 29, 2014
 *      Author: edans
 */

#include "Partition.hpp"

/*
 * @see definition on header file
 */
Partition::Partition() {
	this->i0 = 0;
	this->j0 = 0;
	this->i1 = 0;
	this->j1 = 0;
}

/*
 * @see definition on header file
 */
Partition::Partition(int i0, int j0, int i1, int j1) {
	this->i0 = i0;
	this->j0 = j0;
	this->i1 = i1;
	this->j1 = j1;
}

/*
 * @see definition on header file
 */
Partition::Partition(const Partition& partition, int i_offset, int j_offset) {
	this->i0 = partition.i0 + i_offset;
	this->j0 = partition.j0 + j_offset;
	this->i1 = partition.i1 + i_offset;
	this->j1 = partition.j1 + j_offset;

}

/*
 * @see definition on header file
 */
Partition::~Partition() {
	// TODO Auto-generated destructor stub
}

/*
 * @see definition on header file
 */
int Partition::getI0() const {
	return i0;
}

/*
 * @see definition on header file
 */
void Partition::setI0(int i0) {
	this->i0 = i0;
}

/*
 * @see definition on header file
 */
int Partition::getI1() const {
	return i1;
}

/*
 * @see definition on header file
 */
void Partition::setI1(int i1) {
	this->i1 = i1;
}

/*
 * @see definition on header file
 */
int Partition::getJ0() const {
	return j0;
}

/*
 * @see definition on header file
 */
void Partition::setJ0(int j0) {
	this->j0 = j0;
}

/*
 * @see definition on header file
 */
int Partition::getJ1() const {
	return j1;
}

/*
 * @see definition on header file
 */
void Partition::setJ1(int j1) {
	this->j1 = j1;
}

/*
 * @see definition on header file
 */
int Partition::getHeight() const {
	return i1-i0;
}

/*
 * @see definition on header file
 */
int Partition::getWidth() const {
	return j1-j0;
}

bool Partition::hasZeroArea() const {
	return getWidth() == 0 || getHeight() == 0;
}


