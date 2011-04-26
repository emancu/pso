all: num_into_string-test

num_into_string-test: num_into_string-test.c
	gcc $@.c ../kernel/lib_str.c -I ../include/ -o $@ -g

clean:
	rm num_into_string-test
