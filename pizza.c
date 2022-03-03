#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "p3190003-p3190167-p3190171-pizza.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
pthread_mutex_t lock_thl, lock_fournoi, lock_psistis, lock_packet, lock_deliv, lock_screen;
pthread_cond_t  cond_thl, cond_fournoi, cond_psistis, cond_packet, cond_deliv;
int telavail = Ntel;
int psistisavail = Ncook;
int fournoiavail = Noven;
bool packetaristas = 1;
int deliveras = Ndeliverer;
int Ncust;
int esoda;
bool k=1;
int epityxhmenes=0;
int apotyxhmenes=0;
int totalan = 0; //synolikos xronos anamonhs gia thlefwnhth
int maxanam = -1; //megistos xronos anamonhs gia thlefwnhth
int totaltime = 0; //synolikos xronos apo thn afiksi tou pelath ws thn paradwsh
int maxtime = -1; //megistos xronos apo thn afixi tou pelath ws thn paradwsh
int totalkryom = 0; //synolikos xronos kryomatos paraggelias
int maxkryom = -1; //megistos xronos kryomatos paraggelias
unsigned int seed;
void rc_check(int rc){
	if(rc != 0){
		printf("ERROR: return code %d from rc",rc);
		exit(-1);
	}
	
}
void *order(void *x){
    int id = *(int *)x;
	unsigned int s_id = seed + id;
	//an den einai h prwth paraggelia  poy ekteleitai
	if (k!=1){ 
		sleep((rand_r(&s_id) %(Torderhigh - Torderlow + 1)) + Torderlow);	
	}
	if (k==1){
		k=2;
	}

    printf("Γεια από την παραγγελία : %d\n",id);
	struct timespec begin, stop, kryom, end, packet;
	clock_gettime(CLOCK_REALTIME, &begin);
	///thlefvnhths
    int rc = pthread_mutex_lock(&lock_thl);
	rc_check(rc);
    while (telavail == 0) {
		rc = pthread_mutex_lock(&lock_screen);
		rc_check(rc);
		printf("Η παραγγελία με αριθμό %d δεν βρήκε διαθέσιμο τηλεφωνητή...\n", id);
		rc = pthread_mutex_unlock(&lock_screen);
		rc_check(rc);
		rc = pthread_cond_wait(&cond_thl, &lock_thl);
		rc_check(rc);
    }
	rc = pthread_mutex_lock(&lock_screen);
	rc_check(rc);
	printf("Η παραγγελία με αριθμό %d βρήκε τηλεφωνητή.\n",id);
	rc = pthread_mutex_unlock(&lock_screen);
	rc_check(rc);
    clock_gettime(CLOCK_REALTIME, &stop);
	double delta = stop.tv_sec - begin.tv_sec;
    totalan += (int)delta;
	if ((int)delta > maxanam) {
		maxanam = (int)delta;
	}
	telavail--;
	Ncust = (rand_r(&s_id) %(Norderhigh - Norderlow + 1)) + Norderlow;
	rc = pthread_mutex_unlock(&lock_thl);
	rc_check(rc);
	sleep((rand_r(&s_id) %(Tpaymenthigh - Tpaymentlow + 1)) + Tpaymentlow);	
	rc = pthread_mutex_lock(&lock_thl);
	rc_check(rc);
	if((double)rand_r(&s_id)/(double)RAND_MAX < Pfail){
		rc = pthread_mutex_lock(&lock_screen);
		rc_check(rc);
		printf("Η παραγγελία με αριθμό %d απέτυχε.\n", id);
		rc = pthread_mutex_unlock(&lock_screen);
		rc_check(rc);
		telavail++;
		apotyxhmenes++;
		rc = pthread_cond_signal(&cond_thl);
		rc_check(rc);
		rc = pthread_mutex_unlock(&lock_thl);
		rc_check(rc);
		pthread_exit(NULL);
	}
	esoda += Cpizza * Ncust;
	rc = pthread_mutex_lock(&lock_screen);
	rc_check(rc);
    printf("H παραγγελία με αριθμό %d καταχωρήθηκε. \n", id);
	rc = pthread_mutex_unlock(&lock_screen);
	rc_check(rc);
    telavail++;
	rc = pthread_cond_signal(&cond_thl);
	rc_check(rc);
    rc = pthread_mutex_unlock(&lock_thl);	
	rc_check(rc);
	//psistis
	rc = pthread_mutex_lock(&lock_psistis);
	rc_check(rc);
	while (psistisavail == 0) {
		rc = pthread_mutex_lock(&lock_screen);
		rc_check(rc);
		printf("H παραγγελία με αριθμό %d δεν βρήκε διαθέσιμο ψήστη...\n", id);
		rc = pthread_mutex_unlock(&lock_screen);
		rc_check(rc);
		rc = pthread_cond_wait(&cond_psistis, &lock_psistis);
		rc_check(rc);
    } 
	rc = pthread_mutex_lock(&lock_screen);
	rc_check(rc);
	printf("H παραγγελία με αριθμό %d βρήκε διαθέσιμο ψήστη!\n", id);
	rc = pthread_mutex_unlock(&lock_screen);
	rc_check(rc);
    psistisavail--;
    rc = pthread_mutex_unlock(&lock_psistis);
	rc_check(rc);
    sleep(Tprep * Ncust);	
	//fournoi
    rc = pthread_mutex_lock(&lock_fournoi);
	rc_check(rc);
	while (fournoiavail < Ncust) {
		rc = pthread_mutex_lock(&lock_screen);
		rc_check(rc);
		printf("H παραγγελία με αριθμό %d δεν βρήκε τον απαραίτητο αριθμό διαθέσιμων φούρνων.... \n", id);
		rc = pthread_mutex_unlock(&lock_screen);
		rc_check(rc);
		rc = pthread_cond_wait(&cond_fournoi, &lock_fournoi);
		rc_check(rc);
    }
	rc = pthread_mutex_lock(&lock_screen);
	rc_check(rc);
	printf("H παραγγελία με αριθμό %d βρήκε τον απαραίτητο αριθμό διαθέσιμων φούρνων!\n", id);
	rc = pthread_mutex_unlock(&lock_screen);
	rc_check(rc);
	fournoiavail -= Ncust;
	psistisavail++;
	rc = pthread_cond_signal(&cond_psistis);
	rc_check(rc);
	rc = pthread_mutex_unlock(&lock_fournoi);
	rc_check(rc);
    sleep(Tbake);	
	rc = pthread_mutex_lock(&lock_screen);
	rc_check(rc);
    printf("H παραγγελία με αριθμό %d ψήθηκε επιτυχώς! \n", id);
	rc = pthread_mutex_unlock(&lock_screen);
	rc_check(rc);
	clock_gettime(CLOCK_REALTIME, &kryom);
    //packet boy
	rc = pthread_mutex_lock(&lock_packet);
	rc_check(rc);
	while (!packetaristas) {
		rc = pthread_mutex_lock(&lock_screen);
		rc_check(rc);
		printf("H παραγγελία με αριθμό %d δεν βρήκε διαθέσιμο τον υπάλληλο πακεταρίσματος...\n", id);
		rc = pthread_mutex_unlock(&lock_screen);
		rc_check(rc);
		rc = pthread_cond_wait(&cond_packet, &lock_packet);
		rc_check(rc);
    }
	rc = pthread_mutex_lock(&lock_screen);
	rc_check(rc);
	printf("H παραγγελία με αριθμό %d βρήκε διαθέσιμο τον υπάλληλο πακεταρίσματος!\n", id);
	rc = pthread_mutex_unlock(&lock_screen);
	rc_check(rc);
    packetaristas=2;
    rc = pthread_mutex_unlock(&lock_packet);
	rc_check(rc);
    sleep(Tpack*Ncust);
	clock_gettime(CLOCK_REALTIME, &packet);
	delta = packet.tv_sec - begin.tv_sec;
    int packettime = (int)delta;
	rc = pthread_mutex_lock(&lock_screen);
	rc_check(rc);
	printf("H παραγγελία με αριθμό %d ετοιμάστηκε σε %d.", id, packettime/60);
	if (packettime%60 < 10)
		printf("0");
	printf("%d λεπτά\n",packettime%60);
	rc = pthread_mutex_unlock(&lock_screen);
	rc_check(rc);
	rc = pthread_mutex_lock(&lock_packet);
	rc_check(rc);
	packetaristas=1;
	rc = pthread_cond_signal(&cond_packet);
	rc_check(rc);
	rc = pthread_mutex_unlock(&lock_packet);
	rc_check(rc);
	rc = pthread_mutex_lock(&lock_fournoi);
	rc_check(rc);
	fournoiavail += Ncust;
	rc = pthread_cond_signal(&cond_fournoi);
	rc_check(rc);
	rc = pthread_mutex_unlock(&lock_fournoi);
	rc_check(rc);
	//deliveras
	rc =  pthread_mutex_lock(&lock_deliv);
	rc_check(rc);
	while (deliveras<1) {
		rc = pthread_mutex_lock(&lock_screen);
		rc_check(rc);
		printf("H παραγγελία με αριθμό %d δεν βρήκε διαθέσιμο διανομέα...\n", id);
		rc = pthread_mutex_unlock(&lock_screen);
		rc_check(rc);
		rc = pthread_cond_wait(&cond_deliv, &lock_deliv);
		rc_check(rc);
    }
	rc = pthread_mutex_lock(&lock_screen);
	rc_check(rc);
	printf("H παραγγελία με αριθμό %d βρήκε διαθέσιμο διανομέα!\n", id);
	rc = pthread_mutex_unlock(&lock_screen);
	rc_check(rc);
    deliveras--;
    rc = pthread_mutex_unlock(&lock_deliv);
	rc_check(rc);
    int time = (rand_r(&s_id) %(Tdelhigh - Tdellow + 1)) + Tdellow;
	sleep(time);
	clock_gettime(CLOCK_REALTIME, &end);
	delta = end.tv_sec - begin.tv_sec;
    totaltime += (int)delta;
	rc = pthread_mutex_lock(&lock_screen);
	rc_check(rc);
	printf("H παραγγελία με αριθμό %d παραδόθηκε σε %d.", id,(int)delta/60);
	if ((int)delta%60 < 10)
		printf("0");
	printf("%d λεπτά\n",(int)delta%60);
	rc = pthread_mutex_unlock(&lock_screen);
	rc_check(rc);
	if ((int)delta > maxtime) {
		maxtime = (int)delta;
	}
	delta = end.tv_sec - kryom.tv_sec;
    totalkryom += (int)delta;
	if ((int)delta > maxkryom) {
		maxkryom = (int)delta;
	}
	epityxhmenes++;
	sleep(time);
	rc = pthread_mutex_lock(&lock_deliv);
	rc_check(rc);
	deliveras++;
    rc = pthread_cond_signal(&cond_deliv);
	rc_check(rc);
    rc = pthread_mutex_unlock(&lock_deliv);
	rc_check(rc);
    pthread_exit(NULL);
}
int main(int argc, char* argv[])  {
	if (argc != 3) {
		printf("ERROR: the program should take one argument, the number of threads to create!\n");
		exit(-1);
	}
	int threadnum = atoi(argv[1]);
	if(threadnum <= 0){
		printf("ERROR: Number of threads should be positive.\n");
		exit(-1);
	}
	seed = atoi(argv[2]);
	if(seed < 0){
		printf("ERROR: Number of random input should be positive.\n");
		exit(-1);
	}
    int rc;
	int id[threadnum];
    pthread_t threads[threadnum];
	pthread_mutex_init(&lock_thl, NULL); 
	pthread_mutex_init(&lock_psistis, NULL);
	pthread_mutex_init(&lock_fournoi, NULL);
	pthread_mutex_init(&lock_packet, NULL);
	pthread_mutex_init(&lock_deliv, NULL);
	pthread_mutex_init(&lock_screen, NULL);
    pthread_cond_init(&cond_thl, NULL); 
	pthread_cond_init(&cond_psistis, NULL);
	pthread_cond_init(&cond_fournoi, NULL);
	pthread_cond_init(&cond_packet, NULL);
	pthread_cond_init(&cond_deliv, NULL);
    for (int i = 0; i < threadnum; i++) {
		id[i] = i+1;
		printf("Main: δημιουργία νήματος %d\n", i+1);
		rc = pthread_create(&threads[i], NULL, order, &id[i]);
		if(rc != 0){
			printf("ERROR: return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
    }
    for (int i = 0; i < threadnum; i++) {
		rc=pthread_join(threads[i], NULL);
		if(rc != 0){
			printf("ERROR: return code from pthread_join() is %d\n", rc);
			exit(-1);
		}
    }
    pthread_mutex_destroy(&lock_thl); 
	pthread_mutex_destroy(&lock_psistis);
	pthread_mutex_destroy(&lock_fournoi);
	pthread_mutex_destroy(&lock_packet);
	pthread_mutex_destroy(&lock_deliv);
	pthread_mutex_destroy(&lock_screen); 
	pthread_cond_destroy(&cond_thl); 
	pthread_cond_destroy(&cond_psistis);
	pthread_cond_destroy(&cond_fournoi);
	pthread_cond_destroy(&cond_packet);
	pthread_cond_destroy(&cond_deliv);
	printf("\n	---------------------------");
	
	printf("\n 	Συνολικά έσοδα πιτσαρίας : %d", esoda);
	printf("\n 	Επιτυχημένες παραγγελίες : %d", epityxhmenes);
	printf("\n	Αποτυχημένες παραγγελίες: %d", apotyxhmenes);
	if (threadnum > 0){
		printf("\n	Μέσος χρόνος αναμονής για εύρεση διαθέσιμου τηλεφωνητή: %d.", totalan/threadnum/60);
		if (totalan/threadnum%60 < 10)
			printf("0");
		printf("%d",totalan/threadnum%60);
		printf("\n	Μέγιστος χρόνος αναμονής για εύρεση διαθέσιμου τηλεφωνητή: %d.", maxanam/60);
		if (maxanam%60 < 10)
			printf("0");
		printf("%d", maxanam%60);
	}
	if (epityxhmenes > 0){
		printf("\n	Μέσος χρόνος αναμονής μέχρι την παράδωση της πίτσας απο την στιγμή που εμφανίζεται ο πελάτης : %d.", totaltime/epityxhmenes/60);
		if (totaltime/epityxhmenes%60 < 10)
			printf("0");
		printf("%d",totaltime/epityxhmenes%60);
		printf("\n	Μέγιστος χρόνος αναμονής μέχρι την παράδωση της πίτσας απο την στιγμή που εμφανίζεται ο πελάτης : %d.", maxtime/60);
		if (maxtime%60 < 10)
			printf("0");
		printf("%d", maxtime%60);
		printf("\n	Μέσος χρόνος κρυώματος των παραγγελιών : %d.", totalkryom/epityxhmenes/60);
		if (totalkryom/epityxhmenes%60 < 10)
			printf("0");
		printf("%d",totalkryom/epityxhmenes%60);
		printf("\n	Μέγιστος χρόνος κρυώματος από τις παραγγελίες : %d.", maxkryom/60);
		if (maxkryom%60 < 10)
			printf("0");
		printf("%d", maxkryom%60);
		printf("\n	---------------------------");
		printf("\n");
	}
    return 0;
}

