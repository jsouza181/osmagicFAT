# SHELL #

### Members ###
* Jason Souza
* Ricardo Castilla
* Philip Scott
* Conor Stephens

### Purpose ###
* Learn how to create a kernel module
* Learn how to compile a custom kernel
* Version: 1.0

### Tar file contents ###

-> elevator/

    -> README.txt
    -> README.md
    -> report.txt

    -> divers/
        -> consumer.c
        -> consumer.x
        -> elevator_calls.c
        -> elevator_calls.h
        -> Makefile
        -> producer.c
        -> producer.h

    -> include/
        -> elevator.h
        -> elevatorproc.h
        -> scheduler.h
        -> syscalls.h

    -> src/
        -> elevator.c
        -> elevatorproc.c
        -> module.c
        -> scheduler.c
        -> sys_issue_request.c
        -> sys_start_elevator.c
        -> sys_stop_elevator.c
        -> syscalls.c

    -> part1/
        -> main.c
        -> Makefile

    -> part2/
        -> Makefile
        -> my_xtime.c

    -> Makefile
    -> proc_view.sh

### Compiling ###
* Use provided makefile to compile the source code
```
$> make
```

* In order to clean the environment, run
```
$> make clean
```

### Using the elevator ###
* In order to start the elevator, run
```
$> make teststart
```
* To create a request
```
$> make testadd
```
* To create 10 requests run
```
$> make testadd10
```
* To crate hundreds of requests, it is possible to call the following in a script
```
$> make testadd10 &
```
* To automate the start, add, and stop of one request (also removes the module)
```
$> make test
```
* To stop and remove the elevator module
```
$> make teststop
```
* To start, or stop the elevator without inserting or removing the module, you may use
```
$> ./drivers/consumer.x --start
$> ./drivers/consumer.x --stop
```
respectively

### Known Bugs ###

- Fraction values do not update correctly, which causes the elevator to accept more weight, and units than allowed (rarely)
- When elevator is stopped, all passengers waiting on a floor are deallocated. This might not be a wanted 'feature'
