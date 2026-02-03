<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
</head>
<body>

    <h1>Team Merkel Secure: Bitcoin UTXO Simulator</h1>
    <p>This project is a high-fidelity <strong>Bitcoin Transaction & UTXO Simulator</strong> developed for the <strong>CS 216: Introduction to Blockchain</strong> coding assignment. It simulates the core mechanisms of the Bitcoin ledger, including UTXO management, transaction validation, and the mining lifecycle.</p>

    <hr>

    <h2>👥 Team Information</h2>
    <ul>
        <li><strong>Team Name</strong>: Merkel Secure</li>
        <li><strong>Members</strong>:
            <ul>
                <li><strong>Devanshu Dubey</strong> (Roll No: 240001024)</li>
                <li><strong>Abhinav Patel</strong> (Roll No: 240001004)</li>
                <li><strong>Pratyush Gupta</strong> (Roll No: 240001054)</li>
                <li><strong>Abhay Lodhi</strong> (Roll No: 240001003)</li>
            </ul>
        </li>
    </ul>

    <hr>

    <h2>🛠️ Design Explanation</h2>
    <p>The simulator is architected to mirror the decentralized logic of a blockchain node using Object-Oriented Programming (OOP) in C++.</p>

    <h3>Core Architecture</h3>
    <ol>
        <li><strong>UTXO Model</strong>: Instead of an account-based system, we track "Unspent Transaction Outputs". Each transaction consumes existing UTXOs as inputs and creates new ones as outputs.</li>
        <li><strong>Modular Validation</strong>: A standalone <code>validator</code> class enforces consensus rules. It checks for double-spending, non-existent inputs, and ensures that the sum of inputs covers the outputs and fees.</li>
        <li><strong>Mempool with Priority</strong>: Valid transactions are held in a <code>std::priority_queue</code> ordered by fee. This simulates miner incentives where high-fee transactions are confirmed first.</li>
        <li><strong>Linked-List Blockchain</strong>: The blockchain is implemented as a reverse <strong>linked list</strong>. Each block node contains a pointer to its predecessor (<code>prevBlock</code>), maintaining a verifiable and secure chain of history.</li>
        <li><strong>Double-Spend Prevention</strong>: We use a "first-seen" rule. A <code>spent_utxos</code> set in the mempool locks inputs immediately, rejecting any subsequent conflicting transactions.</li>
    </ol>

    <hr>

    <h2>🚀 How to Run</h2>
    <h3>Prerequisites</h3>
    <ul>
        <li>A C++ compiler supporting C++11 or higher (GCC/G++, Clang, or MSVC).</li>
        <li>Standard Template Library (STL) — No external blockchain libraries were used.</li>
    </ul>

    <h3>Build Instructions</h3>
    <ol>
        <li>Navigate to the project directory:
            <pre><code>cd CS216-TeamMerkelSecure-UTXO-Simulator</code></pre>
        </li>
        <li>Compile the source code:
            <pre><code>g++ -o utxo_sim src/main.cpp</code></pre>
        </li>
        <li>Execute the simulator:
            <pre><code>./utxo_sim</code></pre>
        </li>
    </ol>

    <hr>

    <h2>📁 Project Structure</h2>
    <pre><code>your-repository/
├── src/
│   ├── main.cpp        # Entry point and Menu Interface
│   ├── utxo_manager.h  # Global state of confirmed coins
│   ├── transaction.h   # Transaction, Input, and Output classes
│   ├── mempool.h       # Priority-based pending storage
│   ├── validator.h     # Consensus rule enforcement
│   ├── block.h         # Linked-list block node structure
│   └── blockchain.h    # Manager for the chain of blocks
├── tests/
│   └── scenarios.cpp   # Automated mandatory test cases
└── README.md           # Project documentation</code></pre>

    <hr>

    <h2>📋 Features & Requirements Met</h2>
    <ul>
        <li>✅ <strong>UTXO Manager</strong>: Dictionary-based lookup for spendable coins.</li>
        <li>✅ <strong>Fee Logic</strong>: Automatic fee calculation (Input - Output) and priority sorting.</li>
        <li>✅ <strong>Double-Spend Protection</strong>: Detection in both the Mempool and the permanent UTXO set.</li>
        <li>✅ <strong>Mining Simulation</strong>: Selection of top transactions and coinbase fee rewards.</li>
        <li>✅ <strong>Linked Blockchain</strong>: Blocks linked via pointers to simulate the chain tip.</li>
    </ul>

</body>
</html>
