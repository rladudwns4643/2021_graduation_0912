#pragma once

#include <fstream>

class CRankMng
{
public:
	CRankMng() = default;
	~CRankMng() {}

	int ranking[5];

	void load(char* _fileName = "RANK.txt") {
		std::ifstream in(_fileName);
		for (int i = 0; i < 5; ++i)	in >> ranking[i];
		in.close();
	}

	// load 선행 필수
	void save(int _score, char* _fileName = "RANK.txt") {
		std::ofstream out(_fileName);

		if (_score > ranking[4]) {
			ranking[4] = _score;
			for (int i = 0; i < 5; ++i) {
				for (int j = 0; j < 4; ++j) {
					if (ranking[j] < ranking[j + 1]) {
						int temp = ranking[j];
						ranking[j] = ranking[j + 1];
						ranking[j + 1] = temp;
					}
				}
			}
			for (int i = 0; i < 5; ++i) out << ranking[i] << std::endl;
		}
		out.close();
	}
};

