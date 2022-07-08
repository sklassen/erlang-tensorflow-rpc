An OTP application with RPC to c.
======
Basic RPC between erlang and a c program

Install
------
You will need gcc

Build
------
$ cmake ./c_src/CMakeLists.txt
$ rebar3 shell

Execute
------

Model is initialized at 1.

```
Erlang/OTP 22 [erts-10.6.4] [source] [64-bit] [smp:12:12] [ds:12:12:10] [async-threads:1]

Eshell V10.6.4  (abort with ^G)
1> model_rpc:start().
true
2> model_rpc:shift(5).
6
3> model_rpc:scale(2).
12

```

Development
------
A suggested .gitignore
```
.*
_*
priv/
*.lock
```
