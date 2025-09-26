#ifndef GENERIC_QUEUE_H
#define GENERIC_QUEUE_H

#pragma once 

#include <assert.h>
#include <stdio.h>
#pragma warning( disable : 4786 )

template <class Type> class Queue {
	public:
		Queue(int s=-1);
		~Queue();
	void Enqueue(Type);
	Type Dequeue();

	void allocate(int s);
	void LockSize(int s){lock_size=s;};
	Type *element;
	int num;
	int array_size;
	int lock_size;
	Type &operator[](int i){assert(i>=0 && i<num);return element[i];}
};

template <class Type> Queue<Type>::Queue(int s){
	num=0;
	array_size=0;
	element=NULL;
	lock_size=s;	
}
template <class Type> Queue<Type>::~Queue(){
	num=0;
	if(element) {
		delete[] element;
		element = NULL;
	}
}

template <class Type> void Queue<Type>::allocate(int s){
	assert(s>0);
	assert(s>=num);
	Type *old=element;
	array_size=s;
	element=new Type[array_size];
	assert(element);
	for(int i=0;i<num;i++)
	{
		element[i]=old[i];
	}
	if(old) {
		delete[] old;
		old = NULL;
	}
}
template <class Type> void Queue<Type>::Enqueue(Type t)
{
	if(lock_size==-1)
	{
		assert(num<=array_size);
		if(num==array_size)
		{
			allocate((array_size)?array_size*2:16);
		}
		element[num++]=t;
	}
	else
	{
		assert(lock_size!=0);
		if(num>=lock_size)
		{
			Dequeue();
			element[num++]=t;
		}
		else
		{
			element[num++]=t;
		}
	}
}
template <class Type> Type Queue<Type>::Dequeue()
{
	assert(num>0);
	Type result=element[0];
	for(int i=1;i<num;i++)
	{
		element[i-1]=element[i];
	}
	num--;
	return result;
}

#endif