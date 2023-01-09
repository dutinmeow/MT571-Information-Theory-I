#include <bits/stdc++.h>
#include <local/random>
#include "board.hpp"

namespace std {

template<class Fun>
class y_combinator_result {
	Fun fun_;
public:
	template<class T>
	explicit y_combinator_result(T &&fun): fun_(std::forward<T>(fun)) {}

	template<class ...Args>
	decltype(auto) operator()(Args &&...args) {
		return fun_(std::ref(*this), std::forward<Args>(args)...);
	}
};

template<class Fun>
decltype(auto) y_combinator(Fun &&fun) {
	return y_combinator_result<std::decay_t<Fun>>(std::forward<Fun>(fun));
}

} // namespace std

int main() {
	freopen("configs.out", "w", stdout);
	std::array<int, 5> ships = {5, 4, 3, 3, 2};
	KnownBoard board;

	int cnt = 0;

	std::y_combinator([&](auto self, int i) -> void {
		std::cerr << i << '\n' << to_string(board) << '\n';
		if (i == (int)ships.size()) {
			cnt++;
			std::cout << to_string(board) << '\n';
			return;
		}

		int ship = ships[i];
		for (int r = 0; r < BOARD_SIZE; r++) {
			for (int c = 0; c < BOARD_SIZE; c++) {
				if (local::rng::rint(0, 7) < 7)
					continue;
				if (r + ship <= BOARD_SIZE && board.checkEmpty(r, c, r + ship - 1, c)) {
					board.addShip(r, c, r + ship - 1, c);
					self(i + 1);
					board.removeShip(r, c, r + ship - 1, c);
				}
				if (c + ship <= BOARD_SIZE && board.checkEmpty(r, c, r, c + ship - 1)) {
					board.addShip(r, c, r, c + ship - 1);
					self(i + 1);
					board.removeShip(r, c, r, c + ship - 1);
				}
			}
		}
	})(0);
}