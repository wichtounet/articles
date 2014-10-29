default: release

.PHONY: default release debug all clean

include make-utils/flags.mk
include make-utils/cpp-utils.mk

SQRT_VALUE=5000000

CXX_FLAGS += -DSQRT_VALUE=$(SQRT_VALUE) -ICatch/include

$(eval $(call src_folder_compile,))
$(eval $(call src_folder_compile,/boost_po))
$(eval $(call src_folder_compile,/intrusive_list))
$(eval $(call src_folder_compile,/linear_sorting))
$(eval $(call src_folder_compile,/threads/benchmark))
$(eval $(call src_folder_compile,/threads/part1))
$(eval $(call src_folder_compile,/threads/part2))
$(eval $(call src_folder_compile,/threads/part3))
$(eval $(call src_folder_compile,/threads/part4))
$(eval $(call src_folder_compile,/vector_list))
$(eval $(call src_folder_compile,/sqrt))
$(eval $(call src_folder_compile,/catch))

$(eval $(call add_src_executable,sqrt_constexpr,sqrt/constexpr.cpp))
$(eval $(call add_src_executable,sqrt_smart_constexpr,sqrt/smart_constexpr.cpp))
$(eval $(call add_src_executable,sqrt_tmp,sqrt/tmp.cpp))
$(eval $(call add_src_executable,sqrt_smart_tmp,sqrt/smart_tmp.cpp))

$(eval $(call add_src_executable,catch_test_1,catch/test1.cpp))

$(eval $(call add_src_executable,threads_p1_hello0,threads/part1/Hello0.cpp))
$(eval $(call add_src_executable,threads_p1_hello1,threads/part1/Hello1.cpp))
$(eval $(call add_src_executable,threads_p1_hello2,threads/part1/Hello2.cpp))

$(eval $(call add_src_executable,threads_p2_counter1,threads/part2/Unsafe.cpp))
$(eval $(call add_src_executable,threads_p2_counter2,threads/part2/Safe.cpp))
$(eval $(call add_src_executable,threads_p2_counter3,threads/part2/Exception.cpp))
$(eval $(call add_src_executable,threads_p2_counter4,threads/part2/SafeGuard.cpp))

$(eval $(call add_src_executable,threads_p3_recursive,threads/part3/recursive_problem.cpp))
$(eval $(call add_src_executable,threads_p3_recursive2,threads/part3/recursive_mutex.cpp))
$(eval $(call add_src_executable,threads_p3_timed,threads/part3/timed_mutex.cpp))
$(eval $(call add_src_executable,threads_p3_call_once,threads/part3/call_once.cpp))
$(eval $(call add_src_executable,threads_p3_condition_variables,threads/part3/condition_variables.cpp))

$(eval $(call add_src_executable,threads_p4_atomic_counter,threads/part4/AtomicCounter.cpp))

$(eval $(call add_src_executable,threads_bench,threads/benchmark/bench.cpp))

$(eval $(call add_src_executable,linear_sorting,linear_sorting/bench.cpp))

$(eval $(call add_src_executable,boost_po_v1,boost_po/v1.cpp,-lboost_program_options))

$(eval $(call add_src_executable,vector_list,vector_list/bench.cpp graphs.cpp demangle.cpp))

$(eval $(call add_src_executable,intrusive_list,intrusive_list/bench.cpp graphs.cpp demangle.cpp))

$(eval $(call add_executable_set,threads_p1,threads_p1_hello0 threads_p1_hello1 threads_p1_hello2))
$(eval $(call add_executable_set,threads_p2,threads_p2_counter1 threads_p2_counter2 threads_p2_counter3 threads_p2_counter4))
$(eval $(call add_executable_set,threads_p3,threads_p3_recursive threads_p3_recursive2 threads_p3_timed threads_p3_call_once threads_p3_condition_variables))
$(eval $(call add_executable_set,threads_p4,threads_p4_atomic_counter))
$(eval $(call add_executable_set,threads_bench,threads_bench))
$(eval $(call add_executable_set,linear_sorting,linear_sorting))
$(eval $(call add_executable_set,boost_po_v1,boost_po_v1))
$(eval $(call add_executable_set,intrusive_list,intrusive_list))
$(eval $(call add_executable_set,vector_list,vector_list))

release: release_threads_p1 release_threads_p2 release_threads_p3 release_threads_p4 release_threads_bench release_linear_sorting release_boost_po_v1 release_vector_list release_intrusive_list
debug: debug_threads_p1 debug_threads_p2 debug_threads_p3 debug_threads_p4 debug_threads_bench debug_linear_sorting debug_boost_po_v1 debug_vector_list debug_intrusive_list

all: release debug

sonar: release
	cppcheck --xml-version=2 --enable=all --std=c++11 src include 2> cppcheck_report.xml
	/opt/sonar-runner/bin/sonar-runner

clean:
	rm -rf release/
	rm -rf debug/

-include $(DEBUG_D_FILES)
-include $(RELEASE_D_FILES)
