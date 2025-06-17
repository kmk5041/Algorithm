#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "node.h"
#include "heap.h"

// 허프만 트리를 순회하며 허프만 코드를 생성하여 codes에 저장
// leaf 노드에서만 코드를 생성
// strdup 함수 사용
// make_huffman_code 함수에서 호출
static void traverse_tree(tNode* root, char* code, int depth, char* codes[]);

// 새로운 노드를 생성
// 좌/우 subtree가 NULL(0)이고 문자(data)와 빈도값(freq)이 저장됨
// make_huffman_tree 함수에서 호출
// return value : 노드의 포인터
static tNode* newNode(unsigned char data, int freq);

////////////////////////////////////////////////////////////////////////////////
// 허프만 코드를 화면에 출력
void print_huffman_code( char *codes[])
{
	int i;
	
	for (i = 0; i < 256; i++)
	{
		printf( "%d\t%s\n", i, codes[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////
// 허프만 트리로부터 허프만 코드를 생성
// traverse_tree 함수 호출
void make_huffman_code( tNode *root, char *codes[])
{
	char code[256];
	
	traverse_tree( root, code, 0, codes);
}

/////////////////////////////////////////////////////////////////////////////////

// 허프만 트리를 순회하며 허프만 코드를 생성하여 codes에 저장
// leaf 노드에서만 코드를 생성
// strdup 함수 사용
// make_huffman_code 함수에서 호출
static void traverse_tree(tNode* root, char* code, int depth, char* codes[])
{
	if (root->right || root->left) {
		if (root->left) {
			code[depth] = '0';
			traverse_tree(root->left, code, depth + 1, codes);
		}

		if (root->right) {
			code[depth] = '1';
			traverse_tree(root->right, code, depth + 1, codes);
		}
	}

	else {
		code[depth] = '\0';
		codes[root->data] = strdup(code);
	}

}

// 새로운 노드를 생성
// 좌/우 subtree가 NULL(0)이고 문자(data)와 빈도값(freq)이 저장됨
// make_huffman_tree 함수에서 호출
// return value : 노드의 포인터
static tNode* newNode(unsigned char data, int freq) {
	tNode* t = malloc(sizeof(tNode));
	t->data = data;
	t->freq = freq;
	t->left = 0;
	t->right = 0;
	return t;
}

// 파일에 속한 각 문자(바이트)의 빈도 저장
// return value : 파일에서 읽은 바이트 수
int read_chars(FILE* fp, int ch_freq[]) {
	int ch, bt = 0;

	do{
		ch = fgetc(fp);
		ch_freq[ch]++;
		bt++;
	} while(ch != EOF); 

	return bt - 1;
}

// 파일로부터 문자별 빈도(256개)를 읽어서 ch_freq에 저장
void get_char_freq(FILE* fp, int ch_freq[]) {
	for (int i = 0; i < 256; i++)   fread(ch_freq+i, sizeof(int), 1, fp);
}

// 허프만 코드에 대한 메모리 해제
void free_huffman_code(char* codes[]) {
	for (int i = 0; i < 256; i++) {
		free(codes[i]);
	}
}

// 허프만 트리를 생성
// 1. capacity 256 짜리 빈(empty) 힙 생성 // HEAP *heap = heapCreate( 256);
// 2. 개별 알파벳에 대한 노드 생성
// 3. 힙에 삽입 (minheap 구성) // heapInsert( heap, node);
// 4. 2개의 최소값을 갖는 트리 추출 // tNode *left = heapDelete( heap); tNode *right = heapDelete( heap);
// 5. 두 트리를 결합 후 새 노드에 추가
// 6. 새 트리를 힙에 삽입
// 7. 힙에 한개의 노드가 남을 때까지 반복
// return value: 트리의 root 노드의 포인터
tNode* make_huffman_tree(int ch_freq[]) {
	HEAP* huffman = heapCreate(256);
	
	tNode* nn;
	tNode* Node;
	for (int i = 0; i < 256; i++) {
		Node=newNode(i, ch_freq[i]);
		heapInsert(huffman,Node);
	}
	while (huffman->last != 0) {
		tNode* ll = heapDelete(huffman);
		tNode* rr = heapDelete(huffman);
		nn = newNode(-1, ll->freq + rr->freq);
		nn->left = ll;
		nn->right = rr;
		heapInsert(huffman, nn);
	}

	heapDestroy(huffman);
	return nn;

}

// 허프만 트리 메모리 해제
void destroyTree(tNode* root) {
	if (root->left || root->right) {
		if (root->left) {
			destroyTree(root->left);
		}
		if (root->right) {
			destroyTree(root->right);
		}
		free(root);
	}
	else
		free(root);
}

// 입력 텍스트 파일(infp)을 허프만 코드를 이용하여 출력 파일(outfp)로 인코딩
// return value : 인코딩된 텍스트의 바이트 수 (파일 크기와는 다름)
int encoding(char* codes[], int ch_freq[], FILE* infp, FILE* outfp) {

	for (int i = 0; i < 256; i++) fwrite(ch_freq+ i, sizeof(int), 1, outfp);

	int nbits = 0;
	int nbytes = 0;
	int number = 0;
	char buffer;
	char* s;
	int ch;

	for (int i = 0; i < 256; i++) {
		number += strlen(codes[i]) * ch_freq[i];
	}

	while (1) {
		ch = fgetc(infp);
		if (ch == EOF) break;
		for (s = codes[ch]; *s; s++) {
			buffer = buffer << 1;
			if (*s == '1') {
				buffer = buffer | 1;
			}
			nbits++;
			if (nbits == 8) {
				fwrite(&buffer, sizeof(char), 1, outfp);
				buffer = 0;
				nbits = 0;
				nbytes++;
			}
		}
	}

		while (nbits) {
			buffer = buffer << 1;
			nbits++;

			if (nbits = 8) {
				fwrite(&buffer, sizeof(char), 1, outfp);
				nbytes++;
				nbits = 0;
				buffer = 0;
			}

		}

		fwrite(&number, sizeof(int), 1, outfp);
		return nbytes;
}

// 입력 파일(infp)을 허프만 트리를 이용하여 텍스트 파일(outfp)로 디코딩
void decoding(tNode* root, FILE* infp, FILE* outfp) {
	
	int nbits = 0;
	tNode* cur =root;

	fseek(infp, -sizeof(int), SEEK_END);
	fread(&nbits, sizeof(int), 1, infp);

	fseek(infp, 256 * sizeof(int), SEEK_SET);
	char strstr[100];

	while (1) {

		int end = fread(strstr, 1, 100, infp);
		if (end == 0) {
			break;
		}
		else {

			for (int i = 0; i < end; i++) {
				for (int j = 0; j < 8; j++) {
					if ((char)(strstr[i] & 0x80) == 0) {

						cur = cur->left;
					}

					else
						cur = cur->right;

					strstr[i] = strstr[i] << 1;
					nbits--;

					if ((cur->left == 0) && (cur->right == 0)) {

						fprintf(outfp, "%c", cur->data);
						
						fflush(outfp);
						cur = root;
					}
					if (nbits == 0)
						return;
				}
			}
		}
	}

}
