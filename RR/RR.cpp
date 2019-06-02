#include <stdio.h>
#include <stdlib.h>


struct process
{
	int name;       //프로세스이름
	int arrive;     //프로세스 도착시간
	int burst;      //프로세스 수행시간

	process *prev;  //이전
	process *next;  //다음
};

process *newprocess(int name, int arrive, int burst); //프로세스 생성
void link_process(process **tail, process *newp);     //프로세스 연결 구성
void RR(process *p_tail, int quantum); //RR 스케쥴링
void delay();                                         //동적으로 보여주기위해
int total_p;										  //프로세스 갯수 체크 init = 0
int name[20],burst[20],arrive[20];					  // 파일에서 불러와서 저장할 공간


void main()
{
	
	
	int pid = 0; // process id 초기화
	int arr = 0; // 도착 시간 초기화
	int bur = 0; // 서비스 시간 초기화
    char quantum[3];  //quantum context
    char c;                     //인터페이스 선택사항
    

	arrive[0] = -1;       //처음에 들어오는 프로세스보다 도착시간이 작기위해

	process *p_tail;	//프로세스 끝 지정(꼬리가 되는 것임)
	
	FILE* fp;
    
	
	fp = fopen("input.txt", "r");  //file open

	if(fp == 0)						 //에러
	{
		printf("파일 불러오기를 실패하였습니다"); 
		return;
	}
     
	for(int i=1;;i++){
		fscanf(fp, "%d,%d,%d", &pid, &arr, &bur);
		name[i] = pid;

		if(name[i] == 0)
			break;

		arrive[i] = arr;   //fscanf처리를 위해 %s로 받은후 int형 변환		 	
		burst[i] = bur;
				
    	total_p++;					
	}

	for (int i = 1; i <= total_p; i++) { // 도착 기준으로 오름차순 정렬
		int temp_pid, temp_arr, temp_bur;	// 임시변수
		for (int j = 1; j <= total_p; j++) {
			if (arrive[i] < arrive[j]) {
				temp_arr = arrive[i];
				arrive[i] = arrive[j];
				arrive[j] = temp_arr;

				temp_pid = name[i];
				name[i] = name[j];
				name[j] = temp_pid;

				temp_bur = burst[i];
				burst[i] = burst[j];
				burst[j] = temp_bur;
			}
		}
	}
	

    while(1) {
            
		    fp = fopen("input.txt", "r");  //file open

			if(fp == 0)						 //에러
			{
				printf("파일 불러오기를 실패하였습니다"); 
				return;
			}
		
			printf("|-------------------------------------------------------------------|\n");
			printf("|                                                                   |\n");                        
			printf("|                      라운드  로빈  스케쥴링                       |\n");
			printf("|                                                                   |\n");   
			printf("|-------------------------------------------------------------------|\n");
		
			printf("[Quantum time Input] : ");
			scanf("%s",quantum);
	        
			if(atoi(quantum) == 1 || !(atoi(quantum) >= 1 && atoi(quantum) <= 9999))
			{
				printf("잘못된 입력입니다.\n");
				continue;
			}    
			
			system("cls");
		    
						
		    p_tail = NULL;
			

			for(int i=1;i<=total_p;i++){
			
				link_process(&p_tail, newprocess(name[i], arrive[i], burst[i]));  //프로세스 연결을 우선적으로 구성 
               
			}  //프로세스 링크 구성
		
			
			
			printf("|-------------------------------------------------------------------|\n");
			printf("|                                                                   |\n");                        
			printf("|                           [CPU BURST]                             |\n");
			printf("|                                                                   |\n");   
			printf("|-------------------------------------------------------------------|\n");
			printf("\n\n");
			printf("                           [Gantt Chart]\n");
			RR(p_tail,atoi(quantum));
			
			
		    
			
			while(1){
				
				printf("[Continue Wanted? 1 ] [Exit Wanted? 2 ]  \n"); 
				printf("[Selection] ");
				scanf(" %c",&c); 
				
				if( c=='1') { system("cls"); break; }
				else if( c=='2'){ return; }
				else { printf("[No.. 1Key OR 2Key]\n"); continue;}
			}
			
	    
		
	}
	getchar();

	return ;
}

/*프로세스 생성*/
process *newprocess(int name, int arrive, int burst)
{
	process *newp = (process*)malloc(sizeof(process));  //메모리 할당 (새로운 프로세스 생성)

	newp->name = name;                                 //프로세스 번호,arrive,burst 할당
	newp->arrive = arrive;                                
	newp->burst = burst;                                  
	
	newp->prev = NULL;                              //새로운프로세스 이전 다음 NULL지정
	newp->next = NULL;
    
	return newp;
}



