// ----------------------------------------------------------------
// From "Algorithms and Game Programming" in C++ by Alessandro Bria
// Copyright (C) 2024 Alessandro Bria (a.bria@unicas.it). 
// All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once

namespace agp
{
    template <typename T>
	class Singleton;
}

// Polymorphic singleton template
template <typename T>
class agp::Singleton 
{
    private:
        
        static T* _instance;

    protected:

        // protected constructor and destructor to prevent direct instantiation and deletion
        Singleton() = default;
        virtual ~Singleton()
        {
            // A scene owner may delete a singleton (e.g. HUD) directly.
            if (_instance && static_cast<Singleton<T>*>(_instance) == this)
                _instance = nullptr;
        }

    public:

        // provides global point of access to the singleton instance
        static T* instance()
        {
            if (!_instance)
                _instance = new T();
            return _instance;
        }

        // needed for polymorphic behavior
        static void setInstance(T* newInstance) 
        {
            if (_instance == newInstance)
                return;
            destroyInstance();
            _instance = newInstance;
        }

        static void destroyInstance()
        {
            if (_instance)
                delete _instance;
            _instance = nullptr;
        }

        // delete copy constructor and assignment operator to prevent copies
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;
};

template <typename T>
T* agp::Singleton<T>::_instance = nullptr;