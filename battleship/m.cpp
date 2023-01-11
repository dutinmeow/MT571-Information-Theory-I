#include <bits/stdc++.h>
#include <local/random>
#include <local/debug>

const short N = 10, ODB = 0.1;
const int M = 1'000'000, B = 50, P = 6, K = 3;
const double H = 0.95, THR = 0.75, EPS = 1e-9;
const std::string HIT = "✅", MISS = "❌";

short compressState(short x, short y, short dir) { return x * N + y + dir * N * N; }

std::tuple<short, short, short> uncompressState(short s) { 
	return {s / N % N, s % N, s / N / N}; 
}

void displayBoard(const std::array<std::array<float, N>, N> &arr) {
	std::cerr << "    ";
	for (int i = 0; i < N; i++) {
		std::cerr << i;
		for (int j = 0; j <= P + 1; j++)
			std::cerr << ' ';
	}
	std::cerr << '\n';
	std::cerr << "  +";
	for (int i = 0; i < N; i++)
		for (int j = 0; j <= P + 2; j++)
			std::cerr << '-';
	std::cerr << '\n';
	for (int i = 0; i < N; i++) {
		std::cerr << i << " | ";
		for (int j = 0; j < N; j++) {
			std::cerr << arr[i][j];
			if (j < N - 1)
				std::cerr << ' ';
		}
		std::cerr << '\n';
	}
}

void displayGradient(const std::array<std::array<float, N>, N> &arr) {
	const static int num = 5;
	const static std::array<std::string, 5> grad = {"🖕🏿", "🖕🏾", "🖕🏽", "🖕🏼", "🖕🏻"};
	const static std::string opt = "🖕";
	// const static std::array<std::string, 8> grad = {"🟥", "🟧", "🟨", "🟩", "🟦", "🟪", "🟫", "⬛️"};

	float amx = 0.0, amn = 1.0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (arr[i][j] >= 2.0)
				continue;
			amx = std::max(amx, arr[i][j]);
			amn = std::min(amn, arr[i][j]);
		}
	}

	float sep = (amx - amn) / (float)num;
	std::cerr << "    0 1 2 3 4 5 6 7 8 9\n  +---------------------\n";
	for (int i = 0; i < N; i++) {
		std::cerr << i << " | ";
		for (int j = 0; j < N; j++) {
			if (arr[i][j] == 2.0) {
				std::cerr << MISS;
				continue;
			} else if (arr[i][j] == 3.0) {
				std::cerr << HIT;
				continue;
			}

			if (arr[i][j] == amx) {
				std::cerr << opt;
				continue;
			}
			int ind = (arr[i][j] - amn + EPS) / sep;
			if (ind == -1)
				ind++;
			if (ind == num)
				ind--;
			// if (ind < 0 || ind > num) {
			// 	std::cerr << arr[i][j] << ' ' << amn << ' ' << amx << ' ' << ind << '\n';
			// }
			std::cerr << grad[ind];
		}
		std::cerr << '\n';
	}
}

