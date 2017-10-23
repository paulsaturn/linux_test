/**
 * 
 **/
#ifndef __STRONG_POINTER_H__
#define __STRONG_POINTER_H__

#include "atomic.h"

#define COMPARE(_op_)                                           \
inline bool operator _op_ (const sp<T>& o) const {              \
    return m_ptr _op_ o.m_ptr;                                  \
}                                                               \
inline bool operator _op_ (const T* o) const {                  \
    return m_ptr _op_ o;                                        \
}                                                               \
template<typename U>                                            \
inline bool operator _op_ (const sp<U>& o) const {              \
    return m_ptr _op_ o.m_ptr;                                  \
}                                                               \
template<typename U>                                            \
inline bool operator _op_ (const U* o) const {                  \
    return m_ptr _op_ o;                                        \
}

template <class T>  
class LightRefBase  
{  
public:  
    inline LightRefBase() : mCount(0) { }  
    inline void incStrong(const void* id) const {  
        android_atomic_inc(&mCount);
    }  
    inline void decStrong(const void* id) const {  
        if (android_atomic_dec(&mCount) == 1) {
            delete static_cast<const T*>(this);  
        }  
    }  
    //! DEBUGGING ONLY: Get current strong ref count.  
    inline int32_t getStrongCount() const {  
        return mCount;  
    }  
  
protected:  
    inline ~LightRefBase() { }  
  
private:  
    mutable volatile int32_t mCount;  
};

template <typename T>
class sp
{
public:
	inline sp() : m_ptr(0) { }

	sp(T* other);
	sp(const sp<T>& other);
	template<typename U> sp(U* other);
	template<typename U> sp(const sp<U>& other);

	~sp();

	// Assignment

	sp& operator = (T* other);
	sp& operator = (const sp<T>& other);

	template<typename U> sp& operator = (const sp<U>& other);
	template<typename U> sp& operator = (U* other);

	//! Special optimization for use by ProcessState (and nobody else).
	void force_set(T* other);

	// Reset

	void clear();

	// Accessors

	inline  T&      operator* () const  { return *m_ptr; }
	inline  T*      operator-> () const { return m_ptr;  }
	inline  T*      get() const         { return m_ptr; }

	// Operators
	COMPARE(==)
		COMPARE(!=)
		COMPARE(>)
		COMPARE(<)
		COMPARE(<=)
		COMPARE(>=)
        
private:
	T*              m_ptr;
};

#endif //__STRONG_POINTER_H__
