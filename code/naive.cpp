// -------------------------------------------------
// Copyright 2016 Authors. All rights reserved.
// Create Date: 2016-7-15
// Author: Yiji Liao
// E-mail: 1360275010@qq.com
// -------------------------------------------------

#include<cstdio>
#include<cstring>
#include<cmath>
#include<ctime>
#include<algorithm>
using namespace std;

const int maxk = 35;
const int maxl = 50;
const int maxn = 1000010;

// 参数说明 
// n: 文档总数
// m: 当前查询的词项个数 
// K: top-K的K值 
// totalTerm: 词项总数
// termList: 词项列表，记录出现过的词项 
// Listlength: 每个倒排列表的长度 
// pc: pc[i]表示当前查询中第i个词项在总词项列表中的下标
// dis: dis[i]表示id为i的文档与查询Q之间的距离
// idList: 按id排序的倒排列表 
// recList: 记录所有文档的总评分的列表

struct point {
	int x, y;
} location[maxn], query;

struct listNode {
	int id;
	double score;
} idList[maxk][maxn], recList[maxn];

int n, m, K, totalTerm, Listlength[maxk], pc[maxk];
double alpha, gamma, dis[maxn];
char termList[maxk][maxl];
FILE *fp1, *fp2;

inline void readint(FILE *fp, int &ret) {
	char c;
	if (c = fgetc(fp), c == EOF) return;
	while (c < '0' || c > '9') c = fgetc(fp);
	ret = c-'0';
	while (c = fgetc(fp), c >= '0' && c <= '9') ret = ret*10+(c-'0');
}

inline void readdb(FILE *fp, double &ret) {
	char c;
	int x, y = 0, div = 1;
	readint(fp, x);
	while (c = fgetc(fp), c >= '0' && c <= '9') {
		y = y*10+(c-'0');
		div *= 10;
	}
	ret = (double)y/div+x;
}

void loadLocation() {
	fp1 = fopen("location.txt", "r");
	for (int i = 1; i <= n; i++) {
		readint(fp1, location[i].x);
		readint(fp1, location[i].y);
	}
	fclose(fp1);
}

double sqr(double x) {
	return x*x;
}

double calDis(point a, point b) {
	return sqrt(sqr(a.x-b.x)+sqr(a.y-b.y));
}

void findMaxLocation() {
	gamma = -1;
	for (int i = 1; i <= n; i++) {
		dis[i] = calDis(location[i], query);
		if (gamma < dis[i]) gamma = dis[i];
	}
}

void initialize() {
	n = 30000;
	K = 20;
	totalTerm = 0;
	alpha = 0.4;
	loadLocation();
}

char *getname(const char *a, char* b, const char *c) {
	char *s = new char[50];
	strcpy(s, a);
	strcat(s, b);
	strcat(s, c);
	return s;
}

int findTerm(char *word) {
	for (int i = 1; i <= totalTerm; i++)
		if (strcmp(termList[i], word) == 0) return i;
	return -1;
}

void loadInvertedList(char *word) {
	totalTerm++;
	strcpy(termList[totalTerm], word);
	fp2 = fopen(getname("inverted_list\\", word, "_id.txt"), "r");
	readint(fp2, Listlength[totalTerm]);
	for (int i = 1; i <= Listlength[totalTerm]; i++) {
		readint(fp2, idList[totalTerm][i].id);
		readdb(fp2, idList[totalTerm][i].score);
	}
	fclose(fp2);
}

void readdata() {
	char ch, word[20];
	int len;
	readint(fp1, query.x);
	readint(fp1, query.y);
	readint(fp1, m);
	for (int i = 1; i <= m; i++) {
		len = 0;
		while (ch = fgetc(fp1), ch >= 'a' && ch <= 'z')
			word[len++] = ch;
		word[len] = '\0';
		pc[i] = findTerm(word);
		if (pc[i] == -1) {
			loadInvertedList(word);
			pc[i] = totalTerm;
		}
	}
}

bool cmp(listNode i, listNode j) {
	if (i.score == j.score) return i.id < j.id;
	else return i.score > j.score;
}

void solve() {
	findMaxLocation();
	for (int i = 1; i <= n; i++) {
		recList[i].score = 0;
		recList[i].id = i;
	}
	for (int i = 1; i <= m; i++)
		for (int j = 1; j <= Listlength[pc[i]]; j++)
			recList[idList[pc[i]][j].id].score += alpha*idList[pc[i]][j].score;
	for (int i = 1; i <= n; i++)
		if (recList[i].score > 0)
			recList[i].score += (1-alpha)*(1-dis[i]/gamma);
	sort(recList+1, recList+n+1, cmp);
}

void outputdata(int queryID) {
	printf("Query %d\n", queryID);
	for (int i = 1; i <= K; i++)
		printf("id = %d   score = %lf\n", recList[i].id, recList[i].score);
	printf("\n");
}

int main() {
	freopen("answer_naive.txt", "w", stdout);
	
	int T;
	double startTime_total, endTime_total, elapsedTime_total = 0;
	double startTime_read, endTime_read, elapsedTime_read = 0;
	double startTime_solve, endTime_solve, elapsedTime_solve = 0;
	
	startTime_total = clock();
	startTime_read = clock();
	initialize();
	endTime_read = clock();
	elapsedTime_read += (endTime_read - startTime_read) / CLOCKS_PER_SEC;
	
	fp1 = fopen("queryset.txt", "r");
	readint(fp1, T);
	for (int i = 1; i <= T; i++) {
		startTime_read = clock();
		readdata();
		endTime_read = clock();
		elapsedTime_read += (endTime_read - startTime_read) / CLOCKS_PER_SEC;
		
		startTime_solve = clock();
		solve();
		endTime_solve = clock();
		elapsedTime_solve += (endTime_solve - startTime_solve) / CLOCKS_PER_SEC;
	//	printf("Query %d solve time: %.3lfs\n", i, (endTime_solve - startTime_solve) / CLOCKS_PER_SEC);
		
		outputdata(i);
	}
	endTime_total = clock();
	elapsedTime_total = (endTime_total - startTime_total) / CLOCKS_PER_SEC;
	
//	printf("Read data time: %.3lfs\n", elapsedTime_read);
	printf("Solve time: %.3lfs\n", elapsedTime_solve);
//	printf("Total time: %.3lfs\n", elapsedTime_total);
	fclose(fp1);
	return 0;
}
