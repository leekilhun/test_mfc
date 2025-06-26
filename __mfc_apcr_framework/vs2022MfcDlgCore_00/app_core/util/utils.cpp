/**
 * @file utils.cpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2025-06-25
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "utils.hpp"
namespace apcr
{

  void machine_step_t::SetStep(uint8_t step, bool reset_tmr)
  {
    elap_ms   = tim::millis() - prev_ms;
    prev_ms   = tim::millis();
    pre_step  = curr_step;
    curr_step = step;
    steps.in(step);
    is_timerOn = !(reset_tmr);
  }

  uint8_t machine_step_t::GetStep() const
  {
    return curr_step;
  }

  uint32_t machine_step_t::ElapTime() const
  {
    return elap_ms;
  }

  bool machine_step_t::LessThan(uint32_t msec)
  {
    uint32_t curr_time = tim::millis();
    if (is_timerOn)
    {
      elap_ms = curr_time - prev_ms;
      return (elap_ms < msec);
    }
    else
    {
      prev_ms    = curr_time;
      is_timerOn = true;
      return true;
    }
  }

  bool machine_step_t::MoreThan(uint32_t msec)
  {
    return !LessThan(msec);
  }

  uint8_t machine_step_t::operator-(uint8_t dec_index)
  {
    if (dec_index > steps.available())
    {
      LOG_PRINT("dec_index is out of range");
      return 0;
    }
    else
    {
      return steps.pop(dec_index);
    }
  }
//--------------------------------------------------
//MARK: stimr class
//--------------------------------------------------
  bool stimr::LessThan(uint32_t msec)
  {
    if (is_started)
    {
      elap_ms = tim::millis() - prev_ms;
      return (elap_ms < msec);
    }
    else
    {
      prev_ms    = tim::millis();
      is_started = true;
      return true;
    }

    elap_ms = tim::millis() - prev_ms;
    if (elap_ms < msec)
      return true;
    else
      return false;
  }

  bool stimr::MoreThan(uint32_t msec)
  {
    return !LessThan(msec);
  }

  uint32_t stimr::ElapTime() const
  {
    return is_started ? elap_ms : 0;
  }

  // 타이머 초기화
  void stimr::Reset()
  {
    is_started = false;
    elap_ms    = 0;
    prev_ms    = 0;
  }

  //-------------------------------------------
  //MARK: ThreadWorker
  //-------------------------------------------
  ThreadWorker::ThreadWorker() :
  stop_thread(false)
  {
    worker_thread = std::thread(&ThreadWorker::workerFunction, this);
  }

  ThreadWorker::~ThreadWorker()
  {
    {
      std::unique_lock<std::mutex> lock(mtx);
      stop_thread = true;
    }
    cv.notify_one();
    worker_thread.join();
  }

  void ThreadWorker::enqueueTask(std::function<void()> task)
  {
    std::unique_lock<std::mutex> lock(mtx);
    task_queue.push(task);
    cv.notify_one();
  }

  void ThreadWorker::workerFunction()
  {
    while (true)
    {
      std::function<void()> task;
      {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]
                { return !task_queue.empty() || stop_thread; });

        if (stop_thread && task_queue.empty())
          return;

        task = task_queue.front();
        task_queue.pop();
      }

      task();
    }
  }
}
// end of namespace apcr
