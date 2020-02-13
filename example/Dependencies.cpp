// Copyright (c) 2013 Doug Binks
// 
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
// 
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgement in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "TaskScheduler.h"
#include "Timer.h"

#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <vector>

#ifndef _WIN32
    #include <string.h>
#endif

using namespace enki;

TaskScheduler g_TS;

struct TaskA : ITaskSet
{
    void ExecuteRange( TaskSetPartition range, uint32_t threadnum ) override
    {
        printf("A on thread %u\n", threadnum);
    }
};

struct TaskB : ITaskSet
{
    Dependency          m_Dependency;

    void ExecuteRange( TaskSetPartition range, uint32_t threadnum ) override
    {
        printf("B on thread %u\n", threadnum);
    }
};

struct TaskC : ITaskSet
{
    Dependency          m_Dependencies[4];

    void ExecuteRange( TaskSetPartition range, uint32_t threadnum ) override
    {
        printf("C on thread %u\n", threadnum);
    }
};

struct TaskD : ITaskSet
{
    Dependency m_Dependency;

    void ExecuteRange( TaskSetPartition range, uint32_t threadnum ) override
    {
        printf("D on thread %u\n", threadnum);
    }
};

struct TaskE : ITaskSet
{
    Dependency          m_Dependencies[10];

    void ExecuteRange( TaskSetPartition range, uint32_t threadnum ) override
    {
        printf("E on thread %u\n", threadnum);
    }
};

static const int RUNS       = 20;

int main(int argc, const char * argv[])
{
    g_TS.Initialize();

    // construct the graph once
    TaskA taskA;

    TaskB taskBs[4];
    for( auto& task : taskBs )
    {
        task.m_Dependency.SetDependency(&taskA,&task);
    }

    TaskC taskC;
    int i = 0;
    for( auto& task : taskBs )
    {
        taskC.m_Dependencies[i++].SetDependency(&task,&taskC);
    }

    TaskD taskDs[10];
    for( auto& task : taskDs )
    {
        task.m_Dependency.SetDependency(&taskC,&task);
    }

    TaskE taskE;
    i = 0;
    for( auto& task : taskDs )
    {
        taskE.m_Dependencies[i++].SetDependency( &task, &taskE );
    }

    // run graph many times
    for( int run = 0; run< RUNS; ++run )
    {
        printf("Run %d / %d.....\n", run+1, RUNS);

        g_TS.AddTaskSetToPipe( &taskA );

        g_TS.WaitforTask( &taskE );
    }

    return 0;
}
