#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "transaction.h"
#include "utxo_manager.h"
#include <string>
#include <set>
#include <vector>

using namespace std;

class validator
{
public:
    static bool validate(transaction &tx, utxo_manager &um, string &error_message)
    {
        if (tx.inputs.empty())
        {
            error_message = "No inputs";
            return false;
        }

        set<utxo_key> seen_inputs;
        double input_sum = 0;
        for (auto const &in : tx.inputs)
        {
            utxo_key key(in.prev_tx_id, in.index);
            if (seen_inputs.find(key) != seen_inputs.end())
            {
                error_message = "Duplicate input in transaction";
                return false;
            }
            seen_inputs.insert(key);

            if (!um.exists(in.prev_tx_id, in.index))
            {
                error_message = "Input UTXO does not exist";
                return false;
            }

            if (um.get_owner(in.prev_tx_id, in.index) != in.owner)
            {
                error_message = "Input UTXO owner mismatch";
                return false;
            }

            input_sum += um.get_amount(in.prev_tx_id, in.index);
        }

        double output_sum = 0;
        for (auto const &out : tx.outputs)
        {
            if (out.amount < 0)
            {
                error_message = "Negative output amount";
                return false;
            }
            output_sum += out.amount;
        }

        if (input_sum < output_sum)
        {
            error_message = "Insufficient funds";
            return false;
        }

        tx.fee = input_sum - output_sum;
        return true;
    }
};

#endif