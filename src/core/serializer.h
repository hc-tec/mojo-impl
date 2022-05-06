//
// Created by titto on 2022/5/6.
//

#ifndef MOJO_IMPL_SERIALIZER_H
#define MOJO_IMPL_SERIALIZER_H

#include <algorithm>
#include <cstdint>
#include <map>
#include <set>
#include <sstream>
#include <string.h>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "core/byte_array.h"
#include "core/protocol.h"

namespace tit {

namespace mojo {
/**
 * Serializer / Deserializer
 * following these rules:
 * 1.Won't zip 8/16 bit type when serializing,
 *   but 32/16 bit type signed/unsigned number would zipped by zigzag and varints code
 *
 * Supported STL containers：
 * string, list, vector
 * set, multiset, map, multimap
 * unordered_set, unordered_multiset, unordered_map, unordered_multimap
 * tuple
 */
class Serializer {
public:
    using Ptr = std::shared_ptr<Serializer>;

    Serializer() {
        m_byteArray = std::make_shared<ByteArray>();
    }

    Serializer(ByteArray::ptr byteArray) {
        m_byteArray = byteArray;
    }

    Serializer(const std::string& in) {
        m_byteArray = std::make_shared<ByteArray>();
        writeRowData(&in[0], in.size());
        reset();
    }

    Serializer(const char* in, int len) {
        m_byteArray = std::make_shared<ByteArray>();
        writeRowData(in, len);
        reset();
    }

public:
    int size(){
        return m_byteArray->getSize();
    }

    void reset() {
        m_byteArray->setPosition(0);
    }

    void offset(int off){
        int old = m_byteArray->getPosition();
        m_byteArray->setPosition(old + off);
    }

    std::string toString(){
        return m_byteArray->toString();
    }

    ByteArray::ptr getByteArray() {
        return m_byteArray;
    }

    void writeRowData(const char* in, int len){
        m_byteArray->write(in, len);
    }
    // unzip
    template<class T>
    void writeFint(T value){
        m_byteArray->writeFint(value);
    }

    void clear(){
        m_byteArray->clear();
    }

    template<typename T>
    void read(T& t) {
        if constexpr(std::is_same_v<T, bool>){
            t = m_byteArray->readFint8();
        } else if constexpr(std::is_same_v<T, float>){
            t = m_byteArray->readFloat();
        } else if constexpr(std::is_same_v<T, double>){
            t = m_byteArray->readDouble();
        } else if constexpr(std::is_same_v<T, int8_t>){
            t = m_byteArray->readFint8();
        } else if constexpr(std::is_same_v<T, uint8_t>){
            t = m_byteArray->readFuint8();
        } else if constexpr(std::is_same_v<T, int16_t>){
            t = m_byteArray->readFint16();
        } else if constexpr(std::is_same_v<T, uint16_t>){
            t = m_byteArray->readFuint16();
        } else if constexpr(std::is_same_v<T, int32_t>){
            t = m_byteArray->readInt32();
        } else if constexpr(std::is_same_v<T, uint32_t>){
            t = m_byteArray->readUint32();
        } else if constexpr(std::is_same_v<T, int64_t>){
            t = m_byteArray->readInt64();
        } else if constexpr(std::is_same_v<T, uint64_t>){
            t = m_byteArray->readUint64();
        } else if constexpr(std::is_same_v<T, std::string>){
            t = m_byteArray->readStringVint();
        }
    }

    template<typename T>
    void write(T t) {
        if constexpr(std::is_same_v<T, bool>){
            m_byteArray->writeFint8(t);
        } else if constexpr(std::is_same_v<T, float>){
            m_byteArray->writeFloat(t);
        } else if constexpr(std::is_same_v<T, double>){
            m_byteArray->writeDouble(t);
        } else if constexpr(std::is_same_v<T, int8_t>){
            m_byteArray->writeFint8(t);
        } else if constexpr(std::is_same_v<T, uint8_t>){
            m_byteArray->writeFuint8(t);
        } else if constexpr(std::is_same_v<T, int16_t>){
            m_byteArray->writeFint16(t);
        } else if constexpr(std::is_same_v<T, uint16_t>){
            m_byteArray->writeFuint16(t);
        } else if constexpr(std::is_same_v<T, int32_t>){
            m_byteArray->writeInt32(t);
        } else if constexpr(std::is_same_v<T, uint32_t>){
            m_byteArray->writeUint32(t);
        } else if constexpr(std::is_same_v<T, int64_t>){
            m_byteArray->writeInt64(t);
        } else if constexpr(std::is_same_v<T, uint64_t>){
            m_byteArray->writeUint64(t);
        } else if constexpr(std::is_same_v<T, std::string>){
            m_byteArray->writeStringVint(t);
        } else if constexpr(std::is_same_v<T, char*>){
            m_byteArray->writeStringVint(std::string(t));
        } else if constexpr(std::is_same_v<T, const char*>){
            m_byteArray->writeStringVint(std::string(t));
        }
    }

public:
    template<typename T>
    [[maybe_unused]]
    Serializer &operator >> (T& i){
        read(i);
        return *this;
    }

    template<typename T>
    [[maybe_unused]]
    Serializer &operator << (const T& i){
        write(i);
        return *this;
    }

    template<typename... Args>
    Serializer &operator >> (std::tuple<Args...>& t){

        const auto& deserializer = [this]<typename Tuple, std::size_t... Index>
        (Tuple& t, std::index_sequence<Index...>) {
            (void)((*this) >> ... >> std::get<Index>(t));
        };
        deserializer(t, std::index_sequence_for<Args...>{});
        return *this;
    }

