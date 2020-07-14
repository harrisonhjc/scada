#pragma once
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>


template <typename T>
class ConcurrentQueue
{
private:
    QQueue<T> queue;
    QMutex mutex;
    QWaitCondition emptyCondition;
    QWaitCondition fullCondition;
    qint32 capacity;
public:

    ConcurrentQueue(int capacity = -1): capacity(capacity)
    {
    }

    ~ConcurrentQueue(void)
    {
    }

    T dequeue ()
    {
        QMutexLocker locker(&mutex);
        //while(queue.empty()){
        //        emptyCondition.wait(&mutex);
        //}
        if(queue.empty()){
			fullCondition.wakeAll();
			return NULL;
        }
		else{
			T result = queue.dequeue();
	        fullCondition.wakeAll();
    	    return result;
		}

		
    }
    void enqueue(const T &t)
    {
        QMutexLocker locker(&mutex);
        if(capacity > 0 ){
                while(capacity <= queue.size()){
                        fullCondition.wait(&mutex);
                }
        }
        queue.enqueue(t);
        emptyCondition.wakeAll();
    }
    T& pick ()
    {
        QMutexLocker locker(&mutex);
        if(queue.empty()){
                return NULL;
        }
        return queue.first();
    }
    const T& pick () const
    {
        QMutexLocker locker(&mutex);
        if(queue.empty()){
                return NULL;
        }
        return queue.first();
    }

    int size() const
    {
        QMutexLocker locker(&mutex);
        return queue.size();
    }

    void clear()
    {
        QMutexLocker locker(&mutex);
        queue.clear();
    }
};
