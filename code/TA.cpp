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
#include<vector>
#include<algorithm>
using namespace std;

const int maxk = 35;
const int maxl = 50;
const int maxn = 1000010;
const int maxK = 210;

// 参数说明 
// n: 文档总数
// m: 当前查询的词项个数 
// K: top-K的K值 
// threshold: 临界值 
// totalTerm: 词项总数
// termList: 词项列表，记录出现过的词项 
// Listlength: 每个倒排列表的长度 
// pc: pc[i]表示当前查询中第i个词项在总词项列表中的下标
// recList: 记录已经访问过的文档
// posHeap: posHeap[i]表示id为i的文档在堆中的下标
// Wk: 堆中的最小值
// dis: dis[i]表示id为i的文档与查询Q之间的距离
// W: 文档评分的下界值
// E: 记录每篇文档已经被访问过的属性
// topkHeap: 维护topk大的二叉堆 
// idList: 按id排序的倒排列表 
// scoreList: 按分数从大到小排序的倒排列表

int n, m, K;
int totalTerm, Listlength[maxk], pc[maxk];
double alpha, gamma, Wk, threshold;
double dis[maxn];
double W[maxn];
int E[maxn];
char termList[maxk][maxl];
FILE *fp1, *fp2;

struct BinaryHeap {
	struct heapNode {
		int id;
		double val;
		heapNode() : id(0), val(0) {};
		heapNode(int doc, double value) {
			id = doc;
			val = value;
		}
	} heap[maxK];
	int n;
	
	BinaryHeap() : n(0) {}
	
	void push(int doc, double value) {
		heap[++n] = heapNode(doc, value);
		up(n);
	}
	
	bool empty() {
		return n == 0;
	}
	
	heapNode top() {
		return heap[1];
	}
	
	void pop() {
		swap(heap[1], heap[n]);
		n--;
		if (n) down(1);
	}
	
	void up(int i) {
		double x = heap[i].val;
		int y = heap[i].id;
		for (int j = i/2; j >= 1; j /= 2) {
			if (heap[j].val > x) {
				heap[i] = heap[j];
				i = j;
			} else {
				break;
			}
		}
		heap[i].val = x;
		heap[i].id = y;
	}
	
	void down(int i) {
		double x = heap[i].val;
		int y = heap[i].id;
		for (int j = i*2; j <= n; j *= 2) {
			if (j < n && heap[j].val > heap[j+1].val) j++;
			if (heap[j].val < x) {
				heap[i] = heap[j];
				i = j;
			} else {
				break;
			}
		}
		heap[i].val = x;
		heap[i].id = y;
	}
} topkHeap;

struct point {
	int x, y;
} location[maxn], query;

struct listNode {
	int id;
	double score;
} idList[maxk][maxn], scoreList[maxk][maxn], ans[maxk];

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

bool cmp(listNode i, listNode j) {
	if (i.score == j.score) return i.id < j.id;
	else return i.score > j.score;
}

void makeSpaceList() {
	for (int i = 1; i <= n; i++) {
		scoreList[0][i].id = i;
		idList[0][i].score = scoreList[0][i].score = 1-dis[i]/gamma;
	}
	sort(scoreList[0]+1, scoreList[0]+n+1, cmp);
}

void initialize() {
	n = 30000;
	K = 20;
	totalTerm = 0;
	alpha = 0.4;
	Listlength[0] = n;
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
	
	fp2 = fopen(getname("inverted_list\\", word, "_text.txt"), "r");
	readint(fp2, Listlength[totalTerm]);
	for (int i = 1; i <= Listlength[totalTerm]; i++) {
		readint(fp2, scoreList[totalTerm][i].id);
		readdb(fp2, scoreList[totalTerm][i].score);
	}
	fclose(fp2);
	
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

double search_score(int k, int len, int doc) {
	int l = 1, r = len, mid;
	while (l <= r) {
		mid = (l+r)/2;
		if (doc == idList[k][mid].id) return idList[k][mid].score;
		if (doc < idList[k][mid].id) r = mid-1; else l = mid+1;
	}
	return -1;
}

void updateTopk(int doc) {
	if (W[doc] > Wk) {
		topkHeap.pop();
		topkHeap.push(doc, W[doc]);
		Wk = topkHeap.top().val;
	}
}

void seqAccess(int doc, int x, double score) {
	threshold += score;
	if (((1<<x) & E[doc]) == 0) {
		W[doc] += score;
		E[doc] |= 1<<x;
	}
}

void randomAccess(int doc) {
	if (((1<<(m+1)) & E[doc]) == 0) {
		for (int i = 1; i <= m; i++)
			if (((1<<i) & E[doc]) == 0) {
				double score = search_score(pc[i], Listlength[pc[i]], doc);
				if (score > 0) {
					W[doc] += alpha*score;
					E[doc] |= 1<<i;
				}
			}
		if ((1 & E[doc]) == 0) {
			W[doc] += (1-alpha)*(1-dis[doc]/gamma);
			E[doc] |= 1;
		}
		if (E[doc] == 1) W[doc] = 0;
		E[doc] |= 1<<(m+1);
		updateTopk(doc);
	}
}

void solve() {
	findMaxLocation();
	makeSpaceList();
	while (!topkHeap.empty()) topkHeap.pop();
	for (int i = 1; i <= K; i++) topkHeap.push(0, 0);
	for (int i = 1; i <= n; i++) {
		W[i] = 0;
		E[i] = 0;
	}
	Wk = 0;
	for (int i = 1; i <= n; i++) {
		threshold = 0;
		for (int j = 1; j <= m; j++)
			if (i <= Listlength[pc[j]])
				seqAccess(scoreList[pc[j]][i].id, j, alpha*scoreList[pc[j]][i].score);
		seqAccess(scoreList[0][i].id, 0, (1-alpha)*scoreList[0][i].score);
		
		for (int j = 0; j <= m; j++)
			if (i <= Listlength[pc[j]])
				randomAccess(scoreList[pc[j]][i].id);
		
		if (Wk >= threshold) break;
	}
	
	for (int i = K; i >= 1; i--) {
		ans[i].id = topkHeap.top().id;
		ans[i].score = topkHeap.top().val;
		topkHeap.pop();
	}
}

void outputdata(int queryID) {
	printf("Query %d\n", queryID);
	for (int i = 1; i <= K; i++)
		printf("id = %d   score = %lf\n", ans[i].id, ans[i].score);
	printf("\n");
}

int main() {
	freopen("answer_TA.txt", "w", stdout);
	
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
