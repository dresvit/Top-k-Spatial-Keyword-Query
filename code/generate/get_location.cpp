// -------------------------------------------------
// Copyright 2016 Authors. All rights reserved.
// Create Date: 2016-7-15
// Author: Yiji Liao
// E-mail: 1360275010@qq.com
// -------------------------------------------------

// 从数据集中获取文档坐标并保存起来 

#include<cstdio>
#include<cstring>
#include<algorithm>
using namespace std;

const int maxl = 100100;

char s[maxl];

void solve() {
	int n, x, y;
	n = 30000;
	for (int d = 1; d <= n; d++) {
		scanf("%*d%d%d", &x, &y);
		getchar();
		gets(s);
		printf("%d %d\n", x, y);
	}
}

int main() {
	freopen("dataset.txt", "r", stdin);
	freopen("location.txt", "w", stdout);
	solve();
	return 0;
}