int main() {
	std::cerr << std::fixed << std::setprecision(6);
	int numShips = 5;
	std::vector<short> shipSizes{2, 3, 3, 4, 5};
	std::array<std::array<char, N>, N> board;
	for (auto &r : board)
		r.fill('U');
	int moves = 0;
	short numHits = 0, total = std::accumulate(shipSizes.begin(), shipSizes.end(), 0);

	std::string mode;
	std::cout << "Enable automatic tester?\n";
	std::cin >> mode;

	bool automatic = (mode == "YES");
	std::array<std::array<short, N>, N> god;
	if (automatic) {
		for (auto &r : god)
			r.fill(-1);
		for (int i = 0; i < numShips; i++) {
			short len = shipSizes[i];
			while (true) {
				int x = local::rng::rint(0, N - 1);
				int y = local::rng::rint(0, N - 1);
				int d = local::rng::rint(0, 1);
				if (d == 0) {
					if (x + len - 1 < N) {
						bool another = false;
						for (int xx = x; xx < x + len; xx++) {
							if (god[xx][y] != -1) {
								another = true;
								break;
							}
						}
						if (another)
							continue;
						for (int xx = x; xx < x + len; xx++)
							god[xx][y] = i;
						break;
					}
				} else {
					if (y + len - 1 < N) {
						bool another = false;
						for (int yy = y; yy < y + len; yy++) {
							if (god[x][yy] != -1) {
								another = true;
								break;
							}
						}
						if (another)
							continue;
						for (int yy = y; yy < y + len; yy++)
							god[x][yy] = i;
						break;
					}
				}
			}
		}

		std::cerr << BOLD << "Automatic Mode Enabled\nRandomly Generated Board Configuration:\n" << RESET;
		std::cout << "  0123456789\n +----------\n";
		for (int x = 0; x < N; x++) {
			std::cerr << x << "|";
			for (int y = 0; y < N; y++) {
				if (god[x][y] == -1)
					std::cerr << ' ';
				else
					std::cerr << god[x][y];
			}
			std::cerr << '\n';
		}
		std::cerr << '\n';

		std::string valid;
		std::cout << "Confirm that the generated board is valid: ";
		while (true) {
			std::cin >> valid;
			if (valid == "NO") {
				return 0;
			} else if (valid == "YES") {
				break;
			} else {
				std::cout << "Invalid answer, please enter again: ";
			}
		}

	}

	for (; numHits < total; moves++) {
		std::cerr << "loading ship positions...\n";
		std::vector<std::vector<short>> shipPositions(numShips);
		for (short i = 0; i < numShips; i++) {
			short len = shipSizes[i];
			// std::cerr << "possible locations for ship of length " << len << '\n';
			for (short x = 0; x < N; x++) {
				for (short y = 0; y < N; y++) {
					if (x + len - 1 < N) {
						bool seenSunk = false, seenUnknown = false, seenMiss = false;
						for (int xx = x; xx < x + len; xx++) {
							if (board[xx][y] == 'S')
								seenSunk = true;
							else if (board[xx][y] == 'U')
								seenUnknown = true;
							else if (board[xx][y] == 'M')
								seenMiss = true;
						}
						if (!seenMiss && !(seenSunk && seenUnknown)) {
							shipPositions[i].push_back(compressState(x, y, 0));
							// std::cerr << "(" << x << ", " << y << ") facing down\n";
						}
					}
					if (y + len - 1 < N) {
						bool seenSunk = false, seenUnknown = false, seenMiss = false;
						for (int yy = y; yy < y + len; yy++) {
							if (board[x][yy] == 'S')
								seenSunk = true;
							else if (board[x][yy] == 'U')
								seenUnknown = true;
							else if (board[x][yy] == 'M')
								seenMiss = true;
						}
						if (!seenMiss && !(seenSunk && seenUnknown)) {
							shipPositions[i].push_back(compressState(x, y, 1));
							// std::cerr << "(" << x << ", " << y << ") facing right\n";
						}
					}
				}
			}
		}

		std::cerr << "generating configurations... (with examples)\n";
		int validConfigs = 0;
		std::array<std::array<short, N>, N> curFreq;
		std::array<std::array<int, N>, N> freqHit;
		for (short x = 0; x < N; x++)
			for (short y = 0; y < N; y++)
				freqHit[x][y] = 0;
		for (int itr = 0; itr < M || validConfigs < B; itr++) {
			for (int x = 0; x < N; x++)
				for (int y = 0; y < N; y++)
					curFreq[x][y] = 0;
			bool invalid = false;
			for (int i = 0; i < numShips; i++) {
				int ind = local::rng::rint(0, (int)shipPositions[i].size() - 1);
				auto [x, y, d] = uncompressState(shipPositions[i][ind]);
				if (d == 0) {
					for (int xx = x; xx < x + shipSizes[i]; xx++) {
						if (curFreq[xx][y] > 0) {
							invalid = true;
							break;
						}
						curFreq[xx][y]++;
					}
				} else {
					for (int yy = y; yy < y + shipSizes[i]; yy++) {
						if (curFreq[x][yy] > 0) {
							invalid = true;
							break;
						}
						curFreq[x][yy]++;
					}
				}
			}
			
			if (invalid) 
				continue;
			for (int x = 0; x < N; x++) {
				for (int y = 0; y < N; y++) {
					if (board[x][y] == 'H' || board[x][y] == 'S') {
						if (curFreq[x][y] == 0) {
							invalid = true;
							break;
						}
					}
				}
			}

			if (invalid) 
				continue;
			for (int x = 0; x < N; x++) 
				for (int y = 0; y < N; y++) 
					if (curFreq[x][y] == 1)
						freqHit[x][y]++;
			validConfigs++;

			if (validConfigs < K) {
				std::cerr << "  0123456789\n +----------\n";
				for (int x = 0; x < N; x++) {
					std::cerr << x << '|';
					for (int y = 0; y < N; y++) {
						std::cerr << (curFreq[x][y] == 0 ? ' ' : '*') << RESET;
					}
					std::cerr << '\n';
				}
				std::cerr << '\n';
			}
		}

		std::cerr << "Number of Valid Configurations are " << validConfigs << '\n';

		std::cerr << "calculating entropy...\n";
		float maxEntropy = 0.0;
		int shotX, shotY;
		std::array<std::array<float, N>, N> entropy;
		for (int x = 0; x < N; x++) {
			for (int y = 0; y < N; y++) {
				if (board[x][y] != 'U') {
					entropy[x][y] = (board[x][y] == 'M' ? 2.0 : 3.0);
					continue;
				}
				double hit = (double)freqHit[x][y] / validConfigs;
				double miss = 1.0 - hit;
				if (hit > THR) {
					maxEntropy = 1.0;
					shotX = x, shotY = y;
					break;
				}
				entropy[x][y] = (hit < EPS || miss > 1.0 - EPS ? 0.0 : hit * log2(1.0 / hit) * H) + 
					(miss < EPS || miss > 1.0 - EPS ? 0.0 : miss * log2(1.0 / miss) * (1.0 - H));
				if ((x + y) % 2 == 0)
					entropy[x][y] += ODB;
				if (entropy[x][y] > maxEntropy) {
					maxEntropy = entropy[x][y];
					shotX = x, shotY = y;
				}
			}
		}

		std::cerr << BOLD << "Currently at Step " << moves << '\n' << RESET;

		std::cerr << "\n###    Entropy Distribution   ###\n";
		displayBoard(entropy);

		std::cerr << "\n###    Entropy Heat Map       ###\n";
		displayGradient(entropy);

		std::cerr << "We've hit " << numHits << " / 17 ships so far\n";

		std::cerr << GREEN << BOLD << "\n\n\nGuess at (" << shotX << ", " << shotY << ")\n" << RESET;

		std::cout << "Response: ";
		std::string verdict;
		if (automatic) {
			assert(god[shotX][shotY] != -2);
			if (god[shotX][shotY] == -1) {
				verdict = "MISS";
			} else {
				int tem = god[shotX][shotY];
				god[shotX][shotY] = -2;
				bool anyLeft = false;
				for (int i = 0; i < N; i++) {
					for (int j = 0; j < N; j++) {
						if (god[i][j] == tem) {
							anyLeft = true;
							break;
						}
					}
					if (anyLeft)
						break;
				}
				if (anyLeft)
					verdict = "HIT";
				else
					verdict = "SUNK";
			}
			std::cout << verdict << '\n';
		} else {
			while (true) {
				std::cin >> verdict;
				if (verdict == "STOP" || verdict == "BREAK" || verdict == "RETURN") {
					return 0;
				} else if (verdict != "HIT" && verdict != "MISS" && verdict != "SUNK") {
					std::cout << "Invalid response, enter again: ";
				} else {
					break;
				}
			}
		}

		if (verdict == "HIT") {
			board[shotX][shotY] = 'H';
			numHits++;
		} else if (verdict == "MISS") {
			board[shotX][shotY] = 'M';
		} else if (verdict == "SUNK") {
			board[shotX][shotY] = 'S';
			numHits++;
		}
	}

	std::cout << BOLD << CYAN << "\n\nTotal Moves: " << moves << RESET << "\n\n";
	std::cout << "  0123456789\n +----------\n";
	for (int x = 0; x < N; x++) {
		std::cout << x << '|';
		for (int y = 0; y < N; y++) {
			std::string col = "";
			if (board[x][y] == 'H' || board[x][y] == 'S')
				col = GREEN;
			else if (board[x][y] == 'M')
				col = RED;
			if (board[x][y] == 'U')
				std::cout << ' ';
			else
				std::cout << col << '*' << RESET;
		}
		std::cout << '\n';
	}


	std::cerr << BOLD << "Automatic Mode Enabled\nRandomly Generated Board Configuration:\n" << RESET;
	std::cout << "  0123456789\n +----------\n";
	for (int x = 0; x < N; x++) {
		std::cerr << x << "|";
		for (int y = 0; y < N; y++) {
			if (god[x][y] == -1)
				std::cerr << ' ';
			else
				std::cerr << GREEN << '*' << RESET;
		}
		std::cerr << '\n';
	}
	std::cerr << '\n';

}