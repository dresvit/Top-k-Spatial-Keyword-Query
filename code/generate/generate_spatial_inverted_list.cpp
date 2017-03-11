// -------------------------------------------------
// Copyright 2016 Authors. All rights reserved.
// Create Date: 2016-7-15
// Author: Yiji Liao
// E-mail: 1360275010@qq.com
// -------------------------------------------------

// 计算每篇文档的Z-order值，然后生成按Z-order排序的倒排列表

#include<cstdio>
#include<cstring>
#include<algorithm>
#include<cmath>
using namespace std;

const int maxc = 3010;
const int maxt = 35;
const int maxn = 1000005;
const int maxl = 50;

struct node {
	int id, order;
} Ls[maxt][maxn];

char word[maxl], termList[maxt][maxl];
int totalTerm, df[maxt], zorder[maxc][maxc];
bool vis[maxt];

bool cmp(node i, node j) {
	return i.order < j.order;
}

char *getname(const char *a, char* b, const char *c) {
	char *s = new char[50];
	strcpy(s, a);
	strcat(s, b);
	strcat(s, c);
	return s;
}

void solve() {
	int n, t, x, y;
	FILE *fp1, *fp2;
	
	n = 30000;
	
	fp1 = fopen("zorder.txt", "r");
	for (int i = 0; i < 3000; i++)
		for (int j = 0; j < 3000; j++)
			fscanf(fp1, "%d", &zorder[i][j]);
	fclose(fp1);
	
	fp1 = fopen("dataset.txt", "r");
	memset(df, 0, sizeof(df));
	totalTerm = 0;
	fscanf(fp1, "%*d");
	for (int d = 1; d <= n; d++) {
		memset(vis, false, sizeof(vis));
		fscanf(fp1, "%d%d", &x, &y);
		while (fscanf(fp1, "%s", word) != EOF && (word[0] < '0' || word[0] > '9')) {
			t = -1;
			for (int i = 1; i <= totalTerm; i++)
				if (strcmp(termList[i], word) == 0) {
					t = i;
					break;
				}
			if (t == -1) {
				totalTerm++;
				strcpy(termList[totalTerm], word);
				t = totalTerm;
			}
			if (!vis[t]) {
				vis[t] = true;
				df[t]++;
				Ls[t][df[t]].id = d;
				Ls[t][df[t]].order = zorder[y][x];
			}
		}
	}
	fclose(fp1);
	
	for (int i = 1; i <= totalTerm; i++) {
		sort(Ls[i]+1, Ls[i]+df[i]+1, cmp);
		fp2 = fopen(getname("inverted_list\\", termList[i], "_space.txt"), "w");
		fprintf(fp2, "%d\n", df[i]);
		for (int j = 1; j <= df[i]; j++)
			fprintf(fp2, "%d %d\n", Ls[i][j].id, Ls[i][j].order);
		fclose(fp2);
	}
}

int main() {
	solve();
	return 0;
}
