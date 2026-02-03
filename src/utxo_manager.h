#ifndef UTXO_MANAGER_H
#define UTXO_MANAGER_H

#include <string>
#include <map>
#include <vector>

using namespace std;

class utxo_key
{
public:
    string tx_id;
    int index;

    utxo_key(string id = "", int idx = 0) {
        this->tx_id = id;
        this->index = idx;
    }

    bool operator<(const utxo_key &other) const
    {
        if (tx_id != other.tx_id){
            return tx_id < other.tx_id;
        }
        return index < other.index;
    }
};

class utxo_value
{
public:
    double amount;
    string owner;

    utxo_value(double amt = 0.0, string own = ""){
        this->amount = amt;
        this->owner = own;
    }
};

class utxo_manager
{
 
    
    public:
    map<utxo_key, utxo_value> utxo_set;
    void add_utxo(string tx_id, int index, double amount, string owner)
    {
        utxo_set[utxo_key(tx_id, index)] = utxo_value(amount, owner);
    }

    void remove_utxo(string tx_id, int index)
    {
        utxo_set.erase(utxo_key(tx_id, index));
    }

    bool exists(string tx_id, int index)
    {
        return utxo_set.find(utxo_key(tx_id, index)) != utxo_set.end();
    }

    double get_amount(string tx_id, int index)
    {
        auto it = utxo_set.find(utxo_key(tx_id, index));
        if (it != utxo_set.end()){
            return it->second.amount;
        }
        return 0;
    }

    string get_owner(string tx_id, int index)
    {
        auto it = utxo_set.find(utxo_key(tx_id, index));
        if (it != utxo_set.end()){
            return it->second.owner;
        }
        return "";
    }

    double get_balance(string owner)
    {
        double balance = 0;
        for (auto const &[key, val] : utxo_set)
        {
            if (val.owner == owner){
                balance += val.amount;
            }
        }
        return balance;
    }

    void show_utxos(string owner)
    {
        for (auto const &[key, val] : utxo_set)
        {
            if(val.owner == owner)
            cout<<key.index + 1<<". "<<val.amount<<" BTC"<<endl;
        }
    }

    map<utxo_key, utxo_value> get_utxo_set()
    {
        return utxo_set;
    }

    vector<pair<utxo_key, utxo_value>> get_utxos_for_owner(string owner)
    {
        vector<pair<utxo_key, utxo_value>> result;
        for (auto const &[key, val] : utxo_set)
        {
            if (val.owner == owner)
            {
                result.push_back({key, val});
            }
        }
        return result;
    }
};

#endif