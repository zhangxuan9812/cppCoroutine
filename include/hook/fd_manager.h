#ifndef _FD_MANAGER_H_
#define _FD_MANAGER_H_

#include <memory>
#include <shared_mutex>
#include "thread/thread.h"

namespace cppCoroutine {

/*
 * @brief FdCtx class, which is used to store the context of the file descriptor
 * @property m_isInit whether the FdCtx is initialized
 * @property m_isSocket whether the file descriptor is a socket
 * @property m_sysNonblock whether the file descriptor is set to nonblock
 * @property m_userNonblock whether the file descriptor is set to nonblock by the user
 * @property m_isClosed whether the file descriptor is closed
 * @property m_fd the file descriptor
 * @property m_recvTimeout the read event timeout
 * @property m_sendTimeout the write event timeout
 */
class FdCtx : public std::enable_shared_from_this<FdCtx> {
 private:
  bool m_isInit = false;
  bool m_isSocket = false;
  bool m_sysNonblock = false;
  bool m_userNonblock = false;
  bool m_isClosed = false;
  int m_fd;

  // read event timeout
  uint64_t m_recvTimeout = (uint64_t)-1;
  // write event timeout
  uint64_t m_sendTimeout = (uint64_t)-1;

 public:
  FdCtx(int fd);
  ~FdCtx();
  /*
   * @brief Initialize the FdCtx
   * @return whether the initialization is successful
   */
  bool init();
  bool isInit() const { return m_isInit; }
  bool isSocket() const { return m_isSocket; }
  bool isClosed() const { return m_isClosed; }

  void setUserNonblock(bool v) { m_userNonblock = v; }
  bool getUserNonblock() const { return m_userNonblock; }

  void setSysNonblock(bool v) { m_sysNonblock = v; }
  bool getSysNonblock() const { return m_sysNonblock; }
  /*
   * @brief Set the event context
   * @param event the event type
   * @param v the timeout value
   */
  void setTimeout(int type, uint64_t v);
  /*
   * @brief Get the timeout value
   * @param type the event type
   * @return the timeout value
   */
  uint64_t getTimeout(int type);
};

/*
 * @brief FdManager class, which is used to manage the file descriptors
 * @property m_mutex the mutex
 * @property m_datas the vector to store the FdCtx
 */
class FdManager {
 public:
  FdManager();
  /*
   * @brief Get the FdCtx by the file descriptor
   * @param fd the file descriptor
   * @param auto_create whether to create the FdCtx automatically
   * @return the FdCtx
   */
  std::shared_ptr<FdCtx> get(int fd, bool auto_create = false);
  /*
   * @brief Delete the FdCtx by the file descriptor
   * @param fd the file descriptor
   */
  void del(int fd);

 private:
  std::shared_mutex m_mutex;
  // Store the FdCtx
  std::vector<std::shared_ptr<FdCtx>> m_datas;
};

/*
 * @brief Singleton class, which is used to manage the FdManager
 * @property instance the instance of the FdManager
 * @property mutex the mutex
 */
template <typename T>
class Singleton {
 private:
  static T *instance;
  static std::mutex mutex;

 protected:
  Singleton() {}

 public:
  // Delete copy constructor and assignment operation
  Singleton(const Singleton &) = delete;
  Singleton &operator=(const Singleton &) = delete;

  static T *GetInstance() {
    std::lock_guard<std::mutex> lock(mutex);  // Ensure thread safety
    if (instance == nullptr) {
      instance = new T();
    }
    return instance;
  }

  static void DestroyInstance() {
    std::lock_guard<std::mutex> lock(mutex);
    delete instance;
    instance = nullptr;
  }
};

typedef Singleton<FdManager> FdMgr;

}  // namespace cppCoroutine

#endif