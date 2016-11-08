import threading
import logging

_thread_list = []
_thread_num_default = 4


class bluThread(threading.Thread):
    def __init__(self, thread_id, target, **kwargs):
        threading.Thread.__init__(self)
        self._thread_id = thread_id
        self._target = target
        self._kwargs = kwargs

    def run(self):
        self._target(self._thread_id, **self._kwargs)


#  Creates a number of threads and has them all run a function, passed by the user
def start_threaded_job(target, thread_num=_thread_num_default, **kwargs):
    for i in range(thread_num):
        thread = bluThread(i, target, **kwargs)
        thread.start()
        _thread_list.append(thread)
        i += 1


#  Called to wait for all threads in _thread_list[] to finish
def wait_for_threads(logger=None):
    for t in _thread_list:
        t.join()
    if logger is None:
        print("All threads rejoined gracefully")
    else:
        logger.debug("All threads rejoined gracefully")


if __name__ == "__main__":
    def thread_test_job(thread_id, arg1, arg2):
        print("arg + thread_id = " + str(arg1 + thread_id) + \
        " \targ2 + thread_id = " + str(arg2 + thread_id))

    kwargs={"arg1":1, "arg2":2}
    start_threaded_job(thread_test_job, **kwargs)
    wait_for_threads()
