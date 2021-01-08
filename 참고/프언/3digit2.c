#include <stdio.h>
#define M_SIZE 1000
#define R_SIZE 10
//메모리,레지스터 크기 정의

int M[M_SIZE]; 
int R[R_SIZE]; 
int opcode,opnd1,opnd2;
//메모리,레지스터,피연산자 전역 변수

void init();	//메모리,레지스터 초기화
void input();	//메모리 파일에서 읽어 옴
int interpreter();	//명령어 해석
void fetch();	//메모리에서 레지스터로 명령어 인출
void decode();	//명령어 해석
void execute();	//명령어 연산 실행
void print(int counter);	//출력

int main() {
	int counter = 0;

	init(M, R);
	input(M);	

	counter = interpreter(M, R);
	print(counter, M, R);

	return 0;
}

void init(int* M, int* R) { //메모리,레지스터 초기화
	for (int i = 0; i < M_SIZE; i++) {
		M[i] = 0;
	}
	for (int i = 0; i < R_SIZE; i++) {
		R[i] = 0;
	}
}
void input() { //메모리 파일에서 읽어 옴
	FILE* mp = fopen("oddplus.txt", "r");

	if (mp == NULL) {
		exit();
	}

	int i = 0;
	while (feof(mp) == 0) {
		fscanf(mp, "%d\n", &M[i++]);
	}

	fclose(mp);
}

int interpreter() {	//명령어 해석
			R[1] = 0;
			int counter = 0;
			int halt = 100;

			do {
				fetch();
				decode();
				execute();
				counter++;

			} while (R[0] != halt); //halt(100)명령을 받을 때까지
			return counter;
		}

void fetch() {			//메모리에서 레지스터로 명령어 인출
	R[0] = M[R[1]];		//프로그램 계수기의 주소에 해당하는 메모리의 명령어를 레지스터에 적재하고 
	R[1] = R[1] + 1;	//프로그램 계수기 1 증가
}	

void decode() { //명령어 해석
	opcode = R[0] / 100;
	opnd1 = (R[0] % 100) / 10;
	opnd2 = R[0] % 10;
//명령어 코드와 피연산자 1,2 파싱

void execute() { //명령어 연산 실행, 모듈러 연산 이용
	switch (opcode) {
	case 2: //Rd<-n
		R[opnd1] = opnd2;
		R[opnd1] = R[opnd1] % 1000;
		break;
	case 3: //Rd <-Rd+n
		R[opnd1] = R[opnd1] + opnd2;
		R[opnd1] = R[opnd1] % 1000;
		break;

	case 4: //Rd <-Rd*n
		R[opnd1] = R[opnd1] * opnd2;
		R[opnd1] = R[opnd1] % 1000;
		break;

	case 5: //Rd <-Rs
		R[opnd1] = R[opnd2];
		R[opnd1] = R[opnd1] % 1000;
		break;
	case 6: //Rd <-Rd+Rs
		R[opnd1] = R[opnd1] + R[opnd2];
		R[opnd1] = R[opnd1] % 1000;
		break;
	case 7: //Rd <-Rd*Rs
		R[opnd1] = R[opnd1] * R[opnd2];
		R[opnd1] = R[opnd1] % 1000;
		break;
	case 8: //Rd <-[Ra]
		R[opnd1] = M[R[opnd2]];
		break;
	case 9: //[Ra]<-Rs
		M[R[opnd2]] = R[opnd2];
		R[opnd1] = R[opnd1] % 1000;
		break;
	case 0: //if Rs != 0 then goto [Rd]
		if (R[opnd2] != 0) {
			R[1] = R[opnd1];
		}
		break;
	}
}
void print(int counter) { //출력
	printf("*****메모리 파일 출력*****\n");
	for (int i = 0; i < M_SIZE; i++) {
		if (M[i] == 0)
			continue;
		printf("\tM[%d] : %d\n",i, M[i]);
	}

	printf("*****레지스터 파일 출력*****\n");
	for (int i = 0; i < R_SIZE; i++) {
		printf("\tR[%d] : %d\n", i, R[i]);
	}
	printf("\n총 %d개의 명령어 실행\n", counter);
	printf("Memory[500] : %d\n", M[500]);
}