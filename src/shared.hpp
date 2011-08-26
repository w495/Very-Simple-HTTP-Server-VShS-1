#include<cerrno>
#include<cstdlib>
#include<cstdio>

extern "C" {
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
}


template<typename T>
class Shared{
  public:

/*! \brief Allocates shared memory for some object of len elements.

	May be we should use allocators for standart stl-containers.
	But this way is more simple. More than that, in our task we need not
	to realock our memory. If we want to use shared allocators
	we should use Boost.Interprocess or Qt::Threads.

	\param len=1 number of elements.
*/

	Shared(size_t len = 1){
		if(0 == len){
			_isValid = false;
			_t = NULL;
		}
		else{
			_isValid = true;
			size_t _size = sizeof(*_t)*len;
			
			const char* shm_name = "shm-name";

			/* Create a new memory object */
			int fd = shm_open(shm_name, O_RDWR | O_CREAT, 0777);
			if (fd == -1) {
				fprintf(stderr, "Open failed : %s\n", strerror(errno));
				_isValid = false;
				_t = NULL;
			}

			/* Set the memory object's size */
			if (ftruncate(fd, _size ) == -1) {
				fprintf(stderr, "ftruncate : %s\n", strerror(errno));
				_isValid = false;
			}

			/* Map the memory object */
			_t = (T*)mmap(0, _size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
			if (_t == MAP_FAILED) {
				fprintf(stderr, "mmap failed:%s\n", strerror(errno));
				_isValid = false;
				_t = NULL;
				exit(1);
			}

			#ifdef DEBUG
				_debug("Map addr is %6.6X\n", _t);
			#endif /*DEBUG*/

			/* We unlink so object goes away on last close. */
			close(fd);
			shm_unlink(shm_name);

			pthread_mutex_init(&_mutex, NULL);
		}
	}

	~Shared(){
		munmap(_t, _size);
		pthread_mutex_destroy(&_mutex);
	}
	
	bool isValid(){
		return _isValid;
	}

	T* getPointer(){
		return _t;
	}

	T& V(){
		return *_t;
	}
	
	T& getValue() const {
		return *_t;
	}
	
	void setValue(const T& new_t){
		if(0 == pthread_mutex_lock(&_mutex)){
			*_t = new_t;
			if(pthread_mutex_unlock(&_mutex)){
				perror("pthread_mutex_unlock failed\n");
			}
		}
		else{
			perror("pthread_mutex_lock failed\n");
		}
	}
	
	Shared<T>& operator = (const T &new_t){
		setValue(new_t);
		return *this;
	}
	
	Shared<T>& operator += (const T &new_t){
		setValue(getValue() + new_t);
		return *this;
	}

	Shared<T>& operator -= (const T &new_t){
		setValue(getValue() - new_t);
		return *this;
	}
	
  private:
	T *_t;
	
	size_t _size;
	bool _isValid;
	pthread_mutex_t _mutex;
		
};

