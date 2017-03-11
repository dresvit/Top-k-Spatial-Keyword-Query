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

const int maxb = 1010;
const int maxk = 35;
const int maxl = 50;
const int maxn = 1000010;
const int maxK = 210;
const int maxco = 2999;

// 参数说明 
// n: 文档总数
// m: 当前查询的词项个数 
// K: top-K的K值 
// totalTerm: 词项总数
// termList: 词项列表，记录出现过的词项 
// Listlength: 每个倒排列表的长度 
// pc: pc[i]表示当前查询中第i个词项在总词项列表中的下标
// pt: 文本列表顺序访问的指针
// pf: 空间列表顺序访问时向后扫描的指针
// pb: 空间列表顺序访问时向前扫描的指针
// recList: 记录已经访问过的文档
// posHeap: posHeap[i]表示id为i的文档在堆中的下标
// Wk: 堆中的最小值
// dis: dis[i]表示id为i的文档与查询Q之间的距离
// W: 文档评分的下界值
// E: 记录每篇文档已经被访问过的属性
// buf: 临时存放顺序访问空间列表时距离大于当前半径的文档 
// topkHeap: 维护下界值topk大的二叉堆 
// idList: 按id排序的记录文本评分的倒排列表 
// textList: 按分数从大到小排序的记录文本评分的倒排列表
// spaceList: 按Z-order排序的记录文档id的倒排列表

int n, m, K;
int totalTerm, Listlength[maxk], pc[maxk];
int pt[maxk], pf[maxk], pb[maxk];
int recNum, recList[maxn];
int tmpNum, tmpList[maxK];
int posHeap[maxn];
double alpha, gamma, lambdas, etat, etas, Wk;
double dis[maxn];
double W[maxn];
int E[maxn];
char termList[maxk][maxl];
vector<int> buf[maxb];
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
		posHeap[doc] = n;
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
		posHeap[heap[1].id] = 1;
		posHeap[heap[n].id] = 0;
		n--;
		if (n) down(1);
	}
	
	void change(int doc, double value) {
		heap[posHeap[doc]].val = value;
		down(posHeap[doc]);
		up(posHeap[doc]);
	}
	
	void up(int i) {
		double x = heap[i].val;
		int y = heap[i].id;
		for (int j = i/2; j >= 1; j /= 2) {
			if (heap[j].val > x) {
				heap[i] = heap[j];
				posHeap[heap[i].id] = i;
				i = j;
			} else {
				break;
			}
		}
		heap[i].val = x;
		heap[i].id = y;
		posHeap[y] = i;
	}
	
	void down(int i) {
		double x = heap[i].val;
		int y = heap[i].id;
		for (int j = i*2; j <= n; j *= 2) {
			if (j < n && heap[j].val > heap[j+1].val) j++;
			if (heap[j].val < x) {
				heap[i] = heap[j];
				posHeap[heap[i].id] = i;
				i = j;
			} else {
				break;
			}
		}
		heap[i].val = x;
		heap[i].id = y;
		posHeap[y] = i;
	}
} topkHeap;

struct point {
	int x, y;
} location[maxn], query;

struct listNode {
	int id;
	double score;
} idList[maxk][maxn], textList[maxk][maxn], ans[maxk];

