// -------------------------------------------------
// Copyright 2016 Authors. All rights reserved.
// Create Date: 2016-7-15
// Author: Yiji Liao
// E-mail: 1360275010@qq.com
// -------------------------------------------------

#include<cstdio>
#include<cstring>
#include<algorithm>
#include<ctime>
#include<cstdlib>
using namespace std;

const int maxn = 110;
const int maxco = 2999;

const char *word[] = {"apple", "banana", "pear", "grape", "watermelon",
					"orange", "peach", "strawberry", "mango", "lemon",
					"coconut", "apricot", "plum", "blueberry", "cherry",
					"cucumber", "tomato", "potato", "carrot", "cabbage",
					"onion", "pumpkin", "mushroom", "pepper", "bean"};

bool f[maxn];

void solve() {
	int n, m, x, y, now;
	n = 50;
	printf("%d\n", n);
	for (int i = 1; i <= n; i++) {
		x = (double)rand()/RAND_MAX*maxco;
		y = (double)rand()/RAND_MAX*maxco;
		printf("%d %d\n", x, y);
		
		m = 3;
		printf("%d ", m);
		memset(f, false, sizeof(f));
		for (int j = 1; j <= m; j++) {
			now = rand()%25;
			while (f[now]) now = rand()%25;
			f[now] = true;
			printf("%s", word[now]);
			if (j == m) printf("\n"); else printf(" ");
		}
	}
}

int main() {
	freopen("queryset.txt", "w", stdout);
	
	srand((unsigned)time(NULL));
	solve();
	return 0;
}
