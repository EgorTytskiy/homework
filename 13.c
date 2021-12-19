#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <uuid/uuid.h>
#include <grp.h>
#include <sys/wait.h>

int printinfo(void)
{
    printf("Process id: %d\n", getpid());
    printf("Process parent id: %d\n", getppid());
    struct passwd *pwd;
    if (!(pwd = getpwuid(getuid()))){
        perror("petpwuid");
    } else {
        printf("user id: %d\n", pwd->pw_uid);
        printf("user name: %s\n", pwd->pw_name);
        printf("group id: %d\n", pwd->pw_gid);
    }
    struct group *grp;
    if (!(grp = getgrgid(pwd->pw_gid))){
        perror("getgrgid");
    } else {
        printf("group name: %s\n", grp->gr_name);
    }
    return 1;
}

int main(void)
{
    pid_t child_id = fork();
    switch (child_id)
    {
        case -1:
            perror("fork");
            return 1;
        case 0:
            printf("child:\n");
            printinfo();
            return 0;
        default:
            wait(NULL);
            printf("parent:\n");
            printinfo();
            return 0;
    }
    return 0;
}