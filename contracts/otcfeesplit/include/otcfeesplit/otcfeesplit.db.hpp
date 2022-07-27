 #pragma once

#include <eosio/asset.hpp>
#include <eosio/privileged.hpp>
#include <eosio/singleton.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>
#include <eosio/name.hpp>
#include <otcfeesplit/wasm_db.hpp>

#include <optional>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <type_traits>

#include <otcfeesplit/utils.hpp>

namespace otc {

using namespace std;
using namespace eosio;
using namespace wasm;

#define CONTRACT_TBL [[eosio::table, eosio::contract("otcfeesplit")]]

#define SYMBOL(sym_code, precision) symbol(symbol_code(sym_code), precision)

static constexpr name MIRROR_BANK = name("amax.mtoken");
static constexpr name AMAX_BANK = name("amax.token");
static constexpr name CNYD_BANK = name("cnyd.token");
static constexpr uint32_t percent_boost = 10000;

struct [[eosio::table("global"), eosio::contract("otcfeesplit")]] global_t {
    name admin = "armoniaadmin"_n;

    map<name,uint32_t> split_ratios = { 
        { "amax.daodev"_n,    2000 },
        { "meta.settle"_n,    4000 },
        { "meta.swap"_n,      4000 }
    };

    global_t() {}

    EOSLIB_SERIALIZE( global_t, (admin)(split_ratios) )
};
typedef eosio::singleton< "global"_n, global_t > global_singleton;

} // OTC