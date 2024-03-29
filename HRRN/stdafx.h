// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define EMPTY -1		// 비어있을 때의 값을 -1로 약속
//#define DEBUG			// 디버깅을 위한 출력값 표시 여부 결정 정의


// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
typedef struct process Process;

struct process {
	int pid;	// 프로세스 id
	int t;	// 도착 시간
	int s;	// 예상되는 서비스 시간
	int w;	// 처리기를 기다리며 대기한 시간
	int et;	// 실행된 시간
	int ft;	// 종료된 시간
	float r;	// 응답 비율; r = (w + s) / s
	bool onExecute;	// 실행 여부
	bool onReady;	// 준비큐 진입 여부
	bool onFinished;	// 프로세스 종료 여부
	Process *np;	// 다음 프로세스 주소
};

FILE* ReadProcessData(char* fn);	// 파일 읽기 함수
Process* GetProcessList(FILE* fp);	// 파일에서 프로세스 정보를 읽어오는 함수
void ReleaseProcess(Process* pl);	// 동적 할당 해제 함수
Process* InitProcess(Process* pl, int pid, int t, int s);	// 프로세스 동적할당 및 초기화 함수
void RunProcessByHRRN(Process* pl);	// 프로세스를 HRRN스케줄링 방식으로 실행하는 함수
void ProcessSelecter(Process* pl, int* time);	// 프로세스를 준비큐에 넣는 함수
Process* ProcessScheduler(Process* pl, bool* isRunning);	// 프로세스 스케줄링 함수
void ProcessDispatcher(Process* pl, Process* sp, int* time, bool* isRunning);		// 프로세스 실행 함수
bool checkFinished(Process* pl);	// 프로세스가 종료상태인지 검사하는 함수
void PrintProcessStatus(Process* p);	// 프로세스의 상태를 출력하는 함수