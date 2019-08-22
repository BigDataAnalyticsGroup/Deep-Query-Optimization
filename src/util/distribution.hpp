//  Licence:
//      Copyright 2019 Joris Nix
//
//      Licensed under the Apache License, Version 2.0 (the "License");
//      you may not use this file except in compliance with the License.
//      You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//      Unless required by applicable law or agreed to in writing, software
//      distributed under the License is distributed on an "AS IS" BASIS,
//      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//      See the License for the specific language governing permissions and
//      limitations under the License.
//
//  Author: Joris Nix <joris.nix@bigdata.uni-saarland.de>

#pragma once

#include <iostream>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <limits>
#include <random>
#include <unordered_set>

template<typename T>
void create_uniform_dense(T *data, std::size_t size, std::size_t num_groups) {
    using std::sort;
    static_assert(std::numeric_limits<T>::is_integer, "type T is not an integer type");

    for (T *p = data, *end = data + size; p != end;) {
        for (std::size_t i = 0; i < num_groups && p != end; ++i, ++p) {
            *(p) = i;
        }
    }
    sort(data, data + size);
}

template<typename T, typename RandomGenerator>
void create_uniform_dense_shuffled(T *data, std::size_t size, std::size_t num_groups, RandomGenerator& g) {
    using std::shuffle;
    create_uniform_dense(data, size, num_groups);
    shuffle(data, data + size, g);
}

template<typename T>
void create_uniform_sparse(T *data, std::size_t size, const std::vector<T> &vec)
{
    using std::sort;
    for (T *p = data, *end = data + size; p != end;) {
        for (std::size_t i = 0, v_size = vec.size(); i < v_size && p != end; ++i, ++p) {
            *(p) = vec[i];
        }
    }
    sort(data, data + size);
}

template<typename T, typename RandomGenerator>
void create_uniform_sparse_shuffled(T *data, std::size_t size, const std::vector<T> &vec, RandomGenerator &g)
{
    using std::shuffle;
    for (T *p = data, *end = data + size; p != end;) {
        for (std::size_t i = 0, v_size = vec.size(); i < v_size && p != end; ++i, ++p) {
            *(p) = vec[i];
        }
    }
    shuffle(data, data + size, g);
}
