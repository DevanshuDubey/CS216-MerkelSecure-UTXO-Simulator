#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "utxo_manager.h"
#include "transaction.h"
#include "mempool.h"
#include "validator.h"
#include "blockchain.h"
#include "mining.h"
#include "../tests/test_scenarios.h"

using namespace std;

void display_menu()
{
    cout << "\nMain Menu:" << endl;
    cout << "1. Create new transaction" << endl;
    cout << "2. View UTXO set" << endl;
    cout << "3. View mempool" << endl;
    cout << "4. Mine block" << endl;
    cout << "5. Run test scenarios" << endl;
    cout << "6. Exit" << endl;
    cout << "Enter choice: ";
}

void initialize_genesis(utxo_manager &um)
{
    um.add_utxo("genesis", 0, 50.0, "Alice");
    um.add_utxo("genesis", 1, 30.0, "Bob");
    um.add_utxo("genesis", 2, 20.0, "Charlie");
    um.add_utxo("genesis", 3, 10.0, "David");
    um.add_utxo("genesis", 4, 5.0, "Eve");
}

int main()
{
    utxo_manager um;
    mempool mp;
    blockchain chain;
    initialize_genesis(um);

    cout << "=== Bitcoin Transaction Simulator ===" << endl;
    cout << "Initial UTXOs (Genesis Block):" << endl;
    cout << "- Alice: 50.0 BTC" << endl;
    cout << "- Bob: 30.0 BTC" << endl;
    cout << "- Charlie: 20.0 BTC" << endl;
    cout << "- David: 10.0 BTC" << endl;
    cout << "- Eve: 5.0 BTC" << endl;

    int choice;
    while (true)
    {
        display_menu();
        cin >> choice;

        if (choice == 1)
        {
            string sender, address;
            double amount, change, send;
            cout << "Enter sender : ";
            cin >> sender;

            double balance = um.get_balance(sender);
            cout << "Available balance : " << balance << " BTC" << endl;
            set<int> st;
            if (balance <= 0)
            {
                cout << "Error: Sender has no balance." << endl;
                continue;
            }
            else
            {

                cout << "Select the input utxos by their index. Enter 0 when you finish selecting." << endl;
                um.show_utxos(sender);
                int idx;

                do
                {
                    cin >> idx;
                    if (idx != 0)
                        st.insert(idx - 1);

                } while (idx != 0);
            }
            transaction tx("tx_" + to_string(time(0)));

            for (auto const &[key, val] : um.utxo_set)
            {
                
                if (st.find(key.index) == st.end())
                    continue;

                
                if (val.owner != sender)
                    continue;

                tx.add_input(key.tx_id, key.index, sender);
                amount += val.amount;
            }

            cout << "Enter recipient : ";
            cin >> address;
            cout << "Enter amount to send: ";
            cin >> send;
            cout << "Enter change:";
            cin >> change;

            if (change > amount)
            {
                cout << "Error: Insufficient funds." << endl;
                continue;
            }

            auto utxos = um.get_utxo_set();
            double total_input = 0;

            // Main payment output
            tx.add_output(send, address);
    
            if (change > 0)
            {
                tx.add_output(change, sender);
            }

            cout << "\n--- Transaction Summary ---" << endl;
            cout << "Sender:    " << sender << endl;
            cout << "Recipient: " << address << endl;
            cout << "UTXO Amount Used:    " << amount << " BTC" << endl;
            cout << "Amount Sent    " << send << " BTC" <<endl;
            cout << "Change:    " << change << " BTC" << endl;
            cout << "Fee:       " << amount - change - send << " BTC" << endl;
            cout << "---------------------------" << endl;
            cout << "Confirm transaction? (y/n): ";

            char confirm;
            cin >> confirm;

            if (confirm == 'y' || confirm == 'Y')
            {
                auto result = mp.add_transaction(tx, um);
                if (result.first)
                {
                    cout << "Transaction valid! Fee: " << tx.fee << " BTC" << endl;
                    cout << "Transaction ID: " << tx.tx_id << endl;
                    cout << "Transaction added to mempool." << endl;
                    cout << "Mempool now has "
                         << mp.get_all_transactions().size()
                         << " transactions." << endl;
                }
                else
                {
                    cout << "Error: " << result.second << endl;
                }
            }
            else
            {
                cout << "Transaction cancelled." << endl;
            }
        }
        else if (choice == 2)
        {
            auto utxos = um.get_utxo_set();
            cout << "\n--- Current UTXO Set ---" << endl;
            for (auto const &[key, val] : utxos)
            {
                cout << "TX: " << key.tx_id << " [" << key.index << "] | Amount: " << val.amount << " | Owner: " << val.owner << endl;
            }
        }
        else if (choice == 3)
        {
            auto txs = mp.get_all_transactions();
            cout << "\n--- Current Mempool (Sorted by Fee) ---" << endl;
            if (txs.empty())
                cout << "(Empty)" << endl;
            for (auto const &t : txs)
            {
                cout << "ID: " << t.tx_id << " | Fee: " << t.fee << " BTC" << endl;
            }
        }
        else if (choice == 4)
        {
            string miner;
            int num;
            cout << "Enter miner name: ";
            cin >> miner;
            cout << "Enter number of blocks to mine: ";
            cin >> num;
            mine_block(miner, mp, um, chain, num);
        }
        else if (choice == 5)
        {
            cout << "\n--- Running Test Scenarios ---" << endl;
            run_tests();
        }
        else if (choice == 6)
        {
            return 0;
        }
        else if (choice == 256)
        {
            chain.display_status();
        }
        else
        {
            cout << "Invalid choice." << endl;
        }
    }

    return 0;
}