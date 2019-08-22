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

struct Murmur3
{
    uint32_t operator()(uint32_t v) const {
        v ^= v >> 16;
        v *= 0x85ebca6b;
        v ^= v >> 13;
        v *= 0xc2b2ae35;
        v ^= v >> 16;
        return v;
    }

    uint64_t operator()(uint64_t v) const {
        v ^= v >> 33;
        v *= 0xff51afd7ed558ccd;
        v ^= v >> 33;
        v *= 0xc4ceb9fe1a85ec53;
        v ^= v >> 33;
        return v;
    }
};
