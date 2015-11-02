#include "SplayTree.h"
#include <iostream>
#include <algorithm>
#include <functional>
#include <utility>
#include <iterator>
#include <stdexcept>
#include <chrono>
#include <map>
#include <vector>
#include <sstream>
#include "AssocVector.h"

namespace chrono = std::chrono;
const int N = 50000;

template <typename T>
T lexical_cast(size_t number)
{
    T var;
    std::stringstream ioss;
    ioss << number;
    ioss >> var;
    return var;
}

struct MyObj
{
public:
    int ID;
};

MyObj* CreateTestObj()
{
    static int counter = 4;
    MyObj* obj = new MyObj();
    obj->ID = counter++;
    return obj;
};

struct classcomp {
    bool operator() (const std::string& lhs, const std::string& rhs) const
    {
        return lhs<rhs;
    }
};

/*for insert in std::map*/
template<typename K, typename V>
auto countInsertTime(const std::vector<V> & src, std::map<K, V> & container) -> chrono::microseconds {
    auto start = chrono::system_clock::now();
    for (size_t idx = 0, size = src.size(); idx < size; ++idx) {
        container.emplace(lexical_cast<K>(idx), src[idx]);
    }
    return chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - start);
}

/*for find in std::map*/
template<typename K, typename V>
auto countFindTime(std::map<K, V> & container, K key) -> chrono::microseconds {
    auto start = chrono::system_clock::now();
    std::cout << container.find(key)->first << " ";
    
    return chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - start);
    }
    
    /*for insert in my SplayTreeMap*/
    template<typename K, typename V>
    auto countInsertTime(const std::vector<V> & src, SplayTree<K, V> & container) -> chrono::microseconds {
        auto start = chrono::system_clock::now();
        for (size_t idx = 0, size = src.size(); idx < size; ++idx) {
            container.emplace(lexical_cast<K>(idx), src[idx]);
        }
        return chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - start);
    }
    
    /*for find in SplayTreeMap*/
    template<typename K, typename V>
    auto countFindTime(SplayTree<K, V> & container, K key)->chrono::microseconds {
        auto start = chrono::system_clock::now();
        std::cout << container.find(key)->first << " ";
        return chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - start);
    }
    
    /*for insert in my LokiMap*/
    template<typename K, typename V>
    auto countInsertTime(const std::vector<V> & src, Loki::AssocVector<K, V> & container) -> chrono::microseconds {
        auto start = chrono::system_clock::now();
        for (size_t idx = 0, size = src.size(); idx < size; ++idx) {
            container[lexical_cast<K>(idx)] = src[idx];
        }
        return chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - start);
    }
    
    /*for find in LokiMap*/
    template<typename K, typename V>
    auto countFindTime(Loki::AssocVector<K, V> & container, K key)->chrono::microseconds {
        auto start = chrono::system_clock::now();
        std::cout << container.find(key) -> first << " ";
        return chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - start);
    }
    
    
    int main()
    {
        std::map<int, int> STL_map_int;
        SplayTree<int, int> MySplayTree_int;
        Loki::AssocVector<int, int> Loki_int;
        
        std::map<int, MyObj> STL_map_myObj;
        SplayTree<int, MyObj> MySplayTree_myObj;
        Loki::AssocVector<int, MyObj> Loki_myObj;
        
        std::map<std::string, MyObj> STL_map_string;
        SplayTree<std::string, MyObj> MySplayTree_string;
        Loki::AssocVector<std::string, MyObj> Loki_string;
        
        std::vector<int> src_int;
        for (int i = 0; i < N; i++)
            src_int.push_back(rand() % N);
        
        std::vector<MyObj> src_myObj;
        for (int i = 0; i < N; i++)
        {
            MyObj *temp = new MyObj(*CreateTestObj());
            src_myObj.push_back(*temp);
            delete temp;
        }
        
        long long mapTime = 0, splayTreeMapTime = 0, lokiMapTime = 0, tempTime = 0;
        
        mapTime = countInsertTime(src_int, STL_map_int).count();
        std::cout << "map<int, int> insert  = " << mapTime << " ms" << std::endl;
        
        lokiMapTime = countInsertTime(src_int, Loki_int).count();
        std::cout << "lokiMap<int, int> insert  = " << lokiMapTime << " ms" << std::endl;
        Loki_int.clear();
        
        splayTreeMapTime = countInsertTime(src_int, MySplayTree_int).count();
        std::cout << "mySplayTree<int, int> insert = " << splayTreeMapTime << " ms" << std::endl << std::endl;
        
        tempTime = countInsertTime(src_myObj, STL_map_myObj).count();
        mapTime += tempTime;
        std::cout << "map<int, myObj> insert  = " << tempTime << " ms" << std::endl;
        
        tempTime = countInsertTime(src_myObj, Loki_myObj).count();
        lokiMapTime += tempTime;
        std::cout << "lokiMap<int, myObj> insert  = " << tempTime << " ms" << std::endl;
        
        tempTime = countInsertTime(src_myObj, MySplayTree_myObj).count();
        splayTreeMapTime += tempTime;
        std::cout << "mySplayTree<int, myObj> insert = " << tempTime << " ms" << std::endl <<std::endl;
        
        tempTime = countInsertTime(src_myObj, STL_map_string).count();
        mapTime += tempTime;
        std::cout << "map<string, myObj> insert  = " << tempTime << " ms" << std::endl;
        
        tempTime = countInsertTime(src_int, Loki_int).count();
        lokiMapTime += tempTime;
        std::cout << "lokiMap<string, myObj> insert  = " << tempTime << " ms" << std::endl;
        
        tempTime = countInsertTime(src_myObj, MySplayTree_string).count();
        splayTreeMapTime += tempTime;
        std::cout << "mySplayTree<string, myObj> insert = " << tempTime << " ms" << std::endl <<std::endl;
        
        std::cout << "averege INSERT in std::map = " << mapTime / 3 << " ms" << std::endl;
        std::cout << "averege INSERT in lokiMap = " << lokiMapTime / 3 << " ms" << std::endl;
        std::cout << "averege INSERT in mySplayTreeMap = " << splayTreeMapTime / 3 << " ms" << std::endl << std::endl;
        
        mapTime = 0; lokiMapTime = 0; splayTreeMapTime = 0;
        
        for (size_t i = 0; i < 10; i++)
        {
            auto number = rand() % 5000;
            tempTime = countFindTime(STL_map_myObj, number).count();
            mapTime += tempTime;
            std::cout << "map<int, myObj> FIND element = " << tempTime << " ms" << std::endl;
            
            tempTime = countFindTime(Loki_myObj, number).count();
            lokiMapTime += tempTime;
            std::cout << "lokiMap<int, myObj> FIND element = " << tempTime << " ms" << std::endl;
            
            tempTime = countFindTime(MySplayTree_myObj, number).count();
            splayTreeMapTime += tempTime;
            std::cout << "mySplayTree<int, myObj> FIND element = " << tempTime << " ms" << std::endl << std::endl;
        }
        
        
        
        std::cout << "averege FIND in std::map = " << mapTime / 5 << " ms" << std::endl;
        std::cout << "averege FIND in lokiMap = " << lokiMapTime / 5 << " ms" << std::endl;
        std::cout << "averege FIND in mySplayTreeMap = " << splayTreeMapTime / 5 << " ms" << std::endl << std::endl;
        
        return EXIT_SUCCESS;
        
    }