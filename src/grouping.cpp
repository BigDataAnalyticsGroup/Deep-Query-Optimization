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
//          Immanuel Haffner <immanuel.haffner@bigdata.uni-saarland.de>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <chrono>
#include <random>
#include <unordered_map>
#include <vector>

#include "util/fn.hpp"
#include "util/distribution.hpp"

using namespace std::chrono;

constexpr std::size_t SIZE = 100000000;
constexpr std::size_t MAX_GROUPS = 40000;
constexpr std::size_t NUM_RUNS = 5;

using key_type = uint32_t;

struct aggregate_data
{
    uint64_t count = 0;
    uint64_t sum = 0;
};

struct group_type
{
    key_type key;
    aggregate_data data;

    group_type() = default;
    group_type(key_type k) : key(k) { }

    bool operator==(const group_type &other) const { return this->key == other.key; }
    bool operator!=(const group_type &other) const { return not operator==(other); }
    bool operator<(const group_type &other) const { return this->key < other.key; }
};

std::size_t inc_num_groups(std::size_t num_groups) {
    if (num_groups < 100) return num_groups + 1;
    if (num_groups < 1000) return num_groups + 10;
    if (num_groups < 10000) return num_groups + 100;
    return  num_groups + 1000;
}

/*======================================== Implementations ========================================*/

void order_based_grouping(const key_type *data, std::size_t size, std::size_t num_groups)
{
    group_type *groups = new group_type[num_groups];

    key_type k = *data;
    std::size_t idx = 0;
    groups[idx].key = k;
    aggregate_data *agg = &(groups[idx].data);
    for (auto p = data, end = data + size; p != end; ++p) {
        if (k != *p) {
            idx += 1;
            k = *p;
            groups[idx].key = k;
            agg = &(groups[idx].data);
        }
        agg->count += 1;
        agg->sum += k;
    }
#ifndef NDEBUG
    std::size_t n_grps = 0;
    std::size_t sum_count = 0;
    for (auto g = groups, end = groups + num_groups; g != end; ++g) {
        n_grps += 1; 
        sum_count += (g->data).count;
        assert((g->data.sum / g->data.count) == g->key);
    }
    assert(n_grps == num_groups);
    assert(sum_count == size);
#endif
    delete[] groups;
}

void sort_and_order_based_grouping(key_type *data, std::size_t size, std::size_t num_groups)
{
    std::sort(data, data + size);
    order_based_grouping(data, size, num_groups);
}

void hashing(const key_type *data, std::size_t size, std::size_t num_groups)
{
    std::unordered_map<key_type, aggregate_data, Murmur3> groups(num_groups);

    /* Group and aggregate. */
    for (auto p = data, end = data + size; p != end; ++p) {
        aggregate_data &agg = groups[*p];
        agg.count += 1;
        agg.sum += *p;
    }

#ifndef NDEBUG
    std::size_t n_grps = 0;
    std::size_t sum_count = 0;
    for (auto g : groups) {
        n_grps += 1; 
        sum_count += g.second.count;
        assert((g.second.sum / g.second.count) == g.first);
    }
    assert(n_grps == num_groups);
    assert(sum_count == size);
#endif
}

void static_perfect_hashing(const key_type *data, std::size_t size, std::size_t num_groups)
{
    group_type *groups = new group_type[num_groups];
    	
    for (auto p = data, end = data + size; p != end; ++p) {
        key_type k = *p;
        groups[k].key = k;
        aggregate_data &agg = groups[k].data;
        agg.count += 1;
        agg.sum += k;
    }

#ifndef NDEBUG
    std::size_t n_grps = 0;
    std::size_t sum_count = 0;
    for (auto g = groups, end = groups + num_groups; g != end; ++g) {
        n_grps += 1; 
        sum_count += (g->data).count;
        assert((g->data.sum / g->data.count) == g->key);
    }
    assert(n_grps == num_groups);
    assert(sum_count == size);
#endif
    delete[] groups;
}

void binary_search(const key_type *data, std::size_t size, std::size_t num_groups, const std::vector<key_type> &elements) {
    assert(elements.size() == num_groups);
    assert(std::is_sorted(elements.begin(), elements.end()));
    group_type *groups = new group_type[num_groups];

    /* Initialize array containing the groups with sparse keys. */
    std::size_t idx = 0;
    for (auto g = groups, end = groups + num_groups; g != end; ++g) {
        g->key = elements[idx];
        idx += 1;
    }
    
    /* Iterate over input and compute aggregate. */
    for (auto p = data, end = data + size; p != end; ++p) {
        key_type k = *p;
        auto g = std::lower_bound(groups, groups + num_groups, group_type(k));
        aggregate_data &agg = g->data;
        agg.count += 1;
        agg.sum += k;
    }

#ifndef NDEBUG
    std::size_t n_grps = 0;
    std::size_t sum_count = 0;
    for (auto g = groups, end = groups + num_groups; g != end; ++g) {
        n_grps += 1;
        sum_count += (g->data).count;
        assert((g->data.sum / g->data.count) == g->key);
    }
    assert(n_grps == num_groups);
    assert(sum_count == size);
#endif

    delete[] groups;
}

