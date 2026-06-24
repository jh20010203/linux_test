#include "mutex.h"

//创建共享工作区
struct msg
{
    struct msg *next;
    int num;
};

struct msg *head;//全局变量
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;//初始化全局互斥锁
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;//初始化全局条件变量

void thread_err(int ret , char *str)
{
    if(ret!=0)
    {
        fprintf(stderr,"%s:%s\n",str,strerror(ret));
        pthread_exit(NULL);
    }
}

//生产者
void *produce(void *argc)
{
    while(1)
    {
    //生成数据
    struct msg *mp;
    mp=malloc(sizeof(struct msg));
    mp->num=rand()%1000+1;//生产随即数据

    pthread_mutex_lock(&mutex);//加锁，解锁在条件变量中做过

    //将数据放置共享区
    mp->next=head;
    head=mp;

    printf("produce:%d\n",mp->num);

    pthread_mutex_unlock(&mutex);//解锁

    pthread_cond_signal(&cond);//通知阻塞在条件变量上的消费线程
  
    sleep(rand()%3);
    }

    return NULL;
}

//消费者
void *consumer(void *argc)
{
    while(1)
    {
    struct msg *mp;

    pthread_mutex_lock(&mutex);//加锁

    if(head==NULL)//判断共享区内无数据，此时阻塞线程
    {
    pthread_cond_wait(&cond,&mutex);//条件变量:1.阻塞等条件变量满足；2.解锁；3.加锁    
    }

    //访问共享数据
    mp=head;
    head=head->next;

    pthread_mutex_unlock(&mutex);//解锁

    printf("consumer:%d\n",mp->num);
    free(mp);    

    sleep(rand()%3);
    }
    
    return NULL;
}

int main(int argc , char *argv[])
{
    //tid1对应生产者，tid2对应消费者
    pthread_t tid1,tid2;
    int ret;
    srand(time(NULL));//随机数种子

    //pthread_mutex_init(&mutex,NULL);//初始化互斥锁

    ret=pthread_create(&tid1,NULL,produce,NULL);//生产者线程
    thread_err(ret,"pthread_create_produce error");

    ret=pthread_create(&tid2,NULL,consumer,NULL);//消费者线程
    thread_err(ret,"pthread_create_consumer error");


    //线程回收
    ret=pthread_join(tid1,NULL );
    thread_err(ret,"pthread_join error");
    ret=pthread_join(tid2,NULL);
    thread_err(ret,"pthread_join error");

    //pthread_mutex_destroy(&mutex);//消除互斥锁

    pthread_exit((void *)0);

    return 0;

}