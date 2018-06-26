//////////////////////////////////////////////////////////////////
//
// singleton.h
//
// Copyright (c) 2001-2011, Jan Willamowius
//
// All singleton objects are put into a list
// so that it would be delete when program exits.
//
// This work is published under the GNU Public License version 2 (GPLv2)
// see file COPYING for details.
// We also explicitly grant the right to link this code
// with the OpenH323/H323Plus and OpenSSL library.
//
//////////////////////////////////////////////////////////////////

#ifndef SINGLETON_H
#define SINGLETON_H "@(#) $Id: singleton.h,v 1.22 2011/08/19 18:08:06 willamowius Exp $"

// STL
#include <list>
#include <algorithm>

#ifdef WIN32
#include <QMutex>
#else
#include <pthread.h>
#endif
//
// a list of pointers that would delete all objects
// referred by the pointers in the list on destruction
//
template<class T> class listptr : public std::list<void *> {
  public:
	listptr() : clear_list(false) {}
	~listptr();
	bool clear_list;

  private:
	static void delete_obj(void *t) { delete static_cast<T *>(t); }
};

template<class T> listptr<T>::~listptr()
{
	clear_list=true;
	std::for_each(begin(), end(), delete_obj);
}


// Base class for all singletons
class SingletonBase {
  public:
	SingletonBase(const char *);
	virtual ~SingletonBase();

  private:
	const char *m_name;
	// Note the SingletonBase instance is not singleton itself :p
	// However, list of singletons *are* singleton
	// But we can't put the singleton into the list :(
	static listptr<SingletonBase> _instance_list;
};

//
// A singleton class should be derived from this template.
// class Ts : public Singleton<Ts> {
//     ...
// };
//
// If the class is instantiated more than once,
// a runtime error would be thrown
//
// To access the singleton use T::Instance()
//
template<class T> class Singleton : public SingletonBase {
  public:
	static T *Instance();
	static bool InstanceExists();

  protected:
	Singleton(const char *);
	virtual ~Singleton();

  public:
	static T *m_Instance;
#ifdef WIN32
    static QMutex m_CreationLock;
#else
	static pthread_mutex_t m_CreationLock;
#endif
};

template<class T> Singleton<T>::Singleton(const char *n) : SingletonBase(n)
{
#ifndef WIN32
    pthread_mutex_init(&m_CreationLock, NULL);
#endif
}

template<class T> Singleton<T>::~Singleton()
{
    //PWaitAndSignal lock(m_CreationLock);
#ifdef WIN32
    m_CreationLock.lock();
#else
    pthread_mutex_lock(&m_CreationLock);
#endif
    m_Instance = 0;
#ifdef WIN32
    m_CreationLock.unlock();
#else
    pthread_mutex_unlock(&m_CreationLock);
#endif
}

// Function to access the singleton
template<class T> T *Singleton<T>::Instance()
{
	if (m_Instance == 0) {
	    //PWaitAndSignal lock(m_CreationLock);
#ifdef WIN32
        m_CreationLock.lock();
#else
        pthread_mutex_lock(&m_CreationLock);
#endif
	    // We have to check it again after we got the lock
	    if (m_Instance == 0)
		m_Instance = new T;
#ifdef WIN32
        m_CreationLock.unlock();
#else
        pthread_mutex_unlock(&m_CreationLock);
#endif
	}
	return m_Instance;
}

// Function to check for existance of singleton
template<class T> bool Singleton<T>::InstanceExists()
{
	return (m_Instance != 0);
}

// static members
template<class T> T *Singleton<T>::m_Instance = 0;
#ifdef WIN32
template<class T> QMutex Singleton<T>::m_CreationLock;
#else
template<class T> pthread_mutex_t Singleton<T>::m_CreationLock;
#endif


#endif // SINGLETON_H