    template<typename... Args>
    Serializer &operator << (const std::tuple<Args...>& t){
        /**
         * @brief 实际的序列化函数，利用折叠表达式展开参数包
         */
        const auto& package = [this]<typename Tuple, std::size_t... Index>
        (const Tuple& t, std::index_sequence<Index...>) {
            (void)((*this) << ... << std::get<Index>(t));
        };
        package(t, std::index_sequence_for<Args...>{});
        return *this;
    }

    template<typename T>
    Serializer &operator >> (std::list<T>& v){
        size_t size;
        read(size);
        for (size_t i = 0; i < size; ++i) {
            T t;
            read(t);
            v.template emplace_back(t);
        }
        return *this;
    }

    template<typename T>
    Serializer &operator << (const std::list<T>& v){
        write(v.size());
        for(auto& t : v) {
            (*this) << t;
        }
        return *this;
    }

    template<typename T>
    Serializer &operator >> (std::vector<T>& v){
        size_t size;
        read(size);
        for (size_t i = 0; i < size; ++i) {
            T t;
            read(t);
            v.template emplace_back(t);
        }
        return *this;
    }

    template<typename T>
    Serializer &operator << (const std::vector<T>& v){
        write(v.size());
        for(auto& t : v) {
            (*this) << t;
        }
        return *this;
    }

    template<typename T>
    Serializer &operator >> (std::set<T>& v){
        size_t size;
        read(size);
        for (size_t i = 0; i < size; ++i) {
            T t;
            read(t);
            v.template emplace(t);
        }
        return *this;
    }

    template<typename T>
    Serializer &operator << (const std::set<T>& v){
        write(v.size());
        for(auto& t : v) {
            (*this) << t;
        }
        return *this;
    }

    template<typename T>
    Serializer &operator >> (std::multiset<T>& v){
        size_t size;
        read(size);
        for (size_t i = 0; i < size; ++i) {
            T t;
            read(t);
            v.template emplace(t);
        }
        return *this;
    }

    template<typename T>
    Serializer &operator << (const std::multiset<T>& v){
        write(v.size());
        for(auto& t : v) {
            (*this) << t;
        }
        return *this;
    }

    template<typename T>
    Serializer &operator >> (std::unordered_set<T>& v){
        size_t size;
        read(size);
        for (size_t i = 0; i < size; ++i) {
            T t;
            read(t);
            v.template emplace(t);
        }
        return *this;
    }

    template<typename T>
    Serializer &operator << (const std::unordered_set<T>& v){
        write(v.size());
        for(auto& t : v) {
            (*this) << t;
        }
        return *this;
    }

    template<typename T>
    Serializer &operator >> (std::unordered_multiset<T>& v){
        size_t size;
        read(size);
        for (size_t i = 0; i < size; ++i) {
            T t;
            read(t);
            v.template emplace(t);
        }
        return *this;
    }

    template<typename T>
    Serializer &operator << (const std::unordered_multiset<T>& v){
        write(v.size());
        for(auto& t : v) {
            (*this) << t;
        }
        return *this;
    }


    template<typename K, typename V>
    Serializer &operator << (const std::pair<K,V>& m){
        (*this) << m.first << m.second;
        return *this;
    }

    template<typename K, typename V>
    Serializer &operator >> (std::pair<K,V>& m){
        (*this) >> m.first >> m.second;
        return *this;
    }

    template<typename K, typename V>
    Serializer &operator >> (std::map<K,V>& m){
        size_t size;
        read(size);
        for (size_t i = 0; i < size; ++i) {
            std::pair<K,V> p;
            (*this) >> p;
            m.template emplace(p);
        }
        return *this;
    }

    template<typename K, typename V>
    Serializer &operator << (const std::map<K,V>& m){
        write(m.size());
        for(auto& t : m) {
            (*this) << t;
        }
        return *this;
    }

    template<typename K, typename V>
    Serializer &operator >> (std::unordered_map<K,V>& m){
        size_t size;
        read(size);
        for (size_t i = 0; i < size; ++i) {
            std::pair<K,V> p;
            (*this) >> p;
            m.template emplace(p);
        }
        return *this;
    }

    template<typename K, typename V>
    Serializer &operator << (const std::unordered_map<K,V>& m){
        write(m.size());
        for(auto& t : m) {
            (*this) << t;
        }
        return *this;
    }

    template<typename K, typename V>
    Serializer &operator >> (std::multimap<K,V>& m){
        size_t size;
        read(size);
        for (size_t i = 0; i < size; ++i) {
            std::pair<K,V> p;
            (*this) >> p;
            m.template emplace(p);
        }
        return *this;
    }

    template<typename K, typename V>
    Serializer &operator << (const std::multimap<K,V>& m){
        write(m.size());
        for(auto& t : m) {
            (*this) << t;
        }
        return *this;
    }

    template<typename K, typename V>
    Serializer &operator >> (std::unordered_multimap<K,V>& m){
        size_t size;
        read(size);
        for (size_t i = 0; i < size; ++i) {
            std::pair<K,V> p;
            (*this) >> p;
            m.template emplace(p);
        }
        return *this;
    }

    template<typename K, typename V>
    Serializer &operator << (const std::unordered_multimap<K,V>& m){
        write(m.size());
        for(auto& t : m) {
            (*this) << t;
        }
        return *this;
    }

private:
    ByteArray::ptr m_byteArray;
};

}  // namespace mojo

}  // namespace tit
#endif //MOJO_IMPL_SERIALIZER_H
