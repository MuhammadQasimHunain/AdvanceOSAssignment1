//
//  LinkedQueueDatastructures.h
//  Client
//
//  Created by Apple PC on 10/04/2017.
//  Copyright © 2017 ArhamSoft. All rights reserved.
//

#ifndef LinkedQueueDatastructures_h
#define LinkedQueueDatastructures_h

#define ProxyPortNo             5744
#define ServerPortNo			ProxyPortNo + 100
#define LocalGet                "LOCAL-GET"
#define BYTES                   1024
#define MaxNumberOfThred        10
#define BufferLength            1024
#define SegmentLength           10
#define FileTokenKey            "/FileToken"
#define ProxyIpAddress          "127.0.0.1"

// MARK: - Global Variables

bool SHOULD_USE_SHARED_MEMORY = false;
pthread_cond_t Condition[MaxNumberOfThred];
pthread_mutex_t Mutex[MaxNumberOfThred];
char *ROOT;


struct Thread
{
    pthread_t ThreadID;
    int ThreadNo;
    bool IsFree; // If THe Thread Is Free
    int SocketId; // Comunicate to client via Socket
};

struct ThreadPoolManager
{
    struct Thread ThreadArray[MaxNumberOfThred];
    int totalThreadCount;
};

struct SharedMemory
{
    pthread_cond_t Condition;
    pthread_mutex_t Mutex;
    pthread_condattr_t ConditionAttribute;
    pthread_mutexattr_t MutexAttribute;
    bool hasFileCompletelyWritten;
    char data[BufferLength];
};


#endif /* LinkedQueueDatastructures_h */
