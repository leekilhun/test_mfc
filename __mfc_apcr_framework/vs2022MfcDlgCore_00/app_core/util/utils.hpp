/**
 * @file utils.hpp
 * @author lee kil hun (gns2.lee@)
 * @brief 
 * @version 0.1
 * @date 2025-04-07
 * @ed : 20250608 : timer do_reset 추가
 * 
 * @copyright Copyright (c) 2025
 * 
 */


#pragma once
#ifndef _CORE_COMMON_UTIL_UTILS_HPP_
#define _CORE_COMMON_UTIL_UTILS_HPP_

#include "../def.hpp"

namespace apcr
{
  struct machine_step_t
  {
    using buff_t = TinyC::tiny_fifo<uint8_t, 32>;

    uint8_t curr_step{};
    uint8_t pre_step{};
    uint8_t sub_step{};
    uint32_t prev_ms{};
    uint32_t elap_ms{};
    uint8_t retry_cnt{};
    buff_t steps{};
    bool is_timerOn{};

    machine_step_t()                                     = default;
    ~machine_step_t()                                    = default;
    machine_step_t(const machine_step_t &rhs)            = default; // copy constructor
    machine_step_t &operator=(const machine_step_t &rhs) = default; // copy assignment operator
    machine_step_t(machine_step_t &&rhs)                 = default; // move constructor
    machine_step_t &operator=(machine_step_t &&rhs)      = default; // move assignment operator

    void SetStep(uint8_t step, bool reset_tmr = false);
    uint8_t GetStep() const;
    uint32_t ElapTime() const;
    bool LessThan(uint32_t msec);
    bool MoreThan(uint32_t msec);
    uint8_t operator-(uint8_t dec_index);
  };
  // end of struct machine_step_t

#if 0
  template <typename Type, std::size_t Size = 128>
  struct prc_step_t
  {
    uint8_t          curr_step{};
    uint8_t          pre_step{};
    uint8_t          sub_step{};
    uint32_t         prev_ms{};
    uint32_t         elap_ms{};
    uint8_t          retry_cnt{};
    TinyC::Que<Type> msgQ{Size};
    bool             wait_resp{}; // true - wait step complete, false - completed step

    inline void SetStep(uint8_t step)
    {
      elap_ms   = tim::millis() - prev_ms;
      prev_ms   = tim::millis();
      pre_step  = curr_step;
      curr_step = step;
    }

    inline uint8_t GetStep() const
    {
      return curr_step;
    }

    inline uint32_t ElapTime() const
    {
      return elap_ms;
    }

    inline bool LessThan(uint32_t msec)
    {
      elap_ms = tim::millis() - prev_ms;
      if (elap_ms < msec)
        return true;
      else
        return false;
    }

    inline bool MoreThan(uint32_t msec)
    {
      return !LessThan(msec);
    }

    inline bool Available() const
    {
      return !wait_resp;
    }
  };
  // end of struct prc_step_t

  // Deduction guide for prc_step_t
  template <typename Type>
  prc_step_t(Type) -> prc_step_t<Type>;
#endif

//--------------------------------------------------
//MARK: stimr class
//--------------------------------------------------
  struct stimr
  {

    uint32_t prev_ms{};
    uint32_t elap_ms{};
    bool is_started{};

    stimr()                            = default;
    ~stimr()                           = default;
    stimr(const stimr &rhs)            = default; // copy constructor
    stimr &operator=(const stimr &rhs) = default; // copy assignment operator
    stimr(stimr &&rhs)                 = default; // move constructor
    stimr &operator=(stimr &&rhs)      = default; // move assignment operator

    bool LessThan(uint32_t msec);
    bool MoreThan(uint32_t msec);
    uint32_t ElapTime() const;
    // 타이머 초기화
    void Reset();
  };
  // end of simple timer



  //------------------------------------------------
  //MARK: Timer class
  //------------------------------------------------ 
  template <std::size_t Size = 512>
  class Timer
  {
  public:
    struct Element
    {
      bool     isOn   = false;
      uint32_t prevMs = 0;
      uint32_t elapMs = 0;
    };

  private:
    static constexpr std::size_t   MaxTimers = Size;
    std::array<Element, MaxTimers> timers{};

  public:
    // 모든 멤버 함수를 인라인으로 정의
    inline bool IsOn(int idx = 0) const
    {
      std::size_t tId = idx % MaxTimers;
      return timers[tId].isOn;
    }

    inline bool LessThan(uint32_t msec, int idx = 0)
    {
      std::size_t tId         = idx % MaxTimers;
      auto       &timer       = timers[tId];
      uint32_t    currentTime = tim::millis();

      if (timer.isOn)
      {
        timer.elapMs = currentTime - timer.prevMs;
        return timer.elapMs < msec;
      }
      else
      {
        timer.prevMs = currentTime;
        timer.isOn   = true;
        return true;
      }
    }

    inline bool MoreThan(uint32_t msec, int idx = 0)
    {
      return !LessThan(msec, idx);
    }

    inline uint32_t ElapTime(int idx, bool is_reset = false)
    {
      std::size_t tId         = idx % MaxTimers;
      auto       &timer       = timers[tId];
      uint32_t    currentTime = tim::millis();

      timer.elapMs = currentTime - timer.prevMs;
      if (is_reset)
        timer.prevMs = currentTime;

      return timer.elapMs;
    }

    inline void Reset(int idx = 0)
    {
      std::size_t tId   = idx % MaxTimers;
      auto       &timer = timers[tId];
      timer.isOn        = true;
      timer.prevMs      = tim::millis();
    }

    inline void Flush()
    {
      for (auto &timer : timers)
      {
        timer.isOn   = false;
        timer.prevMs = 0;
        timer.elapMs = 0;
      }
    }

    template <size_t... IDs>
    inline void do_reset()
    {
      (Reset(IDs), ...);
    }
  };
  // end of class Timer


  //-------------------------------------------
  //MARK: ThreadWorker
  //-------------------------------------------
  class ThreadWorker
  {
  public:
    ThreadWorker();
    ~ThreadWorker();

    void enqueueTask(std::function<void()> task);

  private:
    std::thread                       worker_thread;
    std::queue<std::function<void()>> task_queue;
    std::mutex                        mtx;
    std::condition_variable           cv;
    std::atomic<bool>                 stop_thread;

    void workerFunction();
  };

  

  namespace types
  {
    template <typename... Args>
    concept same_args_c = std::conjunction_v<std::is_same<Args, Args>...>;

  } // namespace types

}
// end of namespace apcr

#endif // _CORE_COMMON_UTIL_UTILS_HPP_
