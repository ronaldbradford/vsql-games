# Sudoku Solver Architecture

## Foundation
- **Extension Name:** vsql-games
- **Primary Function:** solve_sudoku
- **Language:** cpp
- **PostgreSQL Port:** false
- **SDK Directory:** /Users/rbradfor/.villagesql/villagesql-extension-sdk-0.0.4
- **Preview APIs:** none (stable-only)

## Function Map
- `solve_sudoku(STRING) -> STRING`: Accepts 81+ character Sudoku puzzle, returns solved 81-digit string.

## Design
- **Solver Algorithm:** Backtracking with basic pruning.
- **Input Sanitization:** Extract the first 81 digits ('1'-'9'). Treat any other character as an empty cell.
- **Validation:** 
  - Must have at least 81 slots.
  - Initial digits must not violate Sudoku rules (row/column/box uniqueness).
- **Output:**
  - 81-digit string on success.
  - NULL on failure (invalid input, unsolvable, too short).
