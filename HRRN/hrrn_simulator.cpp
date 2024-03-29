// hrrn_simulator.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"


int main(int argc, char* argv[])
{
	FILE *fp;	// 파일 포인터 생성
	Process *pl = NULL;	// 프로세스 포인터 생성
	char fn[] = "input.txt";

	fp = ReadProcessData(fn);	// 파일 읽어오기
	pl = GetProcessList(fp);	// 프로세스 리스트 생성

	RunProcessByHRRN(pl);	// hrrn스케줄링 방식으로 프로세스 실행

	ReleaseProcess(pl);	// 동적할당 해제
	fclose(fp);	// 파일 닫기

	getchar();

    return 0;
}

