// -------------------------------------------------
// Copyright 2016 Authors. All rights reserved.
// Create Date: 2016-7-15
// Author: Yiji Liao
// E-mail: 1360275010@qq.com
// -------------------------------------------------

// 计算每个词项的tfidf值，然后生成文本评分倒排列表，
// 分别按分数和id各排一次序输出，保存到磁盘上 

#include<cstdio>
#include<cstring>
#include<algorithm>
#include<cmath>
using namespace std;

const int maxt = 35;
const int maxn = 1000005;
const int maxl = 50;

struct node {
	int id;
	double score;
} Lt[maxt][maxn];

char s[maxl], word[maxl], termList[maxt][maxl];
int totalTerm, df[maxt], freq[maxt];
double tf[maxt][maxn], idf[maxt], tfidf[maxt][maxn];

bool cmp1(node i, node j) {
	return i.score > j.score;
}

bool cmp2(node i,node j) {
	return i.id < j.id;
}

char *getname(const char *a, char* b, const char *c) {
	char *s = new char[50];
	strcpy(s, a);
	strcat(s, b);
	strcat(s, c);
	return s;
}

void solve() {
	int n, t, totalWord;
	FILE *fp;
	n = 30000;
	memset(df, 0, sizeof(df));
	totalTerm = 0;
	scanf("%*d");
	for (int d = 1; d <= n; d++) {
		memset(freq, 0, sizeof(freq));
		totalWord = 0;
		scanf("%*d%*d");
		while (scanf("%s", word) != EOF && (word[0] < '0' || word[0] > '9')) {
			t = -1;
			for (int i = 1; i <= totalTerm; i++)
				if (strcmp(termList[i], word) == 0){
					t = i;
					break;
				}
			if (t == -1) {
				totalTerm++;
				strcpy(termList[totalTerm], word);
				t = totalTerm;
			}
			if (freq[t] == 0) {
				df[t]++;
				Lt[t][df[t]].id = d;
			}
			freq[t]++;
			totalWord++;
		}
		for (int i = 1; i <= totalTerm; i++)
			tf[i][d] = (double)freq[i]/totalWord;
	}
	
	for (int i = 1; i <= totalTerm; i++)
		idf[i] = log((double)n/df[i])/log(2.0);
	for (int i = 1; i <= totalTerm; i++)
		for (int d = 1; d <= n; d++)
			tfidf[i][d] = tf[i][d]*idf[i];
	for (int i = 1; i <= totalTerm; i++)
		for (int j = 1; j <= df[i]; j++)
			Lt[i][j].score = tfidf[i][Lt[i][j].id];
	
	for (int i = 1; i <= totalTerm; i++) {
		sort(Lt[i]+1, Lt[i]+df[i]+1, cmp1);
		fp = fopen(getname("inverted_list\\", termList[i], "_text.txt"), "w");
		fprintf(fp, "%d\n", df[i]);
		for (int j = 1; j <= df[i]; j++)
			fprintf(fp, "%d %lf\n", Lt[i][j].id, Lt[i][j].score);
		fclose(fp);
	}
	
	for (int i = 1; i <= totalTerm; i++) {
		sort(Lt[i]+1, Lt[i]+df[i]+1, cmp2);
		fp = fopen(getname("inverted_list\\", termList[i], "_id.txt"), "w");
		fprintf(fp, "%d\n", df[i]);
		for (int j = 1; j <= df[i]; j++)
			fprintf(fp, "%d %lf\n", Lt[i][j].id, Lt[i][j].score);
		fclose(fp);
	}
}

int main() {
	freopen("dataset.txt", "r", stdin);
	
	solve();
	return 0;
}
