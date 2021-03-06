/*
 * AbstractAlignerSafe.hpp
 *
 *  Created on: May 12, 2013
 *      Author: edans
 */

#ifndef ABSTRACTALIGNERSAFE_HPP_
#define ABSTRACTALIGNERSAFE_HPP_

#include "AbstractAligner.hpp"
#include <pthread.h>
#include <queue>
using namespace std;


struct dispatch_job_t {
	enum dispatch_type_e {JOB_SCORE} type;
	union dispatch_params_t {
		struct params_score_t {
			score_t score;
			int bx;
			int by;
		} params_score;
	} dispatch_params;
};

/** @brief A thread-safe AbstractAligner extension.
 *
 * The current MASA implementation is not thread-safe. So, if an Aligner
 * executes simultaneous threads, the dispatch methods may generate an
 * inconsistent state leading to segmentation faults. To avoid this behavior,
 * the Aligner must extend the AbstractAlignerSafe class in order to
 * serialize the dispatch calls.
 *
 * There are two serialization strategies.
 *
 * <ul>
 *  <li>Serialize by Mutex: the class uses mutex to lock simultaneous threads.
 *      The threads are locked until no other thread is dispatching.
 *  <li>Serialize by Queue: the class enqueue all dispatch executions and a
 *      consumer thread dispatches the requisitions to the MASA engine. The
 *      threads are only locked during the enqueue process. By now, only the
 *      dispatchScore function is queued. To enable this feature, use
 *      the createDispatcherQueue and destroyDispatcherQueue in the
 *      alignPartition method.
 * </ul>
 */
class AbstractAlignerSafe : public AbstractAligner {
public:
	AbstractAlignerSafe();
	virtual ~AbstractAlignerSafe();

	virtual void dispatchColumn(int j, const cell_t* buffer, int len);
	virtual void dispatchRow(int i, const cell_t* buffer, int len);
	virtual void dispatchScore(score_t score, int bx=-1, int by=-1);

protected:
	void createDispatcherQueue();
	void destroyDispatcherQueue();

private:
	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_cond_t condition;
	bool dispatcherQueueActive;
	queue<dispatch_job_t> dispatcherQueue;

	static void *staticFunctionThread(void *arg);
	void executeLoop();
};

#endif /* ABSTRACTALIGNERSAFE_HPP_ */
