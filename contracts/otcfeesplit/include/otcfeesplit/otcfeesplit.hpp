/*
 * @Author: your name
 * @Date: 2022-04-13 15:58:25
 * @LastEditTime: 2022-04-14 15:40:31
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: /deotc.contracts/contracts/otcfeesplit/include/otcfeesplit/otcfeesplit.hpp
 */
#pragma once

#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/transaction.hpp>
#include <eosio/system.hpp>
#include <eosio/crypto.hpp>
#include <eosio/action.hpp>
#include <string>

#include "wasm_db.hpp"
#include "otcfeesplit.db.hpp"

using namespace wasm::db;

namespace otc {

using eosio::asset;
using eosio::check;
using eosio::datastream;
using eosio::name;
using eosio::symbol;
using eosio::symbol_code;
using eosio::unsigned_int;

using std::string;

class [[eosio::contract("otcfeesplit")]] otcfeesplit: public eosio::contract {
private:
    global_singleton    _global;
    global_t            _gstate;
    dbc                 _db;
    
public:
    using contract::contract;
    otcfeesplit(eosio::name receiver, eosio::name code, datastream<const char*> ds):
        _db(_self),
        contract(receiver, code, ds), _global(_self, _self.value) {
        _gstate = _global.exists() ? _global.get() : global_t{};
    }

    ~otcfeesplit() {
        _global.set( _gstate, get_self() );
    }

    /**
     * reset the global with default values
     * only code maintainer can init
     */
    ACTION init(const name& admin);
    ACTION setratios(const map<name, uint32_t>& ratios, const bool& to_add);
    /**
     * ontransfer, trigger by recipient of transfer()
     */
    [[eosio::on_notify("amax.mtoken::transfer")]] 
    void ontransfer(const name& from, const name& to, const asset& quantity, const string& memo);

};

}
