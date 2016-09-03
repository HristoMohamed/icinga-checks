#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iso646.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#define BUFSIZE 128
#define SHMSZ   128

pid_t child_pid = -1 ; //Global
static int alarm_fired = 0;

void kill_child(int sig)
{
    kill(child_pid,SIGKILL);
    alarm_fired=1;

}

int main(){

    char *buf;
    int shmid = shmget (IPC_PRIVATE, sizeof *buf * BUFSIZE, IPC_CREAT | IPC_EXCL | 0600);
    buf = shmat (shmid, 0, 0);

    signal(SIGALRM,(void (*)(int))kill_child);
    child_pid = fork();
    if (child_pid > 0) {
        alarm(10);
        wait(NULL);
    }
    else if (child_pid == 0){
        struct stat st;
        int ret;
        char *getNfs="cat /proc/mounts | grep nfs:| awk '{print $2}'";
        int allPoints=0;
        int ok=0;
        char stalePoints[32768];
        //char buf[BUFSIZE];
        FILE *fp;

        if ((fp = popen(getNfs, "r")) == NULL) {
            printf("Error opening pipe!\n");
            return -1;
        }

        while (fgets(buf, BUFSIZE, fp) != NULL) {
            allPoints+=1;
            buf[strcspn(buf, "\n")] = 0;
            ret = stat(buf, &st);
            if(ret == -1 and errno == ESTALE){
                sprintf(stalePoints, "Directory %s is stale!\t", buf);
            } else {
                ok+=1;
            }
        }

        if(pclose(fp))  {
            printf("Command not found or exited with error status\n");
            exit(3);
        }

        if ( ok == allPoints ){
            printf("All mount points are OK\n");
            exit(0);
        }
        else{
            printf("%s\n",stalePoints);
            exit(2);
        }
  }
    if (alarm_fired) {
        printf("%s times out on access!!!\n", buf);
        exit(3);
    }
}
