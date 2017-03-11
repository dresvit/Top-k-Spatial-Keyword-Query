// -------------------------------------------------
// Copyright 2016 Authors. All rights reserved.
// Create Date: 2016-7-15
// Author: Yiji Liao
// E-mail: 1360275010@qq.com
// -------------------------------------------------

#include<cstdio>
#include<cstring>
#include<algorithm>
using namespace std;

const int maxn = 5010;

int a[maxn][maxn];

void solve() {
	int n, x, y, ed;
	n = 3000; ed = 26;
	for (int i = 0; i < n; i++) {
		x = 0;
		for (int k = 0; k < ed; k++)
			if (i & (1<<k)) x |= 1<<(2*k+1);
		for (int j = 0; j < n; j++) {
			y = 0;
			for (int k = 0; k < ed; k++)
				if (j & (1<<k)) y |= 1<<(2*k);
			a[i][j] = x|y;
		}
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++)
			printf("%d ", a[i][j]);
		printf("\n");
	}
}

int main() {
	freopen("zorder.txt", "w", stdout);
	
	solve();
	return 0;
}
