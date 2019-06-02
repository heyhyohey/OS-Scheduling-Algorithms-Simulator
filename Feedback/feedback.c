#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define QUEUESIZE 100

typedef struct process {
	int pid;	// ���μ��� id
	int arv;	// �����ð�
	int svc;	// ���� �ð�
	int level;	// ť�� �ܰ�
	int runtime;	// ����� �ð�
	bool run;		// ���࿩��
	bool ready;		// �غ�ť ���Կ���
	bool finish;		// ���� ����
}Process;

void PrintProcess(Process p, int time);

int main(void) {
	FILE *fp;	// ���� ������
	Process pl[QUEUESIZE];	// ���μ��� ����Ʈ
	Process temp;	// ������ ���� �ӽú���
	bool allFinish = false;
	bool run = false;	// ���� �������� ���μ����� �ִ����� ����
	bool onSingle = false;	// �ϳ��� ���μ����� �غ�ť�� �ִ��� �˻�
	int select = 0;			// ���� �������� ���μ��� ����ü �ε��� ���庯��
	int time = 0;	// �ð� ī����
	int pRun = 1;	// ���μ����� ����ð� �ʱ�ȭ
	int pTime = 0;		// �����ٸ� �� ���μ����� ����� �ð� ���� ����
	int i = 0, j = 0;
	int pid, arv, svc;

	// ���� ������ ����
	fp = fopen("input.txt", "r");


	// ���μ��� ���� �Է¹���
	while (1) {
		
		fscanf(fp, "%d,%d,%d", &pid, &arv, &svc);
		// ���μ��� �ʱ�ȭ
		pl[i].pid = pid;
		pl[i].arv = arv;
		pl[i].svc = svc;
		pl[i].runtime = 0;	// ����� �ð� �ʱ�ȭ
		pl[i].level = 0;	// 0�ܰ� ť�� �ʱ�ȭ
		pl[i].ready = false;	// �غ�ť ���Կ��� �ʱ�ȭ
		pl[i].run = false;		// ���࿩�� �ʱ�ȭ
		pl[i].finish = false;	// ���Ῡ�� �ʱ�ȭ

		if (pid == 0)
			break;

		//printf("%d %d %d\n", pl[i].pid, pl[i].arv, pl[i].svc);
		i++;
	}
	// pid = -1 ����ִ� ����
	for (; i < QUEUESIZE; i++) {
		pl[i].pid = -1;
	}

	// ���μ����� �����ð��� �������� ���μ��� �迭�� ������������ ����
	for (i = 0; pl[i].pid != -1; i++) {
		for (j = i; pl[j].pid != -1; j++) {
			if (pl[i].arv > pl[j].arv) {
				temp = pl[i];
				pl[i] = pl[j];
				pl[j] = temp;
			}
		}
	}

	// �ǵ�� �˰������� �����ٸ� �� ����
	printf("Feedback Simulator\n\n");
	while (!allFinish) {
		int i = 0;
		int cnt = 0;	// ī��Ʈ ����
		
		//printf("----------------%d--------------\n", time+1);

		// 1. �غ�ť�� ����ð��� ���μ����� �����ð��� ���� ���μ����� ����
		for (i = 0; pl[i].pid != -1; i++) {
			// ���� �ð��� ���μ��� �����ð��� ���� ���
			if (pl[i].arv == time) {
				pl[i].ready = true;
				//printf("%d �غ�ť ����\n", pl[i].pid);
			}
		}

		// ����� ����� �ð��� ť�� ������ ���� ����ð��� ���� ��� �����ٸ� ����� �� ���� ����
		if (pRun == pTime) {
			run = false;
			pTime = 0;
			pRun = 1;
			pl[select].run = false;
			// ������ �غ�ť�� �ٸ� ���μ����� ���ٸ� 1����
			for (i = 0; pl[i].pid != -1; i++) {	// �غ�ť�� �����ϴ� ���μ����� �� ī��Ʈ
				if (pl[i].ready)
					cnt++;
			}
			// �غ�ť�� �����ϴ� ���μ����� 2�� �̻��̶��
			if (cnt > 1)
				(pl[select].level)++;
		}
		
		// 2. �غ�ť�� �� ���μ��� �� �ϳ��� ����
		// �ε��� �ʱ�ȭ
		if (run == false) {
			for (i = 0; pl[i].pid != -1; i++) {
				if (pl[i].finish != true) {
					select = i;
					pTime = 0;		// �����ٸ� �� ����� �ð� �ʱ�ȭ
					for (j = 0; j<pl[select].level; j++)
						pRun *= 2;
					break;
				}
			}
		}
		if (run == false) {	// ���� ���� ���μ����� ���ٸ�
			for (i = 0; pl[i].pid != -1; i++) {
				if(pl[i].ready) {	// �غ�ť�� ����ִ��� Ȯ��
					if (pl[i].level == 0) {	// �ֱٿ� ���� ���μ��� ���� Ȯ��
						select = i;		// �ε����� ����
						pTime = 0;		// �����ٸ� �� ����� �ð� �ʱ�ȭ
						pRun = 1;		// ť�� �ܰ谡 0 �̹Ƿ� ����ð��� 1�� �ʱ�ȭ
						break;	// �ֱٿ� ���� ���μ��� �̹Ƿ� �ٷ� �����ٸ��Ͽ� ����
					} else {	// �ֱٿ� ���� ���μ����� ���ٸ� ������ ���� ť �� ���� ���� ���� ���μ��� ����
						if (pl[select].level > pl[i].level) {
							select = i;		// �ε����� ����
							pTime = 0;		// �����ٸ� �� ����� �ð� �ʱ�ȭ
							pRun = 1;
							for (j=0; j<pl[select].level; j++)
								pRun *= 2;		// ť�� �ܰ谡 0 �̹Ƿ� ����ð��� 1�� �ʱ�ȭ
						}
					}
				}
			}
		}
		//printf("%d ���õ�\n", pl[select].pid);
		// 3. ���õ� ���μ����� ����
		run = true;		// ���࿩�� true�� ����
		pl[select].run = true;	// ������� true�� ����
		(pl[select].runtime)++;	// ����ð� ����
		pTime++;	// �����ٸ� ���� ����ð� ����
		//printf("%d ����� %d��\n", pl[select].pid, pl[select].runtime);
		// ���μ����� ��� ����Ǿ��ٸ� ������·� ����
		if (pl[select].runtime == pl[select].svc) {
			pl[select].ready = false;	// �غ�ť���� ����
			pl[select].run = false;	// ���࿩�� ����
			pl[select].finish = true;	// ���Ῡ�� true�� ����
			run = false;	// ���࿩�� false�� ����
			//printf("%d ����\n", pl[select].pid);
			PrintProcess(pl[select], time+1);
		}
		time++;	// �ð� ����

		// 4. ��� ���μ����� ����Ǿ��ٸ� ���α׷� ����
		allFinish = true;
		for (i = 0; pl[i].pid != -1; i++) {
			if (pl[i].finish == false) {
				allFinish = false;
				continue;
			}
		}
	}
	
	// ���� ������ ����
	fclose(fp);
	getchar();

	return 0;
}

void PrintProcess(Process p, int time) {
	printf("���μ��� %d ����\n", p.pid);
	printf("pid: %d\t���� �ð�: %d\t���� �ð�: %d\t���� �ð�: %d\t��ȯ �ð�: %d\t����ȭ�� ��ȯ �ð�: %.2f\n\n"
		, p.pid, p.arv, p.svc, time, time - p.arv, (float)(((time - p.arv)-p.svc)+p.svc)/(float)(p.svc));
	// ����ȭ�� ��ȯ �ð� (��� �ð� + ���� �ð�/����ð�)
}