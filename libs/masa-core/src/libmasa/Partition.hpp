/*
 * Partition.hpp
 *
 *  Created on: Mar 29, 2014
 *      Author: edans
 */

#ifndef PARTITION_HPP_
#define PARTITION_HPP_

class Partition {
private:
	/** First Row */
	int i0;
	/** First Column */
	int j0;
	/** Last Row */
	int i1;
	/** Last Column */
	int j1;

public:
	/**
	 * Default constructor.
	 */
	Partition();

	/**
	 * Creates a partition with the given coordinates.
	 */
	Partition(int i0, int j0, int i1, int j1);

	/**
	 * Creates a partition copying the coordinates from other partition
	 * and applying a translation operation.
	 * @param i_offset translate the $i$ coordinates
	 * @param j_offset translate the $j$ coordinates
	 */
	Partition(const Partition& partition, int i_offset, int j_offset);

	/**
	 * Destructor.
	 */
	virtual ~Partition();

	/**
	 * @return the start row of the partition.
	 */
	int getI0() const;
	void setI0(int i0);

	/**
	 * @return the end row (exclusive) of the partition.
	 */
	int getI1() const;
	void setI1(int i1);

	/**
	 * @return the start column of the partition.
	 */
	int getJ0() const;
	void setJ0(int j0);

	/**
	 * @return the end column (exclusive) of the partition.
	 */
	int getJ1() const;
	void setJ1(int j1);

	/**
	 * @return the height of the partition \f$j1-j0\f$.
	 */
	int getHeight() const;

	/**
	 * @return the width of the partition \f$j1-j0\f$.
	 */
	int getWidth() const;

	bool hasZeroArea() const;

};

#endif /* PARTITION_HPP_ */
