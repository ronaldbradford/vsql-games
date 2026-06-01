# GEMINI.md

## Project Context

This extension provides game-related utilities for VillageSQL.

## Implementation Details

### solve_sudoku
- **Input**: `STRING`
- **Output**: `STRING` (81 digits)
- **Algorithm**: Backtracking solver.
- **Sanitization**: Treats non-digit characters as empty cells. Skips `\r` and `\n`.
- **Validation**: Rejects puzzles with initial contradictions or fewer than 81 slots.

## Testing

Tests are located in `mysql-test/`. Use `mysql-test-run.pl` to verify.
Ensure no residual state is left between tests.
Uninstall the extension at the end of each test file.
