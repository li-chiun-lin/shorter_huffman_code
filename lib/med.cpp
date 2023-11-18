#include "pch.h"
#include "framework.h"

#include "lib.h"

void print(vector<int>& v)
{
	for (auto x : v)
		cout << setw(4) << x;
	cout << "\n";
}

void print(vector<vector<int>>& v)
{
	for (auto& r : v)
		print(r);
	cout << "\n";
}

vector<vector<int>> med(const vector<vector<int>>& v)
{
	auto m = v.size();
	auto n = v[0].size();
	vector<vector<int>> pv(m, vector<int>(n, -1));

	for (int i = 1; i < m; ++i)
		for (int j = 1; j < n; ++j)
		{
			if (v[i][j] <= min(v[i - 1][j], v[i][j - 1]))
				pv[i][j] = max(v[i - 1][j], v[i][j - 1]);
			else if (v[i][j] >= max(v[i - 1][j], v[i][j - 1]))
				pv[i][j] = min(v[i - 1][j], v[i][j - 1]);
			else
				pv[i][j] = v[i - 1][j] + v[i][j - 1] - v[i - 1][j - 1];
		}

	return pv;
}

vector<vector<int>> pred_err(const vector<vector<int>>& p, const vector<vector<int>>& pv, int ref)
{
	auto m = p.size();
	auto n = p[0].size();
	vector<vector<int>> pe(m, vector<int>(n, -1));

	for (int j = ref; j < n; ++j)
		pe[0][j] = p[0][j - 1] ^ p[0][j];

	for (int i = 1; i < m; ++i)
		pe[i][0] = p[i - 1][0] ^ p[i][0];

	for (int i = 1; i < m; ++i)
		for (int j = 1; j < n; ++j)
			pe[i][j] = p[i][j] ^ pv[i][j];

	return pe;
}

vector<vector<int>> emb_cap(const vector<vector<int>>& pe, int ref)
{
	auto m = pe.size();
	auto n = pe[0].size();
	vector<vector<int>> t(m, vector<int>(n, -1));

	for (int j = ref; j < n; ++j)
		t[0][j] = pe[0][j] ? (7 - (int)log2(pe[0][j])) : 8;

	for (int i = 1; i < m; ++i)
		for (int j = 0; j < n; ++j)
			t[i][j] = pe[i][j] ? (7 - (int)log2(pe[i][j])) : 8;

	return t;
}
