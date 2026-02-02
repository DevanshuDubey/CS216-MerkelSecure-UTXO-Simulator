#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "transaction.h"
#include "utxo_manager.h"
#include <string>
#include <set>
#include <iostream>

using namespace std;

class mempool;

class validator
{
public:
    static double calculate_fee(const transaction &tx, utxo_manager &um)
    {
        double input_sum = 0;
        for (auto const &in : tx.getinputs())
        {
            input_sum += um.get_amount(in.getprev_tx(), in.getoutput_index());
        }

        double output_sum = 0;
        for (auto const &out : tx.getoutputs())
        {
            output_sum += out.getamount();
        }

        return input_sum - output_sum;
    }

    static bool validate_transaction(
        const transaction &tx,
        utxo_manager &um,
        mempool &mp,
        string &error_message,
        double &fee);
};

#include "mempool.h"

inline bool validator::validate_transaction(
    const transaction &tx,
    utxo_manager &um,
    mempool &mp,
    string &error_message,
    double &fee)
{
    if (tx.getinputs().empty())
    {
        error_message = "No inputs";
        return false;
    }

    set<utxo_key> seen_inputs;
    double input_sum = 0;
    for (auto const &in : tx.getinputs())
    {
        utxo_key key(in.getprev_tx(), in.getoutput_index());
        
        if (seen_inputs.find(key) != seen_inputs.end())
        {
            error_message = "Duplicate input in transaction";
            return false;
        }
        seen_inputs.insert(key);

        if (!um.exists(in.getprev_tx(), in.getoutput_index()))
        {
            error_message = "Input UTXO does not exist";
            return false;
        }

        if (mp.is_spent(key))
        {
            error_message = "UTXO already spent in mempool";
            return false;
        }

        if (um.get_owner(in.getprev_tx(), in.getoutput_index()) != in.getowner())
        {
            error_message = "Input UTXO owner mismatch";
            return false;
        }

        input_sum += um.get_amount(in.getprev_tx(), in.getoutput_index());
    }

    double output_sum = 0;
    for (auto const &out : tx.getoutputs())
    {
        if (out.getamount() <= 0)
        {
            error_message = "Output amount must be positive";
            return false;
        }
        output_sum += out.getamount();
    }

    if (input_sum < output_sum)
    {
        error_message = "Insufficient funds";
        return false;
    }

    fee = calculate_fee(tx, um);
    return true;
}

#endif