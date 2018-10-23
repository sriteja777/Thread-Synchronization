#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

int org_busy = 0;
pthread_mutex_t mutex, mutex_orgn, mutex_nopao, mutex_norao, mutex_arg, mutex_wtng_plrs, mutex_wtng_rfrs; 
pthread_cond_t cond;
int nopao = 0;
int norao = 0;
int *waiting_players;
int *waiting_referees;
int ind_wtng_plrs = 0;
int ind_wtng_rfrs = 0;
typedef struct args
{
	int type;
	int id;
} args;
void *meet_organizer(int person, int id);
void *enter_court();
void *warm_up();
void *adjust_equipment();
void *start_game();
void *enter_academy(void* person) {

	// printf("came\n");
	args* giv = (args*)person;
	if (giv->type == 0)  {
		printf("Player %d entered academy\n",giv->id);
		pthread_mutex_lock(&mutex_wtng_plrs);
		waiting_players[ind_wtng_plrs] = giv->id;
		ind_wtng_plrs ++;
		pthread_mutex_unlock(&mutex_wtng_plrs);
		pthread_mutex_lock(&mutex_nopao);
		
		nopao ++;
		pthread_mutex_unlock(&mutex_nopao);
	}
	else if (giv->type == 1)  {
		printf("Referee %d entered academy\n", giv->id);
		pthread_mutex_lock(&mutex_wtng_rfrs);
		waiting_referees[ind_wtng_rfrs] = giv->id;
		ind_wtng_rfrs ++;
		pthread_mutex_unlock(&mutex_wtng_rfrs);
		pthread_mutex_lock(&mutex_norao);
		norao ++;
		pthread_mutex_unlock(&mutex_norao);
	}
	meet_organizer(giv->type, giv->id);
	// pthread_mutex_lock(&mutex);
	// pthread_cond_wait(&cond, &mutex);
}
void *meet_organizer(int person, int id) {
	// printf("came here\n");
	int plr_id_1, plr_id_2, rfr_id; 
	while(org_busy==1);
	if (org_busy != 1) {
		if (person==0)  {
			printf("Player %d meet organizer.\n",id);

		}
		else if(person==1) printf("Referee %d meet organizer\n",id);
		if (nopao >=2 && norao >= 1) {
			// printf("sd\n");
			pthread_mutex_lock(&mutex_orgn);
			org_busy = 1;
			pthread_mutex_unlock(&mutex_orgn);
			pthread_mutex_lock(&mutex_wtng_plrs);
			plr_id_1 = waiting_players[ind_wtng_plrs - nopao];
			plr_id_2 = waiting_players[ind_wtng_plrs - nopao+1];
			pthread_mutex_unlock(&mutex_wtng_plrs);
			pthread_mutex_lock(&mutex_nopao);
			nopao -= 2;
			pthread_mutex_unlock(&mutex_nopao);
			pthread_mutex_lock(&mutex_wtng_rfrs);
			rfr_id = waiting_referees[ind_wtng_rfrs - norao];
			pthread_mutex_unlock(&mutex_wtng_rfrs);
			pthread_mutex_lock(&mutex_norao);
			norao -= 1;
			pthread_mutex_unlock(&mutex_norao);
			enter_court(plr_id_1, plr_id_2, rfr_id);
		}
	}

	// if (org_busy != 1) {

	// 	if (nopao >= 2 && norao >= 1) {
	// 		// printf("came here\n");
	// 		pthread_mutex_lock(&mutex_orgn);
	// 		org_busy = 1;
	// 		pthread_mutex_unlock(&mutex_orgn);
	// 		pthread_mutex_lock(&mutex_nopao);
	// 		nopao -= 2;
	// 		pthread_mutex_unlock(&mutex_nopao);
	// 		pthread_mutex_lock(&mutex_norao);
	// 		norao -= 1;
	// 		pthread_mutex_unlock(&mutex_norao);
	// 		printf("Players and referee entered the court \n");
			
	// 		pthread_mutex_lock(&mutex_orgn);
	// 		org_busy = 0;
	// 		pthread_mutex_unlock(&mutex_orgn);
	// 	}
	// }
	// pthread_mutex_lock(&mutex);
	// pthread_cond_wait(&cond, &mutex);
	// printf("came here\n");
	// if (person == 0) {
	// 	printf("Player %d met organizer\n",id);
	// }
	// else if(person==1) {
	// 	printf("Referee %d met organizer\n",id);
	// }
	// pthread_mutex_unlock(&mutex);
}
void *enter_court(int plr_id_1, int plr_id_2, int rfr_id) {
	// printf("came here\n");
	pthread_t ref_th, plrs_th, game_th;
	printf("Players %d, %d and referee %d entered the court \n", plr_id_1, plr_id_2, rfr_id);
	// warmUp();
	// adjust_equipment();
	int ids[3];
	ids[0] = rfr_id;
	ids[1] = plr_id_1;
	ids[2] = plr_id_2;
	pthread_create(&ref_th,NULL,warm_up, (void*)&ids[1]);
	pthread_create(&plrs_th, NULL, adjust_equipment, (void*)&rfr_id);
	pthread_join(ref_th, NULL);
	pthread_join(plrs_th, NULL);
	pthread_create(&game_th, NULL, start_game, (void*)ids);
	pthread_mutex_lock(&mutex_orgn);
	org_busy = 0;
	pthread_mutex_unlock(&mutex_orgn);
	pthread_join(game_th, NULL);
}
void *warm_up(void* plr_ids) {
	int* ids = (int*)plr_ids;
	int ret_val;
	printf("Players %d, %d started warm up.\n",ids[0], ids[1]);
	sleep(1);

	pthread_exit((void *)&ret_val);
}
void *adjust_equipment(void* rfr_id) {
	void * ret_val;
	printf("Referee %d is adjusting equipment.\n",*((int*)rfr_id));
	sleep(0.5);
	pthread_exit((void*)&ret_val);
}
void *start_game(void* ids) {
	int* id = (int*)ids;
	printf("Players %d, %d and referee %d started the game.\n",id[1], id[2], id[0]);
}


int main() {
	int num_referees, num_players, rem_players, rem_referees;
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutex_norao, NULL);
	pthread_mutex_init(&mutex_nopao, NULL);
	pthread_mutex_init(&mutex_orgn, NULL);
	pthread_mutex_init(&mutex_arg, NULL);
	// pthread_cond_init(&cond, NULL);
	
	scanf("%d",&num_referees);
	num_players = 2*num_referees;
	rem_players = num_players;
	rem_referees = num_referees;
	int players[num_players];
	int referees[num_referees];
	pthread_t tid[3*num_referees];
	args *arg[3*num_referees] ;
	waiting_referees = (int*)malloc(sizeof(int)*num_referees);
	waiting_players = (int*)malloc(sizeof(int)*num_players);
	for (int i = 0; rem_referees > 0 || rem_players > 0; ++i) {
		arg[i] = (args*)malloc(sizeof(args));
		if (rem_players > rem_referees) {
		
			arg[i]->type = 0;
			arg[i]->id = num_players - rem_players + 1;
			// printf("cae\n");
			rem_players --;
			pthread_create(&tid[i], NULL, enter_academy, (void *)arg[i]);
			// printf("thread created\n");
		}
		else {
			arg[i]->type = 1;
			arg[i]->id = num_referees - rem_referees + 1;
			rem_referees --;
			pthread_create(&tid[i], NULL, enter_academy, (void *)arg[i]);
			
		}
		sleep(rand()%3);
		// printf("looping here\n");
	}

	for(int i=0;i<3*num_referees; i++){
		pthread_join(tid[i], NULL);
	}
}