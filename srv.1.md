SRV(1) - General Commands Manual

# NAME

**srv** - listens on random port and execute command

# SYNOPSIS

**srv**
*utility*
\[*...*]

# DESCRIPTION

The
**srv**
utility listen on a random TCP on
ip(4)
address family, prints the random port number on standard output,
then waits for an incoming connection, when it arrives it executes
*utility*
passing the socket of the incoming connection as standard input and output to
*utility*
but standard error is leaved untouched.

The options are as follows:

**-w** *timeout*

> **srv**
> exits with error status if after
> *timeout*
> seconds no connection is received.
> 0 equals no timeout.
> The default is 0.

# EXIT STATUS

The
**srv**
utility exits with one of the following values:

1

> Some error happened before invocation of
> *utility*
> or
> *utility*
> exited with 1 status.

&gt;1

> Some other error happened.

Otherwise, the exit status of
**srv**
shall be that of
*utility*.

# EXAMPLES

To start a simple echo server:

		% srv cat
		5343

Now to test it:

		% echo hello | nc localhost 5343
		hello
		%

To send and store a file:

		machine1% srv sh -c 'cat > file'
		5432
	
		machine2% nc -N machine1 5432 <file

# SEE ALSO

nc(1)

# AUTHORS

Abel Abraham Camarillo Ojeda &lt;[acamari@verlet.org](mailto:acamari@verlet.org)&gt;

# CAVEATS

*utility*
argument is only
execv(3)'ed
*after*
the connection is
accept(2)'ed,
so it's not currently possible to know if its valid until a connection arrives.
A future version of this utility might fix that.

OpenBSD 6.3 - December 11, 2017
