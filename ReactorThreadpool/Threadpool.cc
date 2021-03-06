 ///
 /// @file    threadPool.cc
 /// @author  yangwenhao
 /// @date    2017-05-11 10:17:36
 ///
 
#include "Threadpool.h"
#include "Thread.h"

#include <unistd.h>
#include <iostream>
#include <memory>
#include <functional>
using std::cout;
using std::endl;
using std::shared_ptr;
using std::bind;

namespace sh
{

Threadpool::Threadpool(size_t buffsize, size_t threadNum)
: _buffsize(buffsize)	
, _threadNum(threadNum)
, _buffer(_buffsize)
, _isExit(false)
{}

Threadpool::~Threadpool()
{
	if(!_isExit)
		stop();
}

void Threadpool::start()
{
	for(size_t idx = 0; idx != _threadNum; ++idx)	//创建_threadNum个线程
	{
		shared_ptr<Thread> sp(new Thread(bind(&Threadpool::threadFunc, this, idx)));
		_threads.push_back(sp);						//将线程存放在一个vector中
		sp->start();
	}
}

void Threadpool::stop()
{
	if(!_isExit)
	{
		while(!_buffer.empty())
		{
			sleep(1);
		}
		_isExit = true;
	    _buffer.wakeup();
		for(auto & elem : _threads)
		{
			elem->join();
		}
	}
}

void Threadpool::addTask(TaskPtr task)
{
	_buffer.push(task);
}

TaskPtr Threadpool::getTask()		
{
	return _buffer.pop();
}

void Threadpool::threadFunc(int idx)
{
	while(!_isExit)
	{
		TaskPtr task = getTask();	//从任务池(buffer类)获取服务器要做的任务
		if(task)
			task->execute();//执行任务中的方法
	}
}

}// end of namespace sh