struct spaceListNode {
	int id, order;
} spaceList[maxk][maxn];

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
	etat = 20;
	alpha = 0.4;
	etas = etat*alpha/(1-alpha);
	for (int i = 0; i <= maxb-10; i++) buf[i].clear();
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
		readint(fp2, textList[totalTerm][i].id);
		readdb(fp2, textList[totalTerm][i].score);
	}
	fclose(fp2);
	
	fp2 = fopen(getname("inverted_list\\", word, "_id.txt"), "r");
	readint(fp2, Listlength[totalTerm]);
	for (int i = 1; i <= Listlength[totalTerm]; i++) {
		readint(fp2, idList[totalTerm][i].id);
		readdb(fp2, idList[totalTerm][i].score);
	}
	fclose(fp2);
	
	fp2 = fopen(getname("inverted_list\\", word, "_space.txt"), "r");
	readint(fp2, Listlength[totalTerm]);
	for (int i = 1; i <= Listlength[totalTerm]; i++) {
		readint(fp2, spaceList[totalTerm][i].id);
		readint(fp2, spaceList[totalTerm][i].order);
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

int getZorder(int i, int j) {
	if (i < 0) i = 0;
	if (j < 0) j = 0;
	if (i > maxco) i = maxco;
	if (j > maxco) j = maxco;
	int x, y;
	x=0;
	for (int k = 0; k <= 15; k++)
		if (i & (1<<k)) x|=1<<(2*k+1);
	y=0;
	for (int k = 0; k <= 15; k++)
		if (j & (1<<k)) y|=1<<(2*k);
	return x|y;
}

int search_pf(int k, int len, int zq) {
	int l = 1, r = len, mid;
	while (l <= r) {
		mid = (l+r)/2;
		if (zq <= spaceList[k][mid].order) r = mid-1; else l = mid+1;
	}
	return r+1;
}

double search_score(int k, int len, int doc) {
	int l = 1, r = len, mid;
	while (l <= r) {
		mid = (l+r)/2;
		if (doc == idList[k][mid].id) return idList[k][mid].score;
		if (doc < idList[k][mid].id) r = mid-1; else l = mid+1;
	}
	return 0;
}

void updateTopk(int doc) {
	if (W[doc] > Wk) {
		if (posHeap[doc]) topkHeap.change(doc, W[doc]);
		else {
			topkHeap.pop();
			topkHeap.push(doc, W[doc]);
		}
		Wk = topkHeap.top().val;
	}
}

void seqAccess(int doc, int x, double score) {
	if (((1<<x) & E[doc]) == 0) {
		if (E[doc] == 0) recList[++recNum] = doc;
		W[doc] += score;
		E[doc] |= 1<<x;
		updateTopk(doc);
	}
}

void randomAccess(int doc) {
	if (E[doc] < (1<<(m+1))-1) {
		for (int i = 1; i <= m; i++)
			if (((1<<i) & E[doc]) == 0)
				W[doc] += alpha*search_score(pc[i], Listlength[pc[i]], doc);
		if ((1 & E[doc]) == 0)
			W[doc] += (1-alpha)*(1-dis[doc]/gamma);
		E[doc] = (1<<(m+1))-1;
		updateTopk(doc);
	}
}

void exploreTextList(double minT) {
	for (int i = 1; i <= m; i++)
		while (pt[i] <= Listlength[pc[i]] && textList[pc[i]][pt[i]].score > minT) {
			int doc = textList[pc[i]][pt[i]].id;
			seqAccess(doc, i, alpha*textList[pc[i]][pt[i]].score);
			pt[i]++;
		}
}

void exploreForwardSpatialList(int maxZ, double radius) {
	for (int i = 1; i <= m; i++)
		while (pf[i] <= Listlength[pc[i]] && spaceList[pc[i]][pf[i]].order <= maxZ) {
			int doc = spaceList[pc[i]][pf[i]].id;
			if (dis[doc] <= radius)
				seqAccess(doc, 0, (1-alpha)*(1-dis[doc]/gamma));
			else
				buf[(int)(dis[doc]/lambdas)].push_back(doc);
			pf[i]++;
		}
}

void exploreBackwardSpatialList(int minZ, double radius) {
	for (int i = 1; i <= m; i++)
		while (pb[i] >= 1 && spaceList[pc[i]][pb[i]].order >= minZ) {
			int doc = spaceList[pc[i]][pb[i]].id;
			if (dis[doc] <= radius)
				seqAccess(doc, 0, (1-alpha)*(1-dis[doc]/gamma));
			else
				buf[(int)(dis[doc]/lambdas)].push_back(doc);
			pb[i]--;
		}
}

bool viable(int doc) {
	double B = W[doc];
	if (E[doc] < (1<<(m+1))-1) {
		for (int i = 1; i <= m; i++)
			if (((1<<i) & E[doc]) == 0) {
				if (pt[i] == 1) return true;
				else B += alpha*textList[pc[i]][pt[i]-1].score;
			}
		if ((1 & E[doc]) == 0)
			B += (1-alpha)*(1-dis[doc]/gamma);
	}
	return B > Wk;
}

void solve() {
	int zmin, zmax;
	double Bk, Bt, Bs, ed, radius;
	findMaxLocation();
	lambdas = gamma/etas;
	recNum = 0;
	while (!topkHeap.empty()) topkHeap.pop();
	for (int i = 1; i <= K; i++) topkHeap.push(0, 0);
	for (int i = 1; i <= n; i++) {
		W[i] = 0;
		E[i] = 0;
	}
	Wk = 0;
	Bk = 1;
	for (int i = 1; i <= m; i++) {
		pt[i] = 1;
		pf[i] = search_pf(pc[i], Listlength[pc[i]], getZorder(query.y, query.x));
		pb[i] = pf[i]-1;
	}
	ed = (int)ceil(max(etat, etas));
	for (int i = 1; i <= ed; i++) {
		Bt = max(0.0, 1-i/etat);
		Bs = max(0.0, 1-i/etas);
		
		exploreTextList(Bt);
		
		radius = i*lambdas;
		zmin = getZorder((int)(query.y-radius), (int)(query.x-radius));
		zmax = getZorder((int)(query.y+radius), (int)(query.x+radius));
		exploreForwardSpatialList(zmax, radius);
		exploreBackwardSpatialList(zmin, radius);
		
		int size = buf[i].size();
		for (int j = 0; j < size; j++)
			seqAccess(buf[i][j], 0, (1-alpha)*(1-dis[buf[i][j]]/gamma));
		buf[i].clear();
		
		tmpNum = 0;
		for (int j = 1; j <= K; j++)
			if (topkHeap.heap[j].id) tmpList[++tmpNum] = topkHeap.heap[j].id;
		for (int j = 1; j <= tmpNum; j++)
			randomAccess(tmpList[j]);
		
		Bk = alpha*m*Bt+(1-alpha)*Bs;
		if (Bk <= Wk) {
			for (int j = 1; j <= recNum; j++) {
				int doc = recList[j];
				if (posHeap[doc] == 0 && viable(doc))
					randomAccess(doc);
			}
			break;
		}
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
	freopen("answer_RCA.txt", "w", stdout);
	
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
