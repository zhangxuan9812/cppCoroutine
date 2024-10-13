#include "hook/fd_manager.h"
#include "hook/hook.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace cppCoroutine {

// instantiate
template class Singleton<FdManager>;

// Static variables need to be defined outside the class
// Set the instance to nullptr
template <typename T>
T *Singleton<T>::instance = nullptr;

template <typename T>
std::mutex Singleton<T>::mutex;

FdCtx::FdCtx(int fd) : m_fd(fd) { init(); }

FdCtx::~FdCtx() {}

bool FdCtx::init() {
  // if it is already initialized, return true
  if (m_isInit) {
    return true;
  }
  // statbuf store the file status
  struct stat statbuf;
  // fd is in valid
  // fstat() used to get the file status, m_fd is the file descriptor, statbuf is the place to store the file status
  // return 0 if success, -1 if failed
  if (-1 == fstat(m_fd, &statbuf)) {
    m_isInit = false;
    m_isSocket = false;
  } else {
    m_isInit = true;
    // S_ISSOCK() is a macro that returns true if the file is a socket
    m_isSocket = S_ISSOCK(statbuf.st_mode);
  }

  // If it is a socket, set the nonblock
  if (m_isSocket) {
    // fcntl_f() is used to get or set the file descriptor flags
    // The return value of fcntl_f() is the file descriptor flags, which is an integer
    // If the flags is not O_NONBLOCK, set the flags to O_NONBLOCK
    int flags = fcntl_f(m_fd, F_GETFL, 0);
    if (!(flags & O_NONBLOCK)) {
      // if not -> set to nonblock
      fcntl_f(m_fd, F_SETFL, flags | O_NONBLOCK);
    }
    m_sysNonblock = true;
  } else {
    m_sysNonblock = false;
  }
  return m_isInit;
}

void FdCtx::setTimeout(int type, uint64_t v) {
  // SO_RCVTIMEO means the read event timeout
  if (type == SO_RCVTIMEO) {
    m_recvTimeout = v;
  } else {
    m_sendTimeout = v;
  }
}

uint64_t FdCtx::getTimeout(int type) {
  if (type == SO_RCVTIMEO) {
    return m_recvTimeout;
  } else {
    return m_sendTimeout;
  }
}

FdManager::FdManager() { m_datas.resize(64); }

std::shared_ptr<FdCtx> FdManager::get(int fd, bool auto_create) {
  if (fd == -1) {
    return nullptr;
  }

  std::shared_lock<std::shared_mutex> read_lock(m_mutex);
  if (m_datas.size() <= static_cast<size_t>(fd)) {
    if (auto_create == false) {
      return nullptr;
    }
  } else {
    if (m_datas[fd] || !auto_create) {
      return m_datas[fd];
    }
  }

  read_lock.unlock();
  std::unique_lock<std::shared_mutex> write_lock(m_mutex);
  if (m_datas.size() <= static_cast<size_t>(fd)) {
    m_datas.resize(fd * 1.5);
  }
  m_datas[fd] = std::make_shared<FdCtx>(fd);
  return m_datas[fd];
}

void FdManager::del(int fd) {
  std::unique_lock<std::shared_mutex> write_lock(m_mutex);
  if (m_datas.size() <= static_cast<size_t>(fd)) {
    return;
  }
  m_datas[fd].reset();
}

}  // namespace cppCoroutine