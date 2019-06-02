#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define QUEUESIZE 100

typedef struct process {
	int pid;	// 프로세스 id
	int arv;	// 도착시간
	int svc;	// 서비스 시간
	int level;	// 큐의 단계
	int runtime;	// 실행된 시간
	bool run;		// 실행여부
	bool ready;		// 준비큐 진입여부
	bool finish;		// 종료 여부
}Process;

void PrintProcess(Process p, int time);

int main(void) {
	FILE *fp;	// 파일 포인터
	Process pl[QUEUESIZE];	// 프로세스 리스트
	Process temp;	// 정렬을 위한 임시변수
	bool allFinish = false;
	bool run = false;	// 현재 실행중인 프로세스가 있는지의 여부
	bool onSingle = false;	// 하나의 프로세스만 준비큐에 있는지 검사
	int select = 0;			// 현재 실행중인 프로세스 구조체 인덱스 저장변수
	int time = 0;	// 시간 카운터
	int pRun = 1;	// 프로세스의 실행시간 초기화
	int pTime = 0;		// 스케줄링 후 프로세스가 실행된 시간 저장 변수
	int i = 0, j = 0;
	int pid, arv, svc;

	// 파일 포인터 지정
	fp = fopen("input.txt", "r");


	// 프로세스 정보 입력받음
	while (1) {
		
		fscanf(fp, "%d,%d,%d", &pid, &arv, &svc);
		// 프로세스 초기화
		pl[i].pid = pid;
		pl[i].arv = arv;
		pl[i].svc = svc;
		pl[i].runtime = 0;	// 실행된 시간 초기화
		pl[i].level = 0;	// 0단계 큐로 초기화
		pl[i].ready = false;	// 준비큐 진입여부 초기화
		pl[i].run = false;		// 실행여부 초기화
		pl[i].finish = false;	// 종료여부 초기화

		if (pid == 0)
			break;

		//printf("%d %d %d\n", pl[i].pid, pl[i].arv, pl[i].svc);
		i++;
	}
	// pid = -1 비어있는 상태
	for (; i < QUEUESIZE; i++) {
		pl[i].pid = -1;
	}

	// 프로세스의 도착시간을 기준으로 프로세스 배열을 오름차순으로 정렬
	for (i = 0; pl[i].pid != -1; i++) {
		for (j = i; pl[j].pid != -1; j++) {
			if (pl[i].arv > pl[j].arv) {
				temp = pl[i];
				pl[i] = pl[j];
				pl[j] = temp;
			}
		}
	}

	// 피드백 알고리즘으로 스케줄링 및 실행
	printf("Feedback Simulator\n\n");
	while (!allFinish) {
		int i = 0;
		int cnt = 0;	// 카운트 변수
		
		//printf("----------------%d--------------\n", time+1);

		// 1. 준비큐에 현재시간과 프로세스의 도착시간이 같은 프로세스를 삽입
		for (i = 0; pl[i].pid != -1; i++) {
			// 현재 시간과 프로세의 도착시간이 같을 경우
			if (pl[i].arv == time) {
				pl[i].ready = true;
				//printf("%d 준비큐 진입\n", pl[i].pid);
			}
		}

		// 실재로 실행된 시간과 큐의 레벨에 따른 실행시간이 같을 경우 스케줄링 재실행 및 레벨 증가
		if (pRun == pTime) {
			run = false;
			pTime = 0;
			pRun = 1;
			pl[select].run = false;
			// 레벨은 준비큐에 다른 프로세스가 없다면 1증가
			for (i = 0; pl[i].pid != -1; i++) {	// 준비큐에 존재하는 프로세스의 수 카운트
				if (pl[i].ready)
					cnt++;
			}
			// 준비큐에 존재하는 프로세스가 2개 이상이라면
			if (cnt > 1)
				(pl[select].level)++;
		}
		
		// 2. 준비큐에 들어간 프로세스 중 하나를 선택
		// 인덱스 초기화
		if (run == false) {
			for (i = 0; pl[i].pid != -1; i++) {
				if (pl[i].finish != true) {
					select = i;
					pTime = 0;		// 스케줄링 후 실행된 시간 초기화
					for (j = 0; j<pl[select].level; j++)
						pRun *= 2;
					break;
				}
			}
		}
		if (run == false) {	// 실행 중인 프로세스가 없다면
			for (i = 0; pl[i].pid != -1; i++) {
				if(pl[i].ready) {	// 준비큐에 들어있는지 확인
					if (pl[i].level == 0) {	// 최근에 들어온 프로세스 인지 확인
						select = i;		// 인덱스값 저장
						pTime = 0;		// 스케줄링 후 실행된 시간 초기화
						pRun = 1;		// 큐의 단계가 0 이므로 실행시간은 1로 초기화
						break;	// 최근에 들어온 프로세스 이므로 바로 스케줄링하여 실행
					} else {	// 최근에 들어온 프로세스가 없다면 레벨이 낮은 큐 중 가장 먼저 들어온 프로세스 선택
						if (pl[select].level > pl[i].level) {
							select = i;		// 인덱스값 저장
							pTime = 0;		// 스케줄링 후 실행된 시간 초기화
							pRun = 1;
							for (j=0; j<pl[select].level; j++)
								pRun *= 2;		// 큐의 단계가 0 이므로 실행시간은 1로 초기화
						}
					}
				}
			}
		}
		//printf("%d 선택됨\n", pl[select].pid);
		// 3. 선택된 프로세스를 실행
		run = true;		// 실행여부 true로 변경
		pl[select].run = true;	// 실행상태 true로 변경
		(pl[select].runtime)++;	// 실행시간 증가
		pTime++;	// 스케줄링 이후 실행시간 증가
		//printf("%d 실행됨 %d초\n", pl[select].pid, pl[select].runtime);
		// 프로세스가 모두 실행되었다면 종료상태로 변경
		if (pl[select].runtime == pl[select].svc) {
			pl[select].ready = false;	// 준비큐에서 해제
			pl[select].run = false;	// 실행여부 해제
			pl[select].finish = true;	// 종료여부 true로 변경
			run = false;	// 실행여부 false로 변경
			//printf("%d 종료\n", pl[select].pid);
			PrintProcess(pl[select], time+1);
		}
		time++;	// 시간 증가

		// 4. 모든 프로세스가 종료되었다면 프로그램 종료
		allFinish = true;
		for (i = 0; pl[i].pid != -1; i++) {
			if (pl[i].finish == false) {
				allFinish = false;
				continue;
			}
		}
	}
	
	// 파일 포인터 해제
	fclose(fp);
	getchar();

	return 0;
}

void PrintProcess(Process p, int time) {
	printf("프로세스 %d 종료\n", p.pid);
	printf("pid: %d\t도착 시간: %d\t서비스 시간: %d\t종료 시간: %d\t반환 시간: %d\t정규화된 반환 시간: %.2f\n\n"
		, p.pid, p.arv, p.svc, time, time - p.arv, (float)(((time - p.arv)-p.svc)+p.svc)/(float)(p.svc));
	// 정규화된 반환 시간 (대기 시간 + 실행 시간/실행시간)
}