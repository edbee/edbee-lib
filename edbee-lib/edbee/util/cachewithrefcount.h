#pragma once

#include <QCache>
#include <QDebug>
#include <QHash>

/// A smart cache with uses a QCache and a QMap for reference counting
/// You can mark elements as in use, this way those elements are guaranteed and not automaticly deleted
template <class Key, class T>
class CacheWithRefCount
{
    struct ObjectRef{
        T* object;
        int refCount;
        ObjectRef(T* object=0, int refCount=0)
            : object(object)
            , refCount(refCount)
        {}
    };

public:
    CacheWithRefCount(int maxcount=100)
        : cache_(maxcount)
    {
    }


    /// clears the cache.
    /// Warning there shouldn't be any referenced objects when calling clear!
    void clear()
    {
        Q_ASSERT(referencedObjects_.isEmpty());
        // when it's not empty we're leaking the referenced errors
        referencedObjects_.clear();
        cache_.clear();
    }

    /// reserve an object
    T* aquire( const Key& key )
    {
        if( referencedObjects_.find(key) != referencedObjects_.end() ) {
           referencedObjects_[key].refCount += 1;
           return referencedObjects_[key].object;
        }

        // object doesn't exist
        T* object = cache_.object(key);
        if( !object ) return 0;

        // add the object to the pool
        object = cache_.take(key);
        referencedObjects_[key] = ObjectRef(object,1);
        return object;
    }

    void release( const Key& key )
    {
        if( referencedObjects_.find(key) != referencedObjects_.end() ) {
            referencedObjects_[key].refCount -= 1;
            if( referencedObjects_[key].refCount <= 0 ) {
                cache_.insert(key, referencedObjects_[key].object );
                referencedObjects_.remove(key);
            }
        }
    }

    QList<Key> keys() const
    {
        QList<Key> result(cache_.keys());
        result.append(referencedObjects_.keys());
        return result;
    }


    /// references a cached/referenced object without aquiring it
    T* object( const Key& key ) const
    {
        typename QHash<Key,ObjectRef>::const_iterator itr = referencedObjects_.find(key);
        if( itr != referencedObjects_.end() ) return itr.value().object;
        return cache_.object(key);
    }

    bool insert( const Key& key, T* object )
    {
        Q_ASSERT( referencedObjects_.find(key) == referencedObjects_.end() );
        return cache_.insert(key,object);
    }

    inline bool remove(const Key& key)
    {
        Q_ASSERT( referencedObjects_.find(key) == referencedObjects_.end() && "Removing a referenced object isn't good" );
        return cache_.remove(key);
    }

    /// returns the total number of elements in the cache
    int size() const
    {
        return cache_.size() + referencedObjects_.size();
    }


protected:
    QCache<Key,T> cache_;
    QHash<Key,ObjectRef> referencedObjects_;
};