int main(int argc, char** argv) {
    std::string ofile;

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <OUTPUT-FILE>" << std::endl;
        return 1;
    }

    ofile = argv[1];

    /* Random Generator. */
    std::mt19937_64 g(42);

    /* Output file. */
    std::ofstream ofs {ofile};
    if (!ofs) {
        std::cerr << "Can't open output file " << ofile << '\n';
        return 1;
    }

#define RUN(Q,S,D) { \
    for (std::size_t i = 0; i < NUM_RUNS; ++i) { \
        auto start = high_resolution_clock::now(); \
        Q(data, SIZE, num_groups); \
        auto stop = high_resolution_clock::now(); \
        ofs << num_groups << "," << S << "," << D << "," << #Q << "," << i << "," << duration_cast<nanoseconds>(stop - start).count() / 1e6 << '\n'; \
    } \
}

#define RUN_SOG(Q,S,D) { \
    for (std::size_t i = 0; i < NUM_RUNS; ++i) { \
        std::copy(data, data + SIZE, copy_data); \
        auto start = high_resolution_clock::now(); \
        Q(copy_data, SIZE, num_groups); \
        auto stop = high_resolution_clock::now(); \
        ofs << num_groups << "," << S << "," << D << "," << #Q << "," << i << "," << duration_cast<nanoseconds>(stop - start).count() / 1e6 << '\n'; \
    } \
}

#define RUN_BSG(Q,S,D) { \
    for (std::size_t i = 0; i < NUM_RUNS; ++i) { \
        auto start = high_resolution_clock::now(); \
        Q(data, SIZE, num_groups, elements); \
        auto stop = high_resolution_clock::now(); \
        ofs << num_groups << "," << S << "," << D << "," << #Q << "," << i << "," << duration_cast<nanoseconds>(stop - start).count() / 1e6 << '\n'; \
    } \
}

    ofs << "groups,sortedness,density,algorithm,num_run,execution_time\n";

    std::size_t num_groups = 1;

    while (num_groups <= MAX_GROUPS) {
        key_type *data = new key_type[SIZE];
        key_type *copy_data = new key_type[SIZE];

        /* Sorted, uniform, dense data. */
        create_uniform_dense(data, SIZE, num_groups);

        RUN(hashing, "sorted", "dense");
        RUN(static_perfect_hashing, "sorted", "dense");
        RUN(order_based_grouping, "sorted", "dense");
        RUN_SOG(sort_and_order_based_grouping, "sorted", "dense");

        /* Unsorted, uniform, dense data. */
        std::shuffle(data, data + SIZE, g);
        RUN(hashing, "unsorted", "dense");
        RUN(static_perfect_hashing, "unsorted", "dense");
        RUN_SOG(sort_and_order_based_grouping, "unsorted", "dense");

        /* Sorted, uniform, sparse data. */
        const key_type min = std::numeric_limits<key_type>::min();
        const key_type max = std::numeric_limits<key_type>::max();
        std::uniform_int_distribution<key_type> d(min, max);
        std::vector<key_type> elements;
        std::size_t num_elements = 0;
        while (num_elements < num_groups) {
            key_type v = d(g);
            if (std::find(elements.begin(), elements.end(), v) != elements.end()) {
                continue; 
            }
            elements.push_back(v);
            num_elements += 1;
        }
        std::sort(elements.begin(), elements.end());
        create_uniform_sparse(data, SIZE, elements);

        RUN(hashing, "sorted", "sparse");
        RUN_BSG(binary_search, "sorted", "sparse");
        RUN(order_based_grouping, "sorted", "sparse");
        RUN_SOG(sort_and_order_based_grouping, "sorted", "sparse");

        /* Unsorted, uniform, sparse data. */
        std::shuffle(data, data + SIZE, g);
        RUN(hashing, "unsorted", "sparse");
        RUN_BSG(binary_search, "unsorted", "sparse");
        RUN_SOG(sort_and_order_based_grouping, "unsorted", "sparse");

        num_groups = inc_num_groups(num_groups);

        delete[] data;
        delete[] copy_data;
    }
    ofs.close();
#undef RUN
#undef RUN_SOG
#undef RUN_BSG
}
