# VillageSQL Games Extension

A collection of games and solvers for VillageSQL, providing high-performance SQL functions for game logic.

## Building

Requires the VillageSQL SDK and a C++17 compiler.

```bash
export VillageSQL_BUILD_DIR=/path/to/villagesql/build
./build.sh
```

The build produces `build/vsql_games.veb`.

## Installing

```sql
INSTALL EXTENSION vsql_games;
```

## Function Reference

### solve_sudoku(puzzle STRING) -> STRING
Accepts an 81+ character string representing a Sudoku puzzle.
- Characters '1'-'9' are treated as fixed cells.
- Any other character (space, dot, dash, newline) is treated as an empty cell.
- Returns the 81-digit solved string.
- Returns `NULL` if the input is too short (fewer than 81 slots), contains initial contradictions, or is unsolvable.

### print_sudoku(puzzle STRING) -> STRING
Accepts an 81-digit Sudoku string and returns a formatted 9x9 grid with row and box separators.

**Example:**
```sql
SELECT print_sudoku(solve_sudoku('...26.7.168..7..9.19...45..82.1...4...46.29...5...3.28..93...74.4..5..367.3.18...')) as result\G
```

**Output:**
```text
result: 4 3 5 | 2 6 9 | 7 8 1
6 8 2 | 5 7 1 | 4 9 3
1 9 7 | 8 3 4 | 5 6 2
------+-------+------
8 2 6 | 1 9 5 | 3 4 7
3 7 4 | 6 8 2 | 9 1 5
9 5 1 | 7 4 3 | 6 2 8
------+-------+------
5 1 9 | 3 2 6 | 8 7 4
2 4 8 | 9 5 7 | 1 3 6
7 6 3 | 4 1 8 | 2 5 9
```

**NULL-handling:** Returns `NULL` if the input is `NULL`.

## Known Limitations

- **No extension upgrade path:** `ALTER EXTENSION` is not supported. To update the extension, use `UNINSTALL` followed by `INSTALL`. Tracked in [villagesql-server#11](https://github.com/villagesql/villagesql-server/issues/11).
- **Aggregate support:** While `solve_sudoku` can be used inside `SUM()` via implicit casting, native VEF aggregate support for custom types is under development. Tracked in [villagesql-server#14](https://github.com/villagesql/villagesql-server/issues/14).

## Testing

See [TESTING.md](TESTING.md) for details on running the test suite.

## Reporting Bugs and Requesting Features

Please report issues on the [GitHub repository](https://github.com/villagesql/vsql-games/issues).

## Contact

- Discord: [https://discord.gg/KSr6whd3Fr](https://discord.gg/KSr6whd3Fr)
- GitHub Issues: [https://github.com/villagesql/vsql-games/issues](https://github.com/villagesql/vsql-games/issues)

## License

GPL-2.0
