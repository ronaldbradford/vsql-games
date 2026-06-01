/* Copyright (c) 2026 VillageSQL Contributors
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 */

#include <villagesql/vsql.h>

#include <array>
#include <string_view>

namespace {

struct SudokuBoard {
  std::array<int, 81> cells;
  std::array<int, 9> row_masks;
  std::array<int, 9> col_masks;
  std::array<int, 9> box_masks;

  static constexpr int idx(int r, int c) { return r * 9 + c; }
  static constexpr int box_idx(int r, int c) { return (r / 3) * 3 + (c / 3); }

  bool load(std::string_view input) {
    cells.fill(0);
    row_masks.fill(0);
    col_masks.fill(0);
    box_masks.fill(0);
    int count = 0;
    for (char c : input) {
      if (count == 81) break;
      if (c == '\r' || c == '\n') continue;
      if (c >= '1' && c <= '9') {
        int val = c - '0';
        int row = count / 9;
        int col = count % 9;
        int b = box_idx(row, col);
        int mask = 1 << val;
        if ((row_masks[row] | col_masks[col] | box_masks[b]) & mask) {
          return false; // Contradiction!
        }
        cells[count] = val;
        row_masks[row] |= mask;
        col_masks[col] |= mask;
        box_masks[b] |= mask;
      }
      count++;
    }
    return count == 81;
  }

  void format(vsql::Span<char> out) const {
    for (int i = 0; i < 81; ++i) {
      out[i] = static_cast<char>(cells[i] + '0');
    }
  }

  bool solve_recursive(int index) {
    if (index == 81) return true;
    if (cells[index] != 0) return solve_recursive(index + 1);

    int row = index / 9;
    int col = index % 9;
    int b = box_idx(row, col);
    int base_mask = row_masks[row] | col_masks[col] | box_masks[b];

    for (int val = 1; val <= 9; ++val) {
      int mask = 1 << val;
      if (!(base_mask & mask)) {
        cells[index] = val;
        row_masks[row] |= mask;
        col_masks[col] |= mask;
        box_masks[b] |= mask;

        if (solve_recursive(index + 1)) return true;

        cells[index] = 0;
        row_masks[row] &= ~mask;
        col_masks[col] &= ~mask;
        box_masks[b] &= ~mask;
      }
    }
    return false;
  }

  bool solve() { return solve_recursive(0); }

  bool is_complete_and_valid() const {
    for (int i = 0; i < 9; ++i) {
      if (row_masks[i] != 1022 || col_masks[i] != 1022 || box_masks[i] != 1022) {
        return false;
      }
    }
    return true;
  }
};

}  // namespace

void solve_sudoku_impl(vsql::StringArg puzzle, vsql::StringResult out) try {
  if (puzzle.is_null()) {
    out.set_null();
    return;
  }

  SudokuBoard board;
  if (!board.load(puzzle.value()) || !board.solve()) {
    out.set_null();
    return;
  }

  auto buf = out.buffer();
  if (buf.size() < 81) {
    out.error("Output buffer too small for Sudoku result");
    return;
  }

  board.format(buf);
  out.set_length(81);
} catch (...) {
  out.error("Internal error in solve_sudoku");
}

void sudoku_print_impl(vsql::StringArg puzzle, vsql::StringResult out) try {
  if (puzzle.is_null()) {
    out.set_null();
    return;
  }

  SudokuBoard board;
  if (!board.load(puzzle.value())) {
    out.error("print_sudoku requires a valid Sudoku puzzle input (81 cells)");
    return;
  }

  auto buf = out.buffer();
  if (buf.size() < 256) {
    out.error("Output buffer too small for Sudoku print");
    return;
  }

  size_t off = 0;
  for (int r = 0; r < 9; ++r) {
    if (r > 0 && r % 3 == 0) {
      const char* sep = "------+-------+------\n";
      size_t len = 22;
      memcpy(buf.data() + off, sep, len);
      off += len;
    }
    for (int c = 0; c < 9; ++c) {
      if (c > 0 && c % 3 == 0) {
        buf[off++] = '|';
        buf[off++] = ' ';
      }
      int val = board.cells[SudokuBoard::idx(r, c)];
      buf[off++] = (val == 0) ? '.' : static_cast<char>(val + '0');
      if (c < 8) {
        buf[off++] = ' ';
      }
    }
    buf[off++] = '\n';
  }
  out.set_length(off);
} catch (...) {
  out.error("Internal error in print_sudoku");
}

void is_valid_sudoku_impl(vsql::StringArg puzzle, vsql::IntResult out) try {
  if (puzzle.is_null()) {
    out.set_null();
    return;
  }

  SudokuBoard board;
  if (!board.load(puzzle.value())) {
    out.set(0);
    return;
  }

  out.set(board.is_complete_and_valid() ? 1 : 0);
} catch (...) {
  out.error("Internal error in is_valid_sudoku");
}

VEF_GENERATE_ENTRY_POINTS(
  vsql::make_extension()
    .func(vsql::make_func<&solve_sudoku_impl>("solve_sudoku")
      .returns(vsql::STRING)
      .param(vsql::STRING)
      .buffer_size(81)
      .deterministic()
      .build())
    .func(vsql::make_func<&sudoku_print_impl>("print_sudoku")
      .returns(vsql::STRING)
      .param(vsql::STRING)
      .buffer_size(256)
      .deterministic()
      .build())
    .func(vsql::make_func<&is_valid_sudoku_impl>("is_valid_sudoku")
      .returns(vsql::INT)
      .param(vsql::STRING)
      .deterministic()
      .build())
)
