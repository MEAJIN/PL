#include <stdio.h>
#define M_SIZE 1000
#define R_SIZE 10
//�޸�,�������� ũ�� ����

int M[M_SIZE]; 
int R[R_SIZE]; 
int opcode,opnd1,opnd2;
//�޸�,��������,�ǿ����� ���� ����

void init();	//�޸�,�������� �ʱ�ȭ
void input();	//�޸� ���Ͽ��� �о� ��
int interpreter();	//��ɾ� �ؼ�
void fetch();	//�޸𸮿��� �������ͷ� ��ɾ� ����
void decode();	//��ɾ� �ؼ�
void execute();	//��ɾ� ���� ����
void print(int counter);	//���

int main() {
	int counter = 0;

	init(M, R);
	input(M);	

	counter = interpreter(M, R);
	print(counter, M, R);

	return 0;
}

void init(int* M, int* R) { //�޸�,�������� �ʱ�ȭ
	for (int i = 0; i < M_SIZE; i++) {
		M[i] = 0;
	}
	for (int i = 0; i < R_SIZE; i++) {
		R[i] = 0;
	}
}
void input() { //�޸� ���Ͽ��� �о� ��
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

int interpreter() {	//��ɾ� �ؼ�
			R[1] = 0;
			int counter = 0;
			int halt = 100;

			do {
				fetch();
				decode();
				execute();
				counter++;

			} while (R[0] != halt); //halt(100)����� ���� ������
			return counter;
		}

void fetch() {			//�޸𸮿��� �������ͷ� ��ɾ� ����
	R[0] = M[R[1]];		//���α׷� ������� �ּҿ� �ش��ϴ� �޸��� ��ɾ �������Ϳ� �����ϰ� 
	R[1] = R[1] + 1;	//���α׷� ����� 1 ����
}	

void decode() { //��ɾ� �ؼ�
	opcode = R[0] / 100;
	opnd1 = (R[0] % 100) / 10;
	opnd2 = R[0] % 10;
//��ɾ� �ڵ�� �ǿ����� 1,2 �Ľ�

void execute() { //��ɾ� ���� ����, ��ⷯ ���� �̿�
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
void print(int counter) { //���
	printf("*****�޸� ���� ���*****\n");
	for (int i = 0; i < M_SIZE; i++) {
		if (M[i] == 0)
			continue;
		printf("\tM[%d] : %d\n",i, M[i]);
	}

	printf("*****�������� ���� ���*****\n");
	for (int i = 0; i < R_SIZE; i++) {
		printf("\tR[%d] : %d\n", i, R[i]);
	}
	printf("\n�� %d���� ��ɾ� ����\n", counter);
	printf("Memory[500] : %d\n", M[500]);
}