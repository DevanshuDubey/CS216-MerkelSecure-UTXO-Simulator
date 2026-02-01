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

void mine_new_block(mempool& mp, Blockchain& chain, utxo_manager& um, string miner_name, int tx_to_mine = 3) {
    cout << "Mining block..." << endl;

    
    vector<transaction> mempool_txs = mp.mine_top_transactions(tx_to_mine);

    if (mempool_txs.empty()) {
        cout << "Error: No transactions available in mempool to mine!" << endl;
        return;
    }

    int removed_count = mempool_txs.size();

    
    double total_fees = 0;
    for (const auto& tx : mempool_txs) {
        total_fees += tx.fee;
    }

    string coinbase_id = "CB-" + miner_name + "-" + to_string(time(0));
    transaction coinbase_tx(coinbase_id);
    coinbase_tx.add_output(total_fees, miner_name);
    coinbase_tx.fee = 0.0;

   
    vector<transaction> final_block_txs;
    final_block_txs.push_back(coinbase_tx);
    for (const auto& tx : mempool_txs) {
        final_block_txs.push_back(tx);
    }

    // 4. Update UTXO Manager for every transaction in the block
    for (const auto& tx : final_block_txs) {
        // Step A: Remove spent UTXOs (inputs)
        for (const auto& input : tx.inputs) {
            um.remove_utxo(input.prev_tx_id, input.index);
        }

        // Step B: Add new UTXOs (outputs)
        for (int i = 0; i < tx.outputs.size(); i++) {
            um.add_utxo(tx.tx_id, i, tx.outputs[i].amount, tx.outputs[i].recipient);
        }
    }

   
    block* new_mined_block = new block(miner_name, total_fees, final_block_txs, chain);
    chain.add_block(new_mined_block);

    cout << "Block mined successfully!" << endl;
    cout << "UTXO set updated. Miner " << miner_name << " balance updated." << endl;
}

#endif