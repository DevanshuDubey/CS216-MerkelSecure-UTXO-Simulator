#include <iostream>
#include <string>
#include <vector>
#include "utxo_manager.h"
#include "transaction.h"
#include "mempool.h"

using namespace std;

void display_menu()
{
    cout << "\n=== Bitcoin transaction Simulator ===" << endl;
    cout << "Main Menu:" << endl;
    cout << "1. Create new transaction" << endl;
    cout << "2. View UTXO set" << endl;
    cout << "3. View mempool" << endl;
    cout << "4. Exit" << endl;
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
    initialize_genesis(um);

    int choice;
    while (true)
    {
        display_menu();
        if (choice == 1)
        {
            string sender, recipient;
            double amount;
            cout << "Enter sender: ";
            cin >> sender;
            cout << "Available balance: " << um.get_balance(sender) << " BTC" << endl;
            cout << "Enter recipient: ";
            cin >> recipient;
            cout << "Enter amount: ";
            cin >> amount;

            transaction tx("tx_" + to_string(time(0)));
            auto utxos = um.get_utxo_set();
            double total_input = 0;
            for (auto const &[key, val] : utxos)
            {
                if (val.owner == sender)
                {
                    tx.add_input(key.tx_id, key.index, sender);
                    total_input += val.amount;
                    if (total_input >= amount + 0.001){
                        break;
                    }
                }
            }

            if (total_input < amount + 0.001)
            {
                cout << "Insufficient funds (including 0.001 fee)" << endl;
            }
            else
            {
                tx.add_output(amount, recipient);
                if (total_input > amount + 0.001)
                {
                    tx.add_output(total_input - amount - 0.001, sender);
                }
                auto res = mp.add_transaction(tx, um);
                if (res.first)
                {
                    cout << "transaction valid! Fee: 0.001 BTC" << endl;
                    cout << "transaction ID: " << tx.tx_id << endl;
                    cout << "transaction added to mempool." << endl;
                }
                else
                {
                    cout << "Error: " << res.second << endl;
                }
            }
        }
        else if (choice == 2)
        {
            auto utxos = um.get_utxo_set();
            cout << "\n--- UTXO Set ---" << endl;
            for (auto const &[key, val] : utxos)
            {
                cout << "(" << key.tx_id << ", " << key.index << "): " << val.amount << " BTC, Owner: " << val.owner << endl;
            }
        }
        else if (choice == 3)
        {
            auto txs = mp.get_all_transactions();
            cout << "\n--- Mempool ---" << endl;
            for (auto const &tx : txs)
            {
                cout << "TX ID: " << tx.tx_id << ", Fee: " << tx.fee << " BTC" << endl;
            }
        }
        else if (choice == 4)
        {
            break;
        }
    }

    return 0;
}