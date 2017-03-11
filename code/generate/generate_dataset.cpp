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
const int maxm = 1010;
const int maxl = 50;
const int maxco = 2999;

//总共的词项种类，从中随机选择词项 
const char *word[] = {"apple", "banana", "pear", "grape", "watermelon",
					"orange", "peach", "strawberry", "mango", "lemon",
					"coconut", "apricot", "plum", "blueberry", "cherry",
					"cucumber", "tomato", "potato", "carrot", "cabbage",
					"onion", "pumpkin", "mushroom", "pepper", "bean"};


// num: 每个词项的个数，在按大比例的基础上随机分配 
// p: 词项的对应num数组的一个排列 
// f: 判断该词项是否已使用
// out: 存打乱后的单词的下标，用于输出
// s: 存放单词 

int num[maxn], p[maxn], out[maxm];
bool f[maxn];
char s[maxm][maxl];

void solve() {
	int n, m, x, y, now, len, res, st, ed, op, tot;
	n = 30000;
	for (int i = 1; i <= n; i++) {
		x = (double)rand()/RAND_MAX*maxco;
		y = (double)rand()/RAND_MAX*maxco;
		printf("%d\n%d %d\n", i, x, y);
		
		m = rand()%101+150;
	//	printf("%d\n", m);
		
		memset(f, false, sizeof(f));
		for (int j = 1; j <= 5; j++) {
			now = rand()%25;
			while (f[now]) now = rand()%25;
			f[now] = true;
			p[j] = now;
		}
		len = m/15;
		res = m;
		for (int j = 1; j <= 4; j++) {
			now = rand()%len+1+(j-1)*len;
			num[j] = now;
			res -= now;
		}
		num[5] = res;
	/*	for (int j = 1; j <= 5; j++) printf("%d ", p[j]);
		printf("\n");
		for (int j = 1; j <= 5; j++) printf("%d ", num[j]);
		printf("\n");*/
		
		tot = 0;
		for (int j = 1; j <= 5; j++) {
			for (int k = 1; k <= num[j]; k++) {
				tot++;
				strcpy(s[tot], word[p[j]]);
			}
		}
		
	//	printf("%d\n", tot);
		
		for (int j = 1; j <= m; j++) out[j] = j;
		for (int j = 1; j <= m; j++) {          //洗牌算法，用于打乱输出的单词 
			now = rand()%(m-j+1)+j;
			swap(out[j], out[now]);
		}
		
		for (int j = 1; j <= m; j++) {
			printf("%s", s[out[j]]);
			if (j == m) printf("\n"); else printf(" ");
		}
	}
}

int main() {
	freopen("dataset.txt", "w", stdout);
	
	srand((unsigned)time(NULL));
	solve();
	return 0;
}
