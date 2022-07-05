#include "pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

void JobQueue::Push(JobRef job, bool pushOnly)
{
	// 카운트 증가시키고 Job 추가
	const int32 prevCount = _jobCount.fetch_add(1);
	_jobs.Push(job); // WRITE_LOCK;

	// 첫번째 Job 넣은 스레드가 실행까지 담당
	if (prevCount == 0)
	{
		// 이미 실행중인 JobQueue가 없으면 실행
		if (LCurrentJobQueue == nullptr && pushOnly == false)
			Execute();
		else
		{
			// 여유있는 다른 쓰레드가 실행하도록 GlobalQueue 에 넘긴다
			GGlobalQueue->Push(shared_from_this());
		}
	}
}

// 한쪽만 몰빵하게되서 일감이 너무 몰리면?
// DoAsync 타고 타고 가서 절대 끊나지 않는 상황(일감이 한 쓰레드에 몰리게 됨)
// 일감 배분을 잘해야함
void JobQueue::Execute()
{
	LCurrentJobQueue = this;

	while (true)
	{
		Vector<JobRef> jobs;
		_jobs.PopAll(OUT jobs);

		const int32 jobCount = static_cast<int32>(jobs.size());
		for (int32 i = 0; i < jobCount; ++i)
			jobs[i]->Execute();

		// 남은 일감이 0개라면 종료
		// 잡 실행하고 카운트 감소(카운트 증가와 _jobs.Push(job)사이에 job이 추가될수있음)
		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}

		const uint64 now = ::GetTickCount64();
		if (now >= LEndTickCount)
		{
			LCurrentJobQueue = nullptr;
			// 여유있는 다른 쓰레드가 실행하도록 GlobalQueue 에 넘긴다
			GGlobalQueue->Push(shared_from_this());
		}
	}
}