/*프로세스 연결구성(원형 더블 링크드 리스트) - Ready queue 생성 */
void link_process(process **tail, process *newp)  //**다중포인터는 포인터의 포인터 구성( *process 를 call by reference 참조하기 위해 )
{
	if((*tail) == NULL)                //처음 프로세스 
	{
		(*tail) = newp;                 //프로세스 할당 
		newp->next = newp;			    //다음프로세스 자신을 가리킴 
		newp->prev = newp;             //이전프로세스 자신을 가리킴
	}
	else                               //2번째 이후의 프로세스
	{
		(*tail)->next->prev = newp;    // 처음과 새로 들어온 프로세스 연결
		newp -> next = (*tail)->next;  // 새로들어온 프로세스 다음에 처음(tail->next)에 연결
		newp -> prev = (*tail);        // 새로 프로세스가 들어오면서 꼬리 변경  
		(*tail)->next = newp;

		(*tail) = newp;
	}

	return;
}

void RR(process *p_tail, int quantum)
{
	process *q_tail = NULL, *target = NULL;   //레디큐 = q_tail 레디큐에서 수행 타겟 = target 
	process *del_target;                      //프로세스 수행완료 타겟
	
    float waiting_sum=0;                       //프로세스들의 기다리는 시간 합
	float endtime[20]={0};					   //프로세스마다 종료시간

	float total_t = 0, p_count = 0;        //실시간 total_t 프로세스가 수행한 시간 p_count
	float switch_time = 0;                 //count_t 시간 체크
    int check =0;                         //context 체크
	
	process *p; //준비 큐에 들어갈 대상 프로세스

	while(1)
	{
		

		if(p_tail == NULL && q_tail == NULL) {break;}               //프로세스가 없고 큐가 비어있으면 종료 

		
        
		/*ready 큐 구성*/
		
		if(p_tail != NULL)
		{
			

            if(p_tail->next->arrive <= total_t )                       //도착시간이 현재 시간과 같거나 작으면 큐에 구성 
			{
				
				/*ⓐ 자신이 끝을 가지고 있을경우(하나일경우) */
				 if(p_tail->next == p_tail)
				 {
					p = p_tail->next;
					p_tail = NULL;
				 }   //큐에 들어갈 타겟이 바로 자신이 되게됨 
				
				else /*ⓑ 처음 프로세스와 끝의 프로세스가 존재하는 경우(두개이상존재)*/
				{
					p = p_tail->next;
					p-> prev->next = p->next;
					p-> next->prev = p->prev;
				}   //큐에 들어갈 이전 프로세스와 다음 프로세스를 이어준다
				
				p->prev = NULL;
				p->next = NULL;   //큐에 들어가기전에 자신의 링크를 끊어준다.
                 
				link_process(&q_tail,p);   //큐에 삽입
			}
			
		}
		
		
		if(q_tail == NULL) 
		{
		    printf("━");total_t++;
		    //target = q_tail;         // 1 프로세스 수행시간동안 2프로세스가 도착안하면 에러를 방지하기위함
			continue;
		}		//큐가 비어있을경우 전체시간만 늘리고 다시진행 (현재시간에 어떤 프로세스도 도착하지 않을 경우) 
		
		if(target == NULL) { target = q_tail;}  //큐에 프로세스가 하나였을때 target 은 널이된다.
		
		if(check == 1) { target = target->next; check=0;} //context switch

	
		/* Cpu 사용 */
		delay();
		printf("P%d", target->name);                 
		target->burst--;              
		total_t++;                    //CPU 사용후 현재 시간증가
		p_count++;                    //프로세스가 사용한 시간 체크

		/*프로세스 더이상 사용할 버스트가 없음 not context switch*/
		if(target->burst <= 0)
		{
			
		    if(target == target->next)  //q_tail->next == start q_tail->next->next = start->next 준비 큐 다음에 출력할것이 자기 자신인경우 
			{
					endtime[target->name] = total_t;//+process_context[target->name];  // 프로세스마다 수행종료시간 
				    q_tail = NULL;
					del_target = target;
					free(del_target);
					target = NULL;
					p_count = 0;
					
			}
			else
			{
			endtime[target->name] = total_t;//+process_context[target->name];  //프로세스마다 수행 종료시간
		    target->prev->next = target->next;
			target->next->prev = target->prev;
            
		    p_count = 0;    //프로세스 사용한 시간 초기화
			
			del_target = target;
            target = target->next;
			free(del_target);
			}
		}
		
	
		/* context switch */
		if(p_count >= quantum)   //context switch (quantum 시간에 일을 끝냈을때)
		{
		   
			p_count = 0;          //프로세스가 사용한 시간 초기화
			delay();
			printf("(C)");
			check = 1;
			q_tail = target;
		 }
        
		
	 
		}
    
	printf("\n\n\n");
  	
   for(int i=1 ; i<=total_p ; i++){
	   	
	   printf("     [ Process%d ] 도착 시간 = %d 서비스 시간 = %d 종료 시간  = %.2f  반환 시간 = %.2f 정규화된 반환 시간 = %.2f\n",name[i],arrive[i], burst[i],endtime[i], endtime[i]-arrive[i], (endtime[i]-arrive[i])/burst[i]);

   }

	

	return ;
}

void delay()
{
	for(int i = 0; i < 1500; i++)
	  for(int j = 0; j < 30000; j++);

}