#ifndef VALIDATOR_H
#define VALIDATOR_H

#include <string>
#include "transaction.h"
#include "utxo_manager.h"
#include "mempool.h"
#include <set>
#include <iostream>
using namespace std;

class validator
{
public:
    static bool validate_transaction(
        const transaction &tx,
        UTXOManager &utxo_manager,
        mempool &mempool,
        string &error_message,
        double &fee)
    {

        set<pair<string, int>> seen_inputs;
        double input_sum = 0.0;

        for (const tx_input &in : tx.getinputs())
        {
            pair<string, int> key = {in.getprev_tx_id(), in.getoutput_index()};

            if (seen_inputs.find(key) != seen_inputs.end())
            {
                error_message = "Duplicate input";
                return false;
            }

            seen_inputs.insert(key);

            if (!utxo_manager.exists(in.getprev_tx_id(), in.getoutput_index()))
            {
                error_message = "UTXO does not exist";
                return false;
            }

            if (mempool.is_spent(key))
            {
                error_message = "UTXO is spent in mempool";
                return false;
            }

            input_sum += utxo_manager.getAmount(in.getprev_tx_id(),
                                                in.getoutput_index());

            double output_sum = 0.0;
            for (const tx_output &out : tx.getoutputs())
            {
                if (out.getamount() < 0)
                {
                    error_message = "Negative output amount";
                    return false;
                }
                output_sum += out.getamount();
            }

            if (input_sum < output_sum)
            {
                cout << "input_sum: " << input_sum << " output_sum: " << output_sum << endl;

                error_message = "Insufficient input amount";
                return false;
            }

            fee = input_sum - output_sum;
        }

        return true;
    }
};

#endif // VALIDATOR_H