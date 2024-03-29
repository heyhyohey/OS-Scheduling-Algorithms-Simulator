// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// hrrn_simulator.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

// TODO: 필요한 추가 헤더는
// 이 파일이 아닌 STDAFX.H에서 참조합니다.


// 파일 읽기 함수
FILE* ReadProcessData(char* fn) {
	FILE *fp;	// 파일 포인터 선언
	errno_t err;	// 에러 정보 저장 변수 선언
	err = fopen_s(&fp, fn, "r");	// 파일을 읽음

	if (err) {	// 에러가 있다면 프로그램 종료
		fputs("File open fault", stdout);
		fputc('\n', stdout);
		exit(1);
	}
	else {
		return fp;	// 에러가 없다면 파일 포인터를 리턴하고 종료
	}	
}

// 파일에서 프로세스 정보를 읽어오는 함수
Process* GetProcessList(FILE* fp) {
	int pid = EMPTY, t, s;
	Process* pl = NULL;
	
	while (pid) {	// pid의 값이 0이 되면 파일 입력 종료
		fscanf_s(fp, "%d,%d,%d\n", &pid, &t, &s);
		if (!pid)	// pid의 값이 0이면 아래의 작업을 수행하지 않음
			continue;

		pl = InitProcess(pl, pid, t, s);	// 프로세스 생성
	}

	return pl;
}


// 프로세스 생성 및 초기화
Process* InitProcess(Process* pl, int pid, int t, int s) {
	Process *cp;	// 생성된 프로세스
	Process *temp = pl;	// 프로세스 검색을 위한 임시 프로세스

	cp = (Process*)malloc(sizeof(Process));	// 프로세스 동적할당

	cp->pid = pid;	// 프로세스 id 초기화
	cp->t = t;		// 프로세스 도착시간 초기화
	cp->s = s;		// 프로세스 실행시간 초기화
	cp->w = 0;		// 프로세스 대기시간 초기화
	cp->et = 0;		// 프로세스가 실행된 시간 초기화
	cp->ft = 0;		// 프로세스가 종료된 시간 초기화
	cp->r = 0;	// R값 초기화
	cp->onExecute = false;	// 프로세스 실행여부 초기화
	cp->onReady = false;	// 준비큐 진입여부 초기화
	cp->onFinished = false;	// 프로세스 종료여부 초기화

	if (pl == NULL) {
		pl = cp;		// 현재 프로세스를 첫번재로 등록
		pl->np = NULL;	// 다음 프로세스 초기화
	} else {
		while (temp->np != NULL) {	// 마지막 프로세스를 찾음
			temp = temp->np;
		}
		cp->np = NULL;	// 프로세스 초기화
		temp->np = cp;	// 다음 프로세스 초기화
	}
	return pl;
}

// 프로세스를 HRRN스케줄링 방식으로 실행
void RunProcessByHRRN(Process* pl) {
	int time;	// 시간 변수
	bool isEmpty = false;	// 실행될 프로세스가 없는지의 정보
	bool isRunning = false;	// 비선점으로 실행중인지의 정보
	Process *sp = NULL;	// 선택된 프로세스

	time = 0;	// 시간 초기화
	printf("HRRN Simulator\n\n");
	while (!isEmpty) {	// 실행될 프로세스가 존재할 경우에만 실행
#ifdef DEBUG
		printf("%d\n---------------\n", time);
#endif
		// 1. 준비큐에 프로세스를 넣는 모듈
		ProcessSelecter(pl, &time);
		// 2. 비선점으로 실행중인 프로세스가 없다면 프로세스 선택
		if(!isRunning)
			sp = ProcessScheduler(pl, &isRunning);
		// 3. 선택된 프로세스를 실행
		ProcessDispatcher(pl, sp, &time, &isRunning);
		// 4. 프로세스가 모두 종료되었는지 검사
		isEmpty = checkFinished(pl);
	}
}


// 프로세스를 준비큐에 넣는 함수
void ProcessSelecter(Process* pl, int* time) {
	Process *temp = pl;

	// 현재 시간과 프로세스의 도착시간이 같다면 준비큐에 넣음
	while (temp != NULL) {
		if(!temp->onFinished){	// 종료된 프로세스인지 검사
			if (temp->t == *time) {
				temp->onReady = true;	// 준비큐 진입여부를 true로 설정
#ifdef DEBUG
				printf("진입 pid:%d\n", temp->pid);
#endif
			}
		}
		temp = temp->np;
	}
}

// 프로세스 스케줄링 함수
Process* ProcessScheduler(Process* pl, bool* isRunning) {
	Process *temp = pl;
	Process *sp = NULL;	// 실행될 프로세스
	float rMax = 0;	// 가장 큰 r값을 저장하는 변수

	// 준비큐에 진입한 프로세스의 r값을 구함
	while (temp != NULL) {
		if (temp->onReady)	// 준비큐에 진입했을 경우
			temp->r = ((float)(temp->w) + (float)(temp->s)) / (float)(temp->s);	// r = 대기시간 + 실행시간 / 실행시간
		temp = temp->np;
	}
	temp = pl;
	// 준비큐에 진입한 프로세스 중 r값이 가장 큰 프로세스를 실행상태로 바꿈
	while (temp != NULL) {
		if (!temp->onFinished) {	// 종료시 실행하지 않음
			if (temp->onReady)	// 준비큐에 진입했을 경우
				if (temp->r > rMax) {	// r값이 더 큰경우
					rMax = temp->r;	// 가장 큰 r값 대입
					sp = temp;	// 현재의 프로세스를 선택
#ifdef DEBUG
					printf("r값 pid:%d %.2f\n", sp->pid, sp->r);
#endif
				}
		}
		temp = temp->np;
	}
	if(sp!=NULL) {	// 선택된 프로세스가 없다면 예외처리
		sp->onExecute = true;	// 선택된 프로세스의 상태를 실행상태로 바꿈
		*isRunning = true;		// 비선점 실행상태로 바꿈
#ifdef DEBUG
	printf("선택됨 pid:%d r:%.2f\n", sp->pid, sp->r);
#endif
	}
	return sp;	// 선택된 프로세스 리턴
}

// 프로세스 실행 함수
void ProcessDispatcher(Process* pl, Process* sp, int* time, bool* isRunning) {
	Process *temp = pl;

	(*time)++;	// 시간 증가
	if(sp!=NULL) {	// 선택된 프로세서가 있을 경우 아래 실행
		// 준비큐에 존재하지만 실행되지 않은 프로세스들의 대기시간 증가
		while (temp != NULL) {
			if(!temp->onFinished) {	// 종료된 프로세스인지 검사
				if (!temp->onExecute) {	// 실행중인지 검사
					if (temp->onReady) {	// 준비큐에 있는지 검사
						(temp->w)++;		// 대기시간 1증가
#ifdef DEBUG
						printf("대기시간 증가 pid:%d %d\n", temp->pid, temp->w);
#endif
					}
				}
			}
			temp = temp->np;
		}

		// 선택된 프로세스 실행
		(sp->et)++;	// 실행시간 1증가
#ifdef DEBUG
		printf("실행 pid:%d %d\n", sp->pid, sp->ed);
#endif

		// 선택된 프로세스가 모두 완료되었는지 확인
		if (sp->s == sp->et) {	// 실행시간과 실재로 실행된 시간 비교
			sp->onReady = false;	// 준비큐에서 해제
			sp->onExecute = false;	// 실행상태 해제
			sp->onFinished = true;	// 종료상태 활성화
			sp->ft = *time;			// 프로세스의 종료된 시간을 현재 시간으로 등록
			*isRunning = false;	// 비선점 실행상태 해제
			printf("프로세스%d 종료\n", sp->pid);
			PrintProcessStatus(sp);	// 종료된 프로세스의 상태 출력
#ifdef DEBUG
			printf("종료 pid:%d\n", sp->pid);
#endif
		}
	}
	else {
#ifdef DEBUG
		printf("선택된 프로세서 없음\n");
#endif
	}
}

// 모든 프로세스가 종료상태인지 검사하는 함수
bool checkFinished(Process* pl) {
	Process *temp = pl;

	// 프로세스가 모두 종료상태인지 검사
	while (temp != NULL) {
		if (!temp->onFinished)	// 종료상태가 아닌 프로세스가 하나라도 있다면 false리턴
			return false;
		temp = temp->np;
	}
	return true;
}

// 프로세스의 상태를 출력
void PrintProcessStatus(Process* p) {
	// ①프로세스 id, ②도착시간, ③서비스 시간, ④종료 시간, ⑤반환 시간, ⑥정규화된 반환 시간
	printf("PID: %d\t도착시간: %d\t서비스 시간: %d\t종료 시간: %d\t반환 시간: %d\t대기 시간: %d\t정규화된 반환 시간: %.2f\n\n"
		, p->pid, p->t, p->s, p->ft, p->ft - p->t,p->w, p->r);
}

// 동적 할당 해제
void ReleaseProcess(Process* pl) {
	Process *p = pl;
	Process *temp;
	
	// 프로세스가 없을 경우
	if (p == NULL)
		return;

	while (p != NULL) {
		temp = p;
		p = p->np;
		free(temp);
	}
}
