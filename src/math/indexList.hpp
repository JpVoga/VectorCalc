#pragma once

#include <vector>
#include <variant>
#include <iostream>
#include <type_traits>
#include "util.hpp"
#include "../lib/glad/glad.h"
#include "../lib/GLFW/glfw3.h"

#ifndef GL_UNSIGNED_BYTE
    #define GL_UNSIGNED_BYTE 0
#endif

#ifndef GL_UNSIGNED_SHORT
    #define GL_UNSIGNED_SHORT 1
#endif

#ifndef GL_UNSIGNED_INT
    #define GL_UNSIGNED_INT 2
#endif

namespace Math {
    template<typename T>
    concept validIndexType = (
        (std::is_same_v<std::decay_t<T>, uint8_t >) ||
        (std::is_same_v<std::decay_t<T>, uint16_t>) ||
        (std::is_same_v<std::decay_t<T>, uint32_t>)
    );

    class IndexList
    {
        private:
            static constexpr const size_t possibleEnumTypes[3] = {
                GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT
            };

            std::variant<std::vector<uint8_t>, std::vector<uint16_t>, std::vector<uint32_t>> data;

        public:
            // Statics/free functions:

            friend std::ostream &operator<<(std::ostream &out, const IndexList &indexList)
            {
                out << "IndexList" << (indexList.getTypeByteDepth() * 8) << '{';
                for (size_t i = 0; i < indexList.getCount(); i++)
                {
                    if (i > 0) out << ',' << ' ';
                    out << indexList.getAt(i);
                }
                out << '}';

                return out;
            }


            // Constructors:

            IndexList()
            {
                this->data = (std::vector<uint8_t>());
            }

            template<validIndexType T>
            IndexList(const T * const newdata, const size_t count)
            {
                this->setData(newdata, count);
            }

            IndexList(const indexable1TimeType auto data, size_t count) {
                using DataT = std::decay_t<decltype(data[0])>;
                static_assert(validIndexType<DataT>);

                if (count == 0) {
                         if constexpr (std::is_same_v<DataT, uint8_t >) this->data = (std::vector<uint8_t >());
                    else if constexpr (std::is_same_v<DataT, uint16_t>) this->data = (std::vector<uint16_t>());
                    else if constexpr (std::is_same_v<DataT, uint32_t>) this->data = (std::vector<uint32_t>());
                }
                else this->setData(&(data[0]), count);
            }

            template<validIndexType T>
            IndexList(const std::vector<T> &vectorData)
            {
                this->data = vectorData;
            }

            template<validIndexType T>
            IndexList(std::vector<T> &&vectorData)
            {
                this->data = std::move(vectorData);
            }

            IndexList(const IndexList &other)
            {
                this->data = other.data;
            }

            IndexList(IndexList &&other)
            {
                this->data = std::move(other.data);
                other.data = (std::vector<uint8_t>());
            }


            // Indices getters setters:

            template<validIndexType T>
            void setData(const T * const newData, const size_t count)
            {
                std::vector<T> newVector = {};
                newVector.reserve(count);
                for (size_t i = 0; i < count; ++i) newVector.emplace_back(newData[i]);
                this->data = (std::move(newVector));
            }

            uint32_t getAt(size_t index) const
            {
                return std::visit(
                    ([=](const auto &v) -> uint32_t {return v.at(index);}),
                    data);
            }

            void setAt(size_t index, uint32_t newValue)
            {
                return std::visit(
                    ([=](auto &v) -> void {v[index] = newValue;}),
                    data);
            }

            void ensureCapacity(size_t minimumCapacity)
            {
                if (minimumCapacity <= (this->getCapacity())) return;
                else return std::visit(
                    ([=](auto &v) -> void {v.reserve(minimumCapacity);}),
                    data);
            }

            void add(uint32_t newValue)
            {
                return std::visit(
                    ([=](auto &v) -> void {v.emplace_back(newValue);}),
                    data);
            }

            void removeLast()
            {
                return std::visit(
                    ([=](auto &v) -> void {v.pop_back();}),
                    data);
            }

            void insert(size_t index, uint32_t newValue)
            {
                return std::visit(
                    ([=](auto &v) -> void {v.insert((v.begin() + index), newValue);}),
                    data);
            }

            template<typename InputIterator>
            void insert(size_t index, InputIterator begin, size_t count)
            {
                return std::visit(
                    ([=](auto &v) -> void {v.insert((v.begin() + index), begin, (begin+count));}),
                    data);
            }

            void removeAt(size_t index)
            {
                return std::visit(
                    ([=](auto &v) -> void {v.erase(v.begin() + index);}),
                    data);
            }

            void removeAt(size_t begin, size_t count)
            {
                return std::visit(
                    ([=](auto &v) -> void {
                        v.erase((v.begin() + begin), (v.begin() + begin + count));}),
                    data);
            }


            // Read general object data:
            
            void *getDataPtr()
            {
                return std::visit(([=](auto &v) -> void * {return v.data();}), data);
            }

            const void *getDataPtr() const
            {
                return std::visit(([=](const auto &v) -> const void * {return v.data();}), data);
            }

            size_t getCount() const
            {
                return std::visit(
                    ([=](const auto &v) -> size_t {return v.size();}),
                    data);
            }

            size_t getCapacity() const
            {
                return std::visit(
                    ([=](const auto &v) -> size_t {return v.capacity();}),
                    data);
            }

            size_t getTypeByteDepth() const
            {
                static constexpr size_t possibleResults[3] = {sizeof(uint8_t),sizeof(uint16_t),sizeof(uint32_t)};
                return (possibleResults[data.index()]);
            }

            size_t getMemorySize() const {return ((getCount()) * (getTypeByteDepth()));}

            size_t getTypeEnum() const {return (possibleEnumTypes[data.index()]);}


            // Change number type:

            void setTypeEnum(size_t newTypeEnum)
            {
                switch (newTypeEnum)
                {
                    case GL_UNSIGNED_BYTE:  return (void)setTypeEnumTypeSpecific<uint8_t>();
                    case GL_UNSIGNED_SHORT: return (void)setTypeEnumTypeSpecific<uint16_t>();
                    case GL_UNSIGNED_INT:   return (void)setTypeEnumTypeSpecific<uint32_t>();
                    default: std::cerr << "Invalid index type enum \'" << newTypeEnum << "\'\n";
                }
            }

            template<validIndexType T>
            void setTypeEnumTypeSpecific()
            {
                std::vector<T> newVector = {};
                size_t count = this->getCount();

                newVector.reserve(std::max(count, this->getCapacity()));
                for (size_t i = 0; i < count; ++i) newVector.emplace_back(static_cast<T>(this->getAt(i)));

                this->data = std::move(newVector);
            }


            // Operators:

            IndexList &operator=(const IndexList &other)
            {
                this->data = other.data;
                return (*this);
            }

            IndexList &operator=(IndexList &&other)
            {
                if ((this) != (&other))
                {
                    this->data = std::move(other.data);
                    other.data = (std::vector<uint8_t>());
                }

                return (*this);
            }
    };
}