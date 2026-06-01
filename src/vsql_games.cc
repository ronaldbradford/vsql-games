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

  bool load(std::string_view input) {
    int count = 0;
    cells.fill(0);
    for (char c : input) {
      if (count == 81) break;
      if (c == '\r' || c == '\n') continue;
      if (c >= '1' && c <= '9') {
        int val = c - '0';
        int row = count / 9;
        int col = count % 9;
        if (!is_valid(row, col, val)) return false;
        cells[count] = val;
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

  bool solve() { return solve_recursive(0); }

 private:
  static constexpr int idx(int r, int c) { return r * 9 + c; }

  bool is_valid(int row, int col, int val) const {
    for (int i = 0; i < 9; ++i) {
      if (cells[idx(row, i)] == val || cells[idx(i, col)] == val) return false;
    }

    int box_r = (row / 3) * 3;
    int box_c = (col / 3) * 3;
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        if (cells[idx(box_r + i, box_c + j)] == val) return false;
      }
    }
    return true;
  }

  bool solve_recursive(int index) {
    if (index == 81) return true;
    if (cells[index] != 0) return solve_recursive(index + 1);

    int row = index / 9;
    int col = index % 9;
    for (int val = 1; val <= 9; ++val) {
      if (is_valid(row, col, val)) {
        cells[index] = val;
        if (solve_recursive(index + 1)) return true;
        cells[index] = 0;
      }
    }
    return false;
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

  std::string_view input = puzzle.value();
  if (input.size() != 81) {
    out.error("print_sudoku requires exactly 81 digits");
    return;
  }

  auto buf = out.buffer();
  // Format: "N N N | N N N | N N N\n" (21 chars) * 9 rows
  // Plus 2 separator lines: "------+-------+------\n" (22 chars) * 2
  // Total approx: 189 + 44 = 233 chars.
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
      buf[off++] = input[r * 9 + c];
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
)
