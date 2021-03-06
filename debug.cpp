// Debug.cpp
// This file is part of the EScript programming language (https://github.com/EScript)
//
// Copyright (C) 2011-2013 Claudius Jähn <ClaudiusJ@live.de>
// Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
//
// Licensed under the MIT License. See LICENSE file for details.
// ---------------------------------------------------------------------------------
#include "escript.h"

#include "escript.h"

#include <iostream>
#include <map>
#include <string>
#include <utility>

namespace EScript
{
    std::map<void*, Object*> debug_objects = std::map<void*, Object*>();
    int debug_objects_counter = 0;

    void Debug::registerObj(Object* v)
    {
        static int i = 0;
        ++i;
        if(i > 1)
        {
            //std::cout << "+"<<&debug_objects;
            debug_objects[reinterpret_cast<void*>(v)] = v;
            //std::cout << "+";
        }
        ++debug_objects_counter;
    }
    void Debug::unRegisterObj(Object* v)
    {
        debug_objects[reinterpret_cast<void*>(v)] = nullptr;
        --debug_objects_counter;
        //std::cout << "-"<<&debug_objects;
    }
    void Debug::showObjects()
    {
        if(debug_objects_counter == 0)
            return;
        for(const auto& obj : debug_objects)
        {
            if(obj.second == nullptr)
                continue;
            //<< (void*)(obj.second)
            std::cout << " : " << (obj.second) << (obj.second)->toString();
            if((obj.second)->getType())
                std::cout << " ---|> " << (obj.second)->getType() << "\n";
        }
        std::cout << "\nObjects left: " << debug_objects_counter << "\n";
    }
    void Debug::clearObjects()
    {
        debug_objects.clear();
        debug_objects_counter = 0;
    }
}// namespace EScript
