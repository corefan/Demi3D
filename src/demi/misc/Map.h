/**********************************************************************
This source file is a part of Demi3D
   __  ___  __  __  __
  |  \|_ |\/||   _)|  \ 
  |__/|__|  ||  __)|__/ 

Copyright (c) 2013-2014 Demi team
https://github.com/wangyanxing/Demi3D

Released under the MIT License
https://github.com/wangyanxing/Demi3D/blob/master/License.txt
***********************************************************************/

#ifndef DiMap_h__
#define DiMap_h__

#include <map>

template <class KEY, class T, class comp = std::less<KEY>>
class DiMap : public std::map<KEY, T, comp, std::allocator< std::pair<const KEY, T> > >
{
public:

    bool contains(const KEY& key)
    {
        return this->find(key) != this->end();
    }

    // Find the key in the map, if no result return the default value
    inline T find_def(const KEY& key, T defaultVal)
    {
        auto it = this->find (key);
        if (it == this->end())
            return defaultVal;
        else
            return it->second;
    }

    inline bool removeKey(const KEY& key)
    {
        auto it = this->find(key);
        if (it == this->end())
            return false;
        else
        {
            this->erase(it);
            return true;
        }
    }
};


struct string_nocase_lt
{
    bool operator()( const char *s1,const char *s2 ) const
    {
        return stricmp(s1,s2) < 0;
    }
    
    bool operator()( const Demi::DiString& s1,const Demi::DiString& s2 ) const
    {
        return stricmp(s1.c_str(), s2.c_str()) < 0;
    }
};



#endif
