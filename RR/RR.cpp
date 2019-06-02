#include <stdio.h>
#include <stdlib.h>


struct process
{
	int name;       //���μ����̸�
	int arrive;     //���μ��� �����ð�
	int burst;      //���μ��� ����ð�

	process *prev;  //����
	process *next;  //����
};

process *newprocess(int name, int arrive, int burst); //���μ��� ����
void link_process(process **tail, process *newp);     //���μ��� ���� ����
void RR(process *p_tail, int quantum); //RR �����층
void delay();                                         //�������� �����ֱ�����
int total_p;										  //���μ��� ���� üũ init = 0
int name[20],burst[20],arrive[20];					  // ���Ͽ��� �ҷ��ͼ� ������ ����


void main()
{
	
	
	int pid = 0; // process id �ʱ�ȭ
	int arr = 0; // ���� �ð� �ʱ�ȭ
	int bur = 0; // ���� �ð� �ʱ�ȭ
    char quantum[3];  //quantum context
    char c;                     //�������̽� ���û���
    

	arrive[0] = -1;       //ó���� ������ ���μ������� �����ð��� �۱�����

	process *p_tail;	//���μ��� �� ����(������ �Ǵ� ����)
	
	FILE* fp;
    
	
	fp = fopen("input.txt", "r");  //file open

	if(fp == 0)						 //����
	{
		printf("���� �ҷ����⸦ �����Ͽ����ϴ�"); 
		return;
	}
     
	for(int i=1;;i++){
		fscanf(fp, "%d,%d,%d", &pid, &arr, &bur);
		name[i] = pid;

		if(name[i] == 0)
			break;

		arrive[i] = arr;   //fscanfó���� ���� %s�� ������ int�� ��ȯ		 	
		burst[i] = bur;
				
    	total_p++;					
	}

	for (int i = 1; i <= total_p; i++) { // ���� �������� �������� ����
		int temp_pid, temp_arr, temp_bur;	// �ӽú���
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

			if(fp == 0)						 //����
			{
				printf("���� �ҷ����⸦ �����Ͽ����ϴ�"); 
				return;
			}
		
			printf("|-------------------------------------------------------------------|\n");
			printf("|                                                                   |\n");                        
			printf("|                      ����  �κ�  �����층                       |\n");
			printf("|                                                                   |\n");   
			printf("|-------------------------------------------------------------------|\n");
		
			printf("[Quantum time Input] : ");
			scanf("%s",quantum);
	        
			if(atoi(quantum) == 1 || !(atoi(quantum) >= 1 && atoi(quantum) <= 9999))
			{
				printf("�߸��� �Է��Դϴ�.\n");
				continue;
			}    
			
			system("cls");
		    
						
		    p_tail = NULL;
			

			for(int i=1;i<=total_p;i++){
			
				link_process(&p_tail, newprocess(name[i], arrive[i], burst[i]));  //���μ��� ������ �켱������ ���� 
               
			}  //���μ��� ��ũ ����
		
			
			
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

/*���μ��� ����*/
process *newprocess(int name, int arrive, int burst)
{
	process *newp = (process*)malloc(sizeof(process));  //�޸� �Ҵ� (���ο� ���μ��� ����)

	newp->name = name;                                 //���μ��� ��ȣ,arrive,burst �Ҵ�
	newp->arrive = arrive;                                
	newp->burst = burst;                                  
	
	newp->prev = NULL;                              //���ο����μ��� ���� ���� NULL����
	newp->next = NULL;
    
	return newp;
}



/*���μ��� ���ᱸ��(���� ���� ��ũ�� ����Ʈ) - Ready queue ���� */
void link_process(process **tail, process *newp)  //**���������ʹ� �������� ������ ����( *process �� call by reference �����ϱ� ���� )
{
	if((*tail) == NULL)                //ó�� ���μ��� 
	{
		(*tail) = newp;                 //���μ��� �Ҵ� 
		newp->next = newp;			    //�������μ��� �ڽ��� ����Ŵ 
		newp->prev = newp;             //�������μ��� �ڽ��� ����Ŵ
	}
	else                               //2��° ������ ���μ���
	{
		(*tail)->next->prev = newp;    // ó���� ���� ���� ���μ��� ����
		newp -> next = (*tail)->next;  // ���ε��� ���μ��� ������ ó��(tail->next)�� ����
		newp -> prev = (*tail);        // ���� ���μ����� �����鼭 ���� ����  
		(*tail)->next = newp;

		(*tail) = newp;
	}

	return;
}

void RR(process *p_tail, int quantum)
{
	process *q_tail = NULL, *target = NULL;   //����ť = q_tail ����ť���� ���� Ÿ�� = target 
	process *del_target;                      //���μ��� ����Ϸ� Ÿ��
	
    float waiting_sum=0;                       //���μ������� ��ٸ��� �ð� ��
	float endtime[20]={0};					   //���μ������� ����ð�

	float total_t = 0, p_count = 0;        //�ǽð� total_t ���μ����� ������ �ð� p_count
	float switch_time = 0;                 //count_t �ð� üũ
    int check =0;                         //context üũ
	
	process *p; //�غ� ť�� �� ��� ���μ���

	while(1)
	{
		

		if(p_tail == NULL && q_tail == NULL) {break;}               //���μ����� ���� ť�� ��������� ���� 

		
        
		/*ready ť ����*/
		
		if(p_tail != NULL)
		{
			

            if(p_tail->next->arrive <= total_t )                       //�����ð��� ���� �ð��� ���ų� ������ ť�� ���� 
			{
				
				/*�� �ڽ��� ���� ������ �������(�ϳ��ϰ��) */
				 if(p_tail->next == p_tail)
				 {
					p = p_tail->next;
					p_tail = NULL;
				 }   //ť�� �� Ÿ���� �ٷ� �ڽ��� �ǰԵ� 
				
				else /*�� ó�� ���μ����� ���� ���μ����� �����ϴ� ���(�ΰ��̻�����)*/
				{
					p = p_tail->next;
					p-> prev->next = p->next;
					p-> next->prev = p->prev;
				}   //ť�� �� ���� ���μ����� ���� ���μ����� �̾��ش�
				
				p->prev = NULL;
				p->next = NULL;   //ť�� �������� �ڽ��� ��ũ�� �����ش�.
                 
				link_process(&q_tail,p);   //ť�� ����
			}
			
		}
		
		
		if(q_tail == NULL) 
		{
		    printf("��");total_t++;
		    //target = q_tail;         // 1 ���μ��� ����ð����� 2���μ����� �������ϸ� ������ �����ϱ�����
			continue;
		}		//ť�� ���������� ��ü�ð��� �ø��� �ٽ����� (����ð��� � ���μ����� �������� ���� ���) 
		
		if(target == NULL) { target = q_tail;}  //ť�� ���μ����� �ϳ������� target �� ���̵ȴ�.
		
		if(check == 1) { target = target->next; check=0;} //context switch

	
		/* Cpu ��� */
		delay();
		printf("P%d", target->name);                 
		target->burst--;              
		total_t++;                    //CPU ����� ���� �ð�����
		p_count++;                    //���μ����� ����� �ð� üũ

		/*���μ��� ���̻� ����� ����Ʈ�� ���� not context switch*/
		if(target->burst <= 0)
		{
			
		    if(target == target->next)  //q_tail->next == start q_tail->next->next = start->next �غ� ť ������ ����Ұ��� �ڱ� �ڽ��ΰ�� 
			{
					endtime[target->name] = total_t;//+process_context[target->name];  // ���μ������� ��������ð� 
				    q_tail = NULL;
					del_target = target;
					free(del_target);
					target = NULL;
					p_count = 0;
					
			}
			else
			{
			endtime[target->name] = total_t;//+process_context[target->name];  //���μ������� ���� ����ð�
		    target->prev->next = target->next;
			target->next->prev = target->prev;
            
		    p_count = 0;    //���μ��� ����� �ð� �ʱ�ȭ
			
			del_target = target;
            target = target->next;
			free(del_target);
			}
		}
		
	
		/* context switch */
		if(p_count >= quantum)   //context switch (quantum �ð��� ���� ��������)
		{
		   
			p_count = 0;          //���μ����� ����� �ð� �ʱ�ȭ
			delay();
			printf("(C)");
			check = 1;
			q_tail = target;
		 }
        
		
	 
		}
    
	printf("\n\n\n");
  	
   for(int i=1 ; i<=total_p ; i++){
	   	
	   printf("     [ Process%d ] ���� �ð� = %d ���� �ð� = %d ���� �ð�  = %.2f  ��ȯ �ð� = %.2f ����ȭ�� ��ȯ �ð� = %.2f\n",name[i],arrive[i], burst[i],endtime[i], endtime[i]-arrive[i], (endtime[i]-arrive[i])/burst[i]);

   }

	

	return ;
}

void delay()
{
	for(int i = 0; i < 1500; i++)
	  for(int j = 0; j < 30000; j++);

}