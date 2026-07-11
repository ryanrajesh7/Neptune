# Neptune Chess Engine

A lightweight, functional chess engine built from scratch in C++. Neptune parses board states, enforces fundamental chess rules, and executes a brute-force lookahead search to make tactical move decisions. 

## Features & Implementation Details

*   **Lookahead Depth:** The engine looks ahead up to **4 plies (Depth 4)** to calculate its best move response. 
*   **Move Validation:** Correctly handles piece movements, capturing mechanics, and basic turn-by-turn game states.
*   **Tactical Baseline:** Uses a straightforward evaluation function to weight material values and positional basics—capable of defeating absolute beginners and casual players who leave pieces undefended.

## Tech Stack & Environment
*   **Language:** C++ 
*   **Environment:** Linux / Command Line Interface (CLI)
*   **Compilation:** Standard `g++` compiler

