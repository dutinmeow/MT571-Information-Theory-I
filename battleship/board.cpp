#include <array>
#include <string>

const std::size_t BOARD_SIZE = 10;

class board_t {
	std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> board;
	// 'h' is hit, 'm' is miss, 'u' is unknown, 's' is sunk

public:
	board_t() {
		for (auto &row : board)
			row.fill('u');
	}

	void setIndex(int i, int j, char c) { board[i][j] = c; }

	const std::array<char, BOARD_SIZE> &operator[](std::size_t index) const { 
		return board[index]; 
	}
};

std::string to_string(const board_t &board, bool display_unknown = false) {
	const static std::string RED = "\033[31m";
	const static std::string GREEN = "\033[32m";
	const static std::string BLUE = "\033[34m";
	const static std::string RESET = "\033[0m";

	std::string ret = "  1234567890\n +----------\n";
	for (int i = 0; i < BOARD_SIZE; i++) {
		ret += std::string(1, 'a' + i) + '|';
		for (int j = 0; j < BOARD_SIZE; j++) {
			std::string col = "";
			switch (board[i][j]) {
			case 'h':
				col = GREEN;
				break;
			case 'm':
				col = RED;
				break;
			case 's':
				col = BLUE;
				break;
			default:
				col = "";
			}
			if (col == "" && !display_unknown)
				ret += col + ' ' + RESET;
			else 
				ret += col + board[i][j] + RESET;
		}
		ret += '\n';
	}
	return ret;
}