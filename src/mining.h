#ifndef MINING_ENGINE_H
#define MINING_ENGINE_H

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "mempool.h"
#include "blockchain.h"
#include "block.h"
#include "utxo_manager.h"

using namespace std;

void mine_block(string miner_name, mempool& mp, utxo_manager& um, blockchain& chain, int num_txs = 5) {
    auto all_pending = mp.get_all_transactions();
    int count = min((int)all_pending.size(), num_txs);
    
    if (count == 0) {
        cout << "Error: No transactions available in mempool to mine!" << endl;
        return;
    }

    cout << "Mining block ..." << endl;
    vector<transaction> mempool_txs = mp.mine_top_transactions(num_txs);

    double total_fees = 0;
    for (const auto& tx : mempool_txs) {
        total_fees += tx.fee;
    }

    cout << "Selected " << mempool_txs.size() << " transactions from mempool ." << endl;
    cout << "Total fees : " << total_fees << " BTC" << endl;
    cout << "Miner " << miner_name << " receives " << total_fees << " BTC" << endl;

    string coinbase_id = "CB-" + miner_name + "-" + to_string(time(0));
    transaction coinbase_tx(coinbase_id);
    if (total_fees > 0) {
        coinbase_tx.add_output(total_fees, miner_name);
    }
    coinbase_tx.fee = 0.0;

    vector<transaction> final_block_txs;
    final_block_txs.push_back(coinbase_tx);
    for (const auto& tx : mempool_txs) {
        final_block_txs.push_back(tx);
    }

    for (const auto& tx : final_block_txs) {
        for (const auto& input : tx.inputs) {
            um.remove_utxo(input.prev_tx, input.index);
        }

        for (int i = 0; i < (int)tx.outputs.size(); i++) {
            um.add_utxo(tx.tx_id, i, tx.outputs[i].amount, tx.outputs[i].address);
        }
    }

    block* new_mined_block = new block(miner_name, total_fees, final_block_txs, chain.get_latest_block());
    chain.add_block(new_mined_block);

    cout << "Block mined successfully !" << endl;
    cout << "Removed " << mempool_txs.size() << " transactions from mempool ." << endl;
}

#endif