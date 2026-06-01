# Testing VillageSQL Games

This extension uses the MySQL Test Runner (MTR) framework for verification.

## Prerequisites

- VillageSQL build directory.
- The `vsql_games.veb` package copied to the server's extension directory.

## Running Tests

From the server's `mysql-test` directory:

```bash
perl mysql-test-run.pl --suite=/path/to/vsql-games/mysql-test
```

## Regenerating Results

If the output changes intentionally, update the expected result files:

```bash
perl mysql-test-run.pl --suite=/path/to/vsql-games/mysql-test --record
```

## Test Files

| File | Description |
|------|-------------|
| `sudoku_basic.test` | Verifies Sudoku solver against various puzzles and error conditions. |
