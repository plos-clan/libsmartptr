#pragma once

/*移植接口定义*/
template<class T, typename... Types> T *smart_ptr_new(const Types &...args);
template<class T> void smart_ptr_delete(const T *ptr);

/*smart_ptr的实现*/
template<typename T> class smart_ptr {
private:
	typedef void (*destroy_fp_type)(smart_ptr<T> *p);
	// 销毁函数的函数指针类型
	destroy_fp_type destroy_fp;
	// 销毁函数指针
	T *ptr;
	// 数据
	unsigned int *ref_cnt;
	// 引用计数

public:
	smart_ptr(T *pointer);
	// 只传递指针，销毁方式默认为smart_ptr_delete
	smart_ptr(T *pointer, destroy_fp_type fp);
	// 传递指针和销毁函数
	smart_ptr(const smart_ptr<T> &value);
	// 拷贝构造
	smart_ptr<T> &operator=(const smart_ptr<T> &value);
	// 重载赋值
	T *get();
	// 获取指针
	T *operator->();
	// 直接访问成员
	~smart_ptr();
	// 析构函数
};

/*移植接口，这里用标准C++实现，开发者可以自行修改*/
template<class T, typename... Types> T *smart_ptr_new(const Types &...args) {
	// 新建一个类型为T的对象，args为构造函数的初始化参数，返回新建的对象指针
	return new T(args...);
}
template<class T> void smart_ptr_delete(const T *ptr) {
	// 删除一个类型为T的对象，ptr为待删除的类型指针
	delete ptr;
}

/*主体实现部分*/
template<typename T> void __smart_ptr_default_destroy_function__(smart_ptr<T> *p) {
	// 默认的销毁函数，直接delete
	smart_ptr_delete(p.get());
}

template<typename T> smart_ptr<T>::smart_ptr(T *pointer) {
	ref_cnt = smart_ptr_new<unsigned int>();
	(*ref_cnt) = 1;
	ptr = pointer;
	destroy_fp = __smart_ptr_default_destroy_function__<T>;
	// 设置为默认的销毁函数
}

template<typename T> smart_ptr<T>::smart_ptr(T *pointer, destroy_fp_type destroy_funcptr) {
	ref_cnt = smart_ptr_new<unsigned int>();
	(*ref_cnt) = 1;
	ptr = pointer;
	destroy_fp = destroy_funcptr;
}

template<typename T> smart_ptr<T>::smart_ptr(const smart_ptr<T> &value) {
	ref_cnt = value.ref_cnt;
	ptr = value.ptr;
	destroy_fp = value.destroy_fp;
	(*ref_cnt)++; // 引用计数器自增
}

template<typename T> smart_ptr<T> &smart_ptr<T>::operator=(const smart_ptr<T> &value) {
	/*
	 * 将当前smart_ptr重新指定一个新的指针
	 * 就意味着需要抛弃当前的指针，指向新的指针
	 * 因此需要先减去当前指针的计数器，再将value的计数器加一
	 */
	if (&value == this) {
		// 自我赋值，直接返回即可
		return *this;
	}

	(*ref_cnt)--; // 减去当前计数器

	if ((*ref_cnt) == 0) {
		smart_ptr_delete(ref_cnt);
		destroy_fp(this);
		// 如果已经没有任何smart_ptr指向该指针
		// 那么此ptr可以被视作垃圾指针，需要被销毁
	}

	ref_cnt = value.ref_cnt; // 复制value
	ptr = value.ptr;
	destroy_fp = value.destroy_fp;

	(*ref_cnt)++; // value的计数器加一

	return *this;
}

template<typename T> T *smart_ptr<T>::get() {
	return ptr;
}

template<typename T> T *smart_ptr<T>::operator->() {
	return ptr;
}

template<typename T> smart_ptr<T>::~smart_ptr() {
	// 当前计数器需要减一
	(*ref_cnt)--;
	if ((*ref_cnt) == 0) {
		smart_ptr_delete(ref_cnt);
		destroy_fp(this);
		// 原理同上
	}
}