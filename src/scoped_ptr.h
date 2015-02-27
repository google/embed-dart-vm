#ifndef SCOPED_PTR_H_
#define SCOPED_PTR_H_

template<typename T>
class scoped_ptr {
 public:
  explicit scoped_ptr(T* ptr) : ptr_(ptr) {}
  ~scoped_ptr() { delete ptr_; ptr_ = NULL; }

  T* get() const { return ptr_; }
  T* operator->() const { return get(); }
  bool operator!() const { return get() == NULL; }
 private:
  T* ptr_;
};

#endif // SCOPED_PTR_H_
